//*********************************************************
//	Sim_Veh_Data.hpp - simulator vehicle data
//*********************************************************

#ifndef SIM_VEH_DATA_HPP
#define SIM_VEH_DATA_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Veh_Data class definition
//---------------------------------------------------------

class Sim_Veh_Data
{
public:
	Sim_Veh_Data (void)                        { Clear (); }

	Sim_Veh_Data (int lnk, int ln, int off)    { Location (lnk, ln, off); }

	bool  Parked (void)                        { return (parked != 0); }
	void  Parked (bool flag)                   { parked = (flag) ? 1 : 0; }

	void  Location (int lnk, int ln, int off)  { link = lnk; lane = ln; offset = off; parked = 0; }
	void  Location (Sim_Veh_Data *ptr)         { Location (ptr->link, ptr->lane, ptr->offset); }

	void  Location (vector <Sim_Veh_Data>::iterator itr)   { Location (itr->link, itr->lane, itr->offset); }

	void  Clear (void)                         { link = -1; lane = offset = 0; parked = 1; }

	int   link;
	int   parked : 1;
	int   lane   : 7;
	int   offset : 24;
};

typedef Sim_Veh_Data *            Sim_Veh_Ptr;
typedef vector <Sim_Veh_Data>     Sim_Veh_Array;
typedef Sim_Veh_Array::iterator   Sim_Veh_Itr;

#endif
