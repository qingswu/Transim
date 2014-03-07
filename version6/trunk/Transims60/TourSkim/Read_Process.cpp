//*********************************************************
//	Read_Process.cpp - read processing class
//*********************************************************

#include "TourSkim.hpp"

//---------------------------------------------------------
//	Read_Process
//---------------------------------------------------------

TourSkim::Read_Process::Read_Process (TourSkim *_exe) 
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
}

void TourSkim::Read_Process::operator()() 
{
	int index = 0;

	while (exe->read_queue.Get (index)) {
		Read_Data (index);
	}
}
