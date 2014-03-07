//*********************************************************
//	Write_Subzone.cpp - subzone data processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Write_Subzone
//---------------------------------------------------------

void ArcNet::Write_Subzone (void)
{
	XYZ_Point *point;

	int z_field = subzone_file.Optional_Field ("Z_COORD", "ELEVATION", "Z");

	arcview_subzone.Num_Points (1);

	Show_Message (String ("Reading %s -- Record") % subzone_file.File_Type ());
	Set_Progress ();

	while (subzone_file.Read ()) {
		Show_Progress ();

		arcview_subzone.Copy_Fields (subzone_file);

		point = arcview_zone.Get_Points ();

		point->x = subzone_file.X ();
		point->y = subzone_file.Y ();
		point->z = subzone_file.Get_Double (z_field);

		if (!arcview_subzone.Write_Record ()) {
			Error ("Writing ArcView Subzone File");
		}
	}
	End_Progress ();
	
	subzone_file.Close ();
	arcview_subzone.Close ();

	Print (2, String ("Number of %s Records = %d") % subzone_file.File_Type () % Progress_Count ());
}
