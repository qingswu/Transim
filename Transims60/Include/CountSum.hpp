//*********************************************************
//	CountSum.hpp - Traffic Count Processing
//*********************************************************

#ifndef COUNTSUM_HPP
#define COUNTSUM_HPP

#include "Data_Service.hpp"
#include "Arcview_File.hpp"
#include "Projection_Service.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"
#include "List_Data.hpp"
#include "Link_Data_File.hpp"
#include "Volume_Array.hpp"

//---------------------------------------------------------
//	CountSum - execution class definition
//---------------------------------------------------------

class SYSLIB_API CountSum : public Data_Service
{
public:
	CountSum (void);

	virtual void Execute (void);

protected:
	enum CountSum_Keys { 
		ARC_SIGNAL_FILE = 1, SIGNAL_ID_FIELD, SIGNAL_TYPE_FIELD, SELECT_SIGNAL_TYPES, 
		SIGNAL_PRIMARY_STREET, SIGNAL_CROSS_STREET, 
		SIGNAL_ID_MAP_FILE, SIGNAL_ID_MAP_FORMAT, SIGNAL_ID_MAP_FIELD, SIGNAL_NODE_FIELD, 
		SIGNAL_DETECTOR_FILE, SIGNAL_DETECTOR_FORMAT, DEVICE_ID_FIELD, DETECTOR_ID_FIELD, 
		DETECTOR_TYPE_FIELD, SELECT_DETECTOR_TYPES, DETECTOR_NAME_FIELD, 

		ARC_STATION_FILE, STATION_ID_FIELD, STATION_DIR_FIELD, STATION_OFFSET_FIELD, STATION_TYPE_FIELD, 
		SELECT_STATION_TYPES, SELECT_FACILITY_TYPES,
		LINK_ID_MAP_FILE, LINK_ID_MAP_FORMAT, LINK_ID_FIELD, LINK_DIR_FIELD, LINK_OFFSET_FIELD, 
		MAX_SEARCH_DISTANCE,

		DATA_FILE, DATA_FORMAT, DATA_ID_FIELD, DATA_VOLUME_FIELD, DATA_SPEED_FIELD, DATA_DAY_TIME_FIELD, 
		DATA_DAY_FIELD, DATA_TIME_FIELD, DATA_TIME_FORMAT, SELECT_DAYS, ADJUSTMENT_FACTOR, 
		OUTPUT_DAY_DATA, OUTPUT_MIN_MAX_DATA, NEW_LINK_DATA_FILE, NEW_LINK_DATA_FORMAT, 
		NEW_SIGNAL_NODE_FILE, NEW_SIGNAL_NODE_FORMAT, NEW_LINK_ID_MAP_FILE, NEW_LINK_ID_MAP_FORMAT,
	};
	virtual void Program_Control (void);

private:
	enum Processing_Methods {
		AVERAGE, MINIMUM, MAXIMUM
	};

	String perf_name, perf_ext, link_data_name, link_data_ext;
	int num_records;

	bool script_flag, data_flag, link_data_flag, day_flag, link_map_flag, new_map_flag, method_flag, min_max_flag;
	bool signal_detect_flag, signal_node_flag, signal_map_flag, arc_signal_flag, arc_station_flag;

	int signal_field, type_field, primary_field, cross_field, map_id_field, node_field;
	int link_id_field, link_dir_field, link_offset_field;
	int device_field, detector_field, det_type_field, det_name_field;
	int station_id_field, station_dir_field, station_type_field, station_offset_field;
	int data_id_field, volume_field, speed_field, day_time_field, day_field, time_field;
	double max_dist;
	Units_Type time_format;
    
	Strings signal_types, detector_types;
	Strs_Array station_types;
	Ints_Array facility_types;
	Arcview_File arc_signals, arc_stations;
	Projection_Service projection;
	Db_Header detector_file, signal_node_file, signal_map_file, link_map_file, new_map_file;
	List_Array node_list;
	Link_Data_File link_data_file, *min_data_file, *max_data_file;

	Vol_Spd_Period_Array  count_day_array, vol_spd_array;

	Int_Map id_map;

	//---- signal data ----

	typedef struct {
		int node;
		String type;
		String primary;
		String cross;
	} Signal_Point;
	
	typedef map <int, Signal_Point>     Signal_Pt_Map;
    typedef pair <int, Signal_Point>    Signal_Pt_Data;
	typedef Signal_Pt_Map::iterator     Signal_Pt_Itr;
    typedef pair <Signal_Pt_Itr, bool>  Signal_Pt_Stat;

	Signal_Pt_Map signal_pt_map;

	Int_Map detect_dir_map;

	//---- data groups ----

	typedef struct {
		int group;
		int id_field;
		int volume_field;
		int speed_field;
		int day_time_field;
		int day_field;
		int time_field;
		Units_Type time_format;
		double factor;
		Strings select_days;
		Db_Header *file;
	} Data_Group;

	typedef vector <Data_Group>     Data_Array;
	typedef Data_Array::iterator    Data_Itr;

	Data_Array data_group;

	//---- days of count data ----

	Str_ID day_map;

	typedef vector <Vol_Spd_Period_Array *> Count_Days;
	typedef Count_Days::iterator            Count_Days_Itr;

	Count_Days count_days;

	//---- link extends ----

	typedef struct {
		int low_x;
		int low_y;
		int high_x;
		int high_y;
	} Extents;

	typedef vector <Extents>           Extents_Array;
	typedef Extents_Array::iterator    Extents_Itr;

	Extents_Array extents_array;

	//---- dir_offset ----
	
	typedef map <int, Int2_Key>         ID_Offset_Map;
    typedef pair <int, Int2_Key>        ID_Offset_Data;
	typedef ID_Offset_Map::iterator     ID_Offset_Itr;
    typedef pair <ID_Offset_Itr, bool>  ID_Offset_Stat;

	ID_Offset_Map id_offset_map;

	typedef map <Int2_Key, int>            Offset_Index_Map;
    typedef pair <Int2_Key, int>           Offset_Index_Data;
	typedef Offset_Index_Map::iterator     Offset_Index_Itr;
    typedef pair <Offset_Index_Itr, bool>  Offset_Index_Stat;

	Offset_Index_Map offset_index_map;

	void Read_Signal_Points (void);
	void Read_Signal_Map (void);
	void Read_Signal_Detectors (void);
	void Link_Extents ();
	void Read_Link_Map (void);
	void Read_Link_Points (void);
	void Node_List (void);
	void Read_Data (void);
	void Combine_Data (Processing_Methods method = AVERAGE, int day=-1);
	void Write_Link_Map (void);
};
#endif
