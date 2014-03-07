//********************************************************* 
//	Link_Use_File.cpp - Link Use File Input/Output
//*********************************************************

#include "Link_Use_File.hpp"

//-----------------------------------------------------------
//	Link_Use_File constructors
//-----------------------------------------------------------

Link_Use_File::Link_Use_File (Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();
}

Link_Use_File::Link_Use_File (string filename, Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

Link_Use_File::Link_Use_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Link_Use_File::Link_Use_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Use_File::Setup (void)
{
	File_Type ("Link Use File");
	File_ID ("LinkUse");

	anode = bnode = link = dir = -1;
	num_values = NUM_VALUES;

	for (int i=0; i < num_values; i++) {
		type [i] = use [i] = period [i] = lanes [i] = toll [i] = speed [i] = -1;
	}
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_Use_File::Create_Fields (void) 
{
	Add_Field ("ANODE", DB_INTEGER, 10);
	Add_Field ("BNODE", DB_INTEGER, 10);
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();

	for (int i=0; i < num_values; i++) {
		if (Code_Flag ()) {
			Add_Field ((String ("TYPE%d") % i), DB_UNSIGNED, 1, RESTRICT_CODE, true);
			Add_Field ((String ("USE%d") % i), DB_UNSIGNED, 2, USE_CODE, true);
			Add_Field ((String ("LANES%d") % i), DB_UNSIGNED, 2, LANE_RANGE_CODE, true);
		} else {
			Add_Field ((String ("TYPE%d") % i), DB_STRING, 10, RESTRICT_CODE);
			Add_Field ((String ("USE%d") % i), DB_STRING, STRING_FIELD_SIZE, USE_CODE);
			Add_Field ((String ("LANES%d") % i), DB_STRING, 8, LANE_RANGE_CODE);
		}
		Add_Field ((String ("PERIOD%d") % i), DB_STRING, STRING_FIELD_SIZE);
		Add_Field ((String ("TOLL%d") % i), DB_UNSIGNED, 2, CENTS, true);
		Add_Field ((String ("SPEED%d") % i), DB_DOUBLE, 5.1, MPH);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Use_File::Set_Field_Numbers (void)
{
	int i;

	//---- required fields ----

	link = Required_Field ("LINK");

	if (link < 0) return (false);

	anode = Optional_Field (ANODE_FIELD_NAMES);
	bnode = Optional_Field (BNODE_FIELD_NAMES);
	
	dir = LinkDir_Type_Field ();

	for (i=0; i < NUM_VALUES; i++) {
		type [i] = Optional_Field ((String ("TYPE%d") % i).c_str (), (String ("RESTRICT%d") % i).c_str ());
		use [i] = Optional_Field ((String ("USE%d") % i).c_str (), (String ("USE_TYPE%d") % i).c_str (), (String ("VEHICLE%d") % i).c_str ());
		lanes [i] = Optional_Field ((String ("LANES%d") % i).c_str (), (String ("USE_LANES%d") % i).c_str (), (String ("LANE%d") % i).c_str ());
		period [i] = Optional_Field ((String ("PERIOD%d") % i).c_str (), (String ("USE_PERIOD%d") % i).c_str (), (String ("USE_TIME%d") % 1).c_str ());
		toll [i] = Optional_Field ((String ("TOLL%d") % i).c_str (), (String ("COST%d") % i).c_str (), (String ("CENTS%d") % i).c_str ());
		speed [i] = Optional_Field ((String ("SPEED%d") % i).c_str (), (String ("SPEEDLMT%d") % i).c_str (), (String ("SPD%d") % i).c_str (), (String ("MAXSPD%d") % i).c_str (), (String ("FSPD%d") % i).c_str ());

		if (use [i] == -1) {
			num_values = i;
			break;
		}
	}

	Notes_Field (Optional_Field (NOTES_FIELD_NAMES));

	//---- set default units ----

	for (i=0; i < num_values; i++) {
		Set_Units (type [i], RESTRICT_CODE);
		Set_Units (use [i], USE_CODE);
		if (Version () > 40) Set_Units (lanes [i], LANE_RANGE_CODE);
		Set_Units (toll [i], CENTS);
		if (Version () <= 40) {
			Set_Units (speed [i], MPS);
		} else {
			Set_Units (speed [i], MPH);
		}
	}
	return (true);
}
