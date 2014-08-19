//*********************************************************
//	Merge_Turn_Delays.cpp - merge turn_delay data
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Merge_Turn_Delays
//---------------------------------------------------------

void PerfPrep::Merge_Turn_Delays (void)
{
	int period, index;
	double turn, merge_turn;
	bool factor_flag;

	Turn_Itr turn_itr;
	Turn_Data *turn_ptr;
	Turn_Period_Itr period_itr;
	Turn_Period *period_ptr;

	//---- merge turn delay files ----

	Show_Message ("Merging Turn Delay Data -- Record");
	Set_Progress ();

	if (method == SIMPLE_LINK_AVG) {
		factor = 1.0;
		factor_flag = false;
	} else {
		factor_flag = (factor == 0.0);
	}

	//---- process each time period ----

	for (period=0, period_itr = turn_period_array.begin (); period_itr != turn_period_array.end (); period_itr++, period++) {

		period_ptr = &merge_turn_array [period];

		//---- process each movement ----

		for (index=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, index++) {
			Show_Progress ();

			turn = turn_itr->Turn ();

			if (method == REPLACE_LINKS && turn > 0.0) continue;

			turn_ptr = period_ptr->Data_Ptr (index);

			if (method == ADD_FLOWS) {
				turn_itr->Add_Turn (turn_ptr->Turn ());
				turn_itr->Add_Time (turn_ptr->Time ());
			} else if (method == REPLACE_LINKS || (method == REPLACE_AVERAGE && turn == 0.0)) {
				turn_itr->Turn (turn_ptr->Turn ());
				turn_itr->Time (turn_ptr->Time ());
			} else if (factor_flag) {
				merge_turn = turn_ptr->Turn ();

				if (turn > 0) {
					factor = merge_turn / turn;
				} else {
					factor = merge_turn;
				}
				turn_itr->Average_Turn_Time (*turn_ptr, factor);
			} else {
				turn_itr->Average_Turn_Time (*turn_ptr);
			}
		}
	}
	End_Progress ();
}

