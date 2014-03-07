//*********************************************************
//	Ridership_Output.hpp - Output Interface Class
//*********************************************************

#ifndef RIDERSHIP_OUTPUT_HPP
#define RIDERSHIP_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Data_Range.hpp"
#include "Ridership_File.hpp"

//---------------------------------------------------------
//	Ridership_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Ridership_Output : public Sim_Output_Data
{
public:
	Ridership_Output (int num);

	bool Output_Check (void);

	void Output (void);

private:

	Ridership_File *file;
	Data_Range route_range;
	bool all_stops;
};

#endif
