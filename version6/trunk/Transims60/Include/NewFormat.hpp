//*********************************************************
//	NewFormat.hpp - convert data files to version 4.2 format
//*********************************************************

#ifndef NEWFORMAT_HPP
#define NEWFORMAT_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Toll_File.hpp"
#include "Activity_File.hpp"
#include "Person_File.hpp"
#include "Vehicle_File.hpp"
#include "Vehicle_Index.hpp"
#include "Old_Plan_File.hpp"
#include "Old_Delay_File.hpp"
#include "Data_Queue.hpp"
#include "Snapshot_File.hpp"

//---------------------------------------------------------
//	NewFormat - execution class definition
//---------------------------------------------------------

class SYSLIB_API NewFormat : public Data_Service, public Select_Service
{
public:
	NewFormat (void);

	virtual void Execute (void);

protected:
	enum NewFormat_Keys { 
		COPY_EXISTING_FIELDS = 1, FLATTEN_OUTPUT_FLAG, TOLL_FILE, ACTIVITY_FILE, PERSON_FILE, VEHICLE_FILE, 
		SNAPSHOT_FILE, NEW_SNAPSHOT_FILE, NEW_SNAPSHOT_FORMAT, NEW_SNAPSHOT_COMPRESSION, 
		OLD_TIME_FORMAT, OLD_PLAN_FILE, OLD_PLAN_FORMAT, NODE_LIST_PATHS, TRAVELER_SCALING_FACTOR,
		OLD_ROUTE_HEADER_FILE, OLD_ROUTE_NODES_FILE, OLD_LINK_DELAY_FILE, OLD_PERFORMANCE_FILE,
	};
	virtual void Program_Control (void);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);
	virtual bool Get_Skim_Data (Skim_File &file, Skim_Record &data, int partition = 0);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);
	virtual bool Get_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data_rec);

private:
	bool toll_flag, activity_flag, person_flag, vehicle_flag, route_flag, new_route_flag, snap_flag, copy_flag;
	bool plan_flag, new_plan_flag, new_trip_flag, old_delay_flag;

	int num_new_trip, max_trip_part, num_new_skim, max_skim_part, num_new_act, route_periods, scale;
	Units_Type time_units;

	Activity_File activity_file;
	Toll_File toll_file;
	Person_File person_file;
	Vehicle_File vehicle_file;
	Old_Plan_File old_plan;
	Old_Delay_File old_delay_file;

	Location_File *new_loc_file;
	Zone_File *new_zone_file;

	Vehicle_Map vehicle_type;
	Veh_ID_Map  vehicle40_map;

	Snapshot_File snap_file, new_snap_file;

	Db_Header route_header, route_nodes;

	void Read_Tolls (void);
	void Read_Activity (void);
	void Read_Persons (void);
	void Read_Vehicles (void);
	void Read_Snapshots (void);
	void Check_Phasing_Plans (void);
	void Read_Routes (void);
	void Make_Route_Nodes (void);
	void Read_Delay (void);

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;

	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (NewFormat *_exe);
		~Plan_Processing (void);

		void operator()();

	private:
		NewFormat *exe;
		bool thread_flag;

		Old_Plan_File   *old_plan;
		Plan_File       *new_plan;
	
		void Travel_Plans (int part);
	};
};
#endif
