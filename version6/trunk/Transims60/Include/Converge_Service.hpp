//*********************************************************
//	Converge_Service.hpp - iteration convergence criteria
//*********************************************************

#ifndef CONVERGE_SERVICE_HPP
#define CONVERGE_SERVICE_HPP

#include "Select_Service.hpp"
#include "Data_Service.hpp"
#include "Trip_Gap_Data.hpp"
#include "Gap_Data.hpp"

//---------------------------------------------------------
//	Converge_Service - iteration convergence criteria
//---------------------------------------------------------

class SYSLIB_API Converge_Service : public Select_Service
{
public:

	Converge_Service (void);

	enum Converge_Service_Keys { 
		MAXIMUM_NUMBER_OF_ITERATIONS = CONVERGE_SERVICE_OFFSET, 
		LINK_CONVERGENCE_CRITERIA, TRIP_CONVERGENCE_CRITERIA, TRANSIT_CAPACITY_CRITERIA, 
		INITIAL_WEIGHTING_FACTOR, ITERATION_WEIGHTING_INCREMENT, MAXIMUM_WEIGHTING_FACTOR, 
		MINIMIZE_VEHICLE_HOURS, MAXIMUM_RESKIM_ITERATIONS, RESKIM_CONVERGENCE_CRITERIA,
		SAVE_AFTER_ITERATIONS, NEW_LINK_CONVERGENCE_FILE, NEW_TRIP_CONVERGENCE_FILE
	};

	void Converge_Service_Keys (int *keys = 0);
	
protected:
	bool min_vht_flag, save_iter_flag, link_gap_flag, trip_gap_flag, trip_gap_map_flag, save_link_gap, save_trip_gap;
	bool iteration_flag, first_iteration, link_report_flag, trip_report_flag;
	int iteration, max_iteration, max_speed_updates, num_trip_sel, num_trip_rec, num_reroute, num_reskim;
	double initial_factor, factor_increment, maximum_factor;
	double link_gap, trip_gap, transit_gap, min_speed_diff, percent_selected;
	
	Data_Range save_iter_range;

	Perf_Period_Array old_perf_period_array;
	Turn_Period_Array old_turn_period_array;

	Gap_Sum_Array   link_gap_array, trip_gap_array;
	Gap_Data_Array  gap_data_array;
	Trip_Gap_Map_Array  trip_gap_map_array;

	//---- methods ----	

	void Reroute_Time (Dtime time)                     { reroute_time = time; }

	void Read_Converge_Keys (void);

	void Iteration_Setup (void);

	double Get_Link_Gap (bool zero_flag);
	double Get_Trip_Gap (void);

	void Link_Gap_Report_Flag (bool flag = true)       { link_report_flag = flag; }
	void Link_Gap_Report (int report);
	void Link_Gap_Header (void);
	void Write_Link_Gap (double gap, bool end_flag = false);

	void Initialize_Trip_Gap (void);
	void Trip_Gap_Map_Parts (int parts);
	bool Trip_Gap_Map_Parts (void)                     { return (trip_gap_parts); }
	 
	void Trip_Gap_Report_Flag (bool flag = true)       { trip_report_flag = flag; }
	void Trip_Gap_Report (int report);
	void Trip_Gap_Header (void);
	void Write_Trip_Gap (double gap, bool end_flag = false);

private:
	Db_File link_gap_file, trip_gap_file;

	int num_parts;
	bool trip_gap_parts, memory_flag;
	Dtime reroute_time;
};

#endif
