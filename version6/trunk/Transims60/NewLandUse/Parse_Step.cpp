//*********************************************************
//	Parse_Step.cpp - processing step codes
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Parse_Step
//---------------------------------------------------------

void NewLandUse::Parse_Step (String text, Processing_Data &step)
{
	int num;
	String command, code;
	Strings parts, diff;
	Str_Itr itr;

	text.Parse (parts, ",;");
	step.group_num = step.ptype_num = step.atype_num = -1;

	for (itr = parts.begin (); itr != parts.end (); itr++) {
		command = *itr;
		command.Compress ();

		if (command.Parse (diff, "-") == 2) {
			code = diff [0];

			if (code.Starts_With ("YEAR")) {
				code.erase (0, 4);
				step.year0 = code.Integer ();
				code = diff [1];
				if (code.Starts_With ("YEAR")) {
					code.erase (0, 4);
					step.year1 = code.Integer ();
				} else {
					Error (String ("Syntax in Command: %s") % *itr);
				}
				if (step.year0 > num_years || step.year1 > num_years) {
					Error (String ("Year Values are Out of Range in Command: %s") % *itr);
				} 
				step.year0 = year_index [step.year0];
				step.year1 = year_index [step.year1];
				if (step.year0 < 0 || step.year1 < 0) {
					Error (String ("Year Values are Not Defined for Command: %s") % *itr);
				} 
			} else if (code.Starts_With ("COVER")) {
				code.erase (0, 5);
				step.cover0 = code.Integer ();
				code = diff [1];
				if (code.Starts_With ("COVER")) {
					code.erase (0, 5);
					step.cover1 = code.Integer ();
				} else {
					Error (String ("Syntax in Command = %s") % *itr);
				}
				if (step.cover0 > num_cover || step.cover1 > num_cover) {
					Error (String ("Coverage Values are Out of Range in Command: %s") % *itr);
				}
				step.cover0 = cover_index [step.cover0];
				step.cover1 = cover_index [step.cover1];
				if (step.cover0 < 0 || step.cover1 < 0) {
					Error (String ("Coverage Values are Not Defined for Command: %s") % *itr);
				}
			} else {
				Error (String ("Unrecognized Difference Command: %s") % *itr);
			}
		} else {
			command = *itr;
			command.Compress ();

			if (command.Starts_With ("COVER")) {
				command.erase (0, 5);
				step.cover0 = command.Integer ();
				step.cover1 = -1;
				if (step.cover0 > num_cover) {
					Error (String ("Coverage Value is Out of Range in Command: %s") % *itr);
				}
				step.cover0 = cover_index [step.cover0];
				if (step.cover0 < 0) {
					Error (String ("Coverage Value is Not Defined for Command: %s") % *itr);
				}
			} else if (command.Starts_With ("GROUP")) {
				command.erase (0, 5);
				step.group_num = command.Integer ();
				if (step.group_num > num_group) {
					Error (String ("Group Value is Out of Range in Command: %s") % *itr);
				}
				if (step.group_num > 9) {
					command.erase (0, 2);
				} else {
					command.erase (0, 1);
				}
				step.group_num = group_index [step.group_num];
				if (step.group_num < 0) {
					Error (String ("Group Value is Not Defined for Command: %s") % *itr);
				}
				if (command [0] == '_') {
					command.erase (0, 2);
				}
				if (command [0] == '=') {
					step.group_logic = 1;
				} else if (command.Starts_With ("!=")) {
					step.group_logic = 0;
				} else {
					Error (String ("Group Logic Syntax in Command: %s") % *itr);
				}
			} else if (command.Starts_With ("TYPE")) {
				command.erase (0, 4);
				num = command.Integer ();
				if (num > num_type) {
					Error (String ("Type Value is Out of Range in Command: %s") % *itr);
				}
				if (num > 9) {
					command.erase (0, 2);
				} else {
					command.erase (0, 1);
				}
				num = type_index [num];
				if (num < 0) {
					Error (String ("Type Value is Not Defined for Command: %s") % *itr);
				}
				if (command.Starts_With ("_P")) {
					command.erase (0, 2);
					step.ptype_num = num;
					num = 0;
				} else if (command.Starts_With ("_A")) {
					command.erase (0, 2);
					step.atype_num = num;
					num = 1;
				} else {
					step.ptype_num = step.atype_num = num;
					num = 2;
				}
				if (command [0] == '=') {
					if (num != 1) {
						step.ptype_logic = 1;
					}
					if (num != 0) {
						step.atype_logic = 1;
					}
				} else if (command.Starts_With ("!=")) {
					if (num != 1) {
						step.ptype_logic = 0;
					}
					if (num != 0) {
						step.atype_logic = 0;
					}
				} else {
					Error (String ("Type Logic Syntax in Command: %s") % *itr);
				}
			} else {
				Error (String ("Unrecognized Command: %s") % *itr);
			}
		}
	}
}
