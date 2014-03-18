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
	data_flag = flow_flag = turn_flag = false;
	
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

	//---- get the flow type ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_PERFORMANCE_FLOW_TYPE, num);
	if (!key.empty ()) {
		file->Flow_Units (Flow_Code (key));
	}

	//---- get the lane use flow flag ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_PERFORMANCE_LANE_FLOWS, num);
	if (!key.empty ()) {
		flow_flag = sim->Set_Control_Flag (Sim_Output_Step::NEW_PERFORMANCE_LANE_FLOWS, num);
	}
	file->Lane_Use_Flows (flow_flag);

	//---- get the turn flag ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_PERFORMANCE_TURN_FLAG, num);
	if (!key.empty ()) {
		turn_flag = sim->Set_Control_Flag (Sim_Output_Step::NEW_PERFORMANCE_TURN_FLAG, num);
	}
	file->Turn_Flag (turn_flag);

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

	//---- print the turn flag ----

	sim->Get_Control_Flag (Sim_Output_Step::NEW_PERFORMANCE_TURN_FLAG, num);

	//---- print the flow type ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_PERFORMANCE_FLOW_TYPE, num);
	return;

coord_error:
	sim->Error (String ("Output Link Delay Coordinate Data #%d") % num);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Performance_Output::Output_Check (void)
{
	if (sim->time_step <= sim->Model_Start_Time ()) return (false);;

	if (!time_range.In_Range (sim->time_step)) return (false);

	if (!data_flag) {

		//---- initialize the link data ----

		Link_Perf_Data data_rec;
		Flow_Time_Data turn_rec;

		link_perf.assign (sim->dir_array.size (), data_rec);

		if (turn_flag) {
			turn_perf.assign (sim->connect_array.size (), turn_rec);
		}
		data_flag = true;

	} else {
		Link_Perf_Itr data_itr;

		//---- update the density data ----

		for (data_itr = link_perf.begin (); data_itr != link_perf.end (); data_itr++) {
			if (data_itr->Occupancy () > 0) {
				if (data_itr->Max_Density () < data_itr->Occupancy ()) {
					data_itr->Max_Density (data_itr->Occupancy ());
				}
				data_itr->Add_Density (data_itr->Occupancy ());
				data_itr->Occupancy (0);

				if (data_itr->Max_Queue () < data_itr->Stop_Count ()) {
					data_itr->Max_Queue (data_itr->Stop_Count ());
				}
				data_itr->Add_Queue (data_itr->Stop_Count ());
				data_itr->Stop_Count (0);
			}
		}

		//---- check the output time increment ----

		if (time_range.At_Increment (sim->time_step)) {

			if (sim->Master ()) {
				Write_Summary ();
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Summarize
//---------------------------------------------------------

void Performance_Output::Summarize (Travel_Step &step)
{
	if (step.Traveler () < 0 || !time_range.In_Range (sim->time_step)) return;

	int dir_index, turn_index, cell, cells, offset, occupancy;
	double weight, vmt;
	Dtime vht;
	bool skip;

	Sim_Veh_Itr sim_veh_itr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr;
	Flow_Time_Data *turn_time_ptr;
	Link_Perf_Data *link_perf_ptr;

	//---- get the vehicle type data ----

	if (step.veh_type_ptr == 0) {
		step.veh_type_ptr = &sim->veh_type_array [step.sim_travel_ptr->sim_plan_ptr->Veh_Type ()];
	}
	weight = 1.0;

	if (file->Flow_Units () != VEHICLES || !veh_types.empty ()) {
		if (!veh_types.empty ()) {
			if (!veh_types.In_Range (step.veh_type_ptr->Type ())) return;
		}
		if (file->Flow_Units () == PERSONS) {
			weight += step.sim_travel_ptr->Passengers ();
		} else if (file->Flow_Units () == PCE) {
			if (step.veh_type_ptr > 0) {
				weight = UnRound (step.veh_type_ptr->PCE ());
			} else {
				weight = 1;
			}
		}
	}

	//---- get the connection ID ----

	turn_index = -1;
						
	if (turn_flag) {
		plan_ptr = step.sim_travel_ptr->sim_plan_ptr;
		if (plan_ptr != 0) {
			leg_itr = plan_ptr->begin ();
			if (leg_itr != plan_ptr->end ()) {
				turn_index = leg_itr->Connect ();
			}
		}
	} else {
		plan_ptr = 0;
	}

	//---- movement size ----

	if (step.Time () == 0) {
		step.Time (sim->param.step_size);
	}
	vht = step.Time () * weight;
	cells = (int) step.size ();
	if (cells > 1) vht = vht / (cells - 1);

	//---- process each movement cell ----
	
	dir_index = -1;
	offset = 0;
	skip = false;
	link_perf_ptr = 0;
	turn_time_ptr = 0;

	for (cell=0, sim_veh_itr = step.begin (); sim_veh_itr != step.end (); sim_veh_itr++, cell++) {

		if (sim_veh_itr->link != dir_index) {
			if (cell == 0) {
				offset = sim_veh_itr->offset;
			} else {
				offset = 0;
				if (turn_flag) {
					if (++leg_itr != plan_ptr->end ()) {
						turn_index = leg_itr->Connect ();
					} else {
						turn_index = -1;
					}
				}
			}
			dir_index = sim_veh_itr->link;
			skip = true;

			if (dir_index >= 0) {
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
					Sim_Dir_Ptr sim_dir_ptr = &sim->sim_dir_array [dir_index];

					if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) skip = true;
				}
				if (!skip) {
					link_perf_ptr = &link_perf [dir_index];

					if (turn_flag && turn_index >= 0) {
						turn_time_ptr = &turn_perf [turn_index];
					}
				}
			}
		}
		if (!skip && cell > 0) {
			vmt = (sim_veh_itr->offset - offset) * weight;

			occupancy = Round (weight);

			link_perf_ptr->Add_Flow (vmt);
			link_perf_ptr->Add_Time (vht);
			link_perf_ptr->Add_Occupant (occupancy);

			if (step.Speed () == 0) {
				link_perf_ptr->Add_Stop (occupancy);
			}
			if (turn_flag && turn_index >= 0) {
				turn_time_ptr->Add_Flow (vmt);
				turn_time_ptr->Add_Time (vht);
			}
			offset = sim_veh_itr->offset;
		}
	}
}

//---------------------------------------------------------
//	Cycle_Failure
//---------------------------------------------------------

void Performance_Output::Cycle_Failure (Dtime step, int dir_index, int vehicles, int persons, int veh_type)
{
	if (dir_index < 0 || vehicles < 0) return;

	Link_Perf_Data *data_ptr;

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

	data_ptr = &link_perf [dir_index];

	if (file->Flow_Units () == PERSONS) {
		data_ptr->Add_Failure (persons);
	} else {
		data_ptr->Add_Failure (vehicles);
	}
}

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void Performance_Output::Write_Summary (void)
{
	int num, index, count;
	int i, link [20], increment;
	Dtime time [20];
	double flow [20], speed, ttime, length, rate, density, factor;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Flow_Time_Data *turn_perf_ptr;
	Link_Perf_Itr link_itr;
	Connect_Data *connect_ptr;

	increment = time_range.Increment ();
	if (increment < 1) increment = 1;

	//---- output the current increment ----

	for (num=0, link_itr = link_perf.begin (); link_itr != link_perf.end (); link_itr++, num++) {
		if (link_itr->Flow () == 0) continue;
			
		dir_ptr = &sim->dir_array [num];
		link_ptr = &sim->link_array [dir_ptr->Link ()];

		//---- calculate the performance ----

		if (link_itr->Time () > 0) {
			speed = link_itr->Flow () / link_itr->Time ();
			if (speed < 0.5) speed = 0.5;
		} else {
			speed = 0.5;
		}
		length = UnRound (link_ptr->Length ());
		if (length < 0.1) length = 0.1;

		ttime = length / speed;
		if (ttime < 0.1) ttime = 0.1;

		rate = UnRound (link_itr->Flow () / length);
		if (rate < 0.1) rate = 0.1;

		//---- save the link record ----

		file->Link (link_ptr->Link ());
		file->Dir (dir_ptr->Dir ());
		file->Start (sim->time_step - increment);
		file->End (sim->time_step);
		file->Flow (rate);
		file->Time (Dtime (ttime, SECONDS));
		file->Speed (speed);

		density = (double) link_itr->Density () / increment;
		if (density < 0.01) density = 0.01;

		factor = dir_ptr->Lanes ();
		if (factor < 1) factor = 1;

		if (sim->Metric_Flag ()) {
			factor = 1000.0 / (length * factor);
		} else {
			factor = MILETOFEET / (length * factor);
		}
		density *= factor;
		if (density > 0.0 && density < 0.01) density = 0.01;
		file->Density (density);

		density = UnRound (link_itr->Max_Density ()) * factor;
		if (density > 0.0 && density < 0.01) density = 0.01;
		file->Max_Density (density);

		file->Queue (UnRound (link_itr->Queue ()) / increment);
		file->Max_Queue (Resolve (link_itr->Max_Queue ()));
		file->Cycle_Failure (link_itr->Failure ());

		factor = dir_ptr->Time0 ().Seconds ();
		if (factor < 0.1) factor = 0.1;

		file->Delay (Dtime ((ttime - factor), SECONDS));
		file->Time_Ratio (100 * ttime / factor);

		//---- clear for the next time slice ----

		link_itr->Clear ();

		//---- gather the turn delays ----

		count = 0;

		if (turn_flag) {
			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &sim->connect_array [index];

				turn_perf_ptr = &turn_perf [index];
				if (turn_perf_ptr->Flow () == 0) continue;

				dir_ptr = &sim->dir_array [connect_ptr->To_Index ()];
				link_ptr = &sim->link_array [dir_ptr->Link ()];

				//---- calculate the performance ----

				if (turn_perf_ptr->Time () > 0) {
					speed = turn_perf_ptr->Flow () / turn_perf_ptr->Time ();
					if (speed < 0.5) speed = 0.5;
				} else {
					speed = 0.5;
				}
				ttime = length / speed;
				if (ttime < 0.1) ttime = 0.1;

				rate = UnRound (turn_perf_ptr->Flow () / length);
				if (rate < 0.1) rate = 0.1;

				link [count] = link_ptr->Link ();
				flow [count] = rate;
				time [count] = Dtime (ttime, SECONDS);
				count++;

				//--- clear for the next time slice ----

				turn_perf_ptr->Clear ();
			}
		}
		file->Num_Nest (count);

		if (!file->Write (false)) goto write_error;

		//---- save the turn delays ----

		for (i=0; i < count; i++) {
			file->Out_Link (link [i]);
			file->Out_Flow (flow [i]);
			file->Out_Time (time [i]);

			if (!file->Write (true)) goto write_error;
		}

		////---- calculate the performance ----


	}
	return;

write_error:
	sim->Error (String ("Writing %s") % file->File_Type ());
}

#ifdef MPI_EXE

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Performance_Output::MPI_Processing (Output_Itr output)
{
	int i, num, index, rank, link_size, turn_size, tag;

	Flow_Time_Data *turn_ptr, turn_rec;
	Flow_Time_Itr turn_itr;
	Link_Perf_Data *link_ptr, link_rec;
	Link_Perf_Itr link_itr;
	
	tag = (sim->Num_Threads () > 1) ? NEW_PERFORMANCE_FILE : 0;
	link_size = (int) sizeof (Link_Perf_Data);
	turn_size = (int) sizeof (Flow_Time_Data);

	if (sim->Master ()) {
		for (i=1; i < sim->MPI_Size (); i++) {

			//---- retrieve the data buffer ----

			rank = sim->Get_MPI_Buffer (data, tag);

			num = (int) data.Size () / (sizeof (int) + link_size);

			while (num-- > 0) {
				data.Get_Data (&index, sizeof (int));
				data.Get_Data (&link_rec, link_size);

				link_ptr = &output->link_perf [index];

				link_ptr->Add_Flow (link_rec.Flow ());
				link_ptr->Add_Time (link_rec.Time ());
				link_ptr->Add_Density (link_rec.Density ());
				link_ptr->Add_Max_Density (link_rec.Max_Density ());
				link_ptr->Add_Queue (link_rec.Queue ());
				link_ptr->Add_Max_Queue (link_rec.Max_Queue ());
				link_ptr->Add_Failure (link_rec.Failure ());
				link_ptr->Add_Occupant (link_rec.Occupancy ());
				link_ptr->Add_Stop (link_rec.Stop_Count ());
				link_ptr->Add_Ratio (link_rec.Ratio_Count ());
				link_ptr->Add_Ratio_VMT (link_rec.Ratio_VMT ());
				link_ptr->Add_Ratio_VHT (link_rec.Ratio_VHT ());
			}
			if (output->turn_flag) {
				sim->Get_MPI_Buffer (data, tag, rank);

				num = (int) data.Size () / (sizeof (int) + turn_size);

				while (num-- > 0) {
					data.Get_Data (&index, sizeof (int));
					data.Get_Data (&turn_rec, turn_size);

					turn_ptr = &output->turn_perf [index];

					turn_ptr->Add_Flow (turn_rec.Flow ());
					turn_ptr->Add_Time (turn_rec.Time ());
				}
			}
		}

	} else {	//---- slave ----

		data.Size (0);
			
		for (index=0, link_itr = output->link_perf.begin (); link_itr != output->link_perf.end (); link_itr++, index++) {
			if (link_itr->Time () > 0) {
				data.Add_Data (&index, sizeof (int));
				data.Add_Data (&(*link_itr), link_size);
				link_itr->Clear ();
			}
		}
		sim->Send_MPI_Buffer (data, tag);

		if (output->turn_flag) {
			data.Size (0);

			for (index=0, turn_itr = output->turn_perf.begin (); turn_itr != output->turn_perf.end (); turn_itr++, index++) {
				if (turn_itr->Time () > 0) {
					data.Add_Data (&index, sizeof (int));
					data.Add_Data (&(*turn_itr), turn_size);
					turn_itr->Clear ();
				}
			}
			sim->Send_MPI_Buffer (data, tag);
		}
	}
}
#endif

