//*********************************************************
//	Draw_Service.hpp - shape drawing service keys and data
//*********************************************************

#ifndef DRAW_SERVICE_HPP
#define DRAW_SERVICE_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Shape_Tools.hpp"
#include "Data_Service.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Draw_Service - system data class definition
//---------------------------------------------------------

class SYSLIB_API Draw_Service
{
public:

	Draw_Service (void);

	enum Draw_Service_Keys { 
		DRAW_NETWORK_LANES = DRAW_SERVICE_OFFSET, LANE_WIDTH, CENTER_ONEWAY_LINKS, 
		LINK_DIRECTION_OFFSET, DRAW_AB_DIRECTION, POCKET_SIDE_OFFSET, PARKING_SIDE_OFFSET, 
		LOCATION_SIDE_OFFSET, SIGN_SIDE_OFFSET, SIGN_SETBACK, TRANSIT_STOP_SIDE_OFFSET, 
		TRANSIT_DIRECTION_OFFSET, TRANSIT_OVERLAP_FLAG, MAXIMUM_SHAPE_ANGLE, MINIMUM_SHAPE_LENGTH,
		DRAW_ONEWAY_ARROWS, ONEWAY_ARROW_LENGTH, ONEWAY_ARROW_SIDE_OFFSET, 
		CURVED_CONNECTION_FLAG, DRAW_VEHICLE_SHAPES, BANDWIDTH_FIELD, 
		BANDWIDTH_SCALING_FACTOR, MINIMUM_BANDWIDTH_VALUE, MINIMUM_BANDWIDTH_SIZE, MAXIMUM_BANDWIDTH_SIZE,
		
	};
	void Draw_Service_Keys (int *keys = 0);

	void Add_Arrow (Points &points);
	void Point_Offset (int link, int dir, int offset, double side, Points &points);

protected:

	void Read_Draw_Keys (void);
	void Set_Bandwidth_Options (Strings options);
	void Set_Bandwidth_Options (String options);
	void Set_Bandwidth_Options (const char *options);

	//---- draw data ----
	
	bool lanes_flag, center_flag, direction_flag, overlap_flag, arrow_flag, curve_flag, shape_flag, bandwidth_flag;
	String bandwidth_field;

	double link_offset, pocket_side, parking_side, location_side, sign_side, sign_setback, stop_side;
	double route_offset, lane_width, arrow_length, arrow_side, width_factor, min_value, min_width, max_width;
	int max_angle, min_length;

private:
	Strings bandwidth_options;
};

#endif
