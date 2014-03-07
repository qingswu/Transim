//*********************************************************
//	Link_Delay_Output.hpp - Output Interface Class
//*********************************************************

#ifndef LINK_DELAY_OUTPUT_HPP
#define LINK_DELAY_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Data_Range.hpp"
#include "Link_Delay_File.hpp"
#include "Link_Delay_Data.hpp"
#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Link_Delay_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Delay_Output : public Sim_Output_Data
{
public:
	Link_Delay_Output (int num);

	bool Output_Check (void);

	void Summarize (Travel_Step &step);

private:

	Link_Delay_File  *file;
	Data_Range  link_range;
	Data_Range  subarea_range;
	Data_Range  veh_types;
	Flow_Time_Array  link_delay;
	Flow_Time_Array  turn_delay;
	int   x1, y1, x2, y2;            //---- rounded ----
	bool  data_flag;
	bool  flow_flag;
	bool  turn_flag;
	bool  coord_flag;

	void Write_Summary (void);
};
#endif
