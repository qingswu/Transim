//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void NewLandUse::Execute (void)
{
	int field;
	Str_Itr str_itr;
	Zone_Field_Itr field_itr;
	Processing_Itr process_itr;

	//---- read/open the system zone files ----

	Data_Service::Execute ();

	//---- read zone and target files ----

	Read_Files ();

	//---- coverage type density ----

	Apply_Density ();

	//---- process each data field ----

	for (field=0, str_itr = data_names.begin (); str_itr != data_names.end (); str_itr++, field++) {
		Print (1);
		Write (1, "Processing Data Field ") << *str_itr;
		Show_Message (1);

		if (field >= function.Num_Functions ()) {
			best_function = function.Num_Functions ();
		} else {
			best_function = field + 1;
		}
		for (process_itr = processing_steps.begin (); process_itr != processing_steps.end (); process_itr++) {
			Processing (field, *process_itr);
		}
	}

	//---- output the results ----

	Write_Data ();

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case GROUP_REPORT:		//---- Group Report ----
				Group_Report ((int) Report_Data ());
				break;
			default:
				break;
		}
	}

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void NewLandUse::Page_Header (void)
{
	switch (Header_Number ()) {
		case GROUP_REPORT:
			Group_Header ();
			break;
		default:
			break;
	}
}
