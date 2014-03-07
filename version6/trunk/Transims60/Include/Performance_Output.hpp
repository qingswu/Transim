//*********************************************************
//	Performance_Output.hpp - Output Interface Class
//*********************************************************

#ifndef Performance_OUTPUT_HPP
#define Performance_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Data_Range.hpp"
#include "Performance_File.hpp"
#include "Performance_Data.hpp"
#include "Travel_Step.hpp"
#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Performance_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Performance_Output : public Sim_Output_Data
{
public:
	Performance_Output (int num);

	bool Output_Check (void);

	void Summarize (Travel_Step &step);

private:

	Performance_File  *file;
	Time_Periods  time_range;
	Data_Range  link_range;
	Data_Range  subarea_range;
	Data_Range  veh_types;
	Link_Perf_Array  link_perf;
	Flow_Time_Array  turn_perf;
	int   x1, y1, x2, y2;        //---- rounded ----
	bool  data_flag;
	bool  flow_flag;
	bool  turn_flag;
	bool  coord_flag;

	void Write_Summary (void);
	void Cycle_Failure (Dtime step, int dir_index, int vehicles, int persons, int veh_type);

};
#endif
