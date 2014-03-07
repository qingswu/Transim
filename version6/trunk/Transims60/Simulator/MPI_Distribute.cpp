//*********************************************************
//	MPI_Distribute.cpp - distribute travelers to MPI machines
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	MPI_Distribute
//---------------------------------------------------------

bool Simulator::MPI_Distribute (bool status)
{
#ifdef MPI_EXE
	int i, num, rec, traveler, part, lvalue [3];
	Sim_Travel_Ptr sim_travel_ptr = 0;

	if (Master ()) {
		int rank;
		Int2_Itr itr;
		Integers *list;
		Int_Itr t_itr;

		lvalue [0] = status;
		lvalue [2] = (int) sim_travel_array.size ();

		for (rank=0, itr = mpi_range.begin (); itr != mpi_range.end (); itr++, rank++) {
			if (rank == 0) continue;

			//---- count the part lists assigned to each machine ----

			mpi_buffer.Size (0);
			rec = 0;

			for (part = itr->first; part <= itr->second; part++) {
				rec += (int) mpi_parts [part].size ();
				list = &mpi_parts [part];

				for (t_itr = list->begin (); t_itr != list->end (); t_itr++) {
					traveler = *t_itr;
					sim_travel_ptr = sim_travel_array [traveler];

					mpi_buffer.Add_Data (&traveler, sizeof (traveler));
					mpi_buffer.Add_Data (&part, sizeof (part));

					sim_travel_ptr->Pack (mpi_buffer);
				}
				list->clear ();
			}
			lvalue [1] = rec;

			//---- distribute the travel plans ----

			Send_MPI_Array (lvalue, 3, rank);

			if (rec == 0) continue;

			Send_MPI_Buffer (rank);
		}

	} else {	//---- slave ----

		Get_MPI_Array (lvalue, 3, 0);

		status = (lvalue [0] != 0);

		rec = lvalue [1];

		if (rec == 0) return (status);

		//---- expand the traveler array ----
			
		num = lvalue [2] - (int) sim_travel_array.size ();

		if (num > 0) {
			sim_travel_array.insert (sim_travel_array.end (), num, sim_travel_ptr);
		}

		//---- retrieve the traveler data ----

		Get_MPI_Buffer (0);

		//---- unpack the traveler data ----

		for (i=0; i < rec; i++) {
			mpi_buffer.Get_Data (&traveler, sizeof (traveler));
			
			mpi_buffer.Get_Data (&part, sizeof (part));

			part -= First_Partition ();

			sim_travel_ptr = sim_travel_array [traveler];
			if (sim_travel_ptr != 0) delete sim_travel_ptr;
						
			sim_travel_ptr = new Sim_Travel_Data ();
			sim_travel_ptr->UnPack (mpi_buffer);
			
			sim_travel_array [traveler] = sim_travel_ptr;
			work_step.Put (traveler, part);
		}
	}
#endif
	return (status);
}
