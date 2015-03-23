//*********************************************************
//	Sim_Veh_Data.hpp - simulator vehicle data
//*********************************************************

#ifndef SIM_VEH_DATA_HPP
#define SIM_VEH_DATA_HPP

#include "Data_Pack.hpp"

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
	
	bool  Pack (Data_Buffer &data)             { return (data.Add_Data (this, sizeof (*this))); }
	bool  UnPack (Data_Buffer &data)           { return (data.Get_Data (this, sizeof (*this))); }
	bool  Pack (Pack_File &file)               { return (file.Write (this, sizeof (*this))); }
	bool  UnPack (Pack_File &file)             { return (file.Read (this, sizeof (*this))); }

	int   link;
	int   parked : 1;
	int   lane   : 7;
	int   offset : 24;
};

typedef Sim_Veh_Data *            Sim_Veh_Ptr;
typedef Vector <Sim_Veh_Data>     Sim_Veh_Array;
typedef Sim_Veh_Array::iterator   Sim_Veh_Itr;

#endif
