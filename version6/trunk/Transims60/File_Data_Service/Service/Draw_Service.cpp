//*********************************************************
//	Draw_Service.cpp - shape drawing service keys and data
//*********************************************************

#include "Draw_Service.hpp"

#include <math.h>

//---------------------------------------------------------
//	Draw_Service constructor
//---------------------------------------------------------

Draw_Service::Draw_Service (void)
{
	lanes_flag = center_flag = direction_flag = overlap_flag = arrow_flag = curve_flag = shape_flag = bandwidth_flag = false;

	link_offset = pocket_side = parking_side = location_side = sign_side = sign_setback = 0.0;
	stop_side = route_offset = lane_width = arrow_length = arrow_side = 0.0;
	width_factor = min_value = min_width = max_width = 0.0;
	max_angle = 45;
	min_length = 5;
}

//---------------------------------------------------------
//	Draw_Service_Keys
//---------------------------------------------------------

void Draw_Service::Draw_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DRAW_NETWORK_LANES, "DRAW_NETWORK_LANES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LANE_WIDTH, "LANE_WIDTH", LEVEL0, OPT_KEY, FLOAT_KEY, "12.0 feet", "0..150 feet", NO_HELP },
		{ CENTER_ONEWAY_LINKS, "CENTER_ONEWAY_LINKS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LINK_DIRECTION_OFFSET, "LINK_DIRECTION_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 feet", "0..200 feet", NO_HELP },
		{ DRAW_AB_DIRECTION, "DRAW_AB_DIRECTION", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ POCKET_SIDE_OFFSET, "POCKET_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "7.0 feet", "0..300 feet", NO_HELP },
		{ PARKING_SIDE_OFFSET, "PARKING_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 feet", "0..600 feet", NO_HELP },
		{ LOCATION_SIDE_OFFSET, "LOCATION_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "35.0 feet", "0..1200 feet", NO_HELP },
		{ SIGN_SIDE_OFFSET, "SIGN_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "7.0 feet", "0..600 feet", NO_HELP },
		{ SIGN_SETBACK, "SIGN_SETBACK", LEVEL0, OPT_KEY, FLOAT_KEY, "7.0 feet", "0..1200 feet", NO_HELP },
		{ TRANSIT_STOP_SIDE_OFFSET, "TRANSIT_STOP_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "7.0 feet", "0..600 feet", NO_HELP },
		{ TRANSIT_DIRECTION_OFFSET, "TRANSIT_DIRECTION_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 feet", "0..200 feet", NO_HELP },
		{ TRANSIT_OVERLAP_FLAG, "TRANSIT_OVERLAP_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ MAXIMUM_SHAPE_ANGLE, "MAXIMUM_SHAPE_ANGLE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 feet", "0..200 feet", NO_HELP },
		{ MINIMUM_SHAPE_LENGTH, "MINIMUM_SHAPE_LENGTH", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 feet", "0..200 feet", NO_HELP },
		{ DRAW_ONEWAY_ARROWS, "DRAW_ONEWAY_ARROWS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ONEWAY_ARROW_LENGTH, "ONEWAY_ARROW_LENGTH", LEVEL0, OPT_KEY, FLOAT_KEY, "25.0 feet", "0.3..2200 feet", NO_HELP },
		{ ONEWAY_ARROW_SIDE_OFFSET, "ONEWAY_ARROW_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "6 feet", "0.3..600 feet", NO_HELP },
		{ CURVED_CONNECTION_FLAG, "CURVED_CONNECTION_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ DRAW_VEHICLE_SHAPES, "DRAW_VEHICLE_SHAPES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ BANDWIDTH_FIELD, "BANDWIDTH_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ BANDWIDTH_SCALING_FACTOR, "BANDWIDTH_SCALING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0 units/foot", "0.01..100000 units/foot", NO_HELP },
		{ MINIMUM_BANDWIDTH_VALUE, "MINIMUM_BANDWIDTH_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0", "0..100000", NO_HELP },
		{ MINIMUM_BANDWIDTH_SIZE, "MINIMUM_BANDWIDTH_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "3.5 feet", "0.0..35 feet", NO_HELP },
		{ MAXIMUM_BANDWIDTH_SIZE, "MAXIMUM_BANDWIDTH_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "3500.0 feet", "1..35000 feet", NO_HELP },
		END_CONTROL
	};

	if (keys == 0) {
		exe->Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					exe->Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				exe->Error (String ("Draw Service Key %d was Not Found") % keys [i]);
			}
		}
	}
}

//---------------------------------------------------------
//	Read_Draw_Keys
//---------------------------------------------------------

void Draw_Service::Read_Draw_Keys (void)
{
	exe->Print (2, "Draw Service Controls:");

	if (dat->System_File_Flag (LINK)) {

		//---- draw lanes ----

		if (exe->Control_Key_Status (DRAW_NETWORK_LANES)) {
			lanes_flag = exe->Get_Control_Flag (DRAW_NETWORK_LANES);
		}

		//---- draw vehicle shapes ----

		if (exe->Control_Key_Status (DRAW_VEHICLE_SHAPES)) {
			shape_flag = exe->Set_Control_Flag (DRAW_VEHICLE_SHAPES);
		}

		//---- lane width ----

		if ((lanes_flag || shape_flag || dat->System_File_Flag (CONNECTION)) && exe->Control_Key_Status (LANE_WIDTH)) {
			lane_width = exe->Get_Control_Double (LANE_WIDTH);

			if (lanes_flag && lane_width == 0.0) {
				exe->Error ("Lane Width must be > 0.0 to Draw Lanes");
			}
		}

		//---- center oneway links ----

		if (exe->Control_Key_Status (CENTER_ONEWAY_LINKS)) {
			center_flag = exe->Get_Control_Flag (CENTER_ONEWAY_LINKS);
		}

		if (!lanes_flag) {

			//---- link direction offset ----

			if (exe->Control_Key_Status (LINK_DIRECTION_OFFSET)) {
				link_offset = exe->Get_Control_Double (LINK_DIRECTION_OFFSET);

				if (link_offset == 0.0 && exe->Control_Key_Status (DRAW_AB_DIRECTION)) {
					direction_flag = exe->Get_Control_Flag (DRAW_AB_DIRECTION);
				}
			}

			//---- pocket side offset ----

			if (dat->System_File_Flag (POCKET) && exe->Control_Key_Status (POCKET_SIDE_OFFSET)) {
				pocket_side = exe->Get_Control_Double (POCKET_SIDE_OFFSET);
			}
		}
	}

	//---- activity location side offset ----

	if (dat->System_File_Flag (LOCATION) && exe->Control_Key_Status (LOCATION_SIDE_OFFSET)) {
		location_side = exe->Get_Control_Double (LOCATION_SIDE_OFFSET);
	}

	//---- parking side offset ----

	if (dat->System_File_Flag (PARKING) && exe->Control_Key_Status (PARKING_SIDE_OFFSET)) {
		parking_side = exe->Get_Control_Double (PARKING_SIDE_OFFSET);
	}

	//---- sign side offsets ----

	if (dat->System_File_Flag (SIGN)) {
		if (exe->Control_Key_Status (SIGN_SIDE_OFFSET)) {
			sign_side = exe->Get_Control_Double (SIGN_SIDE_OFFSET);
		}
		if (exe->Control_Key_Status (SIGN_SETBACK)) {
			sign_setback = exe->Get_Control_Double (SIGN_SETBACK);
		}
	}

	//---- transit stop offset ----

	if (dat->System_File_Flag (TRANSIT_STOP) && exe->Control_Key_Status (TRANSIT_STOP_SIDE_OFFSET)) {
		stop_side = exe->Get_Control_Double (TRANSIT_STOP_SIDE_OFFSET);
	}

	//---- transit direction offset ----

	if (dat->System_File_Flag (TRANSIT_ROUTE) || dat->System_File_Flag (TRANSIT_DRIVER)) {

		if (exe->Control_Key_Status (TRANSIT_DIRECTION_OFFSET)) {
			route_offset = exe->Get_Control_Double (TRANSIT_DIRECTION_OFFSET);
		}

		//---- transit overlap ----

		if (exe->Control_Key_Status (TRANSIT_OVERLAP_FLAG)) {
			overlap_flag = exe->Get_Control_Flag (TRANSIT_OVERLAP_FLAG);

			if (!overlap_flag) {
				if (route_offset == 0.0) {
					exe->Error ("Transit Direction Offset is Required for Overlap Processing");
				}
				if (dat->System_File_Flag (TRANSIT_ROUTE) && !dat->System_File_Flag (TRANSIT_DRIVER)) {
					exe->Error ("Transit Driver file is Required for Overlap Processing");
				}
			}
		}
	}

	//---- maximum shape angle ----

	if (exe->Control_Key_Status (MAXIMUM_SHAPE_ANGLE)) {
		max_angle= exe->Get_Control_Integer (MAXIMUM_SHAPE_ANGLE);
	}

	//---- minimum shape length ----

	if (exe->Control_Key_Status (MINIMUM_SHAPE_LENGTH)) {
		min_length = exe->Get_Control_Integer (MINIMUM_SHAPE_LENGTH);
	}

	//---- oneway arrow ----

	if (dat->System_File_Flag (LINK) && exe->Control_Key_Status (DRAW_ONEWAY_ARROWS) &&
		exe->Check_Control_Key (DRAW_ONEWAY_ARROWS)) {
		exe->Print (1);
		arrow_flag = exe->Get_Control_Flag (DRAW_ONEWAY_ARROWS);
		
		if (exe->Control_Key_Status (ONEWAY_ARROW_LENGTH)) {
			if (exe->Check_Control_Key (ONEWAY_ARROW_LENGTH) || lane_width == 0.0) {
				arrow_length = exe->Get_Control_Double (ONEWAY_ARROW_LENGTH);
			} else {
				arrow_length = lane_width * 2.0;
			}
		}
		if (exe->Control_Key_Status (ONEWAY_ARROW_SIDE_OFFSET)) {
			if (exe->Check_Control_Key (ONEWAY_ARROW_SIDE_OFFSET) || lane_width == 0.0) {
				arrow_side = exe->Get_Control_Double (ONEWAY_ARROW_SIDE_OFFSET);
			} else {
				arrow_side = lane_width / 2.0;
			}
		}
	}

	//---- curved connections ----

	if (exe->Control_Key_Status (CURVED_CONNECTION_FLAG)) {
		if ((dat->System_File_Flag (CONNECTION) || dat->System_File_Flag (PHASING_PLAN)) && 
			exe->Check_Control_Key (CURVED_CONNECTION_FLAG)) {
			exe->Print (1);
			curve_flag = exe->Get_Control_Flag (CURVED_CONNECTION_FLAG);
		}
	}

	//---- draw vehicle shapes ----

	if (exe->Control_Key_Status (DRAW_VEHICLE_SHAPES)) {
		shape_flag = exe->Get_Control_Flag (DRAW_VEHICLE_SHAPES);
	}

	//---- bandwidth field ----

	if (exe->Control_Key_Status (BANDWIDTH_FIELD)) {
		exe->Print (1);
		bandwidth_field = exe->Get_Control_Text (BANDWIDTH_FIELD);

		if (!bandwidth_field.empty ()) {
			bandwidth_flag = true;

			if (bandwidth_options.size () > 0) {
				Str_Itr str_itr;
				bool flag = false;

				for (str_itr = bandwidth_options.begin (); str_itr != bandwidth_options.end (); str_itr++) {
					if (str_itr->Equals (bandwidth_field)) {
						flag = true;
						break;
					}
				}
				if (!flag) {
					exe->Error (String ("Bandwidth Field %s was Not Recognized") % bandwidth_field);
				}
			}

			//---- bandwidth scaling factor ----

			width_factor= exe->Get_Control_Double (BANDWIDTH_SCALING_FACTOR);

			//---- minimum bandwidth value ----

			min_value = exe->Get_Control_Double (MINIMUM_BANDWIDTH_VALUE);

			//---- minimum bandwidth size ----

			min_width = exe->Get_Control_Double (MINIMUM_BANDWIDTH_SIZE);

			//---- maximum bandwidth size ----

			max_width = exe->Get_Control_Double (MAXIMUM_BANDWIDTH_SIZE);
		}
	}
}

//---------------------------------------------------------
//	Add_Arrow
//---------------------------------------------------------

void Draw_Service::Add_Arrow (Points &points)
{
	XYZ_Point point, end_point;
	Points_RItr itr;

	double dx, dy, dz, length, arrow, side;

	side = arrow_side;
	arrow = arrow_length;
	dx = dy = dz = 0.0;

	itr = points.rbegin ();
	end_point = *itr;

	for (++itr; itr != points.rend (); itr++) {
		dx = end_point.x - itr->x;
		dy = end_point.y - itr->y;
		dz = end_point.z - itr->z;

		length = dx * dx + dy * dy;

		if (length >= arrow) {
			length = sqrt (length);
			dx /= length;
			dy /= length;
			dz /= length;
			break;
		}
	}
	if (itr == points.rend ()) return;

	point.x = end_point.x - arrow * dx + side * dy;
	point.y = end_point.y - arrow * dy - side * dx;
	point.z = end_point.z - arrow * dz;

	points.push_back (point);

	point.x = end_point.x - arrow * dx - side * dy;
	point.y = end_point.y - arrow * dy + side * dx;

	points.push_back (point);
	points.push_back (end_point);
}

//---------------------------------------------------------
//	Point_Offset
//---------------------------------------------------------

void Draw_Service::Point_Offset (int link, int dir, int offset, double side, Points &points)
{
	Link_Data *link_ptr;

	link_ptr = &dat->link_array [link];

	if (lanes_flag) {
		bool dir_flag;
		int index, center, num_lanes;
		Dir_Data *dir_ptr;

		if (dir == 0) {
			index = link_ptr->AB_Dir ();
			dir_flag = (link_ptr->BA_Dir () >= 0);
		} else {
			index = link_ptr->BA_Dir ();
			dir_flag = (link_ptr->AB_Dir () >= 0);
		}
		if (index >= 0) {
			dir_ptr = &dat->dir_array [index];
			num_lanes = dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right ();
		} else {
			num_lanes = 0;
		}
		if (center_flag && !dir_flag) {
			center = num_lanes + 1;
		} else {
			center = 1;
		}
		side += (2 * num_lanes - center) * lane_width / 2.0;
	}
	dat->Link_Shape (link_ptr, dir, points, exe->UnRound (offset), 0.0, side);
}

//---------------------------------------------------------
//	Set_Bandwidth_Options
//---------------------------------------------------------

void Draw_Service::Set_Bandwidth_Options (Strings options)
{
	bandwidth_options = options;

	exe->Set_Range_String (BANDWIDTH_FIELD, options);
}

void Draw_Service::Set_Bandwidth_Options (String options)
{
	exe->Set_Range_String (BANDWIDTH_FIELD, options);

	options.Parse (bandwidth_options, ",");
}

void Draw_Service::Set_Bandwidth_Options (const char *options)
{
	String opt = options;

	exe->Set_Range_String (BANDWIDTH_FIELD, opt);
}
