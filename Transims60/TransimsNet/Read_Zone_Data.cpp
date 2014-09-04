//*********************************************************
//	Read_Zone_Data.cpp - Read the Data File into Memory
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Zone_Data
//---------------------------------------------------------

void TransimsNet::Read_Zone_Data (void)
{
	int zone, count;

	//---- read the data file ----

	Show_Message (String ("Reading %s -- Record") % zone_data_file.File_Type ());
	Set_Progress ();

	zone_data_array.Replicate_Fields (&zone_data_file);

	count = 0;

	while (zone_data_file.Read ()) {
		Show_Progress ();

		zone = zone_data_file.Zone ();
		if (zone <= 0) continue;

		update_zone_range.Add_Range (zone, zone);

		//---- copy the data fields ----

		zone_data_array.Copy_Fields (zone_data_file);

		//---- save the database record ----

		if (!zone_data_array.Write_Record (zone)) {
			Error ("Writing Zone Data File Database");
		}
		count++;
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % zone_data_file.File_Type () % count);
		
	zone_data_file.Close ();
}
