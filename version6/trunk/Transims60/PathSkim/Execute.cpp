//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PathSkim::Execute (void)
{
	int index;

	//---- read the network data ----

	Router_Service::Execute ();

	if (veh_type > 0 && System_File_Flag (VEHICLE_TYPE)) {
		Int_Map_Itr map_itr = veh_type_map.find (veh_type);
		if (map_itr == veh_type_map.end ()) {
			Warning (String ("Vehicle Type %d was Not Found") % veh_type);
		}
	}
	if ((int) veh_type_array.size () <= veh_type) {
		Veh_Type_Data veh_type_data;
		veh_type_data.Use (use_type);
		veh_type_data.PCE (1.0);

		veh_type_array.assign (veh_type, veh_type_data);
	}

	//---- select locations within zones ----

	if (user_loc_flag) {
		User_Locations ();
	} else if (zone_skim_flag || district_flag || sel_org_zone || sel_des_zone || zone_loc_flag || num_org > 0 || num_des > 0) {
		Zone_Locations ();
	} else {
		int loc;
		Location_Itr loc_itr;
		Int_Itr org_itr, des_itr;
	
		Show_Message (2, "Building the Location List -- Record");
		Set_Progress ();

		for (index=0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, index++) {
			Show_Progress ();

			if (loc_itr->Zone () < 0) continue;
			loc = loc_itr->Location ();

			if (!select_org || org_range.In_Range (loc)) {
				org_loc.push_back (index);
				if (skim_flag) skim_file->Add_Org (loc);
			}
			if (!select_des || des_range.In_Range (loc)) {
				des_loc.push_back (index);
				if (skim_flag) skim_file->Add_Des (loc);
			}
		}
		End_Progress ();
	}

	if (skim_flag && !zone_skim_flag && !district_flag) {
		if (org_loc.size () == 0 && des_loc.size () == 0) {
			Error ("No Locations were Selected");
		}
		if (org_loc.size () == 0) {
			Error ("No Origin Locations were Selected");
		}
		if (des_loc.size () == 0) {
			Error ("No Destination Locations were Selected");
		}
		if (!skim_file->Set_Table ((int) org_loc.size (), (int) des_loc.size ())) {
			Error (String ("Insufficient Memory for %dx%d Skim Matrix") % org_loc.size () % des_loc.size ());
		}
		skim_file->Write_Header ();
	} else if (org_loc_flag || des_loc_flag) {
		New_Zone_Locations ();
	}

	//---- build performance arrays ----

	if (System_File_Flag (NEW_PERFORMANCE)) {
		if (!System_File_Flag (PERFORMANCE)) {
			Build_Perf_Arrays ();
		} else if (Turn_Updates () && turn_period_array.size () == 0) {
			Build_Turn_Arrays ();
		}
	}

	//---- build paths ----

	if (plan_flag || (!skim_flag && problem_flag)) {
		Build_Paths ();
	} else if (zone_skim_flag || district_flag) {
		Zone_Skims ();
	} else if (skim_flag) {
		Skim_Paths ();
	} else {
		Flow_Skims ();
	}

	//---- output performance ----

	if (Flow_Updates () || Time_Updates ()) {
		if (System_File_Flag (NEW_PERFORMANCE)) {
			if (Time_Updates ()) {
				Update_Travel_Times ();
			}
			Show_Message (1);
			Write_Performance ();
		}
	}

	//---- end the program ----

	Report_Problems (total_records);

	Exit_Stat (DONE);
}

