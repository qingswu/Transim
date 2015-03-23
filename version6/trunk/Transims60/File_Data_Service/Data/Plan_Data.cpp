
//*********************************************************
//	Plan_Data.hpp - travel plan data classes
//*********************************************************

#include "Plan_Data.hpp"

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Internal_IDs
//---------------------------------------------------------

bool Plan_Data::Internal_IDs (void)
{
	if (!Trip_Data::Internal_IDs ()) return (false);

	Plan_Leg_Itr itr;

	for (itr = begin (); itr != end (); itr++) {
		if (!itr->Internal_IDs ()) {
			exe->Warning (String ("Plan %d-%d-%d-%d %s %d was Not Found") % Household () % 
				Person () % Tour () % Trip () % exe->ID_Code ((ID_Type) itr->Type ()) % itr->ID ());
			return (false);
		}
	}
	return (true);
}

bool Plan_Leg::Internal_IDs (void)
{
	int dir;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;

	switch (Type ()) {
		case DIR_ID:
		case USE_ID:
			map_itr = dat->dir_map.find (id);
			if (map_itr == dat->dir_map.end ()) return (false);
			id = map_itr->second;
			break;
		case LINK_ID:
			map_itr = dat->link_map.find (abs (id));
			if (map_itr == dat->link_map.end ()) return (false);

			link_ptr = &dat->link_array [map_itr->second];

			if (id > 0) {
				dir = link_ptr->AB_Dir ();
			} else {
				dir = link_ptr->BA_Dir ();
			}
			if (dir < 0) {
				if (id < 0) {
					id = -map_itr->second;
				} else {
					id = map_itr->second;
				}
			} else {
				id = dir;
				Type (DIR_ID);
			}
			break;
		case LINK_AB:
		case USE_AB:
		case LINK_BA:
		case USE_BA:
			map_itr = dat->link_map.find (id);
			if (map_itr == dat->link_map.end ()) return (false);

			link_ptr = &dat->link_array [map_itr->second];

			if (Link_Dir ()) {
				dir = link_ptr->BA_Dir ();
			} else {
				dir = link_ptr->AB_Dir ();
			}
			if (dir < 0) return (false);

			id = dir;
			if (Use_Type ()) {
				Type (USE_ID);
			} else {
				Type (DIR_ID);
			}
			break;
		case NODE_ID:
			map_itr = dat->node_map.find (id);
			if (map_itr == dat->node_map.end ()) return (false);
			id = map_itr->second;
			break;
		case LOCATION_ID:
			map_itr = dat->location_map.find (id);
			if (map_itr == dat->location_map.end ()) return (false);
			id = map_itr->second;
			break;
		case PARKING_ID:
			map_itr = dat->parking_map.find (id);
			if (map_itr == dat->parking_map.end ()) return (false);
			id = map_itr->second;
			break;
		case ACCESS_ID:
			map_itr = dat->access_map.find (abs (id));
			if (map_itr == dat->access_map.end ()) return (false);

			if (id > 0) {
				Type (ACCESS_AB);
			} else {
				Type (ACCESS_BA);
			}
			id = map_itr->second;
			break;
		case ACCESS_AB:
		case ACCESS_BA:
			map_itr = dat->access_map.find (id);
			if (map_itr == dat->access_map.end ()) return (false);
			id = map_itr->second;
			break;
		case STOP_ID:
			map_itr = dat->stop_map.find (id);
			if (map_itr == dat->stop_map.end ()) return (false);
			id = map_itr->second;
			break;
		case ROUTE_ID:
			map_itr = dat->line_map.find (id);
			if (map_itr == dat->line_map.end ()) return (false);
			id = map_itr->second;
			break;
		default:
			return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	External_IDs
//---------------------------------------------------------

bool Plan_Data::External_IDs (void)
{
	if (!Trip_Data::External_IDs ()) return (false);

	if (Path_Problem ()) return (true);

	Plan_Leg_Itr itr;

	for (itr = begin (); itr != end (); itr++) {
		if (!itr->External_IDs ()) {
			exe->Warning (String ("Plan %d-%d-%d-%d Leg Type %s was Not Implemented") % Household () % 
				Person () % Tour () % Trip () % exe->ID_Code ((ID_Type) itr->Type ()));
			return (false);
		}
	}
	return (true);
}

bool Plan_Leg::External_IDs (void)
{
#ifdef CHECK
	int size;
#endif
	Dir_Data *dir_ptr;

	switch (Type ()) {
		case DIR_ID:
#ifdef CHECK
			size = (int) dat->dir_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			dir_ptr = &dat->dir_array [id];
			id = dat->link_array [dir_ptr->Link ()].Link ();

			if (dir_ptr->Dir () == 1) {
				Type (LINK_BA);
			} else {
				Type (LINK_AB);
			}
			break;
		case USE_ID:
#ifdef CHECK
			size = (int) dat->dir_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			dir_ptr = &dat->dir_array [id];
			id = dat->link_array [dir_ptr->Link ()].Link ();

			if (dir_ptr->Dir () == 1) {
				Type (USE_BA);
			} else {
				Type (USE_AB);
			}
			break;
		case LINK_ID:
#ifdef CHECK
			size = (int) dat->link_array.size ();
			if (abs (id) >= size) goto error_msg;
#endif
			if (id >= 0) {
				id = dat->link_array [id].Link ();
			} else {
				id = -dat->link_array [-id].Link ();
			}
			break;
		case LINK_AB:
		case USE_AB:
		case LINK_BA:
		case USE_BA:
#ifdef CHECK
			size = (int) dat->link_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			id = dat->link_array [id].Link ();
			break;
		case NODE_ID:
#ifdef CHECK
			size = (int) dat->node_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			id = dat->node_array [id].Node ();
			break;
		case LOCATION_ID:
#ifdef CHECK
			size = (int) dat->location_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			id = dat->location_array [id].Location ();
			break;
		case PARKING_ID:
#ifdef CHECK
			size = (int) dat->parking_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			id = dat->parking_array [id].Parking ();
			break;
		case ACCESS_ID:
#ifdef CHECK
			size = (int) dat->access_array.size ();
			if (abs (id) >= size) goto error_msg;
#endif
			if (id >= 0) {
				id = dat->access_array [id].Link ();
			} else {
				id = -dat->access_array [-id].Link ();
			}
			break;
		case ACCESS_AB:
		case ACCESS_BA:
#ifdef CHECK
			size = (int) dat->access_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			id = dat->access_array [id].Link ();
			break;
		case STOP_ID:
#ifdef CHECK
			size = (int) dat->stop_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			id = dat->stop_array [id].Stop ();
			break;
		case ROUTE_ID:
#ifdef CHECK
			size = (int) dat->line_array.size ();
			if (id < 0 || id >= size) goto error_msg;
#endif
			id = dat->line_array [id].Route ();
			break;
		default:
			return (false);
	}
	return (true);

#ifdef CHECK
error_msg:
	exe->Error (String ("Path_Leg::External_IDs %s index=%d vs %d") % exe->ID_Code ((ID_Type) Type ()) % id % size);
	return (false);
#endif
}
