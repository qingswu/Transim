//*********************************************************
//	Map_Trip_Plan.cpp - link trip and plan records
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Map_Trip_Plan
//---------------------------------------------------------

void Router::Map_Trip_Plan (void)
{
	int index;

	Trip_Itr trip_itr;
	Trip_Map_Itr map_itr;
	Trip_Index trip_index;
	Trip_Map_Stat trip_stat;
	Plan_Itr plan_itr;
	Plan_Data plan_rec;

	//---- check and create a trip index ----

	if (plan_trip_map.size () == 0 && plan_array.size () > 0) {

		Show_Message ("Initializing Trip Index");
		Set_Progress ();

		for (index=0, plan_itr = plan_array.begin (); plan_itr != plan_array.end (); plan_itr++, index++) {
			Show_Progress ();

			plan_itr->Get_Index (trip_index);
			plan_itr->Index (index);

			plan_trip_map.insert (Trip_Map_Data (trip_index, index));
		}
		End_Progress ();
	}

	//---- check and allocate memory ----

	if (trip_array.size () > plan_array.size ()) {
		plan_array.reserve (trip_array.size ());
		if (trip_array.size () > plan_array.capacity ()) {
			Error ("Insufficient Memory for Plan Data");
		}
	}
	
	//---- find the plan record for each trip record ----

    if (trip_array.size () > 0) {
	    Show_Message ("Initializing Plan Memory");
	    Set_Progress ();

	    for (trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++) {
		    Show_Progress ();

		    trip_itr->Get_Index (trip_index);

		    map_itr = plan_trip_map.find (trip_index);

		    if (map_itr == plan_trip_map.end ()) {
			    plan_rec = *trip_itr;
			    plan_rec.Index ((int) plan_array.size ());

			    trip_stat = plan_trip_map.insert (Trip_Map_Data (trip_index, plan_rec.Index ()));

			    plan_array.push_back (plan_rec);
			    plan_array.Max_Partition (plan_rec);

			    trip_itr->Index (plan_rec.Index ());
		    } else {
			    trip_itr->Index (map_itr->second);
		    }
	    }
	    End_Progress ();
	}

	//---- allocate memory for trip gap data ----

	if (save_trip_gap || trip_gap_map_flag) {
		Show_Message ("Initializing Trip Gap Memory");
		Set_Progress ();

		Gap_Data gap_data;
		memset (&gap_data, '\0', sizeof (gap_data));

		gap_data_array.assign (plan_array.size (), gap_data);

		if (plan_flag) {
			for (index=0, plan_itr = plan_array.begin (); plan_itr != plan_array.end (); plan_itr++, index++) {
				Show_Progress ();

				gap_data_array [index].previous = plan_itr->Impedance ();
			}
		}
		End_Progress ();
	}
}
