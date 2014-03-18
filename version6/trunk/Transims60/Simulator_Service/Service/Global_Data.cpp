//*********************************************************
//	Global_Data.cpp - simulation service data preparation
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Global_Data
//---------------------------------------------------------

void Simulator_Service::Global_Data (void)
{
	int i, j, k, n, use_code, dir, index, in_off, out_off, bnode, lane, min_lane, max_lane;
	int method, transfer, change, record;
	
	int length, offset, *list, c0, c1, bear1, bear2, next, max_cell;
	double factor;
	bool flag;

	Dtime hour = Dtime (1, HOURS);

	Link_Data *link_ptr;
	Dir_Data *dir_ptr, *app_ptr;
	Node_Itr node_itr;
	Connect_Data *connect_ptr, *cross_ptr;
	Sim_Connection *sim_con_ptr;
	Pocket_Data *pocket_ptr;
	Lane_Map_Array lane_map;
	Lane_Map_Itr lane_itr;

	Veh_Type_Itr type_itr;
	Link_Itr link_itr;
	Node_Data *node_ptr;
	Location_Itr loc_itr;
	Parking_Itr park_itr;
	Sim_Park_Data park_rec, *sim_park_ptr;
	Sim_Dir_Data sim_dir_rec, *sim_dir_ptr;
	Sim_Connection sim_con_rec;
	Sim_Signal_Data sim_signal_rec;
	Int2_Key mpi_key;


	//---- set the vehicle cell size ----

	use_code = Use_Code ("CAR");

	if (param.cell_size == 0) {
		for (type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++) {
			if ((type_itr->Use () & use_code) != 0) {
				param.cell_size = type_itr->Length ();
				break;
			}
		}
		if (param.cell_size == 0) {
			Error ("Car Length is Zero");
		}
		Print (2, "Vehicle Cell Size = ") << UnRound (param.cell_size) << ((Metric_Flag ()) ? " meters" : " feet");
	}
	param.half_cell = param.cell_size / 2;

	//---- create transit vehicles -----

	if (param.transit_flag) {
		Transit_Vehicles ();
	} else if (num_travelers > 0) {

		//---- reserve memory ----

		sim_travel_array.reserve (num_travelers);
		sim_veh_array.reserve (2 * num_travelers);
	}

	//---- convert the distance/speed parameters to cells ----

	param.lane_change_levels = 10;
	param.change_priority = param.lane_change_levels / 3;
	param.connect_lane_weight = 4;
	param.lane_use_weight = 4;

	//---- allocate simulation network memory ----

	sim_dir_array.assign (dir_array.size (), sim_dir_rec);
	sim_connection.assign (connect_array.size (), sim_con_rec);
	sim_signal_array.assign (signal_array.size (), sim_signal_rec);
	sim_park_array.assign (parking_array.size (), park_rec);

	node_link.assign (sim->node_array.size (), -1);
	link_list.assign (sim->dir_array.size (), -1);

	//---- initialize sim_dir_data ----

	use_code = Use_Code ("CAR/TRUCK/BUS/RAIL");
	transfer = 0;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {

		if ((link_itr->Use () & use_code) == 0) continue;

		//---- process each direction ----

		for (dir=0; dir < 2; dir++) {

			if (dir) {
				index = link_itr->BA_Dir ();
				bnode = link_itr->Anode ();
				in_off = link_itr->Boffset ();
				out_off = link_itr->Aoffset ();
			} else {
				index = link_itr->AB_Dir ();
				bnode = link_itr->Bnode ();
				in_off = link_itr->Aoffset ();
				out_off = link_itr->Boffset ();
			}
			if (index < 0) continue;

			list = &node_link [bnode];
			link_list [index] = *list;
			*list = index;

			node_ptr = &node_array [link_itr->Anode ()];
			method = param.subarea_method.Best (node_ptr->Subarea ());
			node_ptr->Method (method);

			dir_ptr = &dir_array [index];

			min_lane = dir_ptr->Left ();
			max_lane = dir_ptr->Lanes () + min_lane - 1;
			length = link_itr->Length ();

			max_cell = MAX (((length + param.cell_size - 1) / param.cell_size), 1);

			sim_dir_ptr = &sim_dir_array [index];

			sim_dir_ptr->Speed (dir_ptr->Speed ());
			sim_dir_ptr->Dir (dir);
			sim_dir_ptr->Type (link_itr->Type ());
			sim_dir_ptr->Turn (false);

			sim_dir_ptr->Length (length);
			sim_dir_ptr->In_Offset (in_off);
			sim_dir_ptr->Setback (out_off);
			sim_dir_ptr->Lanes (dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right ());
			sim_dir_ptr->Max_Cell (max_cell);
			sim_dir_ptr->Method (method);
			sim_dir_ptr->Subarea (node_ptr->Subarea ());

			sim_dir_ptr->Use_Type (LIMIT);
			sim_dir_ptr->Use (link_itr->Use ());
			sim_dir_ptr->Min_Veh_Type (-1);
			sim_dir_ptr->Max_Veh_Type (0);
			sim_dir_ptr->Min_Traveler (0);
			sim_dir_ptr->Max_Traveler (0);
			sim_dir_ptr->First_Use (-1);

			sim_dir_ptr->Make_Cells ();

			sim_dir_ptr->In_Cell (MIN (((in_off + param.cell_size - 1) / param.cell_size), max_cell));
			sim_dir_ptr->Out_Cell (MAX (MIN (((length - out_off) / param.cell_size), max_cell), 0));

			//sim_dir_ptr->Max_Flow (DTOI ((dir_ptr->Capacity () * param.step_size * 100.0) / (hour * dir_ptr->Lanes ())));
			//sim_dir_ptr->Density ((int) (length * 3600.0 * dir_ptr->Capacity () / (dir_ptr->Lanes () * dir_ptr->Speed ()) + 0.5));
			//sim_dir_ptr->Max_Move (MAX (DTOI ((double) length * param.step_size / dir_ptr->Time0 ()), 1));

			//capacity = MAX (Round ((length + param.half_cell) / param.cell_size), 1);

			//---- initialize the lane data and capacity ----

			//if (sim_dir_ptr->Method () == MACROSCOPIC) {
			//	lanes = (dir_ptr->First_Lane_Use () >= 0) ? 2 : 1;
			//	sim_dir_ptr->assign (lanes, sim_lane_data);
			//} else if (sim_dir_ptr->Method () == MESOSCOPIC) {
			//	sim_dir_ptr->assign (sim_dir_ptr->Lanes (), sim_lane_data);
			//}

			//---- apply method specific processing ----

			if (sim_dir_ptr->Method () == NO_SIMULATION) {
				No_Sim_Flag (true);

			} else if (sim_dir_ptr->Method () == MACROSCOPIC) {
				//sim_lane_ptr = sim_dir_ptr->Lane (0);
				//sim_lane_ptr->Capacity (dir_ptr->Lanes () * capacity);
				//sim_lane_ptr->Low_Lane (min_lane);
				//sim_lane_ptr->High_Lane (max_lane);

			} else if (sim_dir_ptr->Method () == MESOSCOPIC) {

				//---- initialize the connection array ----

				for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [index];
					sim_con_ptr = &sim_connection [index];

					Lane_Map (connect_ptr, *sim_con_ptr);
				}

				//---- set the thru link and lane for each entry lane ----

				for (index = dir_ptr->First_Connect_From (); index >= 0; index = connect_ptr->Next_From ()) {
					connect_ptr = &connect_array [index];
					
					app_ptr = &dir_array [connect_ptr->Dir_Index ()];
					if ((link_array [app_ptr->Link ()].Use () & use_code) == 0) continue;

					Lane_Map (connect_ptr, lane_map);

					for (lane_itr = lane_map.begin (); lane_itr != lane_map.end (); lane_itr++) {
						if (lane_itr->In_Thru () && lane_itr->Out_Thru ()) {
							//sim_lane_ptr = sim_dir_ptr->Lane (lane_itr->Out_Lane ());
							//if (sim_lane_ptr->Thru_Link () < 0) {
							//	sim_lane_ptr->Thru_Lane (lane_itr->In_Lane ());
							//	sim_lane_ptr->Thru_Link (connect_ptr->Dir_Index ());
							//}
						}
					}
				}
			}

			//---- initialize the pocket lanes and access restrictions ----

			offset = sim_dir_ptr->Out_Offset ();
			max_lane = sim_dir_ptr->Lanes () - dir_ptr->Right ();

			for (i=0; i < sim_dir_ptr->Lanes (); i++) {
				if (i < dir_ptr->Left () || i >= max_lane) {
					for (j=0; j <= sim_dir_ptr->Max_Cell (); j++) {
						sim_dir_ptr->Set (i, j, -1);
					}
				}
			}

			for (index = dir_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [index];

				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == RIGHT_TURN) {
					k = length;
					j = MIN (pocket_ptr->Offset (), offset);
					factor = param.turn_factor;
				} else if (pocket_ptr->Type () == LEFT_MERGE || pocket_ptr->Type () == RIGHT_MERGE) {
					j = 0;
					k = MIN (pocket_ptr->Length (), length);
					factor = param.merge_factor;
				} else {
					j = pocket_ptr->Offset ();
					k = MIN ((pocket_ptr->Offset () + pocket_ptr->Length ()), length);
					factor = param.other_factor;
				}
				if (j < 0) j = 0;

				j = j / param.cell_size;
				k = (k + param.cell_size - 1) / param.cell_size;

				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == LEFT_MERGE) {
					lane = dir_ptr->Left () - pocket_ptr->Lanes ();
				} else {
					lane = dir_ptr->Left () + dir_ptr->Lanes ();
				}

				for (n=0; n < pocket_ptr->Lanes (); n++, lane++) {
					for (i=j; i <= k; i++) {
						sim_dir_ptr->Set (lane, i, 0);
					}
				}
			}

			//---- initialize the traffic controls ----

			node_ptr = &node_array [bnode];

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [index];

				if (node_ptr->Control () < 0) {
					if (dir_ptr->Sign () == STOP_SIGN || dir_ptr->Sign () == ALL_STOP) {
						connect_ptr->Control (STOP_GREEN);
					} else if (dir_ptr->Sign () == YIELD_SIGN) {
						connect_ptr->Control (PERMITTED_GREEN);
					} else {
						connect_ptr->Control (UNCONTROLLED);
					}
				} else {
					connect_ptr->Control (RED_LIGHT);
				}
			}
		}
	}

	//---- parking cell number ----

	for (i=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, i++) {
		link_ptr = &link_array [park_itr->Link ()];

		sim_park_ptr = &sim_park_array [i];

		c0 = park_itr->Offset ();
		c1 = link_ptr->Length () - park_itr->Offset ();

		if (park_itr->Dir () == 0) {
			sim_park_ptr->Offset_AB (c0);
			sim_park_ptr->Offset_BA (c1);
		} else {
			sim_park_ptr->Offset_BA (c0);
			sim_park_ptr->Offset_AB (c1);
		}
		sim_park_ptr->Dir (park_itr->Dir ());
		sim_park_ptr->Type (park_itr->Type ());

		//---- set the access lanes for each direction ----

		for (dir=0; dir < 2; dir++) {
			if (dir) {
				index = link_ptr->BA_Dir ();
				c0 = sim_park_ptr->Offset_BA ();
			} else {
				index = link_ptr->AB_Dir ();
				c0 = sim_park_ptr->Offset_AB ();
			}
			if (index < 0) continue;

			sim_dir_ptr = &sim_dir_array [index];

			c0 /= param.cell_size;
			min_lane = sim_dir_ptr->Lanes ();
			max_lane = 0;

			for (lane=0; lane < sim_dir_ptr->Lanes (); lane++) {
				if (sim_dir_ptr->Check (lane, c0)) {
					max_lane = lane;
					if (lane < min_lane) min_lane = lane;
				}
			}
			if (sim_park_ptr->Type () != BOUNDARY) {
				if (sim_park_ptr->Dir () == dir) {
					min_lane = MAX (max_lane - 1, min_lane);
				} else {
					max_lane = MIN (min_lane + 1, max_lane);
				}
			}
			if (dir) {
				sim_park_ptr->Min_Lane_BA (min_lane);
				sim_park_ptr->Max_Lane_BA (max_lane);
			} else {
				sim_park_ptr->Min_Lane_AB (min_lane);
				sim_park_ptr->Max_Lane_AB (max_lane);
			}
		}
	}

	//---- transit plans ----

	if (param.transit_flag) {
		Transit_Plans ();
	}

	//---- identify conflict links ----

	for (bnode = 0, node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++, bnode++) {
		if (node_itr->Control () == -1) continue;

		flag = (node_itr->Control () >= 0);		//---- signal flag ----

		//---- process each link entering the node ----

		list = &node_link [bnode];

		for (index = *list; index >= 0; index = link_list [index]) {
			dir_ptr = &dir_array [index];

			bear1 = dir_ptr->Out_Bearing ();
			if (flag) bear1 = compass.Flip (bear1);

			//---- find connections with potential conflicts ----

			for (next = dir_ptr->First_Connect (); next >= 0; next = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [next];

				if (flag) {
					if (connect_ptr->Type () != LEFT) continue;
				} else {
					if (connect_ptr->Control () != STOP_GREEN) continue;
					if (connect_ptr->Type () == RIGHT || connect_ptr->Type () == UTURN) continue;
				}
				sim_con_ptr = &sim_connection [next];
				n = sim_con_ptr->Max_Conflicts ();

				//---- search for conflicting approach links ----

				for (dir = *list; dir >= 0; dir = link_list [dir]) {
					if (dir == index) continue;

					app_ptr = &dir_array [dir];

					bear2 = app_ptr->Out_Bearing ();
					change = compass.Change (bear1, bear2);

					//---- check the angle ----

					if (change >= -45 && change <= 45) {
						if (!flag && connect_ptr->Type () != LEFT) continue;
						i = 0;
					} else if (change > -135 && change < -45) {
						if (flag || connect_ptr->Type () == LEFT) continue;
						i = 0;
					} else if (change > 45 && change < 135) {
						if (flag) continue;
						i = 1;
					} else {
						continue;
					}
					
					//---- find the thru movement ----
					
					for (record = app_ptr->First_Connect (); record >= 0; record = cross_ptr->Next_Index ()) {
						cross_ptr = &connect_array [record];

						if (cross_ptr->Type () != THRU) continue;	// r_split l_split, r_merge, l_merge??

						if (sim_con_ptr->Conflict (i) < 0) {
							sim_con_ptr->Conflict (i, record);
						} else if (!flag && 
							((change >= -100 && change <= -80) || (change >= 80 && change <= 100))) {
							sim_con_ptr->Conflict (i, record);
						}
					}
				}
			}
		}
	}
}
