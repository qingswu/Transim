//*********************************************************
//	New_Link.cpp - create a new link record
//*********************************************************

#include "RoutePrep.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	New_Link
//---------------------------------------------------------

int RoutePrep::New_Link (int anode, int bnode, Link_Data &link_rec, Points &points)
{
	int i, index, lanes, num, spd, min_spd, num_detail;
	double length, x1, y1, x2, y2, z1, z2, dx, dy, dz, ratio, speed;

	Dir_Data dir_rec;
	Shape_Data shape_rec;

	XYZ xyz;
	Points_Itr pt_itr;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Int2_Map_Stat ab_stat;
	Int2_Map_Itr ab_itr;

	num_detail = 0;
	min_spd = Round (1);

	//---- create a new record ----

	if (new_link_file->Link () == 0) {
		new_link_file->Link (new_link++);
	}
	if (new_link_file->Link () >= new_link) {
		new_link = new_link_file->Link () + 1;
	}

	//---- process the point data ----

	length = 0.0;
	x1 = y1 = z1 = 0.0;
	num = (int) points.size ();

	if (shape_flag) {
		shape_rec.Clear ();
		shape_rec.Link (new_link_file->Link ());
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
				xyz.z = Round (x2);

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

	if (new_link_file->Length () <= 0) {
		new_link_file->Length (DTOI (length));
	} else {
		ratio = length / (double) new_link_file->Length ();
		if (ratio < 1.0 || ratio > 1.1) {
			new_link_file->Length (DTOI (length));
		}
	}

	//---- translate the data fields ----

	link_rec.Link (new_link_file->Link ());
	link_rec.Length (Round (new_link_file->Length ()));
	link_rec.Aoffset (Round (new_link_file->Setback_A ()));
	link_rec.Boffset (Round (new_link_file->Setback_B ()));
	link_rec.Type (new_link_file->Type ());
	link_rec.Divided (new_link_file->Divided ());
	link_rec.Area_Type (new_link_file->Area_Type ());
	link_rec.Use (new_link_file->Use ());
	link_rec.Grade (Round (new_link_file->Grade ()));
	if (Notes_Name_Flag ()) {
		link_rec.Name (new_link_file->Name ());
		link_rec.Notes (new_link_file->Notes ());
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
