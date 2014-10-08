//*********************************************************
//	Read_Timing_Plans.cpp - read the timing plan file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Timing_Plans
//---------------------------------------------------------

void Data_Service::Read_Timing_Plans (Timing_File &file)
{
	int i, num, count;
	bool keep;

	Timing_Record timing_rec;
	Timing_Data *timing_ptr;
	Signal_Data *signal_ptr;

	//---- store the timing plan data ----

	Show_Message (String ("Reading %s -- Record") % file.File_Type ());
	Set_Progress ();
	
	Initialize_Timing_Plans (file);
	count = 0;

	timing_ptr = &(timing_rec.timing_data);

	while (file.Read (false)) {
		Show_Progress ();

		timing_rec.Clear ();

		keep = Get_Timing_Data (file, timing_rec);

		num = file.Num_Nest ();
		if (num > 0) timing_ptr->reserve (num);

		for (i=1; i <= num; i++) {
			if (!file.Read (true)) {
				Error (String ("Number of Phase Records for Signal %d Plan %d") % 
					file.Signal () % file.Timing ());
			}
			Show_Progress ();

			Get_Timing_Data (file, timing_rec);
		}

		//---- save the results ----

		if (keep) {
			if (file.Version () <= 40) {
				bool keep;
				Timing40_Map_Itr timing40_itr;
				Timing40_Data *timing40_ptr;
				Timing_Itr timing_itr;

				timing40_itr = timing40_map.find (timing_ptr->Timing ());

				if (timing40_itr == timing40_map.end ()) {
					Warning (String ("Timing Plan %d was Not Found in the Signal file") % timing_ptr->Timing ());
					continue;
				}
				timing40_ptr = (Timing40_Data *) &(timing40_itr->second);
				timing_rec.Signal (timing40_ptr->Signal ());

				signal_ptr = &signal_array [timing_rec.Signal ()];
				keep = true;

				for (timing_itr = signal_ptr->timing_plan.begin (); timing_itr != signal_ptr->timing_plan.end (); timing_itr++) {
					if (timing_itr->Timing () == timing40_ptr->Timing ()) {
						Timing_Phase_Itr phase_itr = timing_ptr->begin ();
						timing_itr->push_back (*phase_itr);
						count++;
						keep = false;
						break;
					}
				}
				if (!keep) continue;

				timing_ptr->Timing (timing40_ptr->Timing ());
				timing_ptr->Type (timing40_ptr->Type ());
				timing_ptr->Offset (timing40_ptr->Offset ());
			} else {
				signal_ptr = &signal_array [timing_rec.Signal ()];
			}
			signal_ptr->timing_plan.push_back (*timing_ptr);

			count += (int) timing_ptr->size () + 1;
		}
	}
	End_Progress ();
	file.Close ();

	Print (2, String ("Number of %s Records = %d") % file.File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file.File_ID () % count);
	}
	if (count > 0) System_Data_True (TIMING_PLAN);

	//---- repair Version 4.0 data ----

	if (file.Version () <= 40) {
		int first, prev, next, barrier, ring, position, cycle;
		bool flag;
		Signal_Itr signal_itr;
		Timing_Itr timing_itr;
		Timing_Phase_Itr phase_itr;

		for (signal_itr = signal_array.begin (); signal_itr != signal_array.end (); signal_itr++) {
			for (timing_itr = signal_itr->timing_plan.begin (); timing_itr != signal_itr->timing_plan.end (); timing_itr++) {

				//---- map the ring positions ----

				for (ring=1; ; ring++) {
					flag = false;

					for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
						if (phase_itr->Ring () == ring) {
							flag = true;
							break;
						}
					}
					if (!flag) break;
					first = prev = phase_itr->Phase ();
					next = phase_itr->Position ();

					barrier = phase_itr->Barrier ();
					if (barrier == 0) {
						phase_itr->Barrier ((barrier = 1));
					}
					count = (int) timing_itr->size ();
					position = 1;
					phase_itr->Position (position);

					cycle = MAX ((phase_itr->Min_Green () + phase_itr->Extension ()), phase_itr->Max_Green ()) +
							phase_itr->Yellow () + phase_itr->All_Red ();

					while (next != first && count-- > 0) {
						flag = false;

						for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
							if (phase_itr->Phase () == next) {
								flag = true;
								break;
							}
						}
						if (!flag) {
							Warning (String ("Signal %d Timing %d Phase %d was Not Found") % 
								signal_itr->Signal () % timing_itr->Timing () % next);
							break;
						}
						if (phase_itr->Ring () == 0) {
							phase_itr->Ring (ring);
						} else if (phase_itr->Ring () != ring) {
							Warning (String ("Signal %d Timing %d Phase %d to %d is Not in Ring %d") %
								signal_itr->Signal () % timing_itr->Timing () % prev % next % ring);
							break;
						}
						if (phase_itr->Barrier () == 0) {
							phase_itr->Barrier (barrier);
						}
						if (phase_itr->Barrier () == barrier) {
							position++;
						} else {
							position = 1;
							barrier = phase_itr->Barrier ();
						}
						prev = next;
						next = phase_itr->Position ();
						phase_itr->Position (position);
						cycle += MAX ((phase_itr->Min_Green () + phase_itr->Extension ()), phase_itr->Max_Green ()) +
								phase_itr->Yellow () + phase_itr->All_Red ();					
					}
					if (ring == 1) {
						timing_itr->Cycle (cycle);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Initialize_Timing_Plans
//---------------------------------------------------------

void Data_Service::Initialize_Timing_Plans (Timing_File &file)
{
	Required_File_Check (file, SIGNAL);
}

//---------------------------------------------------------
//	Get_Timing_Data
//---------------------------------------------------------

bool Data_Service::Get_Timing_Data (Timing_File &file, Timing_Record &timing_rec)
{
	int signal;
	Int_Map_Itr map_itr;

	Timing_Data *timing_ptr = &(timing_rec.timing_data);

	//---- process the header record ----
	
	if (!file.Nested ()) {
		timing_ptr->Timing (file.Timing ());
		if (timing_ptr->Timing () == 0) return (false);

		timing_ptr->Type (file.Type ());
		timing_ptr->Cycle (file.Cycle ());
		timing_ptr->Offset (file.Offset ());
		timing_ptr->Notes (file.Notes ());

		if (file.Version () > 40) {
			signal = file.Signal ();
			if (signal == 0) return (false);

			map_itr = signal_map.find (signal);
			if (map_itr == signal_map.end ()) {
				Warning (String ("Timing Signal %d was Not Found") % signal);
				return (false);
			}
			timing_rec.Signal (map_itr->second);
			return (true);
		}
	}
	Timing_Phase phase_rec;

	phase_rec.Phase (file.Phase ());
	phase_rec.Barrier (file.Barrier ());
	phase_rec.Ring (file.Ring ());
	phase_rec.Position (file.Position ());
	phase_rec.Min_Green (file.Min_Green ());
	phase_rec.Max_Green (file.Max_Green ());
	phase_rec.Extension (file.Extension ());
	phase_rec.Yellow (file.Yellow ());
	phase_rec.All_Red (file.All_Red ());

	if (phase_rec.Max_Green () == 0) phase_rec.Max_Green (phase_rec.Min_Green () + phase_rec.Extension ());

	timing_ptr->push_back (phase_rec);
	return (timing_rec.Signal () >= 0 || file.Version () <= 40);
}
