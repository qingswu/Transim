//*********************************************************
//	NetMerge.hpp - merge two networks
//*********************************************************

#ifndef NETMERGE_HPP
#define NETMERGE_HPP

#include "Data_Service.hpp"

//---------------------------------------------------------
//	NetMerge - execution class definition
//---------------------------------------------------------

class NetMerge : public Data_Service
{
public:
	NetMerge (void);
	virtual ~NetMerge (void);

	virtual void Execute (void);
	//virtual void Page_Header (void);

protected:
	enum NetMerge_Keys { 
		MERGE_DIRECTORY = 1, MERGE_NODE_FILE, MERGE_SHAPE_FILE, MERGE_LINK_FILE, MERGE_POCKET_FILE,
		MERGE_LANE_USE_FILE, MERGE_CONNECTION_FILE, MERGE_LOCATION_FILE, MERGE_PARKING_FILE, MERGE_ACCESS_FILE, 
		MERGE_STOP_FILE, MERGE_SIGN_FILE, MERGE_SIGNAL_FILE, MERGE_TIMING_PLAN_FILE, MERGE_PHASING_PLAN_FILE, 
		MERGE_DETECTOR_FILE, MERGE_ZONE_FILE, MERGE_PERFORMANCE_FILE, MERGE_TURN_DELAY_FILE, 
	};

	virtual void Program_Control (void);

private:
	//enum NetMerge_Reports { REPORT = 1, };

	bool shape_flag, pocket_flag, lane_use_flag, location_flag, parking_flag, access_flag, stop_flag, line_flag, connect_flag;
	bool sign_flag, signal_flag, timing_flag, phasing_flag, detector_flag, performance_flag, turn_delay_flag, merge_flag;
	bool zone_flag;

	Node_File node_file;
	Link_File link_file;
	Shape_File shape_file;
	Pocket_File pocket_file;
	Lane_Use_File lane_use_file;
	Location_File location_file;
	Connect_File connect_file;
	Parking_File parking_file;
	Access_File access_file;
	Stop_File stop_file;
	Line_File line_file;
	Sign_File sign_file;
	Signal_File signal_file;
	Timing_File timing_file;
	Phasing_File phasing_file;
	Detector_File detector_file;
	Zone_File zone_file;
	Performance_File performance_file;
	Turn_Delay_File turn_delay_file;
};
#endif
