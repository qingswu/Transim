//*********************************************************
//	Volume_Array.cpp - volume by time period
//*********************************************************

#include "Volume_Array.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Volume_Array::Initialize (Time_Periods *time_periods, int num)
{
	periods = time_periods;
	num_records = num;

	if (num_records > 0) {
		Doubles record;
		record.assign (num_records, 0);

		if (periods == 0) {
			periods = new Time_Periods ();
		}
		num = periods->Num_Periods ();
		if (num < 1) num = 1;

		assign (num, record);
	}
}

//---------------------------------------------------------
//	Zero_Volumes
//---------------------------------------------------------

void Volume_Array::Zero_Volumes (Dtime first_time)
{
	if (num_records > 0) {
		Vol_Array_Itr period_itr;
		Doubles *vol_ptr;
		int period, first_period, last_period;

		first_period = periods->Period (first_time);
		last_period = periods->Num_Periods ();

		for (period=first_period; period < last_period; period++) {
			vol_ptr = Period_Ptr (period);
			vol_ptr->assign (num_records, 0);
		}
	}
}

//---------------------------------------------------------
//	Volume
//---------------------------------------------------------

double Volume_Array::Volume (int period, int index) 
{
	if (period >= 0 && period < periods->Num_Periods ()) {
		Vol_Period_Ptr vol_ptr = &at (period);
		if (index >= 0 && index < (int) vol_ptr->size ()) {
			return (vol_ptr->at (index));
		}
	}
	return (0);
}

void Volume_Array::Volume (int period, int index, double volume) 
{
	if (period >= 0 && period < periods->Num_Periods ()) {
		Vol_Period_Ptr vol_ptr = &at (period);
		if (index >= 0 && index < (int) vol_ptr->size ()) {
			vol_ptr->at (index) = volume;
		}
	}
}

//---------------------------------------------------------
//	Period_Ptr
//---------------------------------------------------------

Vol_Period_Ptr Volume_Array::Period_Ptr (Dtime time)
{
	int period = periods->Period (time);

	if (period >= 0) {
		return (&at (period));
	} else {
		return (0);
	}
}

//---------------------------------------------------------
//	Add_Vol_Spd
//---------------------------------------------------------

void  Vol_Spd_Data::Add_Vol_Spd (double vol, double spd)
{
	if (volume > 0) {
		double tot = volume + vol;
		speed = (float) ((speed * volume + spd * vol) / tot); 
		volume = (float) tot;
	} else {
		volume = (float) vol;
		speed = (float) spd;
	}
}

//---------------------------------------------------------
//	Total_Vol_Spd
//---------------------------------------------------------

Vol_Spd_Data Vol_Spd_Period::Total_Vol_Spd (int index) 
{
	return (Total_Vol_Spd (index, dat->dir_array [index].Use_Index ()));
}

Vol_Spd_Data Vol_Spd_Period::Total_Vol_Spd (int index, int use_index) 
{
	Vol_Spd_Data rec = at (index);

	if (use_index >= 0) {
		Vol_Spd_Data *ptr = &at (use_index);

		rec.Add_Vol_Spd (ptr->Volume (), ptr->Speed ());
	}
	return (rec);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Vol_Spd_Period_Array::Initialize (Time_Periods *time_periods, int num)
{
	periods = time_periods;

	if (num > 0) {
		num_records = num;
	} else {
		num_records = (int) dat->dir_array.size () + dat->Num_Lane_Use_Flows ();
	}
	if (num_records > 0) {
		Vol_Spd_Period_Itr period_itr;
		Vol_Spd_Period period_rec;
		Vol_Spd_Data vol_spd_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, vol_spd_data);
		}
	}
}

//---------------------------------------------------------
//	Replicate
//---------------------------------------------------------

void Vol_Spd_Period_Array::Replicate (Vol_Spd_Period_Array &period_array)
{
	periods = period_array.periods;
	num_records = period_array.num_records;

	if (num_records > 0) {
		Vol_Spd_Period_Itr period_itr;
		Vol_Spd_Period period_rec;
		Vol_Spd_Data vol_spd_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, vol_spd_data);
		}
	}
}

//---------------------------------------------------------
//	Zero_Data
//---------------------------------------------------------

void Vol_Spd_Period_Array::Zero_Data (void)
{
	if (num_records > 0) {
		Vol_Spd_Period_Itr period_itr;
		Vol_Spd_Data vol_spd_data;

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, vol_spd_data);
		}
	}
}

//---------------------------------------------------------
//	Period_Ptr
//---------------------------------------------------------

Vol_Spd_Period * Vol_Spd_Period_Array::Period_Ptr (Dtime time)
{
	int period = periods->Period (time);

	if (period >= 0) {
		return (&at (period));
	} else {
		return (0);
	}
}
	