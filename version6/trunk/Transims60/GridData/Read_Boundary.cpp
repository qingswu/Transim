//*********************************************************
//	Read_Boundary.cpp - read the zone boundary files
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Read_Boundary
//---------------------------------------------------------

void GridData::Read_Boundary (void)
{
	int zone;

	Points_Map_Stat map_stat;

	//---- read the arcview boundary file ----

	Show_Message (String ("Reading %s -- Record") % boundary_file.File_Type ());
	Set_Progress ();

	while (boundary_file.Read_Record ()) {
		Show_Progress ();

		zone = boundary_file.Get_Integer (zone_field);

		map_stat = boundary.insert (Points_Map_Data (zone, boundary_file));

		if (!map_stat.second) {
			Error (String ("Insufficient Memory for Zone Boundary %d") % zone);
		}
	}
	End_Progress ();

	Print (2, "Number of Zone Boundary Records = ") << Progress_Count ();
		
	boundary_file.Close ();
}
