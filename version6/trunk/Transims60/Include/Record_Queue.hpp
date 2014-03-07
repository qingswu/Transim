//*********************************************************
//	Record_Queue.hpp - queue of record numbers
//*********************************************************

#ifndef RECORD_QUEUE_HPP
#define RECORD_QUEUE_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"

class SYSLIB_API Record_Queue
{
public:
	Record_Queue (int num_records = 0);
	~Record_Queue (void);

	void Put (int record);
	int  Get (void);

	bool Check (int record);
	void Reset (void);

	bool Num_Records (int num_records);
	int  Num_Records (void)                { return (num_records); }

	void End_Queue (void);
	void Exit_Queue (void);

private:
	bool exit_queue, end_queue;
	int *records, num_records, first, last;

#ifdef THREADS
	condition_variable  queue_empty, end_wait;
	mutex  queue_mutex;
#endif
};

#endif

