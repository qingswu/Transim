//*********************************************************
//	TourSkim.hpp - build tour-based skim matrices
//*********************************************************

#ifndef TOURSKIM_HPP
#define TOURSKIM_HPP

#include "Execution_Service.hpp"
#include "Db_File.hpp"
#include "TypeDefs.hpp"
#include "String.hpp"
#include "Data_Queue.hpp"

#include "..\TcadLib\CaliperMTX.h"
#include "..\TcadLib\MTXERR.h"

#include <vector>
using namespace std;

extern int tc_status;

//---------------------------------------------------------
//	TourSkim - execution class definition
//---------------------------------------------------------

class TourSkim : public Execution_Service
{
public:
	TourSkim (void);
	virtual ~TourSkim (void);

	virtual void Execute (void);

protected:
	enum TourSkim_Keys { 
		DRIVE_TO_TRANSIT_SKIM = 1, PARKING_NODE_SKIM, NEW_DRIVE_TO_TRANSIT_SKIM, RETURN_PERIOD_OPTIONS,
		WALK_TO_TRANSIT_SKIM, DRIVE_FROM_PARKING_SKIM, RETURN_PERIOD_PREFIX, PARKING_ZONE_MAP_FILE,
		DRIVE_TO_TRANSIT_TABLE, PARKING_NODE_TABLE, WALK_TO_TRANSIT_TABLE, DRIVE_FROM_PARKING_TABLE,
		RETURN_TABLE_MAP,
	};
	virtual void Program_Control (void);

private:
	int num_zones, num_tabs, num_pnr, num_return;
	float ****walk_data, ***drive_data;
	Strings pnr_table, walk_table;
	String node_table, drive_table;
	Integers return_map, pnr_id, walk_id;

	Int_Map parking_zone_map;
	Int_Map parking_id_map;

	typedef struct {
		int group;
		TC_MATRIX walk_skim;
		TC_MATRIX drive_skim;
		String prefix;
	} Return_Data;

	typedef vector <Return_Data>        Return_Array;
	typedef Return_Array::iterator      Return_Itr;

	Return_Array return_array;

	typedef struct {
		int group;
		TC_MATRIX pnr_skim;
		TC_MATRIX node_skim;
		String new_name;
		Integers periods;
	} File_Group;

	typedef vector <File_Group>         File_Group_Array;
	typedef File_Group_Array::iterator  File_Group_Itr;	

	File_Group_Array file_group;
	Db_File parking_zone_file;

	typedef Data_Queue <int> Group_Queue;

	Group_Queue group_queue, read_queue;

	//---------------------------------------------------------
	//	Group_Process - group processing
	//---------------------------------------------------------

	class Group_Process
	{
	public:
		Group_Process (TourSkim *_exe);
		~Group_Process (void);

		void operator()();
	private:
		bool thread_flag;
		float *pnr_data, *ttime;
		long *row_id, *col_id;

		void Process (int index);
		TourSkim *exe;
	};

	//---------------------------------------------------------
	//	Read_Process - read processing
	//---------------------------------------------------------

	class Read_Process
	{
	public:
		Read_Process (TourSkim *_exe);

		void operator()();
	private:
		bool thread_flag;

		void Read_Data (int index);
		TourSkim *exe;
	};
};
#endif
