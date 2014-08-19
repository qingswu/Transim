//*********************************************************
//	Sim_Output_Data.hpp - base class for simulator output
//*********************************************************

#ifndef SIM_OUTPUT_DATA_HPP
#define SIM_OUTPUT_DATA_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "TypeDefs.hpp"
#include "Travel_Step.hpp"
#include "Time_Periods.hpp"

//---------------------------------------------------------
//	Sim_Output_Data - base class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Output_Data : public Static_Service
{
public:
	Sim_Output_Data (void)		        { type = number = 0; }

	int  Type (void)                    { return (type); }
	void Type (int value)               { type = value; }

	int  Number (void)                  { return (number); }
	void Number (int value)             { number = value; }

	virtual void Initialize (void)	    { }

	virtual void Add_Keys (void)        { }

	virtual void Write_Check (void)     { }

	virtual void Output_Check (Travel_Step &step)     { int x = step.Traveler (); x++; }
	
	Time_Periods  time_range;

private:
	int  type, number;
};
#endif

