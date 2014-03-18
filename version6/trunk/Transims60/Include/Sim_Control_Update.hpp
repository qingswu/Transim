//*********************************************************
//	Sim_Control_Update.hpp - Traffic Control Update Class
//*********************************************************

#ifndef SIM_CONTROL_UPDATE_HPP
#define SIM_CONTROL_UPDATE_HPP

#include "Sim_Update_Data.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Control_Update Class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Control_Update : public Sim_Update_Data
{
public:
	Sim_Control_Update (void);

	bool Update_Check (void);

private:
	bool Traffic_Control (int index, Sim_Signal_Itr signal_itr);
	bool Signal_Timing (int index, Sim_Signal_Itr signal_itr);
	bool Check_Detector (int index);

	Dtime signal_update_time, timing_update_time;
};
#endif
