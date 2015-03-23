//*********************************************************
//	RiderSum.hpp - Ridership File Display Utility
//*********************************************************

#ifndef RIDERSUM_HPP
#define RIDERSUM_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Plan_File.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Veh_Type_Data.hpp"

//---------------------------------------------------------
//	RiderSum - execution class definition
//---------------------------------------------------------

class RiderSum : public Data_Service, public Select_Service
{
public:
	RiderSum (void);

	virtual void Execute (void);

protected:
	enum RiderSum_Keys { 
		NEW_LINE_RIDER_FILE = 1, NEW_LINE_GROUP_FILE, NEW_STOP_ROUTE_FILE, NEW_STOP_GROUP_FILE, 
		NEW_STOP_PROFILE, NEW_STOP_LINES_FILE, NEW_STOP_DETAIL_FILE,
	};
	virtual void Program_Control (void);
	
	virtual void Page_Header (void);

private:
	enum RiderSum_Reports { STOP_EQUIV = 1, LINE_EQUIV, LINE_RIDERS, GROUP_RIDERS, 
		LINE_SUM, LINE_GROUP, STOP_SUM, STOP_GROUP, STOP_RUN, STOP_ROUTE};

	Ints_Array board_data, alight_data;
	Integers load_data, stop_list, enter_runs, end_runs, start_runs, enter_cap, start_cap, end_cap;

	Db_File line_rider_file, line_group_file, stop_route_file, stop_group_file, stop_profile, stop_lines, stop_detail_file;
	bool rider_flag, on_off_flag, line_rider_flag, line_group_flag, stop_route_flag, stop_group_flag;
	bool stop_profile_flag, stop_lines_flag, stop_detail_flag;
	Integers run_flag, run_period, period_flag;

	typedef struct {
		int stop;
		int to;
		int alight;
		int board;
		int riders;
		int runs;
		int capacity;
		int length;
		double time;
	} Stop_Group_Data;

	typedef vector <Stop_Group_Data>    Stop_Group_Array;
	typedef Stop_Group_Array::iterator  Stop_Group_Itr;

	Stop_Group_Array stop_group_array;

	//---- methods ----

	bool Run_Selection (Line_Data *ptr);
	bool Link_Selection (Line_Data *ptr);

	void Write_Stop_Route (void);
	void Write_Stop_Group (void);
	void Write_Line_Rider (void);
	void Write_Group_Rider (void);
	void Write_Stop_Profile (void);
	void Write_Stop_Lines (void);
	void Write_Stop_Details (void);

	void Setup_Riders (void);
	void Sum_Riders (void);

	void Line_Sum_Report (void);
	void Line_Sum_Header (void);

	void Line_Group_Report (void);
	void Line_Group_Header (void);

	void Line_Rider_Report (void);
	void Line_Rider_Header (void);

	void Group_Rider_Report (void);
	void Group_Rider_Header (void);

	void Stop_Sum_Report (void);
	void Stop_Sum_Header (void);

	void Stop_Group_Report (void);
	void Stop_Group_Header (void);

	void Stop_Run_Report (void);
	void Stop_Run_Header (void);
	
	void Stop_Route_Report (void);
	void Stop_Route_Header (void);
};
#endif
