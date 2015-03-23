//*********************************************************
//	Read_Trips.cpp - trip distribution data
//*********************************************************

#include "Gravity.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void Gravity::Read_Trips (void)
{
	int period, index, org, des;
	double trips, skim;

	Int_Map *org_ptr, *des_ptr;
	Int_Map_Itr org_itr, des_itr;

	trip_file->Allocate_Data ();

	org_ptr = skim_file->Org_Map ();
	des_ptr = skim_file->Des_Map ();

	org_tot.assign (org_ptr->size (), 0);
	des_tot.assign (des_ptr->size (), 0);

	Show_Message (String ("Reading %s -- Record") % trip_file->File_Type ());
	Set_Progress ();

	for (period = 0; period < num_periods; period++) {
		for (org=0, org_itr = org_ptr->begin (); org_itr != org_ptr->end (); org_itr++, org++) {
			Show_Progress ();

			trip_file->Read_Row (org_itr->first, period);

			for (des=0, des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++, des++) {

				trip_file->Get_Cell (des_itr->first, trip_field, trips);

				if (trips > 0) {
					org_tot [org] += trips;
					des_tot [des] += trips; 

					if (in_len_flag || calib_flag) {
						skim_file->Get_Cell_Index (period, org_itr->second, des_itr->second, skim_field, skim);

						skim *= units_factor;
						
						if (skim > max_value) skim = max_value;
						index = (int) (skim / increment + 0.5);
						
						in_distb [index] += trips;
						total_in += trips;
					}
				}
			}
		}
	}
	End_Progress ();

	trip_file->Close ();
}
