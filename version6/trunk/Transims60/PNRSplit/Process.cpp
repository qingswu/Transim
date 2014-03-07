//*********************************************************
//	Process.cpp - group processing
//*********************************************************

#include "PNRSplit.hpp"

//---------------------------------------------------------
//	Process
//---------------------------------------------------------

void PNRSplit::Process (void)
{
	int i, t, z, num_tabs, len, zone, count;
	char **tab_names;
	MAT_INFO info;

	Int_Map_Itr map_itr;

	//---- create the new matrix ----
	
	Show_Message ("Initializing Output Trip Tables");

	MATRIX_GetInfo (pnr_trip, &info);

	MATRIX_GetIDs (pnr_trip, MATRIX_ROW, row_id);
	MATRIX_GetIDs (pnr_trip, MATRIX_COL, col_id);

	for (z=0; z < num_zones; z++) {
		org_map.insert (Int_Map_Data (row_id [z], z));
		des_map.insert (Int_Map_Data (col_id [z], z));
	}
	num_tabs = 2;
	tab_names = new char * [num_tabs];

	len = (int) walk_table.size () + 1;
	tab_names [0] = new char [len];
	strcpy_s (tab_names [0], len, walk_table.c_str ());

	len = (int) drive_table.size () + 1;
	tab_names [1] = new char [len];
	strcpy_s (tab_names [1], len, drive_table.c_str ());
	
	new_trip = MATRIX_New ((char *) new_filename.c_str (), info.szLabel, num_zones, row_id, 
		num_zones, col_id, num_tabs, tab_names, info.Type, (short) info.Compression);
	
	for (t=0; t < num_tabs; t++) {
		delete [] tab_names [t];
	}

	//---- read and convert the parking nodes ----
	
	Show_Message (2, "Reading Trip and Skim Tables -- Record");
	Set_Progress ();

	MATRIX_SetCore (pnr_trip, (short) pnr_id);
	MATRIX_SetCore (node_skim, (short) node_id);

	for (z=0; z < num_zones; z++) {
		Show_Progress ();
				
		if (select_org_zones && !org_zone_range.In_Range (row_id [z])) continue;

		MATRIX_GetBaseVector (pnr_trip, z, MATRIX_ROW, FLOAT_TYPE, pnr_data);		
		MATRIX_GetBaseVector (node_skim, z, MATRIX_ROW, LONG_TYPE, node_data);

		if (tc_status != TC_OKAY) goto read_error;

		for (i=0; i < num_zones; i++) {
			if (pnr_data [i] > 0.0) {
				if (select_des_zones && !des_zone_range.In_Range (col_id [i])) continue;

				if (node_data [i] == 0) {
					Warning ("Parking Node for Origin ") << row_id [z] << " Destination " << col_id [i] << " was Not Found";
					continue;
				}
				map_itr = parking_zone_map.find (node_data [i]);
				if (map_itr == parking_zone_map.end ()) {
					Warning ("Parking Node ") << node_data [i] << " is Not in the Parking Equiv";
					continue;
				}
				zone = map_itr->second;
				
				map_itr = org_map.find (zone);
				if (map_itr == org_map.end ()) {
					Warning ("Parking Zone ") << zone << " is Not a Trip Origin";
					continue;
				}
				walk_data [map_itr->second] [i] += pnr_data [i];

				map_itr = des_map.find (zone);
				if (map_itr == des_map.end ()) {
					Warning ("Parking Zone ") << zone << " is Not a Trip Destination";
					continue;
				}
				drive_data [z] [map_itr->second] += pnr_data [i];
			}
		}
	}
	End_Progress ();
	MATRIX_Done (pnr_trip);
	MATRIX_Done (node_skim);

	count = 0;

	//---- write the output trip tables ----

	Show_Message ("Writing New Walk Trip Table -- Record");
	Set_Progress ();

	MATRIX_SetCore (new_trip, (short) 0);

	for (z=0; z < num_zones; z++) {
		Show_Progress ();

		MATRIX_SetBaseVector (new_trip, z, MATRIX_ROW, FLOAT_TYPE, walk_data [z]);
		count++;
	}
	End_Progress ();

	//---- write the output trip tables ----
	
	Show_Message ("Writing New Walk Trip Table -- Record");
	Set_Progress ();

	MATRIX_SetCore (new_trip, (short) 1);

	for (z=0; z < num_zones; z++) {
		Show_Progress ();

		MATRIX_SetBaseVector (new_trip, z, MATRIX_ROW, FLOAT_TYPE, drive_data [z]);
		count++;
	}
	End_Progress ();

	Write (1, "Number of New Trip Table Records ") << count;

	MATRIX_Done (new_trip);
	return;

read_error:
	Error (String ("Reading Zone=%d Status=%d") % z % tc_status);
}
