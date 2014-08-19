//*********************************************************
//	Event_Output.cpp - Output Interface Class
//*********************************************************

#include "Event_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Event_Output constructor
//---------------------------------------------------------

Event_Output::Event_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (EVENT_OUTPUT_OFFSET);
	coord_flag = false;
	
	Number (num);

	sim->Break_Check (10);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_EVENT_FILE, num));

	file = new Event_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_EVENT_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_EVENT_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	}

	//---- get the filter ----

	filter = sim->Get_Control_Integer (Sim_Output_Step::NEW_EVENT_FILTER, num);

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_EVENT_TIME_FORMAT, num);

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_EVENT_TIME_RANGE, num));

	//---- event range ----

	sim->Event_Range_Key (Sim_Output_Step::NEW_EVENT_TYPE_RANGE, type_flag, num);

	//---- mode range ----

	sim->Mode_Range_Key (Sim_Output_Step::NEW_EVENT_MODE_RANGE, mode_flag, num);

	//---- get the link range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_EVENT_LINK_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		link_range.Add_Ranges (key);
	}

	//---- get the subarea range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_EVENT_SUBAREA_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		subarea_range.Add_Ranges (key);
	}

	//---- get the coordinates ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_EVENT_COORDINATES, num);

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
	sim->Error (String ("Output Event Coordinate Data #%d") % num);
}

//---------------------------------------------------------
//	Event_Output destructor
//---------------------------------------------------------

Event_Output::~Event_Output ()
{
	if (file != 0) {
		file->Close ();
	}
}

//---------------------------------------------------------
//	Event_Check
//---------------------------------------------------------

void Event_Output::Event_Check (Event_Type type, Travel_Step &step)
{
	if (type >= MAX_EVENT || !type_flag [type]) return;
	if (!time_range.In_Range (sim->time_step)) return;

	if (step.sim_plan_ptr == 0) {
		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
		if (step.sim_plan_ptr == 0) return;
	}
	int mode = step.sim_plan_ptr->Mode ();
	if (mode >= MAX_MODE || !mode_flag [mode]) return;

	if (step.sim_dir_ptr != 0) {
		int subarea = step.sim_dir_ptr->Subarea ();
		if (!subarea_range.empty () && !subarea_range.In_Range (subarea)) return;
	}
	Event_Data data;

	data.Household (step.sim_travel_ptr->Household ());
	data.Person (step.sim_travel_ptr->Person ());
	data.Tour (step.sim_plan_ptr->Tour ());
	data.Trip (step.sim_plan_ptr->Trip ());
	data.Mode (mode);
	if (type == TRIP_START_EVENT) {
		data.Schedule (step.sim_plan_ptr->Start ());
	} else if (type == TRIP_END_EVENT) {
		data.Schedule (step.sim_plan_ptr->End ());
	} else if (type == VEH_START_EVENT) {
		data.Schedule (step.sim_travel_ptr->Next_Event ().Round_Seconds ());
	} else {
		data.Schedule (step.sim_plan_ptr->Schedule ());

		if (step.sim_dir_ptr != 0) {
			data.Dir_Index (step.Dir_Index ());

			if (step.sim_veh_ptr != 0) {
				data.Dir_Index (step.sim_veh_ptr->link);
				data.Lane (step.sim_veh_ptr->lane);
				data.Offset (step.sim_veh_ptr->offset);
			}
		}
	}
	data.Actual (sim->time_step);
	data.Event (type);

	if (abs ((int) (data.Actual () - data.Schedule ())) < filter) return;

	if (data.Dir_Index () >= 0) {
		Dir_Data *dir_ptr = &sim->dir_array [data.Dir_Index ()];
		Link_Data *link_ptr = &sim->link_array [dir_ptr->Link ()];

		if (!link_range.empty () && !link_range.In_Range (link_ptr->Link ())) return;

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
	file->Lock ();
	sim->Put_Event_Data (*file, data);
	file->UnLock ();
}
