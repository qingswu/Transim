//*********************************************************
//	Map.cpp - Interpret a Map
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Map
//---------------------------------------------------------

bool User_Program::Map (void)
{
	int i, j, index;
	String name;

	Str_Map_Itr is_itr;
	Str_ID_Itr si_itr;
	Int_Dbl_Map_Itr ir_itr;

	i = cmd_ptr->token;
	j = cmd_ptr->value;

	if (i == IR_MAP) {
		if (s->type == FLOAT_DATA) {
			index = (int) s->fvalue;
		} else if (s->type == INT_DATA) {
			index = (int) s->lvalue;
		} else {
			goto index_error;
		}
		ir_itr = ir_map [j].find (index);
		if (ir_itr == ir_map [j].end ()) {
			exe->Error (String ("IR_Map%d Index %d was Not Found") % j % index);
			return (false);
		}
		s->type = FLOAT_DATA;
		s->fvalue = ir_itr->second;
	} else if (i == IS_MAP) {
		if (s->type == FLOAT_DATA) {
			index = (int) s->fvalue;
		} else if (s->type == INT_DATA) {
			index = (int) s->lvalue;
		} else {
			goto index_error;
		}
		is_itr = is_map [j].find (index);
		if (is_itr == is_map [j].end ()) {
			exe->Error (String ("IS_Map%d Index %d was Not Found") % j % index);
			return (false);
		}
		s->type = STRING_DATA;
		s->str_ptr = &is_itr->second;
	} else if (i == SI_MAP) {
		if (s->type == STRING_DATA) {
			name = *s->str_ptr;
		} else {
			goto index_error;
		}
		si_itr = si_map [j].find (name);
		if (si_itr == si_map [j].end ()) {
			exe->Error (String ("SI_Map%d Index %s was Not Found") % j % name);
			return (false);
		}
		s->type = INT_DATA;
		s->lvalue = si_itr->second;
	}
	return (true);

index_error:
	exe->Error (String ("Illegal Map Index = %s") % Data_Type (s->type));
	return (false);
}
