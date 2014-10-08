//*********************************************************
//	Relocate.hpp - move trips and plans to a new network
//*********************************************************

#ifndef RELOCATE_HPP
#define RELOCATE_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"

#ifdef THREADS
#include "Ordered_Work.hpp"
typedef Ordered_Work <Plan_Data, Plan_Data> Plan_Queue;
#endif

//---------------------------------------------------------
//	Relocate - execution class definition
//---------------------------------------------------------

class Relocate : public Data_Service, public Select_Service
{
public:
	Relocate (void);
	virtual ~Relocate (void);

	virtual void Execute (void);

	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);
	virtual bool Get_Shape_Data (Shape_File &file, Shape_Data &shape_rec);
	virtual bool Get_Link_Data (Link_File &file, Link_Data &link_rec, Dir_Data &ab_rec, Dir_Data &ba_rec);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &location_rec);
	virtual bool Get_Parking_Data (Parking_File &file, Parking_Data &parking_rec);
	virtual bool Get_Access_Data (Access_File &file, Access_Data &access_rec);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);

protected:
	enum Relocate_Keys { 
		TARGET_DIRECTORY = 1, TARGET_NODE_FILE, TARGET_SHAPE_FILE, TARGET_LINK_FILE, TARGET_LOCATION_FILE, 
		TARGET_PARKING_FILE, TARGET_ACCESS_FILE, TARGET_STOP_FILE, TARGET_ROUTE_FILE,
		MAXIMUM_XY_DIFFERENCE, DELETE_PROBLEM_PLANS
	};

	virtual void Program_Control (void);

private:
	bool shape_flag, parking_flag, access_flag, stop_flag, line_flag, target_flag;
	bool trip_flag, plan_flag, select_flag, new_select_flag, delete_flag;

	int num_node, num_shape, num_link, num_location, num_parking, num_problems;
	int max_xy_diff;
	
	Node_File node_file;
	Link_File link_file;
	Shape_File shape_file;
	Location_File location_file;
	Parking_File parking_file;
	Access_File access_file;
	Stop_File stop_file;
	Line_File line_file;

	Int_Map target_node_map;
	Int_Map target_shape_map;
	Int_Map target_link_map;
	Int_Map target_dir_map;
	Int_Map target_loc_map;
	Int_Map target_park_map;
	Int_Map target_access_map;
	Int_Map target_stop_map;
	Int_Map target_line_map;

	Points points;	
	Point_Map parking_pt, stop_pt;

	//---- methods ----

	void Map_Locations (void);
	void Map_Parking (void);
	void Read_Plans (void);
	bool Process_Plan (Plan_Ptr ptr);

#ifdef THREADS	
	int num_threads;
	Threads threads;
	Plan_Queue plan_queue;

	//---------------------------------------------------------
	//	Plan_Processor - process plan data
	//---------------------------------------------------------

	class SYSLIB_API Plan_Processor
	{
	public:
		Plan_Processor (Relocate *_exe)     { exe = _exe; }

		void operator () ();

		Relocate  *exe;
	} **plan_processors;

	//---------------------------------------------------------
	//	Save_Plan - process the output plan
	//---------------------------------------------------------

	class Save_Plan
	{
	public:
		Save_Plan (void) {}

		void Initialize (Relocate *_exe)   { exe = _exe; }

		void operator()();

		Relocate *exe;
	} save_plan;

#endif

};
#endif
