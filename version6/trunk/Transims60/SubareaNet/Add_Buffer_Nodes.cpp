//*********************************************************
//	Add_Buffer_Nodes.cpp - flag nodes in the subarea buffer
//*********************************************************

#include "SubareaNet.hpp"

#include <math.h>

//---------------------------------------------------------
//	Add_Buffer_Nodes
//---------------------------------------------------------

void SubareaNet::Add_Buffer_Nodes (void)
{
	double dx, dy;

	Node_Itr node_itr, nod_itr;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () == 1) {
			for (nod_itr = node_array.begin (); nod_itr != node_array.end (); nod_itr++) {
				if (nod_itr->Subarea () == 0) {
					dx = node_itr->X () - nod_itr->X ();
					dy = node_itr->Y () - nod_itr->Y ();

					dx = sqrt (dx * dx + dy * dy);

					if (dx <= buffer) {
						nod_itr->Subarea (2);
					}
				}
			}
		}
	}

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () == 2) {
			node_itr->Subarea (1);
		}
	}
}
