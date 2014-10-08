//*********************************************************
//	Get_Timing_Data.cpp - read the timing plan file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Initialize_Timings
//---------------------------------------------------------

void ArcNet::Initialize_Timing_Plans (Timing_File &file)
{
	Data_Service::Initialize_Timing_Plans (file);

	if (timing_db.capacity () == 0) {
		int num = file.Num_Records ();

		if (num > 0) timing_db.reserve (num);
	}
}

//---------------------------------------------------------
//	Get_Timing_Data
//---------------------------------------------------------

bool ArcNet::Get_Timing_Data (Timing_File &file, Timing_Record &timing_rec)
{
	if (Data_Service::Get_Timing_Data (file, timing_rec)) {
		Timing_Data *timing_ptr = &(timing_rec.timing_data);
		timing_db.Copy_Fields (file);

		if (!file.Nested ()) {
			if (time_flag) {
				Signal_Data *signal_ptr = &signal_array [timing_rec.Signal ()];

				Signal_Time_Itr itr;
				bool keep = false;

				for (itr = signal_ptr->begin (); itr != signal_ptr->end (); itr++) {
					if (itr->Timing () == timing_ptr->Timing () && 
						itr->Start () <= time && time <= itr->End ()) {
						keep = true;
						break;
					}
				}
				if (!keep) {
					timing_ptr->Timing (-1);
					return (false);
				}
			}
			return (timing_rec.Signal () >= 0);
		}
		if (timing_ptr->Timing () >= 0) {
			if (!timing_db.Add_Record ()) {
				Error ("Writing Timing Plan Database");
			}
			return (timing_rec.Signal () >= 0);
		}
	}
	return (false);
}
