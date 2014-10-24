//*********************************************************
//	Get_Skim_Data.cpp - Read the Skim File
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Get_Skim_Data
//---------------------------------------------------------

bool NewFormat::Get_Skim_Data (Skim_File &file, Skim_Record &skim_rec, int partition)
{
	static int part_num = -1;
	static Skim_File *out_file;

	if (part_num != partition) {
		part_num = partition;
		out_file = System_Skim_File (true);

		if (out_file->Part_Flag () && part_num > 1) {
			if (!out_file->Open (part_num)) {
				Error (String ("Opening %s") % out_file->Filename ());
			}
			if (part_num > max_skim_part) max_skim_part = part_num;
		} else {
			max_skim_part = 1;
		}
	}
	if (Data_Service::Get_Skim_Data (file, skim_rec, partition)) {
		num_new_skim += Put_Skim_Data (*out_file, skim_rec);
	}
	return (false);
}
