//*********************************************************
//	Process.cpp - group processing
//*********************************************************

#include "TcadImport.hpp"

//---------------------------------------------------------
//	Process
//---------------------------------------------------------

void TcadImport::Process (void)
{
	int org, des, i, z, count;
	double trip;

	//---- read and convert the input trip data ----
	
	Show_Message (2, String ("Reading %s -- Record") % input_file.File_Type ());
	Set_Progress ();

	while (input_file.Read ()) {
		Show_Progress ();

		org = input_file.Get_Integer (org_field);

		if (org < 1 || org > num_zones) {
			Warning (String ("Origin Zone %d is Out of Range (1..%d)") % org % num_zones);
			continue;
		}
		des = input_file.Get_Integer (des_field);

		if (des < 1 || des > num_zones) {
			Warning (String ("Destination Zone %d is Out of Range (1..%d)") % des % num_zones);
			continue;
		}
		org--;
		des--;

		for (i=0; i < tables; i++) {
			trip = input_file.Get_Double (table_fields [i]);

			if (trip > 0.0) {
				trips [i] [org] [des] = (float) trip;
			}
		}
	}
	End_Progress ();

	count = 0;

	//---- write the new trip tables ----

	for (i=0; i < tables; i++) {
		Show_Message (String ("Writing %s -- Record") % table_names [i]);
		Set_Progress ();

		MATRIX_SetCore (new_trip, (short) i);

		for (z=0; z < num_zones; z++) {
			Show_Progress ();

			MATRIX_SetBaseVector (new_trip, z, MATRIX_ROW, FLOAT_TYPE, trips [i] [z]);
			count++;
		}
		End_Progress ();
	}
	Write (1, "Number of New Matrix Records ") << count;

	MATRIX_Done (new_trip);
	return;
}
