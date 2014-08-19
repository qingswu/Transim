//*********************************************************
//	Bounded_Queue.hpp - fixed length circular queue
//*********************************************************

#ifdef THREADS
#ifndef BOUNDED_QUEUE_HPP
#define BOUNDED_QUEUE_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Bounded_Queue -- template class definition
//---------------------------------------------------------
//	Bounded_Queue <Type> (int max_records = 200);
//
//  void operator ();
//
//	void Put (Type &data);
//  bool Get (Type &data);
//	void Finished (void);
//
//	void Start_Work (void);
//	void Complete_Work (void);
//	void End_of_Work (void);
//	void Exit_Queue (void);
//
//  int  Num_Records (void);
//	bool Max_Records (int max_records);
//---------------------------------------------------------

template <typename Type>
class SYSLIB_API Bounded_Queue
{
	typedef vector <Type> Type_Array;

public:

	//---------------------------------------------------------
	//	Bounded_Queue constructor
	//---------------------------------------------------------

	Bounded_Queue (int _max_records = 200)
	{
		Start_Work ();
		Max_Records (_max_records);
	}

	//---------------------------------------------------------
	//	operator ()
	//---------------------------------------------------------

	void operator()()
	{
		mutex_lock lock (work_mutex);
		exit_wait.wait (lock);
	}

	//---------------------------------------------------------
	//	Put
	//---------------------------------------------------------

	void Put (Type &data)
	{
		mutex_lock lock (work_mutex);

		while (num_records == max_records) {
			work_full.wait (lock);
		}
		num_active++;
		num_records++;
		type_array [last++] = data;

		if (last >= max_records) last = 0;

		work_empty.notify_one ();
	}

	//---------------------------------------------------------
	//	Get
	//---------------------------------------------------------

	bool Get (Type &data)
	{
		while (1) {
			mutex_lock lock (work_mutex);

			while (!end_flag && num_records == 0) {
				work_empty.wait (lock);
			}
			if (num_records == 0) {
				if (end_flag) {
					end_wait.notify_one ();
					return (false);
				}
				continue;
			}
			num_records--;

			data = type_array [first++];

			if (first >= max_records) first = 0;

			work_full.notify_one ();
			return (true);
		}
	}

	//--------------------------------------------------------
	//	Finished
	//--------------------------------------------------------

	void Finished (void)
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

	void Start_Work (void)
	{
		end_flag = finish_flag = false;
		num_records = num_active = first = last = 0;
	}

	//--------------------------------------------------------
	//	Complete_Work
	//--------------------------------------------------------

	void Complete_Work (void)
	{
		mutex_lock lock (work_mutex);
	
		while (num_active > 0) {
			finish_flag = true;
			data_wait.wait (lock);
		}
	}

	//--------------------------------------------------------
	//	End_of_Work
	//--------------------------------------------------------

	void End_of_Work (void)
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

	void Exit_Queue (void)
	{ 
		End_of_Work ();
		exit_wait.notify_one ();
	}

	//---------------------------------------------------------
	//	Num_Records
	//---------------------------------------------------------

	int Num_Records (void)
	{
		return (num_records);
	}

	//---------------------------------------------------------
	//	Max_Records
	//---------------------------------------------------------

	bool Max_Records (int max_size)
	{
		type_array.resize (max_size);
		max_records = (int) type_array.size ();
		return (max_records > 0);
	}

private:

	//---- data ----

	int num_records, num_active, max_records, first, last;
	bool end_flag, finish_flag;

	condition_variable  work_full, work_empty, exit_wait, end_wait, data_wait;
	mutex  work_mutex;

	Type_Array type_array;
};

#endif
#endif
