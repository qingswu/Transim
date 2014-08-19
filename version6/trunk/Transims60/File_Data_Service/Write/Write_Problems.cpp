//*********************************************************
//	Write_Problems.cpp - write a new problem file
//*********************************************************

#include "Data_Service.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Write_Problems
//---------------------------------------------------------

void Data_Service::Write_Problems (void)
{
	int part, num_part, count;

	Problem_File *file = (Problem_File *) System_File_Handle (NEW_PROBLEM);

	Problem_Itr itr;
	Partition_Files <Problem_File> new_file_set;

	part = 0;

	if (file->Part_Flag ()) {
		for (num_part=0, itr = problem_array.begin (); itr != problem_array.end (); itr++) {
			//part = Partition_Index (itr->Partition ());
			if (part > num_part) num_part = part;
		}
		new_file_set.Initialize (file, ++num_part);
		Show_Message (String ("Writing %ss -- Record") % file->File_Type ());
	} else {
		num_part = 1;
		Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	}
	Set_Progress ();

	for (count=0, itr = problem_array.begin (); itr != problem_array.end (); itr++) {
		Show_Progress ();

		//if (itr->Partition () < 0) continue;

		//if (file->Part_Flag ()) {
		//	part = Partition_Index (itr->Partition ());
		//	if (part < 0) continue;
		//	file = new_file_set [part];
		//}
		count += Put_Problem_Data (*file, *itr);
	}
	End_Progress ();

	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
	if (num_part > 1) Print (0, String (" (%d files)") % num_part);
}

//---------------------------------------------------------
//	Put_Problem_Data
//---------------------------------------------------------

int Data_Service::Put_Problem_Data (Problem_File &file, Problem_Data &data)
{
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	file.Problem (data.Problem ());

	file.Time (data.Time ());
	file.Survey (data.Survey ());

	if (data.Dir_Index () >= 0) {
		dir_ptr = &dir_array [data.Dir_Index ()];
		link_ptr = &link_array [dir_ptr->Link ()];

		file.Link (link_ptr->Link ());
		file.Dir (dir_ptr->Dir ());
		file.Offset (UnRound (data.Offset ()));
		file.Lane (Make_Lane_ID (dir_ptr, data.Lane ()));
	} else {
		file.Link (0);
		file.Dir (0);
		file.Offset (0);
		file.Lane (0);
	}
	if (data.Route () >= 0 && System_File_Flag (TRANSIT_ROUTE)) {
		Line_Data *line_ptr = &line_array [data.Route ()];
		file.Route (line_ptr->Route ());
	} else {
		file.Route (0);
	}
	if (data.Notes () == 0) {
		file.Notes ((char *) Problem_Code ((Problem_Type) data.Problem ()));
	} else {
		file.Notes (data.Notes ());
	}
	data.External_IDs ();

	return (Put_Trip_Data (file, data));
}
