//*********************************************************
//	Plan_Extend - Build a Path after a Specified Time
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Extend
//---------------------------------------------------------

bool Path_Builder::Plan_Extend (void)
{
	int stat, walk, cost, length, drive, imped, other, transit, wait;
	Dtime start_time, depart;

	Trip_End trip_end;
	Plan_Leg *leg_ptr;

	plan_flag = true;

	//---- set the traveler parameters ----

	exe->Set_Parameters (path_param, plan_ptr->Type (), plan_ptr->Veh_Type ());

	path_param.mode = (Mode_Type) plan_ptr->Mode (),
	parking_duration = plan_ptr->Duration ();
	forward_flag = true;

	//---- initialize the plan ----

	trip_org.clear ();
	trip_des.clear ();

	start_time = plan_ptr->Arrive ();

	leg_ptr = &plan_ptr->at (plan_ptr->size () - 1);

	if (leg_ptr->Type () != LOCATION_ID) return (false);

	walk = plan_ptr->Walk ();
	cost = plan_ptr->Cost ();
	length = plan_ptr->Length ();
	drive = plan_ptr->Drive ();
	imped = plan_ptr->Impedance ();
	other = plan_ptr->Other ();
	transit = plan_ptr->Transit ();
	wait = plan_ptr->Wait ();
	depart = plan_ptr->Depart ();

	//---- set the origin ----

	trip_end.Type (LOCATION_ID);
	trip_end.Index (leg_ptr->ID ());
	trip_end.Time (start_time);

	trip_org.push_back (trip_end);

	//---- set the destination ----

	trip_end.Index (plan_ptr->Destination ());
	trip_end.Type (LOCATION_ID);
	trip_end.Time (start_time + plan_ptr->End () - plan_ptr->Start ());

	trip_des.push_back (trip_end);
	time_limit = MAX_INTEGER;

	stat = Build_Path (-1);

	if (stat < 0) return (false);
	if (stat > 0) {
		if (!path_param.ignore_errors) {
			//skip = true;
		}
		plan_ptr->Problem (stat);
	}
	plan_ptr->Add_Walk (walk);
	plan_ptr->Add_Cost (cost);
	plan_ptr->Add_Length (length);
	plan_ptr->Add_Drive (drive);
	plan_ptr->Add_Impedance (imped);
	plan_ptr->Add_Other (other);
	plan_ptr->Add_Transit (transit);
	plan_ptr->Add_Wait (wait);
	plan_ptr->Depart (depart);

	return (true);
}
