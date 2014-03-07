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
	Version (0);
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
		NEW_ZONE, NEW_SHAPE, NEW_LANE_USE, NEW_TURN_PENALTY, NEW_ACCESS_LINK,
		NEW_SIGN, NEW_SIGNAL, NEW_TIMING_PLAN, NEW_PHASING_PLAN, NEW_DETECTOR,
		NEW_TRANSIT_STOP, NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER,
		VEHICLE_TYPE, 
		END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};

	Control_Key subareanet_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ SUBAREA_BOUNDARY_POLYGON, "SUBAREA_BOUNDARY_POLYGON", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SUBAREA_COORDINATE_BOX, "SUBAREA_COORDINATE_BOX", LEVEL0, OPT_KEY, IN_KEY, "", "XMIN, YMIN, XMAX, YMAX", NO_HELP },
		{ EXTERNAL_OFFSET_LENGTH, "EXTERNAL_OFFSET_LENGTH", LEVEL0, OPT_KEY, LIST_KEY, "30 feet", "10..200 feet", NO_HELP },
		END_CONTROL
	};

	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (subareanet_keys);
	proj_service.Add_Keys ();

	transit_flag = zone_flag = box_flag = false;
	max_location = max_parking = max_access = max_stop = max_route = max_zone = 0;
	new_location = new_parking = new_access = new_stop = new_route = new_zone = 0;
	nnode = nlink = nshort = nboundary = nshape = npocket = nconnect = 0;
	nlocation = nparking = naccess = nlane_use = nturn = ntoll = 0;
	nsign = nsignal = ntiming = nphasing = ndetector = ncoord = 0;
	nstop = nroute = nschedule = ndriver = 0;

	external_offset = 0;	
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
