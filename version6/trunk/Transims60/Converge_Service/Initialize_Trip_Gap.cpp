//*********************************************************
//	Initialize_Trip_Gap.cpp - initialize trip gap data
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Initialize_Trip_Gap
//---------------------------------------------------------

void Converge_Service::Initialize_Trip_Gap (void)
{
	//---- allocate memory for trip gap data ----

	if (save_trip_gap || trip_gap_map_flag) {
		int index;
		Plan_Itr plan_itr;
		Gap_Data gap_data;

		memset (&gap_data, '\0', sizeof (gap_data));

		gap_data_array.assign (dat->plan_array.size (), gap_data);

		if (dat->System_File_Flag (PLAN)) {
			Show_Message ("Initializing Trip Gap Memory");
			Set_Progress ();

			for (index=0, plan_itr = dat->plan_array.begin (); plan_itr != dat->plan_array.end (); plan_itr++, index++) {
				Show_Progress ();

				gap_data_array [index].previous = plan_itr->Impedance ();
			}
			End_Progress ();
		}
		memory_flag = true;
	}
}

//---------------------------------------------------------
//	Trip_Gap_Map_Parts
//---------------------------------------------------------

void Converge_Service::Trip_Gap_Map_Parts (int parts)
{
	if (parts > 0) {
		trip_gap_parts = true;
		num_parts = parts;
		trip_gap_map_array.Initialize (num_parts);
	} else {
		trip_gap_parts = false;
	}
}
