//*********************************************************
//	Read_Shape_Data.cpp - read the shape point file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Shape_Data
//---------------------------------------------------------

void TransimsNet::Read_Shape_Data (void)
{
	int i, num;	

	Shape_Data shape_rec;
	Int_Map_Stat map_stat;

	//---- store the shape point data ----

	Show_Message (String ("Reading %s -- Record") % shape_data_file.File_Type ());
	Set_Progress ();

	while (shape_data_file.Read (false)) {
		Show_Progress ();

		shape_rec.Clear ();

		Data_Service::Get_Shape_Data (shape_data_file, shape_rec);

		num = shape_data_file.Num_Nest ();
		if (num > 0) shape_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!shape_data_file.Read (true)) {
				Error (String ("Number of Nested Records for Link %d") % shape_data_file.Link ());
			}
			Show_Progress ();

			Data_Service::Get_Shape_Data (shape_data_file, shape_rec);
		}
		map_stat = shape_map.insert (Int_Map_Data (shape_rec.Link (), (int) shape_array.size ()));

		if (!map_stat.second) {
			shape_array [map_stat.first->second] = shape_rec;
		} else {
			shape_array.push_back (shape_rec);
		}
	}
	End_Progress ();
	shape_data_file.Close ();
}
