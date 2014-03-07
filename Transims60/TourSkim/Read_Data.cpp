//*********************************************************
//	Read_Data.cpp - read processing
//*********************************************************

#include "TourSkim.hpp"

//---------------------------------------------------------
//	Read_Data
//---------------------------------------------------------

void TourSkim::Read_Process::Read_Data (int index)
{
	int i, r, t, z, p, tabs, drive_tab, count;
	char label [_MAX_FLABEL];
	TC_MATRIX skim;

	Str_Itr str_itr;

	Return_Data *return_ptr;
	Int_Itr int_itr;
	Integers tab_num;

	return_ptr = &exe->return_array [index];
	r = index;
	count = z = 0;

	if (thread_flag) {
		MAIN_LOCK
		exe->Show_Message (String ("Reading Return Group #%d") % return_ptr->group);
		END_LOCK
	} else {
		exe->Show_Message (String ("Reading Return Group #%d -- Record") % return_ptr->group);
		exe->Set_Progress ();
	}

	//---- read the walk skim ----

	skim = return_ptr->walk_skim;
	tabs = MATRIX_GetNCores (skim);

	tab_num.assign (exe->num_tabs, -1);

	for (i=0; i < tabs; i++) {
		MATRIX_GetLabel (skim, (short) i, label);

		for (t=0, str_itr = exe->walk_table.begin (); str_itr != exe->walk_table.end (); str_itr++, t++) {
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
			exe->Warning ("Walk Table \"") << exe->walk_table [t] << "\" was Not Found";
		}
	}

	//---- read the table data into memory ----

	for (t=0, int_itr = tab_num.begin (); int_itr != tab_num.end (); int_itr++, t++) {
		if (*int_itr >= 0) {
			MATRIX_SetCore (skim, (short) *int_itr);

			for (z=0; z < exe->num_zones; z++) {
				if (thread_flag) {
					exe->Show_Dot ();
				} else {
					exe->Show_Progress ();
				}
				MATRIX_GetBaseVector (skim, z, MATRIX_ROW, FLOAT_TYPE, exe->walk_data [r][t][z]);

				if (!thread_flag && tc_status != TC_OKAY) {
					exe->Write (1, "Reading z=") << z << " stat=" << tc_status;
				}
				count++;
			}
		}
	}
	MATRIX_Done (skim);

	//---- drive skims ----

	skim = return_ptr->drive_skim;
	tabs = MATRIX_GetNCores (skim);

	drive_tab = -1;

	for (i=0; i < tabs; i++) {
		MATRIX_GetLabel (skim, (short) i, label);

		if (exe->drive_table.Equals (label)) {
			drive_tab = i;
			break;
		}
	}
	if (drive_tab == -1) {
		exe->Warning ("Drive Table \"") << exe->drive_table << "\" was Not Found";
	}

	//---- read the table data into memory ----

	if (drive_tab >= 0) {
		MATRIX_SetCore (skim, (short) drive_tab);

		for (p=0; p < exe->num_pnr; p++) {
			if (thread_flag) {
				exe->Show_Dot ();
			} else {
				exe->Show_Progress ();
			}
			MATRIX_GetBaseVector (skim, p, MATRIX_ROW, FLOAT_TYPE, exe->drive_data [r][p]);

			if (!thread_flag && tc_status != TC_OKAY) {
				exe->Write (1, "Reading p=") << p << " stat=" << tc_status;
			}
			count++;
		}
	}
	if (!thread_flag) exe->End_Progress ();
	MATRIX_Done (skim);

	MAIN_LOCK
	exe->Print (2, String ("Number of Return Group #%d Records = %d") % return_ptr->group % count);
	END_LOCK
}
