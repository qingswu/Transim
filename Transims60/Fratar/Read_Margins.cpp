//*********************************************************
//	Read_Margins.cpp - read marginal data
//*********************************************************

#include "Fratar.hpp"

//---------------------------------------------------------
//	Read_Margins
//---------------------------------------------------------

void Fratar::Read_Margins (void)
{
	int i, j, type, low, num_org, num_des, num_periods, map;
	double trips, total;

	Doubles columns;
	Dbls_Array od_array;
	Margin_Itr margin_itr;
	Margin_Group margin_data;
	Int_Map type_map;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;

	for (i=0, margin_itr = margin_group.begin (); margin_itr != margin_group.end (); margin_itr++, i++) {

		//---- read the margin type map ----

		if (margin_itr->map_flag) {
			Show_Message (1, String ("Reading %s -- Record") % margin_itr->map_file->File_Type ());
			Set_Progress ();

			type_map.clear ();
			low = 0;

			while (margin_itr->map_file->Read ()) {
				Show_Progress ();

				type = margin_itr->map_file->Get_Integer (0);
				map = margin_itr->map_file->Get_Integer (1);

				map_stat = type_map.insert (Int_Map_Data (map, (int) type_map.size ()));

				margin_itr->type_map.insert (Int_Map_Data (type, map_stat.first->second));

				if (i == 0) {
					new_file.Add_Org (type);
				} else if (i == 1) {
					new_file.Add_Des (type);
				} else if (i == 2) {
					new_file.Add_Range (low, type-1);
					low = type;
				} else {
					continue;
				}
			}
			End_Progress ();

			margin_itr->map_file->Close ();

			margin_itr->target.assign (type_map.size (), 0);
			margin_itr->total.assign (type_map.size (), 0);

			Print (1, String ("%s has %d Types Mapped to %d Margins") % margin_itr->map_file->File_Type () % margin_itr->type_map.size () % type_map.size ());
		}

		//---- process the base margin file ----

		if (margin_itr->base_flag) {
	
			Show_Message (1, String ("Reading %s -- Record") % margin_itr->base->File_Type ());
			Set_Progress ();

			total = 0;
			low = 0;

			while (margin_itr->base->Read ()) {
				Show_Progress ();

				//---- get the type value ----

				type = margin_itr->base->Get_Integer (margin_itr->type_field);
				if (type == 0) continue;

				if (margin_itr->map_flag) {
					map_itr = type_map.find (type);
					if (map_itr == type_map.end ()) {
						Warning (String ("Type %d was Not Found in the Type Map File") % type);
						j = 0;
					} else {
						j = map_itr->second;
					}
				} else {
					if (i == 0) {
						j = new_file.Add_Org (type);
					} else if (i == 1) {
						j = new_file.Add_Des (type);
					} else if (i == 2) {
						j = new_file.Num_Periods ();
						new_file.Add_Range (low, type-1);
						low = type;
					} else {
						continue;
					}
				}
				trips = margin_itr->base->Get_Double (margin_itr->trip_field);

				if (j < (int) margin_itr->total.size ()) {
					margin_itr->total [j] += trips;
				} else {
					margin_itr->total.push_back (trips);
				}
				total += trips;
			}
			End_Progress ();

			margin_itr->base->Close ();

			Print (1, String ("%s has %d Records and %.0lf Trips") % margin_itr->base->File_Type () % Progress_Count () % total);
		}

		//---- process the margin file ----
	
		Show_Message (1, String ("Reading %s -- Record") % margin_itr->file->File_Type ());
		Set_Progress ();

		total = 0;
		low = 0;

		if (i == 3) {
			new_file.Range_Flag (true);
		}

		//---- read the margin data ----

		while (margin_itr->file->Read ()) {
			Show_Progress ();

			//---- get the type value ----

			type = margin_itr->file->Get_Integer (margin_itr->type_field);
			if (type == 0) continue;

			if (margin_itr->map_flag) {
				map_itr = type_map.find (type);
				if (map_itr == type_map.end ()) {
					Warning (String ("Type %d was Not Found in the Type Map File") % type);
					j = 0;
				} else {
					j = map_itr->second;
				}
			} else {
				if (i == 0) {
					j = new_file.Add_Org (type);
				} else if (i == 1) {
					j = new_file.Add_Des (type);
				} else if (i == 2) {
					j = new_file.Num_Periods ();
					new_file.Add_Range (low, type-1);
					low = type;
				} else {
					continue;
				}
			}
			trips = margin_itr->file->Get_Double (margin_itr->trip_field);

			if (j < (int) margin_itr->target.size ()) {
				margin_itr->target [j] += trips;
			} else {
				margin_itr->target.push_back (trips);
			}
			total += trips;
		}
		End_Progress ();

		margin_itr->file->Close ();

		if (!margin_itr->map_flag) {
			type = (int) margin_itr->target.size ();

			if (i == 0) {
				new_file.Num_Org (type);
			} else if (i == 1) {
				new_file.Num_Des (type);
			}
			margin_itr->total.assign (type, 0);
		}
		Print (1, String ("%s has %d Records and %.0lf Trips") % margin_itr->file->File_Type () % Progress_Count () % total);
	}

	num_periods = new_file.Num_Periods ();
	if (num_periods == 0) {
		new_file.Range_Flag (true);
		new_file.Add_Range (0, Dtime (MIDNIGHT));

		margin_data.group = 2;
		margin_data.target.assign (1, 1);
		margin_data.total.assign (1, 0);

		margin_group.push_back (margin_data);
	}

	//---- build the trip table ----

	new_file.Create ();

	num_org = new_file.Num_Org ();
	num_des = new_file.Num_Des ();
	num_periods = new_file.Num_Periods ();

	//---- initialize the trip table ----

	if (in_flag) {
		trips = 0.0;
	} else {
		trips = 1.0;
	}
	columns.assign (num_des, trips);
	od_array.assign (num_org, columns);
	odt_array.assign (num_periods, od_array);
}
