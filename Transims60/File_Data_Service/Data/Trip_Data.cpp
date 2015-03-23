//********************************************************* 
//	Trip_Data.hpp - trip data classes
//*********************************************************

#include "Trip_Data.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Internal_IDs
//---------------------------------------------------------

bool Trip_Data::Internal_IDs (void)
{
	Int_Map_Itr map_itr;

	if (Transit_Mode_Flag () && !dat->System_File_Flag (TRANSIT_ROUTE)) return (false);

	map_itr = dat->location_map.find (Origin ());

	if (map_itr == dat->location_map.end ()) {
		exe->Warning (String ("Trip %d-%d-%d-%d Origin %d was Not Found") % Household () % 
			Person () % Tour () % Trip () % Origin ());
		return (false);
	} 
	Origin (map_itr->second);
		
	map_itr = dat->location_map.find (Destination ());

	if (map_itr == dat->location_map.end ()) {
		exe->Warning (String ("Trip %d-%d-%d-%d Destination %d was Not Found") % Household () % 
			Person () % Tour () % Trip () % Destination ());
		return (false);
	}
	Destination (map_itr->second);

	if (Vehicle () == 0) {
		Vehicle (-1);
		Veh_Type (-1);
	}
	if (Veh_Type () > 0) {
		map_itr = dat->veh_type_map.find (Veh_Type ());

		if (map_itr == dat->veh_type_map.end ()) {
			exe->Warning (String ("Trip %d-%d-%d-%d Vehicle Type %d was Not Found") % Household () % 
				Person () % Tour () % Trip () % Veh_Type ());
			return (false);
		}
		Veh_Type (map_itr->second);
	}
	return (true);
}

//---------------------------------------------------------
//	External_IDs
//---------------------------------------------------------

bool Trip_Data::External_IDs (void)
{
	if (Origin () < 0 || Origin () >= (int) dat->location_array.size ()) return (false);
	if (Destination () < 0 || Destination () >= (int) dat->location_array.size ()) return (false);

	Origin (dat->location_array [Origin ()].Location ());
	Destination (dat->location_array [Destination ()].Location ());

	if (Vehicle () < 0 || Veh_Type () < 0) {
		Vehicle (0);
		Veh_Type (0);
	} else {
		if (Veh_Type () >= (int) dat->veh_type_array.size ()) return (false);
		Veh_Type (dat->veh_type_array [Veh_Type ()].Type ());
	}
	return (true);
}
