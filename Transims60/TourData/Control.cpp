//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TourData.hpp"

#include "TDF_Matrix.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TourData::Program_Control (void)
{
	int i, num;
	String key, name;
	Strings strings;
	Str_Itr str_itr;
	Format_Type format;
	Table_Group table_group, *table_ptr;
	Table_Itr table_itr;

	//---- open network files ----

	Data_Service::Program_Control ();

	random_select.Seed (random.Seed () + 1);
	random_point.Seed (random.Seed () + 2);

	Print (2, String ("%s Control Keys:") % Program ());	

	zone_file = (Zone_File *) System_File_Handle (ZONE);

	//---- zone group field ----

	if (Check_Control_Key (ZONE_GROUP_FIELD)) {
		key = Get_Control_Text (ZONE_GROUP_FIELD);

		if (!key.empty ()) {
			group_flag = true;
			group_field = zone_file->Field_Number (key);

			if (group_field < 0) {
				Error (String ("Zone Group Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (group_field + 1);
		}
	}

	//---- open the tour file ----

	key = Get_Control_String (TOUR_FILE);

	Print (1);
	tour_file.File_Type ("Tour File");
	tour_file.File_ID ("Tour");

	if (Check_Control_Key (TOUR_FORMAT)) {
		tour_file.Dbase_Format (Get_Control_String (TOUR_FORMAT));
	}
	tour_file.Open (Project_Filename (key));

	//---- open the compare tour file ----

	key = Get_Control_String (COMPARE_TOUR_FILE);

	if (!key.empty ()) {
		Print (1);
		compare_file.File_Type ("Compare Tour File");
		compare_file.File_ID ("CompareTour");

		if (Check_Control_Key (COMPARE_TOUR_FORMAT)) {
			compare_file.Dbase_Format (Get_Control_String (COMPARE_TOUR_FORMAT));
		}
		compare_file.Open (Project_Filename (key));
		compare_flag = true;
	}

	//---- open the new tour file ----

	key = Get_Control_String (NEW_TOUR_FILE);

	Print (1);
	new_file.File_Type ("New Tour File");
	new_file.File_ID ("NewTour");

	if (Check_Control_Key (NEW_TOUR_FORMAT)) {
		new_file.Dbase_Format (Get_Control_String (NEW_TOUR_FORMAT));
	}
	new_file.Replicate_Fields (&tour_file);

	new_file.Create (Project_Filename (key));

	Print (1);

	//---- tour purpose field ----

	key = Get_Control_Text (TOUR_PURPOSE_FIELD);

	purpose_field = tour_file.Field_Number (key);

	if (purpose_field < 0) {
		Error (String ("Tour Purpose Field %s was Not Found") % key);
	}
	Print (0, ", Number = ") << (purpose_field + 1);
	
	//---- tour type field ----

	key = Get_Control_Text (TOUR_TYPE_FIELD);

	type_field = tour_file.Field_Number (key);

	if (type_field < 0) {
		Error (String ("Tour Type Field %s was Not Found") % key);
	}
	Print (0, ", Number = ") << (type_field + 1);

	//---- tour income field ----

	if (Check_Control_Key (TOUR_INCOME_FIELD)) {
		key = Get_Control_Text (TOUR_INCOME_FIELD);

		if (!key.empty ()) {
			income_flag = true;
			income_field = tour_file.Field_Number (key);

			if (income_field < 0) {
				Error (String ("Tour Income Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (income_field + 1);
		}
	}

	//---- tour origin field ----

	key = Get_Control_Text (TOUR_ORIGIN_FIELD);

	org_field = tour_file.Field_Number (key);

	if (org_field < 0) {
		Error (String ("Tour Origin Field %s was Not Found") % key);
	}
	Print (0, ", Number = ") << (org_field + 1);

	//---- tour destination field ----

	key = Get_Control_Text (TOUR_DESTINATION_FIELD);

	des_field = tour_file.Field_Number (key);

	if (des_field < 0) {
		Error (String ("Tour Destination Field %s was Not Found") % key);
	}
	Print (0, ", Number = ") << (des_field + 1);

	//---- tour point field ----

	key = Get_Control_Text (TOUR_POINT_FIELD);

	point_field = tour_file.Field_Number (key);

	if (point_field < 0) {
		Error (String ("Tour Point Field %s was Not Found") % key);
	}
	Print (0, ", Number = ") << (point_field + 1);
	Print (1);

	//---- open the target matrix file ----

	key = Project_Filename (Get_Control_String (TARGET_TOUR_MATRIX));

	format = Db_Header::Def_Format (key);

	if (format == UNFORMATED) {
		if (Check_Control_Key (TARGET_TOUR_FORMAT)) {
			name = Get_Control_String (TARGET_TOUR_FORMAT);
		} else {
			name = Get_Default_Text (TARGET_TOUR_FORMAT);
		}
		format = Format_Code (name);
	}
	target_matrix = TDF_Matrix (format);

	target_matrix->File_Type ("Target Tour Matrix");
	target_matrix->File_ID ("Target");
	target_matrix->Dbase_Format (format);

	target_matrix->Open (key);

	if (target_matrix->Num_Periods () > 1) {
		Print (0, " (Periods=") << target_matrix->Num_Periods () << " Zones=" << target_matrix->Num_Des () << " Tables=" << target_matrix->Tables () << ")";
	} else {
		Print (0, " (Zones=") << target_matrix->Num_Des () << " Tables=" << target_matrix->Tables () << ")";
	}

	//---- open the base matrix file ----

	Print (1);
	key = Project_Filename (Get_Control_String (BASE_TOUR_MATRIX));

	format = Db_Header::Def_Format (key);

	if (format == UNFORMATED) {
		if (Check_Control_Key (BASE_TOUR_FORMAT)) {
			name = Get_Control_String (BASE_TOUR_FORMAT);
		} else {
			name = Get_Default_Text (BASE_TOUR_FORMAT);
		}
		format = Format_Code (name);
	}
	base_matrix = TDF_Matrix (format);

	base_matrix->File_Type ("Base Tour Matrix");
	base_matrix->File_ID ("Base");
	base_matrix->Dbase_Format (format);

	base_matrix->Open (key);

	if (base_matrix->Num_Periods () > 1) {
		Print (0, " (Periods=") << base_matrix->Num_Periods () << " Zones=" << base_matrix->Num_Des () << " Tables=" << base_matrix->Tables () << ")";
	} else {
		Print (0, " (Zones=") << base_matrix->Num_Des () << " Tables=" << base_matrix->Tables () << ")";
	}

	//---- open the new result matrix file ----

	key = Get_Control_String (NEW_RESULT_MATRIX);

	if (!key.empty ()) {
		Print (1);

		if (Check_Control_Key (NEW_RESULT_FORMAT)) {
			format = Format_Code (Get_Control_String (NEW_RESULT_FORMAT));
		} else {
			format = Format_Code (Get_Default_Text (NEW_RESULT_FORMAT));
		}
		result_matrix = TDF_Matrix (format);

		result_matrix->File_Type ("New Result Matrix");
		result_matrix->File_ID ("Result");
		result_matrix->Dbase_Format (format);

		result_matrix->Filename (Project_Filename (key));

		//---- add fields ----

		result_matrix->Clear_Fields ();

		result_matrix->Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
		result_matrix->Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);

		num = Highest_Control_Group (TABLE_BASE_FIELD, 0); 

		for (i=1; i <= num; i++) {
			key = Get_Control_String (TABLE_BASE_FIELD, i);
			result_matrix->Add_Field (key, DB_DOUBLE, 12.2);
		}

		//---- create matrix ----

		result_matrix->Copy_OD_Map (base_matrix);

		if (result_matrix->Num_Org () == 0) {
			result_matrix->Num_Org (base_matrix->Num_Org ());
		}
		if (result_matrix->Num_Des () == 0) {
			result_matrix->Num_Des (base_matrix->Num_Des ());
		}
		result_matrix->Create ();
		result_flag = true;

		if (result_matrix->Num_Periods () > 1) {
			Print (0, " (Periods=") << result_matrix->Num_Periods () << " Zones=" << result_matrix->Num_Des () << " Tables=" << result_matrix->Tables () << ")";
		} else {
			Print (0, " (Zones=") << result_matrix->Num_Des () << " Tables=" << result_matrix->Tables () << ")";
		}
		if (!result_matrix->Allocate_Data (true)) {
			Error ("Insufficient Memory for New Result Matrix");
		}
	}

	//---- get the table ranges ----

	num = Highest_Control_Group (TABLE_PURPOSE_RANGE, 0); 

	for (i=1; i <= num; i++) {
		Print (1);

		key = Get_Control_Text (TABLE_PURPOSE_RANGE, i);
		if (key.empty ()) continue;

		table_group.group = i;

		table_groups.push_back (table_group);
		table_ptr = &table_groups.back ();

		table_ptr->purpose.Add_Ranges (key);

		key = Get_Control_Text (TABLE_TYPE_RANGE, i);
		if (!key.empty ()) {
			table_ptr->type.Add_Ranges (key);
		}

		key = Get_Control_Text (TABLE_INCOME_RANGE, i);
		if (!key.empty ()) {
			table_ptr->income.Add_Ranges (key);
		}

		//---- table target field ----

		key = Get_Control_Text (TABLE_TARGET_FIELD, i);

		table_ptr->target = target_matrix->Table_Number (key);

		if (table_ptr->target < 0) {
			Error (String ("Table Target Field %s was Not Found") % key);
		}
		Print (0, ", Number = ") << (table_ptr->target + 1);

		//---- table base field ----

		key = Get_Control_Text (TABLE_BASE_FIELD, i);

		table_ptr->base = base_matrix->Table_Number (key);

		if (table_ptr->base < 0) {
			Error (String ("Table Base Field %s was Not Found") % key);
		}
		Print (0, ", Number = ") << (table_ptr->base + 1);

		//---- special attractions ----

		key = Get_Control_Text (SPECIAL_ATTRACTIONS, i);

		if (!key.empty ()) {
			table_ptr->special = zone_file->Field_Number (key);

			if (table_ptr->special < 0) {
				Error (String ("Special Attractions Field %s was Not Found in the Zone File") % key);
			}
			Print (0, ", Number = ") << (table_ptr->special + 1);
			attr_flag = true;
		} else {
			table_ptr->special = -1;
		}
	}

	//---- minimum absolute difference ----

	min_difference = Get_Control_Double (MIN_ABSOLUTE_DIFFERENCE);

	//---- mininum percent difference ----

	min_percent_diff = Get_Control_Double (MIN_PERCENT_DIFFERENCE) / 100.0;

	//---- distance factor ----

	distance_factor = Scale (Get_Control_Double (DISTANCE_FACTOR));

	//---- attraction factor ----

	attraction_factor = Get_Control_Double (ATTRACTION_FACTOR);

	//---- new attraction file ----

	key = Get_Control_String (NEW_ATTRACTION_FILE);

	if (!key.empty ()) {
		attr_file_flag = true;
		Print (1);
		attr_file.File_Type ("New Attraction File");

		if (Check_Control_Key (NEW_ATTRACTION_FORMAT)) {
			attr_file.Dbase_Format (Get_Control_String (NEW_ATTRACTION_FORMAT));
		}
		attr_file.Clear_Fields ();
		attr_file.Add_Field ("ZONE", DB_INTEGER, 10);
	
		for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
			name = String ("OLD_%d") % table_itr->group;
			attr_file.Add_Field (name, DB_INTEGER, 10);

			name = String ("NEW_%d") % table_itr->group;
			attr_file.Add_Field (name, DB_INTEGER, 10);
		}
		attr_file.Create (Project_Filename (key));
		Print (1);
	}

	//---- read report types ----

	List_Reports ();

	stats_flag = Report_Flag (GROUP_STATS);
}
