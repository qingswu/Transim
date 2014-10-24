//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ProblemSelect.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ProblemSelect::Program_Control (void)
{
	bool partition_flag, type_flag;
	Selection_File *file;

	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	//---- check the selection file ----

	partition_flag = (First_Partition () >= 0);
	type_flag = false;

	if (System_File_Flag (SELECTION)) {
		file = System_Selection_File ();
		if (file->Part_Flag () || file->Partition_Flag ()) {
			partition_flag = true;
		}
		type_flag = (file->Type_Flag ());
	}
	file = System_Selection_File (true);
	if (!file->Part_Flag ()) partition_flag = true;

	if (partition_flag || type_flag) {
		file->Clear_Fields ();
		file->Partition_Flag (partition_flag);
		file->Type_Flag (type_flag);
		file->Create_Fields ();
		file->Write_Header ();
	}
	Print (2, String ("%s Control Keys:") % Program ());
} 
