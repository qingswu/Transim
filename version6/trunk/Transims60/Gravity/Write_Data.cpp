//*********************************************************
//	Write_Data.cpp - write trip distribution data
//*********************************************************

#include "Gravity.hpp"

//---------------------------------------------------------
//	Write_Data
//---------------------------------------------------------

void Gravity::Write_Data (Db_Header &file, Doubles &distb)
{
	double skim;

	Dbl_Itr dbl_itr;

	Show_Message (String ("Writing %s -- Record") % file.File_Type ());;
	Set_Progress ();

	for (skim=0.0, dbl_itr = distb.begin (); dbl_itr != distb.end (); dbl_itr++, skim += increment) {
		Show_Progress ();

		file.Put_Field (0, skim);
		file.Put_Field (1, *dbl_itr);

		file.Write ();
	}
	End_Progress ();

	file.Close ();
}

