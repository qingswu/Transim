//*********************************************************
//	Connectivity_Process.cpp - lane connectivity processing
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Connectivity_Processing
//---------------------------------------------------------

bool Progression::Get_Connect_Data (Connect_File &file, Connect_Data &data)
{
	Int_Map_Stat map_stat;

	if (Data_Service::Get_Connect_Data (file, data)) {

		if (!Link_Equiv_Flag ()) {
			if (data.Type () == THRU) {
				map_stat = thru_connect.insert (Int_Map_Data (data.Dir_Index (), data.To_Index ()));

				if (!map_stat.second) {
					Warning ("Duplicate Thru Connection");
				}
			} else if (!delay_flag) {
				return (false);
			}
		}
		return (true);
	}
	return (false);
}
