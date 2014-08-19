//*********************************************************
//	Read_Turn_Delays.cpp - read the turn volume file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Turn_Delays
//---------------------------------------------------------

void Data_Service::Read_Turn_Delays (Turn_Delay_File &file, Turn_Period_Array &turn_period_array)
{
	int i, num, start, end, count;
	double factor, p1, p2, share;
	Dtime time;

	Time_Periods *period_ptr;
	Int2_Map_Itr map_itr;
	Turn_Delay_Data turn_rec;
	Turn_Data *turn_ptr;
	Turn_Period *turn_period_ptr;
	Turn_Itr turn_itr;
	Turn_Period_Itr turn_period_itr;

	//---- store the turn data ----

	Show_Message (String ("Reading %s -- Record") % file.File_Type ());
	Set_Progress ();
	
	Initialize_Turn_Delays (file, turn_period_array);
	count = 0;

	while (file.Read ()) {
		Show_Progress ();

		turn_rec.Clear ();

		if (!Get_Turn_Delay_Data (file, turn_rec)) continue;

		map_itr = connect_map.find (Int2_Key (turn_rec.Dir_Index (), turn_rec.To_Index ()));

		if (map_itr == connect_map.end ()) continue;

		//---- get the time period ----
			
		period_ptr = turn_period_array.periods;

		if (period_ptr->Period_Range (turn_rec.Start (), turn_rec.End (), start, end)) {

			p1 = period_ptr->Period_Offset (start, turn_rec.Start ());
			p2 = period_ptr->Period_Offset (end, turn_rec.End ());
			if (p1 == p2) continue;

			//---- allocate the data to multiple time periods ----

			num = end - start;
			factor = 1.0 / (end - start + p2 - p1);

			for (i = start; i <= end; i++) {
				if (i == start && i == end) {
					share = 1.0;
				} else if (i == start) {
					share = (1.0 - p1) * factor;
				} else if (i == end) {
					share = p2 * factor;
				} else {
					share = factor;
				}
					
				//---- process the turn record ----

				turn_period_ptr = &turn_period_array [i];

				turn_ptr = turn_period_ptr->Data_Ptr (map_itr->second);

				turn_ptr->Add_Turn_Time (turn_rec.Turn () * share, turn_rec.Time ());
				count++;
			}
		}
	}
	End_Progress ();
	file.Close ();

	Break_Check (3);
	Print (2, String ("Number of %s Records = %d") % file.File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file.File_ID () % count);
	}
	Print (1, "Number of Turning Movement Records = ") << count;

	if (count > 0) System_Data_True (TURN_DELAY);
}

//---------------------------------------------------------
//	Initialize_Turn_Delays
//---------------------------------------------------------

void Data_Service::Initialize_Turn_Delays (Turn_Delay_File &file, Turn_Period_Array &turn_period_array)
{
	Required_File_Check (file, LINK);
	
	if (System_Data_Reserve (TURN_DELAY) == 0) return;

	if (turn_period_array.size () == 0) {
		if (Sum_Flow_Flag ()) {
			turn_period_array.Initialize (&sum_periods);
		} else {
			turn_period_array.Initialize (&time_periods);
		}
	}
}

//---------------------------------------------------------
//	Get_Turn_Delay_Data
//---------------------------------------------------------

bool Data_Service::Get_Turn_Delay_Data (Turn_Delay_File &file, Turn_Delay_Data &turn_rec)
{
	int link, to_link, node, nod;

	Link_Data *link_ptr, *to_link_ptr;
	Int_Map_Itr map_itr;

	//---- set the entry and exit link directions ----
		
	link = file.Link ();

	map_itr = link_map.find (link);

	if (map_itr == link_map.end ()) {
		Warning (String ("Turn Time Link %d was Not Found") % link);
		return (false);
	}
	link_ptr = &link_array [map_itr->second];

	to_link = file.To_Link ();

	map_itr = link_map.find (to_link);

	if (map_itr == link_map.end ()) {
		Warning (String ("Turn Time Link %d was Not Found") % to_link);
		return (false);
	}
	to_link_ptr = &link_array [map_itr->second];

	node = file.Node ();

	if (node != 0) {
		map_itr = node_map.find (node);

		if (map_itr == node_map.end ()) {
			Warning (String ("Turn Time Node %d was Not Found") % node);
			return (false);
		}
		nod = map_itr->second;

		if (link_ptr->Anode () == nod) {
			turn_rec.Dir_Index (link_ptr->BA_Dir ());
		} else if (link_ptr->Bnode () == nod) {
			turn_rec.Dir_Index (link_ptr->AB_Dir ());
		} else {
			Warning (String ("Turn Time Node %d Not Found on Link %d") % node % link);
			return (false);
		}
		if (turn_rec.Dir_Index () < 0) {
			Warning (String ("Turn Time Link %d Node %d Direction") % link % node);
			return (false);
		}

		if (to_link_ptr->Anode () == nod) {
			turn_rec.To_Index (link_ptr->AB_Dir ());
		} else if (link_ptr->Bnode () == nod) {
			turn_rec.To_Index (link_ptr->BA_Dir ());
		} else {
			Warning (String ("Turn Time Node %d Not Found on Link %d") % node % to_link);
			return (false);
		}
		if (turn_rec.To_Index () < 0) {
			Warning (String ("Turn Time Link %d Node %d Direction") % to_link % node);
			return (false);
		}

	} else {
		if (link_ptr->Bnode () == to_link_ptr->Anode ()) {
			turn_rec.Dir_Index (link_ptr->AB_Dir ());
			turn_rec.To_Index (to_link_ptr->AB_Dir ());
		} else if (link_ptr->Anode () == to_link_ptr->Bnode ()) {
			turn_rec.Dir_Index (link_ptr->BA_Dir ());
			turn_rec.To_Index (to_link_ptr->BA_Dir ());	
		} else if (link_ptr->Bnode () == to_link_ptr->Bnode ()) {
			turn_rec.Dir_Index (link_ptr->AB_Dir ());
			turn_rec.To_Index (to_link_ptr->BA_Dir ());
		} else if (link_ptr->Anode () == to_link_ptr->Anode ()) {
			turn_rec.Dir_Index (link_ptr->BA_Dir ());
			turn_rec.To_Index (to_link_ptr->AB_Dir ());
		} else {
			Warning (String ("Turn Time Connection Between Link %d and %d") % link % to_link);
			return (false);
		}
		if (turn_rec.Dir_Index () < 0) {
			Warning (String ("Turn Time Link %d Direction") % link);
			return (false);
		}
		if (turn_rec.To_Index () < 0) {
			Warning (String ("Turn Time Link %d Direction") % to_link);
			return (false);
		}
	}

	//---- optional fields ----

	turn_rec.Start (file.Start ());
	turn_rec.End (file.End ());
	if (turn_rec.End () == 0) turn_rec.End (Model_End_Time ());

	turn_rec.Turn (file.Turn ());
	turn_rec.Time (file.Time ());
	return (true);
}
