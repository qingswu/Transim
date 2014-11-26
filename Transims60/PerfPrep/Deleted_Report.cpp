//*********************************************************
//	Deleted_Report.cpp - deleted performance report
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Deleted_Report
//---------------------------------------------------------

void PerfPrep::Deleted_Report (Performance_File &file)
{
	int link, dir;
	Dtime start, end;
	double volume, flow, ratio;

	link = file.Link ();
	if (link == 0) return;

	dir = file.Dir ();
	start = file.Start ();
	end = file.End ();
	volume = file.Volume ();
	flow = file.Flow ();
	ratio = file.Time_Ratio () / 100.0;

	Print (1, String ("%10d %2d  %7s  %7s %10.1lf %9.1  %9.2lf") % link % dir % 
		start.Time_String () % end.Time_String () % volume % flow % ratio);
}

//---------------------------------------------------------
//	Deleted_Header
//---------------------------------------------------------

void PerfPrep::Deleted_Header (void)
{
	Print (1, "Deleted Performance Records");
	Print (2, "      Link Dir   Start      End    Volume      Flow  Time_Ratio");
	Print (1);
}

/*********************************************|***********************************************

	Deleted Performance Records

	      Link Dir   Start      End    Volume      Flow  Time_Ratio 
	
	dddddddddd dd  xxxx:xx  xxxx:xx ffffffff.f fffffff.f  ffffff.ff


**********************************************|***********************************************/ 
