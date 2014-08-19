//*********************************************************
//	Sim_Control_Update.cpp - Traffic Control Update Class
//*********************************************************

#include "Sim_Control_Update.hpp"

//---------------------------------------------------------
//	Sim_Control_Update constructor
//---------------------------------------------------------

Sim_Control_Update::Sim_Control_Update () : Sim_Update_Data ()
{
	Type (SIM_CONTROL_UPDATE);
	
	signal_update_time = timing_update_time = 0;
}

//---------------------------------------------------------
//	Update_Check
//---------------------------------------------------------

bool Sim_Control_Update::Update_Check (void)
{
	if (signal_update_time > sim->time_step && timing_update_time > sim->time_step) return (false);

	int index;
	bool flag, signal_update_flag, timing_update_flag;

	Sim_Signal_Itr signal_itr;

	//---- check the update times ----

	if (sim->max_method <= MACROSCOPIC) {
		timing_update_time = signal_update_time = sim->end_period;
		return (true);
	}
	signal_update_flag = (signal_update_time <= sim->time_step);
	if (signal_update_flag) signal_update_time = sim->end_period;

	timing_update_flag = (timing_update_time <= sim->time_step);
	if (timing_update_flag) timing_update_time = sim->end_period;

	//---- check each signal ----

	for (index=0, signal_itr = sim->sim_signal_array.begin (); signal_itr != sim->sim_signal_array.end (); signal_itr++, index++) {
		flag = false;

		//---- update signal timing and phasing plans ----

		if (signal_update_flag) {
			if (sim->time_step < signal_itr->End_Time () && signal_itr->Status () > 0) {
				if (signal_itr->Check_Time () < signal_update_time) {
					signal_update_time = signal_itr->Check_Time ();
				}
			} else {
				flag = Traffic_Control (index, signal_itr);
			}
		}

		//---- update signal state ----

		if (!flag && signal_update_flag) {
			if (signal_itr->Check_Time () > sim->time_step) {
				if (signal_itr->Check_Time () < timing_update_time) {
					timing_update_time = signal_itr->Check_Time ();
				}
			} else {
				Signal_Timing (index, signal_itr);
			}
		}
	}
	return (true);
}

