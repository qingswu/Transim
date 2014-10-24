//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Router::Program_Control (void)
{
	String key;

	if (!Set_Control_Flag (UPDATE_FLOW_RATES) && !Set_Control_Flag (UPDATE_TRAVEL_TIMES) && System_Control_Empty (NEW_PERFORMANCE)) {
		Warning ("Flow or Time Updates are required to output a New Performance File");
		System_File_False (NEW_PERFORMANCE);
	}
	if (System_Control_Empty  (TRANSIT_STOP) || System_Control_Empty (TRANSIT_ROUTE) || System_Control_Empty (TRANSIT_SCHEDULE)) {
		Transit_Paths (false);
		ParkRide_Paths (false);
		KissRide_Paths (false);
	}

	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- create the network files ----

	Router_Service::Program_Control ();

	Read_Select_Keys ();

	if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) || !System_File_Flag (TRANSIT_SCHEDULE)) {
		select_modes = true;
		select_mode [TRANSIT_MODE] = false;
		select_mode [PNR_IN_MODE] = false;
		select_mode [PNR_OUT_MODE] = false;
		select_mode [KNR_IN_MODE] = false;
		select_mode [KNR_OUT_MODE] = false;
	}
	if (select_mode [DRIVE_MODE] || select_mode [PNR_IN_MODE] || select_mode [PNR_OUT_MODE] ||
		select_mode [KNR_IN_MODE] || select_mode [KNR_OUT_MODE] || select_mode [TAXI_MODE]) {
		Drive_Paths (true);
	}
										
	Print (2, String ("%s Control Keys:") % Program ());

	//---- application type ----

	key = Get_Control_String (APPLICATION_METHOD);
	method = Router_Method_Code (key);

	Print (1);
	Output_Control_Key (APPLICATION_METHOD, Router_Method_Code (method));

	//---- store trips in memory ----

	trip_memory_flag = Get_Control_Flag (STORE_TRIPS_IN_MEMORY);

	if (!trip_memory_flag) {
		System_Read_False (TRIP);
		System_Data_Reserve (TRIP, 0);
	}

	//---- store plans in memory ----

	plan_memory_flag = Get_Control_Flag (STORE_PLANS_IN_MEMORY);

	if (plan_memory_flag) {
		time_sort_flag = (Trip_Sort () == TIME_SORT);
		Trip_Sort (TRAVELER_SORT);

		if (trip_memory_flag) {
			System_Data_Reserve (TRIP, 0);
		}
	} else {
		System_Read_False (PLAN);
		System_Data_Reserve (PLAN, 0);

		if (Trip_Sort () == TIME_SORT) {
			Error ("Time Sorting Requires Plans in Memory");
		}
	}

	if (!trip_memory_flag || !plan_memory_flag) {
		if (method == DUE_PLANS) {
			Error ("Dynamic User Equilibrium Requires Trips and Plans in Memory");
		}
	} else {
		Memory_Flag (true);
		System_Read_False (TRIP);
		System_Data_Reserve (TRIP, 0);
	}

	//---- check file and selection keys ----

	if (percent_flag) {
		random_select.Seed (random.Seed () + 1);
	}
	select_flag = System_File_Flag (SELECTION);
	rider_flag = System_File_Flag (NEW_RIDERSHIP);

	if (rider_flag || System_File_Flag (RIDERSHIP)) {
		if (!System_File_Flag (TRANSIT_ROUTE) || !System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_SCHEDULE)) {
			Error ("Transit Network Files are Required for Ridership Summaries");
		}
	}

	if (System_File_Flag (TRIP)) {
		trip_flag = true;
		trip_file = System_Trip_File ();

		if (trip_file->Version () < 60) {
			Error (String ("Convert Version %.1lf Trip File to Version 6.0 using NewFormat") % (trip_file->Version () / 10.0));
		}
		if (trip_file->Time_Sort () && !trip_memory_flag) {
			Error ("Trip Files should be Traveler Sorted");
		}
		trip_set_flag = (trip_file->Part_Flag ());

		Required_File_Check (*trip_file, LOCATION);
	}
	if (System_File_Flag (PLAN)) {
		plan_flag = true;
		plan_file = System_Plan_File ();

		if (plan_file->Time_Sort () && !plan_memory_flag) {
			Error ("Plan Files should be Traveler Sorted");
		}
	}
	if (!trip_flag && !plan_flag) {
		Error ("An Input Trip or Plan File is Required");
	}

	if (System_File_Flag (NEW_PLAN)) {
		new_plan_flag = true;
		new_plan_file = System_Plan_File (true);

		if (time_sort_flag) {
			new_plan_file->Sort_Type (TIME_SORT);
		} else if (trip_flag) {
			new_plan_file->Sort_Type (trip_file->Sort_Type ());
		} else {
			new_plan_file->Sort_Type (plan_file->Sort_Type ());
		}
		new_plan_file->Update_Def_Header ();

		//---- check for single input to partitioned output ----

		if (trip_flag) {
			if (!Single_Partition ()) {
				new_set_flag = new_plan_file->Part_Flag ();
				if (new_set_flag && !select_flag && !trip_set_flag) {
					Error ("A Selection File is required to Partition the Trips");
				}
				if (plan_flag) {
					plan_set_flag = (!trip_set_flag && plan_file->Part_Flag ());
				}
			}
		} else if (Num_Threads () > 1 && !Single_Partition ()) {
			new_set_flag = (plan_file->Part_Flag () && new_plan_file->Part_Flag ());
		}
		if (method == DTA_FLOWS) {
			Warning ("New Plans are not Consistent with DTA Flows");
		}
	} else if (method == DUE_PLANS) {
		Error ("A New Plans File is Required for Dynamic User Equilibrium");
	}
	if (System_File_Flag (NEW_PROBLEM)) {
		problem_flag = true;
		problem_file = System_Problem_File (true);
		problem_file->Router_Data ();

		if (new_set_flag) {
			problem_set_flag = (problem_file->Part_Flag ());
		}
	}
	if (method == DTA_FLOWS && !System_File_Flag (NEW_PERFORMANCE)) {
		Error ("A New Performance File is Required for Dynamic Traffic Assignment");
	}

	//---- intialize trip priority ----

	if (Check_Control_Key (INITIALIZE_TRIP_PRIORITY)) {
		key = Get_Control_Text (INITIALIZE_TRIP_PRIORITY);

		if (!key.empty () && !key.Equals ("NO")) {
			priority_flag = true;
			initial_priority = Priority_Code (key);
		} else {
			priority_flag = false;
		}
	} else {
		priority_flag = true;
		initial_priority = CRITICAL;
	}

	if (new_plan_flag && plan_flag) {
		if (script_flag) Print (1);

		update_flag = Get_Control_Flag (UPDATE_PLAN_RECORDS);

		reroute_time = Get_Control_Time (REROUTE_FROM_TIME_POINT);
		reroute_flag = (reroute_time > 0);

		if (update_flag && reroute_flag) {
			Warning ("Re-Routing and Updating Plans are Mutually Exclusive");
			update_flag = false;
		}
		if (update_flag || reroute_flag) {
			warn_flag = Get_Control_Flag (PRINT_UPDATE_WARNINGS);
			Print (1);
		}
	} else if (Get_Control_Time (REROUTE_FROM_TIME_POINT) > 0) {
		Error ("Re-Routing Required an Input and Output Plan File");
	}

	//---- maximum number of iterations ----

	Print (1);
	max_iteration = Get_Control_Integer (MAXIMUM_NUMBER_OF_ITERATIONS);

	if (max_iteration > 1) {
		
		if (!trip_flag) {
			Error ("Iterative Processing Requires a Trip File");
		}
		if (!Flow_Updates () || !Time_Updates ()) {
			Error ("Iterative Processing Requires Flow and Travel Time Updates");
		}
		if (method == TRAVEL_PLANS) {
			Warning ("Application Method Upgraded to Dynamic User Equilibrium");
			method = DUE_PLANS;
		} else if (method == LINK_FLOWS) {
			Warning ("Application Method Upgraded to Dynamic Traffic Assignment");
			method = DTA_FLOWS;
		}
		iteration_flag = true;

		//---- link gap criteria ----

		link_gap = Get_Control_Double (LINK_CONVERGENCE_CRITERIA);
		
		//---- trip gap criteria ----

		trip_gap = Get_Control_Double (TRIP_CONVERGENCE_CRITERIA);
		
		//---- transit gap criteria ----

		transit_gap = Get_Control_Double (TRANSIT_CAPACITY_CRITERIA);

		//---- method specific keys ----

		if (method == DTA_FLOWS) {

			//---- initial weighting factor ----

			factor = Get_Control_Double (INITIAL_WEIGHTING_FACTOR);
		
			//---- iteration weighting increment ----

			increment = Get_Control_Double (ITERATION_WEIGHTING_INCREMENT);

			//---- maximum weighting factor ----

			max_factor = Get_Control_Double (MAXIMUM_WEIGHTING_FACTOR);

			//---- minimize vehicle hours ----
		
			min_vht_flag = Get_Control_Flag (MINIMIZE_VEHICLE_HOURS);

			if (min_vht_flag) {
				if ((Check_Control_Key (INITIAL_WEIGHTING_FACTOR) && factor > 0) ||
					(Check_Control_Key (ITERATION_WEIGHTING_INCREMENT) && increment > 0) ||
					(Check_Control_Key (MAXIMUM_WEIGHTING_FACTOR) && max_factor > 0)) {

					Warning ("Minimizing Vehicle Hours and Iteration Weighting Factors are Incompatible");
				}
				factor = 1.0;
			}

		} else if (method == DUE_PLANS) {

			//---- maximum number fo reskim iterations ----

			max_speed_updates = Get_Control_Integer (MAXIMUM_RESKIM_ITERATIONS);

			//---- reskim converence criteria ----

			min_speed_diff = Get_Control_Double (RESKIM_CONVERGENCE_CRITERIA);
		}

		//---- save after iterations ----

		if (Check_Control_Key (SAVE_AFTER_ITERATIONS)) {
			key = Get_Control_Text (SAVE_AFTER_ITERATIONS);

			if (!key.empty () && !key.Equals ("NONE")) {
				if (key.Equals ("ALL")) {
					key ("%d..%d") % 1 % max_iteration;
				}
				save_iter_flag = true;
				if (!save_iter_range.Add_Ranges (key)) {
					Error ("Adding Iteration Ranges");
				}
			}
		}

		//---- new link convergence file ----

		key = Get_Control_String (NEW_LINK_CONVERGENCE_FILE);

		if (!key.empty ()) {
			Print (1);
			link_gap_file.File_Type ("New Link Convergence File");
			link_gap_file.Create (Project_Filename (key));
			link_gap_flag = true;
		}
		save_link_gap = (link_gap_flag || link_gap > 0.0 || Report_Flag (LINK_GAP));

		//---- new trip convergence file ----

		key = Get_Control_String (NEW_TRIP_CONVERGENCE_FILE);

		if (!key.empty ()) {
			Print (1);
			trip_gap_file.File_Type ("New Trip Convergence File");
			trip_gap_file.Create (Project_Filename (key));
			trip_gap_flag = true;
		}
		save_trip_gap = (trip_gap_flag || trip_gap > 0.0 || Report_Flag (TRIP_GAP));

		trip_gap_map_flag = save_trip_gap;

	} else {

		if (method == DUE_PLANS) {
			Error ("Dynamic User Equilibrium Requires Multiple Iterations");
		} else if (method == DTA_FLOWS) {
			Error ("Dynamic Traffic Assignment Requires Multiple Iterations");
		}
		trip_gap_map_flag = plan_flag;
	}

	//---- output all records ----

	full_flag = Get_Control_Flag (OUTPUT_ALL_RECORDS);

	//---- preload transit vehicles ----

	preload_flag = Get_Control_Flag (PRELOAD_TRANSIT_VEHICLES);

	if (preload_flag && 
		(!System_File_Flag (TRANSIT_ROUTE) || !System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_SCHEDULE) || !System_File_Flag (TRANSIT_DRIVER))) {

		Warning ("Transit Network Files are required to Preload Transit Vehicles");
		Show_Message (1);
		preload_flag = false;
	}
	if (preload_flag && !System_File_Flag (NEW_PERFORMANCE)) {
		Warning ("A New Performance File is required to Preload Transit Vehicles");
		Show_Message (1);
		preload_flag = false;
	}

	//---- list reports ----

	List_Reports ();

	//---- set the output link delay format ----

	if (System_File_Flag (NEW_PERFORMANCE) || Flow_Updates ()) {
		Link_Flows (Flow_Updates ());
		flow_flag = true;
	}
	if (System_File_Flag (NEW_TURN_DELAY) || Turn_Updates ()) {
		if (!Turn_Updates ()) {
			Turn_Flows (false);
			turn_flag = false;	
		} else {
			Turn_Flows (true);
			turn_flag = true;
		}
	}
}
