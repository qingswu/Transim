//*********************************************************
//	Router_Service.cpp - path building service
//*********************************************************

#include "Router_Service.hpp"

//---------------------------------------------------------
//	Set_Parameters
//---------------------------------------------------------

void Router_Service::Set_Parameters (Path_Parameters &p, int type)
{
	if (type < 0) return;

	memcpy (&p, &param, sizeof (param));

	p.traveler_type = type;

	p.value_walk = value_walk.Best (type);
	p.value_bike = value_bike.Best (type);
	p.value_time = value_time.Best (type);
	p.value_wait = value_wait.Best (type);
	p.value_xfer = value_xfer.Best (type);
	p.value_park = value_park.Best (type);
	p.value_dist = value_dist.Best (type);
	p.value_cost = value_cost.Best (type);
	p.freeway_fac = freeway_fac.Best (type);
	p.express_fac = express_fac.Best (type);
	p.left_imped = left_imped.Best (type) * 10;
	p.right_imped = right_imped.Best (type) * 10;
	p.uturn_imped = uturn_imped.Best (type) * 10;
	p.xfer_imped = xfer_imped.Best (type) * 10;
	p.stop_imped = stop_imped.Best (type) * 10;
	p.station_imped = station_imped.Best (type) * 10;
	p.bus_bias = bus_bias.Best (type);
	p.bus_const = bus_const.Best (type) * 10;
	p.rail_bias = rail_bias.Best (type);
	p.rail_const = rail_const.Best (type) * 10;
	p.max_walk = Round (max_walk.Best (type));
	p.walk_pen = Round (walk_pen.Best (type));
	p.walk_fac = walk_fac.Best (type);
	p.max_bike = Round (max_bike.Best (type));
	p.bike_pen = Round (bike_pen.Best (type));
	p.bike_fac = bike_fac.Best (type);
	p.max_wait = max_wait.Best (type);
	p.wait_pen = wait_pen.Best (type);
	p.wait_fac = wait_fac.Best (type);
	p.min_wait = min_wait.Best (type);
	p.max_xfers = max_xfers.Best (type);
	p.max_parkride = max_parkride.Best (type);
	p.max_kissride = max_kissride.Best (type);
	p.kissride_fac = kissride_fac.Best (type);

	if (p.walk_pen == 0 || p.walk_fac == 0.0) p.walk_pen = p.max_walk;
	if (p.bike_pen == 0 || p.bike_fac == 0.0) p.bike_pen = p.max_bike;
	if (p.wait_pen == 0 || p.wait_fac == 0.0) p.wait_pen = p.max_wait;
}
