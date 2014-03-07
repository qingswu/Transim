//*********************************************************
//	Threads.hpp - standard thread definitions
//*********************************************************

#ifndef THREADS_HPP
#define THREADS_HPP

#ifdef THREADS

#include <thread>
#include <mutex>
#include <condition_variable>

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Threads class definition
//---------------------------------------------------------

class Threads : public vector <thread>
{
public:
	Threads (void)  { }

	void Join_All (void) {
		for (vector<thread>::iterator itr = begin (); itr != end (); itr++) itr->join ();
	}
};
typedef Threads::iterator            Thread_Itr;
typedef Threads::reverse_iterator    Thread_RItr;
typedef Threads::pointer             Thread_Ptr;

typedef unique_lock<mutex>           mutex_lock;
#endif
#endif
