//*********************************************************
//	Read_Table.cpp - read trip tables
//*********************************************************

#include "Fratar.hpp"

//---------------------------------------------------------
//	Read_Table
//---------------------------------------------------------

void Fratar::Read_Table (void)
{
	int org, des, ext_org, ext_des, period, trips, total, target, count, num_periods;
	Dtime low, high;
	
	Show_Message (1, String ("Reading %s -- Record") % in_file.File_Type ());
	Set_Progress ();

	total = target = period = count = 0;
	num_periods = new_file.Num_Periods ();

	while (in_file.Read ()) {
		Show_Progress ();

		//---- convert the zone numbers ----

		ext_org = in_file.Origin ();
		if (ext_org == 0) continue;

		trips = (int) in_file.Table (0);

		if (trips < 0) {
			Warning (String ("Number of Trips is Out of Range (%d < 0)") % trips);
			continue;
		}
		total += trips;

		org = new_file.Org_Index (ext_org);
		if (org < 0) continue;

		ext_des = in_file.Destination ();

		des = new_file.Des_Index (ext_des);
		if (des < 0) continue;

		if (period_flag) {
			period = in_file.Period ();
			in_file.Period_Range (period, low, high);
			low = (low + high) / 2;
			period = new_file.Period (low);
			if (period < 0) continue;

			odt_array [period] [org] [des] += trips;
			target += trips;
			count++;
		} else {
			for (period = 0; period < num_periods; period++) {
				odt_array [period] [org] [des] += trips;
				target += trips;
				count++;
			}
		}
	}
	End_Progress ();

	in_file.Close ();

	Print (2, String ("%s has %d Records and %d Trips") % in_file.File_Type () % Progress_Count () % total);

	if (count != Progress_Count ()) {
		Print (1, "Number of Records Kept = ") << count;
	}
	if (target != total) {
		Print (1, "Number of Trips Kept = ") << target;
	}
}
