//*********************************************************
//	Traveler_Output.cpp - Output Interface Class
//*********************************************************

#include "Traveler_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Traveler_Output constructor
//---------------------------------------------------------

Traveler_Output::Traveler_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (TRAVELER_OUTPUT_OFFSET);
	coord_flag = false;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_TRAVELER_FILE, num));

	file = new Traveler_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_TRAVELER_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- traveler ID range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_TRAVELER_ID_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		if (key.find (":") != key.npos) {
			Strings ranges;
			String result, low_text, high_text;
			Str_Itr itr;
			Range_Data hhold, person;

			key.Parse (ranges);

			for (itr = ranges.begin (); itr != ranges.end (); itr++) {
				if (!itr->Range (low_text, high_text)) break;

				if (low_text.Split (result, ":")) {
					hhold.Low (result.Integer ());
					person.Low (low_text.Integer ());
				} else {
					hhold.Low (low_text.Integer ());
					person.Low (0);
				}
				if (high_text.Split (result, ":")) {
					hhold.High (result.Integer ());
					person.High (high_text.Integer ());
				} else {
					hhold.High (high_text.Integer ());
					person.High (MAX_INTEGER);
				}
				sim->hhold_range.push_back (hhold);
				sim->hhold_range.push_back (person);
			}
		} else {
			hhold_range.Add_Ranges (key);
		}
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_TRAVELER_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	}

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_TRAVELER_TIME_FORMAT, num);

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_TRAVELER_TIME_RANGE, num));

	//---- mode range ----

	sim->Mode_Range_Key (Sim_Output_Step::NEW_TRAVELER_MODE_RANGE, mode, num);

	//---- get the link range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_TRAVELER_LINK_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		link_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_TRAVELER_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}

	//---- get the coordinates ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_TRAVELER_COORDINATES, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		String token;

		key.Split (token);
		x1 = Round (token.Double ());

		if (!key.Split (token)) goto coord_error;
		y1 = Round (token.Double ());

		if (!key.Split (token)) goto coord_error;
		x2 = Round (token.Double ());

		if (!key.Split (token)) goto coord_error;
		y2 = Round (token.Double ());

		if (x2 < x1) {
			int x = x2;
			x2 = x1;
			x1 = x;
		}
		if (y2 < y1) {
			int y = y2;
			y2 = y1;
			y1 = y;
		}
		coord_flag = true;
	} else {
		coord_flag = false;
	}
	return;

coord_error:
	sim->Error (String ("Output Traveler Coordinate Data #%d") % num);
}

//---------------------------------------------------------
//	Traveler_Output destructor
//---------------------------------------------------------

Traveler_Output::~Traveler_Output () 
{
	if (file != 0) {
		file->Close ();
	}
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Traveler_Output::Output_Check (Travel_Step &step)
{
	if (step.Traveler () < 2 || !time_range.In_Range (sim->time_step) || step.size () < 1) return;

	Traveler_Data data;
	Sim_Veh_Itr sim_veh_itr;

	//---- check the mode ----

	Sim_Plan_Ptr sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
	if (sim_plan_ptr == 0) return;

	data.Mode (sim_plan_ptr->Mode ());

	if (data.Mode () >= MAX_MODE || !mode [data.Mode ()]) return;

	//---- check the household-person ----

	data.Household (step.sim_travel_ptr->Household ());
	data.Person (step.sim_travel_ptr->Person ());
	data.Tour (sim_plan_ptr->Tour ());
	data.Trip (sim_plan_ptr->Trip ());

	if (!hhold_range.empty ()) {
		int index = hhold_range.In_Index (data.Household ());
		if (index < 0) return;
		if (!person_range.empty ()) {
			Range_Data *range_ptr = &person_range [index];
			if (data.Person () < range_ptr->Low () || data.Person () > range_ptr->High ()) return;
		}
	}

	if (step.sim_veh_ptr != 0 && step.sim_veh_ptr->link >= 0) {
		Dir_Data *dir_ptr = &sim->dir_array [step.sim_veh_ptr->link];
		Link_Data *link_ptr = &sim->link_array [dir_ptr->Link ()];

		if (!link_range.empty () && !link_range.In_Range (link_ptr->Link ())) return;

		if (!subarea_range.empty ()) {
			Sim_Dir_Ptr sim_dir_ptr = &sim->sim_dir_array [step.sim_veh_ptr->link];
	
			if (!subarea_range.In_Range (sim_dir_ptr->Subarea ())) return;
		}
		if (coord_flag) {
			int ax, ay, bx, by;
			Node_Data *node_ptr;

			if (dir_ptr->Dir () == 0) {
				node_ptr = &sim->node_array [link_ptr->Bnode ()];
				bx = node_ptr->X ();
				by = node_ptr->Y ();
				node_ptr = &sim->node_array [link_ptr->Anode ()];
			} else {
				node_ptr = &sim->node_array [link_ptr->Anode ()];
				bx = node_ptr->X ();
				by = node_ptr->Y ();
				node_ptr = &sim->node_array [link_ptr->Bnode ()];
			}
			bx -= ax = node_ptr->X ();
			by -= ay = node_ptr->Y ();

			double length = link_ptr->Length ();

			ax += (int) (bx * data.Offset () / length + 0.5);
			ay += (int) (by * data.Offset () / length + 0.5);

			if (coord_flag) {
				if (ax < x1 || ax > x2 ||
					ay < y1 || ay > y2) {
					return;
				}
			}
		}
	}

	//---- stopped vehicle -----

	if (step.size () == 1) {
		sim_veh_itr = step.begin (); 

		data.Distance (0);
		data.Time (sim->time_step);
		data.Speed (0);

		data.Dir_Index (sim_veh_itr->link);
		data.Lane (sim_veh_itr->lane);
		data.Offset (sim_veh_itr->offset);

		file->Lock ();
		sim->Put_Traveler_Data (*file, data);
		file->UnLock ();
		return;
	}

	//---- movement size ----

	double step_size = sim->param.step_size;

	if (step.sim_dir_ptr != 0) {
		step_size = sim->method_time_step [step.sim_dir_ptr->Method ()];
	}
	double time = sim->time_step - step_size;

	step_size /= (step.size () - 1);

	//---- output movement increments ----

	int dir_index = -1;
	int offset = 0;

	file->Lock ();

	for (sim_veh_itr = step.begin (); sim_veh_itr != step.end (); sim_veh_itr++, time += step_size) {
		if (dir_index >= 0) {
			if (dir_index != sim_veh_itr->link) {
				offset = 0;
			}
			data.Distance (sim_veh_itr->offset - offset);
			data.Time ((Dtime) time);
			data.Speed (data.Distance () / step_size);

			data.Dir_Index (sim_veh_itr->link);
			data.Lane (sim_veh_itr->lane);
			data.Offset (sim_veh_itr->offset);

			sim->Put_Traveler_Data (*file, data);
		}
		dir_index = sim_veh_itr->link;
		offset = sim_veh_itr->offset;
	}
	file->UnLock ();
}
