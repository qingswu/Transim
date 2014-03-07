//*********************************************************
//	Problem_Output.hpp - Output Interface Class
//*********************************************************

#ifndef PROBLEM_OUTPUT_HPP
#define PROBLEM_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Problem_Data.hpp"
#include "Problem_File.hpp"
#include "Data_Buffer.hpp"

#ifdef THREADS
#include "Bounded_Queue.hpp"

typedef Bounded_Queue <Problem_Data> Problem_Queue;
#endif

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Problemt_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Problem_Output : public Static_Service
{
public:
	Problem_Output (void);
	~Problem_Output (void);
	
	void operator()();

	bool Read_Control (void);

	void Output_Problem (Problem_Data &problem);
	
	bool Output_Flag (void)            { return (output_flag); }
	void Output_Flag (bool flag)       { output_flag = flag; }

	void End_Output (void);

private:

	Problem_File *file;

	void Write_Problem (Problem_Data &problem);

	bool output_flag;

#ifdef MPI_EXE
public:
	void MPI_Processing (void);
private:
	Data_Buffer data;
	mutex  data_mutex;
#else
 #ifdef THREADS
	Problem_Queue *problem_queue;
 #endif
#endif
};
#endif
