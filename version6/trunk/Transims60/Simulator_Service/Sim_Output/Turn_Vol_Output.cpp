//*********************************************************
//	Turn_Vol_Output.cpp - Output Interface Class
//*********************************************************

#include "Turn_Vol_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Turn_Vol_Output constructor
//---------------------------------------------------------

Turn_Vol_Output::Turn_Vol_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (TURN_VOL_OUTPUT_OFFSET);
	data_flag = false;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_TURN_VOLUME_FILE, num));

	file = new Turn_Vol_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----
			
	key = sim->Get_Control_String (Sim_Output_Step::NEW_TURN_VOLUME_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_TURN_VOLUME_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	}

	//---- get the filter ----

	filter = sim->Get_Control_Integer (Sim_Output_Step::NEW_TURN_VOLUME_FILTER, num);

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_VOLUME_TIME_FORMAT, num);

	//---- time increment ----

	time_range.Increment (sim->Get_Control_Time (Sim_Output_Step::NEW_TURN_VOLUME_INCREMENT, num));

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_VOLUME_TIME_RANGE, num));

	//---- get the node range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_VOLUME_NODE_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		node_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_TURN_VOLUME_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Turn_Vol_Output::Output_Check (void)
{
	if (sim->time_step <= sim->Model_Start_Time ()) return (false);

	if (time_range.In_Range (sim->time_step)) {
		if (!data_flag) {

			//---- initialize the data ----

			data_flag = true;
		} else {

			//---- check the output time increment ----

			if (time_range.At_Increment (sim->time_step)) {
#ifdef MPI_EXE
				MPI_Processing (output);
#endif
				if (sim->Master ()) {
					Write_Turn ();
				}
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Summarize
//---------------------------------------------------------

void Turn_Vol_Output::Summarize (Travel_Step &step)
{
	if (step.size () < 1) return;
					
	int dir_index = step.Dir_Index ();
	int to_index = (--step.end ())->link;

	if (dir_index == to_index || dir_index < 0 || to_index < 0) return;

	int node = 0;

	Node_Data *node_ptr = 0;
	Sort_Key key;
	Turn_Map_Stat map_stat;

	if (!time_range.In_Range (sim->time_step)) return;

	if (node == 0) {
		Dir_Data *dir_ptr = &dat->dir_array [dir_index];
		Link_Data *link_ptr = &dat->link_array [dir_ptr->Link ()];

		if (dir_ptr->Dir () == 1) {
			node = link_ptr->Anode ();
		} else {
			node = link_ptr->Bnode ();
		}
		node_ptr = &dat->node_array [node];
		node = node_ptr->Node ();
	}
	if (!node_range.empty ()) {
		if (!node_range.In_Range (node)) return;
	}
	if (!subarea_range.empty ()) {
		if (!subarea_range.In_Range (node_ptr->Subarea ())) return;
	}
	key.node = node;
	key.dir_index = dir_index;
	key.to_index = to_index;

	map_stat = turn_map.insert (Turn_Map_Data (key, 1));

	if (!map_stat.second) {
		map_stat.first->second++;
	}
}

//---------------------------------------------------------
//	Write_Turn
//---------------------------------------------------------

void Turn_Vol_Output::Write_Turn (void)
{
	int node, dir_index, to_index, last_dir, last_to, link, to_link;	

	Turn_Map_Itr map_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	last_dir = last_to = link = to_link = -1;

	for (map_itr = turn_map.begin (); map_itr != turn_map.end (); map_itr++) {
		if (map_itr->second >= filter) {
			node = map_itr->first.node;
			dir_index = map_itr->first.dir_index;
			to_index = map_itr->first.to_index;

			if (dir_index != last_dir) {
				dir_ptr = &dat->dir_array [dir_index];
				link_ptr = &dat->link_array [dir_ptr->Link ()];
				link = link_ptr->Link ();
				last_dir = dir_index;
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

			file->Volume (map_itr->second);

			if (!file->Write ()) {
				sim->Error ("Writing Turn Volume File");
			}
		}
		map_itr->second = 0;
	}
}

bool operator < (Turn_Vol_Output::Sort_Key left, Turn_Vol_Output::Sort_Key right)
{
	if (left.node < right.node) return (true);
	if (left.node == right.node) {
		if (left.dir_index < right.dir_index) return (true);
		if (left.dir_index == right.dir_index) {
			if (left.to_index < right.to_index) return (true);
		}
	}
	return (false); 
}
