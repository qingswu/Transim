//*********************************************************
//	Turn_Delay_Output.hpp - Output Interface Class
//*********************************************************

#ifndef TURN_DELAY_OUTPUT_HPP
#define TURN_DELAY_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Turn_Delay_File.hpp"
#include "Turn_Delay_Data.hpp"
#include "Data_Range.hpp"
#include "Travel_Step.hpp"
#include "Data_Buffer.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Turn_Delay_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Delay_Output : public Sim_Output_Data
{
public:
	Turn_Delay_Output (int num);
	~Turn_Delay_Output (void);

	void Write_Check (void);

	void Output_Check (Travel_Step &step);

private:

	typedef map <Int2_Key, Turn_Data>   Turn_Map;
	typedef pair <Int2_Key, Turn_Data>  Turn_Map_Data;
	typedef Turn_Map::iterator          Turn_Map_Itr;
	typedef pair <Turn_Map_Itr, bool>   Turn_Map_Stat;

	Turn_Delay_File *file;
	int  filter;
	bool data_flag;
	Data_Range node_range;
	Data_Range subarea_range;
	Turn_Map turn_map;

	void Write_Turn (void);

};
#endif
