//*********************************************************
//	Map_Trip_Plan.cpp - link trip and plan records
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Map_Trip_Plan
//---------------------------------------------------------

void Router::Map_Trip_Plan (void)
{
	Input_Trips ();

	//---- allocate memory for trip gap data ----

	if (save_trip_gap || trip_gap_map_flag) {
		int index;
		Plan_Itr plan_itr;
		Gap_Data gap_data;

		memset (&gap_data, '\0', sizeof (gap_data));

		gap_data_array.assign (plan_array.size (), gap_data);

		if (plan_flag) {
			Show_Message ("Initializing Trip Gap Memory");
			Set_Progress ();

			for (index=0, plan_itr = plan_array.begin (); plan_itr != plan_array.end (); plan_itr++, index++) {
				Show_Progress ();

				gap_data_array [index].previous = plan_itr->Impedance ();
			}
			End_Progress ();
		}
	}
}
