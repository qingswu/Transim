//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "SimSubareas.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SimSubareas::Program_Control (void)
{
	String key;

	//---- create the network files ----

	Data_Service::Program_Control ();
	
	Node_File *file = System_Node_File (true);
	file->Subarea_Flag (true);
	file->Clear_Fields ();
	file->Create_Fields ();
	file->Write_Header ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- get the subarea boundary file ----

	key = Get_Control_String (SUBAREA_BOUNDARY_FILE);

	if (!key.empty ()) {

		boundary_file.File_Type ("Subarea Boundary File");

		if (!boundary_file.Open (Project_Filename (key))) {
			Error (String ("Opening Boundary File %s") % boundary_file.Filename ());
		}
		if (Check_Control_Key (SUBAREA_DATA_FIELD)) {
			key = Get_Control_Text (SUBAREA_DATA_FIELD);
			subarea_field = boundary_file.Required_Field (key);
			Print (0, String (" (Number = %d)") % (subarea_field + 1));
		} else {
			subarea_field = boundary_file.Required_Field ("SUBAREA", "ID", "NUMBER", "PARTITION", "SUB");
		}
		boundary_flag = true;

		update_flag = Get_Control_Flag (SUBAREA_UPDATE_FLAG);

	} else {

		//---- get the number of subareas ----

		num_subareas = Get_Control_Integer (NUMBER_OF_SUBAREAS);

		subarea_count.assign (num_subareas, 0);

		//---- get the center node number ---

		center = Get_Control_Integer (CENTER_NODE_NUMBER);
	}
}
