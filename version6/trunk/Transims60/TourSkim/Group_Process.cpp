//*********************************************************
//	Group_Process.cpp - group processing class
//*********************************************************

#include "TourSkim.hpp"

//---------------------------------------------------------
//	Group_Process
//---------------------------------------------------------

TourSkim::Group_Process::Group_Process (TourSkim *_exe) 
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	pnr_data = ttime = 0;
	row_id = col_id = 0;
}

TourSkim::Group_Process::~Group_Process (void)
{
	if (pnr_data != 0) {
		delete [] pnr_data;
	}
	if (ttime != 0) {
		delete [] ttime;
	}
	if (row_id != 0) {
		delete [] row_id;
	}
	if (col_id != 0) {
		delete [] col_id;
	}
}

void TourSkim::Group_Process::operator()() 
{
	int index = 0;

	pnr_data = new float [exe->num_zones];
	ttime = new float [exe->num_zones];
	row_id = new long [exe->num_zones];
	col_id = new long [exe->num_zones];

	while (exe->group_queue.Get (index)) {
		Process (index);
	}
}
