//********************************************************* 
//	Approach_Link_File.cpp - Approach_Link File Input/Output
//*********************************************************

#include "Approach_Link_File.hpp"

//-----------------------------------------------------------
//	Approach_Link_File constructors
//-----------------------------------------------------------

Approach_Link_File::Approach_Link_File (Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();
}

Approach_Link_File::Approach_Link_File (string filename, Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

Approach_Link_File::Approach_Link_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Approach_Link_File::Approach_Link_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Approach_Link_File::Setup (void)
{
	File_Type ("Approach Link File");
	File_ID ("Approach");

	link = dir = control = group = -1;
	left_merge = left = left_thru = thru = right_thru = right = right_merge = -1;
	lm_length = lt_length = rt_length = rm_length = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Approach_Link_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	if (Code_Flag ()) {
		Add_Field ("CONTROL", DB_INTEGER, 1, CONTROL_CODE, true);
	} else {
		Add_Field ("CONTROL", DB_STRING, 12, CONTROL_CODE);
	}
	Add_Field ("GROUP", DB_INTEGER, 2);
	Add_Field ("LM_LENGTH", DB_DOUBLE, 6.1, FEET);
	Add_Field ("L_MERGE", DB_INTEGER, 2);
	Add_Field ("LT_LENGTH", DB_DOUBLE, 6.1, FEET);
	Add_Field ("LEFT", DB_INTEGER, 2);
	Add_Field ("LEFT_THRU", DB_INTEGER, 2);
	Add_Field ("THRU", DB_INTEGER, 2);
	Add_Field ("RIGHT_THRU", DB_INTEGER, 2);
	Add_Field ("RIGHT", DB_INTEGER, 2);
	Add_Field ("RT_LENGTH", DB_DOUBLE, 6.1, FEET);
	Add_Field ("R_MERGE", DB_INTEGER, 2);
	Add_Field ("RM_LENGTH", DB_DOUBLE, 6.1, FEET);

	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Approach_Link_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");

	if (link < 0) return (false);
	
	dir = LinkDir_Type_Field ();
	control = Optional_Field ("CONTROL", "TYPE", "INT_TYPE");
	group = Optional_Field ("GROUP", "TYPE");
	lm_length = Optional_Field ("LM_LENGTH", "LM_LEN");
	left_merge = Optional_Field ("L_MERGE", "LEFT_MERGE", "LT_MERGE");
	lt_length = Optional_Field ("LT_LENGTH", "LT_LEN", "LEFT_LEN");
	left = Optional_Field ("LEFT", "LEFT_TURN", "L");
	left_thru = Optional_Field ("LEFT_THRU", "SHARED_LEFT", "LT");
	thru = Optional_Field ("THRU", "T");
	right_thru = Optional_Field ("RIGHT_THRU", "SHARED_RIGHT", "RT");
	right = Optional_Field ("RIGHT", "R");
	rt_length = Optional_Field ("RT_LENGTH", "RT_LEN", "RIGHT_LEN");
	right_merge = Optional_Field ("R_MERGE", "RIGHT_MERGE", "RT_MERGE");
	rm_length = Optional_Field ("RM_LENGTH", "RM_LEN");

	Notes_Field (Optional_Field (NOTES_FIELD_NAMES));

	//---- set default units ----
		
	if (Version () <= 40) {
		Set_Units (control, CONTROL_CODE);
		Set_Units (lm_length, METERS);
		Set_Units (lt_length, METERS);
		Set_Units (rt_length, METERS);
		Set_Units (rm_length, METERS);
	} else {
		Set_Units (control, CONTROL_CODE);
		Set_Units (lm_length, FEET);
		Set_Units (lt_length, FEET);
		Set_Units (rt_length, FEET);
		Set_Units (rm_length, FEET);
	}
	return (true);
}
