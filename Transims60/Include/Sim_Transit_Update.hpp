//*********************************************************
//	Sim_Transit_Update.hpp - Transit Run Update Class
//*********************************************************

#ifndef SIM_TRANSIT_UPDATE_HPP
#define SIM_TRANSIT_UPDATE_HPP

#include "Sim_Update_Data.hpp"

//---------------------------------------------------------
//	Sim_Transit_Update Class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Transit_Update : public Sim_Update_Data
{
public:
	Sim_Transit_Update (void);

	bool Update_Check (void);

private:
	Dtime run_update_time;
};
#endif
