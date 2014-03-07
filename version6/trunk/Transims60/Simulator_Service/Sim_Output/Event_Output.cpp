//*********************************************************
//	Event_Output.cpp - Output Interface Class
//*********************************************************

#include "Event_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Event_Output constructor / destructor
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

	sim->Event_Range_Key (Sim_Output_Step::NEW_EVENT_TYPE_RANGE, type, num);

	//---- mode range ----

	sim->Mode_Range_Key (Sim_Output_Step::NEW_EVENT_MODE_RANGE, mode, num);

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
//	In_Range
//---------------------------------------------------------

bool Event_Output::In_Range (Event_Type t, int m, int subarea)
{
	if (t < MAX_EVENT && type [t]) {
		if (m < MAX_MODE && mode [m]) {
			if (time_range.In_Range (sim->time_step)) {
				if (subarea_range.empty () || subarea_range.In_Range (subarea)) {
					return (true);
				}
			}
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Output_Event
//---------------------------------------------------------

void Event_Output::Output_Event (Event_Data &event_data)
{

#ifdef MPI_EXE
	if (sim->Num_Threads () > 1) {
		mutex_lock lock (data_mutex);
		data.Add_Data (&event_data, sizeof (event_data));
	} else {
		data.Add_Data (&event_data, sizeof (event_data));
	}
#else 
	if (sim->Num_Threads () > 1) {
 #ifdef THREADS
		event_queue->Put (event_data);
 #endif
	} else {
		Write_Event (event_data);
	}
#endif
}

//---------------------------------------------------------
//	Write_Event
//---------------------------------------------------------

void Event_Output::Write_Event (Event_Data &data)
{
	Int_Itr veh_itr;
	
	if (data.Event () >= MAX_EVENT || !type [data.Event ()]) return;
	if (data.Mode () >= MAX_MODE || !mode [data.Mode ()]) return;
	if (!time_range.In_Range (data.Actual ())) return;
	if (abs ((int) (data.Actual () - data.Schedule ())) < filter) return;

	if (data.Dir_Index () >= 0) {
		Dir_Data *dir_ptr = &sim->dir_array [data.Dir_Index ()];
		Link_Data *link_ptr = &sim->link_array [dir_ptr->Link ()];

		if (!link_range.empty () && !link_range.In_Range (link_ptr->Link ())) return;

		if (!subarea_range.empty ()) {
			Sim_Dir_Ptr sim_dir_ptr = &sim->sim_dir_array [data.Dir_Index ()];

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
	sim->Put_Event_Data (*file, data);
}

//---------------------------------------------------------
//	End_Output
//---------------------------------------------------------

void Event_Output::End_Output (void)
{
#ifdef MPI_EXE
#else
 #ifdef THREADS
	if (sim->Num_Threads () > 1) {
		event_queue->End_Queue ();
		event_queue->Exit_Queue ();
	}
 #endif
#endif
}

////---------------------------------------------------------
////	Event_Output constructor / destructor
////---------------------------------------------------------
//
//Event_Output::Event_Output (int num) : Output_Data ()
//{
//	output_flag = false;
//#ifdef THREADS
//	event_queue = 0;
//#endif
//}
//
//Event_Output::~Event_Output ()
//{
//#ifdef THREADS
//	if (event_queue != 0) {
//		delete event_queue;
//	}
//#endif
//}
//
////---------------------------------------------------------
////	Event_Output operator
////---------------------------------------------------------
//
//void Event_Output::operator()()
//{
//#ifdef MPI_EXE
//	while (sim->output_barrier.Go ()) {
//		MPI_Processing ();
//		sim->output_barrier.Finish ();
//	}
//#else
// #ifdef THREADS
//	Event_Data data;
//	event_queue = new Event_Queue ();
//
//	while (event_queue->Get (data)) {
//		Write_Event (data);
//	}
// #endif
//#endif
//}
