//*********************************************************
//	Subtotal.cpp - gather subtotal data
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Subtotal
//---------------------------------------------------------

void Validate::Subtotal (Group_Data &subtot, Group_Data &data)
{
	//---- add to the subtotal ----

	subtot.number += data.number;

	subtot.volume += data.volume;
	subtot.volume_sq += data.volume_sq;

	subtot.count += data.count;
	subtot.count_sq += data.count_sq;

	subtot.error += data.error;
	subtot.error_sq += data.error_sq;
		
	subtot.count_volume += data.count_volume;

	subtot.capacity += data.capacity;
	if (data.max_vc > subtot.max_vc) subtot.max_vc = data.max_vc;
}
