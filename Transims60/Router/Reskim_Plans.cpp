//*********************************************************
//	Reskim_Plans.cpp - update plan trip and leg times
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Reskim_Plans
//---------------------------------------------------------

double Router::Reskim_Plans (bool average_flag)
{
	int mode;
	
	Plan_Ptr_Array *ptr_array;
	Plan_Itr plan_itr;

	//---- average update travel times ----

	Update_Travel_Times (average_flag, true);
	num_time_updates++;
	
	//---- initialize data ----

	plan_processor->Start_Processing (true, true);

	//---- reskim the plans ----

	for (plan_itr = plan_array.begin (); plan_itr != plan_array.end (); plan_itr++) {
		if (plan_itr->Household () < 1 || plan_itr->Problem () > 0) continue;
		Show_Progress ();

		mode = plan_itr->Mode ();

		if (mode == WAIT_MODE || mode == TRANSIT_MODE || mode == WALK_MODE || mode == BIKE_MODE || 
			mode == RIDE_MODE || mode == OTHER_MODE) continue;

		ptr_array = new Plan_Ptr_Array ();

		plan_itr->Method (RESKIM_PLAN);
			
		ptr_array->push_back (&(*plan_itr));
					
		plan_processor->Plan_Build (ptr_array);
	}
	plan_processor->Stop_Processing (true);

	return (Skim_Gap ());
}
