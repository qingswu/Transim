//*********************************************************
//	Read_Vehicles.cpp - read the vehicle file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Read_Vehicles
//---------------------------------------------------------

void NewFormat::Read_Vehicles (void)
{
	int hhold, vehicle, type;
	Vehicle_Index veh_index;
	Int_Map_Itr map_itr;

	//---- process the vehicle file ----

	Show_Message (String ("Reading %s -- Record") % vehicle_file.File_Type ());
	Set_Progress ();

	while (vehicle_file.Read ()) {
		Show_Progress ();

		hhold = vehicle_file.Household ();
		if (hhold <= 0) continue;

		vehicle = vehicle_file.Vehicle ();

		if (vehicle_file.Version () <= 40) {
			vehicle = Fix_Vehicle_ID (vehicle);
		}
		type = vehicle_file.Type ();

		if (vehicle_file.SubType_Flag () && vehicle_file.Version () <= 40) {
			type = VehType40_Map (type, vehicle_file.SubType ());
		}
		if (type > 0) {
			map_itr = veh_type_map.find (type);
			if (map_itr == veh_type_map.end ()) {
				if (System_Data_Flag (VEHICLE_TYPE)) {
					Warning (String ("Household %d Vehicle %d Type %d was Not Found") % hhold % vehicle % type);
					type = 0;
				}
			} else {
				type = map_itr->second;
			}
		} else {
			type = 0;
		}

		veh_index.Household (hhold);
		veh_index.Vehicle (vehicle);

		vehicle_type.insert (Vehicle_Map_Data (veh_index, type));
	}
	End_Progress ();
	vehicle_file.Close ();

	Print (2, String ("Number of %s Records = %d") % vehicle_file.File_Type () % Progress_Count ());
}
