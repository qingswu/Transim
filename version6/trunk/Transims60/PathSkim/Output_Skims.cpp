//*********************************************************
//	Output_Skims.cpp - output skim data
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Output_Skims
//---------------------------------------------------------

void PathSkim::Output_Skims (int period)
{
	int org, des, factor;
	Doubles des_data;
	Skim_Data *skim_ptr;

	factor = skim_file->Neighbor_Factor ();

	if (factor != 0) {
		Int_Map *org_map, *des_map;
		Int_Map_Itr org_itr, des_itr;
		Skim_Data *best_ptr, *intra_ptr;
		bool best_flag, intra_flag;

		org_map = skim_file->Org_Map ();
		des_map = skim_file->Des_Map ();

		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
			best_flag = intra_flag = false;
			best_ptr = intra_ptr = 0;

			org = org_itr->second;

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {

				des = des_itr->second;
				skim_ptr = skim_file->Table (org, des);

				if (org_itr->first == des_itr->first) {
					intra_ptr = skim_ptr;
					intra_flag = true;
				} else if (best_flag) {
					if (best_ptr->Impedance () > skim_ptr->Impedance ()) {
						best_ptr = skim_ptr;
					}
				} else if (skim_ptr->Count () > 0) {
					best_flag = true;
					best_ptr = skim_ptr;
				}
			}
			if (best_flag && intra_flag) {
				intra_ptr->Count (best_ptr->Count ());
				intra_ptr->Walk ((best_ptr->Walk () * factor + 50) / 100);
				intra_ptr->Drive ((best_ptr->Drive () * factor + 50) / 100);
				intra_ptr->Transit ((best_ptr->Transit () * factor + 50) / 100);
				intra_ptr->Wait ((best_ptr->Wait () * factor + 50) / 100);
				intra_ptr->Other ((best_ptr->Other () * factor + 50) / 100);
				intra_ptr->Length ((best_ptr->Length () * factor + 50) / 100);
				intra_ptr->Cost ((best_ptr->Cost () * factor + 50) / 100);
				intra_ptr->Impedance ((best_ptr->Impedance () * factor + 50) / 100);
			}
		}
	}
	for (org=0; org < skim_file->Num_Org (); org++) {
		for (des=0; des < skim_file->Num_Des (); des++) {
			skim_ptr = skim_file->Table (org, des);

			if (skim_ptr->Count () > 0) {
				cells_out++;
				per_out.insert (period);
				org_out.insert (org);
				des_out.insert (des);
			}
		}
	}
	if (accessibility_flag) {
		int zone;
		double wt;
		Dtime ttime;
		Int_Map *org_map, *des_map;
		Int_Map_Itr org_itr, des_itr, int_itr;

		org_map = skim_file->Org_Map ();
		des_map = skim_file->Des_Map ();

		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {

			int_itr = zone_map.find (org_itr->first);
			if (int_itr == zone_map.end ()) continue;

			zone = int_itr->second;
			org = org_itr->second;

			if (org_wt [zone] <= 0.0) continue;

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {

				int_itr = zone_map.find (des_itr->first);
				if (int_itr == zone_map.end ()) continue;

				wt = des_wt [int_itr->second];
				if (wt <= 0.0) continue;

				des = des_itr->second;
				skim_ptr = skim_file->Table (org, des);

				if (skim_file->Data_Type () == TIME_TABLE  || skim_file->Total_Time_Flag ()) {
					ttime = skim_ptr->Time ();
				} else {
					ttime = skim_ptr->Drive () + skim_ptr->Transit ();
				}
				if (ttime <= max_travel_time) {
					des_wt_total [zone] += wt;
				}
			}
		}
	}
	Write_Skims (period);
}
