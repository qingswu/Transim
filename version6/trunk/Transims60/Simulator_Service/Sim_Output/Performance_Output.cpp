//*********************************************************
//	Performance_Output.cpp - Output Interface Class
//*********************************************************

#include "Performance_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Performance_Output constructor
//---------------------------------------------------------

Performance_Output::Performance_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (PERFORMANCE_OUTPUT_OFFSET);
	data_flag = flow_flag = turn_flag = first_step = false;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_PERFORMANCE_FILE, num));

	file = new Performance_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----
			
	key = sim->Get_Control_String (Sim_Output_Step::NEW_PERFORMANCE_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_PERFORMANCE_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	}

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_PERFORMANCE_TIME_FORMAT, num);

	//---- time increment ----

	time_range.Increment (sim->Get_Control_Time (Sim_Output_Step::NEW_PERFORMANCE_INCREMENT, num));

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_PERFORMANCE_TIME_RANGE, num));

	//---- get the link range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_PERFORMANCE_LINK_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		link_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_PERFORMANCE_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}

	//---- get the coordinates ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_PERFORMANCE_COORDINATES, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		String token;

		key.Split (token);
		x1 = Round (token.Double ());

		if (!key.Split (token)) goto coord_error;
		y1 = Round (token.Double ());

		if (!key.Split (token)) goto coord_error;
		x2 = Round (token.Double ());

		if (!key.Split (token)) goto coord_error;
		y2 = Round (token.Double ());

		if (x2 < x1) {
			int x = x2;
			x2 = x1;
			x1 = x;
		}
		if (y2 < y1) {
			int y = y2;
			y2 = y1;
			y1 = y;
		}
		coord_flag = true;
	} else {
		coord_flag = false;
	}

	//---- get the veh types ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_PERFORMANCE_VEH_TYPES, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		veh_types.Add_Ranges (key);
	}

	//---- print the flow type ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_PERFORMANCE_LANE_USE_FLAG, num);
	return;

coord_error:
	sim->Error (String ("Output Link Delay Coordinate Data #%d") % num);
}

//---------------------------------------------------------
//	Performance_Output destructor
//---------------------------------------------------------

Performance_Output::~Performance_Output ()
{
	if (file != 0) {
		file->Close ();
	}
}

//---------------------------------------------------------
//	Write_Check
//---------------------------------------------------------

void Performance_Output::Write_Check (void)
{
	if (sim->time_step <= sim->Model_Start_Time ()) return;

	if (!time_range.In_Range (sim->time_step)) return;

	if (!data_flag) {

		//---- initialize the link data ----

		Perf_Data data_rec;

		data_rec.Count (1);

		perf_period.assign (sim->dir_array.size (), data_rec);

		data_flag = first_step = true;

	} else {
		Perf_Itr data_itr;

		//---- update the density data ----

		for (data_itr = perf_period.begin (); data_itr != perf_period.end (); data_itr++) {

			if (data_itr->Occupancy () > 0) {
				if (data_itr->Max_Volume () < data_itr->Occupancy ()) {
					data_itr->Max_Volume (data_itr->Occupancy ());
				}
				data_itr->Occupancy (0);

				if (data_itr->Max_Queue () < data_itr->Stop_Count ()) {
					data_itr->Max_Queue (data_itr->Stop_Count ());
				}
				data_itr->Add_Queue (data_itr->Stop_Count ());
				data_itr->Stop_Count (0);
			}
			data_itr->Add_Count ();
		}

		//---- check the output time increment ----

		if (time_range.At_Increment (sim->time_step)) {

			if (sim->Master ()) {
				Write_Summary ();
			}
			first_step = true;
		} else {
			first_step = false;
		}
	}
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Performance_Output::Output_Check (Travel_Step &step)
{
	if (step.Traveler () < 2 || !time_range.In_Range (sim->time_step) || step.size () < 1) return;

	int dir_index, cell, cells, offset, lane;
	double pce, persons, vmt, vht;
	bool skip, stop_flag;

	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Veh_Itr sim_veh_itr;
	Sim_Leg_Itr leg_itr;
	Perf_Data *perf_ptr;

	//---- get the vehicle type data ----

	if (step.sim_plan_ptr == 0) {
		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
	}
#ifdef CHECK
	if (step.sim_plan_ptr == 0) sim->Error ("Performance_Output::Output_Check: sim_plan_ptr");
#endif

	if (step.veh_type_ptr == 0) {
		step.veh_type_ptr = &sim->veh_type_array [step.sim_plan_ptr->Veh_Type ()];
	}
	if (step.veh_type_ptr != 0) {
		if (!veh_types.empty () && !veh_types.In_Range (step.veh_type_ptr->Type ())) return;
		pce = UnRound (step.veh_type_ptr->PCE ());
		persons = step.veh_type_ptr->Occupancy () / 100.0;
		if (persons < 1.0) persons = 1.0;
	} else {
		pce = 1.0;
		persons = 1.0 + step.sim_travel_ptr->Passengers ();
	}

	if (step.sim_dir_ptr == 0) {
		step.sim_dir_ptr = &sim->sim_dir_array [step [0].link];
	}
	sim_dir_ptr = step.sim_dir_ptr;

	vht = sim->method_time_step [sim_dir_ptr->Method ()];

	vht *= pce;
	cells = (int) step.size ();
	if (cells > 1) vht = vht / (cells - 1);

	//---- process each movement cell ----
	
	dir_index = -1;
	offset = 0;
	skip = false;
	perf_ptr = 0;
	stop_flag = (step.size () == 1);

	for (cell=0, sim_veh_itr = step.begin (); sim_veh_itr != step.end (); sim_veh_itr++, cell++) {

		if (sim_veh_itr->link != dir_index) {
			if (dir_index >= 0 && !skip) {
				perf_ptr->Add_Exit (pce);
			}
			if (cell == 0) {
				offset = sim_veh_itr->offset;
			} else {
				offset = 0;
			}
			lane = sim_veh_itr->lane;

			dir_index = sim_veh_itr->link;
			sim_dir_ptr = &sim->sim_dir_array [dir_index];
			skip = false;

			if (!link_range.empty () || coord_flag) {
				Dir_Data *dir_ptr = &sim->dir_array [dir_index];
				Link_Data *link_ptr = &sim->link_array [dir_ptr->Link ()];

				if (!link_range.empty ()) {
					if (!link_range.In_Range (link_ptr->Link ())) skip = true;
				}
				if (!skip && coord_flag) {
					Node_Data *node_ptr;

					node_ptr = &sim->node_array [link_ptr->Anode ()];
					if (node_ptr->X () < x1 || node_ptr->X () > x2 ||
						node_ptr->Y () < y1 || node_ptr->Y () > y2) {

						node_ptr = &sim->node_array [link_ptr->Bnode ()];

						if (node_ptr->X () < x1 || node_ptr->X () > x2 ||
							node_ptr->Y () < y1 || node_ptr->Y () > y2) {
							skip = true;
						}
					}
				}
			}
			if (!skip && !subarea_range.empty ()) {
				if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) skip = true;
			}
			if (!skip) {
#ifdef CHECK
				if (sim_dir_ptr->Lock () != step.Process_ID ())  sim->Error (String ("Performance Output::Output_Check: Link Lock=%d vs %d time_step=%d, traveler=%d") % sim_dir_ptr->Lock () % step.Process_ID () % sim->time_step % step.Traveler ());
#endif
				perf_ptr = &perf_period [dir_index];

				if (cell > 0 || (cell == 0 && lane == -1)) {
					perf_ptr->Add_Enter (pce);
				}
				if (cell > 0 || first_step) {
					perf_ptr->Add_Persons (persons);
					perf_ptr->Add_Volume (pce);
					perf_ptr->Add_Occupancy (pce);
				}
			}
		}
		if (!skip && perf_ptr > 0 && (cell > 0 || stop_flag)) {
			if (stop_flag) {
				perf_ptr->Add_Veh_Time (vht);
				perf_ptr->Add_Stop_Count (pce);
			} else {
				vmt = (sim_veh_itr->offset - offset) * pce;

				perf_ptr->Add_Veh_Dist (vmt);
				perf_ptr->Add_Veh_Time (vht);

				if (step.Speed () == 0) {
					perf_ptr->Add_Stop_Count (pce);
				}
				offset = sim_veh_itr->offset;

				if (sim_veh_itr->Parked ()) {
					perf_ptr->Add_Exit (pce);
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Cycle_Failure
//---------------------------------------------------------

void Performance_Output::Cycle_Failure (Dtime step, int dir_index, int vehicles, int veh_type)
{
	if (dir_index < 0 || vehicles < 0) return;

	Perf_Data *perf_ptr;

	//---- get the link data ----

	bool link_flag;
	Dir_Data *dir_ptr = 0;
	Link_Data *link_ptr = 0;

	//---- sum the data ----

	if (!veh_types.empty () && veh_type > 0) {
		if (!veh_types.In_Range (veh_type)) return;
	}
	if (!time_range.In_Range (step)) return;

	link_flag = !link_range.empty ();

	if ((link_flag || coord_flag) && link_ptr == 0) {
		dir_ptr = &sim->dir_array [dir_index];
		link_ptr = &sim->link_array [dir_ptr->Link ()];
	}
	if (link_flag && !link_range.In_Range (link_ptr->Link ())) return;

	if (coord_flag && dir_ptr && link_ptr) {
		Node_Data *node_ptr;

		if (dir_ptr->Dir () == 0) {
			node_ptr = &sim->node_array [link_ptr->Bnode ()];
		} else {
			node_ptr = &sim->node_array [link_ptr->Anode ()];
		}
		if (node_ptr->X () < x1 || node_ptr->X () > x2 ||
			node_ptr->Y () < y1 || node_ptr->Y () > y2) return;
	}
	if (!subarea_range.empty ()) {
		Sim_Dir_Ptr sim_dir_ptr = &sim->sim_dir_array [dir_index];

		if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) return;
	}

	//---- save the failure ----

	perf_ptr = &perf_period [dir_index];

	perf_ptr->Add_Failure (vehicles);
}

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void Performance_Output::Write_Summary (void)
{
	int num;
	int increment;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Perf_Itr perf_itr;
	Performance_Data data;

	increment = time_range.Increment ();
	if (increment < 1) increment = 1;

	//---- output the current increment ----

	for (num=0, perf_itr = perf_period.begin (); perf_itr != perf_period.end (); perf_itr++, num++) {
		if (perf_itr->Volume () == 0) {
			perf_itr->Clear ();
			continue;
		}
		dir_ptr = &sim->dir_array [num];
		link_ptr = &sim->link_array [dir_ptr->Link ()];

		data.Start (sim->time_step - increment);
		data.End (sim->time_step);

		data.Get_Data (&(*perf_itr), dir_ptr, link_ptr);
		perf_itr->Clear ();

		//---- calculate the performance ----

		file->Link (link_ptr->Link ());
		file->Dir (dir_ptr->Dir ());

		file->Type (0);

		file->Start (data.Start ());
		file->End (data.End ());

		file->Time (data.Time ());
		file->Persons (data.Persons ());
		file->Volume (data.Volume ());
		file->Enter (data.Enter ());
		file->Exit (data.Exit ());
		file->Flow (data.Flow ());
		file->Speed (UnRound (data.Speed ()));
		file->Time_Ratio (UnRound (data.Time_Ratio ()));

		file->Delay (data.Delay ());
		file->Density (data.Density ());
		file->Max_Density (data.Max_Density ());
		file->Queue (data.Queue ());
		file->Max_Queue (data.Max_Queue ());
		file->Failure (data.Failure ());
		file->Veh_Dist (UnRound (data.Veh_Dist ()));
		file->Veh_Time (data.Veh_Time ());
		file->Veh_Delay (data.Veh_Delay ());

		if (!file->Write ()) {
			sim->Error (String ("Writing %s") % file->File_Type ());
		}
	}
}


