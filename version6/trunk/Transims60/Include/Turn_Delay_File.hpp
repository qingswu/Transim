//******************************************************** 
//	Turn_Delay_File.hpp - Turn Movement File Input/Output
//********************************************************

#ifndef TURN_DELAY_FILE_HPP
#define TURN_DELAY_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Turn_Delay_File Class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Delay_File : public Db_Header
{
public:
	Turn_Delay_File (Access_Type access, string format);
	Turn_Delay_File (string filename, Access_Type access, string format);
	Turn_Delay_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Turn_Delay_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Node (void)               { return (Get_Integer (node)); }
	int    Link (void)               { return (Get_Integer (link)); }
	int    To_Link (void)            { return (Get_Integer (to_link)); }
	Dtime  Start (void)              { return (Get_Time (start)); }
	Dtime  End (void)                { return (Get_Time (end)); }
	double Turn (void)               { return (Get_Double (turn)); }
	double Time (void)               { return (Get_Double (time)); }

	void   Node (int value)          { Put_Field (node, value); }	
	void   Link (int value)          { Put_Field (link, value); }
	void   To_Link (int value)       { Put_Field (to_link, value); }
	void   Start (Dtime value)       { Put_Field (start, value); }
	void   End (Dtime value)         { Put_Field (end, value); }
	void   Turn (double value)       { Put_Field (turn, value); }
	void   Time (double value)       { Put_Field (time, value); }
	void   Time (Dtime value)        { Put_Field (time, value.Seconds ()); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int node, link, to_link, start, end, turn, time;
};

#endif
