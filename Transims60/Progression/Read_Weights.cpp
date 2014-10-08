//*********************************************************
//	Read_Weights.cpp - read the group period weights
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Read_Weights
//---------------------------------------------------------

void Progression::Read_Weights (void)
{
	int group, num_periods;
	double weight;
	String record;
	Strings parts;
	Str_Itr str_itr;

	Doubles weights;
	Dbls_Map_Itr grp_itr;
	Dbls_Map_Stat map_stat;
	Dbl_Itr wt_itr;

	Show_Message (String ("Reading %s -- Record") % weight_file.File_Type ());
	Set_Progress ();

	num_periods = progress_time.Num_Periods ();

	while (weight_file.Read ()) {
		Show_Progress ();

		record = weight_file.Record_String ();
		record.Parse (parts);

		if ((int) parts.size () < 2) continue;

		str_itr = parts.begin ();
		group = str_itr->Integer ();

		weights.clear ();
		weight = 1.0;

		for (++str_itr; str_itr != parts.end (); str_itr++) {
			weight = str_itr->Double ();

			if (weight < 0.01 || weight > 1000.0) {
				Error (String ("Group %d Period Weight %.2lf is Out of Range (0.01..1000.0)") % group % weight);
			}
			weights.push_back (weight);
		}

		//---- check the data ----

		while ((int) weights.size () < num_periods) {
			weights.push_back (weight);
		}

		map_stat = weight_data.insert (Dbls_Map_Data (group, weights));

		if (!map_stat.second) {
			Warning ("Duplicate Weight Group = ") << group;
		}
	}
	End_Progress ();
	weight_file.Close ();

	Print (2, "Number of Group Period Weight Records = ") << Progress_Count ();

	if (Report_Flag (WEIGHT_REPORT)) {

		//---- print the report ----

		Header_Number (WEIGHT_REPORT);

		if (!Break_Check (weight_data.size () + 6)) {
			Print (1);
			Weight_Header ();
		}

		for (grp_itr = weight_data.begin (); grp_itr != weight_data.end (); grp_itr++) {
			Print (1, String ("%5d") % grp_itr->first);

			for (wt_itr = grp_itr->second.begin (); wt_itr != grp_itr->second.end (); wt_itr++) {
				Print (0, String ("  %8.2lf") % *wt_itr);
			}
		}
		Header_Number (0);
	}
}

//---------------------------------------------------------
//	Weight_Header
//---------------------------------------------------------

void Progression::Weight_Header (void)
{
	int i;

	Print (1, "Group Period Weight Report");
	Print (2, "Group");

	for (i=1; i <= max_period; i++) {
		Print (0, String ("  Period%2d") % i);
	}
	Print (1);
}
	 
/*********************************************|***********************************************

	Group Period Weight Report

	Group  Period 1  Period 2  Period 3  Period 4  Period 5  Period 6  Period 7  Period 8

	ddddd  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff

**********************************************|***********************************************/ 
