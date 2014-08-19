//*********************************************************
//	Repair_Legs.cpp - repair plan legs
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Repair_Legs
//---------------------------------------------------------

int PlanPrep::Repair_Legs (Plan_Ptr plan_ptr)
{
	int id, count;
	Plan_Leg_Itr leg_itr;
	Int_Map_Itr int_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	count = 0;

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		if (leg_itr->Link_Type ()) {
			if (leg_itr->Time () < 1) {
				id = leg_itr->Link_ID ();
				int_itr = link_map.find (id);

				if (int_itr != link_map.end ()) {
					link_ptr = &link_array [int_itr->second];

					if (leg_itr->Link_Dir ()) {
						id = link_ptr->BA_Dir ();
					} else {
						id = link_ptr->AB_Dir ();
					}
					if (id >= 0) {
						dir_ptr = &dir_array [id];

						if (leg_itr->Length () < link_ptr->Length ()) {
							if (link_ptr->Length () > 0) {
								leg_itr->Time (dir_ptr->Time0 () * leg_itr->Length () / link_ptr->Length () + 1);
							}
						} else {
							leg_itr->Time (dir_ptr->Time0 ());
						}
					}
				}
				if (leg_itr->Time () < 1) {
					leg_itr->Time (1);
				}
				count++;
			}
		} else if (leg_itr->Time () < 0) {
			leg_itr->Time (0);
			count++;
		}
	}
	if (count > 0) plan_ptr->Priority (CRITICAL);
	return (count);
}
