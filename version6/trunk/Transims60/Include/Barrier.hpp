//*********************************************************
//	Barrier.hpp - coordinated thread barrier
//*********************************************************

#ifndef BARRIER_HPP
#define BARRIER_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"

//---------------------------------------------------------
//	Barrier Class definition
//---------------------------------------------------------

class SYSLIB_API Barrier
{
public:
	Barrier (int num_barrier = 0);

	bool Go (int &num);
	bool Go (void)                          { int num; return (Go (num)); }

	void Num_Barriers (int num)             { num_barrier = num; }
	int  Num_Barriers (void)                { return (num_barrier); }

	bool Start (void);
	bool Finish (bool status = false);
	bool Result (bool status = false);
	void Exit (void);

private:
	int num_barrier;
	bool exit_flag;

#ifdef THREADS
	int num_go, num_end, end_id;
	bool start_flag, finish_flag;
	
	condition_variable  start_wait, finish_wait;
	mutex  barrier_mutex;
#endif
};
#endif

