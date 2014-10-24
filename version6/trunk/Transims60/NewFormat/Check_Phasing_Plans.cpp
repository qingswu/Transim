//*********************************************************
//	Check_Phasing_Plans.cpp - check phasing plan data
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Check_Phasing_Plans
//---------------------------------------------------------

void NewFormat::Check_Phasing_Plans (void)
{
	int num_delete = 0;
	bool flag, found;

	int count1, count2;
	Signal_Itr signal_itr;
	Signal_Time_Itr time1_itr, time2_itr;
	Phasing_Itr plan1_itr, plan2_itr;
	Movement_Itr move1_itr, move2_itr;
	Int_Map phase_count;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;

	Show_Message ("Checking Phasing Plans -- Record");
	Set_Progress ();

	for (signal_itr = signal_array.begin (); signal_itr != signal_array.end (); signal_itr++) {
		Show_Progress ();

		phase_count.clear ();

		for (plan1_itr = signal_itr->phasing_plan.begin (); plan1_itr != signal_itr->phasing_plan.end (); plan1_itr++) {
			map_stat = phase_count.insert (Int_Map_Data (plan1_itr->Phasing (), 1));
			if (!map_stat.second) {
				map_stat.first->second++;
			}
		}

		for (time1_itr = signal_itr->begin (); time1_itr != signal_itr->end (); time1_itr++) {
			map_itr = phase_count.find (time1_itr->Phasing ());
			if (map_itr != phase_count.end ()) {
				count1 = map_itr->second;
			} else {
				count1 = 0;
			}

			for (time2_itr = time1_itr + 1; time2_itr != signal_itr->end (); time2_itr++) {
				if (time2_itr->Phasing () != time1_itr->Phasing ()) {
					map_itr = phase_count.find (time2_itr->Phasing ());
					if (map_itr != phase_count.end ()) {
						count2 = map_itr->second;
					} else {
						count2 = 0;
					}
					if (count1 != count2) continue;

					flag = false;

					for (plan1_itr = signal_itr->phasing_plan.begin (); plan1_itr != signal_itr->phasing_plan.end (); plan1_itr++) {
						if (plan1_itr->Phasing () == time1_itr->Phasing ()) {
							found = false;

							for (plan2_itr = signal_itr->phasing_plan.begin (); plan2_itr != signal_itr->phasing_plan.end (); plan2_itr++) {
								if (plan2_itr->Phasing () == time2_itr->Phasing () && plan2_itr->Phase () == plan1_itr->Phase ()) {
									found = true;

									if (plan1_itr->size () != plan2_itr->size ()) {
										flag = true;
										break;
									}
									for (move1_itr = plan1_itr->begin (); move1_itr != plan1_itr->end (); move1_itr++) {
										for (move2_itr = plan2_itr->begin (); move2_itr != plan2_itr->end (); move2_itr++) {
											if (move1_itr->Connection () == move2_itr->Connection () &&
												move1_itr->Protection () == move2_itr->Protection ()) break;
										}
										if (move2_itr == plan2_itr->end ()) {
											flag = true;
											break;
										}
									}
									if (flag) break;
								}
							}
							if (!found) {
								flag = true;
							}
							if (flag) break;
						}
					}
					if (flag) break;

					//---- delete the phasing plan ----

					for (plan2_itr = signal_itr->phasing_plan.begin (); plan2_itr != signal_itr->phasing_plan.end (); ) {
						if (plan2_itr->Phasing () == time2_itr->Phasing ()) {
							plan2_itr = signal_itr->phasing_plan.erase (plan2_itr);
						} else {
							plan2_itr++;
						}
					}
					time2_itr->Phasing (time1_itr->Phasing ());
					num_delete++;
				}
			}
		}
	}
	End_Progress ();

	if (num_delete > 0) {
		Print (2, "Number of Deleted Phasing Plans = ") << num_delete;
	}
}
