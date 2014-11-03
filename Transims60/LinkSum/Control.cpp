//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LinkSum::Program_Control (void)
{
	int i, field, ngroup, num;
	bool binary;
	String key, token;

	Location_File *location_file;
	Db_Field *fld;	
	Doubles dbl;
	Doubles_Itr itr;

	//---- set the equivalence flags ----

	Zone_Equiv_Flag (Check_Control_Key (NEW_GROUP_TRAVEL_FILE));

	group_select = Set_Control_Flag (SELECT_BY_LINK_GROUP);
	Link_Equiv_Flag (group_select || Report_Flag (LINK_GROUP) || Report_Flag (TRAVEL_TIME) || 
		Report_Flag (GROUP_REPORT) || Report_Flag (GROUP_SUMMARY) || Check_Control_Key (NEW_GROUP_SUMMARY_FILE));

	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	turn_flag = System_File_Flag (TURN_DELAY);
	
	Print (2, String ("%s Control Keys:") % Program ());	

	//---- open the compare performance file ----
	
	key = Get_Control_String (COMPARE_PERFORMANCE_FILE);

	if (!key.empty ()) {
		compare_file.File_Type ("Compare Performance File");
		Print (1);

		if (Check_Control_Key (COMPARE_PERFORMANCE_FORMAT)) {
			compare_file.Dbase_Format (Get_Control_String (COMPARE_PERFORMANCE_FORMAT));
		}
		compare_file.Open (Project_Filename (key));
		compare_flag = true;
	}
	num_inc = sum_periods.Num_Periods ();
	if (num_inc < 1) num_inc = 1;

	cap_factor = (double) sum_periods.Range_Length () / (Dtime (1, HOURS) * num_inc);
	if (cap_factor <= 0.0) cap_factor = 1.0;

	//---- get minimum volume ----
	
	minimum_volume = Get_Control_Double (MINIMUM_LINK_VOLUME);

	//---- person-based statistics ----

	person_flag = Get_Control_Flag (PERSON_BASED_STATISTICS);

	//---- get the select by link group flag ----

	group_select = Get_Control_Flag (SELECT_BY_LINK_GROUP);
	if (group_select || select_subareas || select_polygon || select_facilities) select_flag = true;

	//---- turning movement data ----

	if (turn_flag) {

		//---- open the compare turn delays file ----
	
		key = Get_Control_String (COMPARE_TURN_DELAY_FILE);

		if (!key.empty ()) {
			turn_compare_file.File_Type ("Compare Turn Delay File");
			Print (1);

			if (Check_Control_Key (COMPARE_TURN_DELAY_FORMAT)) {
				turn_compare_file.Dbase_Format (Get_Control_String (COMPARE_TURN_DELAY_FORMAT));
			}
			turn_compare_file.Open (Project_Filename (key));
			turn_compare_flag = true;
		}

		//---- select turn nodes----
	
		key = exe->Get_Control_Text (TURN_NODE_RANGE);

		if (!key.empty () && !key.Equals ("ALL")) {
			if (!turn_range.Add_Ranges (key)) {
				exe->Error ("Adding Turn Node Ranges");
			}
		}
	}

	//---- create link activity file ----

	key = Get_Control_String (NEW_LINK_ACTIVITY_FILE);

	if (!key.empty ()) {
		if (!System_File_Flag (LOCATION)) {
			Error ("A Location File is needed for the Link Activity File");
		}
		location_file = System_Location_File ();
		binary = (location_file->Record_Format () == BINARY);

		Print (1);
		activity_file.File_Type ("New Link Activity File");

		if (Check_Control_Key (NEW_LINK_ACTIVITY_FORMAT)) {
			activity_file.Dbase_Format (Get_Control_String (NEW_LINK_ACTIVITY_FORMAT));
		}
		activity_file.Create (Project_Filename (key));

		//---- copy location field name ----

		key = Get_Control_Text (COPY_LOCATION_FIELDS);
		if (key.empty ()) {
			Error ("Location Field Names are Required for Link Activity File");
		}
		link_db.File_Type ("Link Location Database");

		//---- set required fields ----

		link_db.Add_Field ("LINK", DB_INTEGER, 10);
		activity_file.Add_Field ("LINK", DB_INTEGER, 10);

		//---- copy selected fields ----

		while (!key.Split (token)) {
			field = location_file->Field_Number (token);
			if (field < 0) {
				Error (String ("Field %s was Not Found in the Location File") % token);
			}
			field_list.push_back (field);

			fld = location_file->Field (field);

			link_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary);
			activity_file.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary);
		}
		activity_file.Write_Header ();
		activity_flag = true;
	}

	//---- create zone travel file ----

	key = Get_Control_String (NEW_ZONE_TRAVEL_FILE);

	if (!key.empty ()) {
		if (!System_File_Flag (LOCATION)) {
			Error ("A Location File is needed for the Zone Travel File");
		}
		Print (1);
		zone_file.File_Type ("New Zone Travel File");

		if (Check_Control_Key (NEW_ZONE_TRAVEL_FORMAT)) {
			zone_file.Dbase_Format (Get_Control_String (NEW_ZONE_TRAVEL_FORMAT));
		}
		zone_file.Create (Project_Filename (key));
		zone_flag = true;
	}

	//---- create zone group travel file ----

	key = Get_Control_String (NEW_GROUP_TRAVEL_FILE);

	if (!key.empty ()) {
		if (!System_File_Flag (LOCATION)) {
			Error ("A Location File is needed for the Group Travel File");
		}
		Print (1);
		group_file.File_Type ("New Group Travel File");

		if (Check_Control_Key (NEW_GROUP_TRAVEL_FORMAT)) {
			group_file.Dbase_Format (Get_Control_String (NEW_GROUP_TRAVEL_FORMAT));
		}
		group_file.Create (Project_Filename (key));
		group_flag = true;
	}

	if (!activity_flag && !zone_flag && !group_flag) {
		System_File_False (LOCATION);
	}

	//---- get the number of direction groups ----

	ngroup = Highest_Control_Group (NEW_LINK_DIRECTION_FILE, 0);

	if (ngroup > 0) {
		Dir_Group group, *group_ptr;

		//---- process each group ----

		for (i=1; i <= ngroup; i++) {
			key = Get_Control_String (NEW_LINK_DIRECTION_FILE, i);
			if (key.empty ()) continue;

			Print (1);
			dir_group.push_back (group);
			group_ptr = &dir_group.back ();

			group_ptr->group = i;
			group_ptr->file = new Link_Direction_File ();
			group_ptr->file->File_Type (String ("New Link Direction File #%d") % i);

			if (Check_Control_Key (NEW_LINK_DIRECTION_FORMAT, i)) {
				group_ptr->file->Dbase_Format (Get_Control_String (NEW_LINK_DIRECTION_FORMAT, i));
			}
			group_ptr->file->Lane_Use_Flows (Lane_Use_Flows ());
			group_ptr->file->Create (Project_Filename (key));

			//---- get the field name ----

			key = Get_Control_Text (NEW_LINK_DIRECTION_FIELD, i);
			if (key.empty ()) {
				Error (String ("New Link Direction Field #%d is Missing") % i);
			}
			group_ptr->field = Performance_Code (key);

			group_ptr->index = Get_Control_Flag (NEW_LINK_DIRECTION_INDEX, i);

			Set_Link_Dir (group_ptr);
		}
	}

	//---- get the number of groups ----

	ngroup = Highest_Control_Group (NEW_LINK_DATA_FILE, 0);

	if (ngroup > 0) {
		Data_Group group, *group_ptr;

		//---- process each group ----

		for (i=1; i <= ngroup; i++) {
			key = Get_Control_String (NEW_LINK_DATA_FILE, i);
			if (key.empty ()) continue;

			Print (1);
			data_group.push_back (group);
			group_ptr = &data_group.back ();

			group_ptr->group = i;
			group_ptr->file = new Link_Data_File ();
			group_ptr->file->File_Type (String ("New Link Data File #%d") % i);

			if (Check_Control_Key (NEW_LINK_DATA_FORMAT, i)) {
				group_ptr->file->Dbase_Format (Get_Control_String (NEW_LINK_DATA_FORMAT, i));
			}
			group_ptr->file->Lane_Use_Flows (Lane_Use_Flows ());
			group_ptr->file->Create (Project_Filename (key));

			//---- get the field name ----

			key = Get_Control_Text (NEW_LINK_DATA_FIELD, i);
			if (key.empty ()) {
				Error (String ("New Link Data Field #%d is Missing") % i);
			}
			group_ptr->field = Performance_Code (key);

			Set_Link_Data (group_ptr);
		}
	}

	//---- data summary file ----

	key = Get_Control_String (NEW_DATA_SUMMARY_FILE);

	if (!key.empty ()) {
		Print (1);
		summary_file.File_Type ("New Data Summary File");

		if (Check_Control_Key (NEW_DATA_SUMMARY_FORMAT)) {
			summary_file.Dbase_Format (Get_Control_String (NEW_DATA_SUMMARY_FORMAT));
		}
		summary_file.Add_Field ("MEASURE", DB_STRING, 40);
		summary_file.Add_Field ("VALUE", DB_DOUBLE, 12.2);
		if (compare_flag) summary_file.Add_Field ("COMPARE", DB_DOUBLE, 12.2);

		summary_file.Create (Project_Filename (key));
		summary_flag = true;

		//---- data summary periods ----

		if (!Control_Key_Empty (NEW_DATA_SUMMARY_PERIODS)) {
			periods_flag = data_periods.Add_Ranges (Get_Control_Text (NEW_DATA_SUMMARY_PERIODS));
		}

		//---- data summary ratios ----

		if (Check_Control_Key (NEW_DATA_SUMMARY_RATIOS)) {
			Double_List list;
			Dbl_Itr itr;

			Get_Control_List (NEW_DATA_SUMMARY_RATIOS, list);

			for (i=0, itr = list.begin (); itr != list.end (); itr++, i++) {
				if (i > 0 && *itr >= 1.0) {
					data_ratios.push_back (Round (*itr * 100.0));
					ratios_flag = true;
				}
			}
		}
	}

	//---- group summary file ----

	key = Get_Control_String (NEW_GROUP_SUMMARY_FILE);

	if (!key.empty ()) {
		Print (1);
		group_sum_file.File_Type ("New Group Summary File");

		if (Check_Control_Key (NEW_GROUP_SUMMARY_FORMAT)) {
			group_sum_file.Dbase_Format (Get_Control_String (NEW_GROUP_SUMMARY_FORMAT));
		}
		group_sum_file.Add_Field ("MEASURE", DB_STRING, 40);
		group_sum_file.Add_Field ("VALUE", DB_DOUBLE, 12.2);
		if (compare_flag) group_sum_file.Add_Field ("COMPARE", DB_DOUBLE, 12.2);

		group_sum_file.Create (Project_Filename (key));
		group_sum_flag = true;
	}

	//---- read report types ----

	List_Reports ();

	if (!compare_flag && (Report_Flag (TIME_CHANGE) || Report_Flag (VOLUME_CHANGE) ||
		Report_Flag (TOP_TIME_CHANGE) || Report_Flag (TOP_VOL_CHANGE) || Report_Flag (RELATIVE_GAP))) {

		Error ("A Compare Performance File is Required for Change Reports");
	}

	//---- process support data ----

	if (Link_Equiv_Flag ()) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}

	if (group_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}

	//---- allocate work space ----

	if (periods_flag || ratios_flag) {
		num = (periods_flag) ? (int) data_periods.size () : 1;
		num += (ratios_flag) ? (int) data_ratios.size () : 1;
		if (num_inc + 1 > num) {
			num = num_inc + 1;
		}
	} else {
		num = num_inc + 1;
	}
	sum_bin.assign (num, dbl);

	for (itr = sum_bin.begin (); itr != sum_bin.end (); itr++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}
} 
