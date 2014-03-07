//*********************************************************
//	ArcDelay.hpp - ArcView Link Delay Files
//*********************************************************

#ifndef ARCDELAY_HPP
#define ARCDELAY_HPP

#include "Data_Service.hpp"
#include "Draw_Service.hpp"
#include "Select_Service.hpp"
#include "Projection_Service.hpp"
#include "Link_Data_File.hpp"
#include "Link_Dir_File.hpp"
#include "Arcview_File.hpp"
#include "Time_Periods.hpp"
#include "Db_Array.hpp"
#include "Dtime.hpp"
#include "Shape_Tools.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	ArcDelay - execution class definition
//---------------------------------------------------------

class ArcDelay : public Data_Service, public Select_Service, public Draw_Service
{
public:
	ArcDelay (void);

	virtual void Execute (void);

protected:
	enum ArcDelay_Keys { 
		LINK_DATA_FILE = 1, LINK_DATA_FORMAT, LINK_DIRECTION_FILE, LINK_DIRECTION_FORMAT,
		SYSTEM_EVENT_FILE, SYSTEM_EVENT_FORMAT, NEW_ARC_LINK_DATA_FILE, LINK_DATA_BY_PERIOD, 
		NEW_ARC_LINK_DIR_FILE, LINK_DIR_BY_PERIOD, NEW_ARC_DELAY_FILE, NEW_ARC_INTERSECTION_FILE, 
		NEW_ARC_TURN_FILE, NEW_ARC_TRAFFIC_IMAGE, TRAFFIC_IMAGE_ATTRIBUTE, VEHICLE_TYPE_DISTRIBUTION,
		ADD_LINK_DIRECTION_INDEX, IGNORE_TIME_RANGE_FIELDS, PERIOD_MAXIMUM_FIELD,
	};
	virtual void Program_Control (void);

private:
	enum Image_Attributes { VOLUME, DENSITY, MAX_DENSITY, QUEUE, MAX_QUEUE, FAILURE };
	enum ArcDelay_Reports { XXX = 1 };
	
	Link_Data_File link_data_file;
	Link_Direction_File link_dir_file;
	Performance_File *delay_file;

	Arcview_File arcview_delay, arcview_turn, arcview_link_dir, arcview_node;
	Arcview_File arcview_link_data;

	Points points;

	int length_field, lane_len_field, vc_field, vmt_field, vht_field, vhd_field;
	int cong_time_field, cong_vmt_field, cong_vht_field, max_period_field;
	int max_angle, min_length, width_field, green_field, yellow_field, red_field, split_field;
	bool turn_flag, subarea_flag, index_flag, image_flag, link_period_flag, dir_period_flag;
	bool delay_flag, link_dir_flag, link_data_flag, intersection_flag, event_flag, max_period_flag;

	Projection_Service projection;	

	String shapename, turnname, nodename, linkname, dirname;
	Doubles type_share;

	typedef struct {
		int  number;
		int  type;
		String name;
		Arcview_File *file;
	} Image_Data;
	
	typedef vector <Image_Data>    Image_Array;
	typedef Image_Array::iterator  Image_Itr;

	Image_Array image_array;

	//typedef struct {
	//	int link_dir;
	//	int pocket_id;
	//} Pocket_Index;

	//Int2_Map pocket_index;

	//typedef struct {
	//	int node;
	//	int plan_phase;
	//	int time;
	//	int type;
	//	int green;
	//	int yellow;
	//	int red;
	//} Signal_Time;

	//Complex_Array signal_time;

	//---- methods ----

	void Select_Links (void);
	void Read_Link_Data (void);
	void Read_Link_Dir (void);
	void Write_Delay (void);
	void Maximum_Delay (void);
	void Write_Turn (void);
	void Write_Image (void);
	void Write_Intersection (void);

	void ArcDelay::Draw_Link (Arcview_File &file, Link_Data *link_ptr, int dir = 0);
};
#endif
