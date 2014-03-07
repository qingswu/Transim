//*********************************************************
//	Read_Factors.cpp - read segment speed factor file
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Speed_Factors
//---------------------------------------------------------

void RoutePrep::Read_Speed_Factors (void)
{
	int segment;
	double factor;

	Show_Message (String ("Reading %s -- Record") % seg_fac_file.File_Type ());
	Set_Progress ();

	while (seg_fac_file.Read ()) {
		Show_Progress ();

		segment = seg_fac_file.Get_Integer (segment_field);

		if (segment == 0) continue;

		factor = seg_fac_file.Get_Double (speed_fac_field);

		segment_factors.insert (Int_Dbl_Map_Data (segment, factor));
	}
	End_Progress ();

	seg_fac_file.Close ();

	Print (2, String ("Number of %s Records = %d") % seg_fac_file.File_Type () % Progress_Count ());
}
