//*********************************************************
//	Control.cpp - simulation service control key processing
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Simulator_Service::Program_Control (void)
{
	int i, j, num, type, num_periods;
	String key;
	Strings strings;
	Str_Itr str_itr;
	Double_List list;
	Dbl_Itr dbl_itr;
	Simulation_Group group;
	Sim_Group_Itr group_itr;
	bool method_flag [MICROSCOPIC + 1];
	Dtime step;

	//---- set / check the signal files ----

	num = Highest_Control_Group (GROUP_PERIOD_METHODS, 0);

	if (num == 0) {
		param.control_flag = true;
	} else {
		param.control_flag = false;

		for (i=1; i <= num; i++) {
			if (!Check_Control_Key (GROUP_PERIOD_METHODS, i)) continue;

			key = Get_Control_String (GROUP_PERIOD_METHODS, i);
			key.Parse (strings);

			for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
				type = Simulation_Code (*str_itr);
				if (type == MESOSCOPIC || type == MICROSCOPIC) {
					param.control_flag = true;
					break;
				}
			}
			if (param.control_flag) break;
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

	//---- simulation time breaks ----

	key = Get_Control_Text (SIMULATION_TIME_BREAKS);

	if (!key.empty ()) {
		sim_periods.Add_Breaks (key);
	} else {
		sim_periods.Add_Breaks ("NONE");
	}
	num_periods = sim_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Print (1, "Number of Simulation Time Periods = ") << sim_periods.Num_Periods ();

	//---- process the simulation groups ----

	num = MAX (Highest_Control_Group (GROUP_PERIOD_METHODS, 0), 
				Highest_Control_Group (SIMULATION_GROUP_SUBAREAS, 0));

	memset (method_flag, false, sizeof (method_flag));

	if (num == 0) {
		group.group = 0;
		group.methods.assign (1, MESOSCOPIC);
		group.subareas.Add_Ranges ("ALL");
		method_flag [MESOSCOPIC] = true;

		sim_group_array.push_back (group);

		Print (2, "Mesoscopic Simulation for All Subareas and Time Periods");

	} else {

		for (i=1; i <= num; i++) {
			if (!Check_Control_Key (GROUP_PERIOD_METHODS, i) && 
				!Check_Control_Key (SIMULATION_GROUP_SUBAREAS, i)) continue;

			Print (1);
			group.group = i;
			group.methods.assign (num_periods, NO_SIMULATION);
			group.subareas.clear ();

			//---- group subareas ----

			key = Get_Control_Text (SIMULATION_GROUP_SUBAREAS, i);

			group.subareas.Add_Ranges (key);

			//---- period methods ----

			key = Get_Control_Text (GROUP_PERIOD_METHODS, i);

			if (!key.empty ()) {
				key.Parse (strings);
				type = NO_SIMULATION;

				for (j=0, str_itr = strings.begin (); str_itr != strings.end (); str_itr++, j++) {
					type = Simulation_Code (*str_itr);
					group.methods [j] = type;
					method_flag [type] = true;
				}
				for (; j < num_periods; j++) {
					group.methods [j] = type;
				}
			} else {
				for (j=0; j < num_periods; j++) {
					group.methods [j] = MESOSCOPIC;
				}
				method_flag [MESOSCOPIC] = true;
			}
			sim_group_array.push_back (group);
		}
	}

	//---- time steps per second ----

	if (Check_Control_Key (UNSIMULATED_TIME_STEPS) || Check_Control_Key (MACROSCOPIC_TIME_STEPS) ||
		Check_Control_Key (MESOSCOPIC_TIME_STEPS) || Check_Control_Key (MICROSCOPIC_TIME_STEPS)) {
		Print (1);
	}
	if (method_flag [NO_SIMULATION]) {
		method_time_step [NO_SIMULATION] = Get_Control_Time (UNSIMULATED_TIME_STEPS);
	}
	if (method_flag [MACROSCOPIC]) {
		method_time_step [MACROSCOPIC] = Get_Control_Time (MACROSCOPIC_TIME_STEPS);
	}
	if (method_flag [MESOSCOPIC]) {
		method_time_step [MESOSCOPIC] = Get_Control_Time (MESOSCOPIC_TIME_STEPS);
	}
	if (method_flag [MICROSCOPIC]) {
		method_time_step [MICROSCOPIC] = Get_Control_Time (MICROSCOPIC_TIME_STEPS);
	}
	period_step_size.assign (num_periods, Dtime (900, SECONDS));

	for (group_itr = sim_group_array.begin (); group_itr != sim_group_array.end (); group_itr++) {
		for (j=0; j < num_periods; j++) {
			step = method_time_step [group_itr->methods [j]];

			if (step < period_step_size [j]) {
				period_step_size [j] = step;
			}
		}
	}
	param.step_size = period_step_size [0];

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

	Get_Control_List_Groups (MAX_COMFORTABLE_SPEED, list, true);
	
	for (dbl_itr = list.begin (); dbl_itr != list.end (); dbl_itr++) {
		param.comfort_speed.push_back (Round (*dbl_itr));
		if (*dbl_itr != 0.0) {
			param.comfort_flag = true;
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

	//---- estimated number of travelers -----

	Print (1);
	num_travelers = Get_Control_Integer (NUMBER_OF_TRAVELERS);

	//---- read the control keys ----

	sim_output_step.Read_Controls ();
}
