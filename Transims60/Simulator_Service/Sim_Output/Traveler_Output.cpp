//*********************************************************
//	Traveler_Output.cpp - Output Interface Class
//*********************************************************

#include "Traveler_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Traveler_Output constructor / destructor
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
//	In_Range
//---------------------------------------------------------

bool Traveler_Output::In_Range (Traveler_Data &data)
{
	if (data.Mode () < MAX_MODE && mode [data.Mode ()]) {
		if (time_range.In_Range (sim->time_step)) {
			if (hhold_range.empty ()) return (true);
			int index = hhold_range.In_Index (data.Household ());
			if (index >= 0) {
				if (person_range.empty ()) return (true);
				Range_Data *range_ptr = &person_range [index];
				if (data.Person () >= range_ptr->Low () && data.Person () <= range_ptr->High ()) {
					return (true);
				}
			}
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Output_Traveler
//---------------------------------------------------------

void Traveler_Output::Output_Traveler (Traveler_Data &traveler_data)
{

#ifdef MPI_EXE
	if (sim->Num_Threads () > 1) {
		mutex_lock lock (data_mutex);
		data.Add_Data (&traveler_data, sizeof (traveler_data));
	} else {
		data.Add_Data (&traveler_data, sizeof (traveler_data));
	}
#else 
	if (sim->Num_Threads () > 1) {
 #ifdef THREADS
		traveler_queue->Put (traveler_data);
 #endif
	} else {
		Write_Traveler (traveler_data);
	}
#endif
}

//---------------------------------------------------------
//	Write_Traveler
//---------------------------------------------------------

void Traveler_Output::Write_Traveler (Traveler_Data &data)
{
	Int_Itr veh_itr;
	
	if (data.Mode () >= MAX_MODE || !mode [data.Mode ()]) return;
	if (!time_range.In_Range (data.Time ())) return;

	if (!hhold_range.empty ()) {
		int index = hhold_range.In_Index (data.Household ());
		if (index < 0) return;
		if (!person_range.empty ()) {
			Range_Data *range_ptr = &person_range [index];
			if (data.Person () < range_ptr->Low () || data.Person () > range_ptr->High ()) return;
		}
	}
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
	sim->Put_Traveler_Data (*file, data);
}

//---------------------------------------------------------
//	End_Output
//---------------------------------------------------------

void Traveler_Output::End_Output (void)
{
#ifdef MPI_EXE
#else
 #ifdef THREADS
	if (sim->Num_Threads () > 1) {
		traveler_queue->End_Queue ();
		traveler_queue->Exit_Queue ();
	}
 #endif
#endif
}

////---------------------------------------------------------
////	Traveler_Output constructor / destructor
////---------------------------------------------------------
//
//Traveler_Output::Traveler_Output (int num) : Output_Data ()
//{
//	output_flag = false;
//#ifdef THREADS
//	traveler_queue = 0;
//#endif
//}
//
//Traveler_Output::~Traveler_Output () 
//{
//#ifdef THREADS
//	if (traveler_queue != 0) {
//		delete traveler_queue;
//	}
//#endif
//}
//
////---------------------------------------------------------
////	Traveler_Output operator
////---------------------------------------------------------
//
//void Traveler_Output::operator()()
//{
//#ifdef MPI_EXE
//	while (sim->output_barrier.Go ()) {
//		MPI_Processing ();
//		sim->output_barrier.Finish ();
//	}
//#else
// #ifdef THREADS
//	Traveler_Data data;
//
//	traveler_queue = new Traveler_Queue ();
//
//	while (traveler_queue->Get (data)) {
//		Write_Traveler (data);
//	}
// #endif
//#endif
//}
