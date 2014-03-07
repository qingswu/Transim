//*********************************************************
//	Traveler_Output.hpp - Output Interface Class
//*********************************************************

#ifndef TRAVELER_OUTPUT_HPP
#define TRAVELER_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Traveler_Data.hpp"
#include "Traveler_File.hpp"
#include "Data_Range.hpp"

#ifdef THREADS
#include "Bounded_Queue.hpp"

typedef Bounded_Queue <Traveler_Data> Traveler_Queue;
#endif

//---------------------------------------------------------
//	Traveler_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Traveler_Output : public Sim_Output_Data
{
public:
	Traveler_Output (int num);
	~Traveler_Output (void);

	bool In_Range (Traveler_Data &data);

	void Output_Traveler (Traveler_Data &data);

	void End_Output (void);

private:

	Traveler_File *file;
	Data_Range hhold_range;
	Data_Range person_range;
	Data_Range link_range;
	Data_Range subarea_range;
	bool mode [MAX_MODE];
	int x1, y1, x2, y2;		//---- rounded ----
	bool coord_flag;

	void Write_Traveler (Traveler_Data &data);

#ifdef THREADS
	Traveler_Queue *traveler_queue;
#endif
};
#endif
