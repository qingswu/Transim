//*********************************************************
//	Function.cpp - array of functions
//*********************************************************

#include "Functions.hpp"

#include "Execution_Service.hpp"
#include "TypeDefs.hpp"

#include <math.h>

//---------------------------------------------------------
//	Add_Function
//---------------------------------------------------------

bool Function_Array::Add_Function (int number, string &parameters, bool print_flag)
{
	Function fun_rec;
	Function_Stat fun_stat;
		
	memset (&fun_rec, '\0', sizeof (fun_rec));

	if (!parameters.empty ()) {
		int num;
		Strings elements;

		num = String_Ptr (parameters)->Parse (elements);

		if (num > 0) fun_rec.type = Function_Code (elements [0]);
		if (num > 1) fun_rec.a = elements [1].Double ();
		if (num > 2) fun_rec.b = elements [2].Double ();
		if (num > 3) fun_rec.c = elements [3].Double ();
		if (num > 4) fun_rec.d = elements [4].Double ();

		if (fun_rec.type == LINEAR) {
			if (fun_rec.c != 0.0 || fun_rec.d != 0.0) {
				if (exe->Send_Messages ()) {
					exe->Warning ("Linear Function Parameters C and D are Ignored");
				}
			}
		} else if (fun_rec.type == EXPONENTIAL) {
			if (fun_rec.d != 0.0) {
				if (exe->Send_Messages ()) {
					exe->Warning ("Exponential Function Parameter D is Ignored");
				}
			}
		} else if (fun_rec.type == LOGARITHMIC) {
			if (fun_rec.d != 0.0) {
				if (exe->Send_Messages ()) {
					exe->Warning ("Logarithmic Function Parameter D is Ignored");
				}
			}
		} else if (fun_rec.type == POWER) {
			if (fun_rec.d != 0.0) {
				if (exe->Send_Messages ()) {
					exe->Warning ("Power Function Parameter D is Ignored");
				}
			}
		}
	} else if (number == 1) {

		fun_rec.type = LINEAR;
		fun_rec.a = 1.0;
		fun_rec.b = 0.0;

	} else {
		Function_Itr fun_itr;
		
		fun_itr = functions.find (number-1);
		if (fun_itr == functions.end ()) return (false);

		fun_rec = fun_itr->second;
	}
	fun_stat = functions.insert (Function_Data (number, fun_rec));

	if (!fun_stat.second) {
		if (exe->Send_Messages ()) {
			exe->Print (1, "Duplicate Function Number = ") << number;
		}
		return (false);
	}
	if (print_flag) {
		Print_Function (number);
	}
	return (true);
}

bool Function_Array::Add_Function (int number, Function &fun_rec, bool print_flag)
{
	Function_Stat fun_stat;

	fun_stat = functions.insert (Function_Data (number, fun_rec));

	if (!fun_stat.second) {
		if (exe->Send_Messages ()) {
			exe->Print (1, "Duplicate Function Number = ") << number;
		}
		return (false);
	}
	if (print_flag) {
		Print_Function (number);
	}
	return (true);
}

//---------------------------------------------------------
//	Update_Function
//---------------------------------------------------------

bool Function_Array::Update_Function (int number, Function &fun_rec)
{
	Function_Itr fun_itr;
	Function *fun_ptr;

	fun_itr = functions.find (number);
	if (fun_itr == functions.end ()) return (false);

	fun_ptr = &(fun_itr->second);

	memcpy (fun_ptr, &fun_rec, sizeof (fun_rec));

	return (true);
}

//---------------------------------------------------------
//	Get_Function
//---------------------------------------------------------

Function Function_Array::Get_Function (int number)
{
	Function_Itr fun_itr;

	fun_itr = functions.find (number);
	if (fun_itr == functions.end ()) {
		Function fun_rec;
		memset (&fun_rec, '\0', sizeof (fun_rec));
		return (fun_rec);
	}
	return (fun_itr->second);
}

//---------------------------------------------------------
//	Apply_Function
//---------------------------------------------------------

double Function_Array::Apply_Function (int number, double value)
{
	Function *fun_ptr;
	Function_Itr fun_itr;
		
	fun_itr = functions.find (number);
	if (fun_itr == functions.end ()) return (0.0);

	fun_ptr = &(fun_itr->second);

	switch (fun_ptr->type) {
		default:
		case LINEAR:
			value = value * fun_ptr->a + fun_ptr->b;
			break;
		case LOGIT:
			value =	exp (fun_ptr->a + fun_ptr->b * value + fun_ptr->c * (value * value) + fun_ptr->d * (value * value * value));
			break;
		case EXPONENTIAL:
			value = fun_ptr->a * exp (value * fun_ptr->b) + fun_ptr->c;
			break;
		case LOGARITHMIC:
			value = fun_ptr->a * log (value * fun_ptr->b) + fun_ptr->c;
			break;
		case POWER:
			value = fun_ptr->a * pow (value, fun_ptr->b) + fun_ptr->c;
			break;
		case POLYNOMIAL:
			value = fun_ptr->a + fun_ptr->b * value + fun_ptr->c * (value * value) + fun_ptr->d * (value * value * value);
			break;
		case GAMMA:
			value = fun_ptr->a * (pow (value, fun_ptr->b) + exp (value * fun_ptr->c)) + fun_ptr->d;
			break;
		case MAX_LOGIT:
			value =	exp (fun_ptr->b * MAX ((fun_ptr->a - value), 0) + fun_ptr->c * value + fun_ptr->d * (value * value));
			break;
	}
	return (value);
}

//---------------------------------------------------------
//	Print_Function
//---------------------------------------------------------

void Function_Array::Print_Function (int number)
{
	Function *fun_ptr;
	Function_Itr fun_itr;
		
	fun_itr = functions.find (number);
	if (fun_itr == functions.end ()) return;

	fun_ptr = &(fun_itr->second);

	exe->Print (1, String ("Function Parameters %d = %s, A = %0.5lf, B = %0.5lf") % number % Function_Code (fun_ptr->type) % fun_ptr->a % fun_ptr->b);

	if (fun_ptr->type != LINEAR) {
		exe->Print (0, String (", C = %.5lf") % fun_ptr->c);
	}
	if ((fun_ptr->type == LOGIT || fun_ptr->type == POLYNOMIAL || fun_ptr->type == GAMMA || fun_ptr->type == MAX_LOGIT) && fun_ptr->d != 0.0) {
		exe->Print (0, String (", D = %.5lf") % fun_ptr->d);
	}
}
