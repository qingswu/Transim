//*********************************************************
//	Plan_Stop - Build a Path with an intermediate stop
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Stop
//---------------------------------------------------------

bool Path_Builder::Plan_Stop (void)
{
	if (plan_ptr->Stop_Location () < 0) return (false);

	int destination = plan_ptr->Destination ();
	Dtime stop_duration = plan_ptr->Activity ();

	plan_ptr->Destination (plan_ptr->Stop_Location ());

	if (!Plan_ReRoute ()) return (false);

	Plan_Leg leg_rec;

	leg_rec.Type (LOCATION_ID);
	leg_rec.ID (plan_ptr->Destination ());
	leg_rec.Mode (WAIT_MODE);
	leg_rec.Impedance (Resolve (path_param.value_time * stop_duration));
	leg_rec.Time (stop_duration);
	leg_rec.Length (0);
	leg_rec.Cost (0);

	plan_ptr->push_back (leg_rec);
	plan_ptr->Add_Wait (stop_duration);

	plan_ptr->Arrive (plan_ptr->Arrive () + stop_duration);
	plan_ptr->Destination (destination);
	plan_ptr->Constraint (START_TIME);
	plan_ptr->Priority (CRITICAL);

	reroute_flag = false;

	return (Plan_Extend ());
}
