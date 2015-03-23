//*********************************************************
//	Router.hpp - Network Path Building 
//*********************************************************

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "APIDefs.hpp"
#include "Converge_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Db_File.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Router - execution class definition
//---------------------------------------------------------

class SYSLIB_API Router : public Converge_Service
{

public:
	Router (void);

	virtual void Execute (void);

protected:
	enum Router_Keys { UPDATE_PLAN_RECORDS = 1, REROUTE_FROM_TIME_POINT, 
	};
	virtual void Program_Control (void);

private:
	Dtime reroute_time;

	bool reroute_flag;

	Integers veh_parking;

	Random random;

	//---- Router methods ----

	void   DUE_Loop (void);
	double Reskim_Plans (bool average_flag);
	void   Iteration_Loop (void);
	double Minimize_VHT (double &factor, bool zero_flag);
	double Merge_Delay (double factor, bool zero_flag);
};
#endif

