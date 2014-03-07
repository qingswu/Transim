//*********************************************************
//	Veh_Cell_Data.hpp - simulator vehicle cell data
//*********************************************************

#ifndef VEH_CELL_DATA_HPP
#define VEH_CELL_DATA_HPP

#include "Dtime.hpp"
#include "Cell_Data.hpp"
#include "Data_Pack.hpp"

//---------------------------------------------------------
//	Veh_Cell_Data class definition
//---------------------------------------------------------

class Veh_Cell_Data
{
public:
	Veh_Cell_Data (void)           { Clear (); }

	int  Driver (void)             { return (driver); }
	int  Type (void)               { return (type); }
	int  Cell (void)               { return (cell); }
	int  Lane (void)               { return (lane); }
	int  Link (void)               { return (link); }
	int  Offset (void)             { return (offset); }
	int  Leader (void)             { return (leader); }
	int  Follower (void)           { return (follower); }

	void Driver (int value)        { driver = value; }
	void Type (int value)          { type = (short) value; }
	void Cell (int value)          { cell = (char) value; }
	void Lane (int value)          { lane = (char) value; }
	void Link (int value)          { link = value; }
	void Offset (int value)        { offset = value; }
	void Leader (int value)        { leader = value; }
	void Follower (int value)      { follower = value; }

	void Lane_Change (int change)  { lane = (char) (lane + change); }
	
	void Location (int *_link, int *_lane, int *_offset) 
									{ *_link = link; *_lane = lane; *_offset = offset; }

	void Location (int link, int lane, int offset) 
									{ Link (link); Lane (lane); Offset (offset); }

	Cell_Data Location (void)       { Cell_Data location (link, lane, offset, leader); return (location); }

	void Clear (void)               { driver = link = leader = follower = offset = -1; type = -1; lane = cell = -1; }

	bool Pack (Data_Buffer &data)   { return (data.Add_Data (this, sizeof (*this))); }
	bool UnPack (Data_Buffer &data) { return (data.Get_Data (this, sizeof (*this))); }

private:
	int   driver;
	short type;
	char  cell;
	char  lane;
	int   link;
	int   offset;
	int   leader;
	int   follower;
};

typedef Veh_Cell_Data *            Veh_Cell_Ptr;
typedef Vector <Veh_Cell_Data>     Veh_Cell_Array;
typedef Veh_Cell_Array::iterator   Veh_Cell_Itr;

#endif
