//*********************************************************
//	Check_Network.cpp - validate the network coding
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Check_Network
//---------------------------------------------------------

void Validate::Check_Network (void)
{
	int num_errors = 0;
	int i, j, k, use_code, dir, index, in_off, out_off, anode, bnode, lane, min_lane, max_lane;
	int num_in, num_out, cap_in, cap_out, spd_in, spd_out;
	int cell_size, lanes, first_right, length, lane_code, *b_list = 0, *a_list = 0;
	bool lane_connect [20], flag, anode_flag, problem_flag;

	Dir_Data *dir_ptr, *to_ptr;
	Connect_Data *connect_ptr;
	Pocket_Data *pocket_ptr;
	Lane_Map_Array lane_map;
	Lane_Map_Itr lane_itr;
	Line_Itr line_itr;  
	Veh_Type_Itr veh_type_itr;
	Link_Itr link_itr;
	Link_Data *link_ptr;
	Node_Data *anode_ptr, *bnode_ptr;
	Integers node_list;

	Show_Message ("Checking Network Coding");
	Set_Progress ();

	problem_flag = (problem_node_flag || problem_coord_flag);

	if (problem_flag) {
		node_list.assign (node_array.size (), 0);
	}

	//---- set the vehicle cell size ----

	if (System_File_Flag (VEHICLE_TYPE)) {

		use_code = Use_Code ("CAR");
		cell_size = 0;

		for (veh_type_itr = veh_type_array.begin (); veh_type_itr != veh_type_array.end (); veh_type_itr++) {
			if ((veh_type_itr->Use () & use_code) != 0) {
				cell_size = veh_type_itr->Length ();
				break;
			}
		}
		if (cell_size == 0) {
			Warning ("Car Length is Zero");
		}
		Print (2, "Vehicle Cell Size = ") << UnRound (cell_size) << ((Metric_Flag ()) ? " meters" : " feet");
	} else {
		cell_size = Round (25.0);
	}

	//---- check links ----

	use_code = Use_Code ("CAR/TRUCK/BUS/RAIL");

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		length = link_itr->Length ();
		flag = false;

		if (length < cell_size) {
			Warning (String ("Link %d Length %.1lf is less than Cell Size %.1lf") % link_itr->Link () % UnRound (link_itr->Length ()) % UnRound (cell_size));
			flag = true;
		}

		//---- process each direction ----

		for (dir=0; dir < 2; dir++) {
			if (dir) {
				index = link_itr->BA_Dir ();
				anode = link_itr->Bnode ();
				bnode = link_itr->Anode ();
				in_off = link_itr->Boffset ();
				out_off = link_itr->Aoffset ();
			} else {
				index = link_itr->AB_Dir ();
				anode = link_itr->Anode ();
				bnode = link_itr->Bnode ();
				in_off = link_itr->Aoffset ();
				out_off = link_itr->Boffset ();
			}
			if (index < 0) continue;

			bnode_ptr = &node_array [bnode];
			if (problem_flag) {
				b_list = &node_list [bnode];
			}
			anode_ptr = &node_array [anode];
			if (problem_flag) {
				a_list = &node_list [anode];
			}

			dir_ptr = &dir_array [index];

			min_lane = dir_ptr->Left ();
			max_lane = dir_ptr->Lanes () + min_lane - 1;

			//---- check connections and lanes ----

			lanes = dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right ();
			first_right = dir_ptr->Left () + dir_ptr->Lanes ();

			if (lanes < 0 || lanes > 20) {
				Warning (String ("Link %d Direction %d has Too Many Lanes = %d") % link_itr->Link () % dir % lanes);
				if (problem_flag) *b_list = bnode_ptr->Node ();
				flag = true;
				continue;
			}

			//---- turn lanes ----

			for (i=0; i < lanes; i++) {
				if (i < dir_ptr->Left () || i >= first_right) {
					lane_connect [i] = true;
				} else {
					lane_connect [i] = false;
				}
			}

			for (j = dir_ptr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [j];

				if (pocket_ptr->Type () == LEFT_TURN) {
					for (k=0, i=dir_ptr->Left () - 1; k < pocket_ptr->Lanes (); k++, i--) {
						lane_connect [i] = false;
					}
				} else if (pocket_ptr->Type () == RIGHT_TURN) {
					for (k=0, i=first_right; k < pocket_ptr->Lanes (); k++, i++) {
						lane_connect [i] = false;
					}
				}
			}

			for (i = dir_ptr->First_Connect (); i >= 0; i = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [i];

				//---- check bottleneck locations ----

				if (connect_ptr->Type () == THRU) {
					num_in = connect_ptr->High_Lane () - connect_ptr->Low_Lane () + 1;
					num_out = connect_ptr->To_High_Lane () - connect_ptr->To_Low_Lane () + 1;

					to_ptr = &dir_array [connect_ptr->To_Index ()];
					
					link_ptr = &link_array [to_ptr->Link ()];

					cap_in = dir_ptr->Capacity ();
					cap_out = to_ptr->Capacity ();

					spd_in = dir_ptr->Speed ();
					spd_out = to_ptr->Speed ();

					if (num_in >= num_out + 2 || cap_in >= (cap_out * 3 / 2) || spd_in >= (spd_out * 3 / 2)) {
						Warning (String ("Link %d to Link %d has Bottleneck Conditions") % link_itr->Link () % link_ptr->Link ());
					}
				}

				//---- check lane connections ----

				Lane_Map (connect_ptr, lane_map);

				for (lane_itr = lane_map.begin (); lane_itr != lane_map.end (); lane_itr++) {
					lane = lane_itr->In_Lane ();
					lane_connect [lane] = true;

					if (lane < dir_ptr->Left ()) {
						for (j = dir_ptr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [j];

							if (pocket_ptr->Type () == LEFT_TURN) {
								if (lane >= dir_ptr->Left () - pocket_ptr->Lanes ()) {
									break;
								}
							}
						}
						if (j < 0) {
							Warning (String ("Link %d Direction %d Left Turn Connection Pocket Missing") % link_itr->Link () % dir);
							if (problem_flag) *b_list = bnode_ptr->Node ();
							flag = true;
						}
					} else if (lane > first_right) {
						for (j = dir_ptr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [j];

							if (pocket_ptr->Type () == RIGHT_TURN) {
								if (lane <= first_right + pocket_ptr->Lanes ()) {
									break;
								}
							}
						}
						if (j < 0) {
							Warning (String ("Link %d Direction %d Right Turn Connection Pocket Missing") % link_itr->Link () % dir);
							if (problem_flag) *b_list = bnode_ptr->Node ();
							flag = true;
						}
					}
				}
			}

			if (dir_ptr->First_Connect () >= 0) {
				for (i=0; i < lanes; i++) {
					if (!lane_connect [i]) {
						lane_code = Make_Lane_ID (dir_ptr, i);
						Warning (String ("Link %d Direction %d Lane %s has No Exit Connections") % link_itr->Link () % dir % Lane_ID_Code (lane_code));
						if (problem_flag) *b_list = bnode_ptr->Node ();
						flag = true;
					}
				}
			}

			//---- merge lanes -----

			for (i=0; i < lanes; i++) {
				if (i < dir_ptr->Left () || i >= first_right) {

					lane_connect [i] = true;
				} else {
					lane_connect [i] = false;
				}
			}

			for (j = dir_ptr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [j];

				if (pocket_ptr->Type () == LEFT_MERGE) {
					for (k=0, i=dir_ptr->Left () - 1; k < pocket_ptr->Lanes (); k++, i--) {
						lane_connect [i] = false;
					}
				} else if (pocket_ptr->Type () == RIGHT_MERGE) {
					for (k=0, i=first_right; k < pocket_ptr->Lanes (); k++, i++) {
						lane_connect [i] = false;
					}
				}
			}

			for (i = dir_ptr->First_Connect_From (); i >= 0; i = connect_ptr->Next_From ()) {
				connect_ptr = &connect_array [i];

				Lane_Map (connect_ptr, lane_map);

				for (lane_itr = lane_map.begin (); lane_itr != lane_map.end (); lane_itr++) {
					lane = lane_itr->Out_Lane ();
					lane_connect [lane] = true;

					if (lane < dir_ptr->Left ()) {
						for (j = dir_ptr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [j];

							if (pocket_ptr->Type () == LEFT_MERGE) {
								if (lane >= dir_ptr->Left () - pocket_ptr->Lanes ()) {
									break;
								}
							}
						}
						if (j < 0) {
							Warning (String ("Link %d Direction %d Left Merge Connection Pocket Missing") % link_itr->Link () % dir);
							if (problem_flag) *a_list = anode_ptr->Node ();
							flag = true;
						}
					} else if (lane > first_right) {
						for (j = dir_ptr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [j];

							if (pocket_ptr->Type () == RIGHT_MERGE) {
								if (lane <= first_right + pocket_ptr->Lanes ()) {
									break;
								}
							}
						}
						if (j < 0) {
							Warning (String ("Link %d Direction %d Right Merge Connection Pocket Missing") % link_itr->Link () % dir);
							if (problem_flag) *a_list = anode_ptr->Node ();
							flag = anode_flag = true;
						}
					}
				}
			}

			if (dir_ptr->First_Connect_From () >= 0) {
				for (i=0; i < lanes; i++) {
					if (!lane_connect [i]) {
						lane_code = Make_Lane_ID (dir_ptr, i);
						Warning (String ("Link %d Direction %d Lane %s has No Entry Connections") % link_itr->Link () % dir % Lane_ID_Code (lane_code));
						if (problem_flag) *a_list = anode_ptr->Node ();
						flag = true;
					}
				}
			}

			//---- initialize the traffic controls ----

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [index];

				if (bnode_ptr->Control () < 0) {
					if (dir_ptr->Sign () == STOP_SIGN || dir_ptr->Sign () == ALL_STOP) {
						connect_ptr->Control (STOP_GREEN);
					} else if (dir_ptr->Sign () == YIELD_SIGN) {
						connect_ptr->Control (PERMITTED_GREEN);
					} else {
						connect_ptr->Control (UNCONTROLLED);
					}
				} else {
					connect_ptr->Control (RED_LIGHT);
					if (link_itr->Type () == FREEWAY) {
						Warning (String ("Link %d is a Freeway with a Traffic Signal at Node %d") % link_itr->Link () % bnode_ptr->Node ());
						if (problem_flag) *b_list = bnode_ptr->Node ();
						flag = true;
					}
				}
			}
		}
		if (flag && problem_link_flag) {
			problem_link_file.File () << link_itr->Link () << endl;
		}
	}
	if (problem_link_flag) {
		problem_link_file.Close ();
	}

	//---- list problem nodes ----

	if (problem_node_flag) {
		Int_Itr itr;

		for (itr = node_list.begin (); itr != node_list.end (); itr++) {
			if (*itr > 0) problem_node_file.File () << *itr << endl;
		}
		problem_node_file.Close ();
	}
	if (problem_coord_flag) {
		Int_Itr itr;

		for (i=0, itr = node_list.begin (); itr != node_list.end (); itr++, i++) {
			if (*itr > 0) {
				anode_ptr = &node_array [i];
				problem_coord_file.File () << *itr << "\t" << UnRound (anode_ptr->X ()) << "\t" << UnRound (anode_ptr->Y ()) << endl;
			}
		}
		problem_coord_file.Close ();
	}
	End_Progress ();
	Print (1, "Number of Network Coding Errors = ") << num_errors;
}
