//*********************************************************
//	Performance_Output.hpp - Output Interface Class
//*********************************************************

#ifndef PERFORMANCE_OUTPUT_HPP
#define PERFORMANCE_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Data_Range.hpp"
#include "Performance_File.hpp"
#include "Performance_Data.hpp"
#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Performance_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Performance_Output : public Sim_Output_Data
{
public:
	Performance_Output (int num);
	~Performance_Output (void);

	void Write_Check (void);

	void Output_Check (Travel_Step &step);

private:

	Performance_File  *file;
	Data_Range  link_range;
	Data_Range  subarea_range;
	Data_Range  veh_types;
	Perf_Period  perf_period;
	int   x1, y1, x2, y2;        //---- rounded ----
	bool  data_flag;
	bool  flow_flag;
	bool  turn_flag;
	bool  coord_flag;
	bool  first_step;

	void Write_Summary (void);
	void Cycle_Failure (Dtime step, int dir_index, int vehicles, int veh_type);

};
#endif
