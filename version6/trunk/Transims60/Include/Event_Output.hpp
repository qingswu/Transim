//*********************************************************
//	Event_Output.hpp - Output Interface Class
//*********************************************************

#ifndef EVENT_OUTPUT_HPP
#define EVENT_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Event_Data.hpp"
#include "Event_File.hpp"
#include "Data_Range.hpp"

#ifdef THREADS
#include "Bounded_Queue.hpp"

typedef Bounded_Queue <Event_Data> Event_Queue;
#endif

//---------------------------------------------------------
//	Event_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Event_Output : public Sim_Output_Data
{
public:
	Event_Output (int num);
	~Event_Output (void);

	bool In_Range (Event_Type type, int mode = DRIVE_MODE, int subarea = 0);

	void Output_Event (Event_Data &data);

	void End_Output (void);

private:

	Event_File *file;
	Data_Range link_range;
	Data_Range subarea_range;
	bool type [MAX_EVENT];
	bool mode [MAX_MODE];
	int filter;
	int x1, y1, x2, y2;		//---- rounded ----
	bool coord_flag;

	void Write_Event (Event_Data &data);

#ifdef MPI_EXE
public:
	void MPI_Processing (void);
private:
	Data_Buffer data;
	mutex  data_mutex;
#else
 #ifdef THREADS
	Event_Queue *event_queue;
 #endif
#endif
};
#endif
