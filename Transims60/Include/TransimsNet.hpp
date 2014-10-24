//*********************************************************
//	TransimsNet.hpp - Network Conversion Utility
//*********************************************************

#ifndef TRANSIMSNET_HPP
#define TRANSIMSNET_HPP

#include "Data_Service.hpp"
#include "Random.hpp"
#include "Db_Header.hpp"
#include "Data_Range.hpp"
#include "Best_List.hpp"
#include "List_Data.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "TypeDefs.hpp"
#include "Link_Use_Data.hpp"
#include "Link_Use_File.hpp"
#include "Approach_Link_Data.hpp"
#include "Approach_Link_File.hpp"
#include "Db_Array.hpp"

#include "Pocket_Warrant.hpp"
#include "Control_Warrant.hpp"
#include "Access_Warrant.hpp"
#include "Parking_Details.hpp"
#include "Street_Parking.hpp"

//---------------------------------------------------------
//	TransimsNet - execution class definition
//---------------------------------------------------------

class SYSLIB_API TransimsNet : public Data_Service
{
public:
	TransimsNet (void);

	virtual void Execute (void);
	
protected:
	enum TransimsNet_Keys { 
		DEFAULT_LINK_SETBACK = 1, DEFAULT_LOCATION_SETBACK, MAXIMUM_CONNECTION_ANGLE, 
		ADD_UTURN_TO_DEAD_END_LINKS, SIGNAL_ID_AS_NODE_ID, EXTERNAL_ZONE_RANGE, 
		EXTERNAL_STATION_OFFSET, REPLICATE_MPO_NETWORK, ZONE_BOUNDARY_FILE, ZONE_FIELD_NAME, 
		POCKET_LANE_WARRANT, TRAFFIC_CONTROL_WARRANT, FACILITY_ACCESS_WARRANT, 
		PARKING_DETAILS_WARRANT, STREET_PARKING_WARRANT, 
		UPDATE_NODE_RANGE, UPDATE_ZONE_RANGE, UPDATE_LINK_RANGE, 
		UPDATE_NODE_FILE, UPDATE_NODE_DATA_FLAG, 
		UPDATE_ZONE_FILE, UPDATE_ZONE_DATA_FLAG, 
		UPDATE_LINK_FILE, UPDATE_LINK_DATA_FLAG, UPDATE_SHAPE_FILE, REPAIR_CONNECTIONS,
		DELETE_NODE_RANGE, DELETE_ZONE_RANGE, DELETE_LINK_RANGE, 
		DELETE_NODE_FILE, DELETE_ZONE_FILE, DELETE_LINK_FILE,
		LINK_USE_FILE, LINK_USE_FORMAT, APPROACH_LINK_FILE, APPROACH_LINK_FORMAT,
	};
	virtual void Program_Control (void);
	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);
	virtual bool Get_Shape_Data (Shape_File &file, Shape_Data &data);
	virtual bool Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba);
	virtual bool Get_Pocket_Data (Pocket_File &file, Pocket_Data &data);
	virtual bool Get_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &data);
	virtual bool Get_Connect_Data (Connect_File &file, Connect_Data &data);
	virtual bool Get_Turn_Pen_Data (Turn_Pen_File &file, Turn_Pen_Data &data);
	virtual bool Get_Parking_Data (Parking_File &file, Parking_Data &data);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Access_Data (Access_File &file, Access_Data &data);
	virtual bool Get_Sign_Data (Sign_File &file, Sign_Data &data);
	virtual bool Get_Signal_Data (Signal_File &file, Signal_Data &data);
	virtual bool Get_Timing_Data (Timing_File &file, Timing_Record &data);
	virtual bool Get_Phasing_Data (Phasing_File &file, Phasing_Record &data);
	virtual bool Get_Detector_Data (Detector_File &file, Detector_Data &data);
	virtual bool Get_Stop_Data (Stop_File &file, Stop_Data &stop_rec);

private:

	bool details_flag, link_use_flag, approach_flag, boundary_flag, street_flag, ext_zone_flag, signal_id_flag;
	bool update_flag, delete_flag, connect_flag, replicate_flag, repair_flag, location_flag, parking_flag;
	bool update_node_flag, update_zone_flag, update_link_flag, update_shape_flag, update_dir_flag;
	bool node_data_flag, zone_data_flag, link_data_flag, access_flag, control_flag;
	bool delete_node_flag, delete_zone_flag, delete_link_flag, repair_signals;
	bool zone_flag, zout_flag, turn_flag, shape_flag, name_flag, collapse_flag, uturn_flag;
	double max_length_factor;

	int straight_diff, thru_diff, max_angle, uturn_angle, short_length, zone_field;

	int location_id, parking_id;
	int location_base, parking_base;

	int nnode, nlink, nparking, nactivity, naccess, npocket, nconnect, nsign, nsignal, nuse;
	int mparking, mactivity, mprocess, mpocket, muse;
	int xlink, xnode, xzone, xshape, xparking, xlocation, xaccess, xpocket, xconnect, xsign, xsignal, xuse, xturn, xstop;
	int nshort, nlength, nexternal, nzone, nzout, max_splits, xdetector;
	int nfixed1, nfixed2, nfixed3, nactuated1, nactuated2, nactuated3, nstop, nyield;
	int nshape, nshapes, nturn;
	int min_length, link_setback, loc_setback, external_offset;

	Db_File keep_nodes, turn_nodes, node_string;
	Db_File update_link_file, update_zone_file, update_node_file;
	Db_File delete_link_file, delete_zone_file, delete_node_file;
	Db_Sort_Array node_data_array, zone_data_array, link_data_array;
	Node_File node_data_file;
	Zone_File zone_data_file;
	Link_File link_data_file;
	Shape_File shape_data_file;

	Arcview_File boundary_file;
	Projection_Service proj_service;
	Points_Map boundary_map;

	Int_Map zone_centroid;

	Link_Use_Map link_use_map;
	Link_Use_File link_use_file;

	Approach_Link_Map approach_map;
	Approach_Link_File approach_file;

	Pocket_Warrant_Map pocket_warrant_map;
	Pocket_Warrant_Array pocket_warrants;

	Control_Warrant_Map control_warrant_map;
	Control_Warrant_Array control_warrants;

	Access_Warrant_Map access_warrant_map;
	Access_Warrant_Array access_warrants;

	Park_Detail_Array parking_details;

	Int2_Map  street_park_map;
	Street_Park_Array  street_parking;

	List_Array node_list;
	List_Array turn_list;

	Data_Range ext_zone_range;
	Data_Range update_node_range, update_zone_range, update_link_range, update_dir_range;
	Data_Range delete_node_range, delete_zone_range, delete_link_range;

	//---- methods ----

	void Read_Node_Data (void);
	void Read_Zone_Data (void);
	void Read_Shape_Data (void);
	void Read_Link_Data (void);
	void Add_Node_Data (void);
	void Add_Zone_Data (void);
	void Add_Link_Data (void);
	void Read_Link_Uses (void);
	void Read_Approach_Links (void);
	void Read_Boundary (void);
	void Zone_Access (void);
	void Link_Access (void);
	void Connections (void);
	void Pocket_Lanes (void);
	void Traffic_Controls (void);
	void Repair_Connections (void);
	void Lane_Ranges (void);
	void Lane_Use (void);
};
#endif


