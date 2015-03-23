//*********************************************************
//	Write_Constraint.cpp - write capacity constraint file
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Write_Constraint
//---------------------------------------------------------

void Converge_Service::Write_Constraint (int period)
{
	int index;
	Loc_Cap_Itr cap_itr;
	Location_Data *loc_ptr;

	Db_File *file = &cap_const_file;

	if (period >= 0) {
		String ext;
		String name = file->Filename ();
		file = new Db_File ();

		name.Split_Last (ext, ".");
		name = String ("%s_%d.%d") % name % period % ext;
		file->First_Open (false);
		file->Create (name);
	}

	file->File () << "LOCATION\tCAPACITY\tDEMAND\tFAILED" << endl;

	for (index=0, cap_itr = loc_cap_array.begin (); cap_itr != loc_cap_array.end (); cap_itr++, index++) {
		if (cap_itr->capacity == 0) continue;

		loc_ptr = &location_array [index];

		file->File () << loc_ptr->Location () << "\t" << cap_itr->capacity << "\t" << cap_itr->demand << "\t" << cap_itr->failed << endl;
	}
	file->Close ();
}
