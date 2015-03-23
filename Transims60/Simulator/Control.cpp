//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Simulator::Program_Control (void)
{
	String key, ext;

	Simulator_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- get the new simulation backup file ----

	key = Get_Control_String (NEW_SIMULATION_BACKUP_FILE);

	if (!key.empty ()) {
		backup_flag = true;

		key.Split_Last (backup_ext, ".");

		backup_name = Project_Filename (key);

		key = backup_name + "_time_of_day." + backup_ext;

		Print_Filename (2, "New Simulation Backup File", key);

		Get_Control_List (BACKUP_TIME_POINTS, backup_times);
		
		if ((int) backup_times.size () == 0) {
			Error ("Backup Time Points are Required for Backup Processing");
		}
	}
}
