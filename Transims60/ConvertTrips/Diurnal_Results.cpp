//*********************************************************
//	Diurnal_Results.cpp - Output the Diurnal Allocations
//*********************************************************

#include "ConvertTrips.hpp"

#include "Execution_Service.hpp"

#include "fstream"

//---------------------------------------------------------
//	Diurnal_Results
//---------------------------------------------------------

void ConvertTrips::Diurnal_Results (void)
{
	int i, j, num, p, num2, periods;
	Dtime t, low, high;

	Share_Data *share_ptr;
	ConvertTrip_Itr group_itr;

	fstream &fh = diurnal_file.File ();

	fh << "MINUTE";
	periods = (Model_End_Time () - Model_Start_Time ()) / minute;

	for (group_itr = convert_group.begin (); group_itr != convert_group.end (); group_itr++) {
		num = group_itr->Num_Shares ();
		if (num < 1) num = 1;

		for (i=1; i <= num; i++) {
			fh << "\tSHARE" << group_itr->Group () << "_" << i;
			fh << "\tTARGET" << group_itr->Group () << "_" << i;
			fh << "\tTRIPS" << group_itr->Group () << "_" << i;
		}
	}
	fh << endl;

	//---- print the data ----

	for (p=0, t=0; p < periods; p++, t += minute) {
		fh << p;

		for (group_itr = convert_group.begin (); group_itr != convert_group.end (); group_itr++) {
			num = group_itr->Num_Shares ();
			if (num < 1) num = 1;

			num2 = group_itr->Diurnal_Periods ();

			for (i=0; i < num; i++) {

				for (j=0; j < num2; j++) {
					group_itr->Diurnal_Time_Range (j, low, high);

					if (low == t) {
						share_ptr = group_itr->Share_Ptr (j, i);

						fh << "\t" << share_ptr->Share ();
						fh << "\t" << share_ptr->Target ();
						fh << "\t" << share_ptr->Trips ();
						break;
					}
				}
				if (j == num2) {
					fh << "\t0\t0\t0";
				}
			}
		}
		fh << endl;
	}
	diurnal_file.Close ();
}
