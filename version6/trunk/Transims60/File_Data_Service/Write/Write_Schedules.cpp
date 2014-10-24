//*********************************************************
//	Write_Schedules.cpp - write a new transit schedule file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Schedules
//---------------------------------------------------------

void Data_Service::Write_Schedules (void)
{
	Schedule_File *file = System_Schedule_File (true);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = line_map.begin (); itr != line_map.end (); itr++) {
		Show_Progress ();

		count += Put_Schedule_Data (*file, line_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();

	line_array.Schedule_Records (count);
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Schedule_Data
//---------------------------------------------------------

int Data_Service::Put_Schedule_Data (Schedule_File &file, Line_Data &data)
{
	Line_Stop_Itr stop_itr;
	Line_Run *run_ptr;
	Stop_Data *stop_ptr;

	int i, j, run, runs, count;
	Dtime schedule;
	bool type_flag;

	runs = count = 0;
	for (stop_itr = data.begin (); stop_itr != data.end (); stop_itr++) {
		run = (int) stop_itr->size ();

		if (run > 0) {
			count++;
			if (run > runs) runs = run;
		}
	}
	if (count == 0) return (0);

	type_flag = (data.run_types.size () > 0);
	if (type_flag) count++;

	file.Route (data.Route ());
	file.Stops (count);
	if (Notes_Name_Flag ()) {
		file.Label (data.Name ());
		file.Notes (data.Notes ());
	}
	count = 0;

	for (i=0; i < runs; i += NUM_SCHEDULE_COLUMNS) {
		file.Clear_Columns ();

		//---- save the header record ----

		for (j=0; j < NUM_SCHEDULE_COLUMNS; j++) {
			run = i + j;
			if (run == runs) break;
			file.Run (j, run + 1);
		}
		if (!file.Write (false)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;

		if (type_flag) {
			Veh_Type_Data *veh_type_ptr;

			//---- write the run type data ----

			file.Stop (0);
			if (Notes_Name_Flag ()) {
				file.Name ("Vehicle Types");
			}
			for (j=0; j < NUM_SCHEDULE_COLUMNS; j++) {
				run = i + j;
				if (run == runs) break;

				veh_type_ptr = &veh_type_array [data.Run_Type (run)];

				file.Run_Type (j, veh_type_ptr->Type ());
			}
			if (!file.Write (true)) {
				Error (String ("Writing %s") % file.File_Type ());
			}
			count++;
		}

		//---- save the nested stop records ----

		for (stop_itr = data.begin (); stop_itr != data.end (); stop_itr++) {
			if (stop_itr->size () == 0) continue;

			stop_ptr = &stop_array [stop_itr->Stop ()];
			file.Stop (stop_ptr->Stop ());
			if (Notes_Name_Flag ()) {
				file.Name (stop_ptr->Name ());
			}
			for (j=0; j < NUM_SCHEDULE_COLUMNS; j++) {
				run = i + j;
				if (run == runs) break;
				if (run < (int) stop_itr->size ()) {
					run_ptr = &(stop_itr->at (run));
					schedule = run_ptr->Schedule ();
				} else {
					schedule = Model_End_Time ();
				}
				file.Time (j, schedule);
			}
			if (!file.Write (true)) {
				Error (String ("Writing %s") % file.File_Type ());
			}
			count++;
		}
	}
	return (count);
}
