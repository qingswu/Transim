//*********************************************************
//	Initial_Map.cpp - Interpret a Map
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Initial_Map
//---------------------------------------------------------

bool User_Program::Initial_Map (String &line)
{
	int i, j, index, lnum, len;
	double fnum;
	bool first;

	Int_Itr litr, litr0;
	Dbl_Itr fitr, fitr0;
	String token, name;
	Str_Itr sitr, sitr0;
	string::iterator ch, ch0;

	Int_Dbl_Map_Stat ir_stat;
	Str_ID_Stat si_stat;
	Str_Map_Stat is_stat;

	i = cmd.token;
	j = cmd.value;

	for (first=true;;) {

		//---- remove leading spaces ----

		line.Trim_Left (" \t");
		if (line.empty ()) return (false);

		ch = ch0 = line.begin ();
	
		//---- determine the token type ----
	
		if ((*ch >= '0' && *ch <= '9') || (*ch == '.' && *(ch+1) != '.') || 
			(*ch == '-' && (*(ch+1) >= '0' && *(ch+1) <= '9'))) {
	
			//---- process a number ----

			if (*ch == '-') ch++;
		
			do {
				if (++ch == line.end ()) break;
			} while ((*ch >= '0' && *ch <= '9') || (*ch == '.' && *(ch+1) != '.'));
			
			len = (int) (ch - ch0);
			token = line.substr (0, len);
			line.erase (0, len);

			if (first) {
				if (map_type == IR_MAP || map_type == IS_MAP) {
					lnum = token.Integer ();
				} else if (map_type == SI_MAP) {
					exe->Error ("Attempt to Assign a Number to a String Map");
					return (false);
				}
				first = false;
			} else {
				if (map_type == IR_MAP) {
					fnum = token.Double ();
					ir_stat = ir_map [j].insert (Int_Dbl_Map_Data (index, fnum));
					if (!ir_stat.second) {
						exe->Error (String ("Inserting %d %lf into Map") % index % fnum);
					}
				} else if (map_type == IS_MAP) {
					exe->Error ("Attempt to Assign a Number to a String Map");
					return (false);
				} else if (map_type == SI_MAP) {
					lnum = token.Integer ();
					si_stat = si_map [j].insert (Str_ID_Data (name, lnum));
					if (!si_stat.second) {
						exe->Error (String ("Inserting %s %d into Map") % name % lnum);
						return (false);
					}
				}
				first = true;
			}

		} else if (*ch == '"') {

			//---- process a string ----
			
			ch++;
			token.clear ();
			
			while (ch != line.end () && *ch != '"') {
				token += *ch++;
			}
			ch++;
			line.erase (0, ch - ch0);

			if (first) {
				if (map_type == IR_MAP || map_type == IS_MAP) {
					exe->Error ("Attempt to Assign a String to a Number Map");
					return (false);
				} else if (map_type == SI_MAP) {
					name = token;
				}
				first = false;
			} else {
				if (map_type == IR_MAP || map_type == SI_MAP) {
					exe->Error ("Attempt to Assign a String to a Number Map");
					return (false);
				} else if (map_type == IS_MAP) {
					is_stat = is_map [j].insert (Str_Map_Data (lnum, token));
					if (!is_stat.second) {
						exe->Error (String ("Inserting %d %s into Map") % lnum % token);
						return (false);
					}
				}
				first = true;
			}

		} else if ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z')) {
				
			do {
				if (++ch == line.end ()) break;
			} while ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z') ||
				(*ch >= '0' && *ch <= '9') || (*ch == '.' && *(ch+1) != '.') || *ch == '_');
			
			len = (int) (ch - ch0);
			token = line.substr (0, len);
			line.erase (0, len);

			if (token.Equals ("ENDMAP")) {
				map_flag = false;
				return (true);
			} else {
				goto error;
			}

		} else if (*ch == ',' || *ch == '=') {
			ch++;
			line.erase (0, ch - ch0);
		} else {
			goto error;
		}
	}
	return (true);

error:
	return (Syntax_Error ("Map Declaration"));
}
