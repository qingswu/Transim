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
		type = vehicle_file.Type ();

		if (vehicle_file.Version () <= 40) {
			type = VehType40_Map (type, vehicle_file.SubType ());

			Veh_ID_Map_Itr itr = vehicle40_map.find (vehicle);
			if (itr != vehicle40_map.end ()) {
				Warning ("Duplicate Vehicle ID ") << vehicle;
				continue;
			}
			int veh = 1;
			veh_index.Household (hhold);
			veh_index.Vehicle (veh);

			Vehicle_Map_Itr map_itr;

			for (map_itr = vehicle_type.find (veh_index); map_itr != vehicle_type.end (); map_itr++) {
				veh_index = map_itr->first;
				if (veh_index.Household () != hhold) break;
				veh = veh_index.Vehicle () + 1;
			}
			veh_index.Household (hhold);
			veh_index.Vehicle (veh);

			vehicle40_map.insert (Veh_ID_Map_Data (vehicle, veh_index));
		} else {
			veh_index.Household (hhold);
			veh_index.Vehicle (vehicle);
		}
		vehicle_type.insert (Vehicle_Map_Data (veh_index, type));
	}
	End_Progress ();
	vehicle_file.Close ();

	Print (2, String ("Number of %s Records = %d") % vehicle_file.File_Type () % Progress_Count ());
}
