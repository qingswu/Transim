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

	typedef struct {
		int node;
		int dir_index;
		int to_index;
	} Sort_Key;

private:

	//---- data index ----
	
	typedef map <Sort_Key, int>       Turn_Map;
	typedef pair <Sort_Key, int>      Turn_Map_Data;
	typedef Turn_Map::iterator        Turn_Map_Itr;
	typedef pair <Turn_Map_Itr, bool> Turn_Map_Stat;

	Turn_Delay_File *file;
	int  filter;
	bool data_flag;
	Data_Range node_range;
	Data_Range subarea_range;
	Turn_Map turn_map;

	void Write_Turn (void);

};
	
bool operator < (Turn_Delay_Output::Sort_Key left, Turn_Delay_Output::Sort_Key right);

#endif
