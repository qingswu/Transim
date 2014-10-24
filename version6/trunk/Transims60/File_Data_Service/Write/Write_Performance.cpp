//*********************************************************
//	Write_Performance.cpp - write a new performance file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Performance
//---------------------------------------------------------

void Data_Service::Write_Performance (bool fill_flag)
{
	Performance_File *file = System_Performance_File (true);

	Write_Performance (*file, perf_period_array, fill_flag);
}

void Data_Service::Write_Performance (Performance_File &file, Perf_Period_Array &array, bool fill_flag)
{
	int i, index, dir, count;
	Dtime low, high;

	Int_Map_Itr itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Perf_Data *perf_ptr;
	Perf_Period_Itr perf_period_itr;
	Performance_Data perf_rec;

	Show_Message (String ("Writing %s -- Record") % file.File_Type ());
	Set_Progress ();

	count = 0;

	//---- process each time period ----

	for (i=0, perf_period_itr = array.begin (); perf_period_itr != array.end (); perf_period_itr++, i++) {

		array.periods->Period_Range (i, low, high);

		perf_rec.Start (low);
		perf_rec.End (high);

		//---- sort the links ----

		for (itr = link_map.begin (); itr != link_map.end (); itr++) {
			link_ptr = &link_array [itr->second];

			for (dir=0; dir < 2; dir++) {
				index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
				if (index < 0) continue;

				Show_Progress ();

				perf_ptr = perf_period_itr->Data_Ptr (index);
				if (!fill_flag && !perf_ptr->Output_Check ()) continue;

				dir_ptr = &dir_array [index];

				perf_rec.Dir_Index (index);
				perf_rec.Type (0);

				perf_rec.Get_Data (perf_ptr, dir_ptr, link_ptr);

				count += Put_Performance_Data (file, perf_rec);

				//---- lane use flows ----

				if (dir_ptr->Use_Index () >= 0) {
					perf_ptr = perf_period_itr->Data_Ptr (dir_ptr->Use_Index ());
					if (!fill_flag && !perf_ptr->Output_Check ()) continue;

					perf_rec.Type (1);

					perf_rec.Get_Data (perf_ptr, dir_ptr, link_ptr);

					count += Put_Performance_Data (file, perf_rec);
				}
			}
		}
	}
	Show_Progress (count);
	End_Progress ();
	file.Close ();
	
	Print (2, String ("%s Records = %d") % file.File_Type () % count);
}

//---------------------------------------------------------
//	Put_Performance_Data
//---------------------------------------------------------

int Data_Service::Put_Performance_Data (Performance_File &file, Performance_Data &data)
{
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	dir_ptr = &dir_array [data.Dir_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	//---- save the link record ----

	file.Link (link_ptr->Link ());
	file.Dir (dir_ptr->Dir ());

	file.Type (data.Type ());

	file.Start (data.Start ());
	file.End (data.End ());

	file.Time (data.Time ());
	file.Persons (data.Persons ());
	file.Volume (data.Volume ());
	file.Enter (data.Enter ());
	file.Exit (data.Exit ());
	file.Flow (data.Flow ());
	file.Speed (data.Speed ());
	file.Time_Ratio (data.Time_Ratio ());
	file.Delay (data.Delay ());
	file.Density (data.Density ());
	file.Max_Density (data.Max_Density ());
	file.Queue (data.Queue ());
	file.Max_Queue (data.Max_Queue ());
	file.Failure (data.Failure ());
	file.Veh_Dist (data.Veh_Dist ());
	file.Veh_Time (data.Veh_Time ());
	file.Veh_Delay (data.Veh_Delay ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}

//---------------------------------------------------------
//	Write_Performance
//---------------------------------------------------------

void Data_Service::Write_Performance (Vol_Spd_Period_Array &array, bool fill_flag)
{
	int i, index, dir, count;
	Dtime low, high;

	Int_Map_Itr itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Vol_Spd_Data *vol_spd_ptr;
	Vol_Spd_Period_Itr period_itr;
	Performance_Data perf_rec;
	
	Performance_File *file = System_Performance_File (true);

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	count = 0;

	//---- process each time period ----

	for (i=0, period_itr = array.begin (); period_itr != array.end (); period_itr++, i++) {

		array.periods->Period_Range (i, low, high);

		perf_rec.Start (low);
		perf_rec.End (high);

		//---- sort the links ----

		for (itr = link_map.begin (); itr != link_map.end (); itr++) {
			link_ptr = &link_array [itr->second];

			for (dir=0; dir < 2; dir++) {
				index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
				if (index < 0) continue;

				Show_Progress ();

				vol_spd_ptr = period_itr->Data_Ptr (index);
				if (!fill_flag && vol_spd_ptr->Volume () == 0) continue;

				dir_ptr = &dir_array [index];

				perf_rec.Dir_Index (index);
				perf_rec.Type (0);

				perf_rec.Get_Data (vol_spd_ptr, dir_ptr, link_ptr);

				count += Put_Performance_Data (*file, perf_rec);

				//---- lane use flows ----

				if (dir_ptr->Use_Index () >= 0) {
					vol_spd_ptr = period_itr->Data_Ptr (dir_ptr->Use_Index ());
					if (!fill_flag && vol_spd_ptr->Volume () == 0) continue;

					perf_rec.Type (1);

					perf_rec.Get_Data (vol_spd_ptr, dir_ptr, link_ptr);

					count += Put_Performance_Data (*file, perf_rec);
				}
			}
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}
