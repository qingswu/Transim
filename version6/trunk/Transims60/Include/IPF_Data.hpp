//*********************************************************
//	IPF_Data.hpp - iterative proportional fit class
//*********************************************************

#ifndef IPF_DATA_HPP
#define IPF_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Attribute_Data.hpp"

#include <math.h>

//---------------------------------------------------------
//	IPF_Data class definition
//---------------------------------------------------------

class SYSLIB_API IPF_Data
{
public:
	IPF_Data (void);
	virtual ~IPF_Data (void)              { Clear (); }

	bool   Add_Attribute (int num_types);
	bool   Set_Cells (void);
	bool   IPF (void);

	double Share (int cell)               { return ((Check_Cell (cell)) ? share [cell] : 0); }
	void   Share (int cell, double value) { if (Check_Cell (cell)) share [cell] = value; }
	void   Zero_Share (void);

	void   Max_Iterations (int value)     { max_iter = value; }
	void   Max_Difference (double value)  { max_diff = value; }

	int    Num_Cells (void)               { return ((int) share.size ()); }
	int    Num_Iterations (void)          { return (num_iter); }
	double Difference (void)              { return (difference); }

	Attribute_Data * Attribute (int num)  { return (&attribute [num]); }

	void   Clear (void)                   { Clear_Cells (); attribute.clear (); }

	bool   Num_Cells (int cells);

	Attribute_Array attribute;

private:
	bool   Check_Cell (int cell)          { return (cell > 0 && cell <= Num_Cells ()); }

	void   Clear_Cells (void)             { share.clear (); previous.clear (); }

	int    num_iter, max_iter;
	double max_diff, difference;
	Doubles share, previous;
};
#endif
