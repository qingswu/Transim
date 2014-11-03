//*********************************************************
//	Converge_Service.cpp - convergence service keys and data
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Converge_Service constructor
//---------------------------------------------------------

Converge_Service::Converge_Service (void) : Select_Service ()
{
	min_vht_flag = link_gap_flag = trip_gap_flag = trip_gap_map_flag = save_link_gap = save_trip_gap = memory_flag = false;
	iteration_flag = first_iteration = save_iter_flag = link_report_flag = trip_report_flag = trip_gap_parts = false;
	max_iteration = 1;
	num_parts = 0;
	max_speed_updates = 20;
	min_speed_diff = 0.01;
	num_trip_rec = num_trip_sel = 0;
	initial_factor = factor_increment = maximum_factor = link_gap = trip_gap = transit_gap = 0.0;
	reroute_time = 0;

	num_reroute = num_reskim = 0;
	percent_selected = 0.0;
}

//---------------------------------------------------------
//	Converge_Service_Keys
//---------------------------------------------------------

void Converge_Service::Converge_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ MAXIMUM_NUMBER_OF_ITERATIONS, "MAXIMUM_NUMBER_OF_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ LINK_CONVERGENCE_CRITERIA, "LINK_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ TRIP_CONVERGENCE_CRITERIA, "TRIP_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ TRANSIT_CAPACITY_CRITERIA, "TRANSIT_CAPACITY_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ INITIAL_WEIGHTING_FACTOR, "INITIAL_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", ">= 0.0", NO_HELP },
		{ ITERATION_WEIGHTING_INCREMENT, "ITERATION_WEIGHTING_INCREMENT", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..5.0", NO_HELP },
		{ MAXIMUM_WEIGHTING_FACTOR, "MAXIMUM_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "20.0", "0.0, >= 2.0", NO_HELP },
		{ MINIMIZE_VEHICLE_HOURS, "MINIMIZE_VEHICLE_HOURS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", RANGE_RANGE, NO_HELP },
		{ MAXIMUM_RESKIM_ITERATIONS, "MAXIMUM_RESKIM_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "10", "0..100", NO_HELP },
		{ RESKIM_CONVERGENCE_CRITERIA, "RESKIM_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.02", "0..10.0", NO_HELP },
		{ SAVE_AFTER_ITERATIONS, "SAVE_AFTER_ITERATIONS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", BOOL_RANGE, NO_HELP },
		{ NEW_LINK_CONVERGENCE_FILE, "NEW_LINK_CONVERGENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_CONVERGENCE_FILE, "NEW_TRIP_CONVERGENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};

	if (keys == 0) {
		exe->Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					exe->Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				exe->Error (String ("Converge Service Key %d was Not Found") % keys [i]);
			}
		}
	}
	int select_service_keys [] = {
		PERCENT_TIME_DIFFERENCE, MINIMUM_TIME_DIFFERENCE, MAXIMUM_TIME_DIFFERENCE, 
		PERCENT_COST_DIFFERENCE, MINIMUM_COST_DIFFERENCE, MAXIMUM_COST_DIFFERENCE, 
		PERCENT_TRIP_DIFFERENCE, MINIMUM_TRIP_DIFFERENCE, MAXIMUM_TRIP_DIFFERENCE,
		SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, 0
	};
	Select_Service_Keys (select_service_keys);
}

//---------------------------------------------------------
//	Read_Converge_Keys
//---------------------------------------------------------

void Converge_Service::Read_Converge_Keys (void)
{
	String key;

	Read_Select_Keys ();
	
	exe->Print (2, "Converge Service Controls:");
	exe->Print (1);

	//---- maximum number of iterations ----

	max_iteration = exe->Get_Control_Integer (MAXIMUM_NUMBER_OF_ITERATIONS);

	if (max_iteration > 1) {
		iteration_flag = true;

		//---- link gap criteria ----

		link_gap = exe->Get_Control_Double (LINK_CONVERGENCE_CRITERIA);
		
		//---- trip gap criteria ----

		trip_gap = exe->Get_Control_Double (TRIP_CONVERGENCE_CRITERIA);
		
		//---- transit gap criteria ----

		transit_gap = exe->Get_Control_Double (TRANSIT_CAPACITY_CRITERIA);


		//---- initial weighting factor ----

		initial_factor = exe->Get_Control_Double (INITIAL_WEIGHTING_FACTOR);
		
		//---- iteration weighting increment ----

		factor_increment = exe->Get_Control_Double (ITERATION_WEIGHTING_INCREMENT);

		//---- maximum weighting factor ----

		maximum_factor = exe->Get_Control_Double (MAXIMUM_WEIGHTING_FACTOR);

		//---- minimize vehicle hours ----
		
		min_vht_flag = exe->Get_Control_Flag (MINIMIZE_VEHICLE_HOURS);

		//---- maximum number for reskim iterations ----

		max_speed_updates = exe->Get_Control_Integer (MAXIMUM_RESKIM_ITERATIONS);

		//---- reskim converence criteria ----

		min_speed_diff = exe->Get_Control_Double (RESKIM_CONVERGENCE_CRITERIA);

		//---- save after iterations ----

		if (exe->Check_Control_Key (SAVE_AFTER_ITERATIONS)) {
			key = exe->Get_Control_Text (SAVE_AFTER_ITERATIONS);

			if (!key.empty () && !key.Equals ("NONE")) {
				if (key.Equals ("ALL")) {
					key ("%d..%d") % 1 % max_iteration;
				}
				save_iter_flag = true;
				if (!save_iter_range.Add_Ranges (key)) {
					exe->Error ("Adding Iteration Ranges");
				}
			}
		}

		//---- new link convergence file ----

		key = exe->Get_Control_String (NEW_LINK_CONVERGENCE_FILE);

		if (!key.empty ()) {
			exe->Print (1);
			link_gap_file.File_Type ("New Link Convergence File");
			link_gap_file.Create (exe->Project_Filename (key));
			link_gap_flag = true;
		}
		save_link_gap = (link_gap_flag || link_gap > 0.0);

		//---- new trip convergence file ----

		key = exe->Get_Control_String (NEW_TRIP_CONVERGENCE_FILE);

		if (!key.empty ()) {
			exe->Print (1);
			trip_gap_file.File_Type ("New Trip Convergence File");
			trip_gap_file.Create (exe->Project_Filename (key));
			trip_gap_flag = true;
		}
		save_trip_gap = (trip_gap_flag || trip_gap > 0.0);
	}
}

//---------------------------------------------------------
//	Iteration_Setup
//---------------------------------------------------------

void Converge_Service::Iteration_Setup (void)
{
	if (iteration == 1) {
		int i, num;

		if (link_gap_flag) {
			link_gap_file.File () << "ITERATION";
			num = dat->sum_periods.Num_Periods ();
			for (i=0; i < num; i++) {
				link_gap_file.File () << "\t" << dat->sum_periods.Range_Label (i);
			}
			link_gap_file.File () << "\tTOTAL" << endl;
			link_gap_file.File () << iteration;
		}
		if (trip_gap_flag) {
			trip_gap_file.File () << "ITERATION";
			num = dat->sum_periods.Num_Periods ();
			for (i=0; i < num; i++) {
				trip_gap_file.File () << "\t" << dat->sum_periods.Range_Label (i);
			}
			trip_gap_file.File () << "\tTOTAL" << endl;
			trip_gap_file.File () << iteration;
		}
	} else if (iteration > 1) {
		if (link_gap_flag) {
			link_gap_file.File () << iteration;
		}
		if (trip_gap_flag) {
			trip_gap_file.File () << iteration;
		}
	}
}

//---------------------------------------------------------
//	Write_Link_Gap
//---------------------------------------------------------

void Converge_Service::Write_Link_Gap (double gap, bool end_flag)
{
	if (link_gap_flag) {
		link_gap_file.File () << "\t" << gap;
		if (end_flag) link_gap_file.File () << endl;
	}
}

//---------------------------------------------------------
//	Write_Trip_Gap
//---------------------------------------------------------

void Converge_Service::Write_Trip_Gap (double gap, bool end_flag)
{
	if (trip_gap_flag) {
		trip_gap_file.File () << "\t" << gap;
		if (end_flag) trip_gap_file.File () << endl;
	}
}

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
			exe->Show_Message ("Initializing Trip Gap Memory");
			exe->Set_Progress ();

			for (index=0, plan_itr = dat->plan_array.begin (); plan_itr != dat->plan_array.end (); plan_itr++, index++) {
				exe->Show_Progress ();

				gap_data_array [index].previous = plan_itr->Impedance ();
			}
			exe->End_Progress ();
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

//---------------------------------------------------------
//	Get_Link_Gap
//---------------------------------------------------------

double Converge_Service::Get_Link_Gap (bool zero_flag)
{
	int period, index, gap_period, last_period, first_period;
	double gap, vht, diff, old_vht, diff_vht, sum_vht;
	double period_diff, period_sum;

	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Gap_Sum gap_sum;

	gap = diff_vht = sum_vht = period_diff = period_sum = 0.0;
	
	memset (&gap_sum, '\0', sizeof (gap_sum));

	if (reroute_time > 0) {
		first_period = dat->perf_period_array.periods->Period (reroute_time);
	} else {
		first_period = 0;
	}
	last_period = -1;

	//---- process each time period ----

	for (period=0, period_itr = dat->perf_period_array.begin (); period_itr != dat->perf_period_array.end (); period_itr++, period++) {
		if (period < first_period) continue;

		//---- check for a new summary time period ----

		gap_period = dat->sum_periods.Period (dat->perf_period_array.periods->Period_Time (period));

		if (gap_period != last_period) {
			diff_vht += period_diff;
			sum_vht += period_sum;

			if (last_period >= 0) {
				if (period_sum > 0.0) {
					gap = period_diff / period_sum;
				} else if (period_diff > 0.0) {
					gap = 1.0;
				} else {
					gap = 0.0;
				}
				Write_Link_Gap (gap);
				if (link_report_flag && gap > gap_sum.max_gap) gap_sum.max_gap = gap;
			}
			period_diff = period_sum = 0.0;
			last_period = gap_period;
		}

		//---- process each link direction ----

		period_ptr = &old_perf_period_array [period];

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			
			perf_ptr = period_ptr->Data_Ptr (index);

			vht = perf_itr->Veh_Time ();
			old_vht = perf_ptr->Veh_Time ();
			diff = fabs (vht - old_vht);
			period_diff += diff;
			period_sum += vht;

			if (link_report_flag) {
				gap_sum.count++;
				gap_sum.abs_diff += diff;
				gap_sum.diff_sq += diff * diff;
				gap_sum.current += vht;
				gap_sum.previous += old_vht;
			}
			*perf_ptr = *perf_itr;
			if (zero_flag) perf_itr->Clear_Flows ();
		}
	}
	
	//---- process the last summary time period ----

	diff_vht += period_diff;
	sum_vht += period_sum;

	if (last_period >= 0) {
		if (period_sum > 0.0) {
			gap = period_diff / period_sum;
		} else if (period_diff > 0.0) {
			gap = 1.0;
		} else {
			gap = 0.0;
		}
		Write_Link_Gap (gap);
		if (link_report_flag && gap > gap_sum.max_gap) gap_sum.max_gap = gap;
	}
	
	//---- overall gap ----

	if (sum_vht > 0.0) {
		gap = diff_vht / sum_vht;
	} else if (diff_vht > 0) {
		gap = 1.0;
	} else {
		gap = 0.0;
	}
	Write_Link_Gap (gap, true);

	if (link_report_flag) {
		link_gap_array.push_back (gap_sum);
	}
	return (gap);
}


//---------------------------------------------------------
//	Get_Trip_Gap
//---------------------------------------------------------

double Converge_Service::Get_Trip_Gap (void)
{
	int i, num, period;
	double diff, imp, sum_diff, total, gap;
	bool gap_flag;

	Trip_Gap_Map_Itr itr;
	Gap_Sum gap_sum;
	Doubles period_diff, period_sum;
	
	num = 0;
	sum_diff = total = gap = 0.0;

	memset (&gap_sum, '\0', sizeof (gap_sum));

	gap_flag = (trip_gap_flag || trip_report_flag);

	if (gap_flag) {
		num = dat->sum_periods.Num_Periods ();
		period_diff.assign (num, 0);
		period_sum.assign (num, 0);
	}

	//---- process each trip ----

	if (memory_flag) {
		Gap_Data_Itr gap_itr;

		for (gap_itr = gap_data_array.begin (); gap_itr != gap_data_array.end (); gap_itr++) {

			total += imp = gap_itr->current;
			sum_diff += diff = abs (imp - gap_itr->previous);

			gap_itr->previous = gap_itr->current;
			gap_itr->current = 0;

			if (trip_report_flag) {
				gap_sum.count++;
				gap_sum.abs_diff += diff;
				gap_sum.diff_sq += diff * diff;
				gap_sum.current += imp;
			}
			if (gap_flag) {
				period = dat->sum_periods.Period (gap_itr->time);

				if (period >= 0) {
					period_diff [period] += diff;
					period_sum [period] += imp;
				}
			}
		}

	} else {

		for (i=0; i < num_parts; i++) {
			Trip_Gap_Map *trip_gap_map_ptr = trip_gap_map_array [i];

			for (itr = trip_gap_map_ptr->begin (); itr != trip_gap_map_ptr->end (); itr++) {
				total += imp = itr->second.current;
				sum_diff += diff = abs (imp - itr->second.previous);

				itr->second.previous = itr->second.current;
				itr->second.current = 0;

				if (trip_report_flag) {
					gap_sum.count++;
					gap_sum.abs_diff += diff;
					gap_sum.diff_sq += diff * diff;
					gap_sum.current += imp;
				}
				if (gap_flag) {
					period = dat->sum_periods.Period (itr->second.time);

					if (period >= 0) {
						period_diff [period] += diff;
						period_sum [period] += imp;
					}
				}
			}
		}
	}

	//---- process the iteration data ----

	if (total > 0) {
		gap = sum_diff / total;
	} else if (sum_diff > 0) {
		gap = 1.0;
	} else {
		gap = 0.0;
	}

	//---- write the trip gap file ----

	if (gap_flag) {
		for (i=0; i < num; i++) {
			imp = period_sum [i];
			diff = period_diff [i];

			if (imp > 0) {
				diff = diff / imp;
			} else if (diff > 0) {
				diff = 1.0;
			} else {
				diff = 0.0;
			}
			Write_Trip_Gap (diff);
		}
		Write_Trip_Gap (gap, true);

		if (trip_report_flag) {
			trip_gap_array.push_back (gap_sum);
		}
	}
	return (gap);
}

//---------------------------------------------------------
//	Link_Gap_Report
//---------------------------------------------------------

void Converge_Service::Link_Gap_Report (int report)
{
	int i;
	double gap, diff, vht, std_dev, rmse, num, hour;

	Gap_Sum_Itr gap_itr;

	hour = Dtime (1.0, HOURS);

	exe->Header_Number (report);

	if (!exe->Break_Check ((int) link_gap_array.size () + 7)) {
		exe->Print (1);
		Link_Gap_Header ();
	}

	for (i=1, gap_itr = link_gap_array.begin (); gap_itr != link_gap_array.end (); gap_itr++, i++) {
		if (gap_itr->count == 0) continue;

		num = gap_itr->count;
		vht = gap_itr->current;
		diff = gap_itr->abs_diff;

		if (vht > 0.0) {
			gap = diff / vht;
			rmse = 100.0 * sqrt (gap_itr->diff_sq / num) * num / vht;

			if (num > 1) {		
				std_dev = (gap_itr->diff_sq - diff * diff / num) / (num - 1.0);
				std_dev = (std_dev > 0.0) ? sqrt (std_dev) : 0.0;
				std_dev = std_dev * num / vht;
			} else {
				std_dev = 0.0;
			}
			vht /= hour;
			diff /= hour;
		} else {
			gap = 1.0;
			std_dev = rmse = 0.0;
		}

		//---- print the data record ----

		exe->Print (1, String ("%6d   %13.6lf %13.6lf %13.6lf %9.1lf %13.0lf %13.0lf") %
			i % gap % std_dev % gap_itr->max_gap % rmse % diff % vht);
	}
	exe->Header_Number (0);
}

//---------------------------------------------------------
//	Link_Gap_Header
//---------------------------------------------------------

void Converge_Service::Link_Gap_Header (void)
{
	exe->Print (1, "Link Gap Report");
	exe->Print (2, "          --------------- Link Gap ---------------      %     ------- PCE Hours -------");
	exe->Print (1, "Iteration        Total       Std.Dev       Maximum     RMSE     Difference         Total");
	exe->Print (1);
}

//---------------------------------------------------------
//	Trip_Gap_Report
//---------------------------------------------------------

void Converge_Service::Trip_Gap_Report (int report)
{
	int i;
	double gap, diff, total, std_dev, rmse, num;

	Gap_Sum_Itr gap_itr;

	exe->Header_Number (report);

	if (!exe->Break_Check ((int) trip_gap_array.size () + 7)) {
		exe->Print (1);
		Trip_Gap_Header ();
	}

	for (i=1, gap_itr = trip_gap_array.begin (); gap_itr != trip_gap_array.end (); gap_itr++, i++) {
		if (gap_itr->count == 0) continue;

		num = gap_itr->count;
		total = gap_itr->current;
		diff = gap_itr->abs_diff;

		if (total > 0.0) {
			gap = diff / total;
			rmse = 100.0 * sqrt (gap_itr->diff_sq / num) * num / total;

			if (num > 1) {		
				std_dev = (gap_itr->diff_sq - diff * diff / num) / (num - 1.0);
				std_dev = (std_dev > 0.0) ? sqrt (std_dev) : 0.0;
				std_dev = std_dev * num / total;
			} else {
				std_dev = 0.0;
			}
			total /= 100;
			diff /= 100;
		} else {
			gap = 1.0;
			std_dev = rmse = 0.0;
		}

		//---- print the data record ----

		exe->Print (1, String ("%6d   %13.6lf %13.6lf %13.6lf %9.1lf %13.0lf %13.0lf") %
			i % gap % std_dev % gap_itr->max_gap % rmse % diff % total);
	}
	exe->Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Gap_Header
//---------------------------------------------------------

void Converge_Service::Trip_Gap_Header (void)
{
	exe->Print (1, "Trip Gap Report");
	exe->Print (2, "          --------------- Trip Gap ---------------      %     ------ Impedance/100 -----");
	exe->Print (1, "Iteration        Total       Std.Dev       Maximum     RMSE     Difference         Total");
	exe->Print (1);
}

/*********************************************|***********************************************

	Link Gap Report

              -------------- Link Gap --------------     %     ------- PCE Hours -------
	Iteration        Total      Std.Dev      Maximum    RMSE     Difference        Total

	dddddd    fffff.ffffff fffff.ffffff fffff.ffffff  fffff.f  ffffffffffff ffffffffffff

**********************************************|***********************************************/ 

/*********************************************|***********************************************

	Trip Gap Report

              --------------- Trip Gap ---------------      %     ----- Impedance/100 -----
	Iteration        Total       Std.Dev       Maximum     RMSE    Difference         Total

	dddddd    fffff.ffffff  fffff.ffffff  fffff.ffffff   fffff.f  ffffffffffff ffffffffffff

**********************************************|***********************************************/ 
