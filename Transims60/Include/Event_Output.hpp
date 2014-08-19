//*********************************************************
//	Event_Output.hpp - Output Interface Class
//*********************************************************

#ifndef EVENT_OUTPUT_HPP
#define EVENT_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Event_Data.hpp"
#include "Event_File.hpp"
#include "Data_Range.hpp"

//---------------------------------------------------------
//	Event_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Event_Output : public Sim_Output_Data
{
public:
	Event_Output (int num);
	~Event_Output (void);

	void Event_Check (Event_Type type, Travel_Step &step);

private:

	Event_File *file;
	Data_Range link_range;
	Data_Range subarea_range;
	bool type_flag [MAX_EVENT];
	bool mode_flag [MAX_MODE];
	int filter;
	int x1, y1, x2, y2;		//---- rounded ----
	bool coord_flag;
};
#endif
