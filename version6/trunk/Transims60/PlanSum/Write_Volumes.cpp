//*********************************************************
//	Write_Volumes.cpp - Write the Link Volume File
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Volume
//---------------------------------------------------------

void PlanSum::Write_Volumes (void)
{
	int i, j, nrec, index, period, p;
	Dtime low, high;
	bool save, ab_flag;	
	double flow;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	Perf_Period_Itr period_itr;
	Perf_Data perf_data;

	nrec = 0;
	flow = 0.0;

	Show_Message (String ("Writing %s -- Record") % volume_file.File_Type ());
	Set_Progress ();

	//---- process each link in sorted order ----

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		save = false;
		volume_file.Zero_Fields ();

		//---- process each direction ----

		for (i=0; i < 2; i++) {

			if (i) {
				index = link_ptr->BA_Dir ();
				ab_flag = false;
			} else {
				index = link_ptr->AB_Dir ();
				ab_flag = true;
			}
			if (index < 0) continue;

			period = -1;
			flow = 0;

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {
				perf_data = period_itr->Total_Performance (index);

				perf_period_array.periods->Period_Range (j, low, high);
				
				p = volume_file.Period (low, high);

				if (p != period) {
					if (period >= 0) {
						if (flow != 0.0) {
							if (ab_flag) {
								volume_file.Data_AB (period, flow);
							} else {
								volume_file.Data_BA (period, flow);
							}
							save = true;
						}
					}
					flow = perf_data.Volume ();
					period = p;
				} else {
					flow += perf_data.Volume ();
				}
			}
			if (period >= 0) {
				if (flow != 0.0) {
					if (ab_flag) {
						volume_file.Data_AB (period, flow);
					} else {
						volume_file.Data_BA (period, flow);
					}
					save = true;
				}
			}
		}

		//---- save non-zero records ----

		if (save) {
			nrec++;

			volume_file.Link (link_ptr->Link ());
			volume_file.Anode (node_array [link_ptr->Anode ()].Node ());
			volume_file.Bnode (node_array [link_ptr->Bnode ()].Node ());

			if (!volume_file.Write ()) {
				Error (String ("Writing %s") % volume_file.File_Type ());
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % volume_file.File_Type () % nrec);
	Print (1, "Number of Time Periods = ") << volume_file.Num_Periods ();

	volume_file.Close ();
}
