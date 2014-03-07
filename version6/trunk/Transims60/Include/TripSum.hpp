//*********************************************************
//	TripSum.hpp - travel trip summary utility
//*********************************************************

#ifndef TRIPSUM_HPP
#define TRIPSUM_HPP

#include "APIDefs.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Data_Queue.hpp"
#include "Partition_Files.hpp"
#include "Trip_Sum_Data.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	TripSum - execution class definition
//---------------------------------------------------------

class SYSLIB_API TripSum : public Data_Service, public Select_Service
{
public:

	TripSum (void);

	virtual void Execute (void);

protected:
	enum TripSum_Keys { 
		NEW_TIME_DISTRIBUTION_FILE = 1, NEW_TRIP_TIME_FILE, NEW_TRIP_LENGTH_FILE, 
		NEW_LINK_TRIP_END_FILE, NEW_LINK_TRIP_END_FORMAT, NEW_LOCATION_TRIP_END_FILE, NEW_LOCATION_TRIP_END_FORMAT, 
		NEW_ZONE_TRIP_END_FILE, NEW_ZONE_TRIP_END_FORMAT, NEW_TRIP_TABLE_FILE, NEW_TRIP_TABLE_FORMAT, 
		TRIP_TIME_INCREMENT, TRIP_LENGTH_INCREMENT, DISTANCE_CALCULATION
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum TripSum_Reports { TOP_100_LINKS = 1, TOP_100_LANES, TOP_100_VC_RATIOS, ZONE_EQUIV, TIME_EQUIV, 
		TRIP_DIURNAL, TRIP_TIME, TRIP_LENGTH, TRIP_PURPOSE, MODE_LENGTH, MODE_PURPOSE };

	Trip_File *trip_file;

	int num_parts;
	String pathname;
	Dtime time_increment;
	int len_increment;
	Distance_Type distance_type;
	Units_Type distance_units;

	bool select_flag, link_flag, time_flag, length_flag, trip_len_flag, trip_purp_flag, mode_len_flag, mode_purp_flag, time_report;
	bool diurnal_flag, time_file_flag, len_file_flag, link_trip_flag, loc_trip_flag, zone_trip_flag, trip_table_flag;
	
	Db_File diurnal_file, trip_time_file, trip_len_file;
	Db_Header link_trip_file, loc_trip_file, zone_trip_file, trip_table_file;

	Integers start_time, mid_time, end_time;
	Ints_Array link_trip_data, loc_trip_data, zone_trip_data;
	Trip_Sum_Data trip_length, trip_purpose, mode_length, mode_purpose;
	Int_Map time_inc_map, len_inc_map;

	void MPI_Setup (void);
	void MPI_Processing (void);
	void Write_Time_Distribution (void);
	void Write_Trip_Time (void);
	void Write_Trip_Length (void);
	void Write_Link_Trips (void);
	void Write_Location_Trips (void);
	void Write_Zone_Trips (void);

	void Top_100_Report (int report_num);
	void Top_100_Header (int report_num);

	void Trip_Diurnal_Report (void);
	void Trip_Diurnal_Header (void);

	void Trip_Time_Report (void);
	void Trip_Time_Header (void);

	void Trip_Length_Report ();
	void Trip_Length_Header ();

	void Trip_Purpose_Report ();
	void Trip_Purpose_Header ();

	void Trip_Mode_Length_Report ();
	void Trip_Mode_Length_Header ();

	void Trip_Mode_Purpose_Report ();
	void Trip_Mode_Purpose_Header ();

	typedef Data_Queue <int> Partition_Queue;

	Partition_Files <Trip_File> new_file_set;
	Partition_Queue partition_queue;

	//---------------------------------------------------------
	//	Trip_Processing - process trip partitions
	//---------------------------------------------------------

	class Trip_Processing
	{
	public:
		Trip_Processing (TripSum *_exe, int thread_num);
		~Trip_Processing (void);

		void operator()();

	private:
		TripSum *exe;
		int  thread_num;
		bool thread_flag;

		Random      random_part;
		Integers    start_time, mid_time, end_time, *start_ptr, *end_ptr, *mid_ptr;
		Int_Map     time_inc_map, len_inc_map, *time_inc_ptr, *len_inc_ptr;
		Ints_Array  link_trip_data, loc_trip_data, zone_trip_data, *link_trip_ptr, *loc_trip_ptr, *zone_trip_ptr;
		Trip_Sum_Data trip_length, trip_purpose, mode_length, mode_purpose;
		Trip_Sum_Data *trip_len_ptr, *trip_purp_ptr, *mode_len_ptr, *mode_purp_ptr;

		Trip_File   *trip_file, *file;

		void Read_Trips (int part);
	};
};
#endif
