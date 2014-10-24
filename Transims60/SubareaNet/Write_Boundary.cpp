//*********************************************************
//	Write_Boundary.cpp - Create Boundary Access Points
//*********************************************************

#include "SubareaNet.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Boundary
//---------------------------------------------------------

void SubareaNet::Write_Boundary (void)
{
	bool access_flag;
	int i, j, node, index;
	double length, len, xy_len, dx, dy, xa, ya, xb, yb;
	double factor;

	Node_Data *anode_ptr, *bnode_ptr;
	Link_Itr link_itr;
	Dir_Itr dir_itr;
	Dir_Data *dir_ptr;
	Line_Itr line_itr;
	Stop_Data stop_rec;
	Int_Itr itr;

	access_flag = System_File_Flag (NEW_ACCESS_LINK);

	Location_File *location_file = System_Location_File (true);
	Parking_File *parking_file = System_Parking_File (true);
	Access_File *access_file = System_Access_File (true);
	Zone_File *zone_file;
	Stop_File *stop_file;

	char *ext_dir [] = {
		"Subarea Origin", "Subarea Destination"
	};

	//---- flag links on transit routes ----

	if (transit_flag) {
		max_stop = ((max_stop + 1000) / 1000) * 1000;
		stop_file = System_Stop_File (true);

		for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
			for (itr = line_itr->driver_array.begin (); itr != line_itr->driver_array.end (); itr++) {
				dir_ptr = &dir_array [*itr];
				dir_ptr->Use_Index (0);
			}
		}
	} else {
		stop_file = 0;
	}
	if (zone_flag) {
		zone_file = System_Zone_File (true);
	} else {
		zone_file = 0;
	}

	//---- round up the ID codes ----

	max_location = ((max_location + 1000) / 1000) * 1000;
	max_parking = ((max_parking + 1000) / 1000) * 1000;
	max_access = ((max_access + 1000) / 1000) * 1000;
	max_zone = ((max_zone + 10) / 10) * 10;
	if (max_zone > 0) max_zone--;

	//---- initialize the data fields ----

	for (i=0; i < location_file->Num_Fields (); i++) {
		location_file->Put_Field (i, 0);
	}

	//---- process each boundary link ----

	Show_Message ("Writing Boundary Link Data -- Record");
	Set_Progress ();

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		//---- set the link orientation ----

		if (link_itr->Type () != 1 && link_itr->Type () != 2) continue;

		//---- get the coordinates ----

		anode_ptr = &node_array [link_itr->Anode ()];

		xa = anode_ptr->X ();
		ya = anode_ptr->Y ();

		bnode_ptr = &node_array [link_itr->Bnode ()];

		xb = bnode_ptr->X ();
		yb = bnode_ptr->Y ();

		//---- check the link length ----

		dx = xb - xa;
		dy = yb - ya;

		xy_len = sqrt (dx * dx + dy * dy);

		if (xy_len < 4 * external_offset) {
			Warning (String ("Link %d from %d to %d is Too Short for Boundary Access ") % 
				link_itr->Link () % anode_ptr->Node () % bnode_ptr->Node ());

			if (xy_len < 2 * external_offset) {
				nshort++;
				continue;
			}
		}

		//--- set the access point ----

		length = UnRound (link_itr->Length ());

		factor = external_offset / xy_len;

		nzone++;
		new_zone++;
		max_zone++;

		if (zone_flag) {
			zone_file->Zone (max_zone);
			zone_file->Area_Type (0);
			if (link_itr->Type () == 1) {
				zone_file->X (UnRound (xb));
				zone_file->Y (UnRound (yb));
			} else {
				zone_file->X (UnRound (xa));
				zone_file->Y (UnRound (ya));
			}
			zone_file->Notes ("Subarea External");
			if (!zone_file->Write ()) {
				Error (String ("Writing %s") % zone_file->File_Type ());
			}
		}

		//--- build activity connections in each direction ----
		
		for (i=0; i < 2; i++) {
			if (i) {
				if (link_itr->BA_Dir () < 0) continue;
				dir_ptr = &dir_array [link_itr->BA_Dir ()];
				node = link_itr->Anode ();

				if (link_itr->Type () == 2) {
					len = UnRound (Round (length * (1.0 - factor)));
					j = 1;
				} else {
					len = UnRound (Round (length * factor));
					j = 0;
				}
			} else {
				if (link_itr->AB_Dir () < 0) continue;
				dir_ptr = &dir_array [link_itr->AB_Dir ()];
				node = link_itr->Bnode ();

				if (link_itr->Type () == 1) {
					len = UnRound (Round (length * (1.0 - factor)));
					j = 1;
				} else {
					len = UnRound (Round (length * factor));
					j = 0;
				}
			}

			//---- add activity locations ----

			nlocation++;
			new_location++;
			max_location++;

			location_file->Location (max_location);
			location_file->Link (link_itr->Link ());

			if (location_file->LinkDir_Type () == LINK_SIGN) {
				if (dir_ptr->Dir () != 0) {
					location_file->Link (-link_itr->Link ());
				}
			} else if (location_file->LinkDir_Type () == LINK_DIR) {
				location_file->Dir (dir_ptr->Dir ());
			} else {
				location_file->Dir (node);
			}
			location_file->Offset (len);
			location_file->Setback (100);

			location_file->Notes (ext_dir [j]);
			location_file->Zone (max_zone);

			if (!location_file->Write ()) {
				Error (String ("Writing %s") % location_file->File_Type ());
			}

			//---- add parking lot ----

			nparking++;
			new_parking++;
			max_parking++;

			parking_file->Parking (max_parking);
			parking_file->Link (link_itr->Link ());

			if (parking_file->LinkDir_Type () == LINK_SIGN) {
				if (dir_ptr->Dir () != 0) {
					parking_file->Link (-link_itr->Link ());
				}
			} else if (parking_file->LinkDir_Type () == LINK_DIR) {
				parking_file->Dir (dir_ptr->Dir ());
			} else {
				parking_file->Dir (node);
			}
			parking_file->Offset (len);
			parking_file->Type (BOUNDARY);
			parking_file->Use (ANY);
			parking_file->Start (0);
			parking_file->End (Model_End_Time ());
			parking_file->Space (0);
			parking_file->Time_In (0);
			parking_file->Time_Out (0);
			parking_file->Hourly (0);
			parking_file->Daily (0);

			parking_file->Notes (ext_dir [j]);

			if (!parking_file->Write (false)) {
				Error (String ("Writing %s") % parking_file->File_Type ());
			}

			//---- add access link ----

			if (access_flag) {
				naccess++;
				new_access++;
				max_access++;

				access_file->Link (max_access);
				access_file->From_ID (max_location);
				access_file->From_Type (LOCATION_ID);
				access_file->To_ID (max_parking);
				access_file->To_Type (PARKING_ID);
				access_file->Dir (2);
				access_file->Time (1);
				access_file->Cost (0);
				access_file->Notes (ext_dir [j]);
			
				if (!access_file->Write ()) {
					Error (String ("Writing %s") % access_file->File_Type ());
				}
			}

			//---- add the transit external ----

			if (transit_flag && dir_ptr->Use_Index () == 0) {

				//---- add transit stop ----

				nstop++;
				new_stop++;
				max_stop++;

				stop_file->Stop (max_stop);
				stop_file->Name ("EXTERNAL");
				stop_file->Link (link_itr->Link ());

				if (stop_file->LinkDir_Type () == LINK_SIGN) {
					if (dir_ptr->Dir () != 0) {
						stop_file->Link (-link_itr->Link ());
					}
				} else if (stop_file->LinkDir_Type () == LINK_DIR) {
					stop_file->Dir (dir_ptr->Dir ());
				} else {
					stop_file->Dir (node);
				}
				stop_file->Offset (len);
				stop_file->Type (EXTLOAD);
				stop_file->Use (ANY);
				stop_file->Space (0);
				stop_file->Notes (ext_dir [j]);

				if (!stop_file->Write ()) {
					Error (String ("Writing %s") % stop_file->File_Type ());
				}

				//---- add stop data ----

				stop_rec.Stop (max_stop);
				stop_rec.Link_Dir (dir_ptr->Link_Dir ());
				stop_rec.Offset (Round (len));

				index = (int) stop_array.size ();

				stop_map.insert (Int_Map_Data (max_stop, index));
				stop_array.push_back (stop_rec);

				dir_ptr->Use_Index (index);

				//---- add access link ----

				if (access_flag) {
					naccess++;
					new_access++;
					max_access++;

					access_file->Link (max_access);
					access_file->From_ID (max_location);
					access_file->From_Type (LOCATION_ID);
					access_file->To_ID (max_stop);
					access_file->To_Type (STOP_ID);
					access_file->Dir (2);
					access_file->Time (1);
					access_file->Cost (0);
					access_file->Notes (ext_dir [i]);
			
					if (!access_file->Write ()) {
						Error (String ("Writing %s") % access_file->File_Type ());
					}
				}
			}
		}
	}
	End_Progress ();
}
