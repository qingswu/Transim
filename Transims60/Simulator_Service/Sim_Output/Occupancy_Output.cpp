//*********************************************************
//	Occupancy_Output.cpp - Output Interface Class
//*********************************************************

#include "Occupancy_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Occupancy_Output constructor
//---------------------------------------------------------

Occupancy_Output::Occupancy_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (OCCUPANCY_OUTPUT_OFFSET);
	data_flag = coord_flag = max_flag = false;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_OCCUPANCY_FILE, num));

	file = new Occupancy_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----
			
	key = sim->Get_Control_String (Sim_Output_Step::NEW_OCCUPANCY_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_OCCUPANCY_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- create the file ----

	max_flag = sim->Set_Control_Flag (Sim_Output_Step::NEW_OCCUPANCY_MAX_FLAG, num);

	file->Max_Flag (max_flag);
	file->Create ();

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_OCCUPANCY_TIME_FORMAT, num);

	//---- time increment ----

	time_range.Increment (sim->Get_Control_Time (Sim_Output_Step::NEW_OCCUPANCY_INCREMENT, num));

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_OCCUPANCY_TIME_RANGE, num));

	//---- get the link range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_OCCUPANCY_LINK_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		link_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_OCCUPANCY_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}

	//---- get the coordinates ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_OCCUPANCY_COORDINATES, num);

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

	//---- print the max flag ----

	sim->Get_Control_Flag (Sim_Output_Step::NEW_OCCUPANCY_MAX_FLAG, num);

	return;

coord_error:
	sim->Error (String ("Output Occupancy Coordinate Data #%d") % num);
}

//---------------------------------------------------------
//	Occupancy_Output destructor
//---------------------------------------------------------

Occupancy_Output::~Occupancy_Output ()
{
	if (file != 0) {
		file->Close ();
	}
}

//---------------------------------------------------------
//	Write_Check
//---------------------------------------------------------

void Occupancy_Output::Write_Check (void)
{
	if (sim->time_step <= sim->Model_Start_Time ()) return;

	if (!time_range.In_Range (sim->time_step)) return;

	if (!data_flag) {

		//---- initialize the link data ----

		int num_rec = (int) sim->dir_array.size ();
		Integers occ_rec, *occ_ptr;

		occ_array.assign (num_rec, occ_rec);
		data_flag = true;
			
		if (max_flag) {
			int index;

			if (!link_range.empty () || coord_flag) {

				Dir_Itr dir_itr;
				Link_Data *link_ptr;
				Sim_Dir_Data *sim_dir_ptr;

				for (index=0, dir_itr = sim->dir_array.begin (); dir_itr != sim->dir_array.end (); dir_itr++, index++) {
					link_ptr = &sim->link_array [dir_itr->Link ()];

					if (!link_range.empty ()) {
						if (!link_range.In_Range (link_ptr->Link ())) continue;
					}
					if (coord_flag) {
						Node_Data *node_ptr;

						node_ptr = &sim->node_array [link_ptr->Anode ()];
						if (node_ptr->X () < x1 || node_ptr->X () > x2 ||
							node_ptr->Y () < y1 || node_ptr->Y () > y2) {

							node_ptr = &sim->node_array [link_ptr->Bnode ()];

							if (node_ptr->X () < x1 || node_ptr->X () > x2 ||
								node_ptr->Y () < y1 || node_ptr->Y () > y2) {
								continue;
							}
						}
					}
					sim_dir_ptr = &sim->sim_dir_array [index];
					if (!subarea_range.empty ()) {
						if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) continue;
					}

					//---- allocate memory ----

					occ_ptr = &occ_array [index];
					occ_ptr->assign (sim_dir_ptr->size (), 0);
				}
			} else {
				Sim_Dir_Itr sim_dir_itr;
							
				//---- allocate memory ----

				for (index=0, sim_dir_itr = sim->sim_dir_array.begin (); sim_dir_itr != sim->sim_dir_array.end (); sim_dir_itr++, index++) {
					if (!subarea_range.empty ()) {
						if (!subarea_range.In_Range (sim_dir_itr->Subarea ())) continue;
					}
					occ_ptr = &occ_array [index];
					occ_ptr->assign (sim_dir_itr->size (), 0);
				}
			}
		}
	} else {
				
		//---- check the output time increment ----

		if (time_range.At_Increment (sim->time_step)) {
			Write_Summary ();
		}
	}

	//---- set the maximum values ----

	if (max_flag) {
		int index, total, sum;
		Ints_Itr dir_itr;
		Int_Itr occ_itr, sim_itr;
		Sim_Dir_Ptr sim_dir_ptr;

		for (index=0, dir_itr = occ_array.begin (); dir_itr != occ_array.end (); dir_itr++, index++) {
			if (dir_itr->size () == 0) continue;
			sim_dir_ptr = &sim->sim_dir_array [index];

			total = 0;

			for (occ_itr = dir_itr->begin (); occ_itr != dir_itr->end (); occ_itr++) {
				total += *occ_itr;
			}
			if (total != 0) {
				sum = 0;

				for (sim_itr = sim_dir_ptr->begin (); sim_itr != sim_dir_ptr->end (); sim_itr++) {
					if (*sim_itr >= 1) sum++;
				}
				if (sum <= total) continue;
			}
			for (occ_itr = dir_itr->begin (), sim_itr = sim_dir_ptr->begin (); occ_itr != dir_itr->end (); occ_itr++, sim_itr++) {
				if (*sim_itr >= 1) {
					*occ_itr = *sim_itr;
				} else {
					*occ_itr = 0;
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Occupancy_Output::Output_Check (Travel_Step &step)
{
	if (max_flag || step.Traveler () < 2 || step.size () == 0 || !data_flag) return;
	
	int dir_index, cell, index;
	bool skip, first;

	Sim_Dir_Data *sim_dir_ptr;
	Sim_Veh_Itr sim_veh_itr;
	Integers *ints_ptr = 0;

	//---- sum the data ----

	if (!time_range.In_Range (sim->time_step)) return;

	//---- movement size ----

	int step_size = sim->param.step_size;

	if (step.sim_dir_ptr != 0) {
		step_size = sim->method_time_step [step.sim_dir_ptr->Method ()];
	}
	if (step.size () > 1) {
		step_size = (int) ((step_size + step.size () / 2) / (step.size () - 1));
		if (step_size < 1) step_size = 1;
	}
	if (step.veh_type_ptr != 0) {
		step_size *= step.veh_type_ptr->Cells ();
	}

	//---- output traveler record ----

	dir_index = -1;
	skip = false;
	first = (step.size () > 1);
	sim_dir_ptr = 0;

	for (sim_veh_itr = step.begin (); sim_veh_itr != step.end (); sim_veh_itr++, first = false) {
		if (dir_index != sim_veh_itr->link) {
			skip = false;

			//---- check the link selection criteria ----

			if (!link_range.empty () || coord_flag) {
				Dir_Data *dir_ptr = &sim->dir_array [sim_veh_itr->link];
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
			if (!skip) {
				sim_dir_ptr = &sim->sim_dir_array [sim_veh_itr->link];
				if (!subarea_range.empty ()) {
					if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) skip = true;
				}
				if (!skip) {

					//---- allocate memory ----

					ints_ptr = &occ_array [sim_veh_itr->link];
					if (ints_ptr->size () == 0) {
						if (sim_dir_ptr->size () > 0) {
							ints_ptr->assign (sim_dir_ptr->size (), 0);
						} else {
							skip = true;
						}
					}
				}
			}
			dir_index = sim_veh_itr->link;
		}
		if (skip || first) continue;

		cell = sim->Offset_Cell (sim_veh_itr->offset);

		index = sim_dir_ptr->Index (sim_veh_itr->lane, cell);

		ints_ptr->at (index) += step_size;
	}
}

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void Occupancy_Output::Write_Summary (void)
{
	int index, idx, offset, lane, cell;
	double occupancy;

	Ints_Itr ints_itr;
	Int_Itr int_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Sim_Dir_Data *sim_dir_ptr;
		
	file->Start (sim->time_step - time_range.Increment ());
	file->End (sim->time_step);

	for (index=0, ints_itr = occ_array.begin (); ints_itr != occ_array.end (); ints_itr++, index++) {
		if (ints_itr->size () == 0) continue;

		sim_dir_ptr = &sim->sim_dir_array [index];
		if (!subarea_range.empty ()) {
			if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) continue;
		}
		dir_ptr = &sim->dir_array [index];
		link_ptr = &sim->link_array [dir_ptr->Link ()];

		if (file->LinkDir_Type () == LINK_SIGN) {
			file->Link (-link_ptr->Link ());
		} else {
			file->Link (link_ptr->Link ());

			if (file->LinkDir_Type () == LINK_DIR) {
				file->Dir (dir_ptr->Dir ());
			}
		}
		for (idx=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, idx++) {
			if (*int_itr == 0) continue;

			lane = sim_dir_ptr->Lane (idx);
			cell = sim_dir_ptr->Cell (idx);

			file->Lane (sim->Make_Lane_ID (dir_ptr, lane));

			if (max_flag) {
				occupancy = *int_itr;
			} else {
				occupancy = Dtime (*int_itr).Seconds ();
			}
			offset = (cell + 1) * sim->param.cell_size;
			if (offset > link_ptr->Length ()) offset = link_ptr->Length ();

			file->Offset (UnRound (offset));
			file->Occupancy (occupancy);

			if (!file->Write ()) {;
				sim->Error ("Writing Occupancy Output File");
			}
		}
		ints_itr->assign (sim_dir_ptr->size (), 0);
	}
}

