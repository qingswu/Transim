//*********************************************************
//	Vehicle_Index.hpp - hash index for vehicle ID
//*********************************************************

#ifndef VEHICLE_INDEX_HPP
#define VEHICLE_INDEX_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Vehicle_Index class definition
//---------------------------------------------------------

class SYSLIB_API Vehicle_Index : public Int2_Key
{
public:
	Vehicle_Index (void)            { Clear (); }
	Vehicle_Index (int hh, int veh) { first = hh; second = veh; }

	int  Household (void)           { return (first); }
	int  Vehicle (void)             { return (second); }

	void Household (int value)      { first = value; }
	void Vehicle (int value)        { second = value; }
	
	void Clear (void)               { first = second = 0; }
};

//---- vehicle map ----

typedef HASH <Vehicle_Index, int>      Vehicle_Map;
typedef pair <Vehicle_Index, int>      Vehicle_Map_Data;
typedef Vehicle_Map::iterator          Vehicle_Map_Itr;
typedef pair <Vehicle_Map_Itr, bool>   Vehicle_Map_Stat;
#endif

