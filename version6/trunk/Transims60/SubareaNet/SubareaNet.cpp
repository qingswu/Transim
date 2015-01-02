//*********************************************************
//	SubareaNet.cpp - Create a subarea network files
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	SubareaNet constructor
//---------------------------------------------------------

SubareaNet::SubareaNet (void) : Data_Service ()
{
	Program ("SubareaNet");
<<<<<<< .working
	Version (6);
=======
	Version (7);
>>>>>>> .merge-right.r1529
	Title ("Subarea Network Files");

	System_File_Type required_files [] = {
		NODE, LINK, POCKET, CONNECTION, PARKING, LOCATION,  
		NEW_NODE, NEW_LINK, NEW_POCKET, NEW_CONNECTION, NEW_PARKING, NEW_LOCATION, 
		END_FILE
	};

	System_File_Type optional_files [] = {
		ZONE, SHAPE, LANE_USE, TURN_PENALTY, ACCESS_LINK,    
		SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR,  
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		PERFORMANCE, TURN_DELAY, RIDERSHIP,
		NEW_ZONE, NEW_SHAPE, NEW_LANE_USE, NEW_TURN_PENALTY, NEW_ACCESS_LINK,
		NEW_SIGN, NEW_SIGNAL, NEW_TIMING_PLAN, NEW_PHASING_PLAN, NEW_DETECTOR,
		NEW_TRANSIT_STOP, NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER,
		VEHICLE_TYPE, NEW_PERFORMANCE, NEW_TURN_DELAY, NEW_RIDERSHIP,
		END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};

	Control_Key subareanet_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ SUBAREA_BOUNDARY_POLYGON, "SUBAREA_BOUNDARY_POLYGON", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SUBAREA_COORDINATE_BOX, "SUBAREA_COORDINATE_BOX", LEVEL0, OPT_KEY, LIST_KEY, "", "XMIN, YMIN, XMAX, YMAX", NO_HELP },
		{ SUBAREA_NODE_CODE, "SUBAREA_NODE_CODE", LEVEL0, OPT_KEY, INT_KEY, "1", "1..100", NO_HELP },
		{ EXTERNAL_OFFSET_LENGTH, "EXTERNAL_OFFSET_LENGTH", LEVEL0, OPT_KEY, INT_KEY, "30 feet", "10..200 feet", NO_HELP },
		{ SUBAREA_BUFFER_DISTANCE, "SUBAREA_BUFFER_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "0 feet", "0..20000 feet", NO_HELP },
		{ SAVE_OUTSIDE_NETWORK, "SAVE_OUTSIDE_NETWORK", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};

	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (subareanet_keys);
	proj_service.Add_Keys ();

	transit_flag = zone_flag = box_flag = node_flag = outside_flag = false;
	max_location = max_parking = max_access = max_stop = max_route = max_zone = 0;
	new_location = new_parking = new_access = new_stop = new_route = new_zone = 0;
	nnode = nlink = nshort = nboundary = nshape = npocket = nconnect = 0;
	nlocation = nparking = naccess = nlane_use = nturn = ntoll = nperf = ndelay = 0;
	nsign = nsignal = ntiming = nphasing = ndetector = ncoord = 0;
	nstop = nroute = nschedule = ndriver = nrider = 0;

	external_offset = node_code = buffer = 0;

	System_Read_False (NODE);
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	SubareaNet *program = 0;
	try {
		program = new SubareaNet ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
