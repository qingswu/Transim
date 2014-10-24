//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Simulator.hpp"

#include "Plan_Processor.hpp"	

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Simulator::Execute (void)
{
	int i, count;
	bool first, read_status;

	Sim_Statistics *stats;

	clock_t start, input_total, update_total, travel_total, output_total, node_total;

	//---- read the network data ----

	Simulator_Service::Execute ();

#ifdef ROUTING

	//---- read trips into memory ----

	Read_Trips ();

	//---- build paths ----

	Show_Message ("Build Plans");
	Set_Progress ();

	bool build_flag, first_iteration = true;
	int last_hhold = -1;
	Trip_Map_Itr map_itr;
	Plan_Ptr plan_ptr;
	Plan_Ptr_Array *plan_ptr_array;
	Plan_Processor plan_processor;

	plan_processor.Initialize (this);

	plan_processor.Start_Processing ();
	plan_ptr_array = new Plan_Ptr_Array ();

	//---- process each trip ----
	
	for (map_itr = plan_trip_map.begin (); map_itr != plan_trip_map.end (); map_itr++) {
	//for (map_itr = trip_map.begin (); map_itr != trip_map.end (); map_itr++) {
		Show_Progress ();

		plan_ptr = new Plan_Data ();

		*plan_ptr = plan_array [map_itr->second];
		//Trip_Data *trip_ptr = &trip_array [map_itr->second];
		//*plan_ptr = *trip_ptr;

		//---- check the household id ----

		if (plan_ptr->Household () < 1) continue;

		if (plan_ptr->Household () != last_hhold) {
			if (last_hhold > 0 && plan_ptr_array->size () > 0) {
				plan_processor.Plan_Build (plan_ptr_array);
				plan_ptr_array = new Plan_Ptr_Array ();
			}
			last_hhold = plan_ptr->Household ();
		}

		//---- update the selection priority flag ----

		if (plan_ptr->Priority () == NO_PRIORITY) {
			plan_ptr->Method (UPDATE_PLAN);
		} else if (!first_iteration && select_priorities) {
			build_flag = select_priority [plan_ptr->Priority ()];

			//if (build_flag && max_percent_flag && percent_selected < 1.0) {
			//	build_flag = (random_select.Probability () <= percent_selected);
			//}
			if (build_flag) {
				plan_ptr->Method (BUILD_PATH);
			} else {
				plan_ptr->Method (UPDATE_PLAN);
			}
		} else {
			plan_ptr->Method (BUILD_PATH);
		}
		plan_ptr_array->push_back (plan_ptr);
	}

	//---- process the last household ----

	if (last_hhold > 0 && plan_ptr_array->size () > 0) {
		plan_processor.Plan_Build (plan_ptr_array);
	} else {
		delete plan_ptr_array;
	}
	plan_processor.Stop_Processing ();
	End_Progress ();
#endif

	//---- initialize the global data structures ----

	Show_Message ("Initializing the Simulator");

	Global_Data ();

	time_step = param.start_time_step;
	sim_period = sim_periods.Period (time_step) - 1;
	end_period = 0;
		
	//---- create simulation partitions ---

	if (!sim_plan_step.First_Plan ()) {
		Error ("Reading Plan File");
	}

	Break_Check (3);
	Print (2, "Simulation Started at Time ") << time_step.Time_String ();

	read_status = first = true;
	input_total = update_total = node_total = output_total = travel_total = 0;

#ifdef ROUTING
	Show_Message (2, "Plan Processing -- Trip");
#else
	if (read_all_flag) {
		Show_Message (2, "Reading Plans into Memory -- Trip");
	} else {
		Show_Message (2, "Processing Time of Day");
	}
#endif
	Set_Progress ();

	//---- process each time step ----

	for (; time_step <= param.end_time_step; time_step += param.step_size) {
		Show_Progress (time_step.Time_String ());

		if (time_step >= end_period) {
			end_period = Set_Sim_Period ();
		}
		Step_Code (time_step);
		io_flag = ((time_step % one_second) == 0);

		if (random_time > 0) {
			random_node_flag = ((time_step & random_time) == 0 && time_step > 0);
		} else {
			random_node_flag = false;
		}
		for (i=0; i <= MICROSCOPIC; i++) {
			if (method_time_step [i] > 0) {
				method_time_flag [i] = ((time_step % method_time_step [i]) == 0);
			} else {
				method_time_flag [i] = false;
			}
		}

		//---- processing for each second ----

		if (io_flag) {

			//---- check for output ----

			start = clock ();
			sim_output_step.Start_Processing ();
			output_total += (clock () - start);

			//---- update the network ----

			start = clock ();
			sim_update_step.Start_Processing ();
			update_total += (clock () - start);

			//---- read plans ----

			if (read_status) {

				start = clock ();
				read_status = sim_plan_step.Start_Processing ();
				input_total += (clock () - start);

				if (read_all_flag) {
					End_Progress ();
					Show_Message (1, "Processing Time of Day");
					Set_Progress ();
				}
			}
		}
		Active (false);

		//---- process the travelers ----

		start = clock ();
		sim_travel_step.Start_Processing ();
		travel_total += (clock () - start);

		//---- process the network traffic ----

		start = clock ();
		sim_node_step.Start_Processing ();
		node_total += (clock () - start);

		if (Num_Vehicles () > max_vehicles) {
			max_vehicles = Num_Vehicles ();
			max_time = time_step;
		}
		if (!Active () && !read_status) break;
	}
	if (Master ()) End_Progress (time_step.Time_String ());

	stats = Stop_Simulation ();

	Print (1, "Simulation Ended at Time ") << time_step.Time_String ();

	start = input_total + output_total + update_total + travel_total + node_total;
	if (start == 0) start = 1;

	Break_Check (6);
	Write (2, String ("Seconds in Input Processing = %.1lf (%.1lf%%)") % 
		((double) input_total / CLOCKS_PER_SEC) % (100.0 * input_total / start) % FINISH);
	Write (1, String ("Seconds in Output Processing = %.1lf (%.1lf%%)") % 
		((double) output_total / CLOCKS_PER_SEC) % (100.0 * output_total / start) % FINISH);
	Write (1, String ("Seconds in Update Processing = %.1lf (%.1lf%%)") % 
		((double) update_total / CLOCKS_PER_SEC) % (100.0 * update_total / start) % FINISH);
	Write (1, String ("Seconds in Travel Processing = %.1lf (%.1lf%%)") % 
		((double) travel_total / CLOCKS_PER_SEC) % (100.0 * travel_total / start) % FINISH);
	Write (1, String ("Seconds in Network Processing = %.1lf (%.1lf%%)") % 
		((double) node_total / CLOCKS_PER_SEC) % (100.0 * node_total / start) % FINISH);

	//---- write summary statistics ----
#ifndef ROUTING
	plan_file->Print_Summary ();
#endif
	Break_Check (4);
	Write (2, "Number of Person Trips Processed = ") << stats->num_trips;
	Write (1, "Number of Person Trips Started   = ") << stats->num_start;
	if (stats->num_trips > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_start / stats->num_trips) % FINISH);
	Write (1, "Number of Person Trips Completed = ") << stats->num_end;
	if (stats->num_trips > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_end / stats->num_trips) % FINISH);

	if (stats->num_veh_trips > 0) {
		Break_Check (7);
		Write (2, "Number of Vehicle Trips Processed = ") << stats->num_veh_trips;
		Write (1, "Number of Vehicle Trips Started   = ") << stats->num_veh_start;
		if (stats->num_veh_trips > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_veh_start / stats->num_veh_trips) % FINISH);
		Write (1, "Number of Vehicle Trips Completed = ") << stats->num_veh_end;
		if (stats->num_veh_trips > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_veh_end / stats->num_veh_trips) % FINISH);
		Write (1, "Number of Vehicle Trips Removed   = ") << stats->num_veh_lost;
		if (stats->num_veh_trips > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_veh_lost / stats->num_veh_trips) % FINISH);

		if (stats->num_veh_end > 0) {
			Print (2, String ("Total Hours for Completed Vehicle Trips = %.1lf hours") % (stats->tot_hours / one_hour));
			Print (1, String ("Average Travel Time for Completed Trips = %.2lf minutes") % ((stats->tot_hours / stats->num_veh_end) / one_minute));
		}
	}
	if (param.transit_flag) {
		Break_Check (5);
		Write (2, "Number of Transit Runs Processed = ") << stats->num_runs;
		Write (1, "Number of Transit Runs Started   = ") << stats->num_run_start;
		if (stats->num_runs > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_run_start / stats->num_runs) % FINISH);
		Write (1, "Number of Transit Runs Completed = ") << stats->num_run_end;
		if (stats->num_runs > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_run_end / stats->num_runs) % FINISH);
		Write (1, "Number of Transit Runs Removed   = ") << stats->num_run_lost;
		if (stats->num_runs > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_run_lost / stats->num_runs) % FINISH);

		Break_Check (4);
		Write (2, "Number of Transit Legs Processed = ") << stats->num_transit;
		Write (1, "Number of Transit Legs Started   = ") << stats->num_board;
		if (stats->num_transit > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_board / stats->num_transit) % FINISH);
		Write (1, "Number of Transit Legs Completed = ") << stats->num_alight;
		if (stats->num_transit > 0) Write (0, String (" (%.1lf%%)") % (100.0 * stats->num_alight / stats->num_transit) % FINISH);
	}

	count = 0;
	if (stats->num_change > 0) count++;
	if (stats->num_swap > 0) count++;
	if (stats->num_look_ahead > 0) count++;
	if (stats->num_slow_down > 0) count++;

	if (count > 0) {
		Break_Check (count + 1);
		Print (1);
		if (stats->num_change > 0) Print (1, "Number of Required Lane Changes = ") << stats->num_change;
		if (stats->num_swap > 0) Print (1, "Number of Swapping Lane Changes = ") << stats->num_swap;
		if (stats->num_look_ahead > 0) Print (1, "Number of Look-Ahead Lane Changes = ") << stats->num_look_ahead;
		if (stats->num_slow_down > 0) Print (1, "Number of Random Slow Downs = ") << stats->num_slow_down;
	}
	Print (2, "Maximum Number of Vehicles on the Network = ") << max_vehicles << " at " << max_time.Time_String ();

	//---- print the problem report ----

	if (traveler_array.size () > 0) {
		Sim_Travel_Itr sim_travel_itr;
		int num_problem = 0;

		for (sim_travel_itr = sim_travel_array.begin (); sim_travel_itr != sim_travel_array.end (); sim_travel_itr++) {
			if (sim_travel_itr->Problem ()) num_problem++;
		}

		if (num_problem) {
			Write (2, String ("Number of Travelers with Problems = %d (%.1lf%%)") % 
				num_problem % (100.0 * num_problem / sim_travel_array.size ()) % FINISH);
		}
	}

	//---- end the program ----

	Report_Problems ();

	Exit_Stat (DONE);
}
