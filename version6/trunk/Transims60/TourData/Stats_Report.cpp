//*********************************************************
//	Stats_Report.cpp - Field Statistic Report
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Group_Stats_Report
//---------------------------------------------------------

void TourData::Group_Stats_Report (void)
{
	double in_len, out_len, percent, input, target, moved, in_tot, out_tot;
	String des_list;

	Stats_Map_Itr stats_itr;
	Stats_Index index;
	Group_Stats *data;
	Int_Set_Itr des_itr;

	Show_Message ("Zone Group Statitics Report -- Record");
	Set_Progress ();

	Header_Number (GROUP_STATS);
	New_Page ();

	input = target = moved = in_tot = out_tot = 0;

	for (stats_itr = group_stats.begin (); stats_itr != group_stats.end (); stats_itr++) {
		Show_Progress ();

		index = stats_itr->first;
		data = &stats_itr->second;

		in_len = data->in_len;
		out_len = data->out_len;

		in_tot += in_len;
		out_tot += out_len;

		input += data->input;
		target += data->target;
		moved += data->moved;

		if (data->moved > 0) {
			in_len /= data->moved;
			out_len /= data->moved;
		}
		if (in_len > 0) {
			percent = 100.0 * (out_len - in_len) / in_len;
		} else {
			percent = 0.0;
		}
		des_list.clear ();
		for (des_itr = data->new_des.begin (); des_itr != data->new_des.end (); des_itr++) {
			if (des_list.empty ()) {
				des_list += (String ("%d") % *des_itr);
			} else {
				des_list += (String (", %d") % *des_itr);
			}
		}
		Print (1, String ("%4d %4d %4d %7d %7d %7d %7.0lf %7.0lf %6.1lf%% ") %
			index.origin % index.destination % index.type %
			data->input % data->target % data->moved %
			in_len % out_len % percent % FINISH);
		Print (0, " ") << des_list;
	}
	End_Progress ();

	if (moved > 0) {
		in_tot /= moved;
		out_tot /= moved;
	}
	if (in_tot > 0) {
		percent = 100.0 * (out_tot - in_tot) / in_tot;
	} else {
		percent = 0.0;
	}
	Print (2, String ("         Total %7.0lf %7.0lf %7.0lf %7.0lf %7.0lf %6.1lf%%") %
		input % target % moved % in_tot % out_tot % percent % FINISH);

	Header_Number (0);
}

//---------------------------------------------------------
//	Group_Stats_Header
//---------------------------------------------------------

void TourData::Group_Stats_Header (void)
{
	Print (1, "Zone Group Statistics");

	Print (2, "--Tour Group--   Input    Move  Number   Input  Output Percent  New");
	Print (1, " Org  Des Type   Tours  Target   Moved  Length  Length  Change  Destination Groups");
	Print (1);
}

/*********************************************|***********************************************

	Zone Group Statistics

	--Tour Group--   Input    Move  Number   Input  Output Percent  New 
	 Org  Des Type   Tours  Target   Moved  Length  Length  Change  Destination Groups

	dddd dddd dddd ddddddd ddddddd ddddddd ddddddd ddddddd  ddd.d%  sssssssssssssssssssssss

	         Total

**********************************************|***********************************************/ 
