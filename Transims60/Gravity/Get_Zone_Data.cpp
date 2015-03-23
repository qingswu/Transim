//*********************************************************
//	Get_Zone_Data.cpp - Read the Zone File
//*********************************************************

#include "Gravity.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool Gravity::Get_Zone_Data (Zone_File &file, Zone_Data &data)
{
	double prod, attr;

	if (Data_Service::Get_Zone_Data (file, data)) {
		if (period_flag) {
			for (int i=0; i < num_periods; i++) {
				prod = file.Get_Double (prod_fields [i]);
				attr = file.Get_Double (attr_fields [i]);

				prod_periods [i].push_back (prod);
				attr_periods [i].push_back (attr);
			}
		} else {
			prod = file.Get_Double (prod_field);
			attr = file.Get_Double (attr_field);

			prod_array.push_back (prod);
			attr_array.push_back (attr);
		}
		return (true);
	}
	return (false);
}

