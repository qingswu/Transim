//*********************************************************
//	Link_Use_Data.hpp - network link use data
//*********************************************************

#ifndef LINK_USE_DATA_HPP
#define LINK_USE_DATA_HPP

#include "APIDefs.hpp"
#include "Link_Dir_Data.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Link_Use_Data
//---------------------------------------------------------

class SYSLIB_API Link_Use_Data
{
public:
	Link_Use_Data (void)               { Clear (); }

	int   Type (void)                  { return (type); }
	int   Use (void)                   { return (use); }
	Dtime Start (void)                 { return (start); }
	Dtime End (void)                   { return (end); }
	int   Low_Lane (void)              { return (low_lane); }
	int   High_Lane (void)             { return (high_lane); }
	int   Toll (void)                  { return (toll); }
	int   Speed (void)                 { return (speed); }

	void  Type (int value)             { type = (short) value; }
	void  Use (int value)              { use = (short) value; }
	void  Start (Dtime value)          { start = value; }
	void  End (Dtime value)            { end = value; }	
	void  Low_Lane (int value)         { low_lane = (short) value; }
	void  High_Lane (int value)        { high_lane = (short) value; }
	void  Toll (int value)             { toll = (short) value; }
	void  Speed (int value)            { speed = (short) value; }
	
	void  Speed (double value)         { speed = (short) exe->Round (value); }
		
	void  Clear (void)
	{
		type = use = toll = speed = 0; low_lane = high_lane = -1; start = end = 0;
	}
private:
	short type;
	short use;
	Dtime start;
	Dtime end;
	short low_lane;
	short high_lane;
	short toll;
	short speed;
};

typedef vector <Link_Use_Data>    Link_Use_Array;
typedef Link_Use_Array::iterator  Link_Use_Itr;

typedef map <int, Link_Use_Array>      Link_Use_Map;
typedef pair <int, Link_Use_Array>     Link_Use_Map_Data;
typedef Link_Use_Map::iterator         Link_Use_Map_Itr;
typedef pair <Link_Use_Map_Itr, bool>  Link_Use_Map_Stat;

#endif
