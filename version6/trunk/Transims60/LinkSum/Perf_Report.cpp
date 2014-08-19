//*********************************************************
//	Perf_Report.cpp - network performance report
//*********************************************************

#include "LinkSum.hpp"

#define LANE_MILES	0
#define VMT			1
#define VHT			2
#define VHD			3
#define TURNS		4

//---------------------------------------------------------
//	Performance_Report
//---------------------------------------------------------

void LinkSum::Performance_Report (void)
{
	int i, j, k, index, use_index;
	double time, len, speed, factor, flow_factor;
	Dtime low, high, tod;
	bool connect_flag;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Turn_Period_Itr turn_itr;
	Perf_Data perf_data;
	Turn_Data *turn_ptr;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;
	Performance_Data data;

	Show_Message ("Creating the Network Performance Summary Report -- Record");
	Set_Progress ();

	//---- clear the summary bins -----

	for (j=0, itr = sum_bin.begin (); itr != sum_bin.end (); itr++, j++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}	
	connect_flag = System_Data_Flag (CONNECTION) && (turn_period_array.size () > 0);

	tod = sum_periods.Increment ();
	if (tod < 1) {
		flow_factor = (double) Dtime (60, MINUTES) / tod;
	} else {
		flow_factor = 1.0;
	}

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		len = UnRound (link_itr->Length ());

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_itr->Use () == -1) continue;
				index = link_itr->BA_Dir ();
			} else {
				if (link_itr->Use () == -2) continue;
				index = link_itr->AB_Dir ();
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];
			use_index = dir_ptr->Use_Index ();


			//---- process each time period ----

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {
				perf_period_array.periods->Period_Range (j, low, high);

				data.Start (low);
				data.End (high);

				perf_data = period_itr->Total_Performance (index, use_index);

				data.Get_Data (&perf_data, dir_ptr, &(*link_itr));

				//---- check the time ratio ----

				if (select_ratio) {
					if (data.Time_Ratio () < time_ratio) continue;
				}

				//---- check the vc ratio ----

				if (select_vc) {
					if (data.VC_Ratio () < vc_ratio) continue;
				}
				sum_bin [j] [LANE_MILES] += data.Lane_Len ();
				sum_bin [j] [VMT] += data.Veh_Dist ();
				sum_bin [j] [VHT] += data.Veh_Time ();
				sum_bin [j] [VHD] += data.Veh_Delay ();
			}

			//---- get the turning movements ----

			if (connect_flag) {
				for (k=dir_ptr->First_Connect (); k >= 0; k = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [k];

					if (connect_ptr->Type () != LEFT && connect_ptr->Type () != RIGHT &&
						connect_ptr->Type () != UTURN) continue;

					for (j=0, turn_itr = turn_period_array.begin (); turn_itr != turn_period_array.end (); turn_itr++, j++) {
						turn_ptr = &turn_itr->at (k);

						sum_bin [j] [TURNS] += turn_ptr->Turn ();
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (PERF_REPORT);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Performance_Header ();
	}
	if (Metric_Flag ()) {
		factor = 1.0 / 1000.0;
	} else {
		factor = 1.0 / MILETOFEET;
	}
	tod.Hours (1);

	for (i=0; i <= num_inc; i++) {
		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			if (sum_bin [i] [VHT] == 0.0) continue;

			Print (1, String ("%-12.12s ") % sum_periods.Range_Format (i));

			sum_bin [num_inc] [LANE_MILES] = MAX (sum_bin [i] [LANE_MILES], sum_bin [num_inc] [LANE_MILES]);
			sum_bin [num_inc] [VMT] += sum_bin [i] [VMT];
			sum_bin [num_inc] [VHT] += sum_bin [i] [VHT];
			sum_bin [num_inc] [VHD] += sum_bin [i] [VHD];
			sum_bin [num_inc] [TURNS] += sum_bin [i] [TURNS];
		}
		len = (sum_bin [i] [VMT] * factor);
		time = sum_bin [i] [VHT] / tod;
		if (time == 0.0) {
			speed = len;
		} else {
			speed = len / time;
		}
		Print (0, String ("%12.2lf %13.2lf %13.2lf %9.2lf %13.2lf") % 
			(sum_bin [i] [LANE_MILES] * factor) % len % time % 
			speed % (sum_bin [i] [VHD] / tod));
		if (connect_flag) {
			Print (0, String (" %10d") % ((int) (sum_bin [i] [TURNS])));
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Performance_Header
//---------------------------------------------------------

void LinkSum::Performance_Header (void)
{
	String units;

	if (Metric_Flag ()) {
		units = "Kilometers";
	} else {
		units = "Miles";
	}
	Print (1, "Network Performance Summary");

	if (turn_period_array.size () > 0) {
		Print (2, String ("                   Lane        Vehicle       Vehicle %10.10s     Hours of      Number") % units);
		Print (1, String ("Time Period   %10.10s    %10.10s        Hours       /Hour       Delay      of Turns") % units % units);
	} else {
		Print (2, String ("                   Lane        Vehicle       Vehicle %10.10s     Hours of") % units);
		Print (1, String ("Time Period   %10.10s    %10.10s        Hours       /Hour       Delay    ") % units % units);
	}
	Print (1);
}

/*********************************************|***********************************************

	Network Performance Summary

	                  Lane        Vehicle       Vehicle  KilomMiles    Hours of      Number
	Time Period  KilomMiles    KilomMiles        Hours        /Hour      Delay      of Turns
	
	xx:xx..xx:xx fffffffff.ff ffffffffff.ff ffffffffff.ff ffffff.ff ffffffffff.ff dddddddddd

	Total        fffffffff.ff ffffffffff.ff ffffffffff.ff ffffff.ff ffffffffff.ff dddddddddd

**********************************************|***********************************************/ 
