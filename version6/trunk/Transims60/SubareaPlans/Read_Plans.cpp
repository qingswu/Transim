//*********************************************************
//	Read_Plans.cpp - read the travel plan file
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void SubareaPlans::Plan_Processing::Read_Plans (int part)
{
	int mode;

	Plan_Data plan;
	Select_Map_Itr sel_itr;

	//---- open the file partition ----

	if (!plan_file->Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % plan_file->Filename ()); 
		END_LOCK
	}
	if (thread_flag) {
		MAIN_LOCK
		if (plan_file->Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d") % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s") % plan_file->File_Type ());
		}
		END_LOCK
	} else {
		if (plan_file->Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d -- Record") % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s -- Record") % plan_file->File_Type ());
		}
		exe->Set_Progress ();
	}

	//---- read the plan file ----

	while (plan_file->Read_Plan (plan)) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}
		num_trips++;

		//---- check the selection criteria ----

		mode = plan.Mode ();
		
		if (exe->select_households && !exe->hhold_range.In_Range (plan.Household ())) continue;
		if (plan.Mode () < MAX_MODE && !exe->select_mode [plan.Mode ()]) continue;
		if (exe->select_purposes && !exe->purpose_range.In_Range (plan.Purpose ())) continue;
		if (exe->select_travelers && !exe->traveler_range.In_Range (plan.Type ())) continue;
		if (exe->select_start_times && !exe->start_range.In_Range (plan.Depart ())) continue;
		if (exe->select_end_times && !exe->end_range.In_Range (plan.Arrive ())) continue;
		if (exe->select_origins && !exe->org_range.In_Range (plan.Origin ())) continue;
		if (exe->select_destinations && !exe->des_range.In_Range (plan.Destination ())) continue;
		if (exe->select_stops && !exe->Select_Plan_Stops (plan)) continue;
		if (exe->select_routes && !exe->Select_Plan_Routes (plan)) continue;
		if (exe->select_links && !exe->Select_Plan_Links (plan)) continue;
		if (exe->select_nodes && !exe->Select_Plan_Nodes (plan)) continue;
		
		//---- check the selection records ----

		if (exe->select_flag) {
			sel_itr = exe->select_map.Best (plan.Household (), plan.Person (), 
				plan.Tour (), plan.Trip ());
			if (sel_itr == exe->select_map.end ()) continue;
		}

		//---- check the deletion records ----
		
		if (exe->delete_households && exe->hhold_delete.In_Range (plan.Household ())) continue;
		if (plan.Mode () < MAX_MODE && exe->delete_mode [plan.Mode ()]) continue;
		if (exe->delete_travelers && exe->traveler_delete.In_Range (plan.Type ())) continue;

		exe->Convert_Plan (plan);
	}
	if (!thread_flag) exe->End_Progress ();

	plan_file->Close ();
}
