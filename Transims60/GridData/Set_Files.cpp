//*********************************************************
//	Set_Files.cpp - Setup Data Files for User Program
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Set_Files
//---------------------------------------------------------

void GridData::Set_Files (void)
{
	int i, nfld;
	bool binary;

	Db_Field *fld;
	Db_Base_Itr db_itr;
	Db_Base *file;
	Db_Sort_Array *data;
	Data_Itr data_itr;
	Polygon_Itr poly_itr;

	//---- create the program data structure ----

	data_rec.push_back (&in_file);
	data_rec.push_back (&out_file);

	for (db_itr = data_files.begin (); db_itr != data_files.end (); db_itr++) {
		data_rec.push_back (*db_itr);
	}

	//---- convert each data file to binary ----

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
		file = data_itr->file;
		data = data_itr->data_db;

		data_rec.push_back ((Db_Base *) data);

		data->File_ID (file->File_ID ());
		data->File_Type (file->File_Type ());
		data->File_Format (BINARY);

		data->Replicate_Fields (file);
	}

	//---- read the arcview boundary file ----

	for (poly_itr = polygons.begin (); poly_itr != polygons.end (); poly_itr++) {
		file = poly_itr->file;
		data = poly_itr->data_db;

		data_rec.push_back ((Db_Base *) data);

		data->File_ID (file->File_ID ());
		data->File_Type (file->File_Type ());

		nfld = file->Num_Fields ();
		binary = (file->Record_Format () == BINARY);

		data->Add_Field ("INDEX", DB_INTEGER, 10);

		for (i=0; i < nfld; i++) {
			fld = file->Field (i);
			if (fld != 0) {
				data->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary);
			}
		}
	}

	//---- add the zone grid counts ----

	data_rec.push_back ((Db_Base *) &zone_grids);

	Write (1, "Compiling Conversion Script");

	if (Report_Flag (PRINT_SCRIPT)) {
		Header_Number (PRINT_SCRIPT);

		if (!Break_Check (10)) {
			Print (1);
			Page_Header ();
		}
	}
	program.Initialize (data_rec, random.Seed () + 1);

	if (!program.Compile (program_file, Report_Flag (PRINT_SCRIPT))) {
		Error ("Compiling Conversion Script");
	}
	if (Report_Flag (PRINT_STACK)) {
		Header_Number (PRINT_STACK);

		program.Print_Commands (false);
	}
	Show_Message (1);
	Header_Number (0);
} 
