//*********************************************************
//	TppMatrix.cpp - TPPlus Matrix DLL Interface
//*********************************************************

#include "TppMatrix.hpp"

#include <windows.h>

//---- matrix types ----

#define TPP      1
#define MINUTP   2
#define TPLAN    3
#define TRIPS    5

//---- license types ----

#define VIPER    1
#define CUBE     2
#define CUBEBASE 3
#define VOYAGER  4
#define CARGO    5

//---- operation codes ----

#define READ_HEADER  1
#define READ_DATA    2
#define SKIP_DATA    -2
#define READ_BOTH    3

TPPlus_Matrix::pf_FileInquire       TPPlus_Matrix::FileInquire = 0;
TPPlus_Matrix::pf_TppMatOpenIP      TPPlus_Matrix::TppMatOpenIP = 0;
TPPlus_Matrix::pf_TppMatOpenOP      TPPlus_Matrix::TppMatOpenOP = 0;
TPPlus_Matrix::pf_TppMatMatSet      TPPlus_Matrix::TppMatMatSet = 0;
TPPlus_Matrix::pf_TppMatClose       TPPlus_Matrix::TppMatClose = 0;

TPPlus_Matrix::pf_TppMatReadSelect  TPPlus_Matrix::TppMatReadSelect = 0;
TPPlus_Matrix::pf_TppMatReadNext    TPPlus_Matrix::TppMatReadNext = 0;
TPPlus_Matrix::pf_TppMatReadDirect  TPPlus_Matrix::TppMatReadDirect = 0;
TPPlus_Matrix::pf_TppMatMatWriteRow TPPlus_Matrix::TppMatMatWriteRow = 0;

TPPlus_Matrix::pf_TppMatPos         TPPlus_Matrix::TppMatPos = 0;
TPPlus_Matrix::pf_TppMatGetPos      TPPlus_Matrix::TppMatGetPos = 0;
TPPlus_Matrix::pf_TppMatSeek        TPPlus_Matrix::TppMatSeek = 0;

TPPlus_Matrix::pf_TppMatMatResize   TPPlus_Matrix::TppMatMatResize = 0;

//---------------------------------------------------------
//	TPPlus_Matrix constructors
//---------------------------------------------------------

TPPlus_Matrix::TPPlus_Matrix (Access_Type access) : Db_Matrix (access, MATRIX)
{
	fh = 0;
	Model_Format (TPPLUS);
}

//---------------------------------------------------------
//	Load_DLL
//---------------------------------------------------------

bool TPPlus_Matrix::Load_DLL (void)
{
	if (FileInquire == 0) {
		HMODULE hMod;

		hMod = LoadLibrary ("tppdlibx.dll");

		FileInquire       = (pf_FileInquire) GetProcAddress (hMod, "FileInquire");
		TppMatOpenIP      = (pf_TppMatOpenIP) GetProcAddress (hMod, "TppMatOpenIP");
		TppMatOpenOP      = (pf_TppMatOpenOP) GetProcAddress (hMod, "TppMatOpenOP");
		TppMatMatSet      = (pf_TppMatMatSet) GetProcAddress (hMod, "TppMatSet");
		TppMatClose       = (pf_TppMatClose) GetProcAddress (hMod, "TppMatClose");

		TppMatReadSelect  = (pf_TppMatReadSelect) GetProcAddress (hMod, "TppMatReadSelect");
		TppMatReadNext    = (pf_TppMatReadNext) GetProcAddress (hMod, "TppMatReadNext");
		TppMatReadDirect  = (pf_TppMatReadDirect) GetProcAddress (hMod, "TppMatReadDirect");
		TppMatMatWriteRow = (pf_TppMatMatWriteRow) GetProcAddress (hMod, "TppMatWriteRow");
	    
		TppMatPos         = (pf_TppMatPos) GetProcAddress (hMod, "TppMatPos");
		TppMatGetPos      = (pf_TppMatGetPos) GetProcAddress (hMod, "TppMatGetPos");
		TppMatSeek        = (pf_TppMatSeek) GetProcAddress (hMod, "TppMatSeek");

		TppMatMatResize   = (pf_TppMatMatResize) GetProcAddress (hMod, "TppMatResize");

		if (FileInquire == 0) return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Db_Open
//---------------------------------------------------------

bool TPPlus_Matrix::Db_Open (string filename)
{
	int i, type;
    char *license = 0;

	if (!Load_DLL ()) return (Status (NO_TPPLUS));
	
	Set_Field_Numbers ();

	if (File_Access () == CREATE) {
		int len;
		unsigned char *header;
		Db_Field *fld_ptr;

		if (Num_Des () == 0 || Tables () == 0) return (false);

		if (Dbase_Format () == TRANPLAN) {
			type = TPLAN;
		} else {
			type = TPP;
		}
		TppMatMatSet (&fh, type, (char *) filename.c_str (), Num_Des (), Tables ());

		header = fh->Mnames;

		for (i=0; i < Tables (); i++) {
			fld_ptr = Table_Field (i);
			if (fld_ptr->Decimal () == 0) {
				fh->Mspecs [i] = 'D';
			} else {
				fh->Mspecs [i] = (unsigned char) fld_ptr->Decimal ();
			}
			len = (int) fld_ptr->Name ().length () + 1;
			memcpy (header, fld_ptr->Name ().c_str (), len);
			header += len;
		}
		if ((i = TppMatOpenOP (fh, (char *) File_ID ().c_str (), (char *) exe->Program (), 0, license, CUBE)) <= 0) {
			if (i < -1) {
				return (Status (TPPLUS_LICENSE));
			} else {
				return (false);
			}
		}
		TppMatMatResize (&fh);
	} else {
		if (FileInquire ((char *) filename.c_str (), &fh) <= 0) return (false);

		if ((i = TppMatOpenIP (fh, license, CUBE)) <= 0) {
			if (i < -1) {
				return (Status (TPPLUS_LICENSE));
			} else {
				return (false);
			}
		}
		Num_Org (fh->zones);
		Num_Des (fh->zones);
		Tables (fh->mats);
	}

	for (i=1; i <= Num_Des (); i++) {
		Add_Org (i);
		Add_Des (i);
	}
	fh->buffer = (void *) new char [fh->bufReq];

	return (true);
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool TPPlus_Matrix::Close (void)
{
	if (fh != 0) {
		delete [] fh->buffer;

	    TppMatClose (fh);
		fh = 0;
	}
	return (Db_Matrix::Close ());
}

//---------------------------------------------------------
//	Rewind
//---------------------------------------------------------

bool TPPlus_Matrix::Rewind (void)
{
	if (fh != 0) {
	    TppMatPos (fh, 0);
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Read_Row
//---------------------------------------------------------

bool TPPlus_Matrix::Read_Row (int org, int period)
{
	if (!Data_Flag ()) {
		if (!Allocate_Data ()) return (false);
	}
	period = 0;

	for (int i=0; i < Tables (); i++) {
		if (TppMatReadSelect (fh, org, i+1, Row_Ptr (i)) != 1) return (false);
	}
	return (true);
}

bool TPPlus_Matrix::Read_Row (void *data, int org, int table, int period)
{
	period = 0;
	return (TppMatReadSelect (fh, org, table, data) == 1);
}

//---------------------------------------------------------
//	Write_Row
//---------------------------------------------------------

bool TPPlus_Matrix::Write_Row (int org, int period)
{
	if (!Data_Flag ()) return (false);
	period = 0;

	for (int i=0; i < Tables (); i++) {
		if (TppMatMatWriteRow (fh, org, i+1, fh->Mspecs [i], Row_Ptr (i)) != 1) return (false);
	}
	return (true);
}

bool TPPlus_Matrix::Write_Row (void *data, int org, int table, int period)
{
	period = 0;
	return (TppMatMatWriteRow (fh, org, table, fh->Mspecs [table-1], data) == 1);
}

//---------------------------------------------------------
//	Read_Matrix
//---------------------------------------------------------

bool TPPlus_Matrix::Read_Matrix (void)
{
	int n;

	Int_Map *org_map;
	Int_Map_Itr org_itr;

	if (!Data_Flag () || !Matrix_Flag ()) {
		if (!Allocate_Data (true)) return (false);
	}
	exe->Show_Message (String ("Reading %s -- Record") % File_Type ());
	exe->Set_Progress ();

	org_map = Org_Map ();

	for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
		for (n=0; n < Tables (); n++) {
			exe->Show_Progress ();
			TppMatReadSelect (fh, org_itr->first, n+1, Matrix_Ptr (0, org_itr->second, n));
		}
	}
	exe->End_Progress ();
	exe->Print (2, String ("Number of %s Records = %d") % File_Type () % exe->Progress_Count ());
	return (true);
}

//---------------------------------------------------------
//	Write_Matrix
//---------------------------------------------------------

bool TPPlus_Matrix::Write_Matrix (void)
{
	int i, n, org;

	Int_Map *org_map;
	Int_Map_Itr org_itr;
	
	if (!Matrix_Flag ()) return (false);

	exe->Show_Message (String ("Writing %s -- Record") % File_Type ());
	exe->Set_Progress ();

	org_map = Org_Map ();

	//---- process each origin zone ----

	for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
		org = org_itr->first - 1;
		i = org_itr->second;

		for (n=0; n < Tables (); n++) {
			exe->Show_Progress ();

			TppMatMatWriteRow (fh, org, n+1, fh->Mspecs [n], Matrix_Ptr (0, i, n));
		}
	}
	exe->End_Progress ();
	exe->Print (2, String ("Number of %s Records = %d") % File_Type () % exe->Progress_Count ());
	return (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool TPPlus_Matrix::Read_Header (bool stat)
{
	int i;
	char *header;
	double size;
	String label;

	//---- extract the table names ----

	stat = false;
	Clear_Fields ();

	header = (char *) fh->Mnames;

	for (i=0; i < Tables (); i++) {
		label = header;
		if (label.empty ()) {
			header++;
			label ("%d") % (i + 1);
		} else {
			header += label.size () + 1;
		}
		size = sizeof (double);
		if (fh->Mspecs [i] != 'D' && fh->Mspecs [i] != 'S') {
			size += fh->Mspecs [i] / 10.0;
		}
		Add_Field (label, DB_DOUBLE, size, NO_UNITS, true);
	}
	Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);

	return (Set_Field_Numbers ());
}
