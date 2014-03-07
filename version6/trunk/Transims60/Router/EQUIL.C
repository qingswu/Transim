#include "assign.h"

/**********************************************************************
	function EQUILIBRIUM calculates the equilibrium factors
**********************************************************************/

float equilibrium (void)
{
	extern unsigned short *base, *value, *capacity, *spd_limit;
	extern unsigned char *function;
	extern long *volume [MAX_RUN], *old_vol [MAX_RUN], *bas_vol [MAX_RUN];

	extern int trip_factor, cap_flag, max_run, max_bas, limit_flag;
	extern unsigned nlink;
	extern float **delay;
	extern short *equation_type, *vol_factor;

	int i, vc1, vc2, pass;
	unsigned rec;
	long *b [MAX_RUN], *vol [MAX_RUN], *old [MAX_RUN], sum_vol, sum_old, base_vol;
	unsigned short *bas, *val, *cap, *lmt;
	unsigned char *eq;
	double volfac, oldfac, fac1, fac2, tot1, tot2, total, new_vol, vc;

	pass = 1;
	tot1 = tot2 = 0.0;
	volfac = 0.0;
	fac1 = 0.0;
	fac2 = 1.0;
	
	do {
		oldfac = 1.0 - volfac;	
		total = 0.0;

		cap = capacity;
		bas = base;
		val = value;
		eq = function;

		if (limit_flag) {
			lmt = spd_limit;
		}

		for (i=0; i < max_run; i++) {
			vol [i] = volume [i];
			old [i] = old_vol [i];
		}
		for (i=0; i < max_bas; i++) {
			b [i] = bas_vol [i];
		}
	
		for (rec=1; rec < nlink; rec++, cap++, bas++, val++, eq++) {
			if (limit_flag) {
				lmt++;
			}
			for (i=0; i < max_run; i++) {
				(vol [i])++;
				(old [i])++;
			}
			for (i=0; i < max_bas; i++) {
				(b [i])++;
			}
			base_vol = 0L;

			for (i=0; i < max_bas; i++) {
				base_vol += *(b [i]);
			}
			sum_old = sum_vol = 0L;

			for (i=0; i < max_run; i++) {
				sum_vol += *(vol [i]);
				sum_old += *(old [i]);
			}
			new_vol = oldfac * sum_old + volfac * sum_vol;
				
			if (trip_factor > 1) {
				new_vol = new_vol / (float) trip_factor;
				base_vol = base_vol / trip_factor;
			}
			total += *bas * new_vol;
				
			if (*cap && equation_type [*eq] < 3) {

				vc = ((new_vol + base_vol) * (float) vol_factor [*eq] / (float) *cap) / 10.0;
			
				if (vc > 0.0) {
					if (vc > MAX_VC) {
						vc = delay [*eq] [MAX_VC];
					} else {
						vc1 = vc;
						vc2 = vc1 + 1;
						
						vc = (vc - vc1) * (delay [*eq] [vc2] - delay [*eq] [vc1]) + delay [*eq] [vc1];
					
					}
					vc = (double) *val * vc;
					
					if (limit_flag) {
						if (limit_flag == 1) {
							if (vc < *lmt) vc = *lmt;
						} else {
							if (vc > *lmt) vc = *lmt;
						}
					}
					total += new_vol * vc;
				}
			}
		}
		if (cap_flag) {
			total += turn_equil (volfac);		
		}
			
		if (pass == 1) {
			tot1 = total;
			fac1 = volfac;
			volfac = oldfac;
		} else {
			if (pass == 2) {
				tot2 = total;
				fac2 = volfac;
			} else if (tot1 > tot2) {
				tot1 = total;
				fac1 = volfac;
			} else {
				tot2 = total;
				fac2 = volfac;
			}
			volfac = (fac1 + fac2) * 0.5;	
		}
		pass++;
		
	} while (fac2-fac1 >= 0.005);
	
	return ((float) volfac);
}
