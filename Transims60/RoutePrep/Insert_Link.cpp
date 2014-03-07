//*********************************************************
//	Insert_Link.cpp - convert the link data
//*********************************************************

#include "RoutePrep.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Insert_Link
//---------------------------------------------------------

int RoutePrep::Insert_Link (Points &points)
{
	int anode, bnode, node, xa, ya, xb, yb;
	int i, link, index, lanes, num, spd, min_spd;
	double len, length, x1, y1, x2, y2, z1, z2, dx, dy, dz, ratio, speed;

	XYZ xyz;
	Points_Itr pt_itr;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Int2_Map_Stat ab_stat;
	Int2_Map_Itr ab_itr;
	
	Node_Data *node_ptr, node_rec;
	Node_Itr node_itr;
	Link_Data link_rec;
	Dir_Data dir_rec, *dir_ptr;
	Shape_Data shape_rec;

	link_rec.Clear ();
	min_spd = Round (1);

	//--- check the anode ----

	anode = new_link_file->Node_A ();
	pt_itr = points.begin ();

	xa = Round (pt_itr->x);
	ya = Round (pt_itr->y);

	if (anode == 0) {
		len = resolution;

		for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
			dx = node_itr->X () - xa;
			dy = node_itr->Y () - ya;
			length = sqrt (dx * dx + dy * dy);

			if (length < len) {
				len = length;
				anode = node_itr->Node ();
				if (len == 0.0) break;
			}
		}
		if (anode == 0) {
			anode = new_node++;
		}
	}
	map_itr = node_map.find (anode);

	if (map_itr == node_map.end ()) {
		node = (int) node_array.size ();
		node_rec.Node (anode);
		node_rec.X (UnRound (xa));
		node_rec.Y (UnRound (ya));
		node_rec.Z (0);

		node_map.insert (Int_Map_Data (anode, node));
		node_array.push_back (node_rec);
	} else {
		node = map_itr->second;
		node_ptr = &node_array [node];

		dx = node_ptr->X () - xa;
		dy = node_ptr->Y () - ya;
		length = sqrt (dx * dx + dy * dy);

		if (length > resolution) {
			Warning (String ("Node %d is at a Different Location") % anode);
		}
		xa = node_ptr->X ();
		ya = node_ptr->Y ();
	}
	link_rec.Anode (node);

	//--- check the bnode ----

	bnode = new_link_file->Node_B ();
	pt_itr = --points.end ();

	xb = Round (pt_itr->x);
	yb = Round (pt_itr->y);

	if (bnode == 0) {
		len = resolution;

		for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
			dx = node_itr->X () - xb;
			dy = node_itr->Y () - yb;
			length = sqrt (dx * dx + dy * dy);

			if (length < len) {
				len = length;
				bnode = node_itr->Node ();
				if (len == 0.0) break;
			}
		}
		if (bnode == 0) {
			bnode = new_node++;
		}
	}
	map_itr = node_map.find (bnode);

	if (map_itr == node_map.end ()) {
		node = (int) node_array.size ();
		node_rec.Node (bnode);
		node_rec.X (UnRound (xb));
		node_rec.Y (UnRound (yb));
		node_rec.Z (0);

		node_map.insert (Int_Map_Data (bnode, node));
		node_array.push_back (node_rec);
	} else {
		node = map_itr->second;
		node_ptr = &node_array [node];

		dx = node_ptr->X () - xb;
		dy = node_ptr->Y () - yb;
		length = sqrt (dx * dx + dy * dy);

		if (length > resolution) {
			Warning (String ("Node %d is at a Different Location") % bnode);
		}
		xb = node_ptr->X ();
		yb = node_ptr->Y ();
	}
	link_rec.Bnode (node);

	//---- check for an existing link ----

	if (anode == bnode) return (-1);

	ab_itr = ab_map.find (Int2_Key (anode, bnode));

	if (ab_itr != ab_map.end ()) {
		dir_ptr = &dir_array [ab_itr->second];

		return (dir_ptr->Link ());
	}

	//---- create a new record ----

	link = new_link_file->Link ();

	if (link == 0) {
		link = new_link++;
	} else if (link > new_link) {
		new_link = link + 1;
	}

	//---- process the point data ----

	length = 0.0;
	x1 = y1 = z1 = 0.0;
	num = (int) points.size ();

	if (shape_flag) {
		shape_rec.Clear ();
		shape_rec.Link (link);
	}

	for (i=1, pt_itr = points.begin (); i <= num; i++, pt_itr++) {
		x2 = pt_itr->x;
		y2 = pt_itr->y;
		z2 = pt_itr->z;

		if (i > 1) {
			dx = x2 - x1;
			dy = y2 - y1;
			dz = z2 - z1;

			length += sqrt (dx * dx + dy * dy + dz * dz);

			//---- write the shape record ----

			if (i < num && shape_flag) {
				xyz.x = Round (x2);
				xyz.y = Round (y2);
				xyz.z = Round (z2);

				shape_rec.push_back (xyz);
			}
		}
		x1 = x2; 
		y1 = y2;
		z1 = z2;
	}
	if (shape_flag && shape_rec.size () > 0) {
		index = (int) shape_array.size ();
		map_stat = shape_map.insert (Int_Map_Data (shape_rec.Link (), index));

		if (!map_stat.second) {
			Warning ("Duplicate Link Number = ") << shape_rec.Link ();
		} else {
			shape_array.push_back (shape_rec);
			link_rec.Shape (index);
		}
	}

	len = new_link_file->Length ();

	if (len <= 0) {
		len = length;
	} else {
		ratio = length / len;
		if (ratio < 1.0 || ratio > 1.1) {
			len = length;
		}
	}

	//---- translate the data fields ----

	link_rec.Link (link);
	link_rec.Length (Round (len));
	link_rec.Aoffset (Round (new_link_file->Setback_A ()));
	link_rec.Boffset (Round (new_link_file->Setback_B ()));
	link_rec.Type (new_link_file->Type ());
	link_rec.Divided (new_link_file->Divided ());
	link_rec.Area_Type (new_link_file->Area_Type ());
	link_rec.Use (new_link_file->Use ());
	link_rec.Grade (Round (new_link_file->Grade ()));
	if (Notes_Name_Flag ()) {
		if (!new_link_file->Name ().empty ()) {
			link_rec.Name (new_link_file->Name ());
		} else {
			link_rec.Name ("");
		}
		if (!new_link_file->Notes ().empty ()) {
			link_rec.Notes (new_link_file->Notes ());
		} else {
			link_rec.Notes ("");
		}
	}
	link_rec.BA_Dir (-1);

	//---- process the AB direction ----

	lanes = new_link_file->Lanes_AB ();
	if (lanes <= 0) {
		lanes = 1;
		new_link_file->Lanes_AB (lanes);
	}
	dir_rec.Link ((int) link_array.size ());
	dir_rec.Dir (0);
	dir_rec.Lanes (lanes);
	dir_rec.In_Bearing (new_link_file->Bearing_A ());
	dir_rec.Out_Bearing (new_link_file->Bearing_B ());

	spd = Round (Internal_Units (new_link_file->Fspd_AB (), (Metric_Flag ()) ? KPH : MPH));
	speed = Internal_Units (new_link_file->Speed_AB (), ((Metric_Flag ()) ? KPH : MPH));

	dir_rec.Speed (Round (speed));
	dir_rec.Capacity (new_link_file->Cap_AB ());

	if (dir_rec.Speed () < min_spd && spd < min_spd) {
		spd = Round (Internal_Units (25.0, MPH));
		dir_rec.Speed (spd);
	}
	dir_rec.Time0 ((double) link_rec.Length () / spd + 0.09);

	//---- save the link record ----

	index = (int) dir_array.size ();

	ab_stat = ab_map.insert (Int2_Map_Data (Int2_Key (anode, bnode), index));

	link_rec.AB_Dir (index);

	dir_array.push_back (dir_rec);

	//---- check for B->A data ----

	lanes = new_link_file->Lanes_BA ();

	if (lanes > 0) {
		dir_rec.Dir (1);
		dir_rec.Lanes (lanes);

		spd = Round (Internal_Units (new_link_file->Fspd_BA (), (Metric_Flag ()) ? KPH : MPH));
		speed = Internal_Units (new_link_file->Speed_BA (), ((Metric_Flag ()) ? KPH : MPH));

		dir_rec.Speed (Round (speed));
		dir_rec.Capacity (new_link_file->Cap_BA ());

		if (dir_rec.Speed () < min_spd && spd < min_spd) {
			spd = Round (Internal_Units (25.0, MPH));
			dir_rec.Speed (spd);
		}
		dir_rec.Time0 ((double) link_rec.Length () / spd + 0.09);

		//---- save the link direction record ----

		index = (int) dir_array.size ();

		ab_stat = ab_map.insert (Int2_Map_Data (Int2_Key (bnode, anode), index));

		link_rec.BA_Dir (index);

		dir_array.push_back (dir_rec);
	}

	//---- save the link record ----

	index = (int) link_array.size ();

	map_stat = link_map.insert (Int_Map_Data (link_rec.Link (), index));

	if (!map_stat.second) {
		Warning ("Duplicate Link Number = ") << link_rec.Link ();
	} else {
		link_array.push_back (link_rec);
	}
	return (index);
}
