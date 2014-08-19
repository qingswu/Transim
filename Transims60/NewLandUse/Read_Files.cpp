//*********************************************************
//	Read_Files.cpp - read zone and target files
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Read_Files
//---------------------------------------------------------

void NewLandUse::Read_Files (void)
{
	int target;	
	Str_Itr str_itr;
	Zone_Year_Itr file_itr;
	Zone_File *file;
	Zone_Data zone_rec;
	Doubles density;
	Dbls_Map_Stat map_stat;

	num_zones = (int) zone_array.size ();
	attr_bal.assign (num_zones, 0.0);

	//---- read zone files ----

	for (year_number = 1, file_itr = zone_year_array.begin (); file_itr != zone_year_array.end (); file_itr++, year_number++) {
		file = *file_itr;

		Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();

			zone_rec.Clear ();
			Get_Zone_Data (*file, zone_rec);
		}
		End_Progress ();
		file->Close ();
	
		Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());
	}

	//---- read the target file ----
		
	Show_Message (String ("Reading %s -- Record") % target_file.File_Type ());
	Set_Progress ();
	
	while (target_file.Read ()) {
		Show_Progress ();

		target = target_file.Get_Integer (index_field);
		density.clear ();

		for (str_itr = data_names.begin (); str_itr != data_names.end (); str_itr++) {
			density.push_back (target_file.Get_Double (*str_itr));
		}
		map_stat = target_density.insert (Dbls_Map_Data (target, density));

		if (!map_stat.second) {
			Warning ("Duplicate Density Data for Target ") << target;
		}
	}
	End_Progress ();
	target_file.Close ();

	Print (2, String ("Number of %s Records = %d") % target_file.File_Type () % Progress_Count ());
}
