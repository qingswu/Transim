//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Fratar.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Fratar::Program_Control (void)
{
	int i, num;
	String key;
	Margin_Group margin_data, *margin_ptr;

	margin_data.base = 0;
	margin_data.file = 0;
	margin_data.map_file = 0;
	margin_data.map_flag = false;
	margin_data.base_flag = false;

	//---- create the network files ----

	Execution_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- trip table file ----

	key = Get_Control_String (TRIP_TABLE_FILE);

	if (!key.empty ()) {
		in_flag = true;
		
		in_file.File_Type ("Trip Table File");

		if (Check_Control_Key (TRIP_TABLE_FORMAT)) {
			in_file.Format_Code (Get_Control_String (TRIP_TABLE_FORMAT));
		}
		in_file.Open (Project_Filename (key));

		period_flag = (in_file.Num_Periods () > 1);
	}
	
	//---- new trip table file ----

	key = Get_Control_String (NEW_TRIP_TABLE_FILE);
	new_file.File_Type ("New Trip Table File");

	if (Check_Control_Key (NEW_TRIP_TABLE_FORMAT)) {
		new_file.Format_Code (Get_Control_String (NEW_TRIP_TABLE_FORMAT));
	}
	new_file.Range_Flag (true);
	new_file.Create (Project_Filename (key));

	//---- process marginal keys ----

	num = Highest_Control_Group (TRIP_MARGIN_FILE, 0);

	//---- open each margin file ----

	for (i=1; i <= num; i++) {

		if (!Check_Control_Key (TRIP_MARGIN_FILE, i)) continue;
		Print (1);

		margin_data.group = i;
		margin_group.push_back (margin_data);

		margin_ptr = &margin_group.back ();

		margin_ptr->file = new Db_Header ();

		//---- open the base margin file ----

		key = Get_Control_String (BASE_MARGIN_FILE, i);

		if (!key.empty ()) {

			if (num > 1) {
				margin_ptr->base->File_Type (String ("Base Margin File #%d") % i);
			} else {
				margin_ptr->base->File_Type ("Base Margin File");
			}
			if (Check_Control_Key (BASE_MARGIN_FORMAT, i)) {
				margin_ptr->base->Format_Code (Get_Control_String (BASE_MARGIN_FORMAT, i));
			}
			margin_ptr->base->Open (Project_Filename (key));
			margin_ptr->base_flag = true;
		}

		//---- open trip margin file ----

		key = Project_Filename (Get_Control_String (TRIP_MARGIN_FILE, i));

		if (num > 1) {
			margin_ptr->file->File_Type (String ("Trip Margin File #%d") % i);
		} else {
			margin_ptr->file->File_Type ("Trip Margin File");
		}
		if (Check_Control_Key (TRIP_MARGIN_FORMAT, i)) {
			margin_ptr->file->Format_Code (Get_Control_String (TRIP_MARGIN_FORMAT, i));
		}
		margin_ptr->file->Open (key);

		key = Get_Control_Text (MARGIN_TYPE_FIELD, i);

		margin_ptr->type_field = margin_ptr->file->Required_Field (key);

		Print (0, ", Number = ") << (margin_ptr->type_field + 1);

		key = Get_Control_Text (MARGIN_TRIP_FIELD, i);

		margin_ptr->trip_field = margin_ptr->file->Required_Field (key);

		Print (0, ", Number = ") << (margin_ptr->trip_field + 1);

		key = Get_Control_String (MARGIN_TYPE_MAP_FILE, i);

		if (!key.empty ()) {
			margin_ptr->map_file = new Db_Header ();

			if (num > 1) {
				margin_ptr->map_file->File_Type (String ("Margin Type Map File #d") % i);
			} else {
				margin_ptr->map_file->File_Type ("Margin Type Map File");
			}
			if (Check_Control_Key (MARGIN_TYPE_MAP_FORMAT, i)) {
				margin_ptr->map_file->Format_Code (Get_Control_String (MARGIN_TYPE_MAP_FORMAT, i));
			}
			margin_ptr->map_file->Open (key);
			margin_ptr->map_flag = true;
		}
	}

	//---- maximum iterations ----

	Print (1);
	max_iterations = Get_Control_Integer (MAXIMUM_ITERATIONS);

	//---- percent trip difference ----

	percent_diff = Get_Control_Double (PERCENT_TRIP_DIFFERENCE) / 100.0;

	//---- minimum trip difference ----

	min_diff = Get_Control_Double (MINIMUM_TRIP_DIFFERENCE);

	//---- maximum trip difference ----

	max_diff = Get_Control_Double (MAXIMUM_TRIP_DIFFERENCE);
}

