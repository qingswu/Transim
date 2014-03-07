//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void MatrixData::Execute (void)
{
	String text, label;

	//---- read the network ----

	Data_Service::Execute ();

	//---- read the zone labels ----

	if (label_flag) {
		while (label_file.Read ()) {
			text = label_file.Record_String ();
			text.Split (label);
			int group = label.Integer ();

			if (equiv_flag) {
				label = zone_equiv.Group_Label (group);
				if (label.empty ()) {
					zone_equiv.Group_Label (group, text);
				}
			} else {
				label_map.insert (Str_Map_Data (group, text));
			}
		}
	}

	//---- process data files ----

	if (data_flag) {
		Read_Data ();
	}

	//---- process matrix files ----

	if (method == EXPAND_METHOD) {
		Expand_Matrix ();
	} else if (matrix_flag) {
		Read_Matrix ();

		if (method == RATIO_METHOD) {
			Matrix_Ratios ();
		}
	} else if (input_flag) {
		new_matrix->Copy_Matrix_Data (input_matrix);
	}

	//---- process factor file ----

	if (factor_flag) {
		Read_Factors ();
	}

	//---- write the new matrix ----

	if (new_flag) {
		new_matrix->Write_Matrix ();
		new_matrix->Close ();
	}

	//---- write margin totals ----

	if (margin_flag) {
		Write_Margin_Totals ();
	}

	//---- write square table ----

	if (square_flag) {
		Write_Square_Table ();
	}

	//---- write start times ----

	if (start_flag) {
		Write_Start_Times ();
	}

	//---- write end times ----

	if (end_flag) {
		Write_End_Times ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case STATS_REPORT:		//---- Field Statistics ----
				Statistics_Report ();
				break;
			case MARGIN_REPORT:		//---- Margin Totals ----
				Margin_Total_Report ();
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

void MatrixData::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		case STATS_REPORT:		//---- Field Statistics ----
			Statistics_Header ();
			break;
		case MARGIN_REPORT:		//---- Margin Totals ----
			Margin_Total_Header ();
			break;
		default:
			break;
	}
}
