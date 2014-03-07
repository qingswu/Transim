//*********************************************************
//	Read_Points.cpp - Read the Point Data File into Memory
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Read_Points
//---------------------------------------------------------

void GridData::Read_Points (void)
{

	//---- read the point data file ----

	Show_Message (String ("Reading %s -- Record") % point_file.File_Type ());
	Set_Progress ();

	while (point_file.Read ()) {
		Show_Progress ();

		//---- copy the data fields ----

		point_data.Copy_Fields (point_file);

		//---- save the database record ----

		if (!point_data.Write_Record ()) {
			Error ("Writing Point Data File Database");
		}
	}
	End_Progress ();

	Print (2, "Number of Point Data Records = ") << Progress_Count ();
		
	point_file.Close ();
}
