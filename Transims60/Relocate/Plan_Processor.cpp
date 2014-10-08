//*********************************************************
//	Plan_Processor.cpp - plan processing thread
//*********************************************************

#include "Relocate.hpp"

#ifdef THREADS

//---------------------------------------------------------
//	Plan_Processor operator
//---------------------------------------------------------

void Relocate::Plan_Processor::operator()()
{
	int number;
	Plan_Ptr plan_ptr;

	while (1) {
		plan_ptr = exe->plan_queue.Get_Work (number);
		if (plan_ptr == 0) break;

		exe->Process_Plan (plan_ptr);

		exe->plan_queue.Put_Result (plan_ptr, number);
	}
}

//---------------------------------------------------------
//	Save_Plan operator
//---------------------------------------------------------

void Relocate::Save_Plan::operator()()
{
	Plan_Ptr plan_ptr;
	Plan_File *new_file = (Plan_File *) exe->System_File_Handle (NEW_PLAN);

	while (1) {
		plan_ptr = exe->plan_queue.Get_Result ();
		if (plan_ptr == 0) break;

		if (plan_ptr->Index () != 0) {
			new_file->Write_Plan (*plan_ptr);
		}
		delete plan_ptr;
		exe->plan_queue.Finished ();
	}
}

#endif
