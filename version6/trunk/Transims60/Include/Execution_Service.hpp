//*********************************************************
//	Execution_Service.hpp - general execution class
//*********************************************************

#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include "APIDefs.hpp"
#include "Control_Service.hpp"
#include "Dtime.hpp"
#include "Random.hpp"
#include "Time_Periods.hpp"
#include "Functions.hpp"

//---------------------------------------------------------
//	Execution_Service - general execution class
//---------------------------------------------------------

class SYSLIB_API Execution_Service : public Control_Service
{
	friend class Control_Service;

public:
	Execution_Service (void);

	int    Service_Level (void)              { return (service_level); }
	void   Service_Level (int code)          { service_level = code; }

	void   Key_List (Control_Key keys []);
	void   Report_List (const char *names []);

	void   Ignore_Keys (int *keys);

	bool   Enable_Threads (void)             { return (thread_flag); }
	void   Enable_Threads (bool flag)        { thread_flag = flag; }
	
	bool   Enable_MPI (void)                 { return (mpi_flag); }
	void   Enable_MPI (bool flag)            { mpi_flag = flag; }

	void   Project_Directory (string dir);
	String Project_Directory (void)          { return (project_directory); }

	String Project_Filename (string name)    { return (Project_Directory () + name); }
	String Project_Filename (string name, string ext)
	                                         { return (Project_Directory () + name + ext); }
	Random random;
	int    Random_Seed (void)                { return (random_seed); }

	Function_Array functions;

	virtual void Execute (void);

	int    Start_Execution (String command_line);
	int    Start_Execution (int commands, char *command []);
	int    Start_Execution (Strings keys);

	int    Num_Threads (void)                { return (num_threads); }
	void   Num_Threads (int value)           { num_threads = MAX (value, 1); }

	int    Sub_Threads (void)                { return (sub_threads); }
	void   Sub_Threads (int value)           { sub_threads = MAX (value, 1); }

	bool   Memory_Flag (void)                { return (memory_flag); }
	void   Memory_Flag (bool flag)           { memory_flag = flag; }

	void   Notes_Name_Flag (bool flag)       { notes_flag = flag; }
	bool   Notes_Name_Flag (void)            { return (notes_flag); }

	Dtime  Model_Start_Time (void)           { return (start_time); }
	void   Model_Start_Time (string value)   { start_time = value; }
	void   Model_Start_Time (Dtime value)    { start_time = value; }

	Dtime  Model_End_Time (void)             { return (end_time); }
	void   Model_End_Time (string value)     { end_time = value; }
	void   Model_End_Time (Dtime value)      { end_time = value; }

	Time_Periods time_periods;

	void   XML_Open (void);
	void   XML_Help (void);

	void   Document (bool user_flag = false);

	bool debug;

protected:
	enum Execution_Service_Keys { TITLE = EXECUTION_OFFSET, REPORT_DIRECTORY, REPORT_FILE, REPORT_FLAG, PAGE_LENGTH, 
		PROJECT_DIRECTORY, DEFAULT_FILE_FORMAT, TIME_OF_DAY_FORMAT, MODEL_START_TIME, MODEL_END_TIME, 
		MODEL_TIME_INCREMENT, UNITS_OF_MEASURE, DRIVE_SIDE_OF_ROAD, RANDOM_NUMBER_SEED, MAX_WARNING_MESSAGES, 
		MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, NUMBER_OF_THREADS, USER_FUNCTIONS, PROGRAM_REPORTS };

	virtual void Program_Control (void);
	void List_Reports (void);

private:

	int  Execution (Strings &commands);
	bool Command_Line (Strings &commands);
	void Show_Syntax (void);

	void Syntax_Help (void);

	void Read_Reports (void);
	bool report_flag, notes_flag;

	String report_key;
	String project_directory;

	int num_threads, sub_threads, service_level, random_seed;
	bool thread_flag, mpi_flag, memory_flag;

	String filename;
	Dtime start_time, end_time;
};

extern SYSLIB_API Execution_Service *exe;

#endif

