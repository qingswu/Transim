//*********************************************************
//	Link_Selection - check link selection criteria
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Link_Selection
//---------------------------------------------------------

bool RiderSum::Link_Selection (Line_Data *line_ptr)
{
	if (!select_links) return (true);

	int n1, n2, num;
	Driver_Itr driver_itr;
	Data_Range_Itr range_itr;

	for (range_itr = link_ranges.begin (); range_itr != link_ranges.end (); range_itr++) {
		num = range_itr->Max_Count ();
		n1 = n2 = 0;

		for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {

			//---- check the link sequence ----

			n2 = range_itr->Range_Count (*driver_itr);
			if (!n2) continue;

			if (n2 != n1 + 1) {
				n2 = 0;
				break;
			}
			if (n2 == num) break;
			n1 = n2;
		}
		if (n2 == num) return (true);
	}
	return (false);
}
