//*********************************************************
//	PerfPrep.hpp - manipulate the performance file
//*********************************************************

#ifndef PERFPREP_HPP
#define PERFPREP_HPP

#include "Data_Service.hpp"
#include "Smooth_Data.hpp"
#include "Arcview_File.hpp"
#include "Select_Service.hpp"
#include "Flow_Time_Service.hpp"

//---------------------------------------------------------
//	PerfPrep - execution class definition
//---------------------------------------------------------

class SYSLIB_API PerfPrep : public Data_Service, public Select_Service, public Flow_Time_Service
{
public:

	PerfPrep (void);
	
	virtual void Execute (void);

protected:
	enum PerfPrep_Keys { 
		MERGE_PERFORMANCE_FILE = 1, MERGE_PERFORMANCE_FORMAT, BASE_PERFORMANCE_FILE, BASE_PERFORMANCE_FORMAT, 
		MERGE_TURN_DELAY_FILE, MERGE_TURN_DELAY_FORMAT, 
		PROCESSING_METHOD, MERGE_WEIGHTING_FACTOR, SET_MIN_TRAVEL_TIME,	MERGE_TRANSIT_DATA, 
		TIME_CONSTRAINT_FILE, TIME_CONSTRAINT_FORMAT, NEW_TIME_RATIO_FILE, 
		NEW_DELETED_RECORD_FILE, NEW_DELETED_RECORD_FORMAT,
	};
	virtual void Program_Control (void);
	virtual bool Get_Performance_Data (Performance_File &file, Performance_Data &data);
	virtual void Page_Header (void);

private:
	enum PerfPrep_Reports { DELETED_RECORDS = 1, };

	Performance_File merge_file, deleted_file, base_file;
	Turn_Delay_File turn_file;
	Db_File time_ratio_file;
	Db_Header constraint_file;

	int method;
	double factor, min_x, min_y, max_x, max_y;	
	bool smooth_flag, merge_flag, turn_flag, turn_merge_flag, min_time_flag, base_flag;
	bool deleted_flag, first_delete, del_file_flag, new_file_flag, time_ratio_flag, constraint_flag;
	bool transit_flag, transit_veh_flag, transit_pce_flag, transit_person_flag;

	typedef struct {
		Dtime period;
		int   max_ratio;
	} Constraint_Data;

	typedef vector <Constraint_Data>     Constraint_Array;
	typedef Constraint_Array::iterator   Constraint_Itr;

	Constraint_Array constraint_array;

	Smooth_Data smooth_data;
	
	void Read_Time_Constraints (void);
	void Deleted_Report (Performance_File &file);
	void Deleted_Header (void);

	void Check_Performance (void);
	void Load_Transit_Data (void);
	void Merge_Performance (void);
	void Merge_Turn_Delays (void);

	void Write_Time_Ratios (void);

	Perf_Period_Array merge_perf_array;
	Perf_Period_Array base_perf_array;
	Turn_Period_Array merge_turn_array;

	void Smooth_Performance (void);
	void Smooth_Turn_Delays (void);
	void Set_Min_Time (void);
};
#endif
