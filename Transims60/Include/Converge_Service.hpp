//*********************************************************
//	Converge_Service.hpp - iteration convergence criteria
//*********************************************************

#ifndef CONVERGE_SERVICE_HPP
#define CONVERGE_SERVICE_HPP

#include "Router_Service.hpp"
#include "Select_Service.hpp"
#include "Data_Service.hpp"
#include "User_Program.hpp"
#include "Trip_Gap_Data.hpp"
#include "Gap_Data.hpp"
#include "Plan_Processor.hpp"
#include "Partition_Files.hpp"
#include "Data_Queue.hpp"

//---------------------------------------------------------
//	Converge_Service - iteration convergence criteria
//---------------------------------------------------------

class SYSLIB_API Converge_Service : public Router_Service, public Select_Service
{
	friend class Part_Processor;
public:
	Converge_Service (void);

	virtual bool Get_Household_Data (Household_File &file, Household_Data &data, int partition = 0);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Save_Plans (Plan_Ptr_Array *array_ptr, int part=0);

protected:
	enum Converge_Service_Keys { 
		APPLICATION_METHOD = CONVERGE_SERVICE_OFFSET, STORE_TRIPS_IN_MEMORY, STORE_PLANS_IN_MEMORY,
		INITIALIZE_TRIP_PRIORITY, PRELOAD_TRANSIT_VEHICLES, MAXIMUM_NUMBER_OF_ITERATIONS,
		LINK_CONVERGENCE_CRITERIA, TRIP_CONVERGENCE_CRITERIA, TRANSIT_CAPACITY_CRITERIA, 
		INITIAL_WEIGHTING_FACTOR, ITERATION_WEIGHTING_INCREMENT, MAXIMUM_WEIGHTING_FACTOR, 
		MINIMIZE_VEHICLE_HOURS, MAXIMUM_RESKIM_ITERATIONS, RESKIM_CONVERGENCE_CRITERIA,
		NEW_LINK_CONVERGENCE_FILE, NEW_TRIP_CONVERGENCE_FILE, SAVE_AFTER_ITERATIONS, 
		NEW_SAVE_PLAN_FILE, NEW_SAVE_PLAN_FORMAT, SAVE_PLAN_HOUSEHOLDS, 
		CAPACITY_CONSTRAINT_FIELD, NEXT_DESTINATION_FIELDS, DESTINATION_SHARE_FIELDS, 
		CONSTRAINED_ACTIVITY_DURATION, NEW_CAPACITY_CONSTRAINT_FILE, DESTINATION_CHOICE_TRAVELERS, 
		FUEL_SUPPLY_FIELD, FUEL_ACTIVITY_DURATION, MINIMUM_INITIAL_FUEL, SEEK_FUEL_LEVEL, 
		NEW_FUEL_CONSTRAINT_FILE, FUEL_INFORMATION_TRAVELERS, SCHEDULED_ACCESS_TRAVELERS, 
		CONSTRAINT_TIME_INCREMENT, 
	};
	enum Converge_Service_Reports { TRAVELER_SCRIPT = 1, TRAVELER_STACK, LINK_GAP, TRIP_GAP, ITERATION_PROBLEMS };

	void Converge_Service_Keys (int *keys = 0);
	
	static const char *reports [];

	virtual void Program_Control (void);
	virtual void Execute (void);	
	virtual void Print_Reports (void);
	virtual void Page_Header (void);

	Router_Method_Type method;

	bool min_vht_flag, save_iter_flag, link_gap_flag, trip_gap_flag, trip_gap_map_flag, save_link_gap, save_trip_gap;
	bool iteration_flag, first_iteration, link_report_flag, trip_report_flag, save_plan_flag, capacity_flag, priority_flag;
	bool trip_flag, plan_flag, trip_set_flag, trip_memory_flag, plan_memory_flag, time_sort_flag, rider_flag, sched_acc_flag;
	bool new_plan_flag, problem_flag, new_set_flag, problem_set_flag, plan_set_flag, preload_flag, time_order_flag;
	bool flow_flag, turn_flag, reroute_flag, cap_const_flag, fuel_const_flag, fuel_flag, choice_flag, info_flag;

	Dtime cap_duration, fuel_duration;
	int iteration, max_iteration, max_speed_updates, num_trip_sel, num_trip_rec, num_reroute, num_reskim;
	int total_records, num_file_sets, num_time_updates, num_trip_parts, select_records, select_weight, potential;
	int num_update, num_build, num_copied, minimum_fuel, seek_fuel;
	double initial_factor, factor_increment, maximum_factor;
	double link_gap, trip_gap, transit_gap, min_speed_diff, percent_selected, total_percent;

	int  loc_cap_field, initial_priority, loc_fuel_field;
	Integers next_des_field, des_share_field, initial_fuel;

	Trip_File *trip_file;
	Plan_File *plan_file, *new_plan_file;
	Problem_File *problem_file;

	Data_Range save_iter_range, save_hhold_range, choice_range, info_range, sched_acc_range;
	Plan_File save_plan_file;
	
	User_Program type_script;
	Int2_Map hhold_type;
	Db_File script_file;
	bool script_flag, hhfile_flag;

	Perf_Period_Array old_perf_period_array;
	Turn_Period_Array old_turn_period_array;

	Gap_Sum_Array   link_gap_array, trip_gap_array;
	Gap_Data_Array  gap_data_array;
	Trip_Gap_Map_Array  trip_gap_map_array;

	//---- methods ----	

	void Reroute_Time (Dtime time)                     { reroute_time = time; reroute_flag = (time > 0); }
	void Update_Flag (bool flag)                       { update_flag = flag; }

	void Read_Converge_Keys (void);

	void Iteration_Setup (void);
	bool Selection (Trip_Data *ptr);

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

	//---- capacity constraint data ----

	typedef struct {
		int      demand;
		int      capacity;
		int      failed;
		Integers next_des;
		Doubles  shares;
	} Loc_Cap_Data;

	typedef vector <Loc_Cap_Data>    Loc_Cap_Array;
	typedef Loc_Cap_Array::iterator  Loc_Cap_Itr;

	Loc_Cap_Array loc_cap_array;

	//---- fuel constraint data ----

	typedef struct {
		int consumed;
		int supply;
		int failed;
		int ran_out;
	} Loc_Fuel_Data;

	typedef vector <Loc_Fuel_Data>    Loc_Fuel_Array;
	typedef Loc_Fuel_Array::iterator  Loc_Fuel_Itr;

	Loc_Fuel_Array loc_fuel_array;

	Time_Periods constraint_periods;
	
	Time_Map_Itr time_itr;
	Time_Map initial_time_map;

	Partition_Files <Trip_File> trip_file_set;
	Partition_Files <Plan_File> plan_file_set;
	Partition_Files <Plan_File> new_file_set;
	Partition_Files <Problem_File> problem_set;

	Plan_Ptr_Array plan_ptr_arrays;

	Partition_Data <Trip_Array> trip_arrays;
	Partition_Data <Plan_Ptr_Array> plan_ptr_set;
	Partition_Data <Plan_Ptr_Array> new_ptr_set;

	//---------------------------------------------------------
	//	Part_Processor - class definition
	//---------------------------------------------------------

	class Part_Processor
	{
	public:
		Part_Processor (void);
		~Part_Processor (void);

		bool Initialize (Converge_Service *exe);
		void Read_Trips (void);
		void Copy_Plans (void);
		void Plan_Build (Plan_Ptr_Array *plan_ptr_array, int partition = 0);

		bool Thread_Flag (void)        { return (num_processors > 1); }

		void Sum_Ridership (Plan_Data &plan, int part=0);
		void Save_Riders (void);
		void Save_Flows (void);

		Plan_Processor *plan_process;

#ifdef THREADS
		Threads threads;
		Data_Queue <int> partition_queue;
		Plan_Ptr_Queue **trip_queue;
		Integers partition_map;

		class Part_Thread
		{
		public:
			Part_Thread (int number, Part_Processor *ptr);
			void operator()();

			Plan_Processor *plan_process;
			
			Line_Array line_array;

		private:
			int number;
			Part_Processor *ptr;
		} **part_thread;
#endif
	private:
		int num_processors, num_path_builders;

		Converge_Service *exe;
	} part_processor;
	
	void   Set_Method (Plan_Data &plan);
	void   Preload_Transit (void);
	void   Trip_Loop (bool message_flag = false);
	bool   Read_Trips (int partition = 0);
	bool   Copy_Plans (int partition = 0);
	void   Iteration_Output (void);
	void   Write_Plan_Files (void);
	void   Write_Problem (Problem_File *file, Plan_Data *plan_ptr);
	void   Write_Constraint (int period = -1);
	void   Write_Fuel_Demand (int period = -1);
	void   Fuel_Check (Plan_Data &plan);
	bool   Capacity_Check (Plan_Data &plan);

private:

	Db_File link_gap_file, trip_gap_file, cap_const_file, fuel_const_file;
	Random random_select, random_des;
	int num_parts;
	bool trip_gap_parts, memory_flag, update_flag;
	Dtime reroute_time;

	void   Set_Partitions (void);
	void   Input_Trips (void);
	void   Best_Destination (Plan_Data &plan);
};

#endif
