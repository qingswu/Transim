//*********************************************************
//	Load_Transit_Data - load transit data to links
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Load_Transit_Data
//---------------------------------------------------------

void PerfPrep::Load_Transit_Data (void)
{
	int offset, run, runs;
	Dtime time;
	double volume, persons, len_fac;
	bool first;

	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Driver_Itr driver_itr;
	Stop_Data *stop_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Veh_Type_Data *veh_type_ptr, *run_type_ptr;
	Perf_Data *perf_ptr;
	Perf_Period *period_ptr;

	Show_Message (String ("Loading Transit Data -- Record"));
	Set_Progress ();

	merge_perf_array.Zero_Flows ();

	veh_type_ptr = 0;
	volume = persons = 1.0;

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		Show_Progress ();

		stop_itr = line_itr->begin ();
		if (stop_itr == line_itr->end ()) continue;

		if (transit_data == PCE) {
			veh_type_ptr = &veh_type_array [line_itr->Type ()];
		}

		runs = (int) stop_itr->size ();

		for (run=0; run < runs; run++) {
			driver_itr = line_itr->driver_array.begin ();
			if (driver_itr == line_itr->driver_array.end ()) continue;

			first = true;
			offset = 0;

			for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
				time = stop_itr->at (run).Schedule ();

				if (line_itr->run_types.size () > 0) {
					run_type_ptr = &veh_type_array [line_itr->Run_Type (run)];
					volume = UnRound (run_type_ptr->PCE ());
				} else {
					volume = UnRound (veh_type_ptr->PCE ());
				}
				persons = stop_itr->at (run).Load ();

				stop_ptr = &stop_array [stop_itr->Stop ()];

				for (; driver_itr != line_itr->driver_array.end (); driver_itr++) {
					period_ptr = merge_perf_array.Period_Ptr (time);

					if (period_ptr != 0) {
						perf_ptr = period_ptr->Data_Ptr (*driver_itr);
					} else {
						perf_ptr = 0;
					}
					dir_ptr = &dir_array [*driver_itr];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (stop_ptr->Link_Dir () == dir_ptr->Link_Dir ()) {
						if (!first && perf_ptr != 0) {
							len_fac = (double) (stop_ptr->Offset () - offset) / link_ptr->Length ();
							perf_ptr->Add_Volume (volume * len_fac);
							perf_ptr->Add_Persons (persons * len_fac);
						}
						break;
					} else {
						if (perf_ptr != 0) {
							len_fac = (double) (link_ptr->Length () - offset) / link_ptr->Length ();
							perf_ptr->Add_Volume (volume * len_fac);
							time += DTOI (len_fac * dir_ptr->Time0 ());
						}
						offset = 0;
					}
				}
				if (driver_itr == line_itr->driver_array.end ()) {
					Error (String ("Route %d Stops and Driver Links are Incompatible") % line_itr->Route ());
				}
				if (first) {
					first = false;
				}
				offset = stop_ptr->Offset ();
			}
		}
	}
	End_Progress ();
}
