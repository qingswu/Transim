//*********************************************************
//	Line_Data.cpp - network transit line data classes
//*********************************************************

#include "Line_Data.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Vehicle_ID
//---------------------------------------------------------

int Line_Array::Vehicle_ID (int vehicles)
{
	int high_route, runs;
	Line_Itr itr;

	runs = 1;

	while (vehicles > 10) {
		vehicles /= 10;
		runs *= 10;
	}
	vehicles = (vehicles + 1) * runs;

	//---- find the highest route number ----

	high_route = num_runs = max_runs = 0;

	for (itr = begin (); itr != end (); itr++) {
		if (itr->Route () > high_route) high_route = itr->Route ();

		runs = (int) itr->begin ()->size ();

		num_runs += runs;
		if (runs > max_runs) max_runs = runs;
	}
	offset = 1;
	runs = max_runs;

	while (runs > 0) {
		runs /= 10;
		offset *= 10;
	}
	runs = 1;

	while (high_route > 0) {
		high_route /= 10;
		runs *= 10;
	}
	runs *= offset;

	if (runs > vehicles) {
		veh_id = runs;
	} else {
		veh_id = vehicles;
	}
	return (veh_id);
}

//---------------------------------------------------------
//	Sum_Ridership
//---------------------------------------------------------

void Line_Array::Sum_Ridership (Plan_Data &plan, bool id_flag)
{
	int board, alight, route, run;
	Dtime time, time2;

	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr, board_itr, alight_itr;
	Line_Run_Itr run_itr;
	Line_Run *run_ptr;
	Plan_Leg_Itr leg_itr;
	Int_Map_Itr map_itr;

	board = alight = route = -1;

	time = plan.Depart ();
	time2 = 0;

	for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++, time += time2) {
		time2 = leg_itr->Time ();

		if (leg_itr->Type () == STOP_ID) {
			if (leg_itr->Mode () == TRANSIT_MODE) {
				alight = leg_itr->ID ();

				if (!id_flag) {
					map_itr = dat->stop_map.find (alight);
					if (map_itr == dat->stop_map.end ()) continue;

					alight = map_itr->second;

					map_itr = dat->stop_map.find (board);
					if (map_itr == dat->stop_map.end ()) continue;

					board = map_itr->second;

					map_itr = dat->line_map.find (route);
					if (map_itr == dat->line_map.end ()) continue;

					line_ptr = &at (map_itr->second);
				} else {
					if (route < 0) continue;
#ifdef CHECK
					if (route < 0 || route >= (int) size ()) exe->Error ("Line_Data::Sum_Ridership: route");
#endif
					line_ptr = &at (route);
				}

				//---- find the boarding and alighting locations ----

				for (board_itr = line_ptr->begin (); board_itr != line_ptr->end (); board_itr++) {
					if (board_itr->Stop () == board) break;
				}
				if (board_itr == line_ptr->end ()) continue;

				for (alight_itr = board_itr; alight_itr != line_ptr->end (); alight_itr++) {
					if (alight_itr->Stop () == alight) break;
					if (alight_itr->Stop () == board) {
						board_itr = alight_itr;
					}
				}
				if (alight_itr == line_ptr->end ()) continue;

				//---- find the run number ----

				for (run=0, run_itr = board_itr->begin (); run_itr != board_itr->end (); run_itr++, run++) {
					if (time <= run_itr->Schedule ()) break;
				}
				if (run_itr == board_itr->end ()) continue;

				//---- load the trip ----

				for (stop_itr = board_itr; stop_itr <= alight_itr; stop_itr++) {
					run_ptr = &stop_itr->at (run);
					if (stop_itr == board_itr) {
						run_ptr->Add_Board ();
					}
					if (stop_itr == alight_itr) {
						run_ptr->Add_Alight ();
					} else {
						run_ptr->Add_Load ();
					}
				}
				board = alight = route = -1;
			} else {
				board = leg_itr->ID ();
			}
		} else if (leg_itr->Type () == ROUTE_ID) {
			route = leg_itr->ID ();
		}
	}
}

//---------------------------------------------------------
//	Clear_Ridership
//---------------------------------------------------------

void Line_Array::Clear_Ridership (void)
{
	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr, run2_itr;

	for (line_itr = begin (); line_itr != end (); line_itr++) {
		for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
			for (run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {
				run_itr->Board (0);
				run_itr->Alight (0);
				run_itr->Load (0);
				run_itr->Factor (0);
			}
		}
	}
}

//---------------------------------------------------------
//	Ridership_Gap
//---------------------------------------------------------

double Line_Array::Ridership_Gap (bool cap_flag, double factor)
{
	int r, capacity, cap, max_load, max, penalty;
	Dtime wait;
	double factor1, delay_fac, delay1, delay2;
	bool type_flag = false;

	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr, run2_itr;
	Veh_Type_Data *veh_type_ptr;

	if (cap_flag && !dat->System_File_Flag (VEHICLE_TYPE)) {
		cap_flag = false;
	}
	capacity = max_load = 2;
	factor1 = factor + 1.0;
	delay1 = delay2 = 0;

	for (line_itr = begin (); line_itr != end (); line_itr++) {
		if (cap_flag) {
			veh_type_ptr = &dat->veh_type_array [line_itr->Type ()];
			capacity = veh_type_ptr->Capacity ();
			max_load = veh_type_ptr->Max_Load ();
		}
		for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
			for (r=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, r++) {
				penalty = run_itr->Penalty ();
				delay1 += penalty;

				run_itr->Penalty (0);

				if (cap_flag && (run_itr->Load () > 0 || penalty > 0)) {
					if (type_flag) {
						veh_type_ptr = &dat->veh_type_array [line_itr->Run_Type (r)];
						cap = veh_type_ptr->Capacity ();
						max = veh_type_ptr->Max_Load ();
					} else {
						cap = capacity;
						max = max_load;
					}
					if (run_itr->Load () > cap && cap > 0) {
						if (max <= cap) max = cap + 1;

						if (run_itr->Load () >= max) {
							delay_fac = 1.0;
						} else {
							delay_fac = (double) (run_itr->Load () - cap) / (max - cap);
						}
						run2_itr = run_itr + 1;
						if (run2_itr != stop_itr->end ()) {
							wait = run2_itr->Schedule () - run_itr->Schedule ();
						} else if (run_itr > stop_itr->begin ()) {
							run2_itr = run_itr - 1;
							wait = run_itr->Schedule () - run2_itr->Schedule ();
						} else {
							wait = Dtime (60.0, MINUTES);
						}
					} else {
						wait = 0;
						delay_fac = 0.0;
					}
					penalty = (int) ((penalty * factor + wait * delay_fac) / factor1);
					delay2 += penalty;

					run_itr->Penalty (penalty);
				}
				run_itr->Board (0);
				run_itr->Alight (0);
				run_itr->Load (0);
				run_itr->Factor (0);
			}
		}
	}
	if (delay1 == delay2) {
		return (0.0);
	} else if (delay1 == 0.0) {
		return (1.0);
	} else if (delay2 > delay1) {
		return ((delay2 - delay1) / delay1);
	} else {
		return ((delay1 - delay2) / delay1);
	}
}
