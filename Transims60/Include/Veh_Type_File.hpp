//******************************************************** 
//	Veh_Type_File.hpp - Vehicle Type File Input/Output
//********************************************************

#ifndef VEH_TYPE_FILE_HPP
#define VEH_TYPE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Veh_Type_File Class definition
//---------------------------------------------------------

class SYSLIB_API Veh_Type_File : public Db_Header
{
public:
	Veh_Type_File (Access_Type access, string format);
	Veh_Type_File (string filename, Access_Type access, string format);
	Veh_Type_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Veh_Type_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Type (void)                { return (Get_Integer (type)); }
	double Length (void)              { return (Get_Double (length)); }
	double Max_Speed (void)           { return (Get_Double (max_speed)); }
	double Max_Accel (void)           { return (Get_Double (max_accel)); }
	double Max_Decel (void)           { return (Get_Double (max_decel)); }
	double Op_Cost (void)             { return (Get_Double (op_cost)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	int    Capacity (void)            { return (Get_Integer (capacity)); }
	int    Max_Load (void)            { return (Get_Integer (max_load)); }
	double Occupancy (void)           { return (Get_Double (occupancy)); }
	double Loading (void)             { return (Get_Double (load)); }
	double Unloading (void)           { return (Get_Double (unload)); }
	int    Method (void)              { return (Get_Integer (method)); }
	Dtime  Min_Dwell (void)           { return (Get_Time (min_dwell)); }
	Dtime  Max_Dwell (void)           { return (Get_Time (max_dwell)); }
	int    Grade_Func (void)          { return (Get_Integer (grade)); }
	int    Fuel_Func (void)           { return (Get_Integer (fuel)); }
	double Fuel_Cap (void)            { return (Get_Double (fuel_cap)); }

	void   Type (int value)           { Put_Field (type, value); }
	void   Length (double value)      { Put_Field (length, value); }
	void   Max_Speed (double value)   { Put_Field (max_speed, value); }
	void   Max_Accel (double value)   { Put_Field (max_accel, value); }
	void   Max_Decel (double value)   { Put_Field (max_decel, value); }
	void   Op_Cost (double value)     { Put_Field (op_cost, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Capacity (int value)       { Put_Field (capacity, value); }
	void   Max_Load (int value)       { Put_Field (max_load, value); }
	void   Occupancy (double value)   { Put_Field (occupancy, value); }
	void   Loading (double value)     { Put_Field (load, value); }
	void   Unloading (double value)   { Put_Field (unload, value); }
	void   Method (int value)         { Put_Field (method, value); }
	void   Min_Dwell (Dtime value)    { Put_Field (min_dwell, value); }
	void   Max_Dwell (Dtime value)    { Put_Field (max_dwell, value); }
	void   Grade_Func (int value)     { Put_Field (grade, value); }
	void   Fuel_Func (int value)      { Put_Field (fuel, value); }
	void   Fuel_Cap (double value)    { Put_Field (fuel_cap, value); }

	//---- Version 4.0 compatibility ----

	bool SubType_Flag (void)          { return (subtype >= 0); }
	int  SubType (void)               { return (Get_Integer (subtype)); }
	void SubType (int value)          { Put_Field (subtype, value); }

	bool Use_Flag (void)              { return (use >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int type, length, max_speed, max_accel, max_decel, op_cost, use, capacity, max_load;
	int occupancy, load, unload, method, min_dwell, max_dwell, subtype, grade, fuel, fuel_cap;
};

#endif
