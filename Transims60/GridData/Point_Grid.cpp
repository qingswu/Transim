//*********************************************************
//	Point_Grid.cpp - allocate points to a grid
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Point_Grid
//---------------------------------------------------------

void GridData::Point_Grid (double x0, double y0, double x1, double y1, double x2, double y2)
{
	int i, point_id, num_sum;
	double dx, dy, x, y, share, dist, max_dist, all_dist;
	bool flag;

	Doubles sum_data;

	flag = false;
	max_dist = max_distance + grid_size * 0.7;
	all_dist = grid_size * 0.45;
	num_sum = point_data.Num_Fields () - 2;
	sum_data.assign (num_sum, 0.0);

	point_data.Rewind ();

	while (point_data.Read_Record ()) {
		point_id = point_data.Get_Integer (0);
		x = point_data.Get_Double (1);
		y = point_data.Get_Double (2);

		dx = x0 - x;
		dy = y0 - y;

		dist = sqrt (dx * dx + dy * dy);
		if (dist > max_dist) continue;

		if (dist > all_dist) {
			share = 0.0;	
			if (dist <= max_distance) share += 0.2;
	
			dx = x1 - x;
			dy = y1 - y;
					
			dist = sqrt (dx * dx + dy * dy);
			if (dist <= max_distance) share += 0.2;

			dx = x1 - x;
			dy = y2 - y;
					
			dist = sqrt (dx * dx + dy * dy);
			if (dist <= max_distance) share += 0.2;

			dx = x2 - x;
			dy = y1 - y;
					
			dist = sqrt (dx * dx + dy * dy);
			if (dist <= max_distance) share += 0.2;

			dx = x2 - x;
			dy = y2 - y;
					
			dist = sqrt (dx * dx + dy * dy);
			if (dist <= max_distance) share += 0.2;
		} else {
			share = 1.0;
		}
		if (share == 0.0) continue;
		flag = true;

		//---- sum the share data ----

		sum_data [0] += share;

		for (i=1; i < num_sum; i++) {
			sum_data [i] += point_data.Get_Double (i + 2) * share;
		}
	}
	if (flag) {
		for (i=0; i < num_sum; i++) {
			out_file.Put_Field (out_fields [i], sum_data [i]);
		}
	}
}

