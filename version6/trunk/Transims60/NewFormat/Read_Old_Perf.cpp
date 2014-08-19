//*********************************************************
//	Read_Old_Perf.cpp - read the performance file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Read_Old_Perf
//---------------------------------------------------------

void NewFormat::Read_Old_Perf (void)
{
	int i, num, link, dir, dir_index, link_count, turn_count, node;
	bool keep_flag, turn_flag, delay_flag;
	Dtime period;

	Performance_File *perf_file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);
	Turn_Delay_File *turn_file = (Turn_Delay_File *) System_File_Handle (NEW_TURN_DELAY);

	Link_Data *link_ptr;
	Int2_Map_Itr map2_itr;
	Int_Map_Itr map_itr;
	Performance_Data perf_rec;
	Turn_Delay_Data turn_rec;

	//---- store the performance data ----

	Show_Message (String ("Reading %s -- Record") % old_perf_file.File_Type ());
	Set_Progress ();

	Required_File_Check (old_perf_file, LINK);

	if (perf_period_array.size () == 0) {
		if (Sum_Flow_Flag ()) {
			perf_period_array.Initialize (&sum_periods);
		} else {
			perf_period_array.Initialize (&time_periods);
		}
	}
	if (old_perf_file.Turn_Flag () && System_File_Flag (NEW_TURN_DELAY) && !System_File_Flag (TURN_DELAY)) {
		if (Sum_Flow_Flag ()) {
			turn_period_array.Initialize (&sum_periods);
		} else {
			turn_period_array.Initialize (&time_periods);
		}
		turn_flag = true;
	} else {
		turn_flag = false;
	}

	link_count = turn_count = node = 0;
	delay_flag = old_perf_file.Link_Delay_Flag ();

	while (old_perf_file.Read (false)) {
		Show_Progress ();

		perf_rec.Clear ();

		keep_flag = false;

		//---- check/convert the link number and direction ----
		
		link = old_perf_file.Link ();
		dir = old_perf_file.Dir ();

		link_ptr = Set_Link_Direction (old_perf_file, link, dir, (old_perf_file.Version () <= 40));

		if (link_ptr == 0) goto turns;

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
			node = link_ptr->Anode ();
		} else {
			dir_index = link_ptr->AB_Dir ();
			node = link_ptr->Bnode ();
		}
		if (dir_index < 0) {
			Warning (String ("%s %d Link %d Direction %s was Not Found") % old_perf_file.File_ID () % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
			node = 0;
			goto turns;
		}
		perf_rec.Dir_Index (dir_index);
		perf_rec.Type (old_perf_file.Type ());

		//---- store the data ----

		perf_rec.End (old_perf_file.End ());
		perf_rec.Flow (old_perf_file.Flow ());

		if (old_perf_file.Version () <= 40 && old_perf_file.Dbase_Format () == VERSION3) {
			if (period == 0) {
				period = perf_rec.End ();
			}
			perf_rec.Start (perf_rec.End () - period);
			if (perf_rec.Flow () > 0) {
				perf_rec.Time (UnRound (old_perf_file.Time ()) / perf_rec.Flow ());
			} else {
				perf_rec.Time (old_perf_file.Time ());
			}
		} else {
			perf_rec.Start (old_perf_file.Start ());
			perf_rec.Time (old_perf_file.Time ());
		}
		perf_rec.Start (old_perf_file.Start ());
		perf_rec.End (old_perf_file.End ());
		perf_rec.Speed (old_perf_file.Speed ());
		perf_rec.Delay (old_perf_file.Delay ());
		perf_rec.Density (old_perf_file.Density ());
		perf_rec.Max_Density (old_perf_file.Max_Density ());
		perf_rec.Time_Ratio (old_perf_file.Time_Ratio ());
		perf_rec.Queue (old_perf_file.Queue ());
		perf_rec.Max_Queue (old_perf_file.Max_Queue ());
		perf_rec.Failure (old_perf_file.Cycle_Failure ());
		perf_rec.Persons (old_perf_file.Volume ());
		perf_rec.Volume (old_perf_file.Volume ());
		perf_rec.Max_Volume (old_perf_file.Volume ());
		perf_rec.Enter (old_perf_file.Enter ());
		perf_rec.Exit (old_perf_file.Exit ());
		perf_rec.Veh_Dist (Round (old_perf_file.VMT ()));
		perf_rec.Veh_Time (Round (old_perf_file.VHT ()));
		perf_rec.Veh_Delay (Round (old_perf_file.Volume () * old_perf_file.Delay ()));

		link_count += Put_Performance_Data (*perf_file, perf_rec);
		keep_flag = true;

turns:
		num = old_perf_file.Num_Nest ();

		for (i=1; i <= num; i++) {
			if (!old_perf_file.Read (true)) {
				Error (String ("Number of Nested Records for Performance %d") % old_perf_file.Link ());
			}
			Show_Progress ();

			if (!turn_flag || !keep_flag) continue;

			//---- convert the to-link number ----

			link = old_perf_file.Out_Link ();

			map_itr = link_map.find (link);
			if (map_itr == link_map.end ()) {
				Warning (String ("%s %d Link %d was Not Found") % old_perf_file.File_ID () % Progress_Count () % 
					link);
				continue;
			}
			link = map_itr->second;

			link_ptr = &link_array [link];

			if (link_ptr->Anode () == node) {
				dir_index = link_ptr->AB_Dir ();
				dir = 0;
			} else if (link_ptr->Bnode () == node) {
				dir_index = link_ptr->BA_Dir ();
				dir = 1;
			} else {
				Node_Data *ptr = &node_array [node];
				Warning (String ("%s %d Node %d is Not on Link %d") % old_perf_file.File_ID () % Progress_Count () % 
					ptr->Node () % link_ptr->Link ());
				continue;
			}
			if (dir_index < 0) {
				Warning (String ("%s %d Link %d Direction %s was Not Found") % old_perf_file.File_ID () % Progress_Count () % 
					link_ptr->Link () % ((dir) ? "BA" : "AB"));
				continue;
			}
			turn_rec.To_Index (dir_index);
			turn_rec.Turn (old_perf_file.Out_Flow ());
			turn_rec.Time (old_perf_file.Out_Time ());

			turn_count += Put_Turn_Delay_Data (*turn_file, turn_rec);
		}
	}
	End_Progress ();
	old_perf_file.Close ();
	perf_file->Close ();
	if (turn_flag) {
		turn_file->Close ();
	}
	Break_Check (5);
	Print (2, String ("Number of %s Records = %d") % old_perf_file.File_Type () % Progress_Count ());

	num = link_count + turn_count;

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % old_perf_file.File_ID () % num);
	}
	Print (1, "Number of Link Direction Records = ") << link_count;
	Print (1, "Number of Link Connection Records = ") << turn_count;
}
