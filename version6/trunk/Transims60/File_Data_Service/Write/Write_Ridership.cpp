//*********************************************************
//	Write_Ridership.cpp - write a new transit ridership file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Ridership
//---------------------------------------------------------

void Data_Service::Write_Ridership (void)
{
	Ridership_File *file = (Ridership_File *) System_File_Handle (NEW_RIDERSHIP);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = line_map.begin (); itr != line_map.end (); itr++) {
		Show_Progress ();

		count += Put_Ridership_Data (*file, line_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Ridership_Data
//---------------------------------------------------------

int Data_Service::Put_Ridership_Data (Ridership_File &file, Line_Data &data)
{
	int i, runs, count;
	double capacity, cap;
	bool type_flag = false;

	Int_Map_Itr itr;
	Line_Stop_Itr stop_itr;
	Line_Run *run_ptr;
	Stop_Data *stop_ptr;
	Veh_Type_Data *veh_type_ptr;
	
	count = 0;

	if (System_File_Flag (VEHICLE_TYPE)) {
		veh_type_ptr = &veh_type_array [data.Type ()];
		capacity = veh_type_ptr->Capacity () / 100.0;		//---- ratio
		type_flag = ((int) data.run_types.size () > 0);
	} else {
		capacity = 100.0;
	}

	file.Route (data.Route ());

	stop_itr = data.begin ();
	runs = (int) stop_itr->size ();

	file.Mode (data.Mode ());
	file.Route (data.Route ());

	for (i=0; i < runs; i++) {
		file.Run (i+1);

		for (stop_itr = data.begin (); stop_itr != data.end (); stop_itr++) {
			run_ptr = &(stop_itr->at (i));

			if (run_ptr->Schedule () == run_ptr->Time () &&	run_ptr->Board () == 0 && 
				run_ptr->Alight () == 0 && run_ptr->Load () == 0) continue;

			stop_ptr = &stop_array [stop_itr->Stop ()];
			file.Stop (stop_ptr->Stop ());

			file.Schedule (run_ptr->Schedule ());
			if (run_ptr->Time () == 0 && run_ptr->Schedule () > 0) {
				file.Time (run_ptr->Schedule ());
			} else {
				file.Time (run_ptr->Time ());
			}
			file.Board (run_ptr->Board ());
			file.Alight (run_ptr->Alight ());
			file.Load (run_ptr->Load ());
			if (run_ptr->Factor () == 0 && run_ptr->Load () > 0) {
				if (type_flag) {
					veh_type_ptr = &veh_type_array [data.Run_Type (i)];
					cap = veh_type_ptr->Capacity () / 100.0;
				} else {
					cap = capacity;
				}
				if (cap > 0) {
					file.Factor (run_ptr->Load () / cap);
				} else {
					file.Factor (100);
				}
			} else {
				file.Factor (run_ptr->Factor ());
			}

			if (!file.Write ()) {
				Error (String ("Writing %s") % file.File_Type ());
			}
			count++;
		}
	}
	return (count);
}
