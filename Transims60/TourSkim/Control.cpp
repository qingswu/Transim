//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TourSkim.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TourSkim::Program_Control (void)
{
	int i, j, k, num, max_pnr, period, rows, cols, tabs;
	DATA_TYPE type;

	String key, record;
	Return_Data return_data;
	File_Group group;
	Int_Set periods;
	Int_Set_Itr period_itr;
	Int_Itr pnr_itr, walk_itr;

	//---- create the network files ----

	Execution_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- initialize the file list ----

	num = Highest_Control_Group (DRIVE_TO_TRANSIT_SKIM, 0);

	if (num == 0) {
		Error ("No Drive to Transit Skim Keys were Found");
	}
	Write (1, "Initialize the TransCAD Matrix DLL");
	tc_status = TC_OKAY;
	
	InitMatDLL (&tc_status);

	//---- open each file ----

	Write (1);

	for (i=1; i <= num; i++) {

		if (!Check_Control_Key (DRIVE_TO_TRANSIT_SKIM, i)) continue;

		//---- open the drive to transit skim ----

		key = Get_Control_Text (DRIVE_TO_TRANSIT_SKIM, i);

		if (key.empty ()) continue;

		group.group = i;

		Show_Message (1, "Open ") << key;
		key = Project_Filename (key);

		group.pnr_skim = MATRIX_LoadFromFile ((char *) key.c_str (), CONTROL_AUTOMATIC);

		if (tc_status != TC_OKAY || group.pnr_skim == 0) {
			Error (String ("Error Opening TransCAD Matrix \"%s\"") % key);
		}

		//---- check the matrix data ----

		rows = MATRIX_GetNRows (group.pnr_skim);
		cols = MATRIX_GetNCols (group.pnr_skim);
		type = MATRIX_GetDataType (group.pnr_skim);
		tabs = MATRIX_GetNCores (group.pnr_skim);

		Show_Message (0, " (") << rows << "x" << cols << "x" << tabs << ")";
		
		if (num_zones == 0) num_zones = rows;

		if (rows != cols) goto match_error;
		if (cols != num_zones) goto zone_error;
		if (type != FLOAT_TYPE) goto type_error;
		
		//---- open the parking node skim ----

		key = Get_Control_Text (PARKING_NODE_SKIM, i);

		if (key.empty ()) {
			Error ("Parking Node Skim is Missing");
		}
		Show_Message (1, "Open ") << key;
		key = Project_Filename (key);

		group.node_skim = MATRIX_LoadFromFile ((char *) key.c_str (), CONTROL_AUTOMATIC);

		if (tc_status != TC_OKAY || group.node_skim == 0) {
			Error (String ("Error Opening TransCAD Matrix \"%s\"") % key);
		}

		//---- check the matrix data ----

		rows = MATRIX_GetNRows (group.node_skim);
		cols = MATRIX_GetNCols (group.node_skim);
		tabs = MATRIX_GetNCores (group.node_skim);

		Show_Message (0, " (") << rows << "x" << cols << "x" << tabs << ")";

		if (rows != cols) goto match_error;
		if (cols != num_zones) goto zone_error;

		//---- get the new skim name ----

		key = Get_Control_Text (NEW_DRIVE_TO_TRANSIT_SKIM, i);
		
		if (key.empty ()) {
			Error ("New Drive to Transit Skim Missing");
		}
		group.new_name = Project_Filename (key);
	
		//---- get the return period options ----

		key = Get_Control_Text (RETURN_PERIOD_OPTIONS, i);
		group.periods.clear ();

		while (key.Split (record)) {
			period = record.Integer ();

			group.periods.push_back (period);
			periods.insert (period);
		}
		Print (1);
		file_group.push_back (group);
	}

	//---- initialize the return array ----

	num_return = (int) periods.size ();

	for (period_itr = periods.begin (); period_itr != periods.end (); period_itr++) {
		i = *period_itr;

		//---- open the walk to transit skim ----

		key = Get_Control_Text (WALK_TO_TRANSIT_SKIM, i);
		if (key.empty ()) {
			Error (String ("Walk to Transit Skim #%d Missing") % i);
		}
		return_data.group = i;

		Show_Message (1, "Open ") << key;
		key = Project_Filename (key);

		return_data.walk_skim = MATRIX_LoadFromFile ((char *) key.c_str (), CONTROL_AUTOMATIC);

		if (tc_status != TC_OKAY || return_data.walk_skim == 0) {
			Error (String ("Error Opening TransCAD Matrix \"%s\"") % key);
		}

		//---- check the matrix data ----

		rows = MATRIX_GetNRows (return_data.walk_skim);
		cols = MATRIX_GetNCols (return_data.walk_skim);
		type = MATRIX_GetDataType (return_data.walk_skim);
		tabs = MATRIX_GetNCores (return_data.walk_skim);

		Show_Message (0, " (") << rows << "x" << cols << "x" << tabs << ")";		

		if (rows != cols) goto match_error;
		if (cols != num_zones) goto zone_error;
		if (type != FLOAT_TYPE) goto type_error;

		//---- open the drive skim ----

		key = Get_Control_Text (DRIVE_FROM_PARKING_SKIM, i);
		
		if (key.empty ()) {
			Error (String ("Drive from Parking Skim #%d Missing") % i);;
		}
		Show_Message (1, "Open ") << key;
		key = Project_Filename (key);

		return_data.drive_skim = MATRIX_LoadFromFile ((char *) key.c_str (), CONTROL_AUTOMATIC);

		if (tc_status != TC_OKAY || return_data.drive_skim == 0) {
			Error (String ("Error Opening TransCAD Matrix \"%s\"") % key);
		}

		//---- check the matrix data ----

		rows = MATRIX_GetNRows (return_data.drive_skim);
		cols = MATRIX_GetNCols (return_data.drive_skim);
		type = MATRIX_GetDataType (return_data.drive_skim);
		tabs = MATRIX_GetNCores (return_data.drive_skim);

		Show_Message (0, " (") << rows << "x" << cols << "x" << tabs << ")";		

		if (cols != num_zones) goto zone_error;
		if (type != FLOAT_TYPE) goto type_error;

		//---- get the period prefix ----

		key = Get_Control_Text (RETURN_PERIOD_PREFIX, i);
		
		if (key.empty ()) {
			return_data.prefix = String ("RET%d") % i;
		} else {
			return_data.prefix = key;
		}
		Print (1);
		return_array.push_back (return_data);
	}

	//---- open parking zone map ----

	key = Get_Control_String (PARKING_ZONE_MAP_FILE);

	if (key.empty ()) {
		Error ("Parking Zone Map File is Missing");
	}
	parking_zone_file.File_Type ("Parking Zone Map File");

	parking_zone_file.Open (Project_Filename (key));

	//---- read the pnr table names ----

	num = Highest_Control_Group (DRIVE_TO_TRANSIT_TABLE, 0);

	if (num == 0) {
		Error ("No Drive to Transit Tables were Found");
	}
	Print (1);
	max_pnr = num;

	for (i=1; i <= num; i++) {
		if (!Check_Control_Key (DRIVE_TO_TRANSIT_TABLE, i)) continue;
		pnr_table.push_back (Get_Control_Text (DRIVE_TO_TRANSIT_TABLE, i));
		pnr_id.push_back (i);
	}

	//---- read the parking node table name ----

	Print (1);
	node_table = Get_Control_Text (PARKING_NODE_TABLE);

	//---- read the walk table names ----

	num = Highest_Control_Group (WALK_TO_TRANSIT_TABLE, 0);

	if (num == 0) {
		Error ("No Walk to Transit Tables were Found");
	}
	Print (1);

	for (i=1; i <= num; i++) {
		if (!Check_Control_Key (WALK_TO_TRANSIT_TABLE, i)) continue;
		walk_table.push_back (Get_Control_Text (WALK_TO_TRANSIT_TABLE, i));
		walk_id.push_back (i);
	}

	//---- read the drive from parking table name ----

	Print (1);
	drive_table = Get_Control_Text (DRIVE_FROM_PARKING_TABLE);

	//---- read the return table map ----

	num = Highest_Control_Group (RETURN_TABLE_MAP, 0);

	if (num > max_pnr) {
		Error (String ("Number of Return Table Maps are Out of Range (%d > %d)") % num % max_pnr);
	} else if (num > 0) {
		Print (1);
	}
	return_map.assign (pnr_id.size (), -1);

	for (i=0, pnr_itr = pnr_id.begin (); pnr_itr != pnr_id.end (); pnr_itr++, i++) {
		if (!Check_Control_Key (RETURN_TABLE_MAP, *pnr_itr)) {
			for (j=0, walk_itr = walk_id.begin (); walk_itr != walk_id.end (); walk_itr++, j++) {
				if (*walk_itr == *pnr_itr) {
					return_map [i] = j;
					break;
				}
			}
			if (walk_itr == walk_id.end ()) {
				Error (String ("A return table map could not be found for table #%d") % *pnr_itr);
			}
			continue;
		}
		key = Get_Control_Text (RETURN_TABLE_MAP, *pnr_itr);

		if (key.Starts_With ("WALK_")) {
			key = key.substr (5);
			k = key.Integer ();

			for (j=0, walk_itr = walk_id.begin (); walk_itr != walk_id.end (); walk_itr++, j++) {
				if (*walk_itr == k) {
					return_map [i] = j;
					break;
				}
			}
			if (walk_itr == walk_id.end ()) {
				Error (String ("Return table map #%d walk table #%d was not found") % *pnr_itr % k);
			}
		} else if (key.Equals ("DRIVE_FROM")) {
			return_map [i] = -2;
		} else {
			Error (String ("Return table map #%d syntax error") % *pnr_itr);
		}
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

