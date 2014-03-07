//*********************************************************
//	IPF_Data.cpp - iterative proportional fit
//*********************************************************

#include "IPF_Data.hpp"

//---------------------------------------------------------
//	IPF_Data constructor
//---------------------------------------------------------

IPF_Data::IPF_Data (void)
{
	max_iter = 10000;
	max_diff = 0.0000001;
	num_iter = 0;
	difference = 0.0;
}

//---- allocate cell memory ----

bool IPF_Data::Num_Cells (int cells)
{
	Clear_Cells ();

	share.assign (cells, 0.0);
	previous.assign (cells, 0.0);
	return (true);
}

//---- add attribute ----

bool IPF_Data::Add_Attribute (int types) 
{
	Attribute_Data data (types);
	attribute.push_back (data);
	return (true);
}

//---- define cells ----

bool IPF_Data::Set_Cells (void) 
{
	Attribute_Itr at_itr;
	int cell, index, num, base, cells, types;

	//---- calculate total number of cells ----

	cells = 1;

	for (at_itr = attribute.begin (); at_itr != attribute.end (); at_itr++) {
		types = at_itr->Num_Types ();
		if (types <= 0) return (false);
		cells *= types;
	}

	//---- calculate cell indices ----

	num = base = cells;

	for (at_itr = attribute.begin (); at_itr != attribute.end (); at_itr++) {
		if (!at_itr->Num_Cells (cells)) return (false);

		types = at_itr->Num_Types ();

		base /= types;

		for (cell=0; cell < cells; cell++) {
			index = (cell % num) / base;
			at_itr->Index (cell, index);
		}
		num = base;
	}
	return (Num_Cells (cells));
}

//---- zero share data ----

void IPF_Data::Zero_Share (void)
{
	int num_cells = Num_Cells ();
	if (num_cells > 0) {
		share.assign (num_cells, 0.0);
	}
}

//---- iterative proportional fit ----

bool IPF_Data::IPF (void) 
{
	int cell;
	double total;
	bool flag;

	Dbl_Itr db_itr;
	Attribute_Itr at_itr;

	if (Num_Cells () == 0) return (false);

	//---- normalize the data ----

	for (at_itr = attribute.begin (); at_itr != attribute.end (); at_itr++) {
		at_itr->Normalize ();
	}
	total = 0.0;

	for (db_itr = share.begin (); db_itr != share.end (); db_itr++) {
		total += *db_itr;
	}
	if (total != 1.0) {
		flag = (total == 0.0);
		if (flag) total = 1.0 / Num_Cells ();

		for (db_itr = share.begin (); db_itr != share.end (); db_itr++) {
			if (flag) {
				*db_itr = total;
			} else {
				*db_itr /= total;
			}
		}
	}

	//---- iterate until converence ----

	for (num_iter=1; num_iter <= max_iter; num_iter++) {

		//---- copy the current shares ----

		previous.assign (share.begin (), share.end ());

		//---- process each attribute ----

		for (at_itr = attribute.begin (); at_itr != attribute.end (); at_itr++) {
			at_itr->Zero_Total ();

			//---- sum the attribute totals ----

			for (cell=0, db_itr = share.begin (); db_itr != share.end (); db_itr++, cell++) {
				at_itr->Total_Cell (cell, *db_itr);
			}

			//---- calculate the correction factors ----

			if (at_itr->Factor_Total () != 0.0) {

				//---- apply the correction factors ----

				for (cell=0, db_itr = share.begin (); db_itr != share.end (); db_itr++, cell++) {
					*db_itr *= at_itr->Cell_Factor (cell);
				}
			}
		}

		//---- check the convergence criteria ----

		difference = 0.0;

		for (cell=0, db_itr = share.begin (); db_itr != share.end (); db_itr++, cell++) {
			difference += fabs (previous [cell] - *db_itr);
		}
		if (difference <= max_diff) return (true);
	}
	return (false);
}

