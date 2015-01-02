//*********************************************************
//	SubareaPlans.hpp - Create a Subarea Plan file
//*********************************************************

#ifndef SUBAREAPLANS_HPP
#define	SUBAREAPLANS_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Arcview_File.hpp"
#include "Db_Array.hpp"
#include "TypeDefs.hpp"
#include "Shape_Tools.hpp"
#include "Data_Queue.hpp"

class SYSLIB_API SubareaPlans : public Data_Service, public Select_Service
{
public:
	SubareaPlans (void);
	
	virtual void Execute (void);

protected:
	enum SubareaZone_Keys { 
	};

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum SubareaPlans_Reports { TRIP_REPORT = 1, ZONE_EQUIV, TIME_EQUIV };

	bool transit_flag, thread_flag, select_flag, access_flag, trip_flag;
	int num_trips;

	Int_Map pnr_loc;
	
	Trip_File *new_trip_file;
	Plan_File *plan_file, *new_plan_file;

	//---- Boundary Link Data ----

	typedef struct {
		int location;
		int parking;
		int stop;
		int parking_offset;
		int stop_offset;
	} Boundary_Link;

	typedef vector <Boundary_Link>  Sublink_Array;
	typedef Sublink_Array::iterator Sublink_Itr;

	Sublink_Array sublink_array;

	//---- methods ----

	void Sublink_Data (void);
	void Convert_Plan (Plan_Data &plan);
	void Trip_Report (void);

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;
	Partition_Queue output_queue;

	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (SubareaPlans *_exe);
		~Plan_Processing (void);

		void operator()();

	private:
		SubareaPlans *exe;
		bool thread_flag;

		int num_trips;

		Plan_File *plan_file;
		Select_Map select_map;

		void Read_Plans (int part);
	};

	//---------------------------------------------------------
	//	Plan_Output - write plan partitions
	//---------------------------------------------------------

	class Plan_Output
	{
	public:
		Plan_Output (SubareaPlans *_exe);
		~Plan_Output (void);

		void operator()();

	private:
		SubareaPlans *exe;
		bool thread_flag;

		Plan_File *plan_file, *new_plan_file;

		void Write_Plans (int part);
	};
};
#endif
