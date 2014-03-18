//*********************************************************
//	Link_Delay_Output.cpp - Output Interface Class
//*********************************************************

#include "Link_Delay_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Link_Delay_Output constructor
//---------------------------------------------------------

Link_Delay_Output::Link_Delay_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (LINK_DELAY_OUTPUT_OFFSET);
	data_flag = flow_flag = turn_flag = false;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_LINK_DELAY_FILE, num));

	file = new Link_Delay_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----
			
	key = sim->Get_Control_String (Sim_Output_Step::NEW_LINK_DELAY_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_LINK_DELAY_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- get the flow type ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_LINK_DELAY_FLOW_TYPE, num);
	if (!key.empty ()) {
		file->Flow_Units (Flow_Code (key));
	}

	//---- get the lane flow flag ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_LINK_DELAY_LANE_FLOWS, num);
	if (!key.empty ()) {
		flow_flag = sim->Set_Control_Flag (Sim_Output_Step::NEW_LINK_DELAY_LANE_FLOWS, num);
	}
	file->Lane_Use_Flows (flow_flag);

	//---- get the turn flag ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_LINK_DELAY_TURN_FLAG, num);
	if (!key.empty ()) {
		turn_flag = sim->Set_Control_Flag (Sim_Output_Step::NEW_LINK_DELAY_TURN_FLAG, num);
	}
	file->Turn_Flag (turn_flag);

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	}

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_LINK_DELAY_TIME_FORMAT, num);

	//---- time increment ----

	time_range.Increment (sim->Get_Control_Time (Sim_Output_Step::NEW_LINK_DELAY_INCREMENT, num));

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_LINK_DELAY_TIME_RANGE, num));

	//---- get the link range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_LINK_DELAY_LINK_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		link_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_LINK_DELAY_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}

	//---- get the coordinates ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_LINK_DELAY_COORDINATES, num);

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

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_LINK_DELAY_VEH_TYPES, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		veh_types.Add_Ranges (key);
	}

	//---- print the turn flag ----

	sim->Get_Control_Flag (Sim_Output_Step::NEW_LINK_DELAY_TURN_FLAG, num);

	//---- print the flow type ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_LINK_DELAY_FLOW_TYPE, num);

	//---- print the lane use flow flag ----

	sim->Get_Control_Flag (Sim_Output_Step::NEW_LINK_DELAY_LANE_FLOWS, num);
	return;

coord_error:
	sim->Error (String ("Output Link Delay Coordinate Data #%d") % num);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Link_Delay_Output::Output_Check (void)
{
	if (sim->time_step <= sim->Model_Start_Time ()) return (false);

	if (!time_range.In_Range (sim->time_step)) return (false);

	if (!data_flag) {

		//---- initialize the link data ----

		Flow_Time_Data data_rec;

		link_delay.assign (sim->dir_array.size (), data_rec);

		if (turn_flag) {
			turn_delay.assign (sim->connect_array.size (), data_rec);
		}
		data_flag = true;

	} else {
				
		//---- check the output time increment ----

		if (time_range.At_Increment (sim->time_step)) {
#ifdef MPI_EXE
			MPI_Processing ();
#endif
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

void Link_Delay_Output::Summarize (Travel_Step &step)
{
	if (step.sim_travel_ptr == 0 || !time_range.In_Range (sim->time_step)) return;

	int dir_index, turn_index, cell, cells, offset;
	double weight, vmt;
	Dtime vht;
	bool skip;

	Sim_Veh_Itr sim_veh_itr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr;
	Flow_Time_Data *flow_time_ptr, *turn_time_ptr;

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
	flow_time_ptr = turn_time_ptr = 0;

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
					flow_time_ptr = &link_delay [dir_index];

					if (turn_flag && turn_index >= 0) {
						turn_time_ptr = &turn_delay [turn_index];
					}
				}
			}
		}
		if (!skip && cell > 0) {
			vmt = (sim_veh_itr->offset - offset) * weight;

			flow_time_ptr->Add_Flow (vmt);
			flow_time_ptr->Add_Time (vht);

			if (turn_flag && turn_index >= 0) {
				turn_time_ptr->Add_Flow (vmt);
				turn_time_ptr->Add_Time (vht);
			}
			offset = sim_veh_itr->offset;
		}
	}
}

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void Link_Delay_Output::Write_Summary (void)
{
	int num, index, count;
	int i, link [20], increment;
	Dtime time [20];
	double flow [20], speed, ttime, length, rate;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Flow_Time_Data *flow_time_ptr;
	Flow_Time_Itr flow_time_itr;
	Connect_Data *connect_ptr;

	increment = time_range.Increment ();
	if (increment < 1) increment = 1;

	//---- output the current increment ----

	for (num=0, flow_time_itr = link_delay.begin (); flow_time_itr != link_delay.end (); flow_time_itr++, num++) {
		if (flow_time_itr->Flow () == 0) continue;
			
		dir_ptr = &sim->dir_array [num];
		link_ptr = &sim->link_array [dir_ptr->Link ()];

		//---- calculate the performance ----

		if (flow_time_itr->Time () > 0) {
			speed = flow_time_itr->Flow () / flow_time_itr->Time ();
			if (speed < 0.5) speed = 0.5;
		} else {
			speed = 0.5;
		}
		length = UnRound (link_ptr->Length ());
		if (length < 0.1) length = 0.1;

		ttime = length / speed;
		if (ttime < 0.1) ttime = 0.1;

		rate = UnRound (flow_time_itr->Flow () / length);
		if (rate < 0.1) rate = 0.1;

		//---- save the link record ----

		file->Link (link_ptr->Link ());
		file->Dir (dir_ptr->Dir ());
		file->Start (sim->time_step - increment);
		file->End (sim->time_step);
		file->Flow (rate);
		file->Time (Dtime (ttime, SECONDS));

		//---- gather the turn delays ----

		count = 0;

		if (turn_flag) {
			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &sim->connect_array [index];

				flow_time_ptr = &turn_delay [index];
				if (flow_time_ptr->Flow () == 0) continue;

				dir_ptr = &sim->dir_array [connect_ptr->To_Index ()];
				link_ptr = &sim->link_array [dir_ptr->Link ()];

				//---- calculate the performance ----

				if (flow_time_ptr->Time () > 0) {
					speed = flow_time_ptr->Flow () / flow_time_ptr->Time ();
					if (speed < 0.5) speed = 0.5;
				} else {
					speed = 0.5;
				}
				ttime = length / speed;
				if (ttime < 0.1) ttime = 0.1;

				rate = UnRound (flow_time_ptr->Flow () / length);
				if (rate < 0.1) rate = 0.1;

				link [count] = link_ptr->Link ();
				flow [count] = rate;
				time [count] = Dtime (ttime, SECONDS);
				count++;

				//--- clear for the next time slice ----

				flow_time_ptr->Clear ();
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

		//--- clear for the next time slice ----

		flow_time_itr->Clear ();
	}
	return;

write_error:
	sim->Error (String ("Writing %s") % file->File_Type ());
}

#ifdef MPI_EXE

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Link_Delay_Output::MPI_Processing (void)
{
	int i, num, index, rank, size, tag;

	Flow_Time_Data *data_ptr, data_rec;
	Flow_Time_Itr data_itr;
	
	tag = (sim->Num_Threads () > 1) ? NEW_LINK_DELAY_FILE : 0;
	size = (int) sizeof (Flow_Time_Data);

	if (sim->Master ()) {
		for (i=1; i < sim->MPI_Size (); i++) {

			//---- retrieve the data buffer ----

			rank = sim->Get_MPI_Buffer (data, tag);

			num = (int) data.Size () / (sizeof (int) + size);

			while (num-- > 0) {
				data.Get_Data (&index, sizeof (int));
				data.Get_Data (&data_rec, size);

				data_ptr = &link_delay [index];

				data_ptr->Add_Flow (data_rec.Flow ());
				data_ptr->Add_Time (data_rec.Time ());
			}
			if (output->turn_flag) {
				sim->Get_MPI_Buffer (data, tag, rank);

				num = (int) data.Size () / (sizeof (int) + size);

				while (num-- > 0) {
					data.Get_Data (&index, sizeof (int));
					data.Get_Data (&data_rec, size);

					data_ptr = &turn_delay [index];

					data_ptr->Add_Flow (data_rec.Flow ());
					data_ptr->Add_Time (data_rec.Time ());
				}
			}
		}

	} else {	//---- slave ----

		data.Size (0);
			
		for (index=0, data_itr = link_delay.begin (); data_itr != link_delay.end (); data_itr++, index++) {
			if (data_itr->Time () > 0) {
				data.Add_Data (&index, sizeof (int));
				data.Add_Data (&(*data_itr), size);
				data_itr->Clear ();
			}
		}
		sim->Send_MPI_Buffer (data, tag);

		if (output->turn_flag) {
			data.Size (0);

			for (index=0, data_itr = turn_delay.begin (); data_itr != turn_delay.end (); data_itr++, index++) {
				if (data_itr->Time () > 0) {
					data.Add_Data (&index, sizeof (int));
					data.Add_Data (&(*data_itr), size);
					data_itr->Clear ();
				}
			}
			sim->Send_MPI_Buffer (data, tag);
		}
	}
}
#endif
