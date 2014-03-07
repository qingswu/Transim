//*********************************************************
//	Selection_Data.cpp - selection data map
//*********************************************************

#include "Selection_Data.hpp"

//---------------------------------------------------------
//	Best
//---------------------------------------------------------

Select_Map_Itr Select_Map::Best (int hhold, int person, int tour, int trip)
{
	Trip_Index trip_index, index;
	Select_Map_Itr map_itr;

	trip_index.Set (hhold, person, tour, trip);

	map_itr = find (trip_index);

	if (map_itr == end ()) {
		trip_index.Person (0);
		trip_index.Tour (0);
		trip_index.Trip (0);

		map_itr = lower_bound (trip_index);
		if (map_itr == end ()) return (end ());

		index = map_itr->first;
		if (index.Household () != hhold) return (end ());
		if (person == 0) return (map_itr);
		if (index.Person () > person) return (end ());

		if (index.Person () != 0 && index.Person () != person) {
			for (++map_itr; map_itr != end (); map_itr++) {
				index = map_itr->first;
				if (index.Household () > hhold || index.Person () > person) return (end ());
				if (index.Person () == person) break;
			}
		}
		if (tour == 0) return (map_itr);
		if (index.Tour () > tour) return (end ());

		if (index.Tour () != 0 && index.Tour () != tour) {
			for (++map_itr; map_itr != end (); map_itr++) {
				index = map_itr->first;
				if (index.Household () > hhold || index.Person () > person || index.Tour () > tour) return (end ());
				if (index.Tour () == tour) break;
			}
		}
		if (trip == 0) return (map_itr);
		if (index.Trip () > trip) return (end ());

		if (index.Trip () != 0 && index.Trip () != trip) {
			for (++map_itr; map_itr != end (); map_itr++) {
				index = map_itr->first;
				if (index.Household () > hhold || index.Person () > person || index.Tour () > tour || index.Trip () > trip) return (end ());
				if (index.Trip () == trip) break;
			}
		}
	}
	return (map_itr); 
}
#ifdef MAKE_MPI
bool Select_Map::Pack (Data_Buffer &data)
{
	size_t num = size ();
	if (data.Add_Data (&num, sizeof (num))) {
		if (num == 0) return (true);
		Select_Map_Itr itr;
		Selection_Data rec;
		for (itr = begin (); itr != end (); itr++) {
			Trip_Index index = itr->first;
			rec.Household (index.Household ());
			rec.Person (index.Person ());
			rec.Tour (index.Tour ());
			rec.Trip (index.Trip ());
			rec.Type (itr->second.Type ());
			rec.Partition (itr->second.Partition ());
			if (!data.Add_Data (&rec, sizeof (rec))) return (false);
		}
	}
	return (false);
}
bool Select_Map::UnPack (Data_Buffer &data)
{
	size_t num;
	if (data.Get_Data (&num, sizeof (num))) {
		Selection_Data record;
		Trip_Index index;
		Select_Data rec;

		while (num-- > 0) {
			if (!data.Get_Data (&record, sizeof (record))) return (false);

			record.Get_Trip_Index (index);
			rec.Type (record.Type ());
			rec.Partition (record.Partition ());

			insert (Select_Map_Data (index, rec));
		}
		return (true);
	}
	return (false);
}
#endif
