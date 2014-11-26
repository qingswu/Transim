//*********************************************************
//	Sublink_Data.cpp - identify subarea boundary links
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Sublink_Data
//---------------------------------------------------------

void SubareaPlans::Sublink_Data (void)
{
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
		if (parking_itr->Type () != BOUNDARY) continue;

		link_ptr = &link_array [parking_itr->Link ()];

		if (parking_itr->Dir () == 0) {
			data_ptr = &sublink_array [link_ptr->AB_Dir ()];
		} else {
			data_ptr = &sublink_array [link_ptr->BA_Dir ()];
		}
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

	//int i;
	//Sublink_Itr itr;
	//for (i=0, itr = sublink_array.begin (); itr != sublink_array.end (); itr++, i++) {
	//	if (itr->parking > 0 || itr->stop > 0 || itr->location > 0) {
	//		Dir_Data *dir_ptr = &dir_array [i];
	//		link_ptr = &link_array [dir_ptr->Link ()];

	//		Write (1, "link=") << link_ptr->Link () << " dir=" << dir_ptr->Dir () << " lot=" << itr->parking << " stop=" << itr->stop << " loc=" << itr->location;
	//	}
	//}
}

