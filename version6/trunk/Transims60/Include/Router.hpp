//*********************************************************
//	Router.hpp - Network Path Building 
//*********************************************************

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "APIDefs.hpp"
#include "Router_Service.hpp"
#include "Select_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Db_File.hpp"
#include "TypeDefs.hpp"
#include "Partition_Files.hpp"
#include "Plan_Processor.hpp"
#include "Bounded_Queue.hpp"
#include "Data_Queue.hpp"

#define  plan_processor     part_processor.plan_process

//---------------------------------------------------------
//	Router - execution class definition
//---------------------------------------------------------

class SYSLIB_API Router : public Router_Service, public Select_Service
{
	friend class Part_Processor;
public:
	Router (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum Router_Keys { APPLICATION_METHOD = 1, STORE_TRIPS_IN_MEMORY, STORE_PLANS_IN_MEMORY,
		INITIALIZE_TRIP_PRIORITY, UPDATE_PLAN_RECORDS, REROUTE_FROM_TIME_POINT, PRINT_UPDATE_WARNINGS, 
		MAXIMUM_NUMBER_OF_ITERATIONS, LINK_CONVERGENCE_CRITERIA, TRIP_CONVERGENCE_CRITERIA, 
		TRANSIT_CAPACITY_CRITERIA, INITIAL_WEIGHTING_FACTOR, ITERATION_WEIGHTING_INCREMENT, 
		MAXIMUM_WEIGHTING_FACTOR, MINIMIZE_VEHICLE_HOURS, MAXIMUM_RESKIM_ITERATIONS, RESKIM_CONVERGENCE_CRITERIA,
		SAVE_AFTER_ITERATIONS, OUTPUT_ALL_RECORDS, PRELOAD_TRANSIT_VEHICLES, 
		NEW_LINK_CONVERGENCE_FILE, NEW_TRIP_CONVERGENCE_FILE
	};
	virtual void Program_Control (void);

	virtual bool Save_Plans (Plan_Ptr_Array *array_ptr, int part=0);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);

private:
	enum Router_Reports { TRAVELER_SCRIPT = 1, TRAVELER_STACK, LINK_GAP, TRIP_GAP, ITERATION_PROBLEMS };

	Dtime reroute_time;
	bool trip_flag, plan_flag, new_plan_flag, problem_flag, min_vht_flag, trip_memory_flag, plan_memory_flag;
	bool trip_set_flag, new_set_flag, problem_set_flag, plan_set_flag, save_iter_flag, time_sort_flag;
	bool new_delay_flag, flow_flag, turn_flag, reroute_flag, warn_flag, rider_flag, priority_flag, full_flag;
	bool link_gap_flag, trip_gap_flag, trip_gap_map_flag, save_link_gap, iteration_flag, first_iteration;
	bool preload_flag;

	Router_Method_Type method;
	int total_records, num_file_sets, num_time_updates, num_trip_parts, max_speed_updates;
	int iteration, max_iteration, num_trip_sel, num_trip_rec, num_reroute, num_reskim, initial_priority;
	double factor, increment, max_factor, link_gap, trip_gap, transit_gap, min_speed_diff;
	
	Trip_File *trip_file;
	Plan_File *plan_file, *new_plan_file;
	Problem_File *problem_file;

	Db_File link_gap_file, trip_gap_file;
	Integers veh_parking;
	Data_Range save_iter_range;

	Random random_select;

	//---------------------------------------------------------
	//	Part_Processor - class definition
	//---------------------------------------------------------

	class Part_Processor
	{
	public:
		Part_Processor (void);
		~Part_Processor (void);

		bool Initialize (Router *exe);
		void Read_Trips (void);
		void Copy_Plans (void);
		void Plan_Build (Plan_Ptr_Array *plan_ptr_array, int partition = 0, Plan_Processor *ptr = 0);

		bool Thread_Flag (void)        { return (num_processors > 1); }

		void Sum_Ridership (Plan_Data &plan, int part=0);
		void Save_Riders (void);

		Plan_Processor *plan_process;

#ifdef THREADS
		typedef Bounded_Queue <Plan_Ptr_Array *> Trip_Queue;

		Threads threads;
		Data_Queue <int> partition_queue;
		Trip_Queue **trip_queue;
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

		Router *exe;
	} part_processor;

	Perf_Period_Array old_perf_period_array;
	Turn_Period_Array old_turn_period_array;

	Partition_Files <Trip_File> trip_file_set;
	Partition_Files <Plan_File> plan_file_set;
	Partition_Files <Plan_File> new_file_set;
	Partition_Files <Problem_File> problem_set;

	Plan_Ptr_Array plan_ptr_arrays;

	Partition_Data <Trip_Array> trip_arrays;
	Partition_Data <Plan_Ptr_Array> plan_ptr_set;
	Partition_Data <Plan_Ptr_Array> new_ptr_set;

	Trip_Gap_Map        trip_gap_map;
	Trip_Gap_Map_Array  trip_gap_map_array;

	//---- Router methods ----

	void   Set_Partitions (void);
	void   Map_Trip_Plan (void);
	void   DUE_Loop (void);
	double Reskim_Plans (bool average_flag);
	void   Iteration_Loop (void);
	void   Plan_Processing (void);
	bool   Selection (Trip_Data *ptr);
	void   MPI_Setup (void);
	double MPI_Link_Delay (bool last_flag);
	void   MPI_Processing (void);
	bool   Memory_Loop (int part = 0, Plan_Processor *plan_process_ptr = 0);
	bool   Read_Trips (int part = 0, Plan_Processor *plan_process_ptr = 0);
	bool   Copy_Plans (int part = 0, Plan_Processor *plan_process_ptr = 0);
	double Minimize_VHT (double &factor, bool zero_flag);
	double Merge_Delay (double factor, bool zero_flag);
	double Get_Link_Gap (bool zero_flag);
	double Get_Trip_Gap (void);
	void   Write_Problem (Problem_File *file, Plan_Data *plan_ptr);
	void   Write_Plan_Files (void);
	void   Preload_Transit (void);

	//---- report methods ----
	
	void Link_Gap_Report (void);
	void Link_Gap_Header (void);

	void Trip_Gap_Report (void);
	void Trip_Gap_Header (void);
};
#endif

