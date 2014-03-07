//*********************************************************
//	PlanSum.hpp - travel plan processing utility
//*********************************************************

#ifndef PLANSUM_HPP
#define PLANSUM_HPP

#include "APIDefs.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Flow_Time_Service.hpp"
#include "Data_Queue.hpp"
#include "Db_File.hpp"
#include "Trip_Sum_Data.hpp"
#include "Link_Data_File.hpp"
#include "Transfer_Data.hpp"

typedef struct {
	int group;
	int period;
	int from_stop;
	int from_line;
	int to_stop;
	int to_line;
} Xfer_IO;

SYSLIB_API bool operator < (Xfer_IO left, Xfer_IO right);

//---------------------------------------------------------
//	PlanSum - execution class definition
//---------------------------------------------------------

class SYSLIB_API PlanSum : public Data_Service, public Select_Service, public Flow_Time_Service
{
public:

	PlanSum (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum PlanSum_Keys { NEW_TRIP_TIME_FILE = 1, NEW_LINK_VOLUME_FILE, NEW_ACCESS_DETAIL_FILE, 
		NEW_ACCESS_GROUP_FILE, NEW_STOP_DIURNAL_FILE, NEW_LINE_ON_OFF_FILE, NEW_STOP_BOARDING_FILE,
	};
	virtual void Program_Control (void);

private:
	enum PlanSum_Reports { TOP_100 = 1, VC_RATIO, LINK_GROUP, LINK_EQUIV, ZONE_EQUIV, STOP_EQUIV, LINE_EQUIV,
		SUM_RIDERS, SUM_STOPS, SUM_TRANSFERS, XFER_DETAILS, STOP_GROUP, STOP_GRP_DETAIL, 
		LINE_GROUP, LINE_GRP_DETAIL, SUM_PASSENGERS, RIDER_GROUP, LINE_TRANSFERS, ACCESS_DETAILS, 
		TRIP_TIME, SUM_TRAVEL };

	int max_rail;
	bool select_flag, new_delay_flag, turn_flag, skim_flag, trip_flag, zone_flag, time_flag, travel_flag;
	bool volume_flag, passenger_flag, transfer_flag, rider_flag, xfer_flag, xfer_detail, line_xfer_flag;
	bool access_flag, zone_file_flag, access_file_flag, diurnal_flag, on_off_flag, boarding_flag;
	bool access_group_flag;
	double cap_factor, minimum_vc;
	String xfer_label;
	
	Plan_File *plan_file;
	Db_File time_file, access_detail_file, access_group_file, diurnal_file, on_off_file, boarding_file;
	Link_Data_File volume_file;

	void MPI_Setup (void);
	void MPI_Processing (void);
	void Write_Times (void);
	void Write_Volumes (void);

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;
	
	typedef vector <Ints_Array>   Board_Array;
	typedef Board_Array::iterator Board_Itr;

	Ints_Array total_on_array, boardings;
	Board_Array walk_on_array;
	Board_Array drive_on_array;

	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (PlanSum *_exe);
		~Plan_Processing (void);

		void operator()();

	private:
		PlanSum *exe;
		bool thread_flag, turn_flag;
		Doubles start_time, mid_time, end_time;

		Plan_File               *plan_file;
		Flow_Time_Period_Array  link_delay_array;
		Flow_Time_Period_Array  turn_delay_array;
		Trip_Sum_Data           trip_sum_data;
		Trip_Sum_Data           pass_sum_data;
		Transfer_Array          transfer_array;
		Ints_Array              total_on_array;
		Board_Array             walk_on_array;
		Board_Array             drive_on_array;
		Ints_Array              boardings;

		void Read_Plans (int part);
	};

	//---- report data ----

	Doubles start_time, mid_time, end_time;

	Trip_Sum_Data trip_sum_data, pass_sum_data;
	Transfer_Array transfer_array;

	typedef struct {
		union {
			int lines;
			int links;
		};
		union {
			int runs;
			int capacity;
		};
		union {
			int riders;
			int volume;
		};
	} Group_Data;

	typedef vector <Group_Data> Group_Array;
	typedef Group_Array::iterator Group_Itr;

	typedef map <Xfer_IO, int>            Xfer_IO_Map;
	typedef pair <Xfer_IO, int>           Xfer_IO_Map_Data;
	typedef Xfer_IO_Map::iterator         Xfer_IO_Map_Itr;
	typedef pair <Xfer_IO_Map_Itr, bool>  Xfer_IO_Map_Stat;

	Xfer_IO_Map xfer_map, access_detail, access_group;
	Int_Map xfer_stop;

	//---- line on-off data ----

	typedef map <Int2_Key, int>             First_Last_Map;
	typedef pair <Int2_Key, int>            First_Last_Map_Data;
	typedef First_Last_Map::iterator        First_Last_Map_Itr;
	typedef pair <First_Last_Map_Itr, bool> First_Last_Map_Stat;

	typedef map <Int2_Key, First_Last_Map>  On_Off_Map;
	typedef pair <Int2_Key, First_Last_Map> On_Off_Map_Data;
	typedef On_Off_Map::iterator            On_Off_Map_Itr;
	typedef pair <On_Off_Map_Itr, bool>     On_Off_Map_Stat;

	typedef map <Int2_Key, On_Off_Map>	    Line_On_Off;
	typedef pair <Int2_Key, On_Off_Map>     Line_On_Off_Data;
	typedef Line_On_Off::iterator           Line_On_Off_Itr;
	typedef pair <Line_On_Off_Itr, bool>    Line_On_Off_Stat;

	Line_On_Off line_on_off;

	//---- report methods ----
	
	void Top_100_Report (void);
	void Top_100_Header (void);

	void VC_Ratio (double min_vc);
	void VC_Ratio_Header (void);

	void Link_Group (double min_vc);
	void Link_Group_Header (void);

	void Trip_Time (void);
	void Trip_Time_Header (void);

	void Transit_Report (void);
	void Transit_Header (void);

	void Stop_Report (void);
	void Stop_Header (void);

	void Stop_Group_Report (void);
	void Stop_Group_Header (void);

	void Stop_Group_Detail (void);
	void Stop_Group_Detail_Header (void);

	void Line_Group_Report (void);
	void Line_Group_Header (void);

	void Line_Group_Detail (void);
	void Line_Group_Detail_Header (void);

	void Transfer_Report (int num, int mode = 0);
	void Transfer_Header (void);

	void Line_Transfers_Report (void);
	void Line_Transfers_Header (void);
	
	void Access_Detail_Report (void);
	void Access_Detail_Header (void);

	void Rider_Group (void);
	void Rider_Header (void);

	void Write_Access_Details (void);
	void Write_Access_Groups (void);
	void Write_Stop_Diurnals (void);
	void Write_Line_On_Off (void);
	void Write_Boardings (void);
};
#endif
