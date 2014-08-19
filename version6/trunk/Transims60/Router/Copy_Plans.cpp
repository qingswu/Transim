//*********************************************************
//	Copy_Plans.cpp - Copy the Plan File
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Copy_Plans
//---------------------------------------------------------

bool Router::Copy_Plans (int part, Plan_Processor *plan_process_ptr)
{
	int last_hhold, hhold, max_hhold, part_number;
	string process_type, process_name;
	bool keep_flag, part_flag;

	Trip_Map_Itr map_itr;
	Plan_File *plan_file = 0;
	Plan_Data plan_rec, *plan_ptr, *copy_ptr;
	Plan_Ptr_Array *plan_ptr_array;

	//---- start processing plans ----

	max_hhold = MAX_INTEGER;

	if (select_households) {
		max_hhold = hhold_range.Max_Value ();
	}
	if (select_flag) {
		hhold = select_map.Max_Household ();
		if (hhold < max_hhold) max_hhold = hhold;
	}

	if (update_flag) {
		process_type = "Updating";
	} else if (reroute_flag) {
		process_type = "Re-Routing";
	} else {
		process_type = "Copying";
	}
    if (plan_memory_flag) {
        process_name = "Travel Plans"; 
        part_flag = false;
        part_number = part;
	} else {
	    if (new_set_flag) {
		    plan_file = plan_file_set [part];
	    } else {
		    plan_file = Router::plan_file;
	    }
        process_name = plan_file->File_Type ();
        part_flag = plan_file->Part_Flag ();
        part_number = plan_file->Part_Number ();
	}

	if (thread_flag) {
		MAIN_LOCK
		if (part_flag) {
			Show_Message (String ("%s %s %d") % process_type % process_name % part_number);
		} else {
			Show_Message (String ("%s %s") % process_type % process_name);
		}
		END_LOCK
	} else {
		if (part_flag) {
			Show_Message (String ("%s %s %d -- Trip") % process_type % process_name % part_number);
		} else {
			Show_Message (String ("%s %s -- Trip") % process_type % process_name);
		}
		Set_Progress ();
	}

	last_hhold = -1;
	plan_ptr_array = new Plan_Ptr_Array ();

    //---- process plans in memory ----

    if (plan_memory_flag) {

	    for (map_itr = plan_trip_map.begin (); map_itr != plan_trip_map.end (); map_itr++) {
		    if (thread_flag) {
			    Show_Dot ();
		    } else {
			    Show_Progress ();
		    }
		    plan_ptr = &plan_array [map_itr->second];

		    //---- check the household id ----

		    hhold = plan_ptr->Household ();
		    if (hhold < 1) continue;

		    if (hhold > max_hhold) break;
		    if (hhold != last_hhold) {
			    if (last_hhold > 0 && plan_ptr_array->size () > 0) {
				    plan_process_ptr->Plan_Build (plan_ptr_array);
				    plan_ptr_array = new Plan_Ptr_Array ();
			    }
			    last_hhold = hhold;
		    }

		    //---- update the paths ----

		    if (update_flag) {
			    plan_ptr->Method (UPDATE_PLAN);
		    } else if (reroute_flag) {
			    if (plan_ptr->Depart () >= reroute_time) {
				    plan_ptr->Method (BUILD_PATH);
			    } else if (plan_ptr->Arrive () < reroute_time) {
				    plan_ptr->Method (COPY_PLAN);
			    } else {
				    plan_ptr->Method (REROUTE_PATH);
				    plan_ptr->Arrive (reroute_time);
			    }
		    } else if (Link_Flows ()) {
			    plan_ptr->Method (PATH_FLOWS);
		    } else {
			    plan_ptr->Method (COPY_PLAN);
		    }
            copy_ptr = new Plan_Data ();
            *copy_ptr = *plan_ptr;

		    plan_ptr_array->push_back (copy_ptr);
		}

	} else {

        //---- file-based processing ----

	    while (plan_file->Read_Plan (plan_rec)) {
		    if (thread_flag) {
			    Show_Dot ();
		    } else {
			    Show_Progress ();
		    }

		    //---- check the household id ----

		    hhold = plan_rec.Household ();
		    if (hhold < 1) continue;

		    if (hhold != last_hhold) {
			    if (last_hhold > 0 && plan_ptr_array->size () > 0) {
				    plan_process_ptr->Plan_Build (plan_ptr_array);
				    plan_ptr_array = new Plan_Ptr_Array ();
			    }
			    last_hhold = hhold;
		    }
		    keep_flag = Selection (&plan_rec);

		    if (!update_flag && !reroute_flag && !keep_flag) continue;
							
		    plan_rec.Internal_IDs ();

		    //---- update the paths ----

		    if (update_flag && keep_flag) {
			    plan_rec.Method (UPDATE_PLAN);
		    } else if (reroute_flag) {
			    if (plan_rec.Depart () >= reroute_time) {
				    plan_rec.Method (BUILD_PATH);
			    } else if (plan_rec.Arrive () < reroute_time) {
				    plan_rec.Method (COPY_PLAN);
			    } else {
				    plan_rec.Method (REROUTE_PATH);
				    plan_rec.Arrive (reroute_time);
			    }
		    } else if (Link_Flows ()) {
			    plan_rec.Method (PATH_FLOWS);
		    } else {
			    plan_rec.Method (COPY_PLAN);
		    }
            copy_ptr = new Plan_Data ();
            *copy_ptr = plan_rec;

		    plan_ptr_array->push_back (copy_ptr);
		}
	    plan_file->Close ();
	}
	if (last_hhold > 0 && plan_ptr_array->size () > 0) {
		plan_process_ptr->Plan_Build (plan_ptr_array);
		plan_ptr_array = new Plan_Ptr_Array ();
	}
	if (!thread_flag) {
		End_Progress ();
	}
	delete plan_ptr_array;
	return (true);
}
