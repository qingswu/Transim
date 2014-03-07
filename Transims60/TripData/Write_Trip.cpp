//*********************************************************
//	Write_Trip.cpp - Write the New Trip Table File
//*********************************************************

#include "TripData.hpp"

//---------------------------------------------------------
//	Write_Trip
//---------------------------------------------------------

void TripData::Write_Trip ()
{
	int num_in, num_out;

	num_in = num_out = 0;

	Show_Message (String ("Writing %s -- Record") % new_trip_file.File_Type ());
	Set_Progress ();

	while (trip_file.Read ()) {
		Show_Progress ();

		num_in++;
		new_trip_file.Copy_Fields (trip_file);

		//---- execute the conversion script ----

		if (script_flag) {
			if (program.Execute (num_in) == 0) continue;
		}

		//---- save the output fields ----

		if (!new_trip_file.Write ()) {
			Error (String ("Writing %s Record %d") % new_trip_file.File_Type () % num_in);
		}
		num_out++;
	}
	End_Progress ();

	trip_file.Close ();
	new_trip_file.Close ();

	Write (2, "Number of Trip Records Read = ") << num_in;
	Write (1, "Number of Trip Records Written = ") << num_out;
}

