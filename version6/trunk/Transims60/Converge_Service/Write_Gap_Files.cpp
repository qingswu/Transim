//*********************************************************
//	Write_Gap_Files.cpp - write link and trip gap file
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Write_Link_Gap
//---------------------------------------------------------

void Converge_Service::Write_Link_Gap (double gap, bool end_flag)
{
	if (link_gap_flag) {
		link_gap_file.File () << "\t" << gap;
		if (end_flag) link_gap_file.File () << endl;
	}
}

//---------------------------------------------------------
//	Write_Trip_Gap
//---------------------------------------------------------

void Converge_Service::Write_Trip_Gap (double gap, bool end_flag)
{
	if (trip_gap_flag) {
		trip_gap_file.File () << "\t" << gap;
		if (end_flag) trip_gap_file.File () << endl;
	}
}
