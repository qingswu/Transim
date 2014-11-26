//*********************************************************
//	Equation.cpp - volume-delay equation array
//*********************************************************

#include "Equation.hpp"

#include "Execution_Service.hpp"
#include "TypeDefs.hpp"

#include <math.h>

//---------------------------------------------------------
//	Add_Equation
//---------------------------------------------------------

bool Equation_Array::Add_Equation (int number, string &parameters)
{
	bool flag = true;
	Equation eq_rec;
	Equation_Stat eq_stat;

	if (!parameters.empty ()) {
		int num;
		Strings elements;

		num = String_Ptr (parameters)->Parse (elements);

		memset (&eq_rec, '\0', sizeof (eq_rec));

		if (num > 0) eq_rec.type = Equation_Code (elements [0]);
		if (num > 1) eq_rec.a = elements [1].Double ();
		if (num > 2) eq_rec.b = elements [2].Double ();
		if (num > 3) eq_rec.c = elements [3].Double ();
		if (num > 4) eq_rec.d = elements [4].Double ();

		if (eq_rec.type == BPR || eq_rec.type == BPR_PLUS) {
			if (eq_rec.a < 0.01 || eq_rec.b < 1.0 || eq_rec.c < 0.5) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("BPR Equation Parameters %.2lf, %.2lf, %.2lf are Out of Range") % eq_rec.a % eq_rec.b % eq_rec.c);
				}
				return (false);
			}
			if (eq_rec.type == BPR_PLUS) {
				if (eq_rec.d <= 0.0) eq_rec.d = 1.0;
				if (eq_rec.d > 7.0) {
					if (exe->Send_Messages ()) {
					exe->Error (String ("BPR Plus Equation Minimum Speed %.2lf is Out of Range") % eq_rec.d);
					}
					return (false);
				}
			}
		} else if (eq_rec.type == EXP) {
			if (eq_rec.a < 0.07 || eq_rec.a > 7.5 || eq_rec.b < 1.0 || eq_rec.b > 20.0 ||
				eq_rec.c < 30.0 || eq_rec.c > 750.0) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("Exponential Equation Parameters %.2lf, %.2lf, %.1lf are Out of Range") % eq_rec.a % eq_rec.b % eq_rec.c);
				}
			}
		} else if (eq_rec.type == CONICAL) {
			if (eq_rec.a <= 1.0 || eq_rec.a > 20.0) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("Conical Equation Parameter %.2lf is Out of Range (1..20)") % eq_rec.a);
				}
				return (false);
			}
			eq_rec.c = (2 * eq_rec.a - 1) / (2 * eq_rec.a - 2);
		} else if (eq_rec.type == AKCELIK) {
			if (eq_rec.a <= 0.05 || eq_rec.a > 2.5) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("AKCELIK Equation Parameter %.2lf is Out of Range (0.05..2.5)") % eq_rec.a);
				}
				return (false);
			}
		} else if (eq_rec.type == FLOW_DENSITY) {
			if (eq_rec.a < 0.7 || eq_rec.a > 1.3 || eq_rec.b < 0.7 || eq_rec.b > 1.3 || eq_rec.c < 1.5 || eq_rec.c > 2.2) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("FLOW_DENSITY Equation Parameters %.2lf, %.2lf, %.2lf are Out of Range") % eq_rec.a % eq_rec.b % eq_rec.c);
				}
				return (false);
			}
		} else if (eq_rec.type == CONSTANT) {
			if (eq_rec.a == 0.0) eq_rec.a = 1.0;
			if (eq_rec.a <= 0.95 || eq_rec.a > 1.2 || eq_rec.b < -5.0 || eq_rec.b > 5.0) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("CONSTANT Equation Parameters %.2lf, %.2lf are Out of Range") % eq_rec.a % eq_rec.b);
				}
				return (false);
			}
		}
	} else if (number == 1) {

		eq_rec.type = BPR;
		eq_rec.a = 0.15;
		eq_rec.b = 4.0;
		eq_rec.c = 0.75;
		eq_rec.d = 0.0;

	} else {
		Equation_Itr eq_itr;
		
		eq_itr = equations.find (number-1);
		if (eq_itr == equations.end ()) return (false);

		eq_rec = eq_itr->second;
		flag = false;
	}

	if (flag && exe->Send_Messages ()) {
		exe->Print (1, String ("Equation Parameters %d = %s, A=%0.2lf") % number % Equation_Code (eq_rec.type) % eq_rec.a);

		if (eq_rec.type == CONSTANT || eq_rec.type == CONICAL) {
			if (eq_rec.b != 0.0) {
				exe->Print (0, String (", B=%.2lf") % eq_rec.b);
			}
		} else if (eq_rec.type != AKCELIK) {
			exe->Print (0, String (", B=%0.2lf, C=%.2lf") % eq_rec.b % eq_rec.c);
		}
		if (eq_rec.type == BPR_PLUS) {
			exe->Print (0, String (", D=%.2lf") % eq_rec.d);
		}
	}
	eq_stat = equations.insert (Equation_Data (number, eq_rec));
	return (eq_stat.second);
}

//---------------------------------------------------------
//	Apply_Equation
//---------------------------------------------------------

int Equation_Array::Apply_Equation (int number, int time0, double volume, int capacity, int length)
{
	if (time0 < 1) time0 = 1;

	if (volume <= 0) {
		return (time0);
	} else {
		double cap, len, ttime;
		Equation *eq_ptr;
		Equation_Itr eq_itr;
		
		eq_itr = equations.find (number);
		if (eq_itr == equations.end ()) return (time0);

		eq_ptr = &(eq_itr->second);

		if (capacity < 1) capacity = 1;
		cap = capacity;

		if (eq_ptr->type == BPR || eq_ptr->type == BPR_PLUS) {
			cap *= eq_ptr->c;
			if (cap < 1.0) cap = 1.0;

			ttime = time0 * (1.0 + eq_ptr->a * pow ((volume / cap), eq_ptr->b));

			if (eq_ptr->type == BPR_PLUS) {
				int max_time = (int) (length / eq_ptr->d + 0.5);
				if (ttime > max_time) ttime = max_time;
			}
		} else if (eq_ptr->type == EXP) {
			if (exe->Metric_Flag ()) {
				len = length / 1000.0;
			} else {
				len = length / 5280.0;
			}
			ttime = time0 + len * MIN (eq_ptr->a * exp (eq_ptr->b * (volume / cap)), eq_ptr->c);

		} else if (eq_ptr->type == CONICAL) {
			cap = 1 - (volume / cap);

			ttime = time0 * (2 - eq_ptr->c - eq_ptr->a * cap + 
				sqrt (eq_ptr->a * eq_ptr->a * cap * cap + eq_ptr->c * eq_ptr->c));

			if (eq_ptr->b > 0) {
				cap = ttime / time0;
				if (cap > eq_ptr->b) {
					ttime = time0 * eq_ptr->b;
				}
			}
		} else if (eq_ptr->type == AKCELIK && length > 0) {
			double vc, fac, spd;
			vc = volume / cap;
			fac = vc - 1;
			
			if (exe->Metric_Flag ()) {
				spd = ((double) time0 / length) / MPSTOMPH;
			} else {
				spd = ((double) time0 / length) * MPHTOFPS;
			}
			ttime = time0 + ((spd + 0.25 * 0.25 * (fac + sqrt ((fac * fac) + (8 * eq_ptr->a * vc)/cap)))) / spd;
		} else if (eq_ptr->type == FLOW_DENSITY && length > 0) {
			double density = volume / length;
			double ff_density = eq_ptr->b * cap * time0 / length;
			if (density <= ff_density) {
				ttime = time0;
			} else {
				double flow = cap * eq_ptr->a - pow ((density - ff_density), eq_ptr->c);
				double speed = flow / density;
				if (speed < 0.1) speed = 0.1;
				ttime = length / speed;
			}
		} else if (eq_ptr->type == CONSTANT) {
			ttime = time0 * eq_ptr->a + eq_ptr->b;
		} else {
			return (time0);
		}
		if (ttime > time0) {
			if (length / ttime < 0.1) {
				ttime = length / 0.1;
			}
			time0 = (int) (ttime + 0.5);
			if (time0 < 1) time0 = 1;
		}
		return (time0);
	}
}
