//*********************************************************
//	Write_Table.cpp - write trip table
//*********************************************************

#include "Fratar.hpp"

//---------------------------------------------------------
//	Write _Table
//---------------------------------------------------------

void Fratar::Write_Table (void)
{
	int org, des, period, num_org, num_des, num_periods, trp;
	double trips, total, bucket;

	Int_Map *org_ptr, *des_ptr;
	Int_Map_Itr org_itr, des_itr;

	num_org = new_file.Num_Org ();
	num_des = new_file.Num_Des ();
	num_periods = new_file.Num_Periods ();

	//---- write the trip table ----

	org_ptr = new_file.Org_Map ();
	des_ptr = new_file.Des_Map ();
	
	Show_Message (1, String ("Writing %s -- Record") % new_file.File_Type ());
	Set_Progress ();
	total = 0;
	bucket = 0.5;
	trp = 0;

	if (num_periods > 1) {

		//---- bucket round ----

		for (org=0; org < num_org; org++) {
			for (des=0; des < num_des; des++) {
				for (period=0; period < num_periods; period++) {
					Show_Progress ();

					trips = odt_array [period] [org] [des];
					if (trips > 0.0) {
						trips += bucket;
						trp = (int) trips;
						bucket = trips - trp;
						odt_array [period] [org] [des] = trp;
					}
				}
			}
		}

		//---- write the results in sorted order ----
	
		Set_Progress ();

		for (period=0; period < num_periods; period++) {
			new_file.Period (period);

			for (org_itr = org_ptr->begin (); org_itr != org_ptr->end (); org_itr++) {
				new_file.Origin (org_itr->first);
				org = org_itr->second;

				for (des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++) {
					Show_Progress ();

					new_file.Destination (des_itr->first);
					des = des_itr->second;

					trp = (int) odt_array [period] [org] [des];
					new_file.Table (0, trp);
					total += trp;

					new_file.Write ();
				}
			}
		}

	} else {

		//---- bucket round in sorted order ----

		new_file.Period (0);

		for (org_itr = org_ptr->begin (); org_itr != org_ptr->end (); org_itr++) {
			new_file.Origin (org_itr->first);
			org = org_itr->second;

			for (des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++) {
				Show_Progress ();

				new_file.Destination (des_itr->first);
				des = des_itr->second;

				trips = odt_array [0] [org] [des];
				if (trips > 0.0) {
					trips += bucket;
					trp = (int) trips;
					bucket = trips - trp;
				}
				new_file.Table (0, trp);
				total += trp;

				new_file.Write ();
			}
		}
	}
	End_Progress ();

	new_file.Close ();

	Print (2, String ("%s has %d Records and %.0lf Trips") % in_file.File_Type () % Progress_Count () % total);
}
