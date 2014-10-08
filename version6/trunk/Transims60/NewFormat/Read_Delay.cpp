//*********************************************************
//	Read_Delay.cpp - read the link delay / performance file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Read_Delay
//---------------------------------------------------------

void NewFormat::Read_Delay (void)
{
	int i, num, link, dir, dir_index, link_count, turn_count, node;
	bool keep_flag, turn_flag, delay_flag;
	double length, speed;
	Dtime period;

	Performance_File *perf_file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);
	Turn_Delay_File *turn_file = (Turn_Delay_File *) System_File_Handle (NEW_TURN_DELAY);

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int2_Map_Itr map2_itr;
	Int_Map_Itr map_itr;
	Performance_Data perf_rec;
	Turn_Delay_Data turn_rec;

	//---- store the performance data ----

	Show_Message (String ("Reading %s -- Record") % old_delay_file.File_Type ());
	Set_Progress ();

	Required_File_Check (old_delay_file, LINK);

	if (old_delay_file.Turn_Flag () && System_File_Flag (NEW_TURN_DELAY) && !System_File_Flag (TURN_DELAY)) {
		turn_flag = true;
	} else {
		turn_flag = false;
	}

	link_count = turn_count = node = dir_index = 0;
	delay_flag = old_delay_file.Link_Delay_Flag ();

	while (old_delay_file.Read (false)) {
		Show_Progress ();

		perf_rec.Clear ();

		keep_flag = false;

		//---- check/convert the link number and direction ----
		
		link = old_delay_file.Link ();
		dir = old_delay_file.Dir ();

		link_ptr = Set_Link_Direction (old_delay_file, link, dir, (old_delay_file.Version () <= 40));

		if (link_ptr == 0) goto turns;

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
			node = link_ptr->Anode ();
		} else {
			dir_index = link_ptr->AB_Dir ();
			node = link_ptr->Bnode ();
		}
		if (dir_index < 0) {
			Warning (String ("%s %d Link %d Direction %s was Not Found") % old_delay_file.File_ID () % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
			node = 0;
			goto turns;
		}
		dir_ptr = &dir_array [dir_index];

		perf_rec.Dir_Index (dir_index);
		perf_rec.Type (old_delay_file.Type ());

		//---- store the data ----

		perf_rec.End (old_delay_file.End ());
		perf_rec.Flow (old_delay_file.Flow ());

		if (old_delay_file.Version () <= 40 && old_delay_file.Dbase_Format () == VERSION3) {
			if (period == 0) {
				period = perf_rec.End ();
			}
			perf_rec.Start (perf_rec.End () - period);
			if (perf_rec.Flow () > 0) {
				perf_rec.Time (UnRound (old_delay_file.Time ()) / perf_rec.Flow ());
			} else {
				perf_rec.Time (old_delay_file.Time ());
			}
		} else {
			perf_rec.Start (old_delay_file.Start ());
			perf_rec.Time (old_delay_file.Time ());
		}
		perf_rec.Start (old_delay_file.Start ());
		perf_rec.End (old_delay_file.End ());

		if (old_delay_file.Link_Delay_Flag ()) {
			if (perf_rec.Time () > 0) {
				speed = (double) link_ptr->Length () / perf_rec.Time ();
			} else {
				speed = (double) link_ptr->Length () / dir_ptr->Time0 ();
			}
			perf_rec.Speed (speed);
			perf_rec.Delay (perf_rec.Time () - dir_ptr->Time0 ());

			length = link_ptr->Length ();
			length = External_Units (length, ((Metric_Flag ()) ? KILOMETERS : MILES)) * dir_ptr->Lanes ();
			if (length < 1) length = 1.0;

			perf_rec.Density (perf_rec.Flow () / length);
			perf_rec.Max_Density (perf_rec.Density ());

			perf_rec.Time_Ratio (Round (perf_rec.Time () * 100.0 / dir_ptr->Time0 ()));
			perf_rec.Queue (0);
			perf_rec.Max_Queue (0);
			perf_rec.Failure (0);

			perf_rec.Persons (perf_rec.Flow ());
			perf_rec.Volume (perf_rec.Flow ());
			perf_rec.Enter (perf_rec.Flow ());
			perf_rec.Exit (perf_rec.Flow ());
			perf_rec.Veh_Dist (perf_rec.Flow () * UnRound (link_ptr->Length ()));
			perf_rec.Veh_Time (perf_rec.Flow () * perf_rec.Time ());
			perf_rec.Veh_Delay (perf_rec.Flow () * perf_rec.Delay ());			
		} else {
			perf_rec.Speed (old_delay_file.Speed ());
			perf_rec.Delay (old_delay_file.Delay ());
			perf_rec.Density (old_delay_file.Density ());
			perf_rec.Max_Density (old_delay_file.Max_Density ());
			perf_rec.Time_Ratio (old_delay_file.Time_Ratio ());
			perf_rec.Queue (old_delay_file.Queue ());
			perf_rec.Max_Queue (old_delay_file.Max_Queue ());
			perf_rec.Failure (old_delay_file.Cycle_Failure ());
			perf_rec.Persons (old_delay_file.Volume ());
			perf_rec.Volume (old_delay_file.Volume ());
			perf_rec.Enter (old_delay_file.Enter ());
			perf_rec.Exit (old_delay_file.Exit ());
			perf_rec.Veh_Dist (old_delay_file.Flow_Dist ());
			perf_rec.Veh_Time (Round (old_delay_file.Flow_Time ()));
			perf_rec.Veh_Delay (perf_rec.Volume () * perf_rec.Delay ());
		}
		link_count += Put_Performance_Data (*perf_file, perf_rec);
		keep_flag = true;

turns:
		num = old_delay_file.Num_Nest ();
		turn_rec.Dir_Index (dir_index);

		for (i=1; i <= num; i++) {
			if (!old_delay_file.Read (true)) {
				Error (String ("Number of Nested Records for Performance %d") % old_delay_file.Link ());
			}
			Show_Progress ();

			if (!turn_flag || !keep_flag) continue;

			//---- convert the to-link number ----

			link = old_delay_file.Out_Link ();

			map_itr = link_map.find (link);
			if (map_itr == link_map.end ()) {
				Warning (String ("%s %d Link %d was Not Found") % old_delay_file.File_ID () % Progress_Count () % 
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
				Warning (String ("%s %d Node %d is Not on Link %d") % old_delay_file.File_ID () % Progress_Count () % 
					ptr->Node () % link_ptr->Link ());
				continue;
			}
			if (dir_index < 0) {
				Warning (String ("%s %d Link %d Direction %s was Not Found") % old_delay_file.File_ID () % Progress_Count () % 
					link_ptr->Link () % ((dir) ? "BA" : "AB"));
				continue;
			}
			turn_rec.To_Index (dir_index);
			turn_rec.Start (perf_rec.Start ());
			turn_rec.End (perf_rec.End ());
			turn_rec.Turn (old_delay_file.Out_Flow ());
			turn_rec.Time (old_delay_file.Out_Time ());

			turn_count += Put_Turn_Delay_Data (*turn_file, turn_rec);
		}
	}
	End_Progress ();
	old_delay_file.Close ();
	perf_file->Close ();
	if (turn_flag) {
		turn_file->Close ();
	}
	Break_Check (5);
	Print (2, String ("Number of %s Records = %d") % old_delay_file.File_Type () % Progress_Count ());

	num = link_count + turn_count;

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % old_delay_file.File_ID () % num);
	}
	Print (1, "Number of Link Direction Records = ") << link_count;
	Print (1, "Number of Link Connection Records = ") << turn_count;
}
