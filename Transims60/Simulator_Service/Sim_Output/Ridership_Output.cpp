//*********************************************************
//	Ridership_Output.cpp - Output Interface Class
//*********************************************************

#include "Ridership_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Ridership_Output constructor
//---------------------------------------------------------

Ridership_Output::Ridership_Output (int num) : Sim_Output_Data ()
{
	String key;

	Type (RIDERSHIP_OUTPUT_OFFSET);
	
	Number (num);	
	
	//---- check the transit file status ----

	if (!sim->System_File_Flag (TRANSIT_STOP) || !sim->System_File_Flag (TRANSIT_ROUTE) ||
		!sim->System_File_Flag (TRANSIT_SCHEDULE)) {
		sim->Print (1);
		sim->Warning ("Ridership Output Requires Transit Network Data");
		return;
	}

	sim->Break_Check (7);
	sim->Print (1);

	//---- get the file name ----

	key = sim->Project_Filename (sim->Get_Control_String (Sim_Output_Step::NEW_RIDERSHIP_FILE, num));

	file = new Ridership_File ();
	file->Filename (key);
	file->File_Type (sim->Current_Label ());

	//---- get the file format ----
			
	key = sim->Get_Control_String (Sim_Output_Step::NEW_RIDERSHIP_FORMAT, num);

	if (!key.empty ()) {
		file->Dbase_Format (key);
	}

	//---- get the time format ----

	key = sim->Get_Control_String (Sim_Output_Step::NEW_RIDERSHIP_TIME_FORMAT, num);
	if (!key.empty ()) {
		file->Time_Format (Time_Code (key));
	}

	//---- create the file ----

	if (sim->Master ()) {
		file->Create ();
	}

	//---- print the time format ----

	sim->Get_Control_Text (Sim_Output_Step::NEW_RIDERSHIP_TIME_FORMAT, num);

	//---- time ranges ----

	time_range.Add_Ranges (sim->Get_Control_Text (Sim_Output_Step::NEW_RIDERSHIP_TIME_RANGE, num));

	//---- get the route range ----

	key = sim->Get_Control_Text (Sim_Output_Step::NEW_RIDERSHIP_ROUTE_RANGE, num);

	if (!key.empty () && !key.Equals ("ALL")) {
		route_range.Add_Ranges (key);
	}

	//---- get all stops flag ----

	all_stops = sim->Get_Control_Flag (Sim_Output_Step::NEW_RIDERSHIP_ALL_STOPS, num);
}

//---------------------------------------------------------
//	Ridership_Output destructor
//---------------------------------------------------------

Ridership_Output::~Ridership_Output ()
{
	if (file != 0) {
		file->Close ();
	}
}

//---------------------------------------------------------
//	Output
//---------------------------------------------------------

void Ridership_Output::Output (void)
{

	//---- validate the offset data ----

	int run, nrun, board, alight, load;
	Dtime time, schedule;
	double capacity;
	bool route_flag;

	Line_Run *run_ptr;
	Line_Stop_Itr stop_itr;
	Line_Itr line_itr;

	Stop_Data *stop_ptr;
	Veh_Type_Data *veh_type_ptr;

	route_flag = !route_range.empty ();

	for (line_itr = sim->line_array.begin (); line_itr != sim->line_array.end (); line_itr++) {

		//---- check the route selection criteria -----

		if (route_flag) {
			if (!route_range.In_Range (line_itr->Route ())) continue;
		}
		file->Mode (line_itr->Mode ());
		file->Route (line_itr->Route ());

		//---- get the vehicle capacity ----

		veh_type_ptr = &sim->veh_type_array [line_itr->Type ()];

		capacity = veh_type_ptr->Capacity ();
		if (capacity > 1.0) {
			capacity = 1.0 / capacity;
		}

		//---- process each run stop ----

		stop_itr = line_itr->begin (); 

		nrun = (int) stop_itr->size ();

		for (run=0; run < nrun; run++) {
			load = 0;

			for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {

				run_ptr = &stop_itr->at (run);

				//---- check for riders ----
					
				board = run_ptr->Board ();
				alight = run_ptr->Alight ();

				load += board - alight;

				if (!all_stops && load == 0 && board == 0 && alight == 0) continue;

				//---- check the time range criteria ----

				time = run_ptr->Time ();
				schedule = run_ptr->Schedule ();

				if (time == 0 && schedule != 0) continue;

				if (!time_range.In_Range (time)) continue;

				//---- output the data record ----

				stop_ptr = &sim->stop_array [stop_itr->Stop ()];

				file->Run (run);
				file->Stop (stop_ptr->Stop ());
				file->Schedule (schedule);
				file->Time (time);
				file->Board (board);
				file->Alight (alight);
				file->Load (load);
				file->Factor (load * capacity);

				if (!file->Write ()) {
					sim->Error ("Writing Ridership Output File");
				}
			}
		}
	}
}

