//*********************************************************
//	Router.hpp - Network Path Building 
//*********************************************************

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "APIDefs.hpp"
#include "Router_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Db_File.hpp"
#include "TypeDefs.hpp"
#include "Partition_Files.hpp"
#include "Plan_Processor.hpp"
#include "Data_Queue.hpp"

#define  plan_processor     part_processor.plan_process

//---------------------------------------------------------
//	Router - execution class definition
//---------------------------------------------------------

class SYSLIB_API Router : public Router_Service
{
	friend class Part_Processor;
public:
	Router (void);

	virtual void Execute (void);
	virtual void Print_Reports (void);
	virtual void Page_Header (void);

protected:
	enum Router_Keys { APPLICATION_METHOD = 1, STORE_TRIPS_IN_MEMORY, STORE_PLANS_IN_MEMORY,
		INITIALIZE_TRIP_PRIORITY, UPDATE_PLAN_RECORDS, REROUTE_FROM_TIME_POINT, PRINT_UPDATE_WARNINGS, 
		OUTPUT_ALL_RECORDS, PRELOAD_TRANSIT_VEHICLES, 
	};
	virtual void Program_Control (void);

	virtual bool Save_Plans (Plan_Ptr_Array *array_ptr, int part=0);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);

private:
	Dtime reroute_time;
	bool trip_flag, plan_flag, new_plan_flag, problem_flag, trip_memory_flag, plan_memory_flag;
	bool trip_set_flag, new_set_flag, problem_set_flag, plan_set_flag, time_sort_flag;
	bool new_delay_flag, flow_flag, turn_flag, reroute_flag, warn_flag, rider_flag, priority_flag, full_flag;
	bool preload_flag;

	Router_Method_Type method;
<<<<<<< .working
	int total_records, num_file_sets, num_time_updates, num_trip_parts, num_selected, initial_priority;
	int num_update, num_build, num_copied;
	double min_speed_diff, percent_selected;

=======
	int total_records, num_file_sets, num_time_updates, num_trip_parts, initial_priority;
	int num_update, num_build, num_copied, select_records, select_weight, random_seed;
	double min_speed_diff, percent_selected;

>>>>>>> .merge-right.r1529
	Trip_File *trip_file;
	Plan_File *plan_file, *new_plan_file;
	Problem_File *problem_file;

	Integers veh_parking;

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

		Router *exe;
	} part_processor;

	//Perf_Period_Array old_perf_period_array;
	//Turn_Period_Array old_turn_period_array;

	Partition_Files <Trip_File> trip_file_set;
	Partition_Files <Plan_File> plan_file_set;
	Partition_Files <Plan_File> new_file_set;
	Partition_Files <Problem_File> problem_set;

	Plan_Ptr_Array plan_ptr_arrays;

	Partition_Data <Trip_Array> trip_arrays;
	Partition_Data <Plan_Ptr_Array> plan_ptr_set;
	Partition_Data <Plan_Ptr_Array> new_ptr_set;

	//---- Router methods ----

	void   Set_Partitions (void);
	void   Input_Trips (void);
	void   DUE_Loop (void);
	double Reskim_Plans (bool average_flag);
	void   Iteration_Loop (void);
	void   Plan_Processing (void);
	bool   Selection (Trip_Data *ptr);
	bool   Memory_Loop (int part = 0, Plan_Processor *plan_process_ptr = 0);
	bool   Read_Trips (int part = 0, Plan_Processor *plan_process_ptr = 0);
	bool   Copy_Plans (int part = 0, Plan_Processor *plan_process_ptr = 0);
	double Minimize_VHT (double &factor, bool zero_flag);
	double Merge_Delay (double factor, bool zero_flag);
	void   Write_Problem (Problem_File *file, Plan_Data *plan_ptr);
	void   Write_Plan_Files (void);
	void   Preload_Transit (void);
};
#endif

