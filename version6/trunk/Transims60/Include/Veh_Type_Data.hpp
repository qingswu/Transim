//*********************************************************
//	Veh_Type_Data.hpp - vehicle type data classes
//*********************************************************

#ifndef VEH_TYPE_DATA_HPP
#define VEH_TYPE_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Notes_Data.hpp"
#include "Execution_Service.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Veh_Type_Data class definition
//---------------------------------------------------------

class SYSLIB_API Veh_Type_Data : public Notes_Data
{
public:
	Veh_Type_Data (void)               { Clear (); }

	int      Type (void)               { return (type); }
	int      Length (void)             { return (length); }
	int      Max_Speed (void)          { return (max_speed); }
	int      Max_Accel (void)          { return (max_accel); }
	int      Max_Decel (void)          { return (max_decel); }
	int      Op_Cost (void)            { return (op_cost); }
	Use_Type Use (void)                { return (use); }
	int      Capacity (void)           { return (capacity); }
	int      Max_Load (void)           { return (max_load); }
	int      Occupancy (void)          { return (occupancy); }
	int      Loading (void)            { return (loading); }
	int      Unloading (void)          { return (unloading); }
	int      Method (void)             { return (method); }
	Dtime    Min_Dwell (void)          { return (min_dwell); }
	Dtime    Max_Dwell (void)          { return (max_dwell); }
	int      Grade_Func (void)         { return (grade); }
	int      Fuel_Func (void)          { return (fuel); }
	int      Fuel_Cap (void)           { return (fuel_cap); }
	int      PCE (void)                { return (pce); }
	int      Cells (void)              { return (cells); }

	void     Type (int value)          { type = value; }
	void     Length (int value)        { length = value; }
	void     Max_Speed (int value)     { max_speed = value; }
	void     Max_Accel (int value)     { max_accel = value; }
	void     Max_Decel (int value)     { max_decel = value; }
	void     Op_Cost (int value)       { op_cost = value; }
	void     Use (int value)           { use = (Use_Type) value; }
	void     Use (Use_Type value)      { use = value; }
	void     Capacity (int value)      { capacity = value; }
	void     Max_Load (int value)      { max_load = value; }
	void     Occupancy (int value)     { occupancy = value; }
	void     Loading (int value)       { loading = value; }
	void     Unloading (int value)     { unloading = value; }
	void     Method (int value)        { method = value; }
	void     Min_Dwell (Dtime value)   { min_dwell = value; }
	void     Max_Dwell (Dtime value)   { max_dwell = value; }
	void     Grade_Func (int value)    { grade = value; }
	void     Fuel_Func (int value)     { fuel = value; }
	void     Fuel_Cap (int value)      { fuel_cap = value; }
	void     PCE (int value)           { pce = value; }
	void     Cells (int value)         { cells = value; }

	void     Length (double value)     { length = exe->Round (value); }
	void     Max_Speed (double value)  { max_speed = exe->Round (value); }
	void     Max_Accel (double value)  { max_accel = exe->Round (value); }
	void     Max_Decel (double value)  { max_decel = exe->Round (value); }
	void     Op_Cost (double value)    { op_cost = exe->Round (value); }
	void     Occupancy (double value)  { occupancy = exe->Round (value * 10.0); }
	void     Loading (double value)    { loading = exe->Round (value * 10.0); }
	void     Unloading (double value)  { unloading = exe->Round (value * 10.0); } 
	void     Fuel_Cap (double value)   { fuel_cap = exe->Round (value); }
	void     PCE (double value)        { pce = exe->Round (value); }


	void     Clear (void)
	{
		type = length = max_speed = max_accel = max_decel = capacity = max_load = occupancy = loading = unloading = method = min_dwell = max_dwell = op_cost = 0;
		use = ANY; grade = fuel = fuel_cap = 0; pce = 10; cells = 1; Notes_Data::Clear ();
	}
private:
	int      type;
	int      length;
	int      max_speed;
	int      max_accel;
	int      max_decel;
	int      op_cost;
	Use_Type use;
	int      capacity;
	int      max_load;
	int      occupancy;
	int      loading;
	int      unloading;
	int      method;
	Dtime    min_dwell; 
	Dtime    max_dwell; 
	int      grade;
	int      fuel;
	int      fuel_cap;
	int      pce;
	int      cells;
};

typedef vector <Veh_Type_Data>    Veh_Type_Array;
typedef Veh_Type_Array::iterator  Veh_Type_Itr;

#endif
