//*********************************************************
//	Plan_Build - Build a Path and Plan records
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Build
//---------------------------------------------------------

bool Path_Builder::Plan_Build (void)
{
	int stat;
	
	Trip_End trip_end;	

	plan_flag = true;

	stat = plan_ptr->Depart ();
	plan_ptr->Clear_Plan ();
	plan_ptr->Depart (stat);

	if (plan_ptr->Mode () >= MAX_MODE || !mode_path_flag [plan_ptr->Mode ()]) return (false);

	//---- set the traveler parameters ----

	exe->Set_Parameters (path_param, plan_ptr->Type (), plan_ptr->Veh_Type ());

	path_param.mode = (Mode_Type) plan_ptr->Mode (),
	parking_duration = plan_ptr->Duration ();
	forward_flag = (plan_ptr->Constraint () != END_TIME);
	time_limit = (forward_flag) ? MAX_INTEGER : 0;

	//---- initialize the plan ----

	trip_org.clear ();
	trip_des.clear ();

	//---- set the origin ----

	trip_end.Type (LOCATION_ID);
	trip_end.Index (plan_ptr->Origin ());

	if (plan_ptr->Depart () > 0) {
		trip_end.Time (plan_ptr->Depart ());
	} else {
		trip_end.Time (plan_ptr->Start ());
	}
	trip_org.push_back (trip_end);

	//---- set the destination ----

	trip_end.Type (LOCATION_ID); 
	trip_end.Index (plan_ptr->Destination ());

	if (plan_ptr->Arrive () > 0) {
		trip_end.Time (plan_ptr->Arrive ());
	} else {
		trip_end.Time (plan_ptr->End ());
	}
	trip_des.push_back (trip_end);

	//---- build the path -----

	stat = Build_Path (parking_lot);

	if (stat < 0) return (false);

	if (stat > 0) {
		if (!path_param.ignore_errors) {
			//skip = true;
		}
		plan_ptr->Problem (stat);
	}
	return (true);
}
