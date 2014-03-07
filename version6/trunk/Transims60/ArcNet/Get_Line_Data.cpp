//*********************************************************
//	Get_Line_Data.cpp - read the transit route file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Line_Data
//---------------------------------------------------------

bool ArcNet::Get_Line_Data (Line_File &file, Line_Data &line_rec)
{
	if (Data_Service::Get_Line_Data (file, line_rec)) {
		if (!file.Nested ()) {
			line_db.Copy_Fields (file);
			line_db.Write_Record (line_rec.Route ());
		}
		return (true);
	}
	return (false);
}
