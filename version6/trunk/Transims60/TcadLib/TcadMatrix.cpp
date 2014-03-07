//*********************************************************
//	TcadMatrix.cpp - TransCAD Matrix DLL Interface
//*********************************************************

#include "TcadMatrix.hpp"

#include "MTXERR.H"

int tc_status;
bool TransCAD_Matrix::DLL_loaded = false;

//---------------------------------------------------------
//	TransCAD_Matrix constructors
//---------------------------------------------------------

TransCAD_Matrix::TransCAD_Matrix (Access_Type access) : Db_Matrix (access, MATRIX)
{
	fh = 0;
	Model_Format (TRANSCAD);
}

//---------------------------------------------------------
//	Load_DLL
//---------------------------------------------------------

bool TransCAD_Matrix::Load_DLL (void)
{
	if (!DLL_loaded) {
		tc_status = TC_OKAY;
		exe->Write (1, "Loading TransCAD DLL");

		InitMatDLL (&tc_status);

		DLL_loaded = (tc_status == TC_OKAY);
	}
	return (DLL_loaded);
}

//---------------------------------------------------------
//	Db_Open
//---------------------------------------------------------

bool TransCAD_Matrix::Db_Open (string filename)
{
	int i;
	DATA_TYPE data_type;

	if (!Load_DLL ()) return (Status (NO_TRANSCAD));
	
	Set_Field_Numbers ();

	if (File_Access () == CREATE) {
		int len;
		char **core_names;
		Db_Field *fld_ptr;

		if (Num_Des () == 0 || Tables () == 0) return (false);

		if (Type () == DB_DOUBLE) {
			data_type = DOUBLE_TYPE;
		} else if (Type () == DB_FLOAT) {
			data_type = FLOAT_TYPE;
		} else if (Type () == DB_SHORT) {
			data_type = SHORT_TYPE;
		} else {
			data_type = LONG_TYPE;
		}
		core_names = new char * [Tables ()];

		for (i=0; i < Tables (); i++) {
			fld_ptr = Table_Field (i);
			len = (int) fld_ptr->Name ().length () + 1;
			core_names [i] = new char [len];
			strcpy_s (core_names [i], len, fld_ptr->Name ().c_str ());
		}
		if ((int) Org_Map ()->size () != Num_Org () || (int) Des_Map ()->size () != Num_Des ()) {
			for (i=1; i <= Num_Org (); i++) {
				Add_Org (i);
			}
			for (i=1; i <= Num_Des (); i++) {
				Add_Des (i);
			}
			fh = MATRIX_New ((char *) filename.c_str (), (char *) File_Type ().c_str (), 
				Num_Org (), 0, Num_Des (), 0, Tables (), core_names, data_type, 1);
		} else {
			int i;
			Integers row_ids, col_ids;
			Int_Map_Itr map_itr;

			row_ids.assign (Num_Org (), 0);

			for (i=0, map_itr = Org_Map ()->begin (); map_itr != Org_Map ()->end (); map_itr++, i++) {
				row_ids [i] = map_itr->first;
			}
			col_ids.assign (Num_Des (), 0);

			for (i=0, map_itr = Des_Map ()->begin (); map_itr != Des_Map ()->end (); map_itr++, i++) {
				col_ids [i] = map_itr->first;
			}
			fh = MATRIX_New ((char *) filename.c_str (), (char *) File_Type ().c_str (), 
				Num_Org (), (long *) &row_ids [0], Num_Des (), (long *) &col_ids [0], 
				Tables (), core_names, data_type, 1);
		}
		for (i=0; i < Tables (); i++) {
			delete [] core_names [i];
		}
		delete [] core_names;

	} else {
		fh = MATRIX_LoadFromFile ((char *) filename.c_str (), CONTROL_AUTOMATIC);

		if (tc_status != TC_OKAY || fh == 0) return (false);

		Num_Org (MATRIX_GetNRows (fh));
		Num_Des (MATRIX_GetNCols (fh));

		data_type = MATRIX_GetDataType (fh);
		if (data_type == DOUBLE_TYPE) {
			Type (DB_DOUBLE);
		} else if (data_type == FLOAT_TYPE) {
			Type (DB_FLOAT);
		} else if (data_type == SHORT_TYPE) {
			Type (DB_SHORT);
		} else {
			Type (DB_INTEGER);
		}
		Tables (MATRIX_GetNCores (fh));

		Integers ids;
		ids.assign (Num_Org (), 0);

		MATRIX_GetIDs(fh, MATRIX_ROW, (long *) &(ids [0]));

		for (i=0; i < Num_Org (); i++) {
			Add_Org (ids [i]);
		}
		ids.assign (Num_Des (), 0);

		MATRIX_GetIDs(fh, MATRIX_COL, (long *) &(ids [0]));

		for (i=0; i < Num_Des (); i++) {
			Add_Des (ids [i]);
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool TransCAD_Matrix::Close (void)
{
	if (fh != 0) {
		MATRIX_Done (fh);
	}
	return (Db_Matrix::Close ());
}
//---------------------------------------------------------
//	Read_Row
//---------------------------------------------------------

bool TransCAD_Matrix::Read_Row (int org, int period)
{
	if (!Data_Flag ()) {
		if (!Allocate_Data ()) return (false);
	}
	period = 0;
	org = Org_Index (org);
	if (org < 0) {
		if (org < -1) return (false);
		Zero_Data ();
		return (true);
	}

	for (int i=0; i < Tables (); i++) {
		MATRIX_SetCore (fh, (short) i);
		switch (Type ()) {
			default:
			case DB_DOUBLE:
				MATRIX_GetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, Row_Ptr (i));
				break;
			case DB_FLOAT:
				MATRIX_GetBaseVector (fh, org, MATRIX_ROW, FLOAT_TYPE, Row_Ptr (i));
				break;
			case DB_INTEGER:
				MATRIX_GetBaseVector (fh, org, MATRIX_ROW, LONG_TYPE, Row_Ptr (i));
				break;
			case DB_SHORT:
				MATRIX_GetBaseVector (fh, org, MATRIX_ROW, SHORT_TYPE, Row_Ptr (i));
				break;
		}
		if (tc_status != TC_OKAY) return (false);
	}
	return (true);
}

bool TransCAD_Matrix::Read_Row (void *data, int org, int table, int period)
{
	period = 0;
	MATRIX_SetCore (fh, (short) table);
	org = Org_Index (org);
	if (org < 0) {
		if (org < -1) return (false);
		int size;
		switch (Type ()) {
			default:
			case DB_DOUBLE:
				size = sizeof (double);
				break;
			case DB_FLOAT:
				size = sizeof (float);
				break;
			case DB_INTEGER:
				size = sizeof (int);
				break;
			case DB_SHORT:
				size = sizeof (short);
				break;
		}
		size *= Num_Des ();
		memset (data, '\0', size);
		return (true);
	}

	switch (Type ()) {
		default:
		case DB_DOUBLE:
			MATRIX_GetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, data);
			break;
		case DB_FLOAT:
			MATRIX_GetBaseVector (fh, org, MATRIX_ROW, FLOAT_TYPE, data);
			break;
		case DB_INTEGER:
			MATRIX_GetBaseVector (fh, org, MATRIX_ROW, LONG_TYPE, data);
			break;
		case DB_SHORT:
			MATRIX_GetBaseVector (fh, org, MATRIX_ROW, SHORT_TYPE, data);
			break;
	}
	return (tc_status == TC_OKAY);
}

//---------------------------------------------------------
//	Write_Row
//---------------------------------------------------------

bool TransCAD_Matrix::Write_Row (int org, int period)
{
	if (!Data_Flag ()) return (false);
	period = 0;
	org = Org_Index (org);
	if (org < 0) {
		if (org < -1) return (false);
		return (true);
	}

	for (int i=0; i < Tables (); i++) {
		MATRIX_SetCore (fh, (short) i);
		switch (Type ()) {
			default:
			case DB_DOUBLE:
				MATRIX_SetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, Row_Ptr (i));
				break;
			case DB_FLOAT:
				MATRIX_SetBaseVector (fh, org, MATRIX_ROW, FLOAT_TYPE, Row_Ptr (i));
				break;
			case DB_INTEGER:
				MATRIX_SetBaseVector (fh, org, MATRIX_ROW, LONG_TYPE, Row_Ptr (i));
				break;
			case DB_SHORT:
				MATRIX_SetBaseVector (fh, org, MATRIX_ROW, SHORT_TYPE, Row_Ptr (i));
				break;
		}
		if (tc_status != TC_OKAY) return (false);
	}
	return (true);
}

bool TransCAD_Matrix::Write_Row (void *data, int org, int table, int period)
{
	period = 0;
	MATRIX_SetCore (fh, (short) table);
	org = Org_Index (org);
	if (org < 0) {
		if (org < -1) return (false);
		return (true);
	}

	switch (Type ()) {
		default:
		case DB_DOUBLE:
			MATRIX_SetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, data);
			break;
		case DB_FLOAT:
			MATRIX_SetBaseVector (fh, org, MATRIX_ROW, FLOAT_TYPE, data);
			break;
		case DB_INTEGER:
			MATRIX_SetBaseVector (fh, org, MATRIX_ROW, LONG_TYPE, data);
			break;
		case DB_SHORT:
			MATRIX_SetBaseVector (fh, org, MATRIX_ROW, SHORT_TYPE, data);
			break;
	}
	return (tc_status == TC_OKAY);
}
//---------------------------------------------------------
//	Read_Matrix
//---------------------------------------------------------

bool TransCAD_Matrix::Read_Matrix (void)
{
	int i, org, n;
	DATA_TYPE type;

	Int_Map *org_map;
	Int_Map_Itr org_itr;

	if (!Data_Flag () || !Matrix_Flag ()) {
		if (!Allocate_Data (true)) return (false);
	}
	exe->Show_Message (String ("Reading %s -- Record") % File_Type ());
	exe->Set_Progress ();

	org_map = Org_Map ();

	switch (Type ()) {
		default:
		case DB_DOUBLE:
			type = DOUBLE_TYPE;
			break;
		case DB_FLOAT:
			type = FLOAT_TYPE;
			break;
		case DB_INTEGER:
			type = LONG_TYPE;
			break;
		case DB_SHORT:
			type = SHORT_TYPE;
			break;
	}	
	
	for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
		org = org_itr->first - 1;
		i = org_itr->second;

		for (n=0; n < Tables (); n++) {
			exe->Show_Progress ();

			MATRIX_SetCore (fh, (short) n);
			MATRIX_GetBaseVector (fh, org, MATRIX_ROW, type, Matrix_Ptr (0, i, n));
		}
	}
	exe->End_Progress ();
	exe->Print (2, String ("Number of %s Records = %d") % File_Type () % exe->Progress_Count ());
	return (true);
}

//---------------------------------------------------------
//	Write_Matrix
//---------------------------------------------------------

bool TransCAD_Matrix::Write_Matrix (void)
{
	int i, n, org;
	DATA_TYPE type;

	Int_Map *org_map;
	Int_Map_Itr org_itr;
	
	if (!Matrix_Flag ()) return (false);

	exe->Show_Message (String ("Writing %s -- Record") % File_Type ());
	exe->Set_Progress ();

	org_map = Org_Map ();

	switch (Type ()) {
		default:
		case DB_DOUBLE:
			type = DOUBLE_TYPE;
			break;
		case DB_FLOAT:
			type = FLOAT_TYPE;
			break;
		case DB_INTEGER:
			type = LONG_TYPE;
			break;
		case DB_SHORT:
			type = SHORT_TYPE;
			break;
	}

	//---- process each origin zone ----

	for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
		org = org_itr->first - 1;
		i = org_itr->second;

		for (n=0; n < Tables (); n++) {
			exe->Show_Progress ();

			MATRIX_SetCore (fh, (short) n);
			MATRIX_SetBaseVector (fh, org, MATRIX_ROW, type, Matrix_Ptr (0, i, n));
		}
	}
	exe->End_Progress ();
	exe->Print (2, String ("Number of %s Records = %d") % File_Type () % exe->Progress_Count ());
	return (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool TransCAD_Matrix::Read_Header (bool stat)
{
	int i;
	char label [_MAX_FLABEL];
	double size;
	Field_Type type;
	String text;

	//---- extract the table names ----

	Clear_Fields ();
	stat = false;

	type = Type ();

	if (type == DB_DOUBLE) {
		size = sizeof (double);
	} else if (type == DB_FLOAT) {
		type = DB_DOUBLE;
		size = sizeof (float);
	} else if (type == DB_SHORT) {
		type = DB_INTEGER;
		size = sizeof (short);
	} else {
		size = sizeof (int);
	}

	for (i=0; i < Tables (); i++) {
		MATRIX_GetLabel (fh, (short) i, label);
		text = label;
		Add_Field (text, type, size, NO_UNITS, true);
	}
	Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);

	return (Set_Field_Numbers ());
}
