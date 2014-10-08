//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Progression::Program_Control (void)
{
	String key;
	Db_Header *file;

	link_equiv.Sort_Flag (false);

	//---- create the network files ----

	Data_Service::Program_Control ();

	delay_flag = System_File_Flag (PERFORMANCE);

	//---- set the field names ----

	Copy_File_Header (SIGNAL, NEW_SIGNAL);

	//---- setup the signal database ----

	file = (Db_Header *) System_File_Handle (SIGNAL);

	signal_db.Replicate_Fields (file, true);
	signal_db.File_ID ("Signal");

	//---- clear existing offsets ----

	clear_flag = Get_Control_Flag (CLEAR_EXISTING_OFFSETS);

	//---- evaluate existing offsets ----

	eval_flag = Get_Control_Flag (EVALUATE_EXISTING_OFFSETS);

	//---- progression time of day ----

	key = Get_Control_Text (PROGRESSION_TIME_PERIODS);

	if (!key.empty ()) {
		progress_time.Add_Ranges (key);
		period_flag = true;
	} else {
		progress_time.Add_Ranges ("0:00..24:00");
	}

	//---- progression speed ----

	key = Get_Control_List_Groups (PROGRESSION_PERIOD_SPEED, progress_speed);

	//---- optimization method ----

	key = Get_Control_String (OPTIMIZATION_METHOD);

	if (!key.empty ()) {
		if (key.Equals ("PERCENT_THRU")) {
			method = PERCENT_THRU;
		} else if (key.Equals("NETWORK_SPEED")) {
			method = MAX_SPEED;
		} else if (key.Equals ("VEHICLE_HOURS")) {
			method = VEH_HOURS;
		} else {
			Error (String ("Unrecognized Optimization Method = %s") % key);
		}
	} else {
		Print (2, "Optimization Method = PERCENT_THRU");
	}

	//---- group period weight file ----

	key = Get_Control_String (GROUP_PERIOD_WEIGHT_FILE);

	if (!key.empty ()) {
		Print (1);
		weight_file.File_Type ("Group Period Weight File");

		if (!weight_file.Open (Project_Filename (key))) {
			Error ("Opening Period Weights");
		}
		weight_flag = true;
	}

	//---- keep link group order ----

	order_flag = Get_Control_Flag (KEEP_LINK_GROUP_ORDER);

	if (order_flag && !Link_Equiv_Flag ()) {
		Error ("A Link Equivalance File is required to Keep Link Group Order");
	}

	//---- get the arcview progression file ----

	key = Get_Control_String (NEW_ARC_PROGRESSION_FILE);

	if (!key.empty ()) {
		arcview_flag = true;

		projection.Read_Control ();

		Print (1);
		arcview_file.File_Type ("New Arc Progression File");
		arcview_file.File_Access (CREATE);
		arcview_file.Shape_Type (VECTOR);

		arcview_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		period_fld = arcview_file.Add_Field ("PERIOD", DB_INTEGER, 2);
		order_fld = arcview_file.Add_Field ("ORDER", DB_INTEGER, 5);
		group_fld = arcview_file.Add_Field ("GROUP", DB_INTEGER, 5);
		percent_fld = arcview_file.Add_Field ("PERCENT", DB_DOUBLE, 6.1);
		time_fld = arcview_file.Add_Field ("TTIME", DB_TIME, 6, MINUTES);
		length_fld = arcview_file.Add_Field ("LENGTH", DB_INTEGER, 6, FEET);
		speed_fld = arcview_file.Add_Field ("SPEED", DB_DOUBLE, 6.1, MPH);
		vht_fld = arcview_file.Add_Field ("VHT", DB_INTEGER, 10, VHT);

		if (!arcview_file.Open (Project_Filename (key))) {
			File_Error ("Opening Arc Progression File", arcview_file.Shape_Filename ());
		}
		arcview_file.Write_Header ();

		//---- link direction offset ----

		link_offset = Get_Control_Double (LINK_DIRECTION_OFFSET);
	}

	//---- read report types ----

	List_Reports ();

	//---- process support data ----

	if (Link_Equiv_Flag ()) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}
} 
