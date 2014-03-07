//*********************************************************
//	ArcRider.hpp - Ridership File Display Utility
//*********************************************************

#ifndef ARCRIDER_HPP
#define ARCRIDER_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Draw_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Plan_File.hpp"
#include "Problem_File.hpp"
#include "Shape_Tools.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Veh_Type_Data.hpp"

//---------------------------------------------------------
//	ArcRider - execution class definition
//---------------------------------------------------------

class ArcRider : public Data_Service, public Select_Service, public Draw_Service
{
public:
	ArcRider (void);

	virtual void Execute (void);

	virtual bool Get_Stop_Data (Stop_File &file, Stop_Data &data);

protected:
	enum ArcRider_Keys { 
		NEW_ARC_LINE_DEMAND_FILE = 1, NEW_ARC_LINE_GROUP_FILE, NEW_ARC_RIDERSHIP_FILE, NEW_ARC_STOP_DEMAND_FILE, 
		NEW_ARC_STOP_GROUP_FILE, NEW_ARC_RUN_CAPACITY_FILE, RIDERSHIP_SCALING_FACTOR, MINIMUM_RIDERSHIP_VALUE, 
		MINIMUM_RIDERSHIP_SIZE, MAXIMUM_RIDERSHIP_SIZE, SET_WIDTH_USING_RUNS, SET_WIDTH_USING_LOAD_FACTOR, 
		SET_WIDTH_USING_CAPACITY, SET_WIDTH_USING_CAP_FACTOR,
	};
	virtual void Program_Control (void);

private:
	enum ArcRider_Reports { STOP_EQUIV = 1, LINE_EQUIV };

	Arcview_File arcview_line, arcview_sum, arcview_rider, arcview_stop, arcview_group, arcview_cap;

	Point_Map stop_pt;

	Points points;

	int num_width, num_time, num_distance, num_access, num_cap;
	int num_line, num_sum, num_rider, num_stop, num_group, width_field;
	Integers load_data, board_data, alight_data, stop_list, enter_runs, end_runs, start_runs, width_data;
	Integers enter_cap, end_cap, start_cap;

	bool type_flag, line_flag, sum_flag;
	bool rider_flag, on_off_flag, demand_flag, group_flag, cap_flag;

	Projection_Service projection;	

	//---- methods ----

	void Setup_Riders (void);
	void Sum_Riders (void);
	void Write_Route (void);
	void Write_Sum (void);
	void Write_Riders (void);
	void Write_Stops (void);
	void Write_Group (void);
	void Write_Capacity (void);
};
#endif


