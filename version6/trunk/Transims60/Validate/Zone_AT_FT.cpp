//*********************************************************
//	Zone_AT_FT.cpp - Zone-Area Type-Facility Type Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Zone_AT_FT_Stats
//---------------------------------------------------------

void Validate::Zone_AT_FT_Stats (void)
{
	int n, type, zone, at, ft, last_at, last_zone;
	String label;

	Group_Data *data, at_tot, zone_tot;
	Group_Map_Itr map_itr;
	Statistics stats;
	Link_Itr link_itr;
	Str_Map_Itr str_itr;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		if (link_itr->AB_Dir () >= 0) {
			n = node_zone [link_itr->Anode ()];
			type = zone_equiv.Zone_Group (n) * 1000 + link_itr->Area_Type () * 25 + link_itr->Type ();

			Sum_Group (type, &(*link_itr), link_itr->AB_Dir ());
		}
		if (link_itr->BA_Dir () >= 0) {
			n = node_zone [link_itr->Bnode ()];
			type = zone_equiv.Zone_Group (n) * 1000 + link_itr->Area_Type () * 25 + link_itr->Type ();

			Sum_Group (type, &(*link_itr), link_itr->BA_Dir ());
		}
	}

	//---- print the page header ----
	
	Header_Number (ZONE_AT_FT);

	if (!Break_Check ((int) group_data.size () + 9)) {
		Print (1);
		Zone_Group_Header ();
	}

	memset (&at_tot, '\0', sizeof (at_tot));
	memset (&zone_tot, '\0', sizeof (zone_tot));

	last_at = last_zone = -1;

	//---- print each zone group ----

	for (map_itr = group_data.begin (); map_itr != group_data.end (); map_itr++) {
		data = &map_itr->second;
		n = data->number;
		if (n == 0) continue;

		zone = data->index / 1000;
		at = (data->index - zone * 1000) / 25;
		ft = data->index % 25;

		//---- print the area type subtotal ----

		if ((zone != last_zone && last_zone > 0) || (at != last_at && last_at > 0)) {
			Calc_Stats (&at_tot, stats);

			label ("Group=%d AT=%d FT=Total") % last_zone % last_at;

			Page_Check (3);
			Print (0, "\n");
			Print (0, String (stats_format) % label % at_tot.number % at_tot.volume % at_tot.count % 
				(at_tot.volume - at_tot.count) % stats.per_diff % stats.avg_error % stats.per_error % 
				stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % at_tot.max_vc);
			Print (0, "\n");

			memset (&at_tot, '\0', sizeof (at_tot));
			last_at = at;
		}
		Subtotal (at_tot, *data);

		//---- print the zone group subtotal ----

		if (zone != last_zone && last_zone > 0) {
			Calc_Stats (&zone_tot, stats);

			label ("Group=%d AT=Total FT=Total") % last_zone;

			Page_Check (3);
			Print (0, "\n");
			Print (0, String (stats_format) % label % zone_tot.number % zone_tot.volume % zone_tot.count % 
				(zone_tot.volume - zone_tot.count) % stats.per_diff % stats.avg_error % stats.per_error % 
				stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % zone_tot.max_vc);
			Print (0, "\n");

			memset (&zone_tot, '\0', sizeof (zone_tot));
			last_zone = zone;
		}
		Subtotal (zone_tot, *data);

		//---- print the facility type statistics ----

		Calc_Stats (data, stats);

		//label = zone_equiv.Group_Label (i);
		//if (label.empty ()) {
		//	label = String ("Zone %d") % i;
		//}
		label ("Group=%d AT=%d FT=%d") % zone % at % ft;

		Page_Check (1);
		Print (0, String (stats_format) % label % n % data->volume % data->count % 
			(data->volume - data->count) % stats.per_diff % stats.avg_error % stats.per_error % 
			stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % data->max_vc);
	}

	//---- print the area type subtotal ----

	if (last_zone > 0 && last_at > 0 && at_tot.number > 0) {
		Calc_Stats (&at_tot, stats);

		label ("Group=%d AT=%d FT=Total") % last_zone % last_at;

		Page_Check (3);
		Print (0, "\n");
		Print (0, String (stats_format) % label % at_tot.number % at_tot.volume % at_tot.count % 
			(at_tot.volume - at_tot.count) % stats.per_diff % stats.avg_error % stats.per_error % 
			stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % at_tot.max_vc);
		Print (0, "\n");
	}

	//---- print the zone group subtotal ----

	if (last_zone > 0 && zone_tot.number > 0) {
		Calc_Stats (&zone_tot, stats);

		label ("Group=%d AT=Total FT=Total") % last_zone;

		Page_Check (3);
		Print (0, "\n");
		Print (0, String (stats_format) % label % zone_tot.number % zone_tot.volume % zone_tot.count % 
			(zone_tot.volume - zone_tot.count) % stats.per_diff % stats.avg_error % stats.per_error % 
			stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % zone_tot.max_vc);
		Print (0, "\n");
	}

	//---- calculate the total statistics ----

	n = total.number;
	if (n > 0) {
		Calc_Stats (&total, stats);
		
		Page_Check (3);
		Print (0, "\n");
		Print (0, String (stats_format) % "TOTAL" % n % total.volume % total.count % 
			(total.volume - total.count) % stats.per_diff % stats.avg_error % stats.per_error % 
			stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % total.max_vc);
		Print (0, "\n");
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Zone_AT_FT_Header
//---------------------------------------------------------

void Validate::Zone_AT_FT_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%32cSummary Statistics by Zone Group, Area Type and Facility Type\n") % BLANK);
	Print (0, header1);
	Print (0, String ("\n         Zone Group-AT-FT") + header2);
}

/**********************************************************|**********************************************************

                                Summary Statistics by Zone Group, Area Type and Facility Type
									
                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
         Zone Group-AT-FT    Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

sssssssssssssssssssssssss ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

                    TOTAL ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
    

***********************************************************|**********************************************************/ 
