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
	num_outputs = 0;
	problem_flag = false;
#ifdef THREADS
	num_threads = 0;
	sim_output_process = 0;
#endif
}

//---------------------------------------------------------
//	Sim_Output_Step -- destructor
//---------------------------------------------------------

Sim_Output_Step::~Sim_Output_Step (void)
{
	Stop_Processing ();
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Output_Step::Initialize (void)
{
	num_outputs = (int) output_array.size ();

#ifdef THREADS
	output_queue.Max_Records (num_outputs);

	num_threads = sim->Num_Threads ();
	if (num_threads > num_outputs) num_threads = num_outputs;
	if (num_threads == 1) return;

	sim_output_process = new Sim_Output_Process * [num_threads];

	for (int i=0; i < num_threads; i++) {
		sim_output_process [i] = new Sim_Output_Process (this);
		threads.push_back (thread (ref (*(sim_output_process [i]))));
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
			{ NEW_SNAPSHOT_METRIC_FLAG, "NEW_SNAPSHOT_METRIC_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (snapshot_keys);

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
			END_CONTROL
		};
		sim->Key_List (performance_keys);

		Control_Key turn_delay_keys [] = { //--- code, key, level, status, type, help ----
			{ NEW_TURN_DELAY_FILE, "NEW_TURN_DELAY_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
			{ NEW_TURN_DELAY_FORMAT, "NEW_TURN_DELAY_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
			{ NEW_TURN_DELAY_FILTER, "NEW_TURN_DELAY_FILTER", LEVEL1, OPT_KEY, INT_KEY, "0", ">= 0", NO_HELP },
			{ NEW_TURN_DELAY_TIME_FORMAT, "NEW_TURN_DELAY_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
			{ NEW_TURN_DELAY_INCREMENT, "NEW_TURN_DELAY_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
			{ NEW_TURN_DELAY_TIME_RANGE, "NEW_TURN_DELAY_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
			{ NEW_TURN_DELAY_NODE_RANGE, "NEW_TURN_DELAY_NODE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			{ NEW_TURN_DELAY_SUBAREA_RANGE, "NEW_TURN_DELAY_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
			END_CONTROL
		};
		sim->Key_List (turn_delay_keys);

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

	problem_flag = sim->System_File_Flag (NEW_PROBLEM);

	if (problem_flag) {
		output_array.push_back (new Problem_Output ());
	}

	//---- snapshot files ----

	num = sim->Highest_Control_Group (NEW_SNAPSHOT_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_SNAPSHOT_FILE, i)) continue;

		output_array.push_back (new Snapshot_Output (i));
	}

	//---- performance files ----

	num = sim->Highest_Control_Group (NEW_PERFORMANCE_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_PERFORMANCE_FILE, i)) continue;

		output_array.push_back (new Performance_Output (i));
	}

	//---- turn delay files ----

	num = sim->Highest_Control_Group (NEW_TURN_DELAY_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_TURN_DELAY_FILE, i)) continue;

		output_array.push_back (new Turn_Delay_Output (i));
	}

	//---- ridership files ----

	num = sim->Highest_Control_Group (NEW_RIDERSHIP_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_RIDERSHIP_FILE, i)) continue;

		output_array.push_back (new Ridership_Output (i));
	}

	//---- occupancy files ----

	num = sim->Highest_Control_Group (NEW_OCCUPANCY_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_OCCUPANCY_FILE, i)) continue;

		output_array.push_back (new Occupancy_Output (i));
	}

	//---- event files ----

	num = sim->Highest_Control_Group (NEW_EVENT_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_EVENT_FILE, i)) continue;

		output_array.push_back (new Event_Output (i));
	}

	//---- traveler files ----

	num = sim->Highest_Control_Group (NEW_TRAVELER_FILE, 0);

	for (i=1; i <= num; i++) {
		if (!sim->Check_Control_Key (NEW_TRAVELER_FILE, i)) continue;

		output_array.push_back (new Traveler_Output (i));
	}
}

//---------------------------------------------------------
//	Start_Processing
//---------------------------------------------------------

void Sim_Output_Step::Start_Processing (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		output_queue.Start_Work ();

		for (int i=0; i < num_outputs; i++) {
			output_queue.Put (i);
		}
		output_queue.Complete_Work ();
		return;
	}
#endif
	Output_Itr itr;

	for (itr = output_array.begin (); itr != output_array.end (); itr++) {
		(*itr)->Write_Check ();
	}
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

void Sim_Output_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_output_process != 0 && num_threads > 1) {
		output_queue.Exit_Queue ();
		threads.Join_All ();

		for (int i=0; i < num_threads; i++) {
			delete sim_output_process [i];
			sim_output_process [i] = 0;
		}
		delete [] sim_output_process;
		sim_output_process = 0;
		num_threads = 0;
	}
#endif
	Output_Itr itr;

	for (itr = output_array.begin (); itr != output_array.end (); itr++) {
		delete (*itr);
	}
	output_array.clear ();
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Sim_Output_Step::Output_Check (Travel_Step &step)
{
	if (step.Traveler () < 2) return;

	Output_Itr itr;

	for (itr = output_array.begin (); itr != output_array.end (); itr++) {
		(*itr)->Output_Check (step);
	}
}

//---------------------------------------------------------
//	Output_Problem
//---------------------------------------------------------

void Sim_Output_Step::Output_Problem (Problem_Data &problem)
{
	if (problem_flag) {
		Problem_Output *ptr = (Problem_Output *) output_array [0];
		ptr->Output_Problem (problem);
	}
}

//---------------------------------------------------------
//	Event_Check
//---------------------------------------------------------

void Sim_Output_Step::Event_Check (Travel_Step &step)
{
	Output_Itr itr;

	for (itr = output_array.begin (); itr != output_array.end (); itr++) {
		if ((*itr)->Type () == EVENT_OUTPUT_OFFSET) {
			Event_Output *ptr = (Event_Output *) (*itr);
			ptr->Event_Check (step);
		}
	}
}

#ifdef THREADS
//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Output_Step::Sim_Output_Process::operator()()
{
	int num;

	for (;;) {
		num = step_ptr->output_queue.Get ();
		if (num < 0) break;

		Sim_Output_Data *output = step_ptr->output_array [num];

		output->Write_Check ();

		step_ptr->output_queue.Finished ();
	}
}
#endif
