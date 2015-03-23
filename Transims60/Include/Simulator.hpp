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
	virtual void Print_Reports (void);
	virtual void Page_Header (void);

protected:
	virtual void Program_Control (void);

private:
	enum Simulator_Keys { NEW_SIMULATION_BACKUP_FILE = 1, BACKUP_TIME_POINTS };

	//enum Simulator_Reports { FIRST_REPORT = 1, SECOND_REPORT };

	bool io_flag, backup_flag;
	String backup_name, backup_ext;
	Dtime_List backup_times;
	Dtime max_time, signal_update_time, timing_update_time, transit_update_time;

	int lane_change_levels, max_vehicles;

	void Read_Trips (void);
};
#endif
