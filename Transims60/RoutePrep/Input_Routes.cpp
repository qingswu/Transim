//*********************************************************
//	Input_Routes.cpp - convert the transit line data
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Line
//---------------------------------------------------------

void RoutePrep::Input_Routes (void)
{
	int i, period, count, last_node, last_saved, in_lines, out_lines;
	double speed;
	bool stop, line_flag, node_flag, first_node;
	Dtime freq;
	String record, text;

	File_Group_Itr group_itr;
	Route_Data data;
	Int_Map_Itr map_itr, end_itr;
	Str_ID_Itr str_id_itr;
	Doubles *speeds;

	node_flag = System_File_Flag (NODE);

	if (convert_node_flag) {
		end_itr = input_map.end ();
	} else {
		end_itr = node_map.end ();
	}

	//---- read each line file ----

	for (group_itr = file_groups.begin (); group_itr != file_groups.end (); group_itr++) {

		if (group_itr->in_file == 0) continue;
		
		Print (2, "Reading ") << group_itr->in_file->File_Type ();
		Show_Message (String ("Reading %s -- Record") % group_itr->in_file->File_Type ());
		Set_Progress ();

		period = 1;

		data.flip = false;
		data.oneway = true;
		data.name.clear ();
		data.nodes.clear ();
		data.speeds.clear ();
		data.headway.assign (num_periods, 0);
		data.ttime = 0;
		data.offset = -1;
		data.mode = -1;
		data.peak = data.offpeak = 0.0;
		data.notes = String ("Group %d") % group_itr->group;

		first_node = false;
		last_node = last_saved = 0;
        in_lines = out_lines = 0;

		//---- process each record ----

		while (group_itr->in_file->Read ()) {
			Show_Progress ();

			record = group_itr->in_file->Record_String ();
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

					if (data.nodes.size () > 1) {
						if (abs (last_node) != abs (last_saved)) {
							Warning (String ("Last Node %d on File %d Line %s was Not Found") % last_node % group_itr->group % data.name);
						}
						if (out_route_flag) {
							Write_Route (group_itr->out_file, data);
						} else if (data.mode >= LOCAL_BUS) {
							Save_Route (data);
						}
                        out_lines++;
					}
					period = 1;
					data.oneway = true;
					data.nodes.clear ();
					data.speeds.clear ();
					data.headway.assign (num_periods, 0);
					data.ttime = 0;
					data.offset = -1;
					data.mode = -1;
					data.peak = data.offpeak = 0.0;
					data.notes = String ("Group %d") % group_itr->group;
					line_flag = true;
					first_node = true;
					last_node = last_saved = 0;
                    in_lines++;

				} else if (text.Equals ("NAME")) {

					record.Split (data.name, ",");

					if (route_speed_flag) {
						str_id_itr = speed_map.find (data.name);

						if (str_id_itr == speed_map.end ()) {
							Warning (String ("Route %s was Not Found in the Route Speed File") % data.name);
						} else {
							speeds = &speed_array [str_id_itr->second];
							data.peak = speeds->at (0);
							data.offpeak = speeds->at (1);
						}
					}

				} else if (text.Equals ("OWNER")) {

					record.Split (data.notes, ",");

				} else if (text.Equals ("ONEWAY")) {

					record.Split (text, ",");
					data.oneway = (text [0] == 'Y' || text [0] == 'y' || text [0] == 'T' || text [0] == 't');

				} else if (text.Equals ("MODE")) {

					record.Split (text, ",");

					data.mode = text.Integer ();

					map_itr = route_mode_map.find (data.mode);

					if (map_itr == route_mode_map.end ()) {
						Warning (String ("Mode %d was Not Converted") % data.mode);
						route_mode_map.insert (Int_Map_Data (data.mode, NO_TRANSIT));
						data.mode = 0;
					} else {
						data.mode = map_itr->second;
					}

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
							data.headway [i] = freq;
						}
					}

				} else if (text.Equals ("RUNTIME") || text.Equals ("RT")) {

					record.Split (text, ",");

					data.ttime.Minutes (text.Integer ());

				} else if (text.Equals ("OFFSET")) {

					record.Split (text, ",");

					data.offset.Minutes (text.Integer ());
					if (data.offset < 0) data.offset = -1;

				} else if (text.Equals ("SPEED")) {

					record.Split (text, ",");

					if (!ignore_speeds) {
						speed = text.Double ();
						
						count = (int) data.nodes.size () - 1;

						while (count > (int) data.speeds.size ()) {
							data.speeds.push_back (0.0);
						}
						data.speeds.push_back (speed);
					}

				} else if ((text [0] >= '0' && text [0] <= '9') || text [0] == '-') {

					i = text.Integer ();
					last_node = i;

					if (node_flag) {
						if (i < 0) {
							i = -i;
							stop = false;
						} else {
							stop = true;
						}
						if (i == last_saved) continue;

						if (convert_node_flag) {
							map_itr = input_map.find (i);
						} else {
							map_itr = node_map.find (i);
						}
						if (map_itr != end_itr) {
							if (map_itr->second == -1) {
								if (first_node) {
									Warning (String ("First Node %d on File %d Line %s was Set to -1") % i % group_itr->group % data.name);
								}
								first_node = false;
								continue;
							}
							last_saved = i;
							i = map_itr->second;
							if (!stop) i = -i;
							data.nodes.push_back (i);
						} else if (first_node) {
							Warning (String ("First Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
						} else {
							Warning (String ("Route Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
						}
					} else {
						if (abs (i) == abs (last_saved)) continue;
						data.nodes.push_back (i);
						last_saved = i;
					}
					first_node = false;

				} else if (text.Equals ("NODES") || text.Equals ("N")) {

					record.Split (text, ", ");
				
					if ((text [0] >= '0' && text [0] <= '9') || text [0] == '-') {

						i = text.Integer ();
						last_node = i;

						if (node_flag) {
							if (i < 0) {
								i = -i;
								stop = false;
							} else {
								stop = true;
							}
							if (i == last_saved) continue;

							if (convert_node_flag) {
								map_itr = input_map.find (i);
							} else {
								map_itr = node_map.find (i);
							}
							if (map_itr != end_itr) {
								if (map_itr->second == -1) {
									if (first_node) {
										Warning (String ("First Node %d on File %d Line %s was Set to -1") % i % group_itr->group % data.name);
									}
									first_node = false;
									continue;
								}
								last_saved = i;
								i = map_itr->second;
								if (!stop) i = -i;
								data.nodes.push_back (i);
							} else if (first_node) {
								Warning (String ("First Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
							} else {
								Warning (String ("Route Node %d on File %d Line %s was Not Found") % i % group_itr->group % data.name);
							}
						} else {
							if (abs (i) == abs (last_saved)) continue;
							data.nodes.push_back (i);
							last_saved = i;
						}
						first_node = false;
					}
				} else {
					Warning ("Unrecognized Command=") << text;
					record.Split (text, ",");
				}
			}
		}
		End_Progress ();

		if (data.nodes.size () > 1) {
			if (out_route_flag) {
				Write_Route (group_itr->out_file, data);
			} else if (data.mode >= LOCAL_BUS) {
				Save_Route (data);
			}
            out_lines++;
		}
		group_itr->in_file->Close ();

		if (out_route_flag) {
			group_itr->out_file->Close ();
		}
		Print (1, group_itr->in_file->File_Type ()) << " Lines Read=" << in_lines << " Lines Written=" << out_lines;
        Print (1);
	}
}
