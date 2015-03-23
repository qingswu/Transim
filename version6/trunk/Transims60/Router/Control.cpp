//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Router::Program_Control (void)
{
	bool update_flag;

	//---- create the network files ----

	Converge_Service::Program_Control ();
										
	Print (2, String ("%s Control Keys:") % Program ());

	if (new_plan_flag && plan_flag) {
		if (script_flag) Print (1);

		update_flag = Get_Control_Flag (UPDATE_PLAN_RECORDS);

		reroute_time = Get_Control_Time (REROUTE_FROM_TIME_POINT);
		reroute_flag = (reroute_time > 0);

		Reroute_Time (reroute_time);
		Update_Flag (update_flag);

		if (update_flag && reroute_flag) {
			Warning ("Re-Routing and Updating Plans are Mutually Exclusive");
			update_flag = false;
		}
	} else if (Get_Control_Time (REROUTE_FROM_TIME_POINT) > 0) {
		Error ("Re-Routing Required an Input and Output Plan File");
	}
}
