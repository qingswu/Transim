//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TcadImport.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TcadImport::Execute (void)
{
	int i, z, size;

	//---- allocate matrix memory ----

	size = (int) (num_zones * sizeof (float));

	trips = new float ** [tables];

	for (i=0; i < tables; i++) {
		trips [i] = new float * [num_zones];

		for (z=0; z < num_zones; z++) {
			trips [i] [z] = new float [num_zones];
			memset (trips [i] [z], '\0', size);
		}
	}

	Process ();

	Exit_Stat (DONE);
}
