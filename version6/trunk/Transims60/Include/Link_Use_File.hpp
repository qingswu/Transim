//******************************************************** 
//	Link_Use_File.hpp - Link Use File Input/Output
//********************************************************

#ifndef LINK_USE_FILE_HPP
#define LINK_USE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

#define NUM_VALUES	10

//---------------------------------------------------------
//	Link_Use_File Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Use_File : public Db_Header
{
public:
	Link_Use_File (Access_Type access, string format);
	Link_Use_File (string filename, Access_Type access, string format);
	Link_Use_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_Use_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Num_Values (void)              { return (num_values); }
	void   Num_Values (int num)           { num_values = MIN (num, NUM_VALUES); }
	
	int    Anode (void)                   { return (Get_Integer (anode)); }
	int    Bnode (void)                   { return (Get_Integer (bnode)); }
	int    Link (void)                    { return (Get_Integer (link)); }
	int    Dir (void)                     { return (Get_Integer (dir)); }
	int    Type (int i)                   { return (Get_Integer (type [i])); }
	int    Use (int i)                    { return (Get_Integer (use [i])); }
	string Period (int i)                 { return (Get_String (period [i])); }
	int    Lanes (int i)                  { return (Get_Integer (lanes [i])); }
	int    Toll (int i)                   { return (Get_Integer (toll [i])); }
	double Speed (int i)                  { return (Get_Double (speed [i])); }

	void   Anode (int value)              { Put_Field (anode, value); }
	void   Bnode (int value)              { Put_Field (bnode, value); }
	void   Link (int value)               { Put_Field (link, value); }
	void   Dir (int value)			      { Put_Field (dir, value); }
	void   Type (int i, int value)        { Put_Field (type [i], value); }
	void   Use (int i, int value)         { Put_Field (use [i], value); }
	void   Period (int i, char * value)   { Put_Field (period [i], value); }
	void   Period (int i, string &value)  { Put_Field (period [i], value); }
	void   Lanes (int i, int value)       { Put_Field (lanes [i], value); }
	void   Toll (int i, int value)        { Put_Field (toll [i], value); }
	void   Speed (int i, double value)    { Put_Field (speed [i], value); }
	
	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int num_values, anode, bnode, link, dir;
	int type [NUM_VALUES], use [NUM_VALUES], period [NUM_VALUES], lanes [NUM_VALUES], toll [NUM_VALUES], speed [NUM_VALUES];
};

#endif
