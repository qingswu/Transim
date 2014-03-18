//*********************************************************
//	Write_Route_Nodes.cpp - write a new route nodes file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Route_Nodes
//---------------------------------------------------------

void Data_Service::Write_Route_Nodes (void)
{
	Route_Nodes_File *file = (Route_Nodes_File *) System_File_Handle (NEW_ROUTE_NODES);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = route_map.begin (); itr != route_map.end (); itr++) {
		Show_Progress ();

		count += Put_Route_Nodes_Data (*file, route_nodes_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Route_Nodes_Data
//---------------------------------------------------------

int Data_Service::Put_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data)
{
	if (file.Model_Format () == TPPLUS) {
		int rec, count, i, node, last_node;
		double speed;
		bool n_flag, node_flag, skip;
	
		Route_Node_Itr node_itr;
		Route_Period_Itr period_itr;

		fstream &fh = file.File ();

		rec = 0;

		//---- update the route nodes ----

		node_flag = dat->System_File_Flag (NODE);

		n_flag = false;

		fh << "LINE NAME=\"" << data.Name () << "\"," << endl;
		fh << "     OWNER=\"" << data.Notes () << "\"," << endl;
		fh << "     ONEWAY=" << ((data.Oneway () == 2) ? "F" : "T") << ", MODE=" << data.Mode ();

		for (i=1, period_itr = data.periods.begin (); period_itr != data.periods.end (); period_itr++, i++) {
			if (i==1 && period_itr->TTime () > 0) {
				fh << ", RUNTIME=" << period_itr->TTime ().Minutes ();
			}
			if (period_itr->Headway () > 0 && i <= 5) {
				fh << ", FREQ[" << i << "]=" << period_itr->Headway ().Minutes ();
			}
		}
		fh << ", " << endl;
		rec += 3;

		skip = false;
		count = last_node = 0;
		fh << "     N=";

		for (i=0, node_itr = data.nodes.begin (); node_itr != data.nodes.end (); node_itr++, i++) {

			if (count > 0) {
				if (!skip) {
					if (count == 8) {
						fh << "," << endl << "     ";
						count = 0;
						rec++;
					} else {
						fh << ", ";
					}
				}
			}
			skip = false;

			if (n_flag) {
				fh << "N=";
				n_flag = false;
			}
			node = node_itr->Node ();

			if (node_flag) {
				node = node_array [node].Node ();
				if (node == last_node) {
					skip = true;
					continue;
				}
				last_node = node;
			}

			fh << ((node_itr->Type () == NO_STOP) ? -node : node);
			count++;

			if (node_itr->Speed () > 0) {
				speed = External_Units (node_itr->Speed (), MPH);
				if (count == 8) {
					fh << "," << endl << "     ";
					count = 0;
				} else {
					fh << ", ";
				}
				fh << (String ("SPEED=%.2lf") % speed);
				n_flag = true;
				count++;
			}
		}
		fh << endl;

		return (rec);
	}
	Route_Node_Itr node_itr;
	Route_Period_Itr period_itr;
	Node_Data *node_ptr;
	Veh_Type_Data *veh_type_ptr;

	int i, node, dwell, count = 0;
	file.Route (data.Route ());
	file.Mode (data.Mode ());

	if (veh_type_array.size () > 0 && data.Veh_Type () >= 0) {
		veh_type_ptr = &veh_type_array [data.Veh_Type ()];
		file.Veh_Type (veh_type_ptr->Type ());
	} else {
		file.Veh_Type (data.Veh_Type ());
	}
	file.Oneway (data.Oneway ());
	file.Name (data.Name ());
	file.Notes (data.Notes ());
	file.Nodes ((int) data.nodes.size ());

	for (i=0, period_itr = data.periods.begin (); period_itr != data.periods.end (); period_itr++, i++) {
		file.Headway (i, period_itr->Headway ());
		file.Offset (i, period_itr->Offset ());
		file.TTime (i, period_itr->TTime ());

		int pattern = period_itr->Pattern ();
		int code = 0;

		if (pattern != 0) {
			String codes;

			bool flag;
			int j, k, n, num;
			int max_n = 8 * sizeof (int);

			num = period_itr->Count ();

			if (num == 0) {
				for (num=2; num <= 9; num++) {
					flag = true;
					for (n=0; n < (max_n - num); n++) {
						j = (pattern & (1 << n));
						k = (pattern & (1 << (n + num)));

						if ((j != 0 && k == 0) || (j == 0 && k != 0)) {
							flag = false;
							break;
						}
					}
					if (flag) break;
				}
			}
			for (n=0; n < num; n++) {
				j = (1 << n);
				if ((pattern & j) != 0) {
					codes += "1";
				} else {
					codes += "2";
				}
			}
			code = codes.Integer ();
		}
		file.Pattern (i, code);
	}
	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (node_itr = data.nodes.begin (); node_itr != data.nodes.end (); node_itr++) {
		node_ptr = &node_array [node_itr->Node ()];
		node = node_ptr->Node ();
		if (node_itr->Type () == NO_STOP && !file.Type_Flag ()) node = -node;
		dwell = node_itr->Dwell ();
		if (node_itr->Type () != NO_STOP && file.Dwell_Flag () && dwell <= 0) dwell = Dtime (5, SECONDS);

		file.Node (node);
		file.Type (node_itr->Type ());
		file.Dwell (dwell);
		file.Time (node_itr->Time ());
		file.Speed (node_itr->Speed ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}

