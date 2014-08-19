//*********************************************************
//	Occupancy_Output.hpp - Output Interface Class
//*********************************************************

#ifndef OCCUPANCY_OUTPUT_HPP
#define OCCUPANCY_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Data_Range.hpp"
#include "Occupancy_File.hpp"
#include "TypeDefs.hpp"
#include "Travel_Step.hpp"
#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Occupancy_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Occupancy_Output : public Sim_Output_Data
{
public:
	Occupancy_Output (int num);
	~Occupancy_Output (void);

	void Write_Check (void);
	
	void Output_Check (Travel_Step &step);

private:

	Occupancy_File *file;
	Data_Range link_range;
	Data_Range subarea_range;
	Ints_Array occ_array;
	int x1, y1, x2, y2;		//---- rounded ----
	bool data_flag;
	bool coord_flag;
	bool max_flag;
	bool total_flag;
	
	void Write_Summary (void);
};
#endif
