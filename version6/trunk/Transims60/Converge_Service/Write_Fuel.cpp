//*********************************************************
//	Write_Fuel.cpp - write fuel demand file
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Write_Fuel_Demand
//---------------------------------------------------------

void Converge_Service::Write_Fuel_Demand (int period)
{
	int index;
	Loc_Fuel_Itr fuel_itr;
	Location_Data *loc_ptr;

	Db_File *file = &fuel_const_file;

	if (period >= 0) {
		String ext;
		String name = file->Filename ();
		file = new Db_File ();

		name.Split_Last (ext, ".");
		name = String ("%s_%d.%d") % name % period % ext;
		file->First_Open (false);
		file->Create (name);
	}
	file->File () << "LOCATION\tSUPPLY\tCONSUMED\tFAILED" << endl;

	for (index=0, fuel_itr = loc_fuel_array.begin (); fuel_itr != loc_fuel_array.end (); fuel_itr++, index++) {
		if (fuel_itr->supply == 0) continue;

		loc_ptr = &location_array [index];

		file->File () << loc_ptr->Location () << "\t" << UnRound (fuel_itr->supply) << "\t" << UnRound (fuel_itr->consumed) << "\t" << fuel_itr->failed << endl;
	}
	file->Close ();
}
