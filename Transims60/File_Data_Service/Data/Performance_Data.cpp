//*********************************************************
//	Performance_Data.cpp - network performance data
//*********************************************************

#include "Performance_Data.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Update_Time
//---------------------------------------------------------

void Perf_Data::Update_Time (Dtime time) 
{
	if (Time () > 0 && time > 0) {
		double factor = (double) time / Time ();
		Veh_Time (Veh_Time () * factor);
		Ratio_Time (Ratio_Time () * factor);
	}
	Time (time);
}

//---------------------------------------------------------
//	Set_Flows
//---------------------------------------------------------

void Perf_Data::Set_Flows (Perf_Data *perf_ptr) 
{
	Persons (perf_ptr->Persons ());
	Volume (perf_ptr->Volume ());
	Enter (perf_ptr->Enter ());
	Exit (perf_ptr->Exit ());
	Max_Volume (perf_ptr->Max_Volume ());
	Queue (perf_ptr->Queue ());
	Max_Queue (perf_ptr->Max_Queue ());
	Failure (perf_ptr->Failure ());
	Veh_Dist (perf_ptr->Veh_Dist ());
	Veh_Time (perf_ptr->Veh_Time ());
	Ratio_Dist (perf_ptr->Ratio_Dist ());
	Ratio_Time (perf_ptr->Ratio_Time ());
	Ratios (perf_ptr->Ratios ());
	Count (perf_ptr->Count ());
}

//---------------------------------------------------------
//	Add_Flows
//---------------------------------------------------------

void Perf_Data::Add_Flows (Perf_Data *perf_ptr) 
{
	Add_Persons (perf_ptr->Persons ());
	Add_Volume (perf_ptr->Volume ());
	Add_Enter (perf_ptr->Enter ());
	Add_Exit (perf_ptr->Exit ());
	Add_Max_Volume (perf_ptr->Max_Volume ());
	Add_Queue (perf_ptr->Queue ());
	Add_Max_Queue (perf_ptr->Max_Queue ());
	Add_Failure (perf_ptr->Failure ());
	Add_Veh_Dist (perf_ptr->Veh_Dist ());
	Add_Veh_Time (perf_ptr->Veh_Time ());
	Add_Ratio_Dist (perf_ptr->Ratio_Dist ());
	Add_Ratio_Time (perf_ptr->Ratio_Time ());
}

//---------------------------------------------------------
//	Average_Flows
//---------------------------------------------------------

void Perf_Data::Average_Flows (Perf_Data *perf_ptr) 
{
	Persons ((Persons () + perf_ptr->Persons ()) / 2.0);
	Volume ((Volume () + perf_ptr->Volume ()) / 2.0);
	Enter ((Enter () + perf_ptr->Enter ()) / 2.0);
	Exit ((Exit () + perf_ptr->Exit ()) / 2.0);
	Max_Volume (Volume ());
	Queue ((Queue () + perf_ptr->Queue ()) / 2.0);
	Max_Queue (Queue ());
	Failure ((Failure () + perf_ptr->Failure ()) / 2.0);
	Veh_Dist ((Veh_Dist () + perf_ptr->Veh_Dist ()) / 2.0);
	Veh_Time ((Veh_Time () + perf_ptr->Veh_Time ()) / 2.0);
	Ratio_Dist ((Ratio_Dist () + perf_ptr->Ratio_Dist ()) / 2.0);
	Ratio_Time ((Ratio_Time () + perf_ptr->Ratio_Time ()) / 2.0);
}

//---------------------------------------------------------
//	Weight_Flows
//---------------------------------------------------------

void Perf_Data::Weight_Flows (Perf_Data *perf_ptr, double weight) 
{
	double weight1 = weight + 1;

	Persons ((Persons () + perf_ptr->Persons () * weight) / weight1);
	Volume ((Volume () + perf_ptr->Volume () * weight) / weight1);
	Enter ((Enter () + perf_ptr->Enter () * weight) / weight1);
	Exit ((Exit () + perf_ptr->Exit () * weight) / weight1);
	Max_Volume (Volume ());
	Queue ((Queue () + perf_ptr->Queue () * weight) / weight1);
	Max_Queue (Queue ());
	Failure ((Failure () + perf_ptr->Failure () * weight) / weight1);
	Veh_Dist ((Veh_Dist () + perf_ptr->Veh_Dist () * weight) / weight1);
	Veh_Time ((Veh_Time () + perf_ptr->Veh_Time () * weight) / weight1);
	Ratio_Dist ((Ratio_Dist () + perf_ptr->Ratio_Dist () * weight) / weight1);
	Ratio_Time ((Ratio_Time () + perf_ptr->Ratio_Time () * weight) / weight1);
}

void Perf_Data::Weight_Flows (Perf_Data *perf1_ptr, double fac1, Perf_Data *perf2_ptr, double fac2) 
{
	Persons (perf1_ptr->Persons () * fac1 + perf2_ptr->Persons () * fac2);
	Volume (perf1_ptr->Volume () * fac1 + perf2_ptr->Volume () * fac2);
	Enter (perf1_ptr->Enter () * fac1 + perf2_ptr->Enter () * fac2);
	Exit (perf1_ptr->Exit () * fac1 + perf2_ptr->Exit () * fac2);
	Max_Volume (Volume ());
	Queue (perf1_ptr->Queue () * fac1 + perf2_ptr->Queue () * fac2);
	Max_Queue (Queue ());
	Failure (perf1_ptr->Failure () * fac1 + perf2_ptr->Failure () * fac2);
	Veh_Dist (perf1_ptr->Veh_Dist () * fac1 + perf2_ptr->Veh_Dist () * fac2);
	Veh_Time (perf1_ptr->Veh_Time () * fac1 + perf2_ptr->Veh_Time () * fac2);
	Ratio_Dist (perf1_ptr->Ratio_Dist () * fac1 + perf2_ptr->Ratio_Dist () * fac2);
	Ratio_Time (perf1_ptr->Ratio_Time () * fac1 + perf2_ptr->Ratio_Time () * fac2);
}

//---------------------------------------------------------
//	Sum_Periods
//---------------------------------------------------------

void Perf_Data::Sum_Periods (Perf_Data *perf_ptr) 
{
	double occ_fac;

	if (perf_ptr->Volume () > 0) {
		occ_fac = perf_ptr->Persons () / perf_ptr->Volume ();
	} else if (Volume () > 0) {
		occ_fac = Persons () / Volume ();
	} else {
		occ_fac = 1.0;
	}
	Add_Persons (perf_ptr->Enter () * occ_fac);
	Add_Volume (perf_ptr->Enter ());
	Add_Enter (perf_ptr->Enter ());
	Add_Exit (perf_ptr->Exit ());
	Sum_Max_Volume (perf_ptr->Max_Volume ());
	Add_Queue (perf_ptr->Queue ());
	Sum_Max_Queue (perf_ptr->Max_Queue ());
	Add_Failure (perf_ptr->Failure ());
	Add_Veh_Dist (perf_ptr->Veh_Dist ());
	Add_Veh_Time (perf_ptr->Veh_Time ());
	Add_Ratio_Dist (perf_ptr->Ratio_Dist ());
	Add_Ratio_Time (perf_ptr->Ratio_Time ());
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Perf_Data::Output_Check (void) 
{
	return (Volume () >= 0.1);
}

//---------------------------------------------------------
//	Total_Performance
//---------------------------------------------------------

Perf_Data Perf_Period::Total_Performance (int index) 
{
	return (Total_Performance (index, dat->dir_array [index].Use_Index ()));
}

Perf_Data Perf_Period::Total_Performance (int index, int use_index) 
{
	Perf_Data rec = at (index);

	if (use_index >= 0) {
		Perf_Data *ptr = &at (use_index);

		rec.Add_Persons (ptr->Persons ());
		rec.Add_Volume (ptr->Volume ());
		rec.Add_Enter (ptr->Enter ());
		rec.Add_Exit (ptr->Exit ());
		rec.Add_Max_Volume (ptr->Max_Volume ());
		rec.Add_Queue (ptr->Queue ());
		rec.Add_Max_Queue (ptr->Max_Queue ());
		rec.Add_Failure (ptr->Failure ());
		rec.Add_Veh_Dist (ptr->Veh_Dist ());
		rec.Add_Veh_Time (ptr->Veh_Time ());

		if (rec.Veh_Time () > 0) {
			rec.Time (rec.Veh_Dist () / rec.Veh_Time ());
		}
		if (dat->Ratio_Flag ()) {
			Dir_Data *dir_ptr = &dat->dir_array [index];
			
			int time_ratio = dat->Round (rec.Time () * 100.0 / dir_ptr->Time0 ());

			if (time_ratio >= dat->Congested_Ratio ()) {
				rec.Ratios (1);
				rec.Ratio_Dist (rec.Veh_Dist ());
				rec.Ratio_Time (rec.Veh_Time ());
			}
		}
	}
	return (rec);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Perf_Period_Array::Initialize (Time_Periods *time_periods, int num)
{
	periods = time_periods;

	if (num > 0) {
		num_records = num;
	} else {
		num_records = (int) dat->dir_array.size () + dat->Num_Lane_Use_Flows ();
	}
	if (num_records > 0) {
		Perf_Period_Itr period_itr;
		Perf_Period period_rec;
		Perf_Data perf_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, perf_data);
		}
	}
}

//---------------------------------------------------------
//	Replicate
//---------------------------------------------------------

void Perf_Period_Array::Replicate (Perf_Period_Array &period_array)
{
	periods = period_array.periods;
	num_records = period_array.num_records;

	if (num_records > 0) {
		Perf_Period_Itr period_itr;
		Perf_Period period_rec;
		Perf_Data perf_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, perf_data);
		}
	}
}

//---------------------------------------------------------
//	Zero_Times
//---------------------------------------------------------

void Perf_Period_Array::Set_Time0 (void)
{
	if (num_records > 0) {
		int num, index, size;
		Perf_Period_Itr period_itr;
		Perf_Itr perf_itr;
		
		size = (int) dat->dir_array.size ();

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			for (num=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, num++) {
				if (num < size) {
					index = num;
				} else {
					index = dat->lane_use_flow_index [num - size];
				}
				perf_itr->Time (dat->dir_array [index].Time0 ());
			}
		}
	}
}

//---------------------------------------------------------
//	Zero_Turns
//---------------------------------------------------------

void Perf_Period_Array::Zero_Flows (Dtime first_time)
{
	if (num_records > 0) {
		Perf_Period_Itr period_itr;
		Perf_Itr perf_itr;
		int period, first_period;

		first_period = periods->Period (first_time);

		for (period=0, period_itr = begin (); period_itr != end (); period_itr++, period++) {
			if (period < first_period) continue;

			for (perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++) {
				perf_itr->Clear_Flows ();
			}
		}
	}
}

//---------------------------------------------------------
//	Copy_Flow_Data
//---------------------------------------------------------

void Perf_Period_Array::Copy_Flow_Data (Perf_Period_Array &period_array, bool zero_flag, Dtime first_time)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i, first_period;
		Perf_Period_Itr period_itr;
		Perf_Period *period_ptr;
		Perf_Itr perf_itr;
		Perf_Data *perf_ptr;

		if (zero_flag) {
			first_period = periods->Period (first_time);
		} else {
			first_period = 0;
		}
		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, i++) {
				perf_ptr = period_ptr->Data_Ptr (i);

				perf_itr->Set_Flows (perf_ptr);
				perf_itr->Time (perf_ptr->Time ());

				if (zero_flag && p >= first_period) perf_ptr->Clear_Flows ();
			}
		}
	}
}

//---------------------------------------------------------
//	Copy_Time_Data
//---------------------------------------------------------

void Perf_Period_Array::Copy_Time_Data (Perf_Period_Array &period_array, bool zero_flag, Dtime first_time)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i, first_period;
		Perf_Period_Itr period_itr;
		Perf_Period *period_ptr;
		Perf_Itr perf_itr;
		Perf_Data *perf_ptr;

		if (zero_flag) {
			first_period = periods->Period (first_time);
		} else {
			first_period = 0;
		}
		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, i++) {
				perf_ptr = period_ptr->Data_Ptr (i);

				if (zero_flag && p >= first_period) {
					perf_itr->Clear_Flows ();
				} else {
					perf_itr->Set_Flows (perf_ptr);
				}
				perf_itr->Time (perf_ptr->Time ());
			}
		}
	}
}

//---------------------------------------------------------
//	Add_Flows
//---------------------------------------------------------

void Perf_Period_Array::Add_Flows (Perf_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Perf_Period_Itr period_itr;
		Perf_Period *period_ptr;
		Perf_Itr perf_itr;
		Perf_Data *perf_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, i++) {
				perf_ptr = period_ptr->Data_Ptr (i);

				perf_itr->Add_Flows (perf_ptr);
				if (zero_flag) perf_ptr->Clear_Flows ();
			}
		}
	}
}

//---------------------------------------------------------
//	Add_Flow_Times
//---------------------------------------------------------

void Perf_Period_Array::Add_Flow_Times (Perf_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Perf_Period_Itr period_itr;
		Perf_Period *period_ptr;
		Perf_Itr perf_itr;
		Perf_Data *perf_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, i++) {
				perf_ptr = period_ptr->Data_Ptr (i);

				perf_itr->Add_Flows (perf_ptr);
				perf_itr->Add_Time (perf_ptr->Time ());
				if (zero_flag) perf_ptr->Clear_Flows ();
			}
		}
	}
}

//---------------------------------------------------------
//	Average_Flows
//---------------------------------------------------------

void Perf_Period_Array::Average_Flows (Perf_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Perf_Period_Itr period_itr;
		Perf_Period *period_ptr;
		Perf_Itr perf_itr;
		Perf_Data *perf_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, i++) {
				perf_ptr = period_ptr->Data_Ptr (i);

				perf_itr->Average_Flows (perf_ptr);
				if (zero_flag) perf_ptr->Clear_Flows ();
			}
		}
	}
}

//---------------------------------------------------------
//	Average_Flow_Times
//---------------------------------------------------------

void Perf_Period_Array::Average_Flow_Times (Perf_Period_Array &period_array, int weight, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Perf_Period_Itr period_itr;
		Perf_Period *period_ptr;
		Perf_Itr perf_itr;
		Perf_Data *perf_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, i++) {
				perf_ptr = period_ptr->Data_Ptr (i);

				//perf_itr->Average_Flow_Time (perf_ptr->Volume (), perf_ptr->Time (), weight);
				perf_itr->Volume (perf_ptr->Volume () * weight);
				perf_itr->Time (perf_ptr->Time ());
				if (zero_flag) perf_ptr->Clear_Flows ();
			}
		}
	}
}

//---------------------------------------------------------
//	Period_Ptr
//---------------------------------------------------------

Perf_Period * Perf_Period_Array::Period_Ptr (Dtime time)
{
	int period = periods->Period (time);

	if (period >= 0) {
		return (&at (period));
	} else {
		return (0);
	}
}

//---------------------------------------------------------
//	Travel_Time
//---------------------------------------------------------

Dtime Perf_Period_Array::Travel_Time (int dir_index, Dtime time, double len_factor, bool forward_flag) 
{
	int period, num_periods, min_period, max_period, num;
	Dtime low, high, end_time, ttime, ttim, increment;
	double factor;
	bool break_flag;

	ttime = 0;
	num_periods = periods->Num_Periods ();
	increment = periods->Increment ();
	if (increment <= 0) increment = Dtime (15.0, MINUTES);
	num = Dtime (1.0, HOURS) / increment; 
	if (num < 1) num = 1;

	period = periods->Period (time);
	min_period = MAX ((period - num), 0);
	max_period = MIN ((period + num), (num_periods - 1));
	break_flag = false;

	for (; period >= min_period && period <= max_period; ) {
		periods->Period_Range (period, low, high);

		Perf_Period *period_ptr = &at (period);

		Perf_Data *perf_ptr = period_ptr->Data_Ptr (dir_index);

		if (perf_ptr->Time () <= 0) return (-1);

		if (len_factor <= 0.0) {
			ttime += perf_ptr->Time ();
			break_flag = true;
			break;
		}
		ttim = (int) (perf_ptr->Time () * len_factor);
		if (ttim < 0) return (-1);
		if (ttim < 1) ttim = 1;

		if (forward_flag) {
			end_time = time + ttim;
			if (end_time <= high) {
				ttime += ttim;
				break_flag = true;
				break;
			} else {
				ttim = high - time;
				ttime += ttim;
				if (perf_ptr->Time () > 0) {
					factor = (double) ttim / perf_ptr->Time ();
				} else {
					factor = 0.001;
				}
				len_factor -= factor;
				if (len_factor <= 0) {
					break_flag = true;
					break;
				}
				time = high + 1;
				period++;
			}
		} else {
			end_time = time - ttim;
			if (end_time >= low) {
				ttime += ttim;
				break_flag = true;
				break;
			} else {
				ttim = time - low;
				ttime += ttim;
				if (perf_ptr->Time () > 0) {
					factor = (double) ttim / perf_ptr->Time ();
				} else {
					factor = 0.001;
				}
				len_factor -= factor;
				if (len_factor <= 0) {
					break_flag = true;
					break;
				}
				time = low - 1;
				period--;
			}
		}
	}
	if (break_flag) {
		return (ttime);
	} else {
		return (-1);
	}
}

//---------------------------------------------------------
//	Flow_Time
//---------------------------------------------------------

Dtime  Perf_Period_Array::Flow_Time (int dir_index, Dtime time, double len_factor, double len, double pce, double occ, bool forward_flag)
{
	int period, num_periods, min_period, max_period, num;
	Dtime low, high, end_time, ttime, ttim, increment;
	double factor, tt;
	bool first;

	num_periods = periods->Num_Periods ();
	len *= pce;
	ttime = 0;
	first = true;

	increment = periods->Increment ();
	if (increment <= 0) increment = Dtime (15.0, MINUTES);
	num = Dtime (1.0, HOURS) / increment; 
	if (num < 1) num = 1;

	period = periods->Period (time);
	min_period = MAX ((period - num), 0);
	max_period = MIN ((period + num), (num_periods - 1));

	for (; period >= min_period && period <= max_period; first = false) {
		periods->Period_Range (period, low, high);

		Perf_Period *period_ptr = &at (period);

		Perf_Data *perf_ptr = period_ptr->Data_Ptr (dir_index);

		if (perf_ptr->Time () <= 0) {
			Dir_Data *dir_ptr = &dat->dir_array [dir_index];
			perf_ptr->Time (dir_ptr->Time0 ());
		}
		if (len_factor <= 0.0) {
			ttime += perf_ptr->Time ();
			goto link_end;
		}
		tt = perf_ptr->Time () * len_factor;
		ttim = (int) (tt + 0.5);
		if (ttim < 0) return (-1);
		if (ttim < 1) ttim = 1;

		if (forward_flag) {
			end_time = time + ttim;
			if (end_time <= high) {
				ttime += ttim;
				perf_ptr->Add_Persons (occ);
				perf_ptr->Add_Volume (pce);
				if (first) perf_ptr->Add_Enter (pce);
				perf_ptr->Add_Max_Volume (pce);
				perf_ptr->Add_Veh_Time (tt * pce);
				perf_ptr->Add_Veh_Dist (len * len_factor);

				if (len == 0.0) {
					perf_ptr->Add_Queue (len * len_factor);
				}
				goto link_end;
			} else {
				ttim = high - time;
				ttime += ttim;
				if (perf_ptr->Time () > 0) {
					factor = (double) ttim / perf_ptr->Time ();
				} else {
					factor = 0.001;
				}
				perf_ptr->Add_Persons (occ);
				perf_ptr->Add_Volume (pce);
				if (first) perf_ptr->Add_Enter (pce);
				perf_ptr->Add_Max_Volume (pce);
				perf_ptr->Add_Veh_Time (tt * pce);
				perf_ptr->Add_Veh_Dist (len * factor);

				len_factor -= factor;
				if (len_factor <= 0) goto link_end;
				time = high + 1;
				period++;
			}
		} else {
			end_time = time - ttim;
			if (end_time >= low) {
				ttime += ttim;
				perf_ptr->Add_Persons (occ);
				perf_ptr->Add_Volume (pce);
				if (first) perf_ptr->Add_Exit (pce);
				perf_ptr->Add_Max_Volume (pce);
				perf_ptr->Add_Veh_Time (tt * pce);
				perf_ptr->Add_Veh_Dist (len * len_factor);
				goto link_end;
			} else {
				ttim = time - low;
				ttime += ttim;
				if (perf_ptr->Time () > 0) {
					factor = (double) ttim / perf_ptr->Time ();
				} else {
					factor = 0.001;
				}
				perf_ptr->Add_Persons (occ);
				perf_ptr->Add_Volume (pce);
				if (first) perf_ptr->Add_Exit (pce);
				perf_ptr->Add_Max_Volume (pce);
				perf_ptr->Add_Veh_Time (tt * pce);
				perf_ptr->Add_Veh_Dist (len * factor);
				len_factor -= factor;
				if (len_factor <= 0) goto link_end;
				time = low - 1;
				period--;
			}
		}
		continue;
link_end:
		if (forward_flag) {
			perf_ptr->Add_Exit (pce);
		} else {
			perf_ptr->Add_Enter (pce);
		}
		return (ttime);
	}
	return (-1);
}

//---------------------------------------------------------
//	Load_Flow
//---------------------------------------------------------

Dtime Perf_Period_Array::Load_Flow (int dir_index, Dtime time, Dtime ttime, double len_factor, double len, double pce, double occ)
{
	int period, num_periods, min_period, max_period, num;
	Dtime low, high, end_time, ttim, increment, perf_time;
	double factor, tt;
	bool first;

	num_periods = periods->Num_Periods ();
	len *= pce;
	perf_time = 0;

	first = true;

	increment = periods->Increment ();
	if (increment <= 0) increment = Dtime (15.0, MINUTES);
	num = Dtime (1.0, HOURS) / increment; 
	if (num < 1) num = 1;

	period = periods->Period (time);
	min_period = MAX ((period - num), 0);
	max_period = MIN ((period + num), (num_periods - 1));

	for (; period >= min_period && period <= max_period; first = false) {

		periods->Period_Range (period, low, high);

		Perf_Period *period_ptr = &at (period);

		Perf_Data *perf_ptr = period_ptr->Data_Ptr (dir_index);
		if (perf_ptr->Time () < 0) return (-1);

		if (len_factor <= 0.0) goto link_end;

		if (perf_ptr->Time () < ttime) {
			tt = perf_ptr->Time ();
		} else {
			tt = ttime;
		}
		perf_time += (int) (perf_ptr->Time () * len_factor + 0.5);

		tt = tt * len_factor;
		ttim = (int) (tt + 0.5);
		if (ttim < 0) return (-1);
		if (ttim < 1) ttim = 1;

		end_time = time + ttim;
		if (end_time <= high) {
			perf_ptr->Add_Persons (occ);
			perf_ptr->Add_Volume (pce);
			if (first) perf_ptr->Add_Enter (pce);
			perf_ptr->Add_Max_Volume (pce);
			perf_ptr->Add_Veh_Time (tt * pce);
			perf_ptr->Add_Veh_Dist (len * len_factor);

			if (len == 0.0) {
				perf_ptr->Add_Queue (len * len_factor);
			}
			goto link_end;
		} else {
			ttim = high - time;
			if (ttime > 0) {
				factor = (double) ttim / ttime;
			} else {
				factor = 0.001;
			}
			perf_ptr->Add_Persons (occ);
			perf_ptr->Add_Volume (pce);
			if (first) perf_ptr->Add_Enter (pce);
			perf_ptr->Add_Max_Volume (pce);
			perf_ptr->Add_Veh_Time (tt * pce);
			perf_ptr->Add_Veh_Dist (len * factor);

			len_factor -= factor;
			if (len_factor <= 0) goto link_end;
			time = high + 1;
			period++;
		}
		continue;
link_end:
		perf_ptr->Add_Exit (pce);
		return (perf_time);
	}
	return (-1);
}

//---------------------------------------------------------
//	Get_Data
//---------------------------------------------------------

bool Performance_Data::Get_Data (Perf_Data *perf_ptr, int dir_index, int max_ratio) 
{
	Dir_Data *dir_ptr = &dat->dir_array [dir_index];
	Link_Data *link_ptr = &dat->link_array [dir_ptr->Link ()];

	return (Get_Data (perf_ptr, dir_ptr, link_ptr, max_ratio));
}

bool Performance_Data::Get_Data (Perf_Data *perf_ptr, Dir_Data *dir_ptr, Link_Data *link_ptr, int max_ratio)
{
	int i, lanes;
	double length, len, lane_len, speed, ratio, factor, min_vol, volume;
	Dtime time, increment;
		
	increment = End () - Start ();
	if (increment <= 0) increment = Dtime (15, MINUTES);

	length = link_ptr->Length ();
	len = exe->UnRound (length);

	Persons (perf_ptr->Persons ());
	Volume (perf_ptr->Volume ());
	Enter (perf_ptr->Enter ());
	Exit (perf_ptr->Exit ());

	Veh_Dist (exe->UnRound (perf_ptr->Veh_Dist ()));
	Veh_Time (perf_ptr->Veh_Time ());

	Flow (Veh_Dist () / len);

	if (perf_ptr->Veh_Time () > 0) {
		speed = perf_ptr->Veh_Dist () / Veh_Time ();
	} else {
		speed = length / dir_ptr->Time0 ();
	}
	if (speed < 0.1) speed = 0.1;

	time = (int) (length / speed + 0.5);
	if (time < dir_ptr->Time0 ()) {
		if (perf_ptr->Time () > dir_ptr->Time0 ()) {
			time = perf_ptr->Time ();
		} else {
			time = dir_ptr->Time0 ();
		}
		speed = length / time;
		if (speed < 0.1) speed = 0.1;

		Veh_Time (perf_ptr->Veh_Dist () / speed);
	}
	if (dir_ptr->Time0 () > 0) {
		ratio = time * 100.0 / dir_ptr->Time0 ();
	} else {
		ratio = 100;
	}

	if (ratio > max_ratio) {
		ratio = max_ratio / ratio;
		Veh_Time (Veh_Time () * ratio);

		time = (int) (time * ratio + 0.5);
		if (time < dir_ptr->Time0 ()) time = dir_ptr->Time0 ();

		speed = length / time;
		ratio = max_ratio;

		if (perf_ptr->Volume () > 0) {
			factor = perf_ptr->Persons () / perf_ptr->Volume ();
		} else {
			factor = 1.0;
		}
		min_vol = volume = MAX (perf_ptr->Enter (), perf_ptr->Exit ());
		if (time > increment) {
			volume = volume * time / increment;
		}
		if (perf_ptr->Volume () < min_vol || volume < perf_ptr->Volume ()) {
			Volume (volume);
			Persons (volume * factor);
		}
	}
	Time_Ratio (ratio);

    Speed (speed);
	Time (time);
	Delay (time - dir_ptr->Time0 ());

	Veh_Delay (Delay () * Flow ());
		
	lanes = dir_ptr->Lanes ();
	i = dir_ptr->First_Lane_Use ();

	if (i >= 0) {
		Lane_Use_Period *use_ptr;

		time = (Start () + End () + 1) / 2;

		for (use_ptr = &dat->use_period_array [i]; ; use_ptr = &dat->use_period_array [++i]) {
			if (use_ptr->Start () <= time && time < use_ptr->End ()) {
				if (type == 1) {
					lanes = use_ptr->Lanes1 ();
				} else {
					lanes = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
				}
				break;
			}
			if (use_ptr->Periods () == 0) break;
		}
	}
	if (lanes < 1) lanes = 1;

	lane_len = exe->External_Units (len, ((exe->Metric_Flag ()) ? KILOMETERS : MILES)) * lanes;
	if (lane_len <= 0) lane_len = 1.0;

	Lane_Len (len * lanes);

	ratio = (double) (End () - Start () + 1) / Dtime (1.0, HOURS);

	Density ((Volume () / ratio) / lane_len);
	Max_Density ((perf_ptr->Max_Volume () / ratio) / lane_len);

	if (perf_ptr->Count () > 1) {
		Queue (perf_ptr->Queue () / perf_ptr->Count ());
	} else {
		Queue (perf_ptr->Queue ());
	}
	Max_Queue (perf_ptr->Max_Queue ());
	Failure (perf_ptr->Failure ());

	if (Volume () > 0 && dir_ptr->Capacity () > 0 && End () > 0) {
		if (lanes != dir_ptr->Lanes () && dir_ptr->Lanes () > 0) {
			ratio = ratio * lanes / dir_ptr->Lanes ();
		}
		VC_Ratio (100.0 * Volume () / (dir_ptr->Capacity () * ratio));
	} else {
		VC_Ratio (0.0);
	}
	Ratio_Dist (exe->UnRound (perf_ptr->Ratio_Dist ()));
	Ratio_Time (perf_ptr->Ratio_Time ());
	Ratios (perf_ptr->Ratios ());
	Count (perf_ptr->Count ());

	return (true);
}

//---------------------------------------------------------
//	Get_Data
//---------------------------------------------------------

bool Performance_Data::Get_Data (Vol_Spd_Data *vol_spd_ptr, Dir_Data *dir_ptr, Link_Data *link_ptr, int max_ratio)
{
	int i, lanes;
	double length, lane_len, speed, ratio;
	Dtime time;

	length = link_ptr->Length ();

	Persons (vol_spd_ptr->Volume ());
	Volume (vol_spd_ptr->Volume ());
	Enter (vol_spd_ptr->Volume ());
	Exit (vol_spd_ptr->Volume ());

	Veh_Dist (vol_spd_ptr->Volume () * exe->UnRound (length));

	Flow (vol_spd_ptr->Volume ());

	speed = vol_spd_ptr->Speed ();
	if (speed < 0.1) speed = 0.1;

	time = (int) (length / speed + 0.5);
	if (time < dir_ptr->Time0 ()) time = dir_ptr->Time0 ();

	if (dir_ptr->Time0 () > 0) {
		ratio = time * 100.0 / dir_ptr->Time0 ();
	} else {
		ratio = 100;
	}

	if (ratio > max_ratio) {
		ratio = max_ratio / ratio;

		time = (int) (time * ratio + 0.5);
		if (time < dir_ptr->Time0 ()) time = dir_ptr->Time0 ();
		
		speed = length / time;
		ratio = max_ratio;
	}
	Time_Ratio (ratio);
    Speed (speed);
	Time (time);
	Delay (time - dir_ptr->Time0 ());

	Veh_Time (vol_spd_ptr->Volume () * time);
	Veh_Delay (Delay () * Volume ());

	lanes = dir_ptr->Lanes ();
	i = dir_ptr->First_Lane_Use ();

	if (i >= 0) {
		Lane_Use_Period *use_ptr;

		time = (Start () + End () + 1) / 2;

		for (use_ptr = &dat->use_period_array [i]; ; use_ptr = &dat->use_period_array [++i]) {
			if (use_ptr->Start () <= time && time < use_ptr->End ()) {
				if (type == 1) {
					lanes = use_ptr->Lanes1 ();
				} else {
					lanes = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
				}
				break;
			}
			if (use_ptr->Periods () == 0) break;
		}
	}
	if (lanes < 1) lanes = 1;

	lane_len = exe->External_Units (length, ((exe->Metric_Flag ()) ? KILOMETERS : MILES)) * lanes;
	if (lane_len <= 0) lane_len = 1.0;
	Lane_Len (lane_len);

	ratio = (double) (End () - Start () + 1) / Dtime (1.0, HOURS);

	Density ((Volume () / ratio) / lane_len);
	Max_Density (Density ());

	Queue (0);
	Max_Queue (0);
	Failure (0);

	if (vol_spd_ptr->Volume () > 0 && dir_ptr->Capacity () > 0 && End () > 0) {
		if (lanes != dir_ptr->Lanes () && dir_ptr->Lanes () > 0) {
			ratio = ratio * lanes / dir_ptr->Lanes ();
		}
		VC_Ratio (100.0 * vol_spd_ptr->Volume () / (dir_ptr->Capacity () * ratio));
	} else {
		VC_Ratio (0.0);
	}
	Ratio_Dist (0);
	Ratio_Time (0);
	Ratios (0);
	Count (0);

	return (true);
}
