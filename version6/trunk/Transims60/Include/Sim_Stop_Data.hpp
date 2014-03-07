//*********************************************************
//	Sim_Stop_Data.hpp - simulator transit stop data
//*********************************************************

#ifndef SIM_STOP_DATA_HPP
#define SIM_STOP_DATA_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Stop_Data class definition
//---------------------------------------------------------
 
class Sim_Stop_Data
{
public:
	Sim_Stop_Data (void)            { Clear (); }

	int   Offset (void)             { return (offset); }
	int   Min_Lane (void)           { return (min); }
	int   Max_Lane (void)           { return (max); }

	void  Offset (int value)        { offset = value; }
	void  Min_Lane (int value)      { min = (short) value; }
	void  Max_Lane (int value)      { max = (short) value; }

	void  Clear (void)              { offset = 0; min = max = 0; }

private:
	int   offset;
	short min;
	short max;
};

typedef vector <Sim_Stop_Data>      Sim_Stop_Array;
typedef Sim_Stop_Array::iterator    Sim_Stop_Itr;
typedef Sim_Stop_Data *             Sim_Stop_Ptr;

#endif
