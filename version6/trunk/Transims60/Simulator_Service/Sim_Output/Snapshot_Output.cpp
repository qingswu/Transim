//*********************************************************
//	Snapshot_Output.cpp - Output Interface Class
//*********************************************************

#include "Snapshot_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Snapshot_Output constructor
//---------------------------------------------------------

Snapshot_Output::Snapshot_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (SNAPSHOT_OUTPUT_OFFSET);
	coord_flag = size_flag = cell_flag = compress = false;
	max_size = num_records = 0;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_SNAPSHOT_FILE, num));

	file = new Snapshot_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_SNAPSHOT_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- check for compression ----

	compress = sim->Set_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_COMPRESSION, num);
	file->Compress_Flag (compress);

	if (compress) {
		file->Dbase_Format (BINARY);
	} else {

		//---- get the file format ----

		if (sim->Master ()) {
			key = sim->Get_Control_String (Sim_Output_Step::NEW_SNAPSHOT_FORMAT, num);

			if (!key.empty ()) {
				file->Dbase_Format (key);
			}
			if (file->Dbase_Format () != BINARY) {
				temp_file = new Snapshot_File (CREATE, BINARY);
			}
		} else {
			file->Dbase_Format (BINARY);
		}

		//---- set the location flag ----

		file->Location_Flag (sim->Set_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_LOCATION_FLAG, num));

		//---- set the cell flag ----

		file->Cell_Flag (sim->Set_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_CELL_FLAG, num));

		//---- set the status flag ----

		file->Status_Flag (sim->Set_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_STATUS_FLAG, num));
	}

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	} else {
		file->Create_Fields ();
	}

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_SNAPSHOT_TIME_FORMAT, num);

	//---- time increment ----

	time_range.Increment (sim->Get_Control_Time (Sim_Output_Step::NEW_SNAPSHOT_INCREMENT, num));

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_SNAPSHOT_TIME_RANGE, num));

	//---- get the link range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_SNAPSHOT_LINK_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		link_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_SNAPSHOT_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}

	//---- get the coordinates ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_SNAPSHOT_COORDINATES, num);

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

	//---- get the max size ----

	max_size = sim->Get_Control_Integer (Sim_Output_Step::NEW_SNAPSHOT_MAX_SIZE, num);

	if (!compress) {
		max_size *= 1024 * 1024;

		//---- print the location flag ----

		sim->Get_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_LOCATION_FLAG, num);

		//---- print the cell flag ----

		cell_flag = sim->Get_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_CELL_FLAG, num);

		//---- print the status flag ----

		sim->Get_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_STATUS_FLAG, num);

	} else {
		max_size = (max_size * 1024 * 1024) / sizeof (Compressed_Snapshot);
		cell_flag = false;
	}

	//---- print the compress flag ----

	sim->Get_Control_Flag (Sim_Output_Step::NEW_SNAPSHOT_COMPRESSION, num);
	return;

coord_error:
	sim->Error (String ("Output Snapshot Coordinate Data #%d") % num);
}

//---------------------------------------------------------
//	Snapshot_Output -- destructor
//---------------------------------------------------------

Snapshot_Output::~Snapshot_Output ()
{
	if (sim->Master ()) {
		if (compress && time_range.In_Range (sim->time_step) && time_range.At_Increment (sim->time_step)) {
			if (!file->Write_Index (sim->time_step, num_records)) {
				sim->Error ("Writing Snapshot Index File");
			}
		}
	}
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Snapshot_Output::Output_Check (void)
{
	if (sim->time_step <= sim->Model_Start_Time ()) return (false);

	bool first = true;
	Int_Itr int_itr;

	if (size_flag) return (false);
	if (time_range.In_Range (sim->time_step) && time_range.At_Increment (sim->time_step)) {

		int dir_index, offset, speed, lane, traveler, idx;

		Dir_Itr dir_itr;
		Link_Data *link_ptr;
		Sim_Dir_Ptr sim_dir_ptr;
		Sim_Travel_Ptr sim_travel_ptr;

		if (first) first = false;

		if (sim->Master ()) {
			if (compress) {
				if (!file->Write_Index (sim->time_step, num_records)) {
					sim->Error ("Writing Snapshot Index File");
				}
			}
		} else {
			data.Size (0);
		}

		//---- process each link ----

		for (dir_index = 0, dir_itr = sim->dir_array.begin (); dir_itr != sim->dir_array.end (); dir_itr++, dir_index++) {
			link_ptr = &sim->link_array [dir_itr->Link ()];

			if (!link_range.empty () && !link_range.In_Range (link_ptr->Link ())) continue;

			sim_dir_ptr = &sim->sim_dir_array [dir_index];

			if (!subarea_range.empty ()) {
				if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) continue;
			}
				
			//---- process each cell ----

			for (idx=0, int_itr = sim_dir_ptr->begin (); int_itr != sim_dir_ptr->end (); int_itr++, idx++) {

				traveler = abs (*int_itr);

				if (traveler < 2) continue;

				offset = sim_dir_ptr->Cell (idx) * sim->param.cell_size;
				if (offset > link_ptr->Length ()) offset = link_ptr->Length ();

				lane = sim_dir_ptr->Lane (idx);

				//---- process each vehicle on the lane ----

				sim_travel_ptr = &sim->sim_travel_array [traveler];

				file->Household (sim_travel_ptr->Household ());
				file->Vehicle (sim_travel_ptr->sim_plan_ptr->Vehicle ());
				file->Time (sim->time_step);

				speed = sim_travel_ptr->Speed (); // * sim->param.cell_size;	

				file->Speed (UnRound (speed));

				Veh_Type_Data *type_ptr = &sim->veh_type_array [sim_travel_ptr->sim_plan_ptr->Veh_Type ()];

				file->Type (type_ptr->Type ());
				file->Passengers (sim_travel_ptr->Passengers ());

				if (file->Status_Flag ()) {
					file->Wait (Resolve (sim_travel_ptr->Wait ()));
					file->Time_Diff (0);
					//file->User (sim_veh_ptr->Priority ());

					Dtime schedule = sim_travel_ptr->Schedule ();
					Sim_Leg_Itr leg_itr = sim_travel_ptr->sim_plan_ptr->begin ();

					schedule += leg_itr->Time () * offset / link_ptr->Length ();
					schedule = sim->time_step - schedule;

					file->Time_Diff (DTOI (schedule.Seconds ()));
				}

				if (file->LinkDir_Type () == LINK_SIGN) {
					file->Link (-link_ptr->Link ());
				} else {
					file->Link (link_ptr->Link ());

					if (file->LinkDir_Type () == LINK_DIR) {
						file->Dir (dir_itr->Dir ());
					}
				}

				if (coord_flag || file->Location_Flag () || 
					file->LinkDir_Type () == LINK_NODE) {

					int ax, ay, bx, by;
					Node_Data *node_ptr;

					if (dir_itr->Dir () == 0) {
						node_ptr = &sim->node_array [link_ptr->Bnode ()];
						bx = node_ptr->X ();
						by = node_ptr->Y ();
						node_ptr = &sim->node_array [link_ptr->Anode ()];
					} else {
						node_ptr = &sim->node_array [link_ptr->Anode ()];
						bx = node_ptr->X ();
						by = node_ptr->Y ();
						node_ptr = &sim->node_array [link_ptr->Bnode ()];
					}
					bx -= ax = node_ptr->X ();
					by -= ay = node_ptr->Y ();

					double length = link_ptr->Length ();

					ax += (int) (bx * offset / length + 0.5);
					ay += (int) (by * offset / length + 0.5);

					if (coord_flag) {
						if (ax < x1 || ax > x2 ||
							ay < y1 || ay > y2) {
									
							if (!cell_flag) break;
							continue;
						}
					}
					if (file->Location_Flag ()) {
						bx = sim->compass.Direction ((double) bx, (double) by);
						file->X (UnRound (ax));
						file->Y (UnRound (ay));
						file->Z (0.0);
						file->Bearing ((double) bx);
					}
					if (file->LinkDir_Type () == LINK_NODE) {
						file->Dir (node_ptr->Node ());
					}

					//---- save the cell ----

					if (compress) {
						file->Lane (lane);
						file->Offset (Resolve (offset));
					} else {
						lane = sim->Make_Lane_ID (&(*dir_itr), lane);

						file->Cell (0);
						file->Lane (lane);
						file->Offset (UnRound (offset));
					}
					if (sim->Master ()) {
						if (!file->Write ()) {
							sim->Error ("Writing Snapshot Output File");
						}
					} else {
						data.Add_Data (file->Record ());
					}
					num_records++;
				}

				//---- check the file size ----

				if (max_size > 0) {
					if (compress) {
						if (num_records >= max_size) {
							size_flag = true;
						}
						break;
					} else {
						if ((unsigned) file->Db_File::Offset () >= max_size) {
							size_flag = true;
						}
						break;
					}
				}
			}
		}
	} else if (sim->Master () && compress && time_range.In_Range (sim->time_step-1) && time_range.At_Increment (sim->time_step)) {
		if (!file->Write_Index (sim->time_step, num_records)) {
			sim->Error ("Writing Snapshot Index File");
		}
	}
	return (true);
}

