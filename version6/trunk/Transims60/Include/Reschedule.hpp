//*********************************************************
//	Reschedule.hpp - reschedule transit routes
//*********************************************************

#ifndef RESCHEDULE_HPP
#define RESCHEDULET_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"

//---------------------------------------------------------
//	Reschedule - execution class definition
//---------------------------------------------------------

class SYSLIB_API Reschedule : public Data_Service, public Select_Service
{
public:
	Reschedule (void);

	virtual void Execute (void);

protected:
	enum Reschedule_Keys { RUN_SCHEDULE_FILE = 1, RUN_SCHEDULE_FORMAT, 
		RUN_FILTER_FIELD, RUN_TYPE_FIELD, RUN_SCHEDULE_LINE, RUN_FILTER_RANGE,
		RUN_STOP_FIELD_FILE, PERFORMANCE_UPDATE_FILE, PERFORMANCE_UPDATE_FORMAT,
	};

	virtual void Program_Control (void);

private:
	enum Reschedule_Reports { MATCH_DUMP = 1, };

	bool run_flag, match_dump, update_flag;

	Performance_File update_file;
	Perf_Period_Array update_array;
	
	typedef struct {
		int line;
		int low;
		int high;
		Int_Map stop_field;
	} Line_Filter;

	typedef vector <Line_Filter>   Filter_Array;
	typedef Filter_Array::iterator Filter_Itr;

	typedef struct {
		Db_Header *run_file;
		int filter;
		int type;
		Filter_Array lines;
	} File_Data;

	typedef vector <File_Data>   File_Group;
	typedef File_Group::iterator File_Itr;

	File_Group file_group;

	//---- methods ----

	void Read_Runs (void);
	void Update_Schedules (void);
};
#endif
