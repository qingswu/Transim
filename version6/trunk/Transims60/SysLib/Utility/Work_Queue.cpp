//*********************************************************
//	Work_Queue.cpp - work index queue class
//*********************************************************

#include "Work_Queue.hpp"
#include "Execution_Service.hpp"
#include "String.hpp"

#ifdef THREADS
//---------------------------------------------------------
//	Work_Queue constructor
//---------------------------------------------------------

Work_Queue::Work_Queue (int max_records) 
{
	Start_Work ();
	queue = 0;
	Max_Records (max_records);
}

//---------------------------------------------------------
//	operator ()
//---------------------------------------------------------

void Work_Queue::operator()()
{
	mutex_lock lock (work_mutex);
	exit_wait.wait (lock);
}

//---------------------------------------------------------
//	Put
//---------------------------------------------------------

void Work_Queue::Put (int index) 
{
	mutex_lock lock (work_mutex);

	while (num_records == max_records) {
		work_full.wait (lock);
	}
	num_active++;
	num_records++;
	queue [last++] = index;

	if (last >= max_records) last = 0;

	work_empty.notify_one ();
}

//---------------------------------------------------------
//	Get
//---------------------------------------------------------

int Work_Queue::Get (void) 
{
	while (1) {
		mutex_lock lock (work_mutex);

		while (!end_flag && num_records == 0) {
			work_empty.wait (lock);
		}
		if (num_records == 0) {
			if (end_flag) {
				end_wait.notify_one ();
				return (-1);
			}
			continue;
		}
		num_records--;

		int index = queue [first++];

		if (first >= max_records) first = 0;

		work_full.notify_one ();
		return (index);
	}
}

//--------------------------------------------------------
//	Finished
//--------------------------------------------------------

void Work_Queue::Finished (void)
{
	mutex_lock lock (work_mutex);

	if (num_active > 0) num_active--;

	if (finish_flag && num_active == 0) {
		data_wait.notify_one ();
	}
}

//--------------------------------------------------------
//	Start_Work
//--------------------------------------------------------

void Work_Queue::Start_Work (void)
{
	end_flag = finish_flag = false;
	num_records = num_active = first = last = 0;
}

//--------------------------------------------------------
//	Complete_Work
//--------------------------------------------------------

void Work_Queue::Complete_Work (void)
{
	mutex_lock lock (work_mutex);

	if (num_active > 0) {
		finish_flag = true;
		data_wait.wait (lock);
	}
}

//--------------------------------------------------------
//	End_of_Work
//--------------------------------------------------------

void Work_Queue::End_of_Work (void)
{
	Complete_Work ();

	mutex_lock lock (work_mutex);

	end_flag = true;
	work_empty.notify_all ();
	end_wait.wait (lock);
}

//---------------------------------------------------------
//	Exit_Queue
//---------------------------------------------------------

void Work_Queue::Exit_Queue (void)
{ 
	End_of_Work ();
	exit_wait.notify_one ();
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Work_Queue::Clear (void)
{
	if (queue != 0) {
		delete [] queue;
		queue = 0;
	}
	max_records = 0;
	Start_Work ();
}

//---------------------------------------------------------
//	Max_Records 
//---------------------------------------------------------

bool Work_Queue::Max_Records (int max_rec) 
{
	Clear ();
	if (max_rec > 0) {
		queue = new int [max_rec];
		if (queue == 0) return (false);
		max_records = max_rec;
	}
	return (true);
}
#endif
