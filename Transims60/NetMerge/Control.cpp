//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "NetMerge.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void NetMerge::Program_Control (void)
{
	String key, merge_dir;

	//---- open network and demand files ----

	Data_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());
	Print (1);

	//---- merge directory ----

	merge_dir = Get_Control_String (MERGE_DIRECTORY);

	if (!merge_dir.empty ()) {
		Print_Filename (1, "Merge Directory", merge_dir);
		Fix_Directory (merge_dir);
	}

	//---- open merge node file ----

	key = Get_Control_String (MERGE_NODE_FILE);

	if (!key.empty ()) {
		node_file.File_Type ("Merge Node File");
		node_file.Open (merge_dir + key);
	}

	//---- open merge link file ----

	key = Get_Control_String (MERGE_LINK_FILE);

	if (!key.empty ()) {
		link_file.File_Type ("Merge Link File");
		link_file.Open (merge_dir + key);
	}

	//---- open merge shape file ----

	key = Get_Control_String (MERGE_SHAPE_FILE);

	if (!key.empty ()) {
		shape_file.File_Type ("Merge Shape File");
		shape_file.Open (merge_dir + key);
		shape_flag = true;
	}

	//---- open merge pocket file ----

	key = Get_Control_String (MERGE_POCKET_FILE);

	if (!key.empty ()) {
		pocket_file.File_Type ("Merge Pocket File");
		pocket_file.Open (merge_dir + key);
		pocket_flag = true;
	}

	//---- open merge lane use file ----

	key = Get_Control_String (MERGE_LANE_USE_FILE);

	if (!key.empty ()) {
		lane_use_file.File_Type ("Merge Lane Use File");
		lane_use_file.Open (merge_dir + key);
		lane_use_flag = true;
	}

	//---- open merge location file ----

	key = Get_Control_String (MERGE_LOCATION_FILE);

	if (!key.empty ()) {
		location_file.File_Type ("Merge Location File");
		location_file.Open (merge_dir + key);
		location_flag = true;
	}

	//---- open merge connection file ----

	key = Get_Control_String (MERGE_CONNECTION_FILE);

	if (!key.empty ()) {
		connect_file.File_Type ("Merge Connection File");
		connect_file.Open (merge_dir + key);
		connect_flag = true;
	}
	//---- open merge parking file ----

	key = Get_Control_String (MERGE_PARKING_FILE);

	if (!key.empty ()) {
		parking_file.File_Type ("Merge Parking File");
		parking_file.Open (merge_dir + key);
		parking_flag = true;
	}

	//---- open merge access file ----

	key = Get_Control_String (MERGE_ACCESS_FILE);

	if (!key.empty ()) {
		access_file.File_Type ("Merge Access File");
		access_file.Open (merge_dir + key);
		access_flag = true;
	}

	//---- open merge stop file ----

	key = Get_Control_String (MERGE_STOP_FILE);

	if (!key.empty ()) {
		stop_file.File_Type ("Merge Stop File");
		stop_file.Open (merge_dir + key);
		stop_flag = true;
	}

	//---- open merge sign file ----

	key = Get_Control_String (MERGE_SIGN_FILE);

	if (!key.empty ()) {
		sign_file.File_Type ("Merge Sign File");
		sign_file.Open (merge_dir + key);
		sign_flag = true;
	}

	//---- open merge signal file ----

	key = Get_Control_String (MERGE_SIGNAL_FILE);

	if (!key.empty ()) {
		signal_file.File_Type ("Merge Signal File");
		signal_file.Open (merge_dir + key);
		signal_flag = true;
	}

	//---- open merge timing file ----

	key = Get_Control_String (MERGE_TIMING_PLAN_FILE);

	if (!key.empty ()) {
		timing_file.File_Type ("Merge Timing Plan File");
		timing_file.Open (merge_dir + key);
		timing_flag = true;
	}

	//---- open merge phasing file ----

	key = Get_Control_String (MERGE_PHASING_PLAN_FILE);

	if (!key.empty ()) {
		phasing_file.File_Type ("Merge Phasing Plan File");
		phasing_file.Open (merge_dir + key);
		phasing_flag = true;
	}

	//---- open merge detector file ----

	key = Get_Control_String (MERGE_DETECTOR_FILE);

	if (!key.empty ()) {
		detector_file.File_Type ("Merge Detector File");
		detector_file.Open (merge_dir + key);
		detector_flag = true;
	}

	//---- open merge zone file ----

	key = Get_Control_String (MERGE_ZONE_FILE);

	if (!key.empty ()) {
		zone_file.File_Type ("Merge Zone File");
		zone_file.Open (merge_dir + key);
		zone_flag = true;
	}

	//---- open merge performance file ----

	key = Get_Control_String (MERGE_PERFORMANCE_FILE);

	if (!key.empty ()) {
		performance_file.File_Type ("Merge Performance File");
		performance_file.Open (merge_dir + key);
		performance_flag = true;
	}

	//---- open merge turn_delay file ----

	key = Get_Control_String (MERGE_TURN_DELAY_FILE);

	if (!key.empty ()) {
		turn_delay_file.File_Type ("Merge Turn Delay File");
		turn_delay_file.Open (merge_dir + key);
		turn_delay_flag = true;
	}

} 
