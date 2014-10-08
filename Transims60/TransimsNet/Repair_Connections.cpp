//*********************************************************
//	Repair_Connections.cpp - repair the lane connectivity
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Repair_Connections
//---------------------------------------------------------

void TransimsNet::Repair_Connections (void)
{
	int i, j, k, index, to_index, bear_in, bear_out, change, type, dir_index;
	int num_left, num_lmerge, num_lsplit, num_thru, num_rmerge, num_rsplit, num_right;
	int lns_left, lns_lmerge, lns_lsplit, lns_thru, lns_rmerge, lns_rsplit, lns_right;
	int in_left1, in_left2, in_right1, in_right2, in_thru1, in_thru2, left_lanes, right_lanes;
	int out_left1, out_left2, out_right1, out_right2, out_thru1, out_thru2;
	int in_low, in_high, out_low, out_high, lanes, first_right, lane_code, num_repair;
	bool lane_connect [20], flag;
	bool approach_lane_flag = false;

	Dir_Data *to_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Dir_Itr dir_itr;
	Int_Map bear_map;
	Int_Map_Itr map_itr;
	Connect_Data *connect_ptr;
	Pocket_Data *pocket_ptr;
	Approach_Link_Map_Itr approach_itr;
	Lane_Map_Array lane_map;
	Lane_Map_Itr lane_itr;

	num_repair = 0;

	Show_Message ("Repair Lane Connectivity -- Record");
	Set_Progress ();

	//---- create connection records between links ----

	for (dir_index = 0, dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, dir_index++) {
		Show_Progress ();

		if (dir_itr->First_Connect () < 0) continue;

		link_ptr = &link_array [dir_itr->Link ()];
		if (dir_itr->Dir () == 1) {
			node_ptr = &node_array [link_ptr->Anode ()];
		} else {
			node_ptr = &node_array [link_ptr->Bnode ()];
		}
		if (update_node_flag && !update_node_range.In_Range (node_ptr->Node ())) continue;

		//---- check connections and lanes ----

		lanes = dir_itr->Lanes () + dir_itr->Left () + dir_itr->Right ();
		first_right = dir_itr->Left () + dir_itr->Lanes ();

		//---- set lane flags ----

		for (i=0; i < lanes; i++) {
			if (i < dir_itr->Left () || i >= first_right) {
				lane_connect [i] =  true;
			} else {
				lane_connect [i] = false;
			}
		}

		for (j = dir_itr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
			pocket_ptr = &pocket_array [j];

			if (pocket_ptr->Type () == LEFT_TURN) {
				for (k=0, i=dir_itr->Left () - 1; k < pocket_ptr->Lanes (); k++, i--) {
					lane_connect [i] = false;
				}
			} else if (pocket_ptr->Type () == RIGHT_TURN) {
				for (k=0, i=first_right; k < pocket_ptr->Lanes (); k++, i++) {
					lane_connect [i] = false;
				}
			}
		}

		for (i = dir_itr->First_Connect (); i >= 0; i = connect_ptr->Next_Index ()) {
			connect_ptr = &connect_array [i];

			if (connect_ptr->Low_Lane () < 0 || connect_ptr->High_Lane () < 0) continue;

			Lane_Map (connect_ptr, lane_map);

			for (lane_itr = lane_map.begin (); lane_itr != lane_map.end (); lane_itr++) {
				lane_connect [lane_itr->In_Lane ()] = true;
			}
		}

		//---- approach lane type ranges ----

		in_thru1 = dir_itr->Left ();
		in_thru2 = dir_itr->Left () + dir_itr->Lanes () - 1;

		in_left1 = in_left2 = in_right1 = in_right2 = -1;

		for (index = dir_itr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
			pocket_ptr = &pocket_array [index];

			if (pocket_ptr->Type () == LEFT_TURN) {
				in_left2 = dir_itr->Left () - 1;
				in_left1 = dir_itr->Left () - pocket_ptr->Lanes ();
			} else if (pocket_ptr->Type () == RIGHT_TURN) {
				in_right1 = dir_itr->Left () + dir_itr->Lanes ();
				in_right2 = in_right1 + pocket_ptr->Lanes () - 1;
			}
		}

		//---- get approach link for shared lanes ----

		if (approach_flag) {
			approach_itr = approach_map.find (dir_index);
			if (approach_itr != approach_map.end ()) {
				lanes = approach_itr->second.Left () + approach_itr->second.Left_Thru () + approach_itr->second.Thru () + approach_itr->second.Right_Thru () + 
					approach_itr->second.Right () + approach_itr->second.Left_Merge () + approach_itr->second.Right_Merge ();
				approach_lane_flag = (lanes > 0);
			} else {
				approach_lane_flag = false;
			}
		}

		//---- sort the link connections ----

		bear_in = dir_itr->Out_Bearing ();
		bear_map.clear ();

		for (index = dir_itr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
			connect_ptr = &connect_array [index];
			to_index = connect_ptr->To_Index ();
			
			to_ptr = &dir_array [to_index];
			bear_out = to_ptr->In_Bearing ();

			change = compass.Change (bear_in, bear_out);
			bear_map.insert (Int_Map_Data (change, index));
		}

		//---- identify the movement groups ----

		num_left = num_lmerge = num_lsplit = num_thru = num_rsplit = num_rmerge = num_right = 0;
		lns_left = lns_lmerge = lns_lsplit = lns_thru = lns_rsplit = lns_rmerge = lns_right = 0;

		for (map_itr = bear_map.begin (); map_itr != bear_map.end (); map_itr++) {
			index = map_itr->second;
			connect_ptr = &connect_array [index];

			to_index = connect_ptr->To_Index ();
			to_ptr = &dir_array [to_index];

			left_lanes = right_lanes = 0;

			for (index = to_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [index];

				if (pocket_ptr->Type () == LEFT_MERGE) {
					if (pocket_ptr->Lanes () > left_lanes) left_lanes = pocket_ptr->Lanes ();
				} else if (pocket_ptr->Type () == RIGHT_MERGE) {
					if (pocket_ptr->Lanes () > right_lanes) right_lanes = pocket_ptr->Lanes ();
				}
			}
			type = connect_ptr->Type ();

			switch (type) {
				case THRU:
					num_thru++;
					lns_thru += to_ptr->Lanes ();
					break;
				case LEFT:
					num_left++;
					lns_left += to_ptr->Lanes () + left_lanes;
					break;
				case L_SPLIT:
					num_lsplit++;
					lns_lsplit += to_ptr->Lanes () + right_lanes;
					break;
				case RIGHT:
					num_right++;
					lns_right += to_ptr->Lanes () + right_lanes;
					break;
				case R_SPLIT:
					num_rsplit++;
					lns_rsplit += to_ptr->Lanes () + left_lanes;
					break;
				case L_MERGE:
					num_lmerge++;
					lns_lmerge += to_ptr->Lanes () + left_lanes;
					break;
				case R_MERGE:
					num_rmerge++;
					lns_rmerge += to_ptr->Lanes () + right_lanes;
					break;
				default:
					break;
			}
		}

		//---- allocate lanes ----

		left_lanes = right_lanes = 0;

		for (map_itr = bear_map.begin (); map_itr != bear_map.end (); map_itr++) {
			index = map_itr->second;
			connect_ptr = &connect_array [index];

			to_index = connect_ptr->To_Index ();
			to_ptr = &dir_array [to_index];

			//---- departure lane type ranges ----

			out_thru1 = to_ptr->Left ();
			out_thru2 = to_ptr->Left () + to_ptr->Lanes () - 1;

			out_left1 = out_left2 = out_right1 = out_right2 = -1;

			for (index = to_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [index];

				if (pocket_ptr->Type () == LEFT_MERGE) {
					out_left2 = to_ptr->Left () - 1;
					out_left1 = to_ptr->Left () - pocket_ptr->Lanes ();
				} else if (pocket_ptr->Type () == RIGHT_MERGE) {
					out_right1 = to_ptr->Left () + to_ptr->Lanes ();
					out_right2 = out_right1 + pocket_ptr->Lanes () - 1;
				}
			}
			type = connect_ptr->Type ();

			switch (type) {
				case THRU:
					in_low = in_thru1;
					in_high = in_thru2;
					out_low = out_thru1;
					out_high = out_thru2;
					if (approach_lane_flag && approach_itr->second.Thru () < (in_high - in_low + 1)) {
						if (approach_itr->second.Left () > 0) {
							if (in_left2 < 0) {
								in_low = MIN ((in_thru1 + approach_itr->second.Left ()), in_thru2);
							} else if (approach_itr->second.Left () > (in_left2 - in_left1 + 1)) {
								in_low = MIN ((in_thru1 + approach_itr->second.Left () - (in_left2 - in_left1 + 1)), in_thru2);
							}
						}
						if (approach_itr->second.Thru () < (in_high - in_low + 1)) {
							if (approach_itr->second.Right () > 0) {
								if (in_right2 < 0) {
									in_high = MAX ((in_thru2 - approach_itr->second.Right ()), in_thru1);
								} else if (approach_itr->second.Right () > (in_right2 - in_right1 + 1)) {
									in_high = MAX ((in_thru2 - approach_itr->second.Right () - (in_right2 - in_right1 + 1)), in_thru1);
								}
							}
						}
					}
					break;
				case LEFT:
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					if (++left_lanes > 1) {
						if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = in_thru1;
							in_high= MIN ((in_thru1 + 1), in_thru2);
						} else {
							in_low = in_high = in_thru1;
						}
						if (lns_thru < dir_itr->Lanes ()) {
							in_thru1 = MIN ((in_thru1 + 1), in_thru2);
						}
					} else if (in_left1 >= 0) {
						in_low = in_left1;

						if (approach_lane_flag && (num_thru > 0 || approach_itr->second.Thru () == 0)) {
							if (approach_itr->second.Left_Thru () > 0) {
								in_high = in_thru1;
							} else {
								in_high = in_left2;
							}
						} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_high = in_thru1;
						} else if (lns_thru == 0) {
							in_high = in_thru1;
						} else {
							in_high = in_left2;
						}
					} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
						in_low = in_thru1;

						if (approach_lane_flag && (approach_itr->second.Left_Thru () > 0 || 
							(num_thru == 0 && approach_itr->second.Thru () > 0))) {
							in_high = MIN ((in_thru1 + 1), in_thru2);
						} else {
							in_high = in_low;
						}
					} else {
						in_low = in_high = in_thru1;
					}
					break;
				case RIGHT:
					if (out_right1 >= 0) {
						out_low = out_right1;
						out_high = out_right1;
					} else {
						out_low = MAX ((out_thru2 - 1), out_thru1);
						out_high = out_thru2;
					}
					if (++right_lanes < num_right) {
						in_high = in_thru2;

						if (approach_lane_flag && (num_thru > 0 || approach_itr->second.Thru () == 0)) {
							if (approach_itr->second.Right_Thru () > 0) {
								in_low = MAX ((in_thru2 - 1), in_thru1);
							} else {
								in_low = in_thru2;
							}
						} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = MAX ((in_thru2 - 1), in_thru1);
						} else {
							in_low = in_thru2;
						}
					} else if (in_right1 >= 0) {
						in_high = in_right2;

						if (approach_lane_flag && (num_thru > 0 || approach_itr->second.Thru () == 0)) {
							if (approach_itr->second.Right_Thru () > 0) {
								in_low = in_thru2;
							} else {
								in_low = in_right1;
							}
						} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = in_thru2;
						} else if (lns_rsplit > 1 || lns_thru == 0) {
							in_low = in_thru2;
						} else {
							in_low = in_right1;
						}
					} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
						in_high = in_thru2;

						if (approach_lane_flag && (num_thru > 0 || approach_itr->second.Thru () == 0)) {
							if (approach_itr->second.Right_Thru () > 0 && approach_itr->second.Right () > 0) {
								in_low = MAX ((in_thru2 - 1), in_thru1);
							} else {
								in_low = in_thru2;
							}
						} else {
							in_low = MAX ((in_thru2 - 1), in_thru1);
						}
					} else {
						in_low = in_high = in_thru2;
					}
					break;
				case L_SPLIT:
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					if (num_left > 0) {
						if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = in_thru1;
							in_high = MIN ((in_thru1 + 1), in_thru2);
						} else {
							in_low = in_high = in_thru1;
						}
					} else if (in_left1 >= 0) {
						in_low = in_left1;
						if (lns_lsplit > 1) {
							in_high = in_thru1;
						} else {
							in_high = in_left2;
						}
					} else {
						in_low = in_high = in_thru1;
					}
					break;
				case R_SPLIT:
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					if (num_right > 0) {
						if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = MAX ((in_thru2 - 1), in_thru1);
							in_high = in_thru2;
						} else {
							in_low = in_high = in_thru2;
						}
					} else if (in_right1 >= 0) {
						if (lns_rsplit > 1) {
							in_low = in_thru2;
						} else {
							in_low = in_right1;
						}
						in_high = in_right2;
					} else if (lns_rsplit > 1 && lns_thru < dir_itr->Lanes ()) {
						in_low = MAX ((in_thru2 - 1), in_thru1);
						in_high = in_thru2;
					} else {
						in_low = in_high = in_thru2;
					}
					break;
				case L_MERGE:
					if (num_thru > 0) {
						if (in_left1 >= 0) {
							in_low = in_left1;
							in_high = in_left2;
						} else {
							in_low = in_high = in_thru1;
						}
					} else {
						if (in_left1 >= 0) {
							in_low = in_left1;
						} else {
							in_low = in_thru1;
						}
						if (in_right1 >= 0) {
							in_high = in_right2;
						} else {
							in_high = in_thru2;
						}
					}
					if (out_left1 >= 0) {
						out_low = out_left1;
						if (lns_lmerge > 2) {
							out_high = out_thru1;
						} else {
							out_high = out_left2;
						}
					} else {
						out_low = out_thru1;
						if (lns_lmerge > 2) {
							out_high = MIN ((out_thru1 + 1), out_thru2);
						} else {
							out_high = out_thru1;
						}
					}
					break;
				case R_MERGE:
					if (num_thru > 0) {
						if (in_left1 >= 0) {
							in_low = in_left1;
							in_high = in_left2;
						} else {
							in_low = in_high = in_thru1;
						}
					} else {
						if (in_left1 >= 0) {
							in_low = in_left1;
						} else {
							in_low = in_thru1;
						}
						if (in_right1 >= 0) {
							in_high = in_right2;
						} else {
							in_high = in_thru2;
						}
					}
					if (out_right1 >= 0) {
						if (lns_rmerge > 2) {
							out_low = out_thru2;
						} else {
							out_low = out_right1;
						}
						out_high = out_right1;
					} else {
						if (lns_rmerge > 2) {
							out_low = MAX ((out_thru2 - 1), out_thru1);
						} else {
							out_low = out_thru2;
						}
						out_high = out_thru2;
					}
					break;
				case UTURN:
					if (in_left1 >= 0) {
						in_low = in_left1;
						in_high = in_left2;
					} else {
						in_low = in_high = in_thru1;
					}
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					break;
				default:
					in_low = in_high = out_low = out_high = 0;
					break;
			}

			//---- update the lane ranges ----

			if (lane_connect [in_low] == false || lane_connect [in_high] == false || 
				connect_ptr->Low_Lane () < in_low || connect_ptr->High_Lane () > in_high ||
				connect_ptr->Low_Lane () > in_high || connect_ptr->High_Lane () < in_low) {

				if (lane_connect [in_low] == false || connect_ptr->Low_Lane () < in_low || connect_ptr->Low_Lane () > in_high) {
					connect_ptr->Low_Lane (in_low);
					lane_connect [in_low] = true;
				}
				if (lane_connect [in_high] == false || connect_ptr->High_Lane () < in_low || connect_ptr->High_Lane () > in_high) {
					connect_ptr->High_Lane (in_high);
					lane_connect [in_high] = true;
				}
				connect_ptr->To_Low_Lane (out_low);
				connect_ptr->To_High_Lane (out_high);
				num_repair++;
			} else if (connect_ptr->To_Low_Lane () < out_low || connect_ptr->To_High_Lane () > out_high ||
				connect_ptr->To_Low_Lane () > out_high || connect_ptr->To_High_Lane () < out_low) {

				if (connect_ptr->To_Low_Lane () < out_low || connect_ptr->To_Low_Lane () > out_high) {
					connect_ptr->To_Low_Lane (out_low);
				}
				if (connect_ptr->To_High_Lane () < out_low || connect_ptr->To_High_Lane () > out_high) {
					connect_ptr->To_High_Lane (out_high);
				}
				num_repair++;
			}
			if (connect_ptr->Type () == THRU && 
				(connect_ptr->Low_Lane () > in_low || connect_ptr->High_Lane () < in_high || 
				connect_ptr->To_Low_Lane () > out_low || connect_ptr->To_High_Lane () < out_high)) {

				num_repair++;
				connect_ptr->Low_Lane (in_low);
				connect_ptr->High_Lane (in_high);
				connect_ptr->To_Low_Lane (out_low);
				connect_ptr->To_High_Lane (out_high);
			}
		}

		//---- check if repairs are needed ----

		for (i=0; i < lanes; i++) {
			if (lane_connect [i] == false) {
				lane_code = Make_Lane_ID (&(*dir_itr), i);
				Warning (String ("Link %d Direction %d Exit Lane %s was Not Repaired") % link_ptr->Link () % dir_itr->Dir () % Lane_ID_Code (lane_code));
			}
		}
	}

	//---- check the entry lanes ----

	for (dir_index = 0, dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, dir_index++) {
		Show_Progress ();

		if (dir_itr->First_Connect_From () < 0) continue;

		link_ptr = &link_array [dir_itr->Link ()];
		if (dir_itr->Dir () == 1) {
			node_ptr = &node_array [link_ptr->Bnode ()];
		} else {
			node_ptr = &node_array [link_ptr->Anode ()];
		}
		if (update_node_flag && !update_node_range.In_Range (node_ptr->Node ())) continue;

		//---- check connections and lanes ----

		lanes = dir_itr->Lanes () + dir_itr->Left () + dir_itr->Right ();
		first_right = dir_itr->Left () + dir_itr->Lanes ();

		//---- set lane flags ----

		for (i=0; i < lanes; i++) {
			if (i < dir_itr->Left () || i >= first_right) {
				lane_connect [i] =  true;
			} else {
				lane_connect [i] = false;
			}
		}

		for (j = dir_itr->First_Pocket (); j >= 0; j = pocket_ptr->Next_Index ()) {
			pocket_ptr = &pocket_array [j];

			if (pocket_ptr->Type () == LEFT_MERGE) {
				for (k=0, i=dir_itr->Left () - 1; k < pocket_ptr->Lanes (); k++, i--) {
					lane_connect [i] = false;
				}
			} else if (pocket_ptr->Type () == RIGHT_MERGE) {
				for (k=0, i=first_right; k < pocket_ptr->Lanes (); k++, i++) {
					lane_connect [i] = false;
				}
			}
		}

		for (i = dir_itr->First_Connect_From (); i >= 0; i = connect_ptr->Next_From ()) {
			connect_ptr = &connect_array [i];

			if (connect_ptr->Low_Lane () < 0 || connect_ptr->High_Lane () < 0) continue;

			Lane_Map (connect_ptr, lane_map);

			for (lane_itr = lane_map.begin (); lane_itr != lane_map.end (); lane_itr++) {
				lane_connect [lane_itr->Out_Lane ()] = true;
			}
		}

		//---- check if repairs are needed ----

		for (i=0; i < lanes; i++) {
			if (lane_connect [i] == false) break;
		}
		if (i == lanes) continue;

		for (i = dir_itr->First_Connect_From (); i >= 0; i = connect_ptr->Next_From ()) {
			connect_ptr = &connect_array [i];

			Lane_Map (connect_ptr, lane_map);

			type = connect_ptr->Type ();
			flag = false;

			out_low = connect_ptr->To_Low_Lane ();
			out_high = connect_ptr->To_High_Lane ();

			switch (type) {
				case THRU:
					for (j=dir_itr->Left (); j < first_right; j++) {
						if (lane_connect [j] == false) {
							if (j < out_low) out_low = j;
							if (j > out_high) out_high = j;
							lane_connect [j] = true;
							flag = true;
						}
					}
					break;
				case L_SPLIT:
					for (j=dir_itr->Left (); j < lanes; j++) {
						if (lane_connect [j] == false) {
							if (j < out_low) out_low = j;
							if (j > out_high) out_high = j;
							lane_connect [j] = true;
							flag = true;
						}
					}
					break;
				case R_SPLIT:
					for (j=0; j < first_right; j++) {
						if (lane_connect [j] == false) {
							if (j < out_low) out_low = j;
							if (j > out_high) out_high = j;
							lane_connect [j] = true;
							flag = true;
						}
					}
					break;
				case LEFT:
				case L_MERGE:
					out_left1 = MIN (dir_itr->Left () + 1, lanes);;

					for (j=0; j < out_left1; j++) {
						if (lane_connect [j] == false) {
							if (j < out_low) out_low = j;
							if (j > out_high) out_high = j;
							lane_connect [j] = true;
							flag = true;
						}
					}
					break;
				case RIGHT:
				case R_MERGE:
					out_right1 = MAX (first_right - 1, 0);;

					for (j=out_right1; j < lanes; j++) {
						if (lane_connect [j] == false) {
							if (j < out_low) out_low = j;
							if (j > out_high) out_high = j;
							lane_connect [j] = true;
							flag = true;
						}
					}
					break;
				default:
					break;
			}
			if (flag) {
				connect_ptr->To_Low_Lane (out_low);
				connect_ptr->To_High_Lane (out_high);
				num_repair++;
			}
		}

		//---- check if repairs are needed ----

		for (i=0; i < lanes; i++) {
			if (lane_connect [i] == false) {
				lane_code = Make_Lane_ID (&(*dir_itr), i);
				Warning (String ("Link %d Direction %d Entry Lane %s was Not Repaired") % link_ptr->Link () % dir_itr->Dir () % Lane_ID_Code (lane_code));
			}
		}
	}
	End_Progress ();

	Print (1);
	Write (1, "Number of Connection Repairs = ") << num_repair;
	Show_Message (1);
}
