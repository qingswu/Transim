//*********************************************************
//	Turn_Delay_Output.cpp - Output Interface Class
//*********************************************************

#include "Turn_Delay_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Turn_Delay_Output constructor
//---------------------------------------------------------

Turn_Delay_Output::Turn_Delay_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (TURN_DELAY_OUTPUT_OFFSET);
	data_flag = false;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_TURN_DELAY_FILE, num));

	file = new Turn_Delay_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----
			
	key = sim->Get_Control_String (Sim_Output_Step::NEW_TURN_DELAY_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_TURN_DELAY_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	}

	//---- get the filter ----

	filter = sim->Get_Control_Integer (Sim_Output_Step::NEW_TURN_DELAY_FILTER, num);

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_DELAY_TIME_FORMAT, num);

	//---- time increment ----

	time_range.Increment (sim->Get_Control_Time (Sim_Output_Step::NEW_TURN_DELAY_INCREMENT, num));

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_DELAY_TIME_RANGE, num));

	//---- get the node range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_DELAY_NODE_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		node_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_DELAY_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}
}

//---------------------------------------------------------
//	Turn_Delay_Output destructor
//---------------------------------------------------------

Turn_Delay_Output::~Turn_Delay_Output ()
{
	if (file != 0) {
		file->Close ();
	}
}

//---------------------------------------------------------
//	Write_Check
//---------------------------------------------------------

void Turn_Delay_Output::Write_Check (void)
{
	if (sim->time_step <= sim->Model_Start_Time ()) return;
	
	if (!time_range.In_Range (sim->time_step)) return;

	if (!data_flag) {

		//---- initialize the data ----	

		int dir_index, index, node;

		Dir_Itr dir_itr;
		Link_Data *link_ptr;
		Node_Data *node_ptr;
		Connect_Data *connect_ptr;
		Int2_Key key;
		Turn_Map_Stat map_stat;
		Turn_Data turn_data;

		for (dir_index = 0, dir_itr = sim->dir_array.begin (); dir_itr != sim->dir_array.end (); dir_itr++, dir_index++) {
			link_ptr = &sim->link_array [dir_itr->Link ()];

			if (dir_itr->Dir () == 1) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
			node_ptr = &sim->node_array [node];

			if (!subarea_range.empty ()) {
				if (!subarea_range.In_Range (node_ptr->Subarea ())) continue;
			}
			node = node_ptr->Node ();
	
			if (!node_range.empty ()) {
				if (!node_range.In_Range (node)) continue;
			}
			key.first = dir_index;

			for (index = dir_itr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &sim->connect_array [index];

				key.second = connect_ptr->To_Index ();

				turn_map.insert (Turn_Map_Data (key, turn_data));
			}
		}
		data_flag = true;
	} else {

		//---- check the output time increment ----

		if (time_range.At_Increment (sim->time_step)) {
			if (sim->Master ()) {
				Write_Turn ();
			}
		}
	}
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Turn_Delay_Output::Output_Check (Travel_Step &step)
{
	if (!data_flag || step.size () < 2 || !time_range.In_Range (sim->time_step)) return;

	Sim_Veh_Data in_veh, out_veh;

	in_veh = step.front ();
	out_veh = step.back ();

	if (in_veh.link == out_veh.link) return;

	Int2_Key key;
	Turn_Map_Itr map_itr;

	key.first = in_veh.link;
	key.second = out_veh.link;

	map_itr = turn_map.find (key);

	if (map_itr != turn_map.end ()) {
		map_itr->second.Add_Turn (1);
	}
}

//---------------------------------------------------------
//	Write_Turn
//---------------------------------------------------------

void Turn_Delay_Output::Write_Turn (void)
{
	int node, dir_index, to_index, last_dir, last_to, link, to_link;	

	Turn_Map_Itr map_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;

	last_dir = last_to = link = to_link = node = -1;

	for (map_itr = turn_map.begin (); map_itr != turn_map.end (); map_itr++) {
		if (map_itr->second.Turn () >= filter) {

			dir_index = map_itr->first.first;
			to_index = map_itr->first.second;

			if (dir_index != last_dir) {
				dir_ptr = &dat->dir_array [dir_index];
				link_ptr = &dat->link_array [dir_ptr->Link ()];
				link = link_ptr->Link ();
				last_dir = dir_index;

				if (dir_ptr->Dir () == 1) {
					node = link_ptr->Anode ();
				} else {
					node = link_ptr->Bnode ();
				}
				node_ptr = &sim->node_array [node];
				node = node_ptr->Node ();
			}
			if (to_index != last_to) {
				dir_ptr = &dat->dir_array [to_index];
				link_ptr = &dat->link_array [dir_ptr->Link ()];
				to_link = link_ptr->Link ();
				last_to = to_index;
			}
			file->Node (node);
			file->Link (link);
			file->To_Link (to_link);

			file->Start (sim->time_step - time_range.Increment ());
			file->End (sim->time_step);

			file->Turn (map_itr->second.Turn ());
			file->Time (map_itr->second.Time ());

			if (!file->Write ()) {
				sim->Error ("Writing Turn Delay File");
			}
		}
		map_itr->second.Clear ();
	}
}
