//*********************************************************
//	Write_Problem.cpp -  save record to the problem file
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Write_Problem
//---------------------------------------------------------

void Converge_Service::Write_Problem (Problem_File *file, Plan_Data *plan_ptr)
{
	file->Problem (plan_ptr->Problem ());
	file->Household (plan_ptr->Household ());
	file->Person (plan_ptr->Person ());
	file->Tour (plan_ptr->Tour ());
	file->Trip (plan_ptr->Trip ());
	file->Start (plan_ptr->Start ());
	file->End (plan_ptr->End ());
	file->Duration (plan_ptr->Duration ());
	file->Origin (plan_ptr->Origin ());
	file->Destination (plan_ptr->Destination ());
	file->Purpose (plan_ptr->Purpose ());
	file->Mode (plan_ptr->Mode ());
	file->Constraint (plan_ptr->Constraint ());
	file->Priority (plan_ptr->Priority ());
	file->Veh_Type (plan_ptr->Veh_Type ());
	file->Vehicle (plan_ptr->Vehicle ());
	file->Type (plan_ptr->Type ());
	file->Notes ((char *) Problem_Code ((Problem_Type) plan_ptr->Problem ()));

	if (!file->Write ()) {
		Warning ("Writing ") << file->File_Type ();
		problem_flag = false;
	}
	file->Add_Trip (plan_ptr->Household (), plan_ptr->Person (), plan_ptr->Tour ());
}
