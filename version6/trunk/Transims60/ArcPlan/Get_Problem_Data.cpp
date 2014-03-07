//*********************************************************
//	Get_Problem_Data.cpp - Read the Problem File
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Get_Problem_Data
//---------------------------------------------------------

bool ArcPlan::Get_Problem_Data (Problem_File &file, Problem_Data &problem_rec, int partition)
{
	Location_Data *loc_ptr;
	Trip_Index trip_index;

	if (Data_Service::Get_Problem_Data (file, problem_rec, partition)) {
		if (select_problems && !problem_range.In_Range (problem_rec.Problem ())) return (false);
		if (select_households && !hhold_range.In_Range (problem_rec.Household ())) return (false);
		if (problem_rec.Mode () < MAX_MODE && !select_mode [problem_rec.Mode ()]) return (false);
		if (select_purposes && !purpose_range.In_Range (problem_rec.Purpose ())) return (false);
		if (select_start_times && !start_range.In_Range (problem_rec.Start ())) return (false);
		if (select_end_times && !end_range.In_Range (problem_rec.End ())) return (false);
		if (select_origins && !org_range.In_Range (file.Origin ())) return (false);
		if (select_destinations && !des_range.In_Range (file.Destination ())) return (false);

		if (select_org_zones) {
			loc_ptr = &location_array [problem_rec.Origin ()];
			if (!org_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
		if (select_des_zones) {
			loc_ptr = &location_array [problem_rec.Destination ()];
			if (!des_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
		if (select_travelers && !traveler_range.In_Range (problem_rec.Type ())) return (false);

		//---- check the selection records ----

		if (System_File_Flag (SELECTION)) {
			Select_Map_Itr sel_itr;

			sel_itr = select_map.Best (problem_rec.Household (), problem_rec.Person (), problem_rec.Tour (), problem_rec.Trip ());
			if (sel_itr == select_map.end ()) return (false);
		}

		//---- draw the problem record ----

		if (problem_out) {
			XYZ_Point point;

			arcview_problem.clear ();
			arcview_problem.Copy_Fields (file);

			if (problem_method < 2) {
				loc_ptr = &location_array [problem_rec.Origin ()];
				point.x = UnRound (loc_ptr->X ());
				point.y = UnRound (loc_ptr->Y ());

				arcview_problem.push_back (point);
			}
			if (problem_method != 1) {
				loc_ptr = &location_array [problem_rec.Destination ()];
				point.x = UnRound (loc_ptr->X ());
				point.y = UnRound (loc_ptr->Y ());

				arcview_problem.push_back (point);
			}
			if (!arcview_problem.Write_Record ()) {
				Error (String ("Writing %s") % arcview_problem.File_Type ());
			}
			num_problem++;
		}

		//---- save the problem index ----

		if (plan_flag) {
			trip_index.Set (problem_rec.Household (), problem_rec.Person (), problem_rec.Tour (), problem_rec.Trip ());

			problem_map.insert (Trip_Map_Data (trip_index, problem_rec.Problem ()));
		}
	}
	return (false);
}
