//*********************************************************
//	Attribute_Data.hpp - IPF attribute data class
//*********************************************************

#ifndef ATTRIBUTE_DATA_HPP
#define ATTRIBUTE_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Attribute_Data class definition
//---------------------------------------------------------

class SYSLIB_API Attribute_Data
{
public:
	Attribute_Data (int num_types = 0)         { Num_Types (num_types); }
	virtual ~Attribute_Data (void)             { Clear (); }

	int    Num_Types (void)                    { return ((int) target.size ()); }
	int    Num_Cells (void)                    { return ((int) index.size ()); }
	double Total (int type)                    { return (Check_Type (type) ? total [type] : 0); }
	double Target (int type)                   { return (Check_Type (type) ? target [type] : 0); }
	double Factor (int type)                   { return (Total (type)); }
	int    Index (int cell)                    { return (Check_Cell (cell) ? index [cell] : 0); }

	bool   Num_Types (int types);
	bool   Num_Cells (int cells);
	void   Total (int type, double value)      { if (Check_Type (type)) total [type] = value; }
	void   Target (int type, double value)     { if (Check_Type (type)) target [type] = value; }
	void   Factor (int type, double value)     { Total (type, value); }
	void   Index (int cell, int type)          { if (Check_Cell (cell)) index [cell] = type; }

	void   Add_Total (int type, double value)  { if (Check_Type (type)) total [type] += value; }
	void   Total_Cell (int cell, double value) { Add_Total (Index (cell), value); }
	double Cell_Factor (int cell)              { return (Factor (Index (cell))); }

	void   Normalize (void);
	void   Zero_Total (void);
	double Factor_Total (void);

	void   Clear (void)                        { Clear_Types (); Clear_Cells (); }

private:
	void   Clear_Types ()                      { target.clear (); total.clear (); }
	void   Clear_Cells ()                      { index.clear (); }

	bool   Check_Type (int type)               { return (type >= 0 && type < Num_Types ()); }
	bool   Check_Cell (int cell)               { return (cell >= 0 && cell <= Num_Cells ()); }

	Doubles total;
	Doubles target;
	Integers index;
};

typedef vector <Attribute_Data>    Attribute_Array;
typedef Attribute_Array::iterator  Attribute_Itr;

#endif
