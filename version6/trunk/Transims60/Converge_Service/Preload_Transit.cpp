//*********************************************************
//	Preload_Transit.cpp - load transit vehicles to links
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Preload_Transit
//---------------------------------------------------------

void Converge_Service::Preload_Transit (void)
{
	int index;
	Dtime time, dwell, ttime;
	double pce, occ;

	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Driver_Itr driver_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Veh_Type_Data *veh_type_ptr;

	Show_Message (1, "Preloading Transit Route");
	Set_Progress ();

	//---- process each transit route ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		if (line_itr->size () == 0 || line_itr->begin ()->size () == 0) continue;
		Show_Progress ();

		veh_type_ptr = &veh_type_array [line_itr->Type ()];

		pce = UnRound (veh_type_ptr->PCE ());
		occ = veh_type_ptr->Occupancy () / 100.0;
		if (occ <= 0.0) occ = 1.0;

		dwell = (veh_type_ptr->Min_Dwell () + veh_type_ptr->Max_Dwell ()) / 2;
		if (dwell < Dtime (2, SECONDS)) dwell.Seconds (2);

		stop_itr = line_itr->begin ();

		for (run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {
			time = run_itr->Schedule ();

			for (driver_itr = line_itr->driver_array.begin (); driver_itr != line_itr->driver_array.end (); driver_itr++) {
				index = *driver_itr;

				dir_ptr = &dir_array [index];
				link_ptr = &link_array [dir_ptr->Link ()];

				if (dir_ptr->Use_Index () >= 0) {
					index = dir_ptr->Use_Index ();
				}
				ttime = perf_period_array.Flow_Time (index, time, 1.0, link_ptr->Length (), pce, occ) + dwell;
				if (ttime < 0) break;
				time += ttime;
			}
		}
	}
	End_Progress (false);
}
