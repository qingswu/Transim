//*********************************************************
//	Select_Service.cpp - selection service keys and data
//*********************************************************

#include "Select_Service.hpp"
#include "Data_Service.hpp"
#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Select_Service constructor
//---------------------------------------------------------

Select_Service::Select_Service (void)
{
	select_households = select_purposes = select_start_times = select_end_times = select_origins = select_destinations = false;
	select_travelers = select_vehicles = select_problems = select_subareas = select_polygon = select_org_zones = select_des_zones = false;
	select_modes = select_facilities = select_priorities = select_links = select_nodes = delete_flag = select_stops = select_routes = false;
	percent_flag = max_percent_flag = select_parking = select_vc = select_ratio = select_time_of_day = false;
	time_diff_flag = cost_diff_flag = trip_diff_flag = false;
	min_time_diff = max_time_diff = min_trip_diff = max_trip_diff = 0;
	min_cost_diff = max_cost_diff = 0;
	max_min_time_diff = max_min_trip_diff = 1;
	max_min_cost_diff = 1;
	select_percent = max_percent_select = percent_time_diff = percent_cost_diff = percent_trip_diff = 100.0;
	vc_ratio = time_ratio = 0.0;
}

//---------------------------------------------------------
//	Select_Service_Keys
//---------------------------------------------------------

void Select_Service::Select_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ SELECT_HOUSEHOLDS, "SELECT_HOUSEHOLDS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_MODES, "SELECT_MODES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "e.g., ALL or 1, 12..14 or WALK, HOV2..HOV4", NO_HELP },
		{ SELECT_PURPOSES, "SELECT_PURPOSES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_PRIORITIES, "SELECT_PRIORITIES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "ALL, or 0..4, or LOW, MEDIUM, HIGH, CRITICAL",  NO_HELP },
		{ SELECT_TIME_OF_DAY, "SELECT_TIME_OF_DAY", LEVEL0, OPT_KEY, TIME_KEY, "ALL", "0:00..24:00", NO_HELP },
		{ SELECT_START_TIMES, "SELECT_START_TIMES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ SELECT_END_TIMES, "SELECT_END_TIMES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ SELECT_ORIGINS, "SELECT_ORIGINS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_DESTINATIONS, "SELECT_DESTINATIONS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_TRAVELER_TYPES, "SELECT_TRAVELER_TYPES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },		
		{ SELECT_FACILITY_TYPES, "SELECT_FACILITY_TYPES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "FREEWAY..EXTERNAL", NO_HELP },
		{ SELECT_PARKING_LOTS, "SELECT_PARKING_LOTS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_VC_RATIOS, "SELECT_VC_RATIOS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, >1.0", NO_HELP },
		{ SELECT_TIME_RATIOS, "SELECT_TIME_RATIOS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, >1.0", NO_HELP },
		{ SELECT_TRANSIT_MODES, "SELECT_TRANSIT_MODES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "e.g., ALL or 1, 3..4 or LOCAL, EXPRESS..RAPIDRAIL", NO_HELP },
		{ SELECT_VEHICLE_TYPES, "SELECT_VEHICLE_TYPES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },		
		{ SELECT_PROBLEM_TYPES, "SELECT_PROBLEM_TYPES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_LINKS, "SELECT_LINKS", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_NODES, "SELECT_NODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_STOPS, "SELECT_STOPS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_ROUTES, "SELECT_ROUTES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_SUBAREAS, "SELECT_SUBAREAS", LEVEL0, OPT_KEY, LIST_KEY, "", RANGE_RANGE, NO_HELP },
		{ SELECT_ORIGIN_ZONES, "SELECT_ORIGIN_ZONES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_DESTINATION_ZONES, "SELECT_DESTINATION_ZONES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ PERCENT_TIME_DIFFERENCE, "PERCENT_TIME_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 percent", "0.0..100.0 percent", NO_HELP },
		{ MINIMUM_TIME_DIFFERENCE, "MINIMUM_TIME_DIFFERENCE", LEVEL0, OPT_KEY, TIME_KEY, "1 minutes", "0..120 minutes", NO_HELP },
		{ MAXIMUM_TIME_DIFFERENCE, "MAXIMUM_TIME_DIFFERENCE", LEVEL0, OPT_KEY, TIME_KEY, "60 minutes", "0..1440 minutes", NO_HELP },
		{ PERCENT_COST_DIFFERENCE, "PERCENT_COST_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 percent", "0.0..100.0 percent", NO_HELP },
		{ MINIMUM_COST_DIFFERENCE, "MINIMUM_COST_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "10 impedance", "0..500 impedance", NO_HELP },
		{ MAXIMUM_COST_DIFFERENCE, "MAXIMUM_COST_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "1000 impedance", "0..10000 impedance", NO_HELP },
		{ PERCENT_TRIP_DIFFERENCE, "PERCENT_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 percent", "0.0..100.0 percent", NO_HELP },
		{ MINIMUM_TRIP_DIFFERENCE, "MINIMUM_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, TIME_KEY, "10 minutes", "0..120 minutes", NO_HELP },
		{ MAXIMUM_TRIP_DIFFERENCE, "MAXIMUM_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, TIME_KEY, "60 minutes", "0..1440 minutes", NO_HELP },
		{ SELECTION_PERCENTAGE, "SELECTION_PERCENTAGE", LEVEL0, OPT_KEY, FLOAT_KEY, "100.0 percent", "0.01..100.0 percent", NO_HELP },
		{ MAXIMUM_PERCENT_SELECTED, "MAXIMUM_PERCENT_SELECTED", LEVEL0, OPT_KEY, FLOAT_KEY, "100.0 percent", "0.1..100.0 percent", NO_HELP },
		{ SELECTION_POLYGON, "SELECTION_POLYGON", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETION_FILE, "DELETION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETION_FORMAT, "DELETION_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ DELETE_HOUSEHOLDS, "DELETE_HOUSEHOLDS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_MODES, "DELETE_MODES", LEVEL0, OPT_KEY, LIST_KEY, "NONE", "e.g., ALL or 1, 12..14 or WALK, HOV2..HOV4", NO_HELP },
		{ DELETE_TRAVELER_TYPES, "DELETE_TRAVELER_TYPES", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },	
		END_CONTROL
	};

	if (keys == 0) {
		exe->Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					exe->Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				exe->Error (String ("Select Service Key %d was Not Found") % keys [i]);
			}
		}
	}
}

//---------------------------------------------------------
//	Read_Select_Keys
//---------------------------------------------------------

void Select_Service::Read_Select_Keys (void)
{
	int i, num;
	String key;
	
	exe->Print (2, "Select Service Controls:");
	exe->Print (1);

	//---- select households ----
	
	if (exe->Control_Key_Status (SELECT_HOUSEHOLDS)) {
		key = exe->Get_Control_Text (SELECT_HOUSEHOLDS);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_households = true;
			if (!hhold_range.Add_Ranges (key)) {
				exe->Error ("Adding Household Ranges");
			}
		}
	}

	//---- select modes ----

	select_modes = exe->Mode_Range_Key (SELECT_MODES, select_mode);

	//---- select purposes ----

	if (exe->Control_Key_Status (SELECT_PURPOSES)) {
		key = exe->Get_Control_Text (SELECT_PURPOSES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_purposes = true;
			if (!purpose_range.Add_Ranges (key)) {
				exe->Error ("Adding Purpose Ranges");
			}
		}
	}

	//---- select priorities ----

	if (exe->Control_Key_Status (SELECT_PRIORITIES)) {
		key = exe->Get_Control_Text (SELECT_PRIORITIES);

		if (!key.empty () && !key.Equals ("ALL")) {
			memset (select_priority, false, sizeof (select_priority));
			int low, high;
			String item, low_text, high_text;

			while (!key.empty ()) {
				key.Split (item);
				item.Range (low_text, high_text);

				low = exe->Priority_Code (low_text);
				high = exe->Priority_Code (high_text);

				if (low < 0 || low > CRITICAL) {
					exe->Error (String ("Specified Priority %s is Out of Range (0..%d)") % low_text % CRITICAL);
				} else if (high < 0 || high > CRITICAL) {
					exe->Error (String ("Specified Priority %s is Out of Range (1..%d)") % high_text % CRITICAL);
				} else if (high < low) {
					exe->Error (String ("Priority Range %s is Out of Order") % item);
				} else {
					if (low == high) {
						select_priority [low] = true;

						if (low_text [0] >= '0' && low_text [0] <= '9') {
							exe->Print (0, String (" (%s)") % exe->Priority_Code ((Priority_Type) low));
						}
					} else {
						for (i=low; i <= high; i++) {
							select_priority [i] = true;
						}
						if (low_text [0] >= '0' && low_text [0] <= '9') {
							exe->Print (0, String (" (%s..%s)") % exe->Priority_Code ((Priority_Type) low) % exe->Priority_Code ((Priority_Type) high));
						}
					}
				}
			}
			for (i=0; i <= CRITICAL; i++) {
				if (!select_priority [i]) {
					select_priorities = true;
					break;
				}
			}
		} else {
			memset (select_priority, true, sizeof (select_priority));
			select_priorities = false;
		}	
	} else {
		memset (select_priority, true, sizeof (select_priority));
		select_priorities = false;
	}

	//---- select time of day ----
	
	if (exe->Control_Key_Status (SELECT_TIME_OF_DAY)) {
		key = exe->Get_Control_Text (SELECT_TIME_OF_DAY);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_time_of_day = true;
			if (!time_range.Add_Ranges (key)) {
				exe->Error ("Adding Time of Day Range");
			}
		}
	}

	//---- select start times ----
	
	if (exe->Control_Key_Status (SELECT_START_TIMES)) {
		key = exe->Get_Control_Text (SELECT_START_TIMES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_start_times = true;
			if (!start_range.Add_Ranges (key)) {
				exe->Error ("Adding Start Time Ranges");
			}
		}
	}

	//---- select end times ----
	
	if (exe->Control_Key_Status (SELECT_END_TIMES)) {
		key = exe->Get_Control_Text (SELECT_END_TIMES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_end_times = true;
			if (!end_range.Add_Ranges (key)) {
				exe->Error ("Adding End Time Ranges");
			}
		}
	}

	//---- select origins ----
	
	if (exe->Control_Key_Status (SELECT_ORIGINS)) {
		key = exe->Get_Control_Text (SELECT_ORIGINS);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_origins = true;
			if (!org_range.Add_Ranges (key)) {
				exe->Error ("Adding Origin Ranges");
			}
		}
	}
	
	//---- select destinations ----
	
	if (exe->Control_Key_Status (SELECT_DESTINATIONS)) {
		key = exe->Get_Control_Text (SELECT_DESTINATIONS);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_destinations = true;
			if (!des_range.Add_Ranges (key)) {
				exe->Error ("Adding Destination Ranges");
			}
		}
	}

	//---- select traveler types ----
	
	if (exe->Control_Key_Status (SELECT_TRAVELER_TYPES)) {
		key = exe->Get_Control_Text (SELECT_TRAVELER_TYPES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_travelers = true;
			if (!traveler_range.Add_Ranges (key)) {
				exe->Error ("Adding Traveler Ranges");
			}
		}
	}

	//---- select facility types ----

	select_facilities = exe->Facility_Range_Key (SELECT_FACILITY_TYPES, select_facility);

	//---- select parking lots ----

	if (exe->Control_Key_Status (SELECT_PARKING_LOTS)) {
		key = exe->Get_Control_Text (SELECT_PARKING_LOTS);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_parking = true;
			if (!parking_range.Add_Ranges (key)) {
				exe->Error ("Adding Parking Lot Ranges");
			}
		}
	}

	//---- select vc ratios ----

	if (exe->Control_Key_Status (SELECT_VC_RATIOS)) {
		vc_ratio = exe->Get_Control_Double (SELECT_VC_RATIOS);
		select_vc = (vc_ratio >= 1.0);
	}

	//---- select time ratios ----

	if (exe->Control_Key_Status (SELECT_TIME_RATIOS)) {
		time_ratio = exe->Get_Control_Double (SELECT_TIME_RATIOS);
		select_ratio = (time_ratio >= 1.0);
	}

	//---- select transit modes ----

	select_transit_modes = exe->Transit_Range_Key (SELECT_TRANSIT_MODES, select_transit);

	//---- select vehicle types ----
	
	if (exe->Control_Key_Status (SELECT_VEHICLE_TYPES)) {
		key = exe->Get_Control_Text (SELECT_VEHICLE_TYPES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_vehicles = true;
			if (!vehicle_range.Add_Ranges (key)) {
				exe->Error ("Adding Vehicle Ranges");
			}
		}
	}

	//---- select problem types ----
	
	if (exe->Control_Key_Status (SELECT_PROBLEM_TYPES)) {
		key = exe->Get_Control_Text (SELECT_PROBLEM_TYPES);

		if (!key.empty () && !key.Equals ("ALL")) {
			int low, high;
			String item, low_text, high_text;

			while (!key.empty ()) {
				key.Split (item);
				item.Range (low_text, high_text);

				low = exe->Problem_Code (low_text);
				high = exe->Problem_Code (high_text);

				if (low < 1 || low >= MAX_PROBLEM) {
					exe->Error (String ("Problem Type %s is Out of Range (1..%d)") % low_text % (MAX_PROBLEM - 1));
				}
				if (high < 1 || high >= MAX_PROBLEM) {
					exe->Error (String ("Problem Type %s is Out of Range (1..%d)") % high_text % (MAX_PROBLEM - 1));
				}
				if (high < low) {
					exe->Error (String ("Problem Type Range %s is Out of Order") % item);
				}
				problem_range.Add_Range (low, high);
				select_problems = true;
			}
		}
	}

	//---- select links ----

	if (exe->Control_Key_Status (SELECT_LINKS)) {
		Data_Range range_rec (1, -MAX_INTEGER);
		Data_Range_Itr range_itr;

		num = exe->Highest_Control_Group (SELECT_LINKS, 0);
		if (num > 1) exe->Print (1);

		//---- process each file ----

		for (i=1; i <= num; i++) {
			key = exe->Get_Control_Text (SELECT_LINKS, i);
			if (key.empty () || key.Equals ("ALL")) continue;

			link_ranges.push_back (range_rec);
			range_itr = --link_ranges.end ();

			if (!range_itr->Add_Ranges (key)) {
				exe->Error ("Adding Select Link Ranges");
			}
			select_links = true;
		}
		if (num > 1) exe->Print (1);
	}

	//---- select nodes ----

	if (exe->Control_Key_Status (SELECT_NODES)) {
		Data_Range range_rec;
		Data_Range_Itr range_itr;

		num = exe->Highest_Control_Group (SELECT_NODES, 0);
		if (num > 1) exe->Print (1);

		//---- process each file ----

		for (i=1; i <= num; i++) {
			key = exe->Get_Control_Text (SELECT_NODES, i);
			if (key.empty () || key.Equals ("ALL")) continue;

			node_ranges.push_back (range_rec);
			range_itr = --node_ranges.end ();

			if (!range_itr->Add_Ranges (key)) {
				exe->Error ("Adding Select Node Ranges");
			}
			select_nodes = true;
		}
		if (num > 1) exe->Print (1);
	}

	//---- select stops ----
	
	if (exe->Control_Key_Status (SELECT_STOPS)) {
		key = exe->Get_Control_Text (SELECT_STOPS);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_stops = true;
			if (!stop_range.Add_Ranges (key)) {
				exe->Error ("Adding Transit Stop Ranges");
			}
		}
	}

	//---- select routes ----
	
	if (exe->Control_Key_Status (SELECT_ROUTES)) {
		key = exe->Get_Control_Text (SELECT_ROUTES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_routes = true;
			if (!route_range.Add_Ranges (key)) {
				exe->Error ("Adding Transit Route Ranges");
			}
		}
	}

	//---- get the select subareas ----

	key = exe->Get_Control_String (SELECT_SUBAREAS);

	if (!key.empty ()) {
		key = exe->Get_Control_Text (SELECT_SUBAREAS);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_subareas = true;
			if (!subarea_range.Add_Ranges (key)) {
				exe->Error ("Adding Subarea Ranges");
			}
		}
	}

	//---- select origin zones ----
	
	if (exe->Control_Key_Status (SELECT_ORIGIN_ZONES)) {
		key = exe->Get_Control_Text (SELECT_ORIGIN_ZONES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_org_zones = true;
			if (!org_zone_range.Add_Ranges (key)) {
				exe->Error ("Adding Origin Zone Ranges");
			}
		}
	}

	//---- select destination zones ----
	
	if (exe->Control_Key_Status (SELECT_DESTINATION_ZONES)) {
		key = exe->Get_Control_Text (SELECT_DESTINATION_ZONES);

		if (!key.empty () && !key.Equals ("ALL")) {
			select_des_zones = true;
			if (!des_zone_range.Add_Ranges (key)) {
				exe->Error ("Adding Destination Zone Ranges");
			}
		}
	}

	//---- percent time difference ----

	if (exe->Control_Key_Status (PERCENT_TIME_DIFFERENCE)) {
		percent_time_diff = exe->Get_Control_Double (PERCENT_TIME_DIFFERENCE);
		time_diff_flag = (percent_time_diff > 0.0);
		percent_time_diff /= 100.0;

		//---- minimum time difference ----

		if (exe->Control_Key_Status (MINIMUM_TIME_DIFFERENCE)) {
			min_time_diff = exe->Get_Control_Time (MINIMUM_TIME_DIFFERENCE);
		}

		//---- maximum time difference ----

		if (exe->Control_Key_Status (MAXIMUM_TIME_DIFFERENCE)) {
			max_time_diff = exe->Get_Control_Time (MAXIMUM_TIME_DIFFERENCE);
		}
		max_min_time_diff = max_time_diff - min_time_diff;
		if (max_min_time_diff < 1) max_min_time_diff = 1;
	}

	//---- percent cost difference ----

	if (exe->Control_Key_Status (PERCENT_COST_DIFFERENCE)) {
		percent_cost_diff = exe->Get_Control_Double (PERCENT_COST_DIFFERENCE);
		cost_diff_flag = (percent_cost_diff > 0.0);
		percent_cost_diff /= 100.0;

		//---- minimum cost difference ----

		if (exe->Control_Key_Status (MINIMUM_COST_DIFFERENCE)) {
			min_cost_diff = exe->Get_Control_Integer (MINIMUM_COST_DIFFERENCE);
		}

		//---- maximum cost difference ----

		if (exe->Control_Key_Status (MAXIMUM_COST_DIFFERENCE)) {
			max_cost_diff = exe->Get_Control_Integer (MAXIMUM_COST_DIFFERENCE);
		}
		max_min_cost_diff = max_cost_diff - min_cost_diff;
		if (max_min_cost_diff < 1) max_min_cost_diff = 1;
	}

	//---- percent trip difference ----

	if (exe->Control_Key_Status (PERCENT_TRIP_DIFFERENCE)) {
		percent_trip_diff = exe->Get_Control_Double (PERCENT_TRIP_DIFFERENCE);
		trip_diff_flag = (percent_trip_diff > 0.0);
		percent_trip_diff /= 100.0;

		//---- minimum trip difference ----

		if (exe->Control_Key_Status (MINIMUM_TRIP_DIFFERENCE)) {
			min_trip_diff = exe->Get_Control_Time (MINIMUM_TRIP_DIFFERENCE);
		}

		//---- maximum trip difference ----

		if (exe->Control_Key_Status (MAXIMUM_TRIP_DIFFERENCE)) {
			max_trip_diff = exe->Get_Control_Time (MAXIMUM_TRIP_DIFFERENCE);
		}
		max_min_trip_diff = max_trip_diff - min_trip_diff;
		if (max_min_trip_diff < 1) max_min_trip_diff = 1;
	}

	//---- selection percentage ----

	if (exe->Control_Key_Status (SELECTION_PERCENTAGE)) {
		select_percent = exe->Get_Control_Double (SELECTION_PERCENTAGE);
		percent_flag = (select_percent > 0.0 && select_percent < 100.0);
		select_percent /= 100.0;
	}

	//---- maximum percent selected ----

	if (exe->Control_Key_Status (MAXIMUM_PERCENT_SELECTED)) {
		max_percent_select = exe->Get_Control_Double (MAXIMUM_PERCENT_SELECTED);
		max_percent_flag = (max_percent_select > 0.0 && max_percent_select < 100.0);
		max_percent_select /= 100.0;
	}

	//---- get the selection polygon ----

	key = exe->Get_Control_String (SELECTION_POLYGON);

	if (!key.empty ()) {
		exe->Print (1);
		polygon_file.File_Type ("Selection Polygon");

		polygon_file.Open (exe->Project_Filename (key));
		
		if (!polygon_file.Read_Record ()) {
			exe->Error (String ("Reading %s") % polygon_file.File_Type ());
		}
		select_polygon = true;
	}

	//---- deletion file ----

	if (exe->Control_Key_Status (DELETION_FILE)) {
		key = exe->Get_Control_String (DELETION_FILE);

		if (!key.empty ()) {
			delete_file.File_Type ("Deletion File");
			delete_file.Filename (exe->Project_Filename (key));

			key = exe->Get_Control_String (DELETION_FORMAT);
			if (!key.empty ()) {
				delete_file.Dbase_Format (key);
			}
			delete_file.Open (0);
			delete_flag = true;

			Read_Deletions ();
		}
	}

	//---- delete households ----
	
	if (exe->Control_Key_Status (DELETE_HOUSEHOLDS)) {
		key = exe->Get_Control_Text (DELETE_HOUSEHOLDS);

		if (!key.empty () && !key.Equals ("NONE")) {
			delete_households = true;
			if (!hhold_delete.Add_Ranges (key)) {
				exe->Error ("Adding Household Ranges");
			}
		}
	}
	if (exe->Control_Key_Status (DELETE_MODES)) {
		key = exe->Get_Control_Text (DELETE_MODES);

		if (!key.empty () && !key.Equals ("NONE")) {
			memset (delete_mode, '\0', MAX_MODE * sizeof (bool));
			int low, high;
			String item, low_text, high_text;

			while (!key.empty ()) {
				key.Split (item);
				item.Range (low_text, high_text);

				low = exe->Mode_Code (low_text);
				high = exe->Mode_Code (high_text);

				if (low < 1 || low >= MAX_MODE) {
					exe->Error (String ("Specified Mode %s is Out of Range (1..%d)") % low_text % (MAX_MODE-1));
				} else if (high < 1 || high >= MAX_MODE) {
					exe->Error (String ("Specified Mode %s is Out of Range (1..%d)") % high_text % (MAX_MODE-1));
				} else if (high < low) {
					exe->Error (String ("Mode Range %s is Out of Order") % item);
				} else {
					delete_modes = true;
					if (low == high) {
						delete_mode [low] = true;

						if (low_text [0] >= '0' && low_text [0] <= '9') {
							exe->Print (0, String (" (%s)") % exe->Mode_Code ((Mode_Type) low));
						}
					} else {
						for (i=low; i <= high; i++) {
							delete_mode [i] = true;
						}
						if (low_text [0] >= '0' && low_text [0] <= '9') {
							exe->Print (0, String (" (%s..%s)") % exe->Mode_Code ((Mode_Type) low) % exe->Mode_Code ((Mode_Type) high));
						}
					}
				}
			}
			for (i=1; i < MAX_MODE; i++) {
				if (delete_mode [i]) {
					delete_modes = true;
					break;
				}
			}
		} else {
			for (i=0; i < MAX_MODE; i++) {
				delete_mode [i] = false;
			}
		}	
	} else {
		for (i=0; i < MAX_MODE; i++) {
			delete_mode [i] = false;
		}
	}

	//---- delete traveler types ----
	
	if (exe->Control_Key_Status (DELETE_TRAVELER_TYPES)) {
		key = exe->Get_Control_Text (DELETE_TRAVELER_TYPES);

		if (!key.empty () && !key.Equals ("NONE")) {
			delete_travelers = true;
			if (!traveler_delete.Add_Ranges (key)) {
				exe->Error ("Adding Traveler Type Ranges");
			}
		}
	}
}

//---------------------------------------------------------
//	Select_Plan_Links
//---------------------------------------------------------

bool Select_Service::Select_Plan_Links (Plan_Data &plan)
{
	int num, n1, n2, id;

	Plan_Leg_Itr leg_itr;
	Data_Range_Itr range_itr;

	for (range_itr = link_ranges.begin (); range_itr != link_ranges.end (); range_itr++) {
		num = range_itr->Max_Count ();

		n1 = n2 = 0;

		for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
			if (!leg_itr->Link_Type ()) continue;

			//---- check the link sequence ----

			id = leg_itr->ID ();
			if (leg_itr->Type () == LINK_BA || leg_itr->Type () == USE_BA) id = -id;

			n2 = range_itr->Range_Count (id);
			if (!n2) continue;

			if (n2 != n1 + 1) {
				n2 = 0;
				break;
			}
			if (n2 == num) return (true);
			n1 = n2;
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Select_Plan_Nodes
//---------------------------------------------------------

bool Select_Service::Select_Plan_Nodes (Plan_Data &plan)
{
	int num, n1, n2, id;

	Plan_Leg_Itr leg_itr;
	Data_Range_Itr range_itr;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;

	for (range_itr = node_ranges.begin (); range_itr != node_ranges.end (); range_itr++) {
		num = range_itr->Max_Count ();

		n1 = n2 = 0;

		for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
			if (leg_itr->Type () == NODE_ID) {
				id = leg_itr->ID ();
			} else if (leg_itr->Link_Type ()) {

				id = leg_itr->Link_ID ();
				map_itr = dat->link_map.find (id);
				if (map_itr == dat->link_map.end ()) continue;

				link_ptr = &dat->link_array [map_itr->second];
				if (leg_itr->Link_Dir () == 1) {
					id = link_ptr->Anode ();
				} else {
					id = link_ptr->Bnode ();
				}
				node_ptr = &dat->node_array [id];
				id = node_ptr->Node ();
			} else {
				continue;
			}
			n2 = range_itr->Range_Count (id);
			if (!n2) continue;

			if (n2 != n1 + 1) {
				n2 = 0;
				break;
			}
			if (n2 == num) return (true);
			n1 = n2;
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Select_Plan_Stops
//---------------------------------------------------------

bool Select_Service::Select_Plan_Stops (Plan_Data &plan)
{
	Plan_Leg_Itr leg_itr;
	
	for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
		if (leg_itr->Type () == STOP_ID) {
			if (stop_range.In_Range (leg_itr->ID ())) return (true);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Select_Plan_Routes
//---------------------------------------------------------

bool Select_Service::Select_Plan_Routes (Plan_Data &plan)
{
	Plan_Leg_Itr leg_itr;
	
	for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
		if (leg_itr->Type () == ROUTE_ID) {
			if (route_range.In_Range (leg_itr->ID ())) return (true);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Select_Plan_Subareas
//---------------------------------------------------------

bool Select_Service::Select_Plan_Subareas (Plan_Data &plan)
{
	int id, node, subarea;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Int_Map_Itr map_itr;
	Plan_Leg_Itr leg_itr;
	
	for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
		if (leg_itr->Type () == NODE_ID) {
			id = leg_itr->ID ();
			map_itr = dat->node_map.find (id);
			if (map_itr == dat->node_map.end ()) continue;
			node = map_itr->second;
		} else if (leg_itr->Link_Type ()) {
			id = leg_itr->Link_ID ();
			map_itr = dat->link_map.find (id);
			if (map_itr == dat->link_map.end ()) continue;

			link_ptr = &dat->link_array [map_itr->second];
			if (leg_itr->Link_Dir () == 1) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
		} else {
			continue;
		}
		node_ptr = &dat->node_array [node];
		subarea = node_ptr->Subarea ();

		if (subarea_range.In_Range (subarea)) return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Select_Plan_Polygon
//---------------------------------------------------------

bool Select_Service::Select_Plan_Polygon (Plan_Data &plan)
{
	int index;
	Plan_Leg_Itr leg_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	
	for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
		if (!leg_itr->Link_Type ()) continue;

		index = leg_itr->Link_ID ();
		map_itr = dat->link_map.find (index);

		if (map_itr == dat->link_map.end ()) continue;

		link_ptr = &dat->link_array [map_itr->second];
		node_ptr = &dat->node_array [link_ptr->Anode ()];

		if (In_Polygon (polygon_file, dat->UnRound (node_ptr->X ()), dat->UnRound (node_ptr->Y ()))) return (true);

		node_ptr = &dat->node_array [link_ptr->Bnode ()];

		if (In_Polygon (polygon_file, dat->UnRound (node_ptr->X ()), dat->UnRound (node_ptr->Y ()))) return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Read_Deletions
//---------------------------------------------------------

void Select_Service::Read_Deletions (void)
{
	Trip_Index trip_index;
	Select_Data select_data;

	//---- check the partition number ----

	if (delete_file.Part_Flag () && exe->First_Partition () != delete_file.Part_Number ()) {
		delete_file.Open (0);
	}

	//---- process each partition ----

	for (int part=0; ; part++) {
		if (part > 0) {
			if (!delete_file.Open (part)) break;
		}
		while (delete_file.Read ()) {
			trip_index.Household (delete_file.Household ());
			if (trip_index.Household () < 1) continue;

			trip_index.Person (delete_file.Person ());
			trip_index.Tour (delete_file.Tour ());
			trip_index.Trip (delete_file.Trip ());

			delete_map.insert (Select_Map_Data (trip_index, select_data));
		}
	}
	delete_file.Close ();
}
