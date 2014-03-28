//*********************************************************
//	Control.cpp - simulation service control key processing
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Simulator_Service::Program_Control (void)
{
	int i;
	String key;
	Double_List list;
	Dbl_Itr dbl_itr;

	//---- set / check the signal files ----

	if (Check_Control_Key (MESOSCOPIC_SUBAREAS) || Check_Control_Key (MICROSCOPIC_SUBAREAS)) {
		param.control_flag = true;
		key = Get_Control_String (MESOSCOPIC_SUBAREAS);
		if (key.Equals ("NONE")) {
			key = Get_Control_String (MICROSCOPIC_SUBAREAS);
			if (key.Equals ("NONE")) {
				param.control_flag = false;
			}
		}
	} else if (Check_Control_Key (UNSIMULATED_SUBAREAS) || Check_Control_Key (MACROSCOPIC_SUBAREAS)) {
		param.control_flag = false;
		key = Get_Control_String (MACROSCOPIC_SUBAREAS);
		if (key.Equals ("NONE")) {
			key = Get_Control_String (UNSIMULATED_SUBAREAS);
			if (key.Equals ("NONE")) {
				param.control_flag = true;
			}
		}
	}
	if (param.control_flag) {
		if (!System_Control_Check (SIGNAL) || !System_Control_Check (TIMING_PLAN) || !System_Control_Check (PHASING_PLAN)) {
			Warning ("Signal, Timing Plan, and Phasing Plan files are Required for Signal Processing");
		}
	} else {
		System_File_False (SIGNAL);
		System_File_False (TIMING_PLAN);
		System_File_False (PHASING_PLAN);
		System_File_False (DETECTOR);
		System_File_False (SIGN);
	}

	//--- set / check the transit files ----

	param.transit_flag = (System_Control_Check (TRANSIT_STOP) && System_Control_Check (TRANSIT_ROUTE) && 
					System_Control_Check (TRANSIT_SCHEDULE) && System_Control_Check (TRANSIT_DRIVER));

	if (!param.transit_flag) {
		if (System_Control_Check (TRANSIT_STOP) || System_Control_Check (TRANSIT_ROUTE) || 
			System_Control_Check (TRANSIT_SCHEDULE) || System_Control_Check (TRANSIT_DRIVER)) {
			Warning ("Transit Stop, Route, Schedule and Driver files are Required for Transit Processing");
		}
		System_File_False (TRANSIT_STOP);
		System_File_False (TRANSIT_ROUTE);
		System_File_False (TRANSIT_SCHEDULE);
		System_File_False (TRANSIT_DRIVER);
		System_File_False (TRANSIT_FARE);
	} 

	//---- process the data controls ----

	//Router_Service::Program_Control ();
	Data_Service::Program_Control ();

	Read_Select_Keys ();

	if (System_File_Flag (NEW_PROBLEM)) {
		param.problem_flag = true;
		Problem_File *problem_file = (Problem_File *) System_File_Handle (NEW_PROBLEM);
		problem_file->Simulator_Data ();
	}
	Print (2, String ("%s Control Keys:") % Program ());

	//---- simulation start and end time ----

	param.start_time_step = Get_Control_Time (SIMULATION_START_TIME);
	param.end_time_step = Get_Control_Time (SIMULATION_END_TIME);

	if (param.start_time_step == 0) param.start_time_step = Model_Start_Time ();
	if (param.end_time_step == 0 || !Check_Control_Key (SIMULATION_END_TIME)) {
		param.end_time_step = Model_End_Time ();
	}
	time_step = param.start_time_step;

	//---- time steps per second ----

	Print (1);
	param.step_size = Get_Control_Time (TIME_STEPS);
	two_step = 2 * param.step_size;

	//---- get the cell size ----

	param.cell_size = Round (Get_Control_Double (CELL_SIZE));

	//---- plan following distance ----

	Print (1);
	param.plan_follow = Get_Control_Integer (PLAN_FOLLOWING_DISTANCE);

	//---- look ahead distance ----

	param.look_ahead = Get_Control_Integer (LOOK_AHEAD_DISTANCE);

	param.look_ahead_flag = (param.look_ahead > 0);

	if (param.look_ahead_flag) {

		//---- look ahead lane factor ----

		param.lane_factor = Get_Control_Double (LOOK_AHEAD_LANE_FACTOR);

		//---- look ahead time factor ----

		param.time_factor = Get_Control_Double (LOOK_AHEAD_TIME_FACTOR);

		//---- look ahead vehicle factor ----

		param.veh_factor = Get_Control_Double (LOOK_AHEAD_VEHICLE_FACTOR);
	}

	//---- maximum swapping speed ----

	param.max_swap_speed = Round (Get_Control_Double (MAXIMUM_SWAPPING_SPEED));

	//---- maximum speed difference ----

	param.max_swap_diff = Round (Get_Control_Double (MAXIMUM_SPEED_DIFFERENCE));

	//---- enforce parking lanes -----

	param.parking_lanes = Get_Control_Flag (ENFORCE_PARKING_LANES);

	//---- driver reaction time ----

	param.reaction_flag = Get_Control_List_Groups (DRIVER_REACTION_TIME, list);
	
	for (i=0; i <= EXTERNAL; i++) {
		param.reaction_time [i] = list.Best (i);
	}

	//---- permission probability ----

	Get_Control_List_Groups (PERMISSION_PROBABILITY, list);

	param.permit_none = param.permit_all = true;

	for (i=0; i <= EXTERNAL; i++) {
		param.permission_prob [i] = list.Best (i) / 100.0;
		if (param.permission_prob [i] != 0.0) param.permit_none = false;
		if (param.permission_prob [i] != 1.0) param.permit_all = false;
	}

	//---- slow down probability ----

	param.slow_down_flag = Get_Control_List_Groups (SLOW_DOWN_PROBABILITY, list);

	for (i=0; i <= EXTERNAL; i++) {
		param.slow_down_prob [i] = list.Best (i) / 100.0;
	}

	//---- slow down percentage ----

	if (param.slow_down_flag) {
		Get_Control_List_Groups (SLOW_DOWN_PERCENTAGE, list);

		for (i=0; i <= EXTERNAL; i++) {
			param.slow_down_percent [i] = list.Best (i) / 100.0;
		}
	}

	//---- max comfortable speed ----

	Get_Control_List_Groups (MAX_COMFORTABLE_SPEED, param.comfort_speed, true);
	
	for (dbl_itr = param.comfort_speed.begin (); dbl_itr != param.comfort_speed.end (); dbl_itr++) {
		if (*dbl_itr != 0.0) {
			param.comfort_flag = true;
			break;
		}
	}

	//---- traveler type factors ----

	Get_Control_List_Groups (TRAVELER_TYPE_FACTORS, param.traveler_fac);

	param.traveler_flag = false;

	for (i=0; i < (int) param.traveler_fac.size (); i++) {
		if (param.traveler_fac [i] != 1.0) {
			param.traveler_flag = true;
		}
	}

	//---- priority loading time ----
	
	Print (1);
	param.min_load_time = Get_Control_Time (PRIORITY_LOADING_TIME);

	//---- maximum loading time ----

	param.max_load_time = Get_Control_Time (MAXIMUM_LOADING_TIME);
	if (param.max_load_time == 0) param.max_load_time = Dtime (MIDNIGHT, SECONDS);

	if (param.min_load_time > param.max_load_time) {
		Error ("Priority Loading Time > Maximum Loading Time");
	}
	param.priority_load = (param.min_load_time + param.max_load_time) / 2;

	//---- priority waiting time ----

	param.min_wait_time = Get_Control_Time (PRIORITY_WAITING_TIME);

	//---- maximum waiting time ----

	param.max_wait_time = Get_Control_Time (MAXIMUM_WAITING_TIME);

	if (param.min_wait_time > param.max_wait_time) {
		Error ("Priority Waiting Time > Maximum Waiting Time");
	}
	param.priority_wait = (param.min_wait_time + param.max_wait_time) / 2;

	//---- max departure time variance ----

	Print (1);
	param.max_start_variance = Get_Control_Time (MAX_DEPARTURE_TIME_VARIANCE);

	//---- max arrival time variance ----

	param.max_end_variance = Get_Control_Time (MAX_ARRIVAL_TIME_VARIANCE);

	//---- problem related flags ----

	if (Check_Control_Key (RELOAD_CAPACITY_PROBLEMS) ||
		Check_Control_Key (COUNT_PROBLEM_WARNINGS) || 
		Check_Control_Key (PRINT_PROBLEM_MESSAGES)) {
		Print (1);
	}

	//---- reload capacity problems ----

	param.reload_problems = Get_Control_Flag (RELOAD_CAPACITY_PROBLEMS);
	
	//---- count problem warnings ----

	param.count_warnings = Get_Control_Flag (COUNT_PROBLEM_WARNINGS);

	if (select_problems && param.count_warnings) {
		for (i=1; i < MAX_PROBLEM; i++) {
			if (i != WAIT_PROBLEM && i != ARRIVAL_PROBLEM && i != DEPARTURE_PROBLEM) {
				if (problem_range.In_Range (i)) {
					param.count_warnings = false;
					break;
				}
			}
		}
	}

	//---- print problem messages ----

	param.print_problems = Get_Control_Flag (PRINT_PROBLEM_MESSAGES);

	//---- simulation methods ----

	Print (1);

	if (Check_Control_Key (UNSIMULATED_SUBAREAS) || 
		Check_Control_Key (MACROSCOPIC_SUBAREAS) || 
		Check_Control_Key (MICROSCOPIC_SUBAREAS)) {

		//---- unsimulated subareas ----

		key = Get_Control_Text (UNSIMULATED_SUBAREAS);

		if (!key.empty ()) {
			no_range.Add_Ranges (key);
		}

		//---- macroscopic subareas ----

		key = Get_Control_Text (MACROSCOPIC_SUBAREAS);

		if (!key.empty ()) {
			macro_range.Add_Ranges (key);
		}

		//---- mesoscopic subareas ----

		key = Get_Control_Text (MESOSCOPIC_SUBAREAS);

		if (!key.empty () && Check_Control_Key (MESOSCOPIC_SUBAREAS)) {
			meso_range.Add_Ranges (key);
		}

		//---- microscopic subareas ----

		key = Get_Control_Text (MICROSCOPIC_SUBAREAS);

		if (!key.empty ()) {
			micro_range.Add_Ranges (key);
		}

		//---- check the subarea definitions ----

		if (no_range.size () == 0 && macro_range.size () == 0 && meso_range.size () == 0 && micro_range.size () == 0) {
			meso_range.Add_Ranges ("ALL");
		}

	} else {

		//---- default subareas ----

		key = sim->Get_Control_Text (MESOSCOPIC_SUBAREAS);

		if (!key.empty ()) {
			meso_range.Add_Ranges (key);
		}
	}

	//---- macroscopic capacity factors ----

	if (macro_range.size () > 0) {
		if (Check_Control_Key (TURN_POCKET_FACTOR) || 
			Check_Control_Key (MERGE_POCKET_FACTOR) || 
			Check_Control_Key (OTHER_POCKET_FACTOR)) {
			Print (1);
		}
		param.turn_factor = Get_Control_Double (TURN_POCKET_FACTOR);
		param.merge_factor = Get_Control_Double (MERGE_POCKET_FACTOR);
		param.other_factor = Get_Control_Double (OTHER_POCKET_FACTOR);
	}

	//---- estimated number of travelers -----

	Print (1);
	num_travelers = Get_Control_Integer (NUMBER_OF_TRAVELERS);

	//---- read the control keys ----

	problem_output.Read_Control ();
	sim_output_step.Read_Controls ();
}
