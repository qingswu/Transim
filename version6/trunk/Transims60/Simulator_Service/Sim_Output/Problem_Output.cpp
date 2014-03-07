//*********************************************************
//	Problem_Output.cpp - Output Interface Class
//*********************************************************

#include "Problem_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Problem_Output constructor
//---------------------------------------------------------

Problem_Output::Problem_Output () : Static_Service ()
{
	output_flag = false;
	file = 0;
#ifdef THREADS
	problem_queue = 0;
#endif
}

Problem_Output::~Problem_Output () 
{
#ifdef THREADS
	if (problem_queue != 0) {
		delete problem_queue;
	}
#endif
}

//---------------------------------------------------------
//	Problem_Output operator
//---------------------------------------------------------

void Problem_Output::operator()()
{
#ifdef MPI_EXE
	while (sim->output_barrier.Go ()) {
		MPI_Processing ();
		sim->output_barrier.Finish ();
	}
#else
 #ifdef THREADS
	Problem_Data problem;
	problem_queue = new Problem_Queue ();

	while (problem_queue->Get (problem)) {
		Write_Problem (problem);
	}
 #endif
#endif
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Problem_Output::Read_Control (void)
{
	output_flag = sim->System_File_Flag (NEW_PROBLEM);

	if (output_flag) {
		file = (Problem_File *) sim->System_File_Handle (NEW_PROBLEM);
	}
	return (output_flag);
}

//---------------------------------------------------------
//	Output_Problem
//---------------------------------------------------------

void Problem_Output::Output_Problem (Problem_Data &problem)
{
	if (!output_flag) return;

#ifdef MPI_EXE
	if (sim->Num_Threads () > 1) {
		mutex_lock lock (data_mutex);
		data.Add_Data (&problem, sizeof (problem));
	} else {
		data.Add_Data (&problem, sizeof (problem));
	}
#else 
	if (sim->Num_Threads () > 1) {
 #ifdef THREADS
		problem_queue->Put (problem);
 #endif
	} else {
		Write_Problem (problem);
	}
#endif
}

//---------------------------------------------------------
//	Write_Problem
//---------------------------------------------------------

void Problem_Output::Write_Problem (Problem_Data &problem)
{
	if (output_flag) {
		sim->Put_Problem_Data (*file, problem);
	}
}

//---------------------------------------------------------
//	End_Output
//---------------------------------------------------------

void Problem_Output::End_Output (void)
{
#ifdef MPI_EXE
	if (sim->Slave ()) {
		file->Close ();
		string message = file->Filename ();
		remove (message.c_str ());
		message += ".def";
		remove (message.c_str ());
	}
#else
 #ifdef THREADS
	if (sim->Num_Threads () > 1) {
		problem_queue->End_Queue ();
		problem_queue->Exit_Queue ();
	}
 #endif
#endif
}

#ifdef MPI_EXE
//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Problem_Output::MPI_Processing (void)
{
	if (!output_flag) return;

	int tag = (sim->Num_Threads () > 1) ? NEW_PROBLEM : 0;

	if (sim->Master ()) {
		int i, num, size;
		Problem_Data problem;

		size = sizeof (problem);

		//---- save master problems ----

		num = (int) data.Size () / size;

		while (num-- > 0) {
			data.Get_Data (&problem, size);

			sim->Put_Problem_Data (*file, problem);
		}

		//---- process each slave ----

		for (i=1; i < sim->MPI_Size (); i++) {

			sim->Get_MPI_Buffer (data, tag);
			if (data.Size () == 0) continue;

			num = (int) data.Size () / size;

			while (num-- > 0) {
				data.Get_Data (&problem, size);

				sim->Put_Problem_Data (*file, problem);
			}
		}
		data.Size (0);

	} else {	//---- slave ----

		//---- send the data buffer ----

		sim->Send_MPI_Buffer (data, tag);
		data.Size (0);
	}
}
#endif
