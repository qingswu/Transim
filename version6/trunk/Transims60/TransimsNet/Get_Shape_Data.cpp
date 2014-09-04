//*********************************************************
//	Get_Shape_Data.cpp - read the shape point file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Shape_Data
//---------------------------------------------------------

bool TransimsNet::Get_Shape_Data (Shape_File &file, Shape_Data &shape_rec)
{
	if (!file.Nested ()) {
		if (delete_link_flag && delete_link_range.In_Range (file.Link ())) {
			xshape++;
			return (false);
		}
	}
	return (Data_Service::Get_Shape_Data (file, shape_rec));
}
