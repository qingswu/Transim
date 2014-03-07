//*********************************************************
//	LinkData.cpp - Convert Link Data to Link ID
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	LinkData constructor
//---------------------------------------------------------

LinkData::LinkData (void) : Data_Service ()
{
	Program ("LinkData");
	Version (4);
	Title ("Convert Link Data to Link ID");

	System_File_Type required_files [] = {
		NODE, LINK, END_FILE
	};
	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ LINK_NODE_LIST_FILE, "LINK_NODE_LIST_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },

		{ INPUT_NODE_FILE, "INPUT_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_NODE_FORMAT, "INPUT_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "CSV_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NODE_MAP_FILE, "NODE_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ COORDINATE_RESOLUTION, "COORDINATE_RESOLUTION", LEVEL0, OPT_KEY, FLOAT_KEY, "3.0 feet", "0..100 feet", NO_HELP },

		{ DIRECTIONAL_DATA_FILE, "DIRECTIONAL_DATA_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DIRECTIONAL_DATA_FORMAT, "DIRECTIONAL_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "CSV_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ FROM_NODE_FIELD_NAME, "FROM_NODE_FIELD_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "ANODE", ANODE_FIELD_RANGE, NO_HELP },
		{ TO_NODE_FIELD_NAME, "TO_NODE_FIELD_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "BNODE", BNODE_FIELD_RANGE, NO_HELP },
		{ VOLUME_DATA_FIELD_NAME, "VOLUME_DATA_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ SPEED_DATA_FIELD_NAME, "SPEED_DATA_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ AB_VOLUME_FIELD_NAME, "AB_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ BA_VOLUME_FIELD_NAME, "BA_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ BA_SPEED_FIELD_NAME, "AB_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ AB_SPEED_FIELD_NAME, "BA_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ NEW_LINK_DATA_FILE, "NEW_LINK_DATA_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FORMAT, "NEW_LINK_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_AB_VOLUME_FIELD_NAME, "NEW_AB_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "AB_VOLUME", "", NO_HELP },
		{ NEW_BA_VOLUME_FIELD_NAME, "NEW_BA_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "BA_VOLUME", "", NO_HELP },
		{ NEW_AB_SPEED_FIELD_NAME, "NEW_AB_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "AB_SPEED", "", NO_HELP },
		{ NEW_BA_SPEED_FIELD_NAME, "NEW_BA_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "BA_SPEED", "", NO_HELP },
		{ NEW_DIRECTIONAL_DATA_FILE, "NEW_DIRECTIONAL_DATA_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DIRECTIONAL_DATA_FORMAT, "NEW_DIRECTIONAL_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },

		{ NEW_LINK_NODE_FILE, "NEW_LINK_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_NODE_MAP_FILE, "NEW_NODE_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },

		END_CONTROL
	};
	Required_System_Files (required_files);
	Data_Service_Keys (data_service_keys);

	Key_List (keys);
	AB_Map_Flag (true);

	volume_flag = speed_flag = two_way_flag = data_flag = custom_flag = output_flag = false;
	link_node_flag = input_flag = node_map_flag = new_map_flag = new_link_node_flag = false;
	nequiv = nab = ndir = nlink = num_fields = 0;

	from_field = to_field = -1;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	LinkData *program = 0;
	try {
		program = new LinkData ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
