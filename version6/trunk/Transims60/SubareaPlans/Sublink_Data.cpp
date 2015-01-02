//*********************************************************
//	Sublink_Data.cpp - identify subarea boundary links
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Sublink_Data
//---------------------------------------------------------

void SubareaPlans::Sublink_Data (void)
{
	int dir, diff, offset, best_diff, best_loc;
	Boundary_Link data_rec, *data_ptr;
	Parking_Itr parking_itr;
	Location_Itr location_itr;
	Access_Itr access_itr;
	Stop_Itr stop_itr;
	Link_Data *link_ptr;
	Parking_Data *parking_ptr;
	Location_Data *location_ptr;
	Stop_Data *stop_ptr;
	Int_Map_Itr map_itr;

	Show_Message ("Identifying Subarea Boundary Links");
	Set_Progress ();

	memset (&data_rec, '\0', sizeof (data_rec));

	sublink_array.assign (dir_array.size (), data_rec);

	//---- process parking lots ----

	for (parking_itr = parking_array.begin (); parking_itr != parking_array.end (); parking_itr++) {
		Show_Progress ();
		if (!transit_flag && parking_itr->Type () == PARKRIDE) {
			best_loc = 0;
			best_diff = MAX_INTEGER;

			link_ptr = &link_array [parking_itr->Link ()];
			offset = link_ptr->Length () - parking_itr->Offset ();

			for (location_itr = location_array.begin (); location_itr != location_array.end (); location_itr++) {
				if (location_itr->Link () == parking_itr->Link ()) {
					if (location_itr->Dir () == parking_itr->Dir ()) {
						diff = abs (location_itr->Offset () - parking_itr->Offset ());
					} else {
						diff = abs (location_itr->Offset () - offset);
					}
					if (diff < best_diff) {
						best_loc = location_itr->Location ();
						best_diff = diff;
					}
				}
			}
			if (best_loc > 0) {
				pnr_loc.insert (Int_Map_Data (parking_itr->Parking (), best_loc));
			}
		}
		if (parking_itr->Type () != BOUNDARY) continue;

		link_ptr = &link_array [parking_itr->Link ()];

		if (parking_itr->Dir () == 0) {
			dir = link_ptr->AB_Dir ();
		} else {
			dir = link_ptr->BA_Dir ();
		}
		if (dir < 0) Write (1, " parking dir wrong for link=") << link_ptr->Link () << " dir=" << parking_itr->Dir ();

		data_ptr = &sublink_array [dir];
		data_ptr->parking = parking_itr->Parking ();
		data_ptr->parking_offset = parking_itr->Offset ();
	}

	//---- process transit stops ----

	if (transit_flag) {
		for (stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++) {
			Show_Progress ();
			if (stop_itr->Type () != EXTLOAD) continue;

			link_ptr = &link_array [stop_itr->Link ()];

			if (stop_itr->Dir () == 0) {
				data_ptr = &sublink_array [link_ptr->AB_Dir ()];
			} else {
				data_ptr = &sublink_array [link_ptr->BA_Dir ()];
			}
			data_ptr->stop = stop_itr->Stop ();
			data_ptr->stop_offset = stop_itr->Offset ();
		}
	}

	//---- process access links ----

	for (access_itr = access_array.begin (); access_itr != access_array.end (); access_itr++) {
		Show_Progress ();

		if (access_itr->To_Type () == LOCATION_ID) {
			if (access_itr->From_Type () == PARKING_ID) {
				parking_ptr = &parking_array [access_itr->From_ID ()];
				if (parking_ptr->Type () != BOUNDARY) continue;

				location_ptr = &location_array [access_itr->To_ID ()];
				link_ptr = &link_array [parking_ptr->Link ()];

				if (parking_ptr->Dir () == 0) {
					data_ptr = &sublink_array [link_ptr->AB_Dir ()];
				} else {
					data_ptr = &sublink_array [link_ptr->BA_Dir ()];
				}
				data_ptr->location = location_ptr->Location ();

			} else if (transit_flag) {
				if (access_itr->From_Type () == STOP_ID) {
					stop_ptr = &stop_array [access_itr->From_ID ()];
					if (stop_ptr->Type () != EXTLOAD) continue;

					location_ptr = &location_array [access_itr->To_ID ()];
					link_ptr = &link_array [stop_ptr->Link ()];

					if (stop_ptr->Dir () == 0) {
						data_ptr = &sublink_array [link_ptr->AB_Dir ()];
					} else {
						data_ptr = &sublink_array [link_ptr->BA_Dir ()];
					}
					data_ptr->location = location_ptr->Location ();
				}
			} else {
				continue;
			}
		} else if (access_itr->From_Type () == LOCATION_ID) {
			if (access_itr->To_Type () == PARKING_ID) {
				parking_ptr = &parking_array [access_itr->To_ID ()];

				if (parking_ptr->Type () != BOUNDARY) continue;

				location_ptr = &location_array [access_itr->From_ID ()];
				link_ptr = &link_array [parking_ptr->Link ()];

				if (parking_ptr->Dir () == 0) {
					data_ptr = &sublink_array [link_ptr->AB_Dir ()];
				} else {
					data_ptr = &sublink_array [link_ptr->BA_Dir ()];
				}
				data_ptr->location = location_ptr->Location ();
			} else if (transit_flag) {
				if (access_itr->To_Type () == STOP_ID) {
					stop_ptr = &stop_array [access_itr->To_ID ()];
					if (stop_ptr->Type () != EXTLOAD) continue;

					location_ptr = &location_array [access_itr->From_ID ()];
					link_ptr = &link_array [stop_ptr->Link ()];

					if (stop_ptr->Dir () == 0) {
						data_ptr = &sublink_array [link_ptr->AB_Dir ()];
					} else {
						data_ptr = &sublink_array [link_ptr->BA_Dir ()];
					}
					data_ptr->location = location_ptr->Location ();
				}
			} else {
				continue;
			}
		}
	}

	for (location_itr = location_array.begin (); location_itr != location_array.end (); location_itr++) {
		Show_Progress ();
		link_ptr = &link_array [location_itr->Link ()];

		if (location_itr->Dir () == 0) {
			data_ptr = &sublink_array [link_ptr->AB_Dir ()];
		} else {
			data_ptr = &sublink_array [link_ptr->BA_Dir ()];
		}
		if (data_ptr->location > 0) continue;

		if (data_ptr->parking > 0) {
			if (data_ptr->parking_offset == location_itr->Offset ()) {
				data_ptr->location = location_itr->Location ();
			}
		} else if (data_ptr->stop > 0) {
			if (data_ptr->stop_offset == location_itr->Offset ()) {
				data_ptr->location = location_itr->Location ();
			}
		}
	}
	End_Progress ();
}

