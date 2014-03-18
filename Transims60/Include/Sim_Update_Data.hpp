//*********************************************************
//	Sim_Update_Data.hpp - base class for update processing
//*********************************************************

#ifndef SIM_UPDATE_DATA_HPP
#define SIM_UPDATE_DATA_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"
#include "Static_Service.hpp"
#include "TypeDefs.hpp"
#include "Barrier.hpp"

//---------------------------------------------------------
//	Sim_Update_Data - base class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Update_Data : public Static_Service
{
public:
	Sim_Update_Data (void)		        { type = 0; };
	
	void operator()();

	int  Type (void)                    { return (type); }
	void Type (int value)               { type = value; }

	virtual bool Update_Check (void)    { return (false); }

private:
	int  type;
};

#endif
