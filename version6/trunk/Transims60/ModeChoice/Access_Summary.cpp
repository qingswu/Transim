//*********************************************************
//	Access_Summary.cpp - access market summary report
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Access_Summary
//---------------------------------------------------------

void ModeChoice::Access_Summary (void)
{
	int i, j;
	double total, percent;
	String text;
	Doubles *mode_ptr;
	Dbl_Itr tab_itr;

	for (j=0; j < num_access; j++) {
		header_value = j;
		Header_Number (ACCESS_MARKET);

		if (!Break_Check (num_modes + 8)) {
			Print (1);
			Access_Header ();
		}

		//---- get the total number of trips ----

		total = access_sum [j] [num_modes] [num_tables];

		if (total == 0.0) return;

		//---- process each mode ----

		for (i=0; i <= num_modes; i++) {
			if (i == num_modes) {
				Print (1);
				text = "Total";
			} else {
				text = mode_names [i];
			}
			Print (1, String ("%-20s") % text);

			mode_ptr = &access_sum [j] [i];

			for (tab_itr = mode_ptr->begin (); tab_itr != mode_ptr->end (); tab_itr++) {
				Print (0, String ("  %10.1lf") % *tab_itr);
			}
			percent = mode_ptr->at (num_tables) * 100.0 / total;

			Print (0, String ("  %6.2lf%%") % percent % FINISH);
		}

		//---- table percents ----

		text = "Percent";
		Print (1, String ("%-20s") % text);

		mode_ptr = &access_sum [j] [num_modes];

		for (tab_itr = mode_ptr->begin (); tab_itr != mode_ptr->end (); tab_itr++) {
			percent = *tab_itr * 100.0 / total;
			Print (0, String ("     %6.2lf%%") % percent % FINISH);
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Access_Header
//---------------------------------------------------------

void ModeChoice::Access_Header (void)
{
	int i;
	String text;
	Field_Ptr fld_ptr;

	Print (1, "Access Market Summary -- ") << access_names [header_value] << " -- " << purpose_label;
	Print (2, "Trips by Mode       ");

	for (i=0; i < num_tables; i++) {
		fld_ptr = trip_file->Field (table_map [i]);
		text = fld_ptr->Name ();
		if (text.length () < 3) {
			text = "Table " + text;
		}
		Print (0, String ("  %10s") % text);
	}
	Print (0, "       Total  Percent");
	Print (1);
}

/*********************************************|***********************************************

	Access Market Summary -- ssssssssssssssss -- sssssssssssss

	Trips by Mode         ssssssssss  ssssssssss  ssssssssss  ssssssssss       Total  Percent

	ssssssss20ssssssssss  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  fff.ff%
	
	Total                 ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f
	Percent                  fff.ff%     fff.ff%     fff.ff%     fff.ff%

**********************************************|***********************************************/ 
