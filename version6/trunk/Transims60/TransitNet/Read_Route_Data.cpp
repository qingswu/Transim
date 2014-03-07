//*********************************************************
//	Read_Route_Data.cpp - Read the Route Data into Memory
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Read_Route_Data
//---------------------------------------------------------

void TransitNet::Read_Route_Data (void)
{
	int id, count;
	String name;
	Str_ID_Stat id_stat;

	Show_Message (String ("Reading %s -- Record") % route_data_file.File_Type ());
	Set_Progress ();

	count = id = 0;

	while (route_data_file.Read ()) {
		Show_Progress ();

		//---- get the join field id ----

		if (route_join_field >= 0) {
			name = route_data_file.Get_String (route_join_field);
			name.Trim ();

			id_stat = join_map.insert (Str_ID_Data (name, ++id));

			if (!id_stat.second) {
				Warning (String ("Duplicate Route Join String = ") % name);
				id--;
				continue;
			}
		} else {
			id = route_data_file.Get_Integer (route_data_field);
			if (id == 0) continue;
		}

		//---- copy the data fields ----

		route_data_array.Copy_Fields (route_data_file);

		//---- save the database record ----

		if (!route_data_array.Write_Record (id)) {
			Error ("Writing Route Data File Database");
		}
		count++;
	}
	End_Progress ();

	Print (2, "Number of Route Data Records = ") << count;
		
	route_data_file.Close ();
}
