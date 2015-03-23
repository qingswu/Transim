//*********************************************************
//	Input_Routes.cpp - convert the transit line data
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Read_Line
//---------------------------------------------------------

void NetPrep::Input_Routes (void)
{
	int i, period, last_node, last_saved, in_lines, out_lines, mode;
	bool stop, line_flag, first_node;
	Dtime freq;
	String record, text;

	File_Group_Itr group_itr;
	Route_Data data;
	Int_Map_Itr map_itr;

	if (oneway_link_flag) {
		Header_Number (ONEWAY_LINKS);
		New_Page ();
	}

	//---- read each line file ----

	for (group_itr = file_groups.begin (); group_itr != file_groups.end (); group_itr++) {

		if (group_itr->line_file == 0) continue;

		Print (2, "Reading ") << group_itr->line_file->File_Type ();
		Show_Message (String ("Reading %s -- Record") % group_itr->line_file->File_Type ());
		Set_Progress ();

		period = 1;

		data.flip = group_itr->flip;
		data.oneway = true;
		data.name.clear ();
		data.nodes.clear ();
		data.headway.assign (num_periods, 0);
		data.ttime = 0;
		data.offset = -1;
		data.mode = -1;
		data.veh_type = group_itr->veh_type;
		data.notes = String ("Group %d") % group_itr->group;
		
		first_node = false;
		last_node = last_saved = 0;
        in_lines = out_lines = 0;

		//---- process each record ----

		while (group_itr->line_file->Read ()) {
			Show_Progress ();

			record = group_itr->line_file->Record_String ();
			record.Clean ();
			line_flag = false;

			while (!record.empty ()) {
				record.Split (text, "=, \t");

				if (text.empty ()) break;
				if (text [0] == ';') {
					if (line_flag && !record.empty ()) {
						data.notes += "; " + record;
					}
					break;
				}
				if (text.Equals ("LINE")) {

					//---- process the existing line ----

					if (data.nodes.size () > 1 && data.mode >= LOCAL_BUS) {
						Save_Route (data);
                        out_lines++;
					}
					period = 1;
					data.oneway = true;
					data.nodes.clear ();
					data.headway.assign (num_periods, 0);
					data.ttime = 0;
					data.offset = -1;
					data.mode = -1;
					data.veh_type = group_itr->veh_type;
					data.notes = String ("Group %d") % group_itr->group;
					line_flag = true;
					first_node = true;
					last_node = last_saved = 0;
                    in_lines++;

				} else if (text.Equals ("NAME")) {

					record.Split (data.name, ",");

				} else if (text.Equals ("OWNER")) {

					record.Split (data.notes, ",");

				} else if (text.Equals ("ONEWAY")) {

					record.Split (text, ",");
					data.oneway = (text [0] == 'Y' || text [0] == 'y' || text [0] == 'T' || text [0] == 't');

				} else if (text.Equals ("MODE")) {

					record.Split (text, ",");

					mode = text.Integer ();
					map_itr = route_mode_map.find (mode);

					if (map_itr == route_mode_map.end ()) {
						Warning (String ("Mode %d was Not Converted") % mode);
						route_mode_map.insert (Int_Map_Data (data.mode, NO_TRANSIT));
						mode = 0;
					} else {
						mode = map_itr->second;
					}
					data.mode = mode;

				} else if (text.Starts_With ("FREQ")) {

					if (text [4] == '[') {
						text.erase (0, 5);
						period = text.Integer ();
					} else {
						period = 1;
					}
					record.Split (text, ",");

					freq.Minutes (text.Integer ());

					for (i=0; i < num_periods; i++) {
						if (group_itr->period_map [i] == period) {
							data.headway [i] = DTOI (freq * group_itr->period_fac [i]);
						}
					}

				} else if (text.Equals ("RUNTIME") || text.Equals ("RT")) {

					record.Split (text, ",");

					data.ttime.Minutes (text.Integer ());

				} else if (text.Equals ("OFFSET")) {

					record.Split (text, ",");

					data.offset.Minutes (text.Integer ());
					if (data.offset < 0) data.offset = -1;

				} else if ((text [0] >= '0' && text [0] <= '9') || text [0] == '-') {

					i = text.Integer ();
					last_node = i;

					if (i < 0) {
						i = -i;
						stop = false;
					} else {
						stop = true;
					}
					map_itr = node_map.find (i);
					if (map_itr != node_map.end ()) {
						last_saved = i;
						i = map_itr->second;
						if (!stop) i = -i;
						data.nodes.push_back (i);
					} else if (first_node) {
						Warning (String ("First Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
					} else {
						Warning (String ("Route Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
					}
					first_node = false;

				} else if (text.Equals ("NODES") || text.Equals ("N")) {

					record.Split (text, ", ");
				
					if ((text [0] >= '0' && text [0] <= '9') || text [0] == '-') {

						i = text.Integer ();
						if (i < 0) {
							i = -i;
							stop = false;
						} else {
							stop = true;
						}
						map_itr = node_map.find (i);
						if (map_itr != node_map.end ()) {
							last_saved = i;
							i = map_itr->second;
							if (!stop) i = -i;
							data.nodes.push_back (i);
						} else if (first_node) {
							Warning (String ("First Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
						} else {
							Warning (String ("Route Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
						}
						first_node = false;
					}
				} else {
					//Warning ("Unrecognized Command=") << text;
					record.Split (text, ",");
				}
			}
		}
		End_Progress ();

		if (data.nodes.size () > 1 && data.mode >= LOCAL_BUS) {
			Save_Route (data);
            out_lines++;
		}
		group_itr->line_file->Close ();
		Print (1, group_itr->line_file->File_Type ()) << " Lines Read=" << in_lines << " Lines Written=" << out_lines;
        Print (1);
	}
	Header_Number (0);
}
