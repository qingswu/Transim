//*********************************************************
//	Control.cpp - path building service
//*********************************************************

#include "Router_Service.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Router_Service::Program_Control (void)
{
	int i;
	double dvalue;
	String text, result;
	Double_List value_list;

	Data_Service::Program_Control ();

	Read_Flow_Time_Keys ();

	Print (2, "Path Building Parameters:");

	memset (&param, '\0', sizeof (param));

	//---- check the transit flags ----

	if (transit_path_flag) {
		if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) ||
			!System_File_Flag (TRANSIT_SCHEDULE)) transit_path_flag = false;
	}
	if (parkride_path_flag && !transit_path_flag) parkride_path_flag = false;
	if (kissride_path_flag && !transit_path_flag) kissride_path_flag = false;

	if (parkride_path_flag || kissride_path_flag) Location_XY_Flag (true);
	param.grade_flag = false;

	//---- impedance sort method ----

	param.sort_method = Get_Control_Flag (IMPEDANCE_SORT_METHOD);

	//---- save only skims ----

	param.skim_only = Get_Control_Flag (SAVE_ONLY_SKIMS);

	//---- write walk path ----

	if (walk_path_flag) {
		param.walk_detail = Get_Control_Flag (WALK_PATH_DETAILS);
	}

	//---- get vehicle flags ----

	if (drive_path_flag) {
		param.ignore_veh_flag = Get_Control_Flag (IGNORE_VEHICLE_ID);

		param.limit_access = Get_Control_Flag (LIMIT_PARKING_ACCESS);

		param.delay_flag = System_File_Flag (LINK_DELAY);
		if (param.delay_flag) {
			Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);
			param.turn_delay_flag = file->Turn_Flag ();
		} else {
			param.turn_delay_flag = false;
			if (Time_Updates () && update_rate > 0) param.delay_flag = true;
		}
	}

	//---- adjust activity schedule ----

	param.adjust_schedule = Get_Control_Flag (ADJUST_ACTIVITY_SCHEDULE);

	//---- ignore activity durations ----

	param.ignore_duration = Get_Control_Flag (IGNORE_ACTIVITY_DURATIONS);

	//---- ignore time constraints ----
	
	param.ignore_time = Get_Control_Flag (IGNORE_TIME_CONSTRAINTS);

	if (!param.ignore_time) {
		param.end_time = Get_Control_Time (END_TIME_CONSTRAINT); 
	}

	//---- ignore routing errors ----
	
	param.ignore_errors = Get_Control_Flag (IGNORE_ROUTING_PROBLEMS);

	//---- transit capacity penalty ----
	
	param.cap_penalty_flag = Get_Control_Flag (TRANSIT_CAPACITY_PENALTY);

	//---- random impedance ----
	
	param.random_imped = Get_Control_Double (PERCENT_RANDOM_IMPEDANCE);

	//---- open traveler type script ----

	if (System_File_Flag (HOUSEHOLD)) {
		hhfile_flag = true;
		text = Get_Control_String (TRAVELER_TYPE_SCRIPT);

		if (!text.empty ()) {
			Print (1);
			script_file.File_Type ("Traveler Type Script");

			script_file.Open (Project_Filename (text));
			script_flag = true;
		}
	}

	//---- open traveler parameter file ----

	text = Get_Control_String (TRAVELER_PARAMETER_FILE);

	if (!text.empty ()) {
		Db_File file;
		file.File_Type ("Traveler Parameter File");
		if (!script_flag) Print (1);

		if (file.Open (Project_Filename (text))) {
			bool first = true;

			while (file.Read ()) {
				if (first) {
					first = false;
					text = file.Record_String ();
					if (text.Starts_With ("Key")) continue;
				}
				Process_Control_String (file.Record_String ());
			}
		}
	}
	Print (1);

	//---- walk speed ----
	
	param.walk_speed = Get_Control_Double (WALK_SPEED);

	//---- bicycle speed ----

	param.bike_speed = Get_Control_Double (BICYCLE_SPEED);

	//---- walk time value ----

	Get_Control_List_Groups (WALK_TIME_VALUES, value_walk);
	param.value_walk = value_walk [0];

	//---- bicycle time value ----

	if (bike_path_flag) {
		Get_Control_List_Groups (BICYCLE_TIME_VALUES, value_bike);
		param.value_bike = value_bike [0];
	}

	if (transit_path_flag) {
		Print (1);

		//---- first wait value ----

		Get_Control_List_Groups (FIRST_WAIT_VALUES, value_wait);
		param.value_wait = value_wait [0];

		//---- transfer wait value ----

		Get_Control_List_Groups (TRANSFER_WAIT_VALUES, value_xfer);
		param.value_xfer = value_xfer [0];
	}

	//---- parking time value ----

	Get_Control_List_Groups (PARKING_TIME_VALUES, value_park);
	param.value_park = value_park [0];

	//---- vehicle time value ----

	Get_Control_List_Groups (VEHICLE_TIME_VALUES, value_time);
	param.value_time = value_time [0];

	//---- distance value ----

	Get_Control_List_Groups (DISTANCE_VALUES, value_dist);
	param.value_dist = value_dist [0];

	//---- cost value ----

	Get_Control_List_Groups (COST_VALUES, value_cost);
	param.value_cost = value_cost [0];

	//---- check the impedance values ----

	for (i=1; i <= (int) value_time.size (); i++) {
		if (value_walk.Best (i) == 0 && value_wait.Best (i) == 0 && value_time.Best (i) == 0 &&
			value_dist.Best (i) == 0 && value_cost.Best (i) == 0) {

			Error (String ("At least One Impedance Value must be Non-Zero for Household Type %d") % i);
		}
	}

	if (drive_path_flag) {
		Print (1);
		//---- freeway bias factors ----

		Get_Control_List_Groups (FREEWAY_BIAS_FACTORS, freeway_fac);
		param.freeway_fac = freeway_fac [0];

		//---- expressway bias factors ----

		Get_Control_List_Groups (EXPRESSWAY_BIAS_FACTORS, express_fac);
		param.express_fac = express_fac [0];
		
		//---- left turn penalty ----

		Get_Control_List_Groups (LEFT_TURN_PENALTIES, left_imped);
		param.left_imped = left_imped [0] * 10;
			
		//---- right turn penalty ----
		
		Get_Control_List_Groups (RIGHT_TURN_PENALTIES, right_imped);
		param.right_imped = right_imped [0] * 10;

		//---- u-turn penalty ----
		
		Get_Control_List_Groups (U_TURN_PENALTIES, uturn_imped);
		param.uturn_imped = uturn_imped [0] * 10;
	}

	if (transit_path_flag) {
		Print (1);

		//---- transfer_penalty ----

		Get_Control_List_Groups (TRANSFER_PENALTIES, xfer_imped);
		param.xfer_imped = xfer_imped [0] * 10;	
	
		//---- stop waiting penalty ----
		
		Get_Control_List_Groups (STOP_WAITING_PENALTIES, stop_imped);
		param.stop_imped = stop_imped [0] * 10;	
		
		//---- station waiting penalty ----
		
		Get_Control_List_Groups (STATION_WAITING_PENALTIES, station_imped);
		param.station_imped = station_imped [0] * 10;	

		//---- bus bias factor ----
		
		Get_Control_List_Groups (BUS_BIAS_FACTORS, bus_bias);
		param.bus_bias = bus_bias [0];

		//---- bus bias constant ----

		Get_Control_List_Groups (BUS_BIAS_CONSTANTS, bus_const);
		param.bus_const = bus_const [0] * 10;

		//---- rail bias factor ----

		Get_Control_List_Groups (RAIL_BIAS_FACTORS, rail_bias);
		param.rail_bias = rail_bias [0];

		//---- rail bias constant ----
		
		Get_Control_List_Groups (RAIL_BIAS_CONSTANTS, rail_const);
		param.rail_const = rail_const [0] * 10;
	}

	//---- max walk distance ----

	Print (1);
	Get_Control_List_Groups (MAX_WALK_DISTANCES, max_walk);
	param.max_walk = Round (max_walk [0]);

	//---- walk penalty distance ----

	Get_Control_List_Groups (WALK_PENALTY_DISTANCES, walk_pen);
	param.walk_pen = Round (walk_pen [0]);

	//---- walk penalty factor ----

	Get_Control_List_Groups (WALK_PENALTY_FACTORS, walk_fac);
	param.walk_fac = walk_fac [0];

	if (bike_path_flag) {

		//---- max bike distance ----

		Get_Control_List_Groups (MAX_BICYCLE_DISTANCES, max_bike);
		param.max_bike = Round (max_bike [0]);

		//---- bike penalty distance ----

		Get_Control_List_Groups (BIKE_PENALTY_DISTANCES, bike_pen);
		param.bike_pen = Round (bike_pen [0]);

		//---- bike penalty factor ----

		Get_Control_List_Groups (BIKE_PENALTY_FACTORS, bike_fac);
		param.bike_fac = bike_fac [0];
	}

	if (transit_path_flag) {
		Print (1);

		//---- max wait time ----
		
		Get_Control_List_Groups (MAX_WAIT_TIMES, max_wait);
		param.max_wait = max_wait [0];

		//---- wait penalty distance ----

		Get_Control_List_Groups (WAIT_PENALTY_TIMES, wait_pen);
		param.wait_pen = wait_pen [0];

		//---- wait penalty factor ----

		Get_Control_List_Groups (WAIT_PENALTY_FACTORS, wait_fac);
		param.wait_fac = wait_fac [0];

		//---- min wait time ----
		
		Get_Control_List_Groups (MIN_WAIT_TIMES, min_wait);
		param.min_wait = min_wait [0];

		//---- max transfers ----

		Get_Control_List_Groups (MAX_NUMBER_OF_TRANSFERS, max_xfers);
		param.max_xfers = max_xfers [0];

		//---- max park ride percentage ----

		if (parkride_path_flag) {
			Print (1);
			Get_Control_List_Groups (MAX_PARK_RIDE_PERCENTS, max_parkride);
			param.max_parkride = max_parkride [0];
		}

		//---- max kiss ride percentage ----

		if (kissride_path_flag) {
			Print (1);
			Get_Control_List_Groups (MAX_KISS_RIDE_PERCENTS, max_kissride);
			param.max_kissride = max_kissride [0];

			//---- kiss ride time factor ----

			Get_Control_List_Groups (KISS_RIDE_TIME_FACTORS, kissride_fac); 
			param.kissride_fac = kissride_fac [0];

			//---- kiss ride stop types ----

			text = Get_Control_Text (KISS_RIDE_STOP_TYPES);

			if (!text.empty ()) {
				memset (param.kissride_type, '\0', sizeof (param.kissride_type));

				while (text.Split (result)) {
					i = Stop_Code (result);

					if (i < 0 || i > EXTLOAD) {
						Error (String ("Kiss-&-Ride Stop Type %s is Out of Range") % result);
					}
					param.kissride_type [i] = true;
				}
			} else {
				for (i=0; i <= EXTLOAD; i++) {
					param.kissride_type [i] = true;
				}
			}

			//---- max kiss ride dropoff walk ----

			param.kissride_walk = Get_Control_Integer (MAX_KISS_RIDE_DROPOFF_WALK);
		}

		//---- open the transit penalty file ----
		
		text = Get_Control_String (TRANSIT_PENALTY_FILE);

		if (!text.empty ()) {
			stop_pen_file.File_Type ("Transit Penalty File");
			Print (1);

			stop_pen_file.Open (Project_Filename (text));
			param.stop_pen_flag = true;

			stop_pen_file.Required_Field ("STOP", "STOP_ID", "ID");
			stop_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
		}
	}

	if (drive_path_flag || transit_path_flag) {

		//---- open the parking penalty file ----
		
		text = Get_Control_String (PARKING_PENALTY_FILE);

		if (!text.empty ()) {
			park_pen_file.File_Type ("Parking Penalty File");
			Print (1);

			park_pen_file.Open (Project_Filename (text));
			param.park_pen_flag = true;

			park_pen_file.Required_Field ("PARKING", "LOT", "ID", "NUMBER", "LOT_ID");
			park_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
		}

		//---- default parking duration ----

		Print (1);
		param.parking_duration = Get_Control_Time (DEFAULT_PARKING_DURATION);
	}

	//---- max legs per path ----

	param.leg_check = Get_Control_Integer (MAX_LEGS_PER_PATH);

	if (transit_path_flag) {
		Print (1);

		//---- max paths ----
		
		param.max_paths = Get_Control_Integer (MAX_NUMBER_OF_PATHS);

		//---- fare class distribution ----

		text = Get_Control_Text (FARE_CLASS_DISTRIBUTION);

		if (!text.empty ()) {
			memset (param.fare_class, '\0', sizeof (param.fare_class));
			param.fare_class_flag = true;
			i = 0;

			while (text.Split (result) && i <= SPECIAL) {
				dvalue = result.Double ();
	
				if (dvalue < 0.0 || dvalue > 100.0) {
					Error (String ("Fare Class Distribution %.2lf is Out of Range (0.0..100.0)") % dvalue);
				}
				param.fare_class [i++] = dvalue;
			}
			dvalue = 0;
			for (--i; i >= 0; i--) {
				dvalue += param.fare_class [i];
			}
			if (dvalue == 0.0) {
				param.fare_class [0] = 1.0;
			} else {
				dvalue = 1.0 / dvalue;

				for (i=0; i <= SPECIAL; i++) {
					param.fare_class [i] *= dvalue;
				}
			}

			//---- convert to a cumulative distribution ----

			dvalue = 0.0;

			for (i=0; i <= SPECIAL; i++) {
				dvalue += param.fare_class [i];
				param.fare_class [i] = dvalue;
			}
			param.fare_class [SPECIAL] = 1.0;
		}
	}

	if (drive_path_flag) {

		//---- local facility type ----

		Print (1);
		text = Get_Control_Text (LOCAL_FACILITY_TYPE);

		if (!text.empty ()) {
			param.local_type = Facility_Code (text);

			if (param.local_type != EXTERNAL) {
				if (param.local_type < MAJOR || param.local_type > LOCAL) {
					Error (String ("Local Facility Type %s is Out of Range (MAJOR..LOCAL)") % text);
				} else {
					param.local_flag = true;

					//---- local access distance ----

					param.local_distance = Get_Control_Integer (LOCAL_ACCESS_DISTANCE);

					//---- local impedance factor ----

					param.local_factor = Get_Control_Double (LOCAL_IMPEDANCE_FACTOR);
				}
			} else {
				param.local_flag = false;
			}
		}

		//---- max circuity ratio ----

		param.max_ratio = Get_Control_Double (MAX_CIRCUITY_RATIO);

		if (param.max_ratio != 0.0) {
			param.distance_flag = true;

			//---- min circuity distance ----

			param.min_distance = Get_Control_Integer (MIN_CIRCUITY_DISTANCE);

			//---- max circuity distance ----

			param.max_distance = Get_Control_Integer (MAX_CIRCUITY_DISTANCE);

			if (param.max_distance < param.min_distance) {
				Error ("Maximum Circuity Distance < Minimum Circuity Distance");
			}
		}
	}

	//---- minimum duration factors ----

	Get_Control_List_Groups (MIN_DURATION_FACTORS, value_list);

	for (i=0; i <= CRITICAL; i++) {
		param.duration_factor [i] = value_list.Best (i+1);
	}
}
