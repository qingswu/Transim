//******************************************************** 
//	Approach_Link_File.hpp - Approach Link File Input/Output
//********************************************************

#ifndef APPROACH_LINK_FILE_HPP
#define APPROACH_LINK_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Approach_Link_File Class definition
//---------------------------------------------------------

class SYSLIB_API Approach_Link_File : public Db_Header
{
public:
	Approach_Link_File (Access_Type access, string format);
	Approach_Link_File (string filename, Access_Type access, string format);
	Approach_Link_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Approach_Link_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Control (void)             { return (Get_Integer (control)); }
	int    Group (void)               { return (Get_Integer (group)); }
	double LM_Length (void)           { return (Get_Double (lm_length)); }    
	int    Left_Merge (void)          { return (Get_Integer (left_merge)); }
	double LT_Length (void)           { return (Get_Double (lt_length)); }
	int    Left (void)                { return (Get_Integer (left)); }
	int    Left_Thru (void)           { return (Get_Integer (left_thru)); }
	int    Thru (void)                { return (Get_Integer (thru)); }
	int    Right_Thru (void)          { return (Get_Integer (right_thru)); }
	int    Right (void)               { return (Get_Integer (right)); }
	double RT_Length (void)           { return (Get_Double (rt_length)); }
	int    Right_Merge (void)         { return (Get_Integer (right_merge)); }
	double RM_Length (void)           { return (Get_Double (rm_length)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Control (int value)        { Put_Field (control, value); }
	void   Group (int value)          { Put_Field (group, value); }
	void   LM_Length (double value)   { Put_Field (lm_length, value); }
	void   Left_Merge (int value)     { Put_Field (left_merge, value); }
	void   LT_Length (double value)   { Put_Field (lt_length, value); }
	void   Left (int value)           { Put_Field (left, value); }
	void   Left_Thru (int value)      { Put_Field (left_thru, value); }
	void   Thru (int value)           { Put_Field (thru, value); }
	void   Right_Thru (int value)     { Put_Field (right_thru, value); }
	void   Right (int value)          { Put_Field (right, value); }
	void   RT_Length (double value)   { Put_Field (rt_length, value); }
	void   Right_Merge (int value)    { Put_Field (right_merge, value); }
	void   RM_Length (double value)   { Put_Field (rm_length, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, control, group;
	int left_merge, left, left_thru, thru, right_thru, right, right_merge;
	int lm_length, lt_length, rt_length, rm_length;
};

#endif
