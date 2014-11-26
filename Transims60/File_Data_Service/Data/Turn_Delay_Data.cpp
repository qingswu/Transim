//*********************************************************
//	Turn_Delay_Data.cpp - turning movement data
//*********************************************************

#include "Turn_Delay_Data.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Add_Turn_Time
//---------------------------------------------------------

void  Turn_Data::Add_Turn_Time (double trn, Dtime tim)
{
	if (turn > 0) {
		double tot = turn + trn;
		if (tim >= 0) {
			time = (int) ((time * turn + tim * trn) / tot + 0.5); 
		}
		turn = (float) tot;
	} else {
		turn = (float) trn;
		time = tim;
	}
}

//---------------------------------------------------------
//	Average_Turn
//---------------------------------------------------------

void Turn_Data::Average_Turn (double trn)
{
	if (time > 0) {
		turn = (float) ((turn * time + trn) / (time + 1));
		time = time + 1;
	} else {
		turn = (float) trn;
		time = 1;
	}
}

//---------------------------------------------------------
//	Average_Turn_Time
//---------------------------------------------------------

void  Turn_Data::Average_Turn_Time (double trn, Dtime tim, double weight)
{
	if (tim < 0) tim = 0;
	if (turn > 0) {
		time = (int) ((time * turn + tim * trn * weight) / (weight + 1) + 0.5); 
		turn = (float) ((turn + trn * weight) / (weight + 1));
	} else {
		turn = (float) trn;
		time = tim;
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Turn_Period_Array::Initialize (Time_Periods *time_periods, int num)
{
	periods = time_periods;

	if (num > 0) {
		num_records = num;
	} else {
		num_records = (int) dat->connect_array.size ();
	}
	if (num_records > 0) {
		Turn_Period_Itr period_itr;
		Turn_Period period_rec;
		Turn_Data turn_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, turn_data);
		}
	}
}

//---------------------------------------------------------
//	Replicate
//---------------------------------------------------------

void Turn_Period_Array::Replicate (Turn_Period_Array &period_array)
{
	periods = period_array.periods;
	num_records = period_array.num_records;

	if (num_records > 0) {
		Turn_Period_Itr period_itr;
		Turn_Period period_rec;
		Turn_Data flow_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, flow_data);
		}
	}
}

//---------------------------------------------------------
//	Zero_Times
//---------------------------------------------------------

void Turn_Period_Array::Zero_Times (void)
{
	if (num_records > 0) {
		Turn_Period_Itr period_itr;
		Turn_Itr turn_itr;

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			for (turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++) {
				turn_itr->Time (0);
			}
		}
	}
}

//---------------------------------------------------------
//	Zero_Turns
//---------------------------------------------------------

void Turn_Period_Array::Zero_Turns (Dtime first_time)
{
	if (num_records > 0) {
		Turn_Period_Itr period_itr;
		Turn_Itr turn_itr;
		int period, first_period;

		first_period = periods->Period (first_time);

		for (period=0, period_itr = begin (); period_itr != end (); period_itr++, period++) {
			if (period < first_period) continue;

			for (turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++) {
				turn_itr->Turn (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Copy_Turn_Data
//---------------------------------------------------------

void Turn_Period_Array::Copy_Turn_Data (Turn_Period_Array &period_array, bool zero_flag, Dtime first_time)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i, first_period;

		Turn_Period_Itr period_itr;
		Turn_Period *period_ptr;
		Turn_Itr turn_itr;
		Turn_Data *turn_ptr;

		if (zero_flag) {
			first_period = periods->Period (first_time);
		} else {
			first_period = 0;
		}

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, i++) {
				turn_ptr = period_ptr->Data_Ptr (i);

				turn_itr->Turn (turn_ptr->Turn ());
				turn_itr->Time (turn_ptr->Time ());

				if (zero_flag && p >= first_period) turn_ptr->Turn (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Copy_Time_Data
//---------------------------------------------------------

void Turn_Period_Array::Copy_Time_Data (Turn_Period_Array &period_array, bool zero_flag, Dtime first_time)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i, first_period;
		Turn_Period_Itr period_itr;
		Turn_Period *period_ptr;
		Turn_Itr turn_itr;
		Turn_Data *turn_ptr;

		if (zero_flag) {
			first_period = periods->Period (first_time);
		} else {
			first_period = 0;
		}
		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, i++) {
				turn_ptr = period_ptr->Data_Ptr (i);

				if (zero_flag && p >= first_period) {
					turn_ptr->Turn (0.0);
				} else {
					turn_itr->Turn (turn_ptr->Turn ());
				}
				turn_itr->Time (turn_ptr->Time ());
			}
		}
	}
}

//---------------------------------------------------------
//	Add_Turns
//---------------------------------------------------------

void Turn_Period_Array::Add_Turns (Turn_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Turn_Period_Itr period_itr;
		Turn_Period *period_ptr;
		Turn_Itr turn_itr;
		Turn_Data *turn_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, i++) {
				turn_ptr = period_ptr->Data_Ptr (i);

				turn_itr->Add_Turn (turn_ptr->Turn ());
				if (zero_flag) turn_ptr->Turn (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Add_Turn_Times
//---------------------------------------------------------

void Turn_Period_Array::Add_Turn_Times (Turn_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Turn_Period_Itr period_itr;
		Turn_Period *period_ptr;
		Turn_Itr turn_itr;
		Turn_Data *turn_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, i++) {
				turn_ptr = period_ptr->Data_Ptr (i);

				turn_itr->Add_Turn_Time (turn_ptr->Turn (), turn_ptr->Time ());
				if (zero_flag) turn_ptr->Turn (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Average_Turns
//---------------------------------------------------------

void Turn_Period_Array::Average_Turns (Turn_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Turn_Period_Itr period_itr;
		Turn_Period *period_ptr;
		Turn_Itr turn_itr;
		Turn_Data *turn_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, i++) {
				turn_ptr = period_ptr->Data_Ptr (i);

				turn_itr->Average_Turn (turn_ptr->Turn ());
				if (zero_flag) turn_ptr->Turn (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Average_Turn_Times
//---------------------------------------------------------

void Turn_Period_Array::Average_Turn_Times (Turn_Period_Array &period_array, int weight, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Turn_Period_Itr period_itr;
		Turn_Period *period_ptr;
		Turn_Itr turn_itr;
		Turn_Data *turn_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, i++) {
				turn_ptr = period_ptr->Data_Ptr (i);

				turn_itr->Average_Turn_Time (turn_ptr->Turn (), turn_ptr->Time (), weight);
				if (zero_flag) turn_ptr->Turn (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Period_Ptr
//---------------------------------------------------------

Turn_Period * Turn_Period_Array::Period_Ptr (Dtime time)
{
	int period = periods->Period (time);

	if (period >= 0) {
		return (&at (period));
	} else {
		return (0);
	}
}
