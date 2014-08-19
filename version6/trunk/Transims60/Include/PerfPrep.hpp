//*********************************************************
//	PerfPrep.hpp - manipulate the performance file
//*********************************************************

#ifndef PERFPREP_HPP
#define PERFPREP_HPP

#include "Data_Service.hpp"
#include "Smooth_Data.hpp"

//---------------------------------------------------------
//	PerfPrep - execution class definition
//---------------------------------------------------------

class SYSLIB_API PerfPrep : public Data_Service
{
public:

	PerfPrep (void);
	
	virtual void Execute (void);

protected:
	enum PerfPrep_Keys { 
		MERGE_PERFORMANCE_FILE = 1, MERGE_PERFORMANCE_FORMAT, MERGE_TURN_DELAY_FILE, MERGE_TURN_DELAY_FORMAT, 
		PROCESSING_METHOD, MERGE_WEIGHTING_FACTOR, MAX_TRAVEL_TIME_RATIO, SET_MIN_TRAVEL_TIME,
		MERGE_TRANSIT_DATA,
	};
	virtual void Program_Control (void);

private:
	Performance_File merge_file;
	Turn_Delay_File turn_file;

	int method, transit_data;
	double factor, time_ratio;	
	bool smooth_flag, merge_flag, ratio_flag, turn_flag, turn_merge_flag, min_time_flag, transit_flag;

	Smooth_Data smooth_data;
	
	void Load_Transit_Data (void);
	void Merge_Performance (void);
	void Merge_Turn_Delays (void);

	Perf_Period_Array merge_perf_array;
	Turn_Period_Array merge_turn_array;

	void Smooth_Performance (void);
	void Smooth_Turn_Delays (void);
	void Set_Min_Time (void);
};
#endif
