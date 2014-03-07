//*********************************************************
//	PlanSelect.hpp - travel plan selection utility
//*********************************************************

#ifndef PLANSELECT_HPP
#define PLANSELECT_HPP

#include "APIDefs.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Data_Queue.hpp"

//---------------------------------------------------------
//	PlanSelect - execution class definition
//---------------------------------------------------------

class SYSLIB_API PlanSelect : public Data_Service, public Select_Service
{
public:

	PlanSelect (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum PlanSelect_Keys { 
	};
	virtual void Program_Control (void);

private:
	enum PlanSelect_Reports { FIRST_REPORT = 1, SECOND_REPORT };

	bool delay_flag, turn_flag, new_plan_flag, select_parts;
	int num_trips, num_select;

	Plan_File *plan_file, *new_plan_file;

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;
	Partition_Queue output_queue;

	void MPI_Setup (void);
	void MPI_Processing (void);
	void Select_Plans (void);
	void MPI_Write (void);
	void MPI_Close (void);

	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (PlanSelect *_exe);
		~Plan_Processing (void);

		void operator()();

	private:
		PlanSelect *exe;
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
		Plan_Output (PlanSelect *_exe);
		~Plan_Output (void);

		void operator()();

	private:
		PlanSelect *exe;
		bool thread_flag;

		Plan_File *plan_file, *new_plan_file;

		void Write_Plans (int part);
	};
};
#endif
