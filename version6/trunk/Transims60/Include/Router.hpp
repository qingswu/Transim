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
#include "Trip_Gap_Data.hpp"
#include "Plan_Processor.hpp"
#include "Bounded_Queue.hpp"
#include "Data_Queue.hpp"

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
	enum Router_Keys { UPDATE_PLAN_RECORDS = 1, REROUTE_FROM_TIME_POINT, PRINT_UPDATE_WARNINGS, 
		MAXIMUM_NUMBER_OF_ITERATIONS, LINK_CONVERGENCE_CRITERIA, TRIP_CONVERGENCE_CRITERIA, 
		TRANSIT_CAPACITY_CRITERIA, INITIAL_WEIGHTING_FACTOR, ITERATION_WEIGHTING_INCREMENT, 
		MAXIMUM_WEIGHTING_FACTOR, MINIMIZE_VEHICLE_HOURS, SAVE_AFTER_EACH_ITERATION,
		NEW_LINK_CONVERGENCE_FILE, NEW_TRIP_CONVERGENCE_FILE,
		LINK_PERSON_FILE, LINK_PERSON_FORMAT, LINK_VEHICLE_FILE, LINK_VEHICLE_FORMAT,
		NEW_LINK_PERSON_FILE, NEW_LINK_PERSON_FORMAT, NEW_LINK_VEHICLE_FILE, NEW_LINK_VEHICLE_FORMAT,
	};
	virtual void Program_Control (void);

	virtual bool Save_Plans (Plan_Ptr_Array *array_ptr, int part=0);

private:
	enum Router_Reports { TRAVELER_SCRIPT = 1, TRAVELER_STACK, LINK_GAP, TRIP_GAP, ITERATION_PROBLEMS };

	Dtime reroute_time;
	bool trip_flag, plan_flag, new_plan_flag, problem_flag, min_vht_flag;
	bool trip_set_flag, plan_set_flag, problem_set_flag, old_plan_flag, save_flag;
	bool new_delay_flag, flow_flag, turn_flag, reroute_flag, warn_flag, rider_flag;
	bool link_gap_flag, trip_gap_flag, trip_gap_map_flag, iteration_flag, first_iteration;
	bool link_person_flag, link_vehicle_flag, old_person_flag, old_vehicle_flag;

	int total_records, num_file_sets, num_time_updates, num_trip_parts;
	int max_iteration, num_trip_sel, num_trip_rec, num_reroute, num_reskim;
	double factor, increment, max_factor, link_gap, trip_gap, transit_gap;
	
	Trip_File *trip_file;
	Plan_File *plan_file, *new_plan_file;
	Problem_File *problem_file;

	Db_File link_gap_file, trip_gap_file;
	Integers veh_parking;

	Random random_select;

	Plan_Ptr_Array old_plan_array;

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
		void Plan_Build (int partition, Plan_Ptr_Array *plan_ptr_array);

		bool Thread_Flag (void)        { return (num_processors > 1); }

		void Sum_Ridership (Plan_Data &plan, int part=0);
		void Save_Riders (void);
		
		void Sum_Persons (Plan_Data &plan, int part=0);
		void Save_Persons (void);

		void Sum_Vehicles (Plan_Data &plan, int part=0);
		void Save_Vehicles (void);

		void Sum_Link_Data (Flow_Time_Period_Array &array, Plan_Data &plan, bool person_flag);

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
			Plan_Processor *plan_processor;
			
			Line_Array line_array;
			Flow_Time_Period_Array link_person_array;
			Flow_Time_Period_Array link_vehicle_array;

		private:
			int number;
			Part_Processor *ptr;
		} **part_thread;
#endif
	private:
		int num_processors, num_path_builders;
		Plan_Processor plan_processor;
		Router *exe;
	} part_processor;

	Flow_Time_Array link_flow_array, turn_flow_array;
	Flow_Time_Period_Array old_link_array;
	Flow_Time_Period_Array old_turn_array;

	Link_Delay_File old_person_file, old_vehicle_file;
	Link_Delay_File link_person_file, link_vehicle_file;
	Flow_Time_Period_Array link_person_array, link_vehicle_array;
	Flow_Time_Period_Array old_person_array, old_vehicle_array;

	Partition_Files <Trip_File> trip_set;
	Partition_Files <Plan_File> plan_set;
	Partition_Files <Plan_File> new_plan_set;
	Partition_Files <Problem_File> problem_set;
	
	//---- gap report data ----

	typedef struct {
		double total;
		double diff;
		double diff_sq;
		double max_gap;
		int    count;
	} Gap_Data; 

	typedef vector <Gap_Data>            Gap_Array;
	typedef Gap_Array::iterator          Gap_Itr;

	Gap_Array link_gap_array, trip_gap_array;

	Trip_Gap_Map        trip_gap_map;
	Trip_Gap_Map_Array  trip_gap_map_array;

	//---- Router methods ----

	void   Set_Partitions (void);
	void   Iteration_Loop (void);
	void   Plan_Processing (void);
	bool   Selection (Trip_Data *ptr);
	void   MPI_Setup (void);
	double MPI_Link_Delay (bool last_flag);
	void   MPI_Processing (void);
	bool   Read_Trips (int part = 0, Plan_Processor *plan_process_ptr = 0);
	bool   Copy_Plans (int part = 0, Plan_Processor *plan_process_ptr = 0);
	double Minimize_VHT (double &factor, bool zero_flag);
	double Merge_Delay (double factor, bool zero_flag);
	double Get_Trip_Gap (void);
	void   Write_Problem (Problem_File *file, Plan_Data *plan_ptr);

	//---- report methods ----
	
	void Link_Gap_Report (void);
	void Link_Gap_Header (void);

	void Trip_Gap_Report (void);
	void Trip_Gap_Header (void);
};
#endif

