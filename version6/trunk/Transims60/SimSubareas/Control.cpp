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
	
	Node_File *file = (Node_File *) System_File_Handle (NEW_NODE);
	file->Subarea_Flag (true);
	file->Clear_Fields ();
	file->Create_Fields ();
	file->Write_Header ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- get the subarea boundary file ----

	key = Get_Control_String (SUBAREA_BOUNDARY_FILE);

	if (!key.empty ()) {

		boundary_file.File_Type ("Subarea Boundary File");

		boundary_file.Open (exe->Project_Filename (key));

		if (Check_Control_Key (SUBAREA_DATA_FIELD)) {
			key = Get_Control_Text (SUBAREA_DATA_FIELD);
			subarea_field = boundary_file.Required_Field (key);
			Print (0, String (" (Number = %d)") % (subarea_field + 1));
		} else {
			subarea_field = boundary_file.Required_Field ("SUBAREA", "ID", "NUMBER", "PARTITION", "SUB");
		}
		boundary_flag = true;

	} else {

		//---- get the number of subareas ----

		num_subareas = Get_Control_Integer (NUMBER_OF_SUBAREAS);

		subarea_count.assign (num_subareas, 0);

		//---- get the center node number ---

		center = Get_Control_Integer (CENTER_NODE_NUMBER);
	}
}
