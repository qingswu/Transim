//*********************************************************
//	Attribute_Data.cpp - household attribute data class
//*********************************************************

#include "Attribute_Data.hpp"

#include <math.h>

//---- allocate memory for attribute data ----

bool Attribute_Data::Num_Types (int types)
{
	Clear_Types ();

	if (types > 0) {
		total.assign (types, 0.0);
		target.assign (types, 0.0);
	}
	return (true);
}

//---- allocate memory for cell matrix ----

bool Attribute_Data::Num_Cells (int cells)
{
	Clear_Cells ();

	if (cells > 0) {
		index.assign (cells, 0);
	}
	return (true);
}

//---- normalize target ----

void Attribute_Data::Normalize (void)
{
	double total = 0.0;
	Dbl_Itr itr;

	for (itr = target.begin (); itr != target.end (); itr++) {
		total += *itr;
	}
	if (total > 0.0 && total != 1.0) {
		for (itr = target.begin (); itr != target.end (); itr++) {
			*itr /= total;
		}
	}
}

//---- zero the attribute totals ----

void Attribute_Data::Zero_Total (void)
{
	int num_types = Num_Types ();
	if (num_types > 0) {
		total.assign (num_types, 0.0);
	}
}

//---- calculate adjustment factors ----

double Attribute_Data::Factor_Total (void)
{
	double diff = 0.0;
	Dbl_Itr itr1, itr2;

	for (itr1 = total.begin (), itr2 = target.begin (); itr1 != total.end (); itr1++, itr2++) {
		diff += fabs (*itr1 - *itr2);
		if (*itr1 != 0.0) *itr1 = *itr2 / *itr1;
	}
	return (diff);
}
