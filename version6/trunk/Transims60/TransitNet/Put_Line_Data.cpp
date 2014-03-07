//*********************************************************
//	Put_Line_Data.cpp - write a new transit route file
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Put_Line_Data
//---------------------------------------------------------

int TransitNet::Put_Line_Data (Line_File &file, Line_Data &data)
{
	if (route_data_flag) {
		int id = data.Route ();

		if (route_join_field >= 0) {
			Str_ID_Itr id_itr = join_map.find (data.Name ());

			if (id_itr != join_map.end ()) {
				id = id_itr->second;
			}
		}
		if (route_data_array.Read_Record (id)) {
			file.Copy_Fields (route_data_array);
		}
	}
	return (Data_Service::Put_Line_Data (file, data));
}
