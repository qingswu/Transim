//*********************************************************
//	Sim_Dir_Data.hpp - network link direction data
//*********************************************************

#ifndef SIM_DIR_DATA_HPP
#define SIM_DIR_DATA_HPP

#include "TypeDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Dir_Data class definition
//---------------------------------------------------------
 
class Sim_Dir_Data : public Integers
{
public:
	Sim_Dir_Data (void)              { Clear (); }

	int   Speed (void)               { return (speed); }
	int   Dir (void)                 { return (dir); }
	int   Type (void)                { return (type); }
	int   Lanes (void)               { return (lanes); }
	int   Length (void)              { return (length); }
	int   Setback (void)             { return (setback); }
	int   In_Offset (void)           { return (in_offset); }
	int   Out_Offset (void)          { return (length - setback - 1); }
	int   Method (void)              { return (method); }
	bool  Turn (void)                { return (turn > 0); }
	int   Subarea (void)             { return (subarea); }
	int   Count (void)               { return (count); }
	int   Max_Cell (void)            { return (max_cell); }
	int   In_Cell (void)             { return (in_cell); }
	int   Out_Cell (void)            { return (out_cell); }
	int   Use (void)                 { return (use); }
	int   Use_Type (void)            { return (use_type); }
	int   Low_Lane (void)            { return (low_lane); }
	int   High_Lane (void)           { return (high_lane); }
	int   Min_Veh_Type (void)        { return (min_veh); }
	int   Max_Veh_Type (void)        { return (max_veh); }
	int   Min_Traveler (void)        { return (min_trav); }
	int   Max_Traveler (void)        { return (max_trav); }
	int   First_Use (void)           { return (first_use); }

	void  Speed (int value)          { speed = (short) value; }
	void  Dir (int value)            { dir = (char) value; }
	void  Type (int value)           { type = (char) value; }
	void  Lanes (int value)          { lanes = (char) value; }
	void  Length (int value)         { length = value; }
	void  Setback (int value)        { setback = (short) value; }
	void  In_Offset (int value)      { in_offset = (short) value; }
	void  Method (int value)         { method = (char) value; }
	void  Turn (bool flag)           { turn = (flag) ? 1 : 0; }
	void  Subarea (int value)        { subarea = (char) value; }
	void  Max_Cell (int value)       { max_cell = (short) value; }
	void  In_Cell (int value)        { in_cell = (char) value; }
	void  Out_Cell (int value)       { out_cell = (short) value; }
	void  Use (int value)            { use = (short) value; }
	void  Use_Type (int value)       { use_type = (char) value; }
	void  Low_Lane (int value)       { low_lane = (char) value; }
	void  High_Lane (int value)      { high_lane = (char) value; }
	void  Min_Veh_Type (int value)   { min_veh = (char) value; }
	void  Max_Veh_Type (int value)   { max_veh = (char) value; }
	void  Min_Traveler (int value)   { min_trav = (char) value; }
	void  Max_Traveler (int value)   { max_trav = (char) value; }
	void  First_Use (int value)      { first_use = value; }
	
	void  Load_Queue (int value)             { load_queue.push_back (value); }
	Integers load_queue;

	void Make_Cells (void)                   { assign ((max_cell + 1) * lanes, 0); }

	int  Index (int lane, int cell)          { return ((max_cell - cell) * lanes + lane); }
	int  Lane (int index)                    { return (index % lanes); }
	int  Cell (int index)                    { return (max_cell - (index / lanes)); }

	int  Front (int index)                   { return ((Cell (index) < max_cell) ? index - lanes : -1); } 
	int  Right (int index)                   { return ((Lane (index) < lanes - 1) ? index + 1 : -1); }
	int  Left (int index)                    { return ((Lane (index) > 0) ? index - 1 : -1); }

	int  Get (int index)                     { return ((index >= 0) ? at (index) : -1); }
	int  Get (int lane, int cell)            { return (at (Index (lane, cell))); }

	void Set (int index, int value)          { at (index) = value; }
	void Set (int lane, int cell, int value) { at (Index (lane, cell)) = value; }

	bool Check (int index)                   { return (at (index) == 0); }
	bool Check (int lane, int cell)          { return (at (Index (lane, cell)) == 0); }

	void Clear (int index)                   { at (index) = 0; }
	void Clear (int lane, int cell)          { at (Index (lane, cell)) = 0; }

	void Add (void)                          { count++; }
	void Add (int index, int value)          { Set (index, value); count++; }
	void Add (int lane, int cell, int value) { Set (lane, cell, value); count++; }

	void Remove (void)                       { count--; }
	void Remove (int index)                  { Clear (index); count--; }
	void Remove (int lane, int cell)         { Clear (lane, cell); count--; }

	void Clear_Use (int use = ANY_USE_CODE) 
	{
		Use (use); use_type = LIMIT; low_lane = high_lane = min_veh = max_veh = min_trav = max_trav = -1; first_use = -1;
	}

	void Clear (void)
	{
		length = 0; speed = setback = in_offset = max_cell = out_cell = count = 0; 
		dir = lanes = method = turn = type = subarea = 0; load_queue.clear ();
		Clear_Use ();
	}

private:
	short  speed;
	char   dir;
	char   lanes;
	int    length;
	short  setback;
	short  in_offset;
	char   method;
	char   turn;
	char   type;
	char   subarea;
	short  count;
	short  max_cell;
	short  out_cell;
	char   in_cell;
	char   use_type;
	short  use;
	char   low_lane;
	char   high_lane;
	char   min_veh;
	char   max_veh;
	char   min_trav;
	char   max_trav;
	int    first_use;
};

typedef vector <Sim_Dir_Data>    Sim_Dir_Array;
typedef Sim_Dir_Array::iterator  Sim_Dir_Itr;
typedef Sim_Dir_Data *           Sim_Dir_Ptr;

#endif
