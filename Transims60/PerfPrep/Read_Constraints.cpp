//*********************************************************
//	Read_Constraints.cpp - read time ratio constraint file
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Read_Time_Constraints
//---------------------------------------------------------

void PerfPrep::Read_Time_Constraints (void)
{
	int time_field, max_field, del_field;
	Constraint_Data constraint_data;

	Show_Message ("Reading Time Constraints -- Record");
	Set_Progress ();

	time_field = constraint_file.Required_Field ("PERIOD", "TIME", "TOD");
	max_field = constraint_file.Required_Field ("MAX", "MAXIMUM", "MAX_RATIO", "RATIO", "CONSTRAINT");
	del_field = constraint_file.Optional_Field ("DEL", "DELETE", "DEL_RATIO", "EXCLUDE", "LIMIT");

	//---- process each time period ----

	while (constraint_file.Read ()) {
		Show_Progress ();

		constraint_data.period = constraint_file.Get_Time (time_field);
		constraint_data.max_ratio = (int) (constraint_file.Get_Double (max_field) * 100);
		constraint_data.del_ratio = (int) (constraint_file.Get_Double (del_field) * 100);

		if (constraint_data.del_ratio == 0) constraint_data.del_ratio = Delete_Time_Ratio ();

		constraint_array.push_back (constraint_data);
	}
	End_Progress ();
}
