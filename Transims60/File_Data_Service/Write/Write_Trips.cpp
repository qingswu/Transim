//*********************************************************
//	Write_Trips.cpp - write a new trip file
//*********************************************************

#include "Data_Service.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Write_Trips
//---------------------------------------------------------

void Data_Service::Write_Trips (void)
{
	int part, num_part, count;

	Trip_File *file = (Trip_File *) System_File_Handle (NEW_TRIP);

	Int_Itr int_itr;
	Trip_Map_Itr itr;
	Trip_Itr trip_itr;
	Trip_Data *trip_ptr;
	Partition_Files <Trip_File> new_file_set;

	if (file->Part_Flag ()) {
		num_part = trip_array.Max_Partition () + 1;
		new_file_set.Initialize (file, num_part);
		Show_Message (String ("Writing %ss -- Record") % file->File_Type ());
	} else {
		num_part = 1;
		Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	}
	Set_Progress ();

	for (count=0, itr = trip_map.begin (); itr != trip_map.end (); itr++) {
		Show_Progress ();

		trip_ptr = &trip_array [itr->second];
		if (trip_ptr->Partition () < 0) continue;

		if (file->Part_Flag ()) {
			part = Partition_Index (trip_ptr->Partition ());
			if (part < 0) continue;
			file = new_file_set [part];
		}
		if (trip_ptr->External_IDs ()) {
			count += Put_Trip_Data (*file, *trip_ptr);
		}
	}
	End_Progress ();

	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
	if (num_part > 1) Print (0, String (" (%d files)") % num_part);
}

//---------------------------------------------------------
//	Put_Trip_Data
//---------------------------------------------------------

int Data_Service::Put_Trip_Data (Trip_File &file, Trip_Data &data)
{
	file.Put_Data (data);

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	file.Add_Trip (data.Household (), data.Person (), data.Tour ());
	return (1);
}
