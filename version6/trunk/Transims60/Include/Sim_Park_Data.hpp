//*********************************************************
//	Sim_Park_Data.hpp - simulator parking data
//*********************************************************

#ifndef SIM_PARK_DATA_HPP
#define SIM_PARK_DATA_HPP

#include "Data_Pack.hpp"

//---------------------------------------------------------
//	Sim_Park_Data class definition
//---------------------------------------------------------
 
class Sim_Park_Data
{
public:
	Sim_Park_Data (void)            { Clear (); }

	int   Offset_AB (void)          { return (offset_ab); }
	int   Offset_BA (void)          { return (offset_ba); }
	int   Dir (void)                { return (dir); }
	int   Type (void)               { return (type); }
	int   Min_Lane_AB (void)        { return (min_ab); }
	int   Max_Lane_AB (void)        { return (max_ab); }
	int   Min_Lane_BA (void)        { return (min_ba); }
	int   Max_Lane_BA (void)        { return (max_ba); }

	void  Offset_AB (int value)     { offset_ab = value; }
	void  Offset_BA (int value)     { offset_ba = value; }
	void  Dir (int value)           { dir = (short) value; }
	void  Type (int value)          { type = (short) value; }
	void  Min_Lane_AB (int value)   { min_ab = (char) value; }
	void  Max_Lane_AB (int value)   { max_ab = (char) value; }
	void  Min_Lane_BA (int value)   { min_ba = (char) value; }
	void  Max_Lane_BA (int value)   { max_ba = (char) value; }

	void  Clear (void)              { offset_ab = offset_ba = 0; dir = type = 0; min_ab = max_ab = min_ba = max_ba = 0; }

private:
	int   offset_ab;
	int   offset_ba;
	short dir;
	short type;
	char  min_ab;
	char  max_ab;
	char  min_ba;
	char  max_ba;
};
	
typedef Vector <Sim_Park_Data>      Sim_Park_Array;
typedef Sim_Park_Array::iterator    Sim_Park_Itr;
typedef Sim_Park_Data *             Sim_Park_Ptr;

#endif
