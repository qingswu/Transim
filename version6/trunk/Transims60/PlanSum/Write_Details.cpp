//*********************************************************
//	Write_Details.cpp - write access detail file
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Access_Details
//---------------------------------------------------------

void PlanSum::Write_Access_Details (void)
{
	int i, j, p, num, last_group, num_periods, trips;
    String label;

	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Zone_Data *zone_ptr;
	Location_Data *loc_ptr;
	Int_Map_Itr int_itr;
	Xfer_IO_Map_Itr map_itr, group_itr;
	Xfer_IO xfer_io, od;
	Int2_Map od_map;
	Int2_Key od_key;
	Int2_Map_Itr o_itr, d_itr;
	Int3_Array odt_trips;
	Ints_Array od_trips;
	Integers od_row;

	Show_Message ("Writing Access Details -- Record");
	Set_Progress ();

	last_group = 0;
	num_periods = sum_periods.Num_Periods ();
	if (num_periods < 1) num_periods = 1;
	num = trips = 0;
	memset (&xfer_io, '\0', sizeof (xfer_io));

	fstream &file = access_detail_file.File ();

	for (map_itr = access_detail.begin (); ; map_itr++) {
		Show_Progress ();

		if (map_itr != access_detail.end ()) {
			xfer_io = map_itr->first;
			trips = map_itr->second;
		}
		if (map_itr == access_detail.end () || last_group != xfer_io.group) {
			if (last_group > 0) {
				for (p=0; p < num_periods; p++) {
					label ("%d %s") % last_group % stop_equiv.Group_Label (last_group);

					if (num_periods > 1) {
						label += "\tPeriod=" + sum_periods.Range_Format (p);
					}
					file << label << endl;

					if (Notes_Name_Flag ()) {
						file << "\t";

						for (d_itr=od_map.begin (); d_itr != od_map.end (); d_itr++) {
							label ("\t");

							if (d_itr->first.second == 0) {
								if (zone_file_flag) {
									int_itr = zone_map.find (d_itr->first.first);
									if (int_itr != zone_map.end ()) {
										zone_ptr = &zone_array [int_itr->second];
										label += zone_ptr->Notes ();
									}
								} else {
									int_itr = location_map.find (xfer_io.to_stop);
									if (int_itr != location_map.end ()) {
										loc_ptr = &location_array [int_itr->second];
										label += loc_ptr->Notes ();
									}
								}
							} else {
								int_itr = line_map.find (d_itr->first.second);
								if (int_itr != line_map.end ()) {
									line_ptr = &line_array [int_itr->second];
									label += line_ptr->Name ();
								}
								int_itr = stop_map.find (d_itr->first.first);
								if (int_itr != stop_map.end ()) {
									stop_ptr = &stop_array [int_itr->second];
									if (!stop_ptr->Notes ().empty ()) {
										label += " ";
										label += stop_ptr->Notes ();
									}
								}
							}
							file << label;
						}
						file << endl << "\t";
					}
					file << "Origin";

					for (d_itr=od_map.begin (); d_itr != od_map.end (); d_itr++) {
						if (d_itr->first.second == 0) {
							label ("\t%d-%s") % d_itr->first.first % ((zone_file_flag) ? "Zone " : "Location");
						} else {
							label ("\t%d-%d") % d_itr->first.first % d_itr->first.second;
						}
						file << label;
					}
					file << endl;

					for (o_itr = od_map.begin (); o_itr != od_map.end (); o_itr++) {

						if (Notes_Name_Flag ()) {
							label ("");

							if (o_itr->first.second == 0) {
								if (zone_file_flag) {
									int_itr = zone_map.find (o_itr->first.first);
									if (int_itr != zone_map.end ()) {
										zone_ptr = &zone_array [int_itr->second];
										label += zone_ptr->Notes ();
									}
								} else {
									int_itr = location_map.find (xfer_io.to_stop);
									if (int_itr != location_map.end ()) {
										loc_ptr = &location_array [int_itr->second];
										label += loc_ptr->Notes ();
									}
								}
							} else {
								int_itr = line_map.find (o_itr->first.second);
								if (int_itr != line_map.end ()) {
									line_ptr = &line_array [int_itr->second];
									label += line_ptr->Name ();
								}
								int_itr = stop_map.find (o_itr->first.first);
								if (int_itr != stop_map.end ()) {
									stop_ptr = &stop_array [int_itr->second];
									if (!stop_ptr->Notes ().empty ()) {
										label += " ";
										label += stop_ptr->Notes ();
									}
								}
							}
							file << label << "\t";
						}

						if (o_itr->first.second == 0) {
							label ("%d-%s") % o_itr->first.first % ((zone_file_flag) ? "Zone " : "Location");
						} else {
							label ("%d-%d") % o_itr->first.first % o_itr->first.second;
						}
						file << label;

						for (d_itr=od_map.begin (); d_itr != od_map.end (); d_itr++) {
							file << "\t" << odt_trips [p] [o_itr->second] [d_itr->second];
						}

						//---- print the label ----

						if (Notes_Name_Flag ()) {
							label ("\t");

							if (o_itr->first.second == 0) {
								if (zone_file_flag) {
									int_itr = zone_map.find (o_itr->first.first);
									if (int_itr != zone_map.end ()) {
										zone_ptr = &zone_array [int_itr->second];
										label += zone_ptr->Notes ();
									}
								} else {
									int_itr = location_map.find (xfer_io.to_stop);
									if (int_itr != location_map.end ()) {
										loc_ptr = &location_array [int_itr->second];
										label += loc_ptr->Notes ();
									}
								}
							} else {
								int_itr = line_map.find (o_itr->first.second);
								if (int_itr != line_map.end ()) {
									line_ptr = &line_array [int_itr->second];
									label += line_ptr->Name ();
								}
								int_itr = stop_map.find (o_itr->first.first);
								if (int_itr != stop_map.end ()) {
									stop_ptr = &stop_array [int_itr->second];
									label += " at ";
									label += stop_ptr->Name ();

									if (!stop_ptr->Notes ().empty ()) {
										label += " -- ";
										label += stop_ptr->Notes ();
									}
								}
							}
							file << label << endl;
						}
					}
				}
			}
			if (map_itr == access_detail.end ()) break;

			//---- build the location index ----

			od_map.clear ();
			od_trips.clear ();
			last_group = xfer_io.group;

			for (group_itr = map_itr; group_itr != access_detail.end (); group_itr++) {
				od = group_itr->first;
				if (od.group != last_group) break;

				od_key.first = od.from_stop;
				od_key.second = od.from_line;
				od_map.insert (Int2_Map_Data (od_key, (int) od_map.size ()));
				
				od_key.first = od.to_stop;
				od_key.second = od.to_line;
				od_map.insert (Int2_Map_Data (od_key, (int) od_map.size ()));
			}
			num = (int) od_map.size ();
			od_row.assign (num, 0);
			od_trips.assign (od_map.size (), od_row);
			odt_trips.assign (num_periods, od_trips);
		}

		od_key.first = xfer_io.from_stop;
		od_key.second = xfer_io.from_line;
		o_itr = od_map.find (od_key);
		if (o_itr == od_map.end ()) {
			Error ("OD Index Not Found");
		}
		i = o_itr->second;

		od_key.first = xfer_io.to_stop;
		od_key.second = xfer_io.to_line;
		d_itr = od_map.find (od_key);
		if (d_itr == od_map.end ()) {
			Error ("OD Index Not Found");
		}
		j = d_itr->second;

		odt_trips [xfer_io.period] [i] [j] += trips;
	}
	End_Progress ();
}
