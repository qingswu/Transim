
//*********************************************************
//	Path_Builder.cpp - Network Path Building
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Path_Builder constructor
//---------------------------------------------------------

Path_Builder::Path_Builder (Router_Service *exe) : Static_Service ()
{
	Initialize (exe);
}

#ifdef THREADS

Path_Builder::Path_Builder (Plan_Ptr_Queue *queue, Router_Service *exe) : Static_Service ()
{
	if (queue == 0) {
		cout << "\tPlan_Ptr Queue is Zero" << endl;
		initialized = false;
	} else {
		Initialize (exe);
		plan_ptr_queue = queue;
		path_param.one_to_many = false;
		trip_flag = true;
		plan_flag = false;
	}
}
Path_Builder::Path_Builder (Plan_Queue *queue, Router_Service *exe) : Static_Service ()
{
	if (queue == 0) {
		cout << "\tPlan Queue is Zero" << endl;
		initialized = false;
	} else {
		Initialize (exe);
		plan_queue = queue;
		path_param.one_to_many = false;
		trip_flag = false;
		plan_flag = true;
	}
}
Path_Builder::Path_Builder (Skim_Queue *queue, Router_Service *exe) : Static_Service ()
{
	if (queue == 0) {
		cout << "\tSkim Queue is Zero" << endl;
		initialized = false;
	} else {
		Initialize (exe);
		skim_queue = queue;
		path_param.one_to_many = true;
		plan_flag = trip_flag = false;
	}
}

//---------------------------------------------------------
//	thread operator
//---------------------------------------------------------

void Path_Builder::operator()()
{
	int number = 0;

	if (path_param.one_to_many) {
		One_To_Many *skim_ptr;

		for (;;) {
			skim_ptr = skim_queue->Get_Work (number);
			if (skim_ptr == 0) break;

			if (!Skim_Build (skim_ptr)) break;

			if (!skim_queue->Put_Result (skim_ptr, number)) break;
		}
	} else {
		Plan_Ptr_Array *array_ptr;

		if (update_time_flag) {
			Update_Times ();
		} else if (zero_flows_flag) {
			Zero_Flows ();
		}
		if (exe->Memory_Flag ()) {
			for (;;) {
				if (!plan_ptr_queue->Get (array_ptr)) break;

				if (!Array_Processing (array_ptr)) break;

				exe->Save_Plans (array_ptr);
				plan_ptr_queue->Finished ();

			}
		} else {
			for (;;) {
				array_ptr = plan_queue->Get_Work (number);
				if (array_ptr == 0) break;

				if (!Array_Processing (array_ptr)) break;

				if (!plan_queue->Put_Result (array_ptr, number)) break;
			}
		}
		Save_Skim_Gap ();
	}
}

//---------------------------------------------------------
//	Save_Flows
//---------------------------------------------------------

void Path_Builder::Save_Flows (void)
{
	MAIN_LOCK
	int index, period;

	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;

	for (period=0, period_itr = perf_period_array_ptr->begin (); period_itr != perf_period_array_ptr->end (); period_itr++, period++) {
		period_ptr = &exe->perf_period_array [period];

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			if (perf_itr->Volume () > 0.0) {
				perf_ptr = period_ptr->Data_Ptr (index);
				perf_ptr->Add_Flows (&(*perf_itr));
				perf_itr->Clear_Flows ();
			}
		}
	}

	if (path_param.turn_flow_flag) {
		Turn_Period_Itr period_itr;
		Turn_Period *period_ptr;
		Turn_Itr turn_itr;
		Turn_Data *turn_ptr;

		for (period=0, period_itr = turn_period_array_ptr->begin (); period_itr != turn_period_array_ptr->end (); period_itr++, period++) {
			period_ptr = &exe->turn_period_array [period];

			for (index=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, index++) {
				if (turn_itr->Turn () > 0.0) {
					turn_ptr = period_ptr->Data_Ptr (index);
					turn_ptr->Add_Turn (turn_itr->Turn ());
					turn_itr->Turn (0);
				}
			}
		}
	}
	END_LOCK
}

//---------------------------------------------------------
//	Update_Times
//---------------------------------------------------------

void Path_Builder::Update_Times (void)
{
	int index, period;

	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;

	for (period=0, period_itr = perf_period_array_ptr->begin (); period_itr != perf_period_array_ptr->end (); period_itr++, period++) {
		period_ptr = &exe->perf_period_array [period];

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			perf_ptr = period_ptr->Data_Ptr (index);
			perf_itr->Time (perf_ptr->Time ());
			if (zero_flows_flag) {
				perf_itr->Clear_Flows ();
			}
		}
	}
	if (zero_flows_flag && path_param.turn_flow_flag) {
		turn_period_array_ptr->Zero_Turns ();
	}
	zero_flows_flag = false;
	update_time_flag = false;
}
#endif

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Path_Builder::Initialize (Router_Service *_exe)
{
	perf_period_array_ptr = 0;
	turn_period_array_ptr = 0;

	if (_exe == 0) {
		initialized = false;
		return;
	}
	exe = _exe;

	initialized = plan_flag = forward_flag = true;
	reroute_flag = link_to_flag = node_to_flag = stop_to_flag = false;
	zero_flows_flag = update_time_flag = false;

	if (exe->Service_Level () < ROUTER_SERVICE) {
		exe->Error ("Path Building Requires Router Services");
	}
	max_imp = MAX_IMPEDANCE;
	imp_diff = 0;
	time_limit = min_time_limit = 0;
	data_ptr = 0;
	parking_duration = 0;
	path_param.op_cost_rate = 0.0;
	parking_lot = -1;

	mode_path_flag [WALK_MODE] = exe->walk_path_flag;
	mode_path_flag [BIKE_MODE] = exe->bike_path_flag;
	mode_path_flag [DRIVE_MODE] = exe->drive_path_flag;
	mode_path_flag [RIDE_MODE] = true;
	mode_path_flag [TRANSIT_MODE] = exe->transit_path_flag;
	mode_path_flag [PNR_OUT_MODE] = exe->parkride_path_flag;
	mode_path_flag [PNR_IN_MODE] = exe->parkride_path_flag;
	mode_path_flag [KNR_OUT_MODE] = exe->kissride_path_flag;
	mode_path_flag [KNR_IN_MODE] = exe->kissride_path_flag;
	mode_path_flag [TAXI_MODE] = exe->drive_path_flag;
	mode_path_flag [OTHER_MODE] = true;
	mode_path_flag [HOV2_MODE] = exe->drive_path_flag;
	mode_path_flag [HOV3_MODE] = exe->drive_path_flag;
	mode_path_flag [HOV4_MODE] = exe->drive_path_flag;

	near_offset = Round (Internal_Units (10.0, FEET));	

	exe->Set_Parameters (path_param);
	plan_flag = !path_param.one_to_many;

	Reset_Skim_Gap ();

	if (path_param.flow_flag) {
		if (exe->Num_Threads () < 2) {
			perf_period_array_ptr = &exe->perf_period_array;
			turn_period_array_ptr = &exe->turn_period_array;
		} else {
#ifdef THREADS
			perf_period_array_ptr = &perf_period_array;
			perf_period_array.Replicate (exe->perf_period_array);

			if (path_param.turn_flow_flag) {
				turn_period_array_ptr = &turn_period_array;
				turn_period_array.Replicate (exe->turn_period_array);
			}
#endif
		}
	}
}

//---------------------------------------------------------
//	Zero_Flows
//---------------------------------------------------------

void Path_Builder::Zero_Flows (void)
{
	if (path_param.flow_flag) {
		perf_period_array_ptr->Zero_Flows ();

		if (path_param.turn_flow_flag) {
			turn_period_array_ptr->Zero_Turns ();
		}
	}
	zero_flows_flag = false;
}

//---------------------------------------------------------
//	Save_Skim_Gap
//---------------------------------------------------------

void Path_Builder::Save_Skim_Gap (void)
{
	MAIN_LOCK
	exe->skim_gap += skim_gap;
	exe->skim_time += skim_time;
	END_LOCK
}

//---------------------------------------------------------
//	Transit_Path_Index operator
//---------------------------------------------------------

bool operator != (Transit_Path_Index left, Transit_Path_Index right)
{
	return (left.Index () != right.Index () ||
		left.Type () != right.Type () || 
		left.Path () != right.Path ());
}
