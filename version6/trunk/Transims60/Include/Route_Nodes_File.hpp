//******************************************************** 
//	Route_Nodes_File.hpp - Route Nodes Input/Output
//********************************************************

#ifndef ROUTE_NODES_FILE_HPP
#define ROUTE_NODES_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Route_Nodes_File Class definition
//---------------------------------------------------------

class SYSLIB_API Route_Nodes_File : public Db_Header
{
public:
	Route_Nodes_File (Access_Type access, string format);
	Route_Nodes_File (string filename, Access_Type access, string format);
	Route_Nodes_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Route_Nodes_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Route (void)               { return (Get_Integer (route)); }
	int    Mode (void)                { return (Get_Integer (mode)); }
	int    Veh_Type (void)            { return (Get_Integer (veh_type)); }
	int    Oneway (void)              { return (Get_Integer (oneway)); }
	int    Nodes (void)               { return (Get_Integer (nodes)); }
	string Name (void)                { return (Get_String (name)); }
	int    Node (void)                { return (Get_Integer (node)); }
	int    Type (void)                { return (Get_Integer (type)); }
	Dtime  Dwell (void)               { return (Get_Time (dwell)); }
	Dtime  Time (void)                { return (Get_Time (time)); }
	double Speed (void)               { return (Get_Double (speed)); }

	Dtime  Headway (int num)          { return (Get_Time (headway [num])); }
	Dtime  Offset (int num)           { return (Get_Time (offset [num])); }
	Dtime  TTime (int num)            { return (Get_Time (ttime [num])); }
	int    Pattern (int num)          { return (Get_Integer (pattern [num])); }

	void   Route (int value)          { Put_Field (route, value); }
	void   Mode (int value)           { Put_Field (mode, value); }
	void   Veh_Type (int value)       { Put_Field (veh_type, value); }
	void   Oneway (int value)         { Put_Field (oneway, value); }
	void   Oneway (bool flag)         { Put_Field (oneway, ((flag) ? 1 : 2)); }
	void   Nodes (int value)          { Put_Field (nodes, value); }
	void   Name (char * value)        { Put_Field (name, value); }
	void   Name (string value)        { Put_Field (name, value); }
	void   Node (int value)           { Put_Field (node, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Dwell (Dtime value)        { Put_Field (dwell, value); }
	void   Time (Dtime value)         { Put_Field (time, value); }
	void   Speed (double value)       { Put_Field (speed, value); }
	
	void   Headway (int n, Dtime t)   { Put_Field (headway [n], t); }
	void   Offset (int n, Dtime t)    { Put_Field (offset [n], t); }
	void   TTime (int n, Dtime t)     { Put_Field (ttime [n], t); }
	void   Pattern (int n, int p)     { Put_Field (pattern [n], p); }

	int    Num_Periods (void)         { return (num_periods); }
	void   Num_Periods (int value)    { num_periods = MIN (value, 24); }
	
	bool   TTime_Flag (void)          { return (ttime_flag); }
	void   TTime_Flag (bool flag)     { ttime_flag = flag; }
	
	bool   Offset_Flag (void)         { return (offset_flag); }
	void   Offset_Flag (bool flag)    { offset_flag = flag; }

	bool   Pattern_Flag (void)        { return (pattern_flag); }
	void   Pattern_Flag (bool flag)   { pattern_flag = flag; }

	bool   Dwell_Flag (void)          { return (dwell_flag); }
	void   Dwell_Flag (bool flag)     { dwell_flag = flag; }

	bool   Time_Flag (void)           { return (time_flag); }
	void   Time_Flag (bool flag)      { time_flag = flag; }

	bool   Speed_Flag (void)          { return (speed_flag); }
	void   Speed_Flag (bool flag)     { speed_flag = flag; }

	bool   Type_Flag (void)           { return (type_flag); }
	void   Type_Flag (bool flag)      { type_flag = flag; }

	virtual bool Create_Fields (void);

	virtual Format_Type Dbase_Format (void)           { return (Db_Base::Dbase_Format ()); }
	virtual void  Dbase_Format (Format_Type type)     { return (Db_Base::Dbase_Format (type)); }
	virtual void  Dbase_Format (string text);

protected:

	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	bool dwell_flag, time_flag, speed_flag, type_flag, ttime_flag, offset_flag, pattern_flag;
	int num_periods;
	int route, mode, veh_type, oneway, nodes, name, node, type, dwell, time, speed;
	int headway [24], offset [24], ttime [24], pattern [24];
};

#endif
