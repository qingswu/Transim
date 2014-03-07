//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PNRSplit.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PNRSplit::Program_Control (void)
{
	int i, rows, cols, tabs;
	DATA_TYPE type;

	char label [_MAX_FLABEL];
	String key;

	//---- create the network files ----

	Execution_Service::Program_Control ();
		
	Read_Select_Keys ();

	Print (2, String ("%s Control Keys:") % Program ());

	Write (1, "Initialize the TransCAD Matrix DLL");
	tc_status = TC_OKAY;
	
	InitMatDLL (&tc_status);

	//---- open the trip table ----

	Write (1);

	key = Get_Control_Text (DRIVE_TO_TRANSIT_TRIPS);

	Show_Message (1, "Opening ") << key;
	key = Project_Filename (key);

	pnr_trip = MATRIX_LoadFromFile ((char *) key.c_str (), CONTROL_AUTOMATIC);

	if (tc_status != TC_OKAY || pnr_trip == 0) {
		Error (String ("Error Opening TransCAD Matrix \"%s\"") % key);
	}

	//---- check the matrix data ----

	rows = MATRIX_GetNRows (pnr_trip);
	cols = MATRIX_GetNCols (pnr_trip);
	type = MATRIX_GetDataType (pnr_trip);
	tabs = MATRIX_GetNCores (pnr_trip);

	Write (0, " (") << rows << "x" << cols << "x" << tabs << ")";
		
	if (num_zones == 0) num_zones = rows;

	if (rows != cols) goto match_error;
	if (cols != num_zones) goto zone_error;
	if (type != FLOAT_TYPE) goto type_error;
		
	//---- open the parking node skim ----

	key = Get_Control_Text (PARKING_NODE_SKIM);

	if (key.empty ()) {
		Error ("Parking Node Skim is Missing");
	}
	Show_Message (2, "Opening ") << key;
	key = Project_Filename (key);

	node_skim = MATRIX_LoadFromFile ((char *) key.c_str (), CONTROL_AUTOMATIC);

	if (tc_status != TC_OKAY || node_skim == 0) {
		Error (String ("Error Opening TransCAD Matrix \"%s\"") % key);
	}

	//---- check the matrix data ----

	rows = MATRIX_GetNRows (node_skim);
	cols = MATRIX_GetNCols (node_skim);
	tabs = MATRIX_GetNCores (node_skim);

	Write (0, " (") << rows << "x" << cols << "x" << tabs << ")";

	if (rows != cols) goto match_error;
	if (cols != num_zones) goto zone_error;
	
	//---- open parking zone map ----

	key = Get_Control_String (PARKING_ZONE_MAP_FILE);

	if (key.empty ()) {
		Error ("Parking Zone Map File is Missing");
	}
	parking_zone_file.File_Type ("Parking Zone Map File");

	parking_zone_file.Open (Project_Filename (key));

	//---- create the new trip file ----

	key = Get_Control_Text (NEW_DRIVE_TO_TRANSIT_TRIPS);
		
	if (key.empty ()) {
		Error ("New Drive to Transit Skim Missing");
	}
	new_filename = Project_Filename (key);

	Print (1);

	//---- park-n-ride table name ----

	key = Get_Control_Text (DRIVE_TO_TRANSIT_TABLE);

	tabs = MATRIX_GetNCores (pnr_trip);
	pnr_id = -1;

	for (i=0; i < tabs; i++) {
		MATRIX_GetLabel (pnr_trip, (short) i, label);

		if (key.Equals (label)) {
			pnr_id = i;
			break;
		}
	}
	if (pnr_id < 0) {
		Error (String ("Table %s was Not Found in the Input Trip File") % key);
	}

	//---- parking node table name ----

	key = Get_Control_Text (PARKING_NODE_TABLE);

	tabs = MATRIX_GetNCores (node_skim);
	node_id = -1;

	for (i=0; i < tabs; i++) {
		MATRIX_GetLabel (node_skim, (short) i, label);

		if (key.Equals (label)) {
			node_id = i;
			break;
		}
	}
	if (node_id < 0) {
		Error (String ("Table %s was Not Found in the Parking Node File") % key);
	}

	//---- new table names ----

	walk_table = Get_Control_Text (WALK_TO_TRANSIT_TABLE);
	drive_table = Get_Control_Text (DRIVE_TO_PARKING_TABLE);

	if (walk_table.empty () || drive_table.empty ()) {
		Error ("Output Table Names are Required");
	}
	return;

match_error:
	Error ("Rows and Columns do not Match");
zone_error:
	Error ("Skims include a Different Number of Zones");
type_error:
	Error ("Skim Does Not Include Float Data");
	return;
}

