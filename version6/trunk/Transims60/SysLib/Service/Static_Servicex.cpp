
//---- facility type codes ----

Static_Service::Code_Text Static_Service::facility_codes [] = {
	{ FREEWAY, "FREEWAY" }, { EXPRESSWAY, "EXPRESSWAY" }, { PRINCIPAL, "PRINCIPAL" },
	{ MAJOR, "MAJOR" }, { MINOR, "MINOR" }, { COLLECTOR, "COLLECTOR" }, 
	{ LOCAL_THRU, "LOCAL_THRU" }, { LOCAL, "LOCAL" }, { FRONTAGE, "FRONTAGE" }, { RAMP, "RAMP" }, 
	{ BRIDGE, "BRIDGE" }, { TUNNEL, "TUNNEL" },	{ OTHER, "OTHER" }, { EXTERNAL, "EXTERNAL" }, 
	{ EXTERNAL, "ZONECONN" }, { LOCAL_THRU, "THRU" }, { LOCAL_THRU, "LOCAL_ACCESS" }, 
	{ EXPRESSWAY, "XPRESSWAY" }, { MAJOR, "PRIARTER" }, { MINOR, "SECARTER" }, 
	{ WALKWAY, "WALKWAY" }, { BIKEWAY, "BIKEWAY" },	{ BUSWAY, "BUSWAY" }, 
	{ LIGHTRAIL, "LIGHTRAIL" }, { HEAVYRAIL, "HEAVYRAIL" }, { FERRY, "FERRY" },
	{ 0, 0 }
};

//---- travel mode codes ----

Static_Service::Code_Text Static_Service::mode_codes [] = {
	{ WAIT_MODE, "WAIT" }, { WALK_MODE, "WALK" }, { BIKE_MODE, "BIKE" }, { DRIVE_MODE, "DRIVE" }, 
	{ RIDE_MODE, "RIDE" }, { TRANSIT_MODE, "TRANSIT" }, { PNR_OUT_MODE,"PNR_OUT" },
	{ PNR_IN_MODE, "PNR_IN" }, { KNR_OUT_MODE, "KNR_OUT" }, { KNR_IN_MODE,"KNR_IN" }, 
	{ TAXI_MODE, "TAXI" }, { OTHER_MODE, "OTHER" }, { HOV2_MODE, "HOV2" }, { HOV3_MODE, "HOV3" }, 
	{ HOV4_MODE, "HOV4" }, { WAIT_MODE, "NONE" }, { WALK_MODE, "ALL_WALK" }, { BIKE_MODE, "BICYCLE" }, 
	{ DRIVE_MODE, "DRIVE_ALONE" }, { RIDE_MODE, "PASSENGER" }, { TRANSIT_MODE, "BUS" }, 
	{ PNR_OUT_MODE, "PARKRIDE" }, { PNR_IN_MODE, "RIDEPARK" }, { KNR_OUT_MODE, "KISSRIDE" }, 
	{ KNR_IN_MODE, "RIDEKISS" }, { TAXI_MODE, "PICKUP" }, { OTHER_MODE, "MAGIC_MOVE" }, 
	{ HOV2_MODE, "CARPOOL2" }, { HOV3_MODE, "CARPOOL3" }, { HOV4_MODE,"CARPOOL4" }, 
	{ DRIVE_MODE, "AUTO_DRIVER" }, { RIDE_MODE, "AUTO_PASS" }, { TRANSIT_MODE, "RAIL" }, 
	{ OTHER_MODE, "SCHOOL_BUS" }, { HOV4_MODE, "HOV4+" }, { 0, 0 }
};

//---- transit mode codes ----

Static_Service::Code_Text Static_Service::transit_codes [] = {
	{ NO_TRANSIT, "NONE" }, { NO_TRANSIT, "NO_MODE" }, { NO_TRANSIT, "N/A" }, { NO_TRANSIT, "" },
	{ LOCAL_BUS, "BUS" }, { LOCAL_BUS, "LOCAL_BUS" }, { EXPRESS_BUS, "EXPRESS" }, 
	{ EXPRESS_BUS, "EXPRESS_BUS" }, { TROLLEY, "TROLLEY" }, { STREETCAR, "STREETCAR" }, 
	{ LRT, "LIGHTRAIL" }, { LRT, "LRT" }, { RAPIDRAIL, "RAPIDRAIL" }, { RAPIDRAIL, "METRORAIL" },
	{ REGIONRAIL, "REGIONRAIL" }, { REGIONALRAIL, "AMTRAK" }, { ANY_TRANSIT, "ANY_MODE" }, { ANY_TRANSIT, "ANY" }, 
	{ 0, 0 }
};

//---- use restriction codes ----

Static_Service::Code_Text  Static_Service::use_codes [] = {
	{ ANY, "ANY" }, { WALK, "WALK" }, { BIKE, "BICYCLE" }, { CAR, "AUTO" }, { TRUCK, "TRUCK" }, 
	{ BUS, "BUS" }, { RAIL, "RAIL" }, { SOV, "SOV" }, { HOV2, "HOV2" }, { HOV3, "HOV3" }, 
	{ HOV4, "HOV4" }, { LIGHTTRUCK, "LIGHTTRUCK" }, { HEAVYTRUCK, "HEAVYTRUCK" }, { RESTRICTED, "RESTRICTED" }, 
	{ NONE, "NONE" }, { CAR, "CAR" }, { BIKE, "BIKE" }, { TAXI, "TAXI" }, { BUS, "LOCAL" }, { BUS, "EXPRESS" },
	{ BUS, "LOCAL_BUS" }, { BUS, "EXPRESS_BUS" }, { RAIL, "TROLLEY" }, { RAIL, "STREETCAR" }, 
	{ RAIL, "LIGHTRAIL" }, { RAIL, "LRT" }, { RAIL, "RAPIDRAIL" }, { RAIL, "METRORAIL"}, { RAIL, "REGIONRAIL" }, 
	{ RAIL, "AMTRAK" }, { 0, 0 }
};
