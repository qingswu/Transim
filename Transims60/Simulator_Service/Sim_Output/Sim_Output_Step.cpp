//*********************************************************
//	Sim_Output_Step.cpp - simulate output processing
//*********************************************************

#include "Sim_Output_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Output_Step -- constructor
//---------------------------------------------------------

Sim_Output_Step::Sim_Output_Step (void) : Static_Service ()
{
	num_threads = 0;
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Output_Step::Initialize (void)
{
	num_threads = sim->Num_Threads ();
	
	//---- create output threads ----

#ifdef THREADS
	if (num_threads > 1) {
		int num_barrier = (int) output_array.size ();

#ifdef MPI_EXE
		if (sim->problem_output.Output_Flag ()) num_barrier++;
		if (sim->event_output.Output_Flag ()) num_barrier++;
		if (sim->traveler_output.Output_Flag ()) num_barrier++;
#endif
		output_barrier.Num_Barriers (num_barrier);

		Output_Itr output_itr;

		for (output_itr = output_array.begin (); output_itr != output_array.end (); output_itr++) {
			threads.push_back (thread (*(*output_itr)));
		}

		//---- not barrier controlled unless MPI ----

		//if (sim->problem_output.Output_Flag ()) {
		//	threads.push_back (thread (sim->problem_output));
		//}
		//if (sim->event_output.Output_Flag ()) {
		//	threads.push_back (thread (sim->event_output));
		//}
		//if (sim->traveler_output.Output_Flag ()) {
		//	threads.push_back (thread (sim->traveler_output));
		//}
	}
#endif
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Sim_Output_Step::Add_Keys (void)
{
	if (sim == 0 || sim->Service_Level () < SIMULATOR_SERVICE) {
		exe->Error ("Simulation Output requires Simulator Service");
	} else {
		Control_Key snapshot_keys [] = { //--- code, key, level, status, type, default, range, help ----
			{ NEW_SNAPSHOT_FILE, "NEW_SNAPSHOT_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_FORMAT, "NEW_SNAPSHOT_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_SNAPSHOT_TIME_FORMAT, "NEW_SNAPSHOT_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_INCREMENT, "NEW_SNAPSHOT_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "900 seconds", SECOND_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_TIME_RANGE, "NEW_SNAPSHOT_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_LINK_RANGE, "NEW_SNAPSHOT_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_SUBAREA_RANGE, "NEW_SNAPSHOT_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_COORDINATES, "NEW_SNAPSHOT_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_MAX_SIZE, "NEW_SNAPSHOT_MAX_SIZE", LEVEL1, OPT_KEY, INT_KEY, "0", "0..4096", NO_HELP },
			{ NEW_SNAPSHOT_LOCATION_FLAG, "NEW_SNAPSHOT_LOCATION_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_CELL_FLAG, "NEW_SNAPSHOT_CELL_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_STATUS_FLAG, "NEW_SNAPSHOT_STATUS_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			{ NEW_SNAPSHOT_COMPRESSION, "NEW_SNAPSHOT_COMPRESSION", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (snapshot_keys);

		Control_Key link_delay_keys [] = { //--- code, key, level, status, type, help ----
			{ NEW_LINK_DELAY_FILE, "NEW_LINK_DELAY_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_FORMAT, "NEW_LINK_DELAY_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_LINK_DELAY_TIME_FORMAT, "NEW_LINK_DELAY_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_INCREMENT, "NEW_LINK_DELAY_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_TIME_RANGE, "NEW_LINK_DELAY_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_LINK_RANGE, "NEW_LINK_DELAY_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_SUBAREA_RANGE, "NEW_LINK_DELAY_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_COORDINATES, "NEW_LINK_DELAY_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_VEH_TYPES, "NEW_LINK_DELAY_VEH_TYPES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_TURN_FLAG, "NEW_LINK_DELAY_TURN_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_FLOW_TYPE, "NEW_LINK_DELAY_FLOW_TYPE", LEVEL1, OPT_KEY, TEXT_KEY, "VEHICLES", FLOW_RANGE, NO_HELP },
			{ NEW_LINK_DELAY_LANE_FLOWS, "NEW_LINK_DELAY_LANE_FLOWS", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (link_delay_keys);

		Control_Key performance_keys [] = { //--- code, key, level, status, type, default, range, help ----
			{ NEW_PERFORMANCE_FILE, "NEW_PERFORMANCE_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_FORMAT, "NEW_PERFORMANCE_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_PERFORMANCE_TIME_FORMAT, "NEW_PERFORMANCE_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_INCREMENT, "NEW_PERFORMANCE_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_TIME_RANGE, "NEW_PERFORMANCE_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_LINK_RANGE, "NEW_PERFORMANCE_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_SUBAREA_RANGE, "NEW_PERFORMANCE_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_COORDINATES, "NEW_PERFORMANCE_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_VEH_TYPES, "NEW_PERFORMANCE_VEH_TYPES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_TURN_FLAG, "NEW_PERFORMANCE_TURN_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_FLOW_TYPE, "NEW_PERFORMANCE_FLOW_TYPE", LEVEL1, OPT_KEY, TEXT_KEY, "VEHICLES", FLOW_RANGE, NO_HELP },
			{ NEW_PERFORMANCE_LANE_FLOWS, "NEW_PERFORMANCE_LANE_FLOWS", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (performance_keys);

		Control_Key turn_vol_keys [] = { //--- code, key, level, status, type, help ----
			{ NEW_TURN_VOLUME_FILE, "NEW_TURN_VOLUME_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_TURN_VOLUME_FORMAT, "NEW_TURN_VOLUME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_TURN_VOLUME_FILTER, "NEW_TURN_VOLUME_FILTER", LEVEL1, OPT_KEY, INT_KEY, "0", ">= 0", NO_HELP },
			{ NEW_TURN_VOLUME_TIME_FORMAT, "NEW_TURN_VOLUME_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_TURN_VOLUME_INCREMENT, "NEW_TURN_VOLUME_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
			{ NEW_TURN_VOLUME_TIME_RANGE, "NEW_TURN_VOLUME_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_TURN_VOLUME_NODE_RANGE, "NEW_TURN_VOLUME_NODE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_TURN_VOLUME_SUBAREA_RANGE, "NEW_TURN_VOLUME_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (turn_vol_keys);

		Control_Key ridership_keys [] = { //--- code, key, level, status, type, help ----
			{ NEW_RIDERSHIP_FILE, "NEW_RIDERSHIP_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_RIDERSHIP_FORMAT, "NEW_RIDERSHIP_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_RIDERSHIP_TIME_FORMAT, "NEW_RIDERSHIP_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_RIDERSHIP_TIME_RANGE, "NEW_RIDERSHIP_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_RIDERSHIP_ROUTE_RANGE, "NEW_RIDERSHIP_ROUTE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_RIDERSHIP_ALL_STOPS, "NEW_RIDERSHIP_ALL_STOPS", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (ridership_keys);

		Control_Key occupancy_keys [] = { //--- code, key, level, status, type, default, range, help ----
			{ NEW_OCCUPANCY_FILE, "NEW_OCCUPANCY_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_OCCUPANCY_FORMAT, "NEW_OCCUPANCY_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_OCCUPANCY_TIME_FORMAT, "NEW_OCCUPANCY_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_OCCUPANCY_INCREMENT, "NEW_OCCUPANCY_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "900 seconds", "1..3600 seconds", NO_HELP },
			{ NEW_OCCUPANCY_TIME_RANGE, "NEW_OCCUPANCY_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_OCCUPANCY_LINK_RANGE, "NEW_OCCUPANCY_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_OCCUPANCY_SUBAREA_RANGE, "NEW_OCCUPANCY_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_OCCUPANCY_COORDINATES, "NEW_OCCUPANCY_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
			{ NEW_OCCUPANCY_MAX_FLAG, "NEW_OCCUPANCY_MAX_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (occupancy_keys);

		Control_Key event_keys [] = { //--- code, key, level, status, type, default, range, help ----
			{ NEW_EVENT_FILE, "NEW_EVENT_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_EVENT_FORMAT, "NEW_EVENT_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_EVENT_FILTER, "NEW_EVENT_FILTER", LEVEL1, OPT_KEY, TIME_KEY, "0 seconds", ">= 0 seconds", NO_HELP },
			{ NEW_EVENT_TIME_FORMAT, "NEW_EVENT_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_EVENT_TIME_RANGE, "NEW_EVENT_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_EVENT_TYPE_RANGE, "NEW_EVENT_TYPE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", EVENT_RANGE, NO_HELP },
			{ NEW_EVENT_MODE_RANGE, "NEW_EVENT_MODE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", MODE_RANGE, NO_HELP },
			{ NEW_EVENT_LINK_RANGE, "NEW_EVENT_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_EVENT_SUBAREA_RANGE, "NEW_EVENT_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_EVENT_COORDINATES, "NEW_EVENT_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (event_keys);

		Control_Key traveler_keys [] = { //--- code, key, level, status, type, default, range, help ----
			{ NEW_TRAVELER_FILE, "NEW_TRAVELER_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_TRAVELER_FORMAT, "NEW_TRAVELER_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_TRAVELER_ID_RANGE, "NEW_TRAVELER_ID_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", "> 1:0", NO_HELP },
			{ NEW_TRAVELER_TIME_FORMAT, "NEW_TRAVELER_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_TRAVELER_TIME_RANGE, "NEW_TRAVELER_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_TRAVELER_MODE_RANGE, "NEW_TRAVELER_MODE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", MODE_RANGE, NO_HELP },
			{ NEW_TRAVELER_LINK_RANGE, "NEW_TRAVELER_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_TRAVELER_SUBAREA_RANGE, "NEW_TRAVELER_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_TRAVELER_COORDINATES, "NEW_TRAVELER_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (traveler_keys);
	}
}

//---------------------------------------------------------
//	Read_Controls
//---------------------------------------------------------

void Sim_Output_Step::Read_Controls (void)
{
	int i, num;
	Sim_Output_Data *out_ptr;

	//---- snapshot files ----

	num = sim->Highest_Control_Group (NEW_SNAPSHOT_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_SNAPSHOT_FILE, i)) continue;

		out_ptr = new Snapshot_Output (i);

		output_array.push_back (out_ptr);
	}

	//---- link delay files ----

	num = sim->Highest_Control_Group (NEW_LINK_DELAY_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_LINK_DELAY_FILE, i)) continue;

		out_ptr = new Link_Delay_Output (i);

		output_array.push_back (out_ptr);
	}

	//---- performance files ----

	num = sim->Highest_Control_Group (NEW_PERFORMANCE_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_PERFORMANCE_FILE, i)) continue;

		out_ptr = new Performance_Output (i);

		output_array.push_back (out_ptr);
	}

	//---- turn volume files ----

	num = sim->Highest_Control_Group (NEW_TURN_VOLUME_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_TURN_VOLUME_FILE, i)) continue;

		out_ptr = new Turn_Vol_Output (i);

		output_array.push_back (out_ptr);
	}

	//---- ridership files ----

	num = sim->Highest_Control_Group (NEW_RIDERSHIP_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_RIDERSHIP_FILE, i)) continue;

		out_ptr = new Ridership_Output (i);

		output_array.push_back (out_ptr);
	}

	//---- occupancy files ----

	num = sim->Highest_Control_Group (NEW_OCCUPANCY_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_OCCUPANCY_FILE, i)) continue;

		out_ptr = new Occupancy_Output (i);

		output_array.push_back (out_ptr);
	}

	//---- event files ----

	num = sim->Highest_Control_Group (NEW_EVENT_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_EVENT_FILE, i)) continue;

		out_ptr = new Event_Output (i);

		output_array.push_back (out_ptr);
	}

	//---- traveler files ----

	num = sim->Highest_Control_Group (NEW_TRAVELER_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_TRAVELER_FILE, i)) continue;

		out_ptr = new Traveler_Output (i);

		output_array.push_back (out_ptr);
	}
}

//---------------------------------------------------------
//	Start_Processing
//---------------------------------------------------------

void Sim_Output_Step::Start_Processing (void)
{
	if (num_threads > 1) {
		output_barrier.Start ();
	} else {
		Output_Itr itr;

		for (itr = output_array.begin (); itr != output_array.end (); itr++) {
			(*itr)->Output_Check ();
		}
	}
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

void Sim_Output_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		//if (sim->problem_output.Output_Flag ()) {
		//	sim->problem_output.End_Output ();
		//}
		//if (sim->event_output.Output_Flag ()) {
		//	sim->event_output.End_Output ();
		//}
		//if (sim->traveler_output.Output_Flag ()) {
		//	sim->traveler_output.End_Output ();
		//}
		if (threads.size () > 0) {
			output_barrier.Exit ();
			threads.Join_All ();
		}
		num_threads = 0;
	}
#endif
}

//---------------------------------------------------------
//	Check_Output
//---------------------------------------------------------

void Sim_Output_Step::Check_Output (Travel_Step &step)
{
	if (step.Traveler () < 0) return;

	Output_Itr itr;

	for (itr = output_array.begin (); itr != output_array.end (); itr++) {
		(*itr)->Summarize (step);
	}
}
