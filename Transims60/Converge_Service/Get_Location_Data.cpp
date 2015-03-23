//*********************************************************
//	Get_Location_Data.cpp - Read the Location File
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool Converge_Service::Get_Location_Data (Location_File &file, Location_Data &data)
{
	if (Data_Service::Get_Location_Data (file, data)) {
		if (capacity_flag) {
			int next, num;
			double share, total;
			Loc_Cap_Data loc_cap_data;
			Int_Itr itr;

			loc_cap_data.capacity = file.Get_Integer (loc_cap_field);
			loc_cap_data.demand = 0;
			loc_cap_data.failed = 0;
			num = 0;

			for (itr = next_des_field.begin (); itr != next_des_field.end (); itr++) {
				next = file.Get_Integer (*itr);
				if (next > 0) {
					loc_cap_data.next_des.push_back (next);
					num++;
				}
			}
			if (num > 1) {
				if (des_share_field.size () > 0) {
					total = 0.0;

					for (next=0; next < num; next++) {
						if (next < (int) des_share_field.size ()) {
							share = file.Get_Double (des_share_field [next]);
							if (share == 0) share = 1.0;
						} else {
							share = 1.0;
						}
						loc_cap_data.shares.push_back (share);
						total += share;
					}
					if (total != 1.0) {
						if (total > 0.0) {
							for (next=0; next < num; next++) {
								loc_cap_data.shares [next] /= total;
							}
						} else {
							share = 1.0 / num;
							for (next=0; next < num; next++) {
								loc_cap_data.shares [next] = share;
							}
						}
					}
				} else {
					share = 1.0 / num;
					for (next=0; next < num; next++) {
						loc_cap_data.shares.push_back (share);
					}
				}
			}
			loc_cap_array.push_back (loc_cap_data);
		}
		if (fuel_flag) {
			Loc_Fuel_Data loc_fuel_data;
			Int_Itr itr;

			loc_fuel_data.supply = Round (file.Get_Integer (loc_fuel_field));
			loc_fuel_data.consumed = 0;
			loc_fuel_data.failed = 0;

			loc_fuel_array.push_back (loc_fuel_data);
		}
		return (true);
	}
	return (false);
}
