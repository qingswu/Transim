//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TcadImport.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TcadImport::Program_Control (void)
{
	int i, num, field, len;
	char **tab_names;

	Integers num_list;
	Field_Ptr fld_ptr;

	char label [_MAX_FLABEL];
	String key;

	//---- initialize output ----

	Execution_Service::Program_Control ();

	Write (1, "Initialize the TransCAD Matrix DLL");
	tc_status = TC_OKAY;
	
	InitMatDLL (&tc_status);

	Print (2, String ("%s Control Keys:") % Program ());

	//---- highest zone number ----

	num_zones = Get_Control_Integer (HIGHEST_ZONE_NUMBER);

	//---- open the input file ----

	Print (1);
	key = Get_Control_String (INPUT_MATRIX_FILE);

	input_file.File_Type ("Input Matrix File");

	if (Check_Control_Key (INPUT_MATRIX_FORMAT)) {
		input_file.Dbase_Format (Get_Control_String (INPUT_MATRIX_FORMAT));
	}
	input_file.Open (Project_Filename (key));

	//---- origin field name ----

	if (Check_Control_Key (INPUT_ORIGIN_FIELD)) {
		key = Get_Control_Text (INPUT_ORIGIN_FIELD);

		org_field = input_file.Required_Field (key);
		Print (0, String (" (Number = %d)") % (org_field + 1));
	} else {
		org_field = input_file.Required_Field (ORIGIN_FIELD_NAMES);
	}

	//---- destination field name ----

	if (Check_Control_Key (INPUT_DESTINATION_FIELD)) {
		key = Get_Control_Text (INPUT_DESTINATION_FIELD);

		des_field = input_file.Required_Field (key);
		Print (0, String (" (Number = %d)") % (des_field + 1));
	} else {
		des_field = input_file.Required_Field (DESTINATION_FIELD_NAMES);
	}

	//---- get input table names ----

	num = Highest_Control_Group (INPUT_TABLE_FIELDS, 0);

	if (num > 0) {
		num_list.assign (num + 1, -1);

		for (i=1; i <= num; i++) {
			key = Get_Control_Text (INPUT_TABLE_FIELDS, i);
			if (key.empty ()) continue;

			field = input_file.Required_Field (key);
			table_fields.push_back (field);
			num_list [i] = field;

			Print (0, String (" (Number = %d)") % (field + 1));
		}
	} else {
		num_list.assign (input_file.Num_Fields () - 1, -1);

		for (i=0, tables=1; i < input_file.Num_Fields (); i++) {
			if (i == org_field || i == des_field) continue;
			fld_ptr = input_file.Field (i);
			table_fields.push_back (i);
			num_list [tables++] = i;
		}
	}

	//---- get the transcad matrix name ----

	Print (1);
	key = Get_Control_String (NEW_TRANSCAD_MATRIX);

	new_filename = Project_Filename (key);

	new_label = Get_Control_Text (NEW_MATRIX_LABEL);

	if (new_label.empty ()) {
		new_label = input_file.Filename ();
	}
	if (new_label.length () >= sizeof (label)) {
		new_label.erase (sizeof (label)-1);
	}
	strcpy_s (label, sizeof (label), new_label.c_str ());

	//---- get new table names ----

	num = Highest_Control_Group (NEW_TABLE_NAMES, 0);
	if (num > (int) num_list.size ()) num = (int) num_list.size ();

	if (num > 0) {
		for (i=1; i <= num; i++) {
			key = Get_Control_Text (NEW_TABLE_NAMES, i);
			if (key.empty ()) {
				if (num_list [i] >= 0) {
					Warning (String ("Input Field #%d was Not Mapped to a Table Name") % i);
					fld_ptr = input_file.Field (table_fields [num_list [i]]);
					table_names.push_back (fld_ptr->Name ());
				}
			} else if (num_list [i] < 0) {
				Warning (String ("Table Name #%d is Not Mapped to an Input Field") % i);
			} else {
				table_names.push_back (key);
			}
		}
	} else {
		num = (int) table_fields.size ();

		for (i=0; i < num; i++) {
			fld_ptr = input_file.Field (table_fields [i]);
			table_names.push_back (fld_ptr->Name ());
		}
	}

	//---- create the new matrix ----
	
	Show_Message ("Initializing Output Trip Tables");

	tables = (int) table_fields.size ();

	tab_names = new char * [tables];

	for (i=0; i < tables; i++) {
		key = table_names [i];
		len = (int) key.size () + 1;
		tab_names [i] = new char [len];
		strcpy_s (tab_names [i], len, key.c_str ());
	}
	new_trip = MATRIX_New ((char *) new_filename.c_str (), label, num_zones, 0, 
		num_zones, 0, tables, tab_names, FLOAT_TYPE, 0);
	
	for (i=0; i < tables; i++) {
		delete [] tab_names [i];
	}
}
