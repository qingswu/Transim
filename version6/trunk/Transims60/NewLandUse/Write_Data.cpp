//*********************************************************
//	Write_Data.cpp - write trip distribution data
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Write_Data
//---------------------------------------------------------

void NewLandUse::Write_Data (void)
{
	int index, cover;
	double dvalue;
	Str_Itr str_itr;
	Zone_Field_Data *field_ptr;
	Zone_File *file, *new_file;
	Int_Map_Itr map_itr;
	Int_Itr int_itr;

	file = (Zone_File *) System_File_Handle (ZONE);
	new_file = (Zone_File *) System_File_Handle (NEW_ZONE);

	file->Open ();

	Show_Message (2, String ("Writing %s -- Record") % new_file->File_Type ());;
	Set_Progress ();

	while (file->Read ()) {
		Show_Progress ();

		new_file->Copy_Fields (*file);

		map_itr = zone_map.find (file->Zone ());
		if (map_itr != zone_map.end ()) {
			field_ptr = &zone_field_array [map_itr->second];

			for (index=0, int_itr = data_fields.begin (); int_itr != data_fields.end (); int_itr++, index++) {
				dvalue = field_ptr->data [0] [index] - field_ptr->change [0] [index];

				for (cover=1; cover < cover_types; cover++) {
					dvalue += field_ptr->change [cover] [index];
				}
				if (dvalue < 0) dvalue = 0;
				new_file->Put_Field (*int_itr, dvalue);
			}
		}
		new_file->Write ();
	}
	End_Progress ();

	file->Close ();
	new_file->Close ();

	Print (2, String ("Number of %s Records = %d") % new_file->File_Type () % Progress_Count ());
}

