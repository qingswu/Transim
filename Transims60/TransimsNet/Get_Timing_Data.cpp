//*********************************************************
//	Get_Timing_Data.cpp - read the timing plan file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Timing_Data
//---------------------------------------------------------

bool TransimsNet::Get_Timing_Data (Timing_File &file, Timing_Record &timing_rec)
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
