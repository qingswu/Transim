//*********************************************************
//	Process.cpp - group processing
//*********************************************************

#include "TourSkim.hpp"

//---------------------------------------------------------
//	Process
//---------------------------------------------------------

void TourSkim::Group_Process::Process (int index)
{
	int i, r, t, z, p, t1, num_zones, num_tabs, num_walk, len, node_tab, zone_tab, tab, count;
	char label [_MAX_FLABEL], **tab_names;
	TC_MATRIX skim, new_skim;
	MAT_INFO info;

	Integers tab_num;
	Strings new_tabs;
	Str_Itr str_itr;
	Int_Itr int_itr;
	Int_Map_Itr map_itr;
	Return_Itr return_itr;
	File_Group *group_ptr;

	group_ptr = &exe->file_group [index];

	if (thread_flag) {
		MAIN_LOCK
		exe->Show_Message (String ("Processing File Group #%d") % group_ptr->group);
		END_LOCK
	} else {
		exe->Show_Message (String ("Processing File Group #%d -- Record") % group_ptr->group);
		exe->Set_Progress ();
	}
	num_zones = exe->num_zones;

	//---- create the new matrix ----
	
	skim = group_ptr->pnr_skim;

	MATRIX_GetInfo (skim, &info);

	MATRIX_GetIDs (skim, MATRIX_ROW, row_id);
	MATRIX_GetIDs (skim, MATRIX_COL, col_id);

	new_tabs.clear ();

	for (str_itr = exe->pnr_table.begin (); str_itr != exe->pnr_table.end (); str_itr++) {
		new_tabs.push_back (*str_itr);
	}
	zone_tab = (int) new_tabs.size ();
	new_tabs.push_back ("PNR_ZONE");

	for (int_itr = group_ptr->periods.begin (); int_itr != group_ptr->periods.end (); int_itr++) {
		for (r=0, return_itr = exe->return_array.begin (); return_itr != exe->return_array.end (); return_itr++, r++) {
			if (*int_itr == return_itr->group) {
				*int_itr = r;
				break;
			}
		}
		for (str_itr = exe->pnr_table.begin (); str_itr != exe->pnr_table.end (); str_itr++) {
			new_tabs.push_back (return_itr->prefix + " " + *str_itr);
		}
	}
	num_tabs = (int) new_tabs.size ();
	tab_names = new char * [num_tabs];

	for (t=0, str_itr = new_tabs.begin (); str_itr != new_tabs.end (); str_itr++, t++) {
		len = (int) str_itr->size () + 1;
		tab_names [t] = new char [len];
		strcpy_s (tab_names [t], len, str_itr->c_str ());
	}
	new_skim = MATRIX_New ((char *) group_ptr->new_name.c_str (), info.szLabel,	num_zones, row_id, 
		num_zones, col_id, num_tabs, tab_names, info.Type, (short) info.Compression);
	
	for (t=0, str_itr = new_tabs.begin (); str_itr != new_tabs.end (); str_itr++, t++) {
		delete [] tab_names [t];
	}

	//---- map the table names to numbers ----

	num_tabs = MATRIX_GetNCores (skim);

	tab_num.assign (exe->pnr_table.size (), -1);
		
	for (i=0; i < num_tabs; i++) {
		MATRIX_GetLabel (skim, (short) i, label);

		for (t=0, str_itr = exe->pnr_table.begin (); str_itr != exe->pnr_table.end (); str_itr++, t++) {
			if (tab_num [t] < 0) {
				if (str_itr->Equals (label)) {
					tab_num [t] = i;
					break;
				}
			}
		}
	}
	for (t=0, int_itr = tab_num.begin (); int_itr != tab_num.end (); int_itr++, t++) {
		if (*int_itr == -1) {
			MAIN_LOCK
			exe->Warning ("PNR Table \"") << exe->pnr_table [t] << "\" was Not Found";
			END_LOCK
		}
	}

	//---- copy input data to the output file ----

	count = 0;

	for (t=0, int_itr = tab_num.begin (); int_itr != tab_num.end (); int_itr++, t++) {
		if (*int_itr >= 0) {
			MATRIX_SetCore (skim, (short) *int_itr);
			MATRIX_SetCore (new_skim, (short) t);

			for (z=0; z < num_zones; z++) {
				if (thread_flag) {
					exe->Show_Dot ();
				} else {
					exe->Show_Progress ();
				}
				MATRIX_GetBaseVector (skim, z, MATRIX_ROW, FLOAT_TYPE, pnr_data);

				if (tc_status != TC_OKAY) goto read_error;

				MATRIX_SetBaseVector (new_skim, z, MATRIX_ROW, FLOAT_TYPE, pnr_data);
				count++;
			}
		}
	}
	MATRIX_Done (skim);

	//---- locate the parking table ----

	skim = group_ptr->node_skim;

	num_tabs = MATRIX_GetNCores (skim);
	node_tab = -1;
		
	for (i=0; i < num_tabs; i++) {
		MATRIX_GetLabel (skim, (short) i, label);
		if (exe->node_table.Equals (label)) {
			node_tab = i;
			break;
		}
	}
	if (node_tab < 0) {
		MAIN_LOCK
		exe->Warning ("Parking Table \"") << exe->node_table << "\" was Not Found";
		END_LOCK
		MATRIX_Done (new_skim);
		return;
	}
	num_tabs = (int) exe->pnr_table.size ();
	num_walk = (int) exe->walk_table.size ();

	//---- read and convert the parking nodes ----

	MATRIX_SetCore (skim, (short) node_tab);

	for (z=0; z < num_zones; z++) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}
		MATRIX_GetBaseVector (skim, z, MATRIX_ROW, LONG_TYPE, row_id);

		if (tc_status != TC_OKAY) goto read_error;

		for (i=0; i < num_zones; i++) {
			if (row_id [i] > 0) {
				map_itr = exe->parking_zone_map.find (row_id [i]);
				if (map_itr == exe->parking_zone_map.end ()) {
					MAIN_LOCK
					exe->Warning ("Parking Node ") << row_id [i] << " is Not in the Parking Equiv";
					END_LOCK
					col_id [i] = row_id [i] = -1;
				} else {
					col_id [i] = map_itr->second;
					map_itr = exe->parking_id_map.find (row_id [i]);
					row_id [i] = map_itr->second;
				}
				pnr_data [i] = (float) col_id [i];
			} else {
				pnr_data [i] = flt_miss;
				row_id [i] = -1;
			}
		}
		MATRIX_SetCore (new_skim, (short) zone_tab);
		MATRIX_SetBaseVector (new_skim, z, MATRIX_ROW, FLOAT_TYPE, pnr_data);
		count++;

		//---- create the return leg data for each time period ----

		tab = zone_tab;

		for (int_itr = group_ptr->periods.begin (); int_itr != group_ptr->periods.end (); int_itr++) {
			r = *int_itr;

			for (t=0; t < num_tabs; t++) {
				t1 = exe->return_map [t];

				if (thread_flag) {
					exe->Show_Dot ();
				} else {
					exe->Show_Progress ();
				}
				for (i=0; i < num_zones; i++) {
					if (row_id [i] >= 0) {
						if (t1 >= 0) {
							p = col_id [i] - 1;
							pnr_data [i] = exe->walk_data [r][t1][i][p];

							//---- temp ----
							if (t == 1) ttime [i] = pnr_data [i];

						} else if (t1 == -2) {
							p = row_id [i];
							pnr_data [i] = exe->drive_data [r][p][z];
						} else if (t1 == -1) {
							//---- temp ----

							p = row_id [i];
							pnr_data [i] = exe->drive_data [r][p][z] / ttime [i];
						}
						//if (t1 < num_walk) {
						//	p = col_id [i] - 1;
						//	pnr_data [i] = exe->walk_data [r][t1][i][p];
						//	if (t == 1) ttime [i] = pnr_data [i];
						//} else if (ttime [i] > 0) {
						//	p = row_id [i];
						//	pnr_data [i] = exe->drive_data [r][p][z] / ttime [i];
						//} else {
						//	pnr_data [i] = ttime [i];
						//}
					} else {
						pnr_data [i] = flt_miss;
					}
				}
				MATRIX_SetCore (new_skim, (short) ++tab);
				MATRIX_SetBaseVector (new_skim, z, MATRIX_ROW, FLOAT_TYPE, pnr_data);
				count++;
			}
		}
	}
	if (!thread_flag) exe->End_Progress ();
	MATRIX_Done (skim);
	MATRIX_Done (new_skim);

	MAIN_LOCK
	exe->Print (2, String ("Number of New Drive to Transit #%d Records = %d") % group_ptr->group % count);
	END_LOCK
	return;

read_error:
	MAIN_LOCK
	exe->Error (String ("Reading Zone=%d Status=%d") % z % tc_status);
	END_LOCK
}
