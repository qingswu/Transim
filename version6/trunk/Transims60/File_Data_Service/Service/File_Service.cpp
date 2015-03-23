//*********************************************************
//	File_Service.cpp - system file management service
//*********************************************************

#include "File_Service.hpp"

File_Service::File_Key  File_Service::file_keys [] = {
	{ NODE, "NODE_FILE", "NODE_FORMAT" },
	{ ZONE, "ZONE_FILE", "ZONE_FORMAT" },
	{ SHAPE, "SHAPE_FILE", "SHAPE_FORMAT" },
	{ LINK, "LINK_FILE", "LINK_FORMAT" },
	{ POCKET, "POCKET_FILE", "POCKET_FORMAT" },
	{ LANE_USE, "LANE_USE_FILE", "LANE_USE_FORMAT" },
	{ CONNECTION, "CONNECTION_FILE", "CONNECTION_FORMAT" },
	{ TURN_PENALTY, "TURN_PENALTY_FILE", "TURN_PENALTY_FORMAT" },
	{ PARKING, "PARKING_FILE", "PARKING_FORMAT" },
	{ LOCATION, "LOCATION_FILE", "LOCATION_FORMAT" },
	{ ACCESS_LINK, "ACCESS_FILE", "ACCESS_FORMAT" },
	{ SIGN, "SIGN_FILE", "SIGN_FORMAT" },
	{ SIGNAL, "SIGNAL_FILE", "SIGNAL_FORMAT" },
	{ PHASING_PLAN, "PHASING_PLAN_FILE", "PHASING_PLAN_FORMAT" },
	{ TIMING_PLAN, "TIMING_PLAN_FILE", "TIMING_PLAN_FORMAT" },
	{ DETECTOR, "DETECTOR_FILE", "DETECTOR_FORMAT" },
	{ TRANSIT_STOP, "TRANSIT_STOP_FILE", "TRANSIT_STOP_FORMAT" },
	{ TRANSIT_FARE, "TRANSIT_FARE_FILE", "TRANSIT_FARE_FORMAT" },
	{ TRANSIT_ROUTE, "TRANSIT_ROUTE_FILE", "TRANSIT_ROUTE_FORMAT" },
	{ TRANSIT_SCHEDULE, "TRANSIT_SCHEDULE_FILE", "TRANSIT_SCHEDULE_FORMAT" },
	{ TRANSIT_DRIVER, "TRANSIT_DRIVER_FILE", "TRANSIT_DRIVER_FORMAT" },
	{ ROUTE_NODES, "ROUTE_NODES_FILE", "ROUTE_NODES_FORMAT" },

	{ SELECTION, "SELECTION_FILE", "SELECTION_FORMAT" },
	{ HOUSEHOLD, "HOUSEHOLD_FILE", "HOUSEHOLD_FORMAT" },
	{ PERFORMANCE, "PERFORMANCE_FILE", "PERFORMANCE_FORMAT" },
	{ TURN_DELAY, "TURN_DELAY_FILE", "TURN_DELAY_FORMAT" },
	{ RIDERSHIP, "RIDERSHIP_FILE", "RIDERSHIP_FORMAT" },
	{ VEHICLE_TYPE, "VEHICLE_TYPE_FILE", "VEHICLE_TYPE_FORMAT" },
	{ TRIP, "TRIP_FILE", "TRIP_FORMAT" },
	{ PROBLEM, "PROBLEM_FILE", "PROBLEM_FORMAT" },
	{ PLAN, "PLAN_FILE", "PLAN_FORMAT" },
	{ SKIM, "SKIM_FILE", "SKIM_FORMAT" },
	{ EVENT, "EVENT_FILE", "EVENT_FORMAT" },
	{ TRAVELER, "TRAVELER_FILE", "TRAVELER_FORMAT" },

	{ NEW_NODE, "NEW_NODE_FILE", "NEW_NODE_FORMAT" },
	{ NEW_ZONE, "NEW_ZONE_FILE", "NEW_ZONE_FORMAT" },
	{ NEW_SHAPE, "NEW_SHAPE_FILE", "NEW_SHAPE_FORMAT" },
	{ NEW_LINK, "NEW_LINK_FILE", "NEW_LINK_FORMAT" },
	{ NEW_POCKET, "NEW_POCKET_FILE", "NEW_POCKET_FORMAT" },
	{ NEW_LANE_USE, "NEW_LANE_USE_FILE", "NEW_LANE_USE_FORMAT" },
	{ NEW_CONNECTION, "NEW_CONNECTION_FILE", "NEW_CONNECTION_FORMAT" },
	{ NEW_TURN_PENALTY, "NEW_TURN_PENALTY_FILE", "NEW_TURN_PENALTY_FORMAT" },
	{ NEW_PARKING, "NEW_PARKING_FILE", "NEW_PARKING_FORMAT" },
	{ NEW_LOCATION, "NEW_LOCATION_FILE", "NEW_LOCATION_FORMAT" },
	{ NEW_ACCESS_LINK, "NEW_ACCESS_FILE", "NEW_ACCESS_FORMAT" },
	{ NEW_SIGN, "NEW_SIGN_FILE", "NEW_SIGN_FORMAT" },
	{ NEW_SIGNAL, "NEW_SIGNAL_FILE", "NEW_SIGNAL_FORMAT" },
	{ NEW_PHASING_PLAN, "NEW_PHASING_PLAN_FILE", "NEW_PHASING_PLAN_FORMAT" },
	{ NEW_TIMING_PLAN, "NEW_TIMING_PLAN_FILE", "NEW_TIMING_PLAN_FORMAT" },
	{ NEW_DETECTOR, "NEW_DETECTOR_FILE", "NEW_DETECTOR_FORMAT" },
	{ NEW_TRANSIT_STOP, "NEW_TRANSIT_STOP_FILE", "NEW_TRANSIT_STOP_FORMAT" },
	{ NEW_TRANSIT_FARE, "NEW_TRANSIT_FARE_FILE", "NEW_TRANSIT_FARE_FORMAT" },
	{ NEW_TRANSIT_ROUTE, "NEW_TRANSIT_ROUTE_FILE", "NEW_TRANSIT_ROUTE_FORMAT" },
	{ NEW_TRANSIT_SCHEDULE, "NEW_TRANSIT_SCHEDULE_FILE", "NEW_TRANSIT_SCHEDULE_FORMAT" },
	{ NEW_TRANSIT_DRIVER, "NEW_TRANSIT_DRIVER_FILE", "NEW_TRANSIT_DRIVER_FORMAT" },
	{ NEW_ROUTE_NODES, "NEW_ROUTE_NODES_FILE", "NEW_ROUTE_NODES_FORMAT" },

	{ NEW_SELECTION, "NEW_SELECTION_FILE", "NEW_SELECTION_FORMAT" },
	{ NEW_HOUSEHOLD, "NEW_HOUSEHOLD_FILE", "NEW_HOUSEHOLD_FORMAT" },
	{ NEW_PERFORMANCE, "NEW_PERFORMANCE_FILE", "NEW_PERFORMANCE_FORMAT" },
	{ NEW_TURN_DELAY, "NEW_TURN_DELAY_FILE", "NEW_TURN_DELAY_FORMAT" },
	{ NEW_RIDERSHIP, "NEW_RIDERSHIP_FILE", "NEW_RIDERSHIP_FORMAT" },
	{ NEW_VEHICLE_TYPE, "NEW_VEHICLE_TYPE_FILE", "NEW_VEHICLE_TYPE_FORMAT" },
	{ NEW_TRIP, "NEW_TRIP_FILE", "NEW_TRIP_FORMAT" },
	{ NEW_PROBLEM, "NEW_PROBLEM_FILE", "NEW_PROBLEM_FORMAT" },
	{ NEW_PLAN, "NEW_PLAN_FILE", "NEW_PLAN_FORMAT" },
	{ NEW_SKIM, "NEW_SKIM_FILE", "NEW_SKIM_FORMAT" },
	{ NEW_EVENT, "NEW_EVENT_FILE", "NEW_EVENT_FORMAT" },
	{ NEW_TRAVELER, "NEW_TRAVELER_FILE", "NEW_TRAVELER_FORMAT" },
};

//---------------------------------------------------------
//	File_Service constructor
//---------------------------------------------------------

File_Service::File_Service (void) : Execution_Service ()
{
	//---- initialize the file information ----

	Service_Level (FILE_SERVICE);

	for (int i=0; i < END_FILE; i++) {
		system_file [i].file = 0;
		system_file [i].flag = false;
		system_file [i].option = false;
		system_file [i].data = false;
		system_file [i].reserve = 100;
		system_file [i].read = (i < NEW_NODE) ? true : false;
		system_file [i].access = (i < NEW_NODE) ? READ : CREATE;
		system_file [i].net = (i < SELECTION || (i >= NEW_NODE && i < NEW_SELECTION)) ? true : false;
		system_file [i].parts = (i == SELECTION || i == HOUSEHOLD || (i >= TRIP && i <= SKIM) ||
			i == NEW_SELECTION || i == NEW_HOUSEHOLD || (i >= NEW_TRIP && i <= NEW_SKIM)) ? true : false;
	}
	control_flag = zone_flag = link_flag = stop_flag = line_flag = time_flag = zone_loc_flag = false;
	lane_use_flows = false;

	AB_Map_Flag (false);
}

//---------------------------------------------------------
//	File_Service destructor
//---------------------------------------------------------

File_Service::~File_Service (void)
{
	for (int i=0; i < END_FILE; i++) {
		if (system_file [i].file != 0) {
			delete system_file [i].file;
			system_file [i].file = 0;
		}
	}
}

//---------------------------------------------------------
//	Required_System_Files
//---------------------------------------------------------

void File_Service::Required_System_Files (System_File_Type files [])
{
	System_File_Type *type;
	File_Key *key_ptr;
	Control_Key key;
	File_Data *data_ptr;

	key.levels = LEVEL0;

	for (type = files; *type < END_FILE; type++) {
		data_ptr = System_File (*type);
		data_ptr->flag = true;

		key_ptr = file_keys + *type;

		//---- add the file key ----

		key.code = *type + SYSTEM_FILE_OFFSET;
		key.key = key_ptr->key;
		key.option = REQ_KEY;
		key.type = (*type < NEW_NODE) ? IN_KEY : OUT_KEY;
		key.value = "";
		key.range = (data_ptr->parts) ? PARTITION_RANGE : FILE_RANGE;
		key.help = NO_HELP;

		Add_Control_Key (key);

		//---- add the format key ----

		key.code = *type + SYSTEM_FORMAT_OFFSET;
		key.key = key_ptr->format;
		key.option = OPT_KEY;
		key.type = TEXT_KEY;
		key.value = Format_Code (Default_Format ());
		key.range = FORMAT_RANGE;
		key.help = FORMAT_HELP;

		Add_Control_Key (key);
	}
}

//---------------------------------------------------------
//	Optional_System_Files
//---------------------------------------------------------

void File_Service::Optional_System_Files (System_File_Type files [])
{
	System_File_Type *type;
	File_Key *key_ptr;
	Control_Key key;
	File_Data *data_ptr;

	key.levels = LEVEL0;
	key.option = OPT_KEY;

	for (type = files; *type < END_FILE; type++) {
		data_ptr = System_File (*type);
		data_ptr->option = true;

		key_ptr = file_keys + *type;

		//---- add the file key ----

		key.code = *type + SYSTEM_FILE_OFFSET;
		key.key = key_ptr->key;
		key.type = (*type < NEW_NODE) ? IN_KEY : OUT_KEY;
		key.value = "";
		key.range = (data_ptr->parts) ? PARTITION_RANGE : FILE_RANGE;
		key.help = NO_HELP;

		Add_Control_Key (key);

		//---- add the format key ----

		key.code = *type + SYSTEM_FORMAT_OFFSET;
		key.key = key_ptr->format;
		key.type = TEXT_KEY;
		key.value = Format_Code (Default_Format ());
		key.range = FORMAT_RANGE;
		key.help = FORMAT_HELP;

		Add_Control_Key (key);
	}
}

//---------------------------------------------------------
//	File_Service_Keys
//---------------------------------------------------------

void File_Service::File_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, help ----
		{ NOTES_AND_NAME_FIELDS, "NOTES_AND_NAME_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SAVE_LANE_USE_FLOWS, "SAVE_LANE_USE_FLOWS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },

		{ TRANSIT_TIME_PERIODS, "TRANSIT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ ROUTE_NODE_OFFSET_FLAG, "ROUTE_NODE_OFFSET_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ROUTE_NODE_RUN_TIME_FLAG, "ROUTE_NODE_RUN_TIME_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ROUTE_NODE_PATTERN_FLAG, "ROUTE_NODE_PATTERN_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ROUTE_NODE_DWELL_FLAG, "ROUTE_NODE_DWELL_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ROUTE_NODE_TYPE_FLAG, "ROUTE_NODE_TYPE_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ROUTE_NODE_LEG_TIME_FLAG, "ROUTE_NODE_LEG_TIME_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ROUTE_NODE_SPEED_FLAG, "ROUTE_NODE_SPEED_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },

		{ SKIM_OD_UNITS, "SKIM_OD_UNITS", LEVEL0, OPT_KEY, TEXT_KEY, "ZONES", MATRIX_OD_RANGE, NO_HELP },
		{ SKIM_TIME_PERIODS, "SKIM_TIME_PERIODS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP},
		{ SKIM_TIME_INCREMENT, "SKIM_TIME_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", MINUTE_RANGE, NO_HELP },
		{ SKIM_TOTAL_TIME_FLAG, "SKIM_TOTAL_TIME_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SKIM_TRAVEL_TIME_FORMAT, "SKIM_TRAVEL_TIME_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "SECONDS", TIME_FORMAT_RANGE, NO_HELP },
		{ SKIM_TRIP_LENGTH_FORMAT, "SKIM_TRIP_LENGTH_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "FEET", LENGTH_FORMAT_RANGE, NO_HELP },
		{ NEAREST_NEIGHBOR_FACTOR, "NEAREST_NEIGHBOR_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..1.0", NO_HELP },
		{ MERGE_TIME_PERIODS, "MERGE_TIME_PERIODS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SKIM_FILE_HEADERS, "SKIM_FILE_HEADERS", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ ZONE_EQUIVALENCE_FILE, "ZONE_EQUIVALENCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_EQUIVALENCE_FILE, "LINK_EQUIVALENCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ STOP_EQUIVALENCE_FILE, "STOP_EQUIVALENCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINE_EQUIVALENCE_FILE, "LINE_EQUIVALENCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TIME_EQUIVALENCE_FILE, "TIME_EQUIVALENCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_LOCATION_MAP_FILE, "ZONE_LOCATION_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};

	if (keys == 0) {
		Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				Error (String ("File Service Key %d was Not Found") % keys [i]);
			} else if (keys [i] == ZONE_EQUIVALENCE_FILE) {
				zone_flag = true;
			} else if (keys [i] == LINK_EQUIVALENCE_FILE) {
				link_flag = true;
			} else if (keys [i] == STOP_EQUIVALENCE_FILE) {
				stop_flag = true;
			} else if (keys [i] == LINE_EQUIVALENCE_FILE) {
				line_flag = true;
			} else if (keys [i] == TIME_EQUIVALENCE_FILE) {
				time_flag = true;
			} else if (keys [i] == ZONE_LOCATION_MAP_FILE) {
				zone_loc_flag = true;
			}
		}
	}
}

//-----------------------------------------------------------
//	Copy_File_Header
//-----------------------------------------------------------

bool File_Service::Copy_File_Header (System_File_Type from, System_File_Type to)
{ 
	if (System_File_Flag (from) && System_File_Flag (to)) {
		Db_Header *file, *new_file;

		file = (Db_Header *) System_File_Base (from);
		new_file = (Db_Header *) System_File_Base (to);

		new_file->Clear_Fields ();
		new_file->Replicate_Fields (file);
		new_file->LinkDir_Type ((Direction_Type) file->LinkDir_Type ());

		return (new_file->Write_Header ());
	}
	return (false);
}

//-----------------------------------------------------------
//	Required_File_Check
//-----------------------------------------------------------

bool File_Service::Required_File_Check (Db_File &file, System_File_Type type)
{
	if (!System_File_Flag (type) && System_Option_Flag (type) && !System_Data_Flag (type)) {
		Error (String ("%s file is required for %s file processing") % System_File_Code (type) % file.File_ID ());
		return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void File_Service::Program_Control (void)
{
	int i, parts;
	bool first_open_net, first_open, first_new_net, first_new, skim_flag, flag;
	String key, format;
	bool turn_delay, turn_perf;

	File_Data *file;

	first_open_net = first_open = first_new_net = first_new = true;
	skim_flag = turn_delay = turn_perf = false;

	Execution_Service::Program_Control ();

	if (Control_Key_Status (SAVE_LANE_USE_FLOWS)) {
		Lane_Use_Flows (Set_Control_Flag (SAVE_LANE_USE_FLOWS));
	}
	if (Control_Key_Status (TRANSIT_TIME_PERIODS)) {
		key = Get_Control_String (TRANSIT_TIME_PERIODS);

		if (!key.empty ()) {
			transit_time_periods.Add_Breaks (key);
		}
	}
	if (Control_Key_Status (NOTES_AND_NAME_FIELDS)) {
		Notes_Name_Flag (Set_Control_Flag (NOTES_AND_NAME_FIELDS));
	}

	//---- open the files ----

	for (i=0; i < END_FILE; i++) {
		file = system_file + i;

		if (!file->flag && !file->option) continue;

		//---- get the filename ----

		key = Get_Control_String (SYSTEM_FILE_OFFSET + i);

		if (key.empty ()) {
			if (file->flag) {
				Error (String ("Missing Control Key = %s") % Current_Key ());
			}
			continue;
		}

		//---- check the access status ----

		if (file->access == READ) {
			if (file->net) {
				if (first_open_net) {
					Print (2, "Input System Network Files:");
					first_open_net = false;
				}
			} else if (first_open) {
				Print (2, "Input System Demand Files:");
				first_open = false;
			}
		} else if (file->net) {
			if (first_new_net) {
				Print (2, "Output System Network Files:");
				first_new_net = false;
			}
		} else if (first_new) {
			Print (2, "Output System Demand Files:");
			first_new = false;
		}

		//---- open the file ----

		key = Project_Filename (key);

		file->flag = true;

		format = Get_Control_String (SYSTEM_FORMAT_OFFSET + i);

		switch (i) {
			case NODE:
			case NEW_NODE:
				file->file = new Node_File (key, file->access, format);
				break;
			case ZONE:
			case NEW_ZONE:
				file->file = new Zone_File (key, file->access, format);
				break;
			case SHAPE:
			case NEW_SHAPE:
				file->file = new Shape_File (key, file->access, format);
				break;
			case LINK:
			case NEW_LINK:
				{
					Format_Type model_format, file_format;
					Data_Format (format, file_format, model_format);

					if (file_format == ARCVIEW) {
						file->file = new Arc_Link_File (key, file->access, format);
						((Arc_Link_File *) file->file)->Shape_Type (VECTOR);
					} else {
						file->file = new Link_File (key, file->access, format);
					}
				}
				break;
			case POCKET:
			case NEW_POCKET:
				file->file = new Pocket_File (key, file->access, format);
				break;
			case LANE_USE:
			case NEW_LANE_USE:
				file->file = new Lane_Use_File (key, file->access, format);
				break;
			case CONNECTION:
				turn_delay = turn_perf = true;
			case NEW_CONNECTION:
				file->file = new Connect_File (key, file->access, format);
				break;
			case TURN_PENALTY:
			case NEW_TURN_PENALTY:
				file->file = new Turn_Pen_File (key, file->access, format);
				if (((Turn_Pen_File *) (file->file))->Node_Based ()) AB_Map_Flag (true);
				break;
			case PARKING:
			case NEW_PARKING:
				file->file = new Parking_File (key, file->access, format);
				break;
			case LOCATION:
			case NEW_LOCATION:
				file->file = new Location_File (key, file->access, format);
				break;
			case ACCESS_LINK:
			case NEW_ACCESS_LINK:
				file->file = new Access_File (key, file->access, format);
				break;
			case SIGN:
			case NEW_SIGN:
				file->file = new Sign_File (key, file->access, format);
				break;
			case SIGNAL:
			case NEW_SIGNAL:
				file->file = new Signal_File (key, file->access, format);
				break;
			case TIMING_PLAN:
			case NEW_TIMING_PLAN:
				file->file = new Timing_File (key, file->access, format);
				break;
			case PHASING_PLAN:
			case NEW_PHASING_PLAN:
				file->file = new Phasing_File (key, file->access, format);
				break;
			case DETECTOR:
			case NEW_DETECTOR:
				file->file = new Detector_File (key, file->access, format);
				break;
			case TRANSIT_STOP:
			case NEW_TRANSIT_STOP:
				file->file = new Stop_File (key, file->access, format);
				break;
			case TRANSIT_FARE:
			case NEW_TRANSIT_FARE:
				file->file = new Fare_File (key, file->access, format);
				break;
			case TRANSIT_ROUTE:
			case NEW_TRANSIT_ROUTE:
				file->file = new Line_File (key, file->access, format);
				break;
			case TRANSIT_SCHEDULE:
			case NEW_TRANSIT_SCHEDULE:
				file->file = new Schedule_File (key, file->access, format);
				break;
			case TRANSIT_DRIVER:
			case NEW_TRANSIT_DRIVER:
				file->file = new Driver_File (key, file->access, format);
				break;
			case ROUTE_NODES:
			case NEW_ROUTE_NODES:
				file->file = new Route_Nodes_File (key, file->access, format);
				if (i == NEW_ROUTE_NODES) {
					Route_Nodes_File *route_file = (Route_Nodes_File *) file->file;
					flag = false;

					if (Control_Key_Status (TRANSIT_TIME_PERIODS)) {
						Get_Control_Text (TRANSIT_TIME_PERIODS);
						route_file->Num_Periods (transit_time_periods.Num_Periods ());
						if (route_file->Num_Periods () > 0) flag = true;
					}
					if (Check_Control_Key (ROUTE_NODE_OFFSET_FLAG)) {
						route_file->Offset_Flag (Get_Control_Flag (ROUTE_NODE_OFFSET_FLAG));
						if (route_file->Offset_Flag () == true) flag = true;
					}
					if (Check_Control_Key (ROUTE_NODE_RUN_TIME_FLAG)) {
						route_file->TTime_Flag (Get_Control_Flag (ROUTE_NODE_RUN_TIME_FLAG));
						if (route_file->TTime_Flag () == true) flag = true;
					}
					if (Check_Control_Key (ROUTE_NODE_PATTERN_FLAG)) {
						route_file->Pattern_Flag (Get_Control_Flag (ROUTE_NODE_PATTERN_FLAG));
						if (route_file->Pattern_Flag () == true) flag = true;
					}
					if (Check_Control_Key (ROUTE_NODE_DWELL_FLAG)) {
						route_file->Dwell_Flag (Get_Control_Flag (ROUTE_NODE_DWELL_FLAG));
						if (route_file->Dwell_Flag () == true) flag = true;
					}
					if (Check_Control_Key (ROUTE_NODE_TYPE_FLAG)) {
						route_file->Type_Flag (Get_Control_Flag (ROUTE_NODE_TYPE_FLAG));
						if (route_file->Type_Flag () == true) flag = true;
					}
					if (Check_Control_Key (ROUTE_NODE_SPEED_FLAG)) {
						route_file->Speed_Flag (Get_Control_Flag (ROUTE_NODE_SPEED_FLAG));
						if (route_file->Speed_Flag () == true) flag = true;
					}
					if (Check_Control_Key (ROUTE_NODE_LEG_TIME_FLAG)) {
						route_file->Time_Flag (Get_Control_Flag (ROUTE_NODE_LEG_TIME_FLAG));
						if (route_file->Time_Flag () == true) flag = true;
					}
					if (flag) {
						route_file->Clear_Fields ();
						route_file->Create_Fields ();
						route_file->Write_Header ();
						Print (1);
					}
				}
				break;
			case SELECTION:
			case NEW_SELECTION:
				file->file = new Selection_File (key, file->access, format);
				if (i == SELECTION && file->file->Part_Flag ()) {
					parts = file->file->Num_Parts ();
					if (parts > 1) Print (0, String (" (%d partitions)") % parts);
				}
				break;
			case HOUSEHOLD:
			case NEW_HOUSEHOLD:
				file->file = new Household_File (key, file->access, format);
				if (i == HOUSEHOLD && file->file->Part_Flag ()) {
					parts = file->file->Num_Parts ();
					if (parts > 1) Print (0, String (" (%d partitions)") % parts);
				}
				break;
			case PERFORMANCE:
			case NEW_PERFORMANCE:
				file->file = new Performance_File (key, file->access, format, lane_use_flows);
				break;
			case TURN_DELAY:
			case NEW_TURN_DELAY:
				file->file = new Turn_Delay_File (key, file->access, format);
				break;
			case RIDERSHIP:
			case NEW_RIDERSHIP:
				file->file = new Ridership_File (key, file->access, format);
				break;
			case VEHICLE_TYPE:
			case NEW_VEHICLE_TYPE:
				file->file = new Veh_Type_File (key, file->access, format);
				break;
			case TRIP:
			case NEW_TRIP:
				file->file = new Trip_File (key, file->access, format);
				if (i == TRIP && file->file->Part_Flag ()) {
					parts = file->file->Num_Parts ();
					if (parts > 1) Print (0, String (" (%d partitions)") % parts);
				}
				break;
			case PROBLEM:
			case NEW_PROBLEM:
				file->file = new Problem_File (key, file->access, format);
				if (i == PROBLEM && file->file->Part_Flag ()) {
					parts = file->file->Num_Parts ();
					if (parts > 1) Print (0, String (" (%d partitions)") % parts);
				}
				break;
			case PLAN:
			case NEW_PLAN:
				file->file = new Plan_File (key, file->access, format);
				if (i == PLAN && file->file->Part_Flag ()) {
					parts = file->file->Num_Parts ();
					if (parts > 1) Print (0, String (" (%d partitions)") % parts);
				}
				break;
			case SKIM:
			case NEW_SKIM:
				file->file = new Skim_File (key, file->access, format);
				if (i == NEW_SKIM) skim_flag = true;
				if (i == SKIM && file->file->Part_Flag ()) {
					parts = file->file->Num_Parts ();
					if (parts > 1) Print (0, String (" (%d partitions)") % parts);
				}
				break;
			case EVENT:
			case NEW_EVENT:
				file->file = new Event_File (key, file->access, format);
				break;
			case TRAVELER:
			case NEW_TRAVELER:
				file->file = new Traveler_File (key, file->access, format);
				break;
			default:
				//Error (String ("System File Type %s was Not Initialized") % System_File_Code ((System_File_Type) i));
				break;
		}
	}

	//---- save link use flows ----

	if (Control_Key_Status (SAVE_LANE_USE_FLOWS)) {
		Print (1);
		Get_Control_Flag (SAVE_LANE_USE_FLOWS);
	}

	if (Control_Key_Status (TRANSIT_TIME_PERIODS) && !System_File_Flag (NEW_ROUTE_NODES)) {
		Print (1);
		key = Get_Control_Text (TRANSIT_TIME_PERIODS);
	}

	//---- skim keys  ----

	if (skim_flag) {
		file = system_file + NEW_SKIM;

		Skim_File *skim_file = (Skim_File *) file->file;

		Print (1);
		if (Control_Key_Status (SKIM_OD_UNITS)) {
			key = Get_Control_Text (SKIM_OD_UNITS);
			if (!key.empty ()) {
				skim_file->OD_Units (Matrix_OD_Code (key));

				if (skim_file->OD_Units () == DISTRICT_OD) {
					zone_flag = Required_Control_Key (ZONE_EQUIVALENCE_FILE, true);
				}
			}
		}

		//---- skim time periods -----

		if (Control_Key_Status (SKIM_TIME_PERIODS)) {
			key = Get_Control_Text (SKIM_TIME_PERIODS);
			if (!key.empty ()) {

				//---- skim time increment -----

				skim_file->Increment (Get_Control_Time (SKIM_TIME_INCREMENT));

				if (!key.Equals ("ALL")) {
					skim_file->Add_Ranges (key);
				}
			}
		}

		//---- total time flag ----

		if (Control_Key_Status (SKIM_TOTAL_TIME_FLAG)) {
			skim_file->Total_Time_Flag (Get_Control_Flag (SKIM_TOTAL_TIME_FLAG));
		}

		//---- skim time format ----

		if (Control_Key_Status (SKIM_TRAVEL_TIME_FORMAT)) {
			skim_file->Time_Format (Time_Code (Get_Control_Text (SKIM_TRAVEL_TIME_FORMAT)));
		}

		//---- skim distance format ----

		if (Control_Key_Status (SKIM_TRIP_LENGTH_FORMAT)) {
			if (Check_Control_Key (SKIM_TRIP_LENGTH_FORMAT)) skim_file->Length_Flag (true);
			skim_file->Length_Format (Length_Code (Get_Control_Text (SKIM_TRIP_LENGTH_FORMAT)));
		}

		//---- nearest neighbor factor ----

		if (Control_Key_Status (NEAREST_NEIGHBOR_FACTOR)) {
			skim_file->Neighbor_Factor (DTOI (Get_Control_Double (NEAREST_NEIGHBOR_FACTOR) * 100.0));
		}

		//---- merge time periods ----

		if (Control_Key_Status (MERGE_TIME_PERIODS)) {
			skim_file->Merge_Flag (Get_Control_Flag (MERGE_TIME_PERIODS));
		}
		
		//---- skim file headers ----

		if (Control_Key_Status (SKIM_FILE_HEADERS)) {
			if (!Get_Control_Flag (SKIM_FILE_HEADERS)) {
				if (skim_file->Dbase_Format () != BINARY) {
					skim_file->Header_Lines (0);
				}
			}
		}

		//---- set the skim fields ----

		skim_file->Create_Fields ();
		skim_file->Write_Header ();
	}
	
	//---- create notes and name fields  ----

	if (Control_Key_Status (NOTES_AND_NAME_FIELDS)) {
		Print (1);
		Get_Control_Flag (NOTES_AND_NAME_FIELDS);
	}
	first_open = true;

	//---- open the zone equivalence ----

	if (zone_flag) {
		key = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (key.empty ()) {
			if (Required_Control_Key (ZONE_EQUIVALENCE_FILE)) {
				Error ("A Zone Equivalence File is Required for Zone Group Processing");
			}
			zone_flag = false;
		} else {
			if (first_open) {
				Print (2, "Input Equivalence Files:");
				first_open = false;
			}
			zone_equiv.Open (Project_Filename (key));
		}
	}

	//---- open the link equivalence ----

	if (link_flag) {
		key = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (key.empty ()) {
			if (Required_Control_Key (LINK_EQUIVALENCE_FILE)) {
				Error ("A Link Equivalence File is Required for Link Group Processing");
			}
			link_flag = false;
		} else {
			if (first_open) {
				Print (2, "Input Equivalence Files:");
				first_open = false;
			}
			link_equiv.Open (Project_Filename (key));
		}
	}

	//---- open the stop equivalence ----

	if (stop_flag) {
		key = Get_Control_String (STOP_EQUIVALENCE_FILE);

		if (key.empty ()) {
			if (Required_Control_Key (STOP_EQUIVALENCE_FILE)) {
				Error ("A Stop Equivalence File is Required for Stop Group Processing");
			}
			stop_flag = false;
		} else {
			if (first_open) {
				Print (2, "Input Equivalence Files:");
				first_open = false;
			}
			stop_equiv.Open (Project_Filename (key));
		}
	}

	//---- open the line equivalence ----

	if (line_flag) {
		key = Get_Control_String (LINE_EQUIVALENCE_FILE);

		if (key.empty ()) {
			if (Required_Control_Key (LINE_EQUIVALENCE_FILE)) {
				Error ("A Line Equivalence File is Required for Line Group Processing");
			}
			line_flag = false;
		} else {
			if (first_open) {
				Print (2, "Input Equivalence Files:");
				first_open = false;
			}
			line_equiv.Open (Project_Filename (key));
		}
	}

	//---- open the time equivalence ----

	if (time_flag) {
		key = Get_Control_String (TIME_EQUIVALENCE_FILE);

		if (key.empty ()) {
			if (Required_Control_Key (TIME_EQUIVALENCE_FILE)) {
				Error ("A Time Equivalence File is Required for Time Period Processing");
			}
			time_flag = false;
		} else {
			if (first_open) {
				Print (2, "Input Equivalence Files:");
				first_open = false;
			}
			time_equiv.Open (Project_Filename (key));
		}
	}

	//---- open the zone location map file ----

	if (zone_loc_flag) {
		key = Get_Control_String (ZONE_LOCATION_MAP_FILE);

		if (key.empty ()) {
			if (Required_Control_Key (ZONE_LOCATION_MAP_FILE)) {
				Error ("A Zone Location Map File is Required for Zone Processing");
			}
			zone_loc_flag = false;
		} else {
			if (first_open) {
				Print (2, "Input Equivalence Files:");
				first_open = false;
			}
			zone_loc_map.Open (Project_Filename (key));
		}
	}
	control_flag = true;
	return;
}
