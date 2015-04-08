//*********************************************************
//	Stop_Board.cpp - process boarding options at a stop
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Stop_Board
//---------------------------------------------------------

bool Path_Builder::Stop_Board (Transit_Path_Index path_index, Path_End_Array *to_ptr, bool best_flag)
{
	int route, stop, s, nstop, index, path, wait, run, imped, xfer, to, to_stop, from_offset;	
	int from_cost, from_len, from_walk, to_cost, to_len, to_walk, wait_cost, wait_len, wait_walk, len;
	unsigned from_imp, wait_imp, to_imp, hi_imp;
	Dtime from_time, wait_time, to_time, min_time, ttime, penalty;
	double factor;
	bool to_flag, exit_flag;

	Path_End_Itr to_itr;
	Path_Ptr path_ptr;
	Path_Itr path_itr;
	Line_Data *line_ptr;
	Line_Stop *line_stop_ptr, *to_stop_ptr;
	Line_Run_Itr run_itr;
	Line_Run_RItr run_ritr;
	Line_Run *run_ptr;
	Route_Stop_Data *route_stop_ptr;
	Stop_Data *stop_ptr;

	exit_flag = false;

	//---- initialize the path building ----

	stop = path_index.Index ();
	path = path_index.Path ();

	//---- get the current path data ----

	path_ptr = &board_path [path] [stop];
	path_ptr->Status (2);

	from_imp = path_ptr->Imped ();

	if (path_param.stop_imped > 0 || path_param.station_imped > 0) {
		stop_ptr = &exe->stop_array [stop];

		if (stop_ptr->Type () == STOP) {
			from_imp += path_param.stop_imped;
		} else if (stop_ptr->Type () == STATION) {
			from_imp += path_param.station_imped;
		}
	}
	xfer = path_ptr->Xfer () + 1;

	if (xfer > 0) {
		from_imp += path_param.xfer_imped;
	}
	if (from_imp >= max_imp) return (exit_flag);

	//---- check the time limits ----
		
	from_time = path_ptr->Time ();

	if (forward_flag) {
		if (from_time >= time_limit) {
			time_flag = true;
			return (exit_flag);
		}
		min_time = from_time + path_param.min_wait;
	} else {
		if (from_time <= time_limit) {
			time_flag = true;
			return (exit_flag);
		}
		min_time = from_time;
		if (xfer > 0) min_time -= path_param.min_wait;
	}

	//---- check max walk ----

	from_walk = path_ptr->Walk ();

	if (walk_flag && from_walk >= path_param.max_walk) {
		length_flag = true;
		return (exit_flag);
	}
	from_len = path_ptr->Length ();
	from_cost = path_ptr->Cost ();

	if (xfer >= path_param.max_xfers) {
		transfer_flag = true;
		return (exit_flag);
	}

	//---- check all routes leaving the stop ----

	for (index = exe->stop_list [stop]; index >= 0; index = route_stop_ptr->Next ()) {
		route_stop_ptr = &exe->route_stop_array [index];

		route = route_stop_ptr->Route ();
		s = route_stop_ptr->Stop ();

		line_ptr = &exe->line_array [route];

		nstop = (int) line_ptr->size ();
		line_stop_ptr = &line_ptr->at (s);

		from_offset = line_stop_ptr->Length ();

		if (forward_flag) {
			if (s == nstop - 1) continue;

			for (run=0, run_itr = line_stop_ptr->begin (); run_itr != line_stop_ptr->end (); run_itr++, run++) {

				penalty = (path_param.transit_penalty) ? run_itr->Penalty () : 0;

				if ((run_itr->Schedule () - penalty) < min_time) continue;

				wait_time = run_itr->Schedule ();

				if (wait_time >= time_limit) {
					time_flag = true;
					break;
				}
				wait = wait_time - from_time;
				if (wait_flag && wait > path_param.max_wait) {
					wait_time_flag = true;
					break;
				}
				if (xfer > 0) {
					imped = DTOI (wait * path_param.value_xfer);
				} else {
					imped = DTOI (wait * path_param.value_wait);
				}
				if (wait_flag && wait > path_param.wait_pen) {
					factor = (double) wait / path_param.wait_pen;
					factor = ((factor * factor) - 1) * path_param.wait_fac + 1;
					imped = DTOI (imped * factor);
				}
				wait_imp = from_imp + imped;
				if (wait_imp > max_imp) break;

				wait_cost = from_cost;
				wait_walk = from_walk;
				wait_len = from_len;

				//---- FARE ----

				to_cost = from_cost;
				to_walk = from_walk;

				//---- check legs to alighting stops ----

				for (++s; s < nstop; s++) {
					to_stop_ptr = &line_ptr->at (s);
					run_ptr = &to_stop_ptr->at (run);

					to_time = run_ptr->Schedule ();

					if (to_time > time_limit) {
						time_flag = true;
						break;
					}
					ttime = to_time - wait_time;

					imped = DTOI (ttime * path_param.value_time);

					//---- apply mode bias ----

					if (line_ptr->Mode () <= EXPRESS_BUS) {
						if (bus_bias_flag) {
							imped = DTOI (imped * path_param.bus_bias + path_param.bus_const);
							if (imped < 1) imped = 1;
						}
					} else if (line_ptr->Mode () == BRT) {
						if (brt_bias_flag) {
							imped = DTOI (imped * path_param.brt_bias + path_param.brt_const);
							if (imped < 1) imped = 1;
						}
					} else if (rail_bias_flag) {
						imped = DTOI (imped * path_param.rail_bias + path_param.rail_const);
						if (imped < 1) imped = 1;
					}
					to_imp = wait_imp + imped;

					len = to_stop_ptr->Length () - from_offset;
					to_imp += DTOI (len * path_param.value_dist);

					if (to_imp >= max_imp) continue;

					to_stop = to_stop_ptr->Stop ();
					to_len = from_len + len;

					//---- check for a destintion stop ----

					if (stop_to_flag && stop_to [to_stop] == 1) {
						to_flag = false;

						for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
							if (to_itr->Type () != STOP_ID || to_stop != to_itr->Index ()) continue;

							if (to_imp + imp_diff >= max_imp) continue;
					
							path_ptr = &to_itr->back ();
					
							if (to_imp >= path_ptr->Imped ()) continue;

							//---- save the destination totals ----

							path_ptr->Imped (to_imp);
							path_ptr->Time (to_time);
							path_ptr->Length (to_len);
							path_ptr->Cost (to_cost);
							path_ptr->Walk (to_walk);
							path_ptr->Mode (TRANSIT_MODE);
							path_ptr->From (route);
							path_ptr->Type (ROUTE_ID);
							path_ptr->Dir (0);
							path_ptr->Path (path);
							path_ptr->Xfer (xfer);
							path_ptr->Run (run);
							path_ptr->Status (1);

							//---- add the wait data ----
					
							path_ptr = &wait_path [path] [to_stop];

							path_ptr->Imped (wait_imp);
							path_ptr->Time (wait_time);
							path_ptr->Length (wait_len);
							path_ptr->Cost (wait_cost);
							path_ptr->Walk (wait_walk);
							path_ptr->Mode (WAIT_MODE);
							path_ptr->From (stop);
							path_ptr->Type (STOP_ID);
							path_ptr->Dir (0);
							path_ptr->Path (path);
							path_ptr->Xfer (xfer);
							path_ptr->Status (1);

							to_flag = true;

							if (best_flag) {
								max_imp = to_imp + imp_diff;
								exit_flag = true;
							}
						}

						//---- update the max impedance ----

						if (to_flag) {
							if (best_flag) continue;
							hi_imp = 0;

							for (to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++) {
								path_ptr = &to_itr->back ();

								if (path_ptr->Imped () < MAX_IMPEDANCE) {
									to_imp = path_ptr->Imped () + imp_diff;
									if (to_imp > hi_imp) hi_imp = to_imp;
								} else {
									hi_imp = MAX_IMPEDANCE;
									break;
								}
							}
							if (hi_imp > 0 && hi_imp < max_imp) max_imp = hi_imp;
							exit_flag = true;
						}
					}

					//---- consider adding the stop to the queue ----
			
					path_ptr = &alight_path [path] [to_stop];

					if (to_imp >= path_ptr->Imped ()) continue;

					//---- add the new path to the queue ----
					
					path_index.Index (to_stop);
					path_index.Type (ROUTE_ID);
					path_index.Path (path);

					if (path_param.sort_method) {
						if (path_ptr->Status () == 1) {
							transit_sort.Update (path_index, to_imp);
						} else {
							transit_sort.Insert (path_index, to_imp);
						}
					} else {
						if (path_ptr->Status () == 0) {
							next_index.push_back (path_index);
						} else if (path_ptr->Status () == 2) {
							next_index.push_front (path_index);
						}
					}
					path_ptr->Imped (to_imp);
					path_ptr->Time (to_time);
					path_ptr->Length (to_len);
					path_ptr->Cost (to_cost);
					path_ptr->Walk (to_walk);
					path_ptr->Mode (TRANSIT_MODE);
					path_ptr->From (route);
					path_ptr->Type (ROUTE_ID);
					path_ptr->Dir (0);
					path_ptr->Path (path);
					path_ptr->Xfer (xfer);
					path_ptr->Run (run);
					path_ptr->Status (1);

					//---- add the wait data ----
			
					path_ptr = &wait_path [path] [to_stop];

					path_ptr->Imped (wait_imp);
					path_ptr->Time (wait_time);
					path_ptr->Length (wait_len);
					path_ptr->Cost (wait_cost);
					path_ptr->Walk (wait_walk);
					path_ptr->Mode (WAIT_MODE);
					path_ptr->From (stop);
					path_ptr->Type (STOP_ID);
					path_ptr->Dir (0);
					path_ptr->Path (path);
					path_ptr->Xfer (xfer);
					path_ptr->Status (1);
				}
				break;
			}

		} else {	//---- backwards ----

			if (s == 0) continue;

			run = (int) line_stop_ptr->size () - 1;

			for (run_ritr = line_stop_ptr->rbegin (); run_ritr != line_stop_ptr->rend (); run_ritr++, run--) {

				penalty = (path_param.transit_penalty) ? run_ritr->Penalty () : 0;

				if ((run_ritr->Schedule () + penalty) > min_time) continue;

				wait_time = run_ritr->Schedule ();

				if (wait_time < time_limit) {
					time_flag = true;
					break;
				}
				wait = from_time - wait_time;

				if (wait_flag && wait > path_param.max_wait) {
					wait_time_flag = true;
					break;
				}
				if (xfer > 0) {
					imped = DTOI (wait * path_param.value_xfer);
				} else {
					imped = DTOI (wait * path_param.value_wait);
				}
				if (wait_flag && wait > path_param.wait_pen) {
					factor = (double) wait / path_param.wait_pen;
					factor = ((factor * factor) - 1) * path_param.wait_fac + 1;
					imped = DTOI (imped * factor);
				}
				wait_imp = from_imp + imped;
				if (wait_imp > max_imp) break;

				wait_cost = from_cost;
				wait_walk = from_walk;
				wait_len = from_len;

				//---- FARE ----

				to_cost = from_cost;
				to_walk = from_walk;

				//---- check legs to boarding stops ----

				for (--s; s >= 0; s--) {
					to_stop_ptr = &line_ptr->at (s);
					run_ptr = &to_stop_ptr->at (run);

					to_time = run_ptr->Schedule ();

					if ((to_time - path_param.min_wait) < time_limit) {
						time_flag = true;
						continue;
					}
					ttime = run_ritr->Schedule () - to_time; 

					imped = DTOI (ttime * path_param.value_time);

					//---- apply mode bias ----

					if (line_ptr->Mode () <= EXPRESS_BUS) {
						if (bus_bias_flag) {
							imped = DTOI (imped * path_param.bus_bias + path_param.bus_const);
							if (imped < 1) imped = 1;
						}
					} else if (line_ptr->Mode () == BRT) {
						if (brt_bias_flag) {
								imped = DTOI (imped * path_param.brt_bias + path_param.brt_const);
								if (imped < 1) imped = 1;
						}
					} else if (rail_bias_flag) {
						imped = DTOI (imped * path_param.rail_bias + path_param.rail_const);
						if (imped < 1) imped = 1;
					}
					to_imp = wait_imp + imped;

					len = from_offset - to_stop_ptr->Length ();
					to_imp += DTOI (len * path_param.value_dist);

					if (to_imp >= max_imp) continue;

					to_stop = to_stop_ptr->Stop ();
					to_len = from_len + len;

					//---- check for a destintion stop ----

					if (stop_to_flag && stop_to [to_stop] == 1) {
						to_flag = false;

						for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
							if (to_itr->Type () != STOP_ID || to_stop != to_itr->Index ()) continue;

							if (to_imp + imp_diff >= max_imp) continue;
					
							path_ptr = &to_itr->back ();
					
							if (to_imp >= path_ptr->Imped ()) continue;

							//---- save the destination totals ----

							path_ptr->Imped (to_imp);
							path_ptr->Time (to_time);
							path_ptr->Length (to_len);
							path_ptr->Cost (to_cost);
							path_ptr->Walk (to_walk);
							path_ptr->Mode (TRANSIT_MODE);
							path_ptr->From (route);
							path_ptr->Type (ROUTE_ID);
							path_ptr->Dir (1);
							path_ptr->Path (path);
							path_ptr->Xfer (xfer);
							path_ptr->Run (run);
							path_ptr->Status (1);

							//---- add the wait data ----
					
							path_ptr = &wait_path [path] [to_stop];

							path_ptr->Imped (wait_imp);
							path_ptr->Time (wait_time);
							path_ptr->Length (wait_len);
							path_ptr->Cost (wait_cost);
							path_ptr->Walk (wait_walk);
							path_ptr->Mode (WAIT_MODE);
							path_ptr->From (stop);
							path_ptr->Type (STOP_ID);
							path_ptr->Dir (1);
							path_ptr->Path (path);
							path_ptr->Xfer (xfer);
							path_ptr->Status (1);

							to_flag = true;

							if (best_flag) {
								max_imp = to_imp + imp_diff;
								exit_flag = true;
							}
						}

						//---- update the max impedance ----

						if (to_flag) {
							if (best_flag) continue;
							hi_imp = 0;

							for (to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++) {
								path_ptr = &to_itr->back ();

								if (path_ptr->Imped () < MAX_IMPEDANCE) {
									to_imp = path_ptr->Imped () + imp_diff;
									if (to_imp > hi_imp) hi_imp = to_imp;
								} else {
									hi_imp = MAX_IMPEDANCE;
									break;
								}
							}
							if (hi_imp > 0 && hi_imp < max_imp) max_imp = hi_imp;
							exit_flag = true;
						}
					}

					//---- consider adding the stop to the queue ----
			
					path_ptr = &alight_path [path] [to_stop];

					if (to_imp >= path_ptr->Imped ()) continue;

					//---- add the new path to the queue ----
					
					path_index.Index (to_stop);
					path_index.Type (ROUTE_ID);
					path_index.Path (path);

					if (path_param.sort_method) {
						if (path_ptr->Status () == 1) {
							transit_sort.Update (path_index, to_imp);
						} else {
							transit_sort.Insert (path_index, to_imp);
						}
					} else {
						if (path_ptr->Status () == 0) {
							next_index.push_back (path_index);
						} else if (path_ptr->Status () == 2) {
							next_index.push_front (path_index);
						}
					}
					path_ptr->Imped (to_imp);
					path_ptr->Time (to_time);
					path_ptr->Length (to_len);
					path_ptr->Cost (to_cost);
					path_ptr->Walk (to_walk);
					path_ptr->Mode (TRANSIT_MODE);
					path_ptr->From (route);
					path_ptr->Type (ROUTE_ID);
					path_ptr->Dir (1);
					path_ptr->Path (path);
					path_ptr->Xfer (xfer);
					path_ptr->Run (run);
					path_ptr->Status (1);

					//---- add the wait data ----
			
					path_ptr = &wait_path [path] [to_stop];

					path_ptr->Imped (wait_imp);
					path_ptr->Time (wait_time);
					path_ptr->Length (wait_len);
					path_ptr->Cost (wait_cost);
					path_ptr->Walk (wait_walk);
					path_ptr->Mode (WAIT_MODE);
					path_ptr->From (stop);
					path_ptr->Type (STOP_ID);
					path_ptr->Dir (1);
					path_ptr->Path (path);
					path_ptr->Xfer (xfer);
					path_ptr->Status (1);
				}
				break;
			}
		}
	}
	return (exit_flag);
}
