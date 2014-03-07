//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcDelay::Execute (void)
{

	//---- read the network ----

	Data_Service::Execute ();

	//---- flag selected links ----

	if (select_links || select_subarea || select_facilities) {
		Select_Links ();
	}

	//---- write the arcview delay file ----

	if (delay_flag) {
		if (max_period_flag) {
			Maximum_Delay ();
		} else {
			Write_Delay ();
		}
	}

	//---- write the arcview intersection file ----

	if (intersection_flag) {
		Write_Intersection ();
	}

	//---- write the arcview turn file ----

	if (turn_flag) {
		Write_Turn ();
	}

	//---- write the arcview traffic image ----

	if (image_flag) {
		Write_Image ();
	}

	//---- read the link data file ----

	if (link_data_flag) {
		Read_Link_Data ();
	}

	//---- read the link direction file ----

	if (link_dir_flag) {
		Read_Link_Dir ();
	}
	Exit_Stat (DONE);
}

