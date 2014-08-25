//*********************************************************
//	Simulator.hpp - Simulate Travel Plans
//*********************************************************

#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "APIDefs.hpp"
#include "Simulator_Service.hpp"
#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Simulator - execution class definition
//---------------------------------------------------------

class SYSLIB_API Simulator : public Simulator_Service
{
public:

	Simulator (void);

	virtual void Execute (void);

protected:
	virtual void Program_Control (void);

private:
	//enum Simulator_Reports { FIRST_REPORT = 1, SECOND_REPORT };

	bool io_flag;
	Dtime max_time, signal_update_time, timing_update_time, transit_update_time;

	int lane_change_levels, max_vehicles;
	
	Plan_File *plan_file;
};
#endif
