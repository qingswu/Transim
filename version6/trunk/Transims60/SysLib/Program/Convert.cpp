//********************************************************* 
//	Convert.cpp - Type Conversion Functions
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Convert
//---------------------------------------------------------

bool User_Program::Convert (void)
{
	String *str_ptr;
	Dtime time;
	
	//--- sub string function ----
	
	if (cmd_ptr->token == SUBSTR) {
		int i1, i2, len;

		if (sindex <= 2) goto string_error;

		Stack_Data *s2 = &stack [sindex - 2];

		if (s2->type != STRING_DATA || s1->type != INT_DATA || 
			s->type != INT_DATA) goto string_error;
	
		i1 = s1->lvalue;
		i2 = s->lvalue;
		str_ptr = &svalue [0];
		*str_ptr = *(s2->str_ptr);
		len = (int) str_ptr->length ();

		if (i1 < 0) i1 = 0;
		if (i2 < i1) i2 = i1;
		if (i2 > len) {
			i2 = len;
			if (i1 > i2) i1 = i2;
		}
		len = i2 - i1 + 1;
		(*str_ptr) = str_ptr->substr (i1, len);

		sindex -= 2;
		s = s2;
		s->str_ptr = str_ptr;
		return (true);
	} else if (cmd_ptr->token == TRIM) {
		if (s->type != STRING_DATA) {
			exe->Error ("Illegal TRIM Function Syntax");
			return (false);
		}
		str_ptr = &svalue [0];
		*str_ptr = *(s->str_ptr);
		str_ptr->Trim ();
		s->str_ptr = str_ptr;
		return (true);
	} else if (cmd_ptr->token == LOW || cmd_ptr->token == HIGH) {
		if (s->type != STRING_DATA) {
			exe->Error ("Illegal Range Function Syntax");
			return (false);
		}
		String low, high;
		s->str_ptr->Range (low, high);

		s->str_ptr = &svalue [0];
		if (cmd_ptr->token == LOW) {
			*(s->str_ptr) = low;
		} else {
			*(s->str_ptr) = high;
		}
		return (true);
	} else if (cmd_ptr->token == RANGE) {
		str_ptr = &svalue [0];
		(*str_ptr)("%lf") % s->fvalue;

		if (s->type == INT_DATA && s1->type == INT_DATA) {
			(*str_ptr)("%d..%d") % s1->lvalue % s->lvalue;
		} else if (s->type == FLOAT_DATA && s1->type == FLOAT_DATA) {
			(*str_ptr)("%lf..%lf") % s1->fvalue % s->fvalue;
		} else if (s->type == STRING_DATA && s1->type == STRING_DATA) {
			(*str_ptr)("%s..%s") % *(s1->str_ptr) % *(s->str_ptr);
		} else {
			exe->Error ("Range Data Values are Incompatible");
			return (false);
		}
		s = &stack [--sindex];
		s->type = STRING_DATA;
		s->str_ptr = str_ptr;
		return (true);
	}

	//---- type conversion functions ----

	if (s->type == STRUCT_DATA) goto error;

	if (cmd_ptr->token != ATOI && cmd_ptr->token != ATOF && cmd_ptr->token != ATOT) {
		if (s->type == STRING_DATA) goto error;
	} else {
		if (s->type != STRING_DATA) goto error;
	}
							
	switch (cmd_ptr->token) {
	
		case FINT:
			if (s->type == FLOAT_DATA) {
				s->type = INT_DATA;
				s->lvalue = (int) s->fvalue;
			}
			break;
			
		case FFLOAT:
			if (s->type == INT_DATA) {
				s->type = FLOAT_DATA;
				s->fvalue = (double) s->lvalue;
			}
			break;
			
		case ROUND:
			if (s->type == FLOAT_DATA) {
				s->type = INT_DATA;
				s->lvalue = (int) (s->fvalue + 0.5);
			}
			break;	

		case ATOI:
			s->type = INT_DATA;
			s->lvalue = s->str_ptr->Integer ();
			break;
			
		case ATOF:
			s->type = FLOAT_DATA;
			s->fvalue = s->str_ptr->Double ();
			break;
			
		case ITOA:
			if (s->type == FLOAT_DATA) {
				s->lvalue = (int) s->fvalue;
			}
			str_ptr = &svalue [0];
			(*str_ptr)("%d") % s->lvalue;

			s->type = STRING_DATA;
			s->str_ptr = str_ptr;
			break;
			
		case FTOA:
			if (s->type == INT_DATA) {
				s->fvalue = s->lvalue;
			}
			str_ptr = &svalue [0];
			(*str_ptr)("%lf") % s->fvalue;

			s->type = STRING_DATA;
			s->str_ptr = str_ptr;
			break;

		case TTOI:
			if (s->type == FLOAT_DATA) {
				s->lvalue = (int) s->fvalue;
			}
			s->type = INT_DATA;
			s->lvalue /= 10;
			break;
			
		case TTOF:
			if (s->type == INT_DATA) {
				s->fvalue = s->lvalue;
			}
			s->type = FLOAT_DATA;
			s->fvalue /= 10.0;
			break;
			
		case ITOT:
			if (s->type == FLOAT_DATA) {
				s->lvalue = (int) s->fvalue;
			}
			s->type = INT_DATA;
			s->lvalue *= 10;
			break;
			
		case FTOT:
			if (s->type == INT_DATA) {
				s->fvalue = s->lvalue;
			}
			s->type = FLOAT_DATA;
			s->fvalue *= 10.0;
			break;

		case ATOT:
			s->type = INT_DATA;
			s->lvalue = time = *s->str_ptr;
			break;

		case EQUIV:
			if (equiv_ptr == 0) goto equiv_error;
			if (s->type == FLOAT_DATA) {
				s->lvalue = (int) s->fvalue;
			}
			s->type = INT_DATA;
			s->lvalue = equiv_ptr->Get_Group (s->lvalue);
			break;

		default:
			goto token_error;
			break;
	}
	return (true);

error:
	exe->Error (String ("Conversion \"%s\" does not support %s Data") % 
		Token_Name (CONVERT, cmd_ptr->token) % Data_Type (s->type));
	return (false);

token_error:
	exe->Error (String ("Conversion Token %d was Unrecognized") % cmd_ptr->token);
	return (false);

string_error:
	exe->Error ("Illegal SUBSTR Function Syntax");
	return (false);

equiv_error:
	exe->Error ("Equivalence Data Not Found");
	return (false);
}
