//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TripSum.hpp"

#include "TDF_Matrix.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TripSum::Program_Control (void)
{
	int p, num_periods, nzone;
	String key, field, format;
	bool period_flag;

	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- create the network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	trip_file = System_Trip_File ();
	if (!trip_file->Part_Flag ()) Num_Threads (1);

	select_flag = System_File_Flag (SELECTION);

	Print (2, String ("%s Control Keys:") % Program ());

	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) {
		Warning ("Summary Time Periods were Not Specified");
		Show_Message (1);

		sum_periods.Copy_Periods (time_periods);
		num_periods = sum_periods.Num_Periods ();
	}

	//---- time distribution file ----

	key = Get_Control_String (NEW_TIME_DISTRIBUTION_FILE);

	if (!key.empty ()) {
		Print (1);
		diurnal_file.File_Type ("New Time Distribution File");

		diurnal_file.Create (Project_Filename (key));
		time_flag = diurnal_flag = true;
	}

	//---- trip time file ----

	key = Get_Control_String (NEW_TRIP_TIME_FILE);

	if (!key.empty ()) {
		Print (1);
		trip_time_file.File_Type ("New Trip Time File");

		trip_time_file.Create (Project_Filename (key));
		time_file_flag = true;
	}

	//---- trip length file ----

	key = Get_Control_String (NEW_TRIP_LENGTH_FILE);

	if (!key.empty ()) {
		Print (1);
		trip_len_file.File_Type ("New Trip Length File");

		trip_len_file.Create (Project_Filename (key));
		len_file_flag = true;
	}

	//---- link trip end file ----

	key = Get_Control_String (NEW_LINK_TRIP_END_FILE);

	if (!key.empty ()) {
		Print (1);
		link_trip_file.File_Type ("New Link Trip End File");

		if (Check_Control_Key (NEW_LINK_TRIP_END_FORMAT)) {
			link_trip_file.Dbase_Format (Get_Control_String (NEW_LINK_TRIP_END_FORMAT));
		}
		link_trip_file.Add_Field ("LINK", DB_INTEGER, 10);

		for (p=0; p < num_periods; p++) {
			field = "O_" + sum_periods.Range_Label (p);
			link_trip_file.Add_Field (field, DB_INTEGER, 10);

			field [0] = 'D';
			link_trip_file.Add_Field (field, DB_INTEGER, 10);
		}
		link_trip_file.Create (Project_Filename (key));
		link_flag = link_trip_flag = true;
	}

	//---- location trip end file ----

	key = Get_Control_String (NEW_LOCATION_TRIP_END_FILE);

	if (!key.empty ()) {
		Print (1);
		loc_trip_file.File_Type ("New Location Trip End File");

		if (Check_Control_Key (NEW_LOCATION_TRIP_END_FORMAT)) {
			loc_trip_file.Dbase_Format (Get_Control_String (NEW_LOCATION_TRIP_END_FORMAT));
		}
		loc_trip_file.Add_Field ("LOCATION", DB_INTEGER, 10);

		for (p=0; p < num_periods; p++) {
			field = "O_" + sum_periods.Range_Label (p);
			loc_trip_file.Add_Field (field, DB_INTEGER, 10);

			field [0] = 'D';
			loc_trip_file.Add_Field (field, DB_INTEGER, 10);
		}
		loc_trip_file.Create (Project_Filename (key));
		loc_trip_flag = true;
	}

	//---- zone trip end file ----

	key = Get_Control_String (NEW_ZONE_TRIP_END_FILE);

	if (!key.empty ()) {
		Print (1);
		zone_trip_file.File_Type ("New Zone Trip End File");

		if (Check_Control_Key (NEW_ZONE_TRIP_END_FORMAT)) {
			zone_trip_file.Dbase_Format (Get_Control_String (NEW_ZONE_TRIP_END_FORMAT));
		}
		zone_trip_file.Add_Field ("ZONE", DB_INTEGER, 10);

		if (System_File_Flag (ZONE)) {
			zone_trip_file.Add_Field ("X_COORD", DB_DOUBLE, 14.1, FEET);
			zone_trip_file.Add_Field ("Y_COORD", DB_DOUBLE, 14.1, FEET);
		}

		for (p=0; p < num_periods; p++) {
			field = "O_" + sum_periods.Range_Label (p);
			zone_trip_file.Add_Field (field, DB_INTEGER, 10);

			field [0] = 'D';
			zone_trip_file.Add_Field (field, DB_INTEGER, 10);
		}
		zone_trip_file.Create (Project_Filename (key));
		zone_trip_flag = true;
	}

	//---- trip table file ----

	key = Get_Control_String (NEW_TRIP_TABLE_FILE);

	if (!key.empty ()) {
		Print (1);
		trip_table_flag = true;

		if (Check_Control_Key (NEW_TRIP_TABLE_FORMAT)) {
			format = Get_Control_String (NEW_TRIP_TABLE_FORMAT);
		} else {
			format = Get_Default_Text (NEW_TRIP_TABLE_FORMAT);
		}
		trip_table_file = TDF_Matrix (CREATE, format);

		trip_table_file->File_Type ("New Trip Table File");
		trip_table_file->File_ID ("NewTable");

		trip_table_file->Filename (Project_Filename (key));

		//---- build matrix fields ----

		trip_table_file->Copy_Periods (sum_periods);
		if (trip_table_file->Num_Periods () > 1) {
			period_flag = true;
		} else if (trip_table_file->Num_Periods () == 0) {
			period_flag = false;
		} else {
			Dtime low, high;
			trip_table_file->Period_Range (0, low, high);
			period_flag = (low != Model_Start_Time () || high != Model_End_Time ());
		}
		trip_table_file->Clear_Fields ();

		trip_table_file->Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
		trip_table_file->Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);

		if (period_flag) {
			trip_table_file->Add_Field ("PERIOD", DB_INTEGER, 2, NO_UNITS, true);
		}
		trip_table_file->Add_Field ("TRIPS", DB_INTEGER, 4, NO_UNITS, true);

		nzone = Max_Zone_Number ();

		//---- create the matrix and allocation memory ----

		if (nzone <= 0) {
			Int_Set zones;

			if (System_File_Flag (LOCATION)) {

				Show_Message ("Scanning Location File for Maximum Zone Number -- Record");
				Set_Progress ();

				Location_File *file = System_Location_File ();

				while (file->Read ()) {
					Show_Progress ();

					nzone = file->Zone ();
					if (nzone > 0) {
						zones.insert (nzone);
					}
				}
				End_Progress ();

				file->Rewind ();

			} else if (System_File_Flag (ZONE)) {
				Zone_File *file = System_Zone_File ();

				Show_Message ("Scanning Zone File for Maximum Zone Number -- Record");
				Set_Progress ();

				while (file->Read ()) {
					Show_Progress ();

					nzone = file->Zone ();
					if (nzone > 0) {
						zones.insert (nzone);
					}
				}
				End_Progress ();

				file->Rewind ();
			}
			nzone = (int) zones.size ();
		}

		if (nzone <= 0) {
			Error ("Highest Zone Number is Not Defined");
		}
		trip_table_file->Num_Org (nzone);
		trip_table_file->Num_Des (nzone);

		Print (1);
		trip_table_file->Create ();

		if (trip_table_file->Num_Periods () > 1) {
			Print (0, " (Periods=") << trip_table_file->Num_Periods () << " Zones=" << trip_table_file->Num_Zones () << " Tables=" << trip_table_file->Tables () << ")";
		} else {
			Print (0, " (Zones=") << trip_table_file->Num_Zones () << " Tables=" << trip_table_file->Tables () << ")";
		}
		if (!trip_table_file->Allocate_Data (true)) {
			Error ("Insufficient Memory for New Matrix");
		}
	}
	
	//---- trip time increment ----

	time_increment = Get_Control_Time (TRIP_TIME_INCREMENT);

	//---- trip length increment ----

	len_increment = Round (Get_Control_Double (TRIP_LENGTH_INCREMENT));
	distance_units = Current_Units ();

	//---- distance calculation ----

	key = Get_Control_Text (DISTANCE_CALCULATION);

	if (!key.empty ()) {
		distance_type = Distance_Code (key);
	}

	//---- read report types ----

	List_Reports ();

	if (Report_Flag (TOP_100_LINKS) || Report_Flag (TOP_100_LANES) || Report_Flag (TOP_100_VC_RATIOS)) link_flag = true; 

	time_report = Report_Flag (TRIP_TIME);
	trip_len_flag = Report_Flag (TRIP_LENGTH);
	trip_purp_flag = Report_Flag (TRIP_PURPOSE);
	mode_len_flag = Report_Flag (MODE_LENGTH);
	mode_purp_flag = Report_Flag (MODE_PURPOSE);

	length_flag = (trip_len_flag || trip_purp_flag || mode_len_flag || mode_purp_flag || len_file_flag);
	if (length_flag) Location_XY_Flag (true);

	if (link_flag || link_trip_flag || loc_trip_flag || zone_trip_flag || trip_table_flag || length_flag) {
		if (!System_File_Flag (NODE) || !System_File_Flag (LINK) || !System_File_Flag (LOCATION)) {
			Error ("Network-Based Summarizes require Network Files");
		}
	}
	if (zone_trip_flag || trip_table_flag) {
		if (!System_File_Flag (ZONE) || !System_File_Flag (NODE) || !System_File_Flag (LINK) || !System_File_Flag (LOCATION)) {
			Error ("Zone-Based Summarizes require Network and Zone Files");
		}
	}
	if (Report_Flag (TRIP_DIURNAL)) time_flag = true;

	if (time_flag) {
		start_time.assign (num_periods, 0);
		mid_time.assign (num_periods, 0);
		end_time.assign (num_periods, 0);
	}
	if (trip_len_flag) trip_length.Copy_Periods (sum_periods);

	//---- process support data ----

	if (Zone_Equiv_Flag ()) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
	if (Time_Equiv_Flag ()) {
		time_equiv.Read (Report_Flag (TIME_EQUIV));
	}
}
