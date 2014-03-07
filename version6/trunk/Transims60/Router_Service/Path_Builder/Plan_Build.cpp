//*********************************************************
//	Plan_Build - Build a Path and Plan records
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Build
//---------------------------------------------------------

bool Path_Builder::Plan_Build (Plan_Data *plan_data)
{
	int veh_id, lot, stat;
	
	Trip_End trip_end;	
	Int_Map_Itr map_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;

	if (plan_data == 0) {
		cout << "\tPlan Pointer is Zero" << endl;
		return (false);
	}
	if (!initialized) {
		cout << "\tPath Building Requires TRANSIMS Router Services" << endl;
		return (false);
	}
	plan_flag = true;
	plan_ptr = plan_data;
	stat = plan_ptr->Depart ();
	plan_ptr->Clear_Plan ();
	plan_ptr->Depart (stat);

	if (plan_ptr->Mode () >= MAX_MODE || !mode_path_flag [plan_ptr->Mode ()]) return (false);

	//---- set the traveler parameters ----

	exe->Set_Parameters (param, plan_ptr->Type ());

	param.mode = (Mode_Type) plan_ptr->Mode (),
	parking_duration = plan_ptr->Duration ();
	forward_flag = (plan_ptr->Constraint () != END_TIME);
	time_limit = (forward_flag) ? MAX_INTEGER : 0;
	reroute_flag = false;

	//---- initialize the plan ----

	trip_org.clear ();
	trip_des.clear ();

	//---- set the origin ----

	map_itr = exe->location_map.find (plan_ptr->Origin ());

	if (map_itr == exe->location_map.end ()) {
		plan_ptr->Problem (LOCATION_PROBLEM);
		return (true);
	}
	trip_end.Type (LOCATION_ID);
	trip_end.Index (map_itr->second);

	if (plan_ptr->Depart () > 0) {
		trip_end.Time (plan_ptr->Depart ());
	} else {
		trip_end.Time (plan_ptr->Start ());
	}
	trip_org.push_back (trip_end);

	//---- set the destination ----

	map_itr = exe->location_map.find (plan_ptr->Destination ());

	if (map_itr == exe->location_map.end ()) {
		plan_ptr->Problem (LOCATION_PROBLEM);
		return (true);
	}
	trip_end.Type (LOCATION_ID); 
	trip_end.Index (map_itr->second);

	if (plan_ptr->Arrive () > 0) {
		trip_end.Time (plan_ptr->Arrive ());
	} else {
		trip_end.Time (plan_ptr->End ());
	}
	trip_des.push_back (trip_end);

	//---- get the vehicle record ----

	veh_id = plan_ptr->Vehicle ();
	lot = -1;
	pce = 1.0;

	if (veh_id <= 0 || !veh_type_flag) {
		grade_flag = false;
		op_cost_rate = 0.0;
		param.use = CAR;
		param.veh_type = -1;
	} else {
		map_itr = exe->veh_type_map.find (plan_ptr->Veh_Type ());

		if (map_itr != exe->veh_type_map.end ()) {
			param.veh_type = map_itr->second;
			veh_type_ptr = &exe->veh_type_array [param.veh_type];

			param.use = veh_type_ptr->Use ();
			op_cost_rate = UnRound (veh_type_ptr->Op_Cost ());

			if (Metric_Flag ()) {
				op_cost_rate /= 1000.0;
			} else {
				op_cost_rate /= MILETOFEET;
			}
			grade_flag = param.grade_flag && veh_type_ptr->Grade_Flag ();
			pce = UnRound (veh_type_ptr->PCE ());
		} else {
			param.veh_type = -1;
			grade_flag = false;
			op_cost_rate = 0.0;
			param.use = CAR;
			exe->Warning (String ("Vehicle Type %d was Not Found") % plan_ptr->Veh_Type ());
		}
	}

	//---- build the path -----

	stat = Build_Path (lot);

	if (stat < 0) return (false);

	if (stat > 0) {
		if (!param.ignore_errors) {
			//skip = true;
		}
		plan_ptr->Problem (stat);
	}
	return (true);
}
