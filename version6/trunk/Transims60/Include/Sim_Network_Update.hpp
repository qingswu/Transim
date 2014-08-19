//*********************************************************
//	Sim_Network_Update.hpp - Network Use Code Update Class
//*********************************************************

#ifndef SIM_NETWORK_UPDATE_HPP
#define SIM_NETWORK_UPDATE_HPP

#include "Sim_Update_Data.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Sim_Network_Update Class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Network_Update : public Sim_Update_Data
{
public:
	Sim_Network_Update (void);

	bool Update_Check (void);

private:
	Dtime use_update_time, turn_update_time;
};
#endif
