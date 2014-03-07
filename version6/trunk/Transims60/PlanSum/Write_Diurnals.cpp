//*********************************************************
//	Write_Diurnals.cpp - write stop diurnal file
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Stop_Diurnals
//---------------------------------------------------------

void PlanSum::Write_Stop_Diurnals (void)
{
	int p, row, num_rows, last_group, num_periods, trips;
	int first_board, xfer_board, total_board, last_alight, xfer_alight, total_alight;

	Xfer_IO_Map_Itr map_itr;
	Xfer_IO od;
	Int_Itr int_itr;
	Integers io_time, *io_ptr;
	Ints_Array io_trips;
	Strings types;

	Show_Message ("Writing Stop Diurnals -- Record");
	Set_Progress ();

	last_group = 0;
	num_periods = sum_periods.Num_Periods ();
	if (num_periods < 1) num_periods = 1;
	trips = 0;
	memset (&od, '\0', sizeof (od));

	fstream &file = diurnal_file.File ();

	//---- print the header ----

	file << "Group\tName\tType";
	for (p=0; p < num_periods; p++) {
		file << "\t" + sum_periods.Range_Label (p);
	}
	file << "\tTotal" << endl;

	first_board = 0;
	xfer_board = 1;
	total_board = 2;
	last_alight = 3;
	xfer_alight = 4;
	total_alight = 5;

	types.push_back ("First Boardings");
	types.push_back ("Transfer Boardings");
	types.push_back ("Total Boardings");

	types.push_back ("Last Alightings");
	types.push_back ("Transfer Alightings");
	types.push_back ("Total Alightings");

	num_rows = (int) types.size ();

	io_time.assign (num_periods, 0);
	io_trips.assign (types.size (), io_time);

	for (map_itr = access_detail.begin (); ; map_itr++) {
		Show_Progress ();

		if (map_itr != access_detail.end ()) {
			od = map_itr->first;
		}
		if (map_itr == access_detail.end () || last_group != od.group) {
			if (last_group > 0) {
				for (row=0; row < num_rows; row++) {
					file << last_group << "\t" << stop_equiv.Group_Label (last_group);
					file << "\t" << types [row];

					io_ptr = &io_trips [row];
					trips = 0;

					for (int_itr = io_ptr->begin (); int_itr != io_ptr->end (); int_itr++) {
						file << "\t" << *int_itr;
						trips += *int_itr;
					}
					file << "\t" << trips << endl;
					io_ptr->assign (num_periods, 0);
				}
			}
			if (map_itr == access_detail.end ()) break;

			last_group = od.group;
		}
		trips = map_itr->second;

		if (od.from_line == 0) {
			if (od.to_line > 0) {
				io_trips [first_board] [od.period] += trips;
				io_trips [total_board] [od.period] += trips;
			}
		} else if (od.to_line > 0) {
			io_trips [xfer_board] [od.period] += trips;
			io_trips [total_board] [od.period] += trips;
			io_trips [xfer_alight] [od.period] += trips;
			io_trips [total_alight] [od.period] += trips;
		} else {
			io_trips [last_alight] [od.period] += trips;
			io_trips [total_alight] [od.period] += trips;
		}
	}
	diurnal_file.Close ();
	End_Progress ();
}
