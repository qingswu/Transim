//*********************************************************
//	Stop_Access - Build an access path to a transit station
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Stop_Access
//---------------------------------------------------------

bool Path_Builder::Stop_Access (void)
{
	//---- build the default path ----

	if (Plan_Build ()) {
		Dtime tod, time, wait, tot_wait0;
		Plan_Leg_Itr leg_itr;

		struct Transit_Leg {
			Dtime walk;
			Dtime wait;
			Dtime start;
			Dtime end;
			int   route;
			int   on;
			int   off;
			int   run;

			Transit_Leg (void) : walk(0), wait(0), start(0), end(0), route(0), on(0), off(0), run(0) {}
		};
		typedef vector <Transit_Leg>    TLeg_Array;
		typedef TLeg_Array::iterator    TLeg_Itr;

		TLeg_Array tleg_array;
		TLeg_Itr tleg_itr;
		Transit_Leg tleg;

		bool first;
		int stop, run;
		Line_Data *line_ptr;
		Line_Stop_Itr stop_itr;
		Line_Run_Itr run_itr;

//		best_impedance = plan_ptr->Impedance ();

		//---- gather transit legs ----

		tod = plan_ptr->Depart ();
		time = tot_wait0 = 0;

		for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
			tod += leg_itr->Time ();
			time += leg_itr->Time ();

			if (leg_itr->Type () == STOP_ID) {
				tleg.walk = time;
				tleg.on = leg_itr->ID ();
				
				//---- route ----

				leg_itr++;

				if (leg_itr == plan_ptr->end () || leg_itr->Mode () != WAIT_MODE || leg_itr->Type () != ROUTE_ID) {
					exe->Write (1, " plan problem");
				}
				tod += leg_itr->Time ();
				tleg.start = tod;
				tleg.wait = leg_itr->Time ();
				tleg.route = leg_itr->ID ();

				//---- alighting stop ----

				leg_itr++;

				if (leg_itr == plan_ptr->end () || leg_itr->Mode () != TRANSIT_MODE || leg_itr->Type () != STOP_ID) {
					exe->Write (1, " plan problem");
				}
				tod += leg_itr->Time ();
				tleg.end = tod;
				tleg.off = leg_itr->ID ();
				tleg.run = -1;

				line_ptr = &exe->line_array [tleg.route];
				first = true;

				for (stop=0, stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++, stop++) {
					if (first) {
						if (stop_itr->Stop () == tleg.on) {
							first = false;
							tleg.on = stop;

							for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
								if (run_itr->Schedule () <= tleg.start) {
									tleg.run = run;
								} else {
									break;
								}
							}
						}
					} else if (stop_itr->Stop () == tleg.off) {
						tleg.off = stop;
						break;
					}
				}
				if (tleg.run < 0) {
					//exe->Write (1, " hhold=") << plan_ptr->Household () << " run problem first=" << first << " start=" << tleg.start << " route=" << line_ptr->Route ();
					continue;
				}
				if (tleg_array.size () > 0) {
					tot_wait0 += tleg.wait;
				}

				//---- save the leg ----

				tleg_array.push_back (tleg);

				time = 0;
			}
		}

		//---- minimize the wait time ----

		if (tleg_array.size () > 0) {
			if (tleg_array.size () == 1) {
				tod = plan_ptr->Depart ();
				tleg_itr = tleg_array.begin ();

				wait = tleg_itr->wait - path_param.min_wait;

				if (tleg_itr->run > 0) {
					line_ptr = &exe->line_array [tleg_itr->route];

					stop_itr = line_ptr->begin () + tleg_itr->on;
					run_itr = stop_itr->begin () + tleg_itr->run - 1;

					time = run_itr->Schedule () - path_param.min_wait - tleg_itr->walk;

					if (tod - time < wait) {
						tod = time;
					} else {
						tod += wait;
					}
				} else {
					tod += wait;
				}
			} else {
				int best_run;
				Dtime tot_wait1, tot_wait2, best_wait;
				tot_wait1 = tot_wait2 = 0;

				//---- try an earlier run ----

				first = true;
				tod = 0;

				for (tleg_itr = tleg_array.begin (); tleg_itr != tleg_array.end (); tleg_itr++) {
					if (first) {
						if (tleg_itr->run < 1) {
							tot_wait1 = MAX_INTEGER;
							break;
						}
						first = false;

						line_ptr = &exe->line_array [tleg_itr->route];

						stop_itr = line_ptr->begin () + tleg_itr->off;
						run_itr = stop_itr->begin () + tleg_itr->run - 1;

						tod = run_itr->Schedule () + path_param.min_wait;
					} else {
						tod += tleg_itr->walk;

						line_ptr = &exe->line_array [tleg_itr->route];

						stop_itr = line_ptr->begin () + tleg_itr->on;

						best_run = -1;
						wait = MAX_INTEGER;

						for (run = tleg_itr->run; run >= 0; run--) {
							run_itr = stop_itr->begin () + run;

							if (run_itr->Schedule () < tod) break;

							best_run = run;
							wait = run_itr->Schedule () - tod;
						}
						if (best_run < 0) {
							tot_wait1 = MAX_INTEGER;
							break;
						}
						tot_wait1 += wait;

						stop_itr = line_ptr->begin () + tleg_itr->off;
						run_itr = stop_itr->begin () + best_run;

						tod = run_itr->Schedule () + path_param.min_wait;
					}
				}

				//---- try a later run ----

				first = true;
				tod = 0;

				for (tleg_itr = tleg_array.begin (); tleg_itr != tleg_array.end (); tleg_itr++) {
					if (first) {
						first = false;

						line_ptr = &exe->line_array [tleg_itr->route];

						stop_itr = line_ptr->begin () + tleg_itr->off;
						if (tleg_itr->run + 1 >= (int) stop_itr->size ()) {
							tot_wait2 = MAX_INTEGER;
							break;
						}
						run_itr = stop_itr->begin () + tleg_itr->run + 1;

						tod = run_itr->Schedule () + path_param.min_wait;
					} else {
						tod += tleg_itr->walk;

						line_ptr = &exe->line_array [tleg_itr->route];

						stop_itr = line_ptr->begin () + tleg_itr->on;

						best_run = -1;
						wait = MAX_INTEGER;

						for (run = tleg_itr->run; run < (int) stop_itr->size (); run++) {
							run_itr = stop_itr->begin () + run;

							if (run_itr->Schedule () < tod) continue;

							best_run = run;
							wait = run_itr->Schedule () - tod;
							break;
						}
						if (best_run < 0) {
							tot_wait2 = MAX_INTEGER;
							break;
						}
						tot_wait2 += wait;

						stop_itr = line_ptr->begin () + tleg_itr->off;
						run_itr = stop_itr->begin () + best_run;

						tod = run_itr->Schedule () + path_param.min_wait;
					}
				}

				//---- set the departure time ----

				tod = plan_ptr->Depart ();
				tleg_itr = tleg_array.begin ();

				line_ptr = &exe->line_array [tleg_itr->route];

				stop_itr = line_ptr->begin () + tleg_itr->on;

				if (tot_wait1 < tot_wait2) {
					if (tot_wait1 < tot_wait0) {
						run_itr = stop_itr->begin () + tleg_itr->run - 1;
					} else {
						run_itr = stop_itr->begin () + tleg_itr->run;	
					}
				} else if (tot_wait2 < tot_wait0) {
					run_itr = stop_itr->begin () + tleg_itr->run + 1;	
				} else {
					run_itr = stop_itr->begin () + tleg_itr->run;	
				}
				tod = run_itr->Schedule () - path_param.min_wait - tleg_itr->walk;
			}
			plan_ptr->Depart (tod);

			return (Plan_Build ());
		} 

	} else {
		Dtime best_start, start, first, last, increment;
		unsigned best_impedance = 0;

		if (plan_ptr->Depart () > 0) {
			start = plan_ptr->Depart ();
		} else {
			start = plan_ptr->Start ();
		}
		best_start = start;

		increment = Dtime (5, MINUTES);

		first = start - (increment * 10);
		if (first < 0) {
			first = 0;
		}
		last = start + (increment * 10);
		if (last > plan_ptr->End ()) {
			last = plan_ptr->End ();
		}
		plan_ptr->Constraint (START_TIME);

		for (start = first; start <= last; start += increment) {
			plan_ptr->Depart (start);

			if (Plan_Build ()) {
				if (best_impedance == 0 || plan_ptr->Impedance () < best_impedance) {
					best_impedance = plan_ptr->Impedance ();
					best_start = start;
				}
			}
		}
		if (best_impedance > 0) {
			plan_ptr->Depart (best_start);
			return (Plan_Build ());
		}
		return (false);
	}
	return (true);
}
