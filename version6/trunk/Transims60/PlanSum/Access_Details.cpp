//*********************************************************
//	Access_Details.cpp - stop group access details
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Access_Details_Report
//---------------------------------------------------------

void PlanSum::Access_Detail_Report (void)
{
	int total, last_stop, last_line, last_group, last_period;
    String label;

	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Zone_Data *zone_ptr;
	Location_Data *loc_ptr;
	Int_Map_Itr int_itr;
	Xfer_IO_Map_Itr map_itr;
	Xfer_IO xfer_io;

	Show_Message ("Stop Group Access Details -- Record");
	Set_Progress ();

	Header_Number (ACCESS_DETAILS);
	New_Page ();

	last_stop = last_line = last_group = last_period = total = 0;

	for (map_itr = access_detail.begin (); map_itr != access_detail.end (); map_itr++) {
		Show_Progress ();
		xfer_io = map_itr->first;

		if (last_group != xfer_io.group || last_period != xfer_io.period) {
			if (last_group > 0) {
				Print (2, String ("     Total                 %8d") % total);
				Print (1);
			}
			last_group = xfer_io.group;
			last_period = xfer_io.period;
			last_stop = last_line = total = 0;

			label ("%d %s") % xfer_io.group % stop_equiv.Group_Label (xfer_io.group);

			if (sum_periods.Num_Periods () > 1) {
				label += "  Period=" + sum_periods.Range_Format (xfer_io.period);
			}
			Print (1, label);
		}

		//---- check for a new subgroup ----

		if (last_stop != xfer_io.from_stop || last_line != xfer_io.from_line) {
			last_stop = xfer_io.from_stop;
			last_line = xfer_io.from_line;

			if (last_line == 0) {
				Print (2, String ("     From %7d %8s") % last_stop % ((zone_file_flag) ? "Zone " : "Location"));

				if (Notes_Name_Flag ()) {
					Print (0, "           ");

					if (zone_file_flag) {
						int_itr = zone_map.find (last_stop);
						if (int_itr != zone_map.end ()) {
							zone_ptr = &zone_array [int_itr->second];

							Print (0, String ("%s") % zone_ptr->Notes ());
						}
					} else {
						int_itr = location_map.find (last_stop);
						if (int_itr != location_map.end ()) {
							loc_ptr = &location_array [int_itr->second];

							Print (0, String ("%s") % loc_ptr->Notes ());
						}
					}
				}
			} else {
				Print (2, String ("     From %7d %7d") % last_stop % last_line);

				if (Notes_Name_Flag ()) {
					Print (0, "            ");

					int_itr = line_map.find (last_line);
					if (int_itr != line_map.end ()) {
						line_ptr = &line_array [int_itr->second];

						Print (0, String ("%s") % line_ptr->Name ());
					}
					int_itr = stop_map.find (last_stop);
					if (int_itr != stop_map.end ()) {
						stop_ptr = &stop_array [int_itr->second];
						Print (0, " at ") << stop_ptr->Name ();

						if (!stop_ptr->Notes ().empty ()) {
							Print (0, " -- ") << stop_ptr->Notes ();
						}
					}
				}
			}
		}
		if (xfer_io.to_line == 0) {
			Print (1, String ("       To %7d %8s %8d  ") % xfer_io.to_stop % 
				((zone_file_flag) ? "Zone " : "Location") % map_itr->second);
			total += map_itr->second;

			if (Notes_Name_Flag ()) {
				if (zone_file_flag) {
					int_itr = zone_map.find (xfer_io.to_stop);
					if (int_itr != zone_map.end ()) {
						zone_ptr = &zone_array [int_itr->second];

						Print (0, String ("%s") % zone_ptr->Notes ());
					}
				} else {
					int_itr = location_map.find (xfer_io.to_stop);
					if (int_itr != location_map.end ()) {
						loc_ptr = &location_array [int_itr->second];

						Print (0, String ("%s") % loc_ptr->Notes ());
					}
				}
			}
		} else {
			Print (1, String ("       To %7d %7d  %8d  ") % xfer_io.to_stop % xfer_io.to_line % map_itr->second);
			total += map_itr->second;

			if (Notes_Name_Flag ()) {
				int_itr = line_map.find (xfer_io.to_line);
				if (int_itr != line_map.end ()) {
					line_ptr = &line_array [int_itr->second];

					Print (0, String ("%s") % line_ptr->Name ());
				}
				int_itr = stop_map.find (xfer_io.to_stop);
				if (int_itr != stop_map.end ()) {
					stop_ptr = &stop_array [int_itr->second];
					Print (0, " at ") << stop_ptr->Name ();
					if (!stop_ptr->Notes ().empty ()) {
						Print (0, " -- ") << stop_ptr->Notes ();
					}
				}
			}
		}
	}
	if (last_group > 0) {
		Print (2, String ("     Total                 %8d") % total);
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Access_Details_Header
//---------------------------------------------------------

void PlanSum::Access_Detail_Header (void)
{
	Print (1, "Stop Group Access Details");
	Print (2, "Group        Stop    Line    Riders");
	Print (1);
}

/*********************************************|***********************************************

	Stop Group Access Details

	Group        Stop    Line    Riders   
	
	ssssssssssssssssssssssssssssssssssssss 

	     From ddddddd ddddddd            ssssssssssssssss at ssssssssssssssss
		   To ddddddd ddddddd  dddddddd  ssssssssssssssss at ssssssssssssssss

	     Total                 dddddddd

**********************************************|***********************************************/ 
