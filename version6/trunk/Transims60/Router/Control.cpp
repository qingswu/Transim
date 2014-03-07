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

	if (!Set_Control_Flag (UPDATE_FLOW_RATES) && !Set_Control_Flag (UPDATE_TRAVEL_TIMES) && Check_Control_Key (NEW_LINK_DELAY)) {
		Warning ("Flow or Time Updates are required to output a New Link Delay File");
		System_File_False (NEW_LINK_DELAY);
	}

	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- create the network files ----

	Router_Service::Program_Control ();

	Read_Select_Keys ();

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
		//Path_Parameters param;
		//Set_Parameters (param);

		trip_flag = true;
		trip_file = (Trip_File *) System_File_Handle (TRIP);
		if (trip_file->Time_Sort ()) {
			Error ("Trip Files should not be Time Sorted");
		}
		trip_set_flag = (trip_file->Part_Flag ());

		Required_File_Check (*trip_file, LOCATION);
	}
	if (System_File_Flag (NEW_PLAN)) {
		new_plan_flag = true;
		new_plan_file = (Plan_File *) System_File_Handle (NEW_PLAN);

		if (trip_flag) {
			new_plan_file->Sort_Type (trip_file->Sort_Type ());
		}
		if (System_File_Flag (PLAN)) {
			plan_flag = true;
			plan_file = (Plan_File *) System_File_Handle (PLAN);

			if (trip_flag) {
				if (plan_file->Time_Sort ()) {
					Error ("Plan Files should not be Time Sorted");
				}
			} else {
				new_plan_file->Sort_Type (plan_file->Sort_Type ());
			}
		}
		new_plan_file->Update_Def_Header ();

		//---- check for single input to partitioned output ----

		if (trip_flag) {
			if (!Single_Partition ()) {
				plan_set_flag = new_plan_file->Part_Flag ();
				if (plan_set_flag && !select_flag && !trip_set_flag) {
					Error ("A Selection File is required to Partition the Trips");
				}
				if (plan_flag) {
					old_plan_flag = (!trip_set_flag && plan_file->Part_Flag ());
				}
			}
		} else if (!plan_flag) {
			Error ("An Input Trip or Plan File is Required");
		} else if (Num_Threads () > 1 && !Single_Partition ()) {
			plan_set_flag = (plan_file->Part_Flag () && new_plan_file->Part_Flag ());
		}
	}
	if (System_File_Flag (NEW_PROBLEM)) {
		problem_flag = true;
		problem_file = (Problem_File *) System_File_Handle (NEW_PROBLEM);
		problem_file->Router_Data ();

		if (plan_set_flag) {
			problem_set_flag = (problem_file->Part_Flag ());
		}
	}

	Print (2, String ("%s Control Keys:") % Program ());

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
	} else if (Set_Control_Flag (REROUTE_FROM_TIME_POINT)) {
		Error ("Re-Routing Required an Input and Output Plan File");
	}
	if (!plan_flag && !trip_flag) {
		Error ("Router requires an input Trip or Plan file");
	}

	if (select_mode [DRIVE_MODE] || select_mode [PNR_IN_MODE] || select_mode [PNR_OUT_MODE] ||
		select_mode [KNR_IN_MODE] || select_mode [KNR_OUT_MODE] || select_mode [TAXI_MODE]) {
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
		//if (Clear_Flow_Flag () && System_File_Flag (LINK_DELAY)) {
		//	Error ("Clear Input Flow Rates should be False for Iterative Processing");
		//}
		iteration_flag = true;

		//---- link gap criteria ----

		link_gap = Get_Control_Double (LINK_CONVERGENCE_CRITERIA);
		
		//---- trip gap criteria ----

		trip_gap = Get_Control_Double (TRIP_CONVERGENCE_CRITERIA);
		
		//---- transit gap criteria ----

		transit_gap = Get_Control_Double (TRANSIT_CAPACITY_CRITERIA);

		//---- initial weighting factor ----

		factor = Get_Control_Double (INITIAL_WEIGHTING_FACTOR);
		
		//---- iteration weighting increment ----

		increment = Get_Control_Double (ITERATION_WEIGHTING_INCREMENT);

		//---- maximum weighting factor ----

		max_factor = Get_Control_Double (MAXIMUM_WEIGHTING_FACTOR);

		//---- minimize vehicle hours ----
		
		min_vht_flag = Get_Control_Flag (MINIMIZE_VEHICLE_HOURS);

		if (min_vht_flag) {
			if (Check_Control_Key (INITIAL_WEIGHTING_FACTOR) ||
				Check_Control_Key (ITERATION_WEIGHTING_INCREMENT) ||
				Check_Control_Key (MAXIMUM_WEIGHTING_FACTOR)) {

				Warning ("Minimizing Vehicle Hours and Iteration Weighting Factors are Incompatible");
			}
			factor = 1.0;
		}

		//---- save after each iteration ----

		save_flag = Get_Control_Flag (SAVE_AFTER_EACH_ITERATION);

		//---- new link convergence file ----

		key = Get_Control_String (NEW_LINK_CONVERGENCE_FILE);

		if (!key.empty ()) {
			Print (1);
			link_gap_file.File_Type ("New Link Convergence File");
			link_gap_file.Create (Project_Filename (key));
			link_gap_flag = true;
		}

		//---- new trip convergence file ----

		key = Get_Control_String (NEW_TRIP_CONVERGENCE_FILE);

		if (!key.empty ()) {
			Print (1);
			trip_gap_file.File_Type ("New Trip Convergence File");
			trip_gap_file.Create (Project_Filename (key));
			trip_gap_flag = true;
		}
		trip_gap_map_flag = (trip_gap_flag || trip_gap > 0.0 || Report_Flag (TRIP_GAP));
	} else {
		trip_gap_map_flag = plan_flag;
	}

	//---- link person file ----

	key = Get_Control_String (LINK_PERSON_FILE);

	if (!key.empty ()) {
		Print (1);
		old_person_file.File_Type ("Link Person File");

		if (Check_Control_Key (LINK_PERSON_FORMAT)) {
			old_person_file.Dbase_Format (Get_Control_String (LINK_PERSON_FORMAT));
		}
		old_person_file.Open (Project_Filename (key));
		old_person_flag = true;
	}

	//---- link vehicle file ----

	key = Get_Control_String (LINK_VEHICLE_FILE);

	if (!key.empty ()) {
		Print (1);
		old_vehicle_file.File_Type ("Link Vehicle File");

		if (Check_Control_Key (LINK_VEHICLE_FORMAT)) {
			old_vehicle_file.Dbase_Format (Get_Control_String (LINK_VEHICLE_FORMAT));
		}
		old_vehicle_file.Open (Project_Filename (key));
		old_vehicle_flag = true;
	}

	//---- new link person file ----

	key = Get_Control_String (NEW_LINK_PERSON_FILE);

	if (!key.empty ()) {
		Print (1);
		link_person_file.File_Type ("New Link Person File");

		if (Check_Control_Key (NEW_LINK_PERSON_FORMAT)) {
			link_person_file.Dbase_Format (Get_Control_String (NEW_LINK_PERSON_FORMAT));
		}
		link_person_file.Flow_Units (PERSONS);
		link_person_file.Turn_Flag (false);
		link_person_file.Create (Project_Filename (key));
		link_person_flag = true;
	}

	//---- new link vehicle file ----

	key = Get_Control_String (NEW_LINK_VEHICLE_FILE);

	if (!key.empty ()) {
		Print (1);
		link_vehicle_file.File_Type ("New Link Vehicle File");

		if (Check_Control_Key (NEW_LINK_VEHICLE_FORMAT)) {
			link_vehicle_file.Dbase_Format (Get_Control_String (NEW_LINK_VEHICLE_FORMAT));
		}
		link_vehicle_file.Flow_Units (VEHICLES);
		link_vehicle_file.Turn_Flag (false);
		link_vehicle_file.Create (Project_Filename (key));
		link_vehicle_flag = true;
	}

	//---- set the output link delay format ----

	if (System_File_Flag (NEW_LINK_DELAY) || Flow_Updates () || Turn_Updates ()) {
		Link_Flows (Flow_Updates ());
		flow_flag = true;

		if (!Turn_Updates ()) {
			Turn_Flows (false);
			turn_flag = false;	
		} else {
			Turn_Flows (true);
			turn_flag = true;
		}

		if (System_File_Flag (NEW_LINK_DELAY) && System_File_Flag (VEHICLE_TYPE)) {
			Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (NEW_LINK_DELAY);

			file->Clear_Fields ();
			file->Flow_Units (PCE);
			file->Turn_Flag (turn_flag);
			file->Create_Fields ();
			file->Write_Header ();
		}
	}
}
