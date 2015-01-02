//*********************************************************
//	Get_Zone_Data.cpp - read the zone file
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool PathSkim::Get_Zone_Data (Zone_File &file, Zone_Data &zone_rec)
{
	if (Data_Service::Get_Zone_Data (file, zone_rec)) {
		if (accessibility_flag) {
			double wt = file.Get_Double (org_wt_fld);
			org_wt.push_back (wt);

			wt = file.Get_Double (des_wt_fld);
			des_wt.push_back (wt);
		}
	}
	return (true);
}
