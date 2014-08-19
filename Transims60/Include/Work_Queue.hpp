//*********************************************************
//	Work_Queue.hpp - work index queue class
//*********************************************************

#ifdef THREADS
#ifndef WORK_QUEUE_HPP
#define WORK_QUEUE_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"

//---------------------------------------------------------
//	Work_Queue class
//---------------------------------------------------------

class SYSLIB_API Work_Queue
{
public:
	Work_Queue (int max_records = 200);
	~Work_Queue (void)                    { Clear (); }

	void operator()();

	void Put (int index);
	int  Get (void);
	void Finished (void);

	void Start_Work (void);
	void Complete_Work (void);
	void End_of_Work (void);
	void Exit_Queue (void);

	void Clear (void);
	bool Max_Records (int max_rec);

private:
	bool end_flag, finish_flag;
	int *queue, num_records, num_active, max_records, first, last;

	condition_variable  work_full, work_empty, exit_wait, end_wait, data_wait;
	mutex  work_mutex;
};
#endif
#endif
