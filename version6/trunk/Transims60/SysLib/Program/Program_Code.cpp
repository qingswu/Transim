//*********************************************************
//	Program_Code.cpp - user program codes
//*********************************************************

#include "User_Program.hpp"

User_Program::Token_Data  User_Program::keywords [] = {
		{"IF",          LOGICAL,    IF          },
		{"THEN",        LOGICAL,    THEN        },
		{"ELSE",        LOGICAL,    ELSE        },
		{"ENDIF",       LOGICAL,    ENDIF       },
		{"WHILE",       LOGICAL,    WHILE       },
		{"LOOP",        LOGICAL,    LOOP        },
		{"BREAK",       LOGICAL,    BREAK       },
		{"ENDLOOP",     LOGICAL,    ENDLOOP     },
		{"EQ",          RELATION,   EQ          },
		{"NE",          RELATION,   NEQ         },
		{"LT",          RELATION,   LT          },
		{"LE",          RELATION,   LE          },
		{"GT",          RELATION,   GT          },
		{"GE",          RELATION,   GE          },
		{"NOT",         RELATION,   NOT         },
		{"AND",         RELATION,   AND         },
		{"OR",          RELATION,   OR          },
		{"MIN",         FUNCTION,   FMIN        },
		{"MAX",         FUNCTION,   FMAX        },
		{"ABS",         FUNCTION,   ABS         },
		{"SQRT",        FUNCTION,   SQRT        },
		{"EXP",         FUNCTION,   EXP         },
		{"LOG",         FUNCTION,   LOG         },
		{"LOG10",       FUNCTION,   LOG10       },
		{"RANDOM",      FUNCTION,   RANDOM      },
		{"POWER",       FUNCTION,   POWER       },
		{"INT",         CONVERT,    FINT        },
		{"FLOAT",       CONVERT,    FFLOAT      },
		{"ROUND",       CONVERT,    ROUND       },
		{"ATOI",        CONVERT,    ATOI        },
		{"ATOF",        CONVERT,    ATOF        },
		{"ITOA",        CONVERT,    ITOA        },
		{"FTOA",        CONVERT,    FTOA        },
		{"SUBSTR",      CONVERT,    SUBSTR      },
		{"TRIM",        CONVERT,    TRIM        },
		{"LOW",         CONVERT,    LOW         },
		{"HIGH",        CONVERT,    HIGH        },
		{"RANGE",       CONVERT,    RANGE       },
		{"TTOI",        CONVERT,    TTOI        },
		{"TTOF",        CONVERT,    TTOF        },
		{"ITOT",        CONVERT,    ITOT        },
		{"FTOT",        CONVERT,    FTOT        },
		{"ATOT",        CONVERT,    ATOT        },
		{"EQUIV",       CONVERT,    EQUIV       },	
		{"DOW",         DATETIME,   DOW         },
		{"HOUR",        DATETIME,   HOUR        },
		{"MONTH",       DATETIME,   MONTH       },
		{"YEAR",        DATETIME,   YEAR        },
		{"DATE",        DATETIME,   DATE        },
		{"DATE_TIME",   DATETIME,   DATE_TIME   },
		{"DATE_STR",    DATETIME,   DATE_STR    },
		{"TIME_STR",    DATETIME,   TIME_STR    },
		{"PRINT",       IN_OUT,     PRINT       },
		{"LIST",        IN_OUT,     LIST        },
		{"FORMAT",      IN_OUT,     FORMAT      },
		{"READ",        IN_OUT,     FREAD       },
		{"WRITE",       IN_OUT,     FWRITE      },
		{"INPUT",       IN_OUT,     INPUT       },
		{"OUTPUT",      IN_OUT,     OUTPUT      },
		{"RETURN",      RETURN,     RETURN      },
		{"END",         LIMIT,      END_CMD     },
		{"",            0,          0           }
};

User_Program::Token_Data  User_Program::symbols [] = {
		{"=",       LIMIT,      EQUALS      },
		{"+",       MATH,       PLUS        },
		{"-",       MATH,       MINUS       },
		{"*",       MATH,       MULTIPLY    },
		{"/",       MATH,       DIVIDE      },
		{"%",       MATH,       MODULO      },
		{"**",      MATH,       POWER       },
		{"==",      RELATION,   EQ          },
		{"!=",      RELATION,   NEQ         },
		{"<",       RELATION,   LT          },
		{"<=",      RELATION,   LE          },
		{">",       RELATION,   GT          },
		{">=",      RELATION,   GE          },
		{"!",       RELATION,   NOT         },
		{"&&",      RELATION,   AND         },
		{"||",      RELATION,   OR          },
		{"{",       LOGICAL,    LOOP        },
		{"}",       LOGICAL,    ENDLOOP     },
		{"",        0,          0           }
};

User_Program::Token_Data  User_Program::specials [] = {
		{"INTEGER", DECLARE,    IVAR        },
		{"REAL",    DECLARE,    RVAR        },
		{"STRING",  DECLARE,    SVAR        },
		{"ITAB",    TABLE,      ITAB        },
		{"RTAB",    TABLE,      RTAB        },
		{"STAB",    TABLE,      STAB        },
		{"ITAB0",   TABLE,      ITAB        },
		{"RTAB0",   TABLE,      RTAB        },
		{"STAB0",   TABLE,      STAB        },
		{"ITAB1",   TABLE,      ITAB + 10   },
		{"RTAB1",   TABLE,      RTAB + 10   },
		{"STAB1",   TABLE,      STAB + 10   },
		{"ITAB2",   TABLE,      ITAB + 20   },
		{"RTAB2",   TABLE,      RTAB + 20   },
		{"STAB2",   TABLE,      STAB + 20   },
		{"ITAB3",   TABLE,      ITAB + 30   },
		{"RTAB3",   TABLE,      RTAB + 30   },
		{"STAB3",   TABLE,      STAB + 30   },
		{"ITAB4",   TABLE,      ITAB + 40   },
		{"RTAB4",   TABLE,      RTAB + 40   },
		{"STAB4",   TABLE,      STAB + 40   },
		{"ITAB5",   TABLE,      ITAB + 50   },
		{"RTAB5",   TABLE,      RTAB + 50   },
		{"STAB5",   TABLE,      STAB + 50   },
		{"ITAB6",   TABLE,      ITAB + 60   },
		{"RTAB6",   TABLE,      RTAB + 60   },
		{"STAB6",   TABLE,      STAB + 60   },
		{"ITAB7",   TABLE,      ITAB + 70   },
		{"RTAB7",   TABLE,      RTAB + 70   },
		{"STAB7",   TABLE,      STAB + 70   },
		{"ITAB8",   TABLE,      ITAB + 80   },
		{"RTAB8",   TABLE,      RTAB + 80   },
		{"STAB8",   TABLE,      STAB + 80   },
		{"ITAB9",   TABLE,      ITAB + 90   },
		{"RTAB9",   TABLE,      RTAB + 90   },
		{"STAB9",   TABLE,      STAB + 90   },
		{"IR_MAP",  MAP,        IR_MAP      },
		{"IS_MAP",  MAP,        IS_MAP      },
		{"SI_MAP",  MAP,        SI_MAP      },
		{"IR_MAP0", MAP,        IR_MAP      },
		{"IS_MAP0", MAP,        IS_MAP      },
		{"SI_MAP0", MAP,        SI_MAP      },
		{"IR_MAP1", MAP,        IR_MAP + 10 },
		{"IS_MAP1", MAP,        IS_MAP + 10 },
		{"SI_MAP1", MAP,        SI_MAP + 10 },
		{"IR_MAP2", MAP,        IR_MAP + 20 },
		{"IS_MAP2", MAP,        IS_MAP + 20 },
		{"SI_MAP2", MAP,        SI_MAP + 20 },
		{"IR_MAP3", MAP,        IR_MAP + 30 },
		{"IS_MAP3", MAP,        IS_MAP + 30 },
		{"SI_MAP3", MAP,        SI_MAP + 30 },
		{"IR_MAP4", MAP,        IR_MAP + 40 },
		{"IS_MAP4", MAP,        IS_MAP + 40 },
		{"SI_MAP4", MAP,        SI_MAP + 40 },
		{"IR_MAP5", MAP,        IR_MAP + 50 },
		{"IS_MAP5", MAP,        IS_MAP + 50 },
		{"SI_MAP5", MAP,        SI_MAP + 50 },
		{"IR_MAP6", MAP,        IR_MAP + 60 },
		{"IS_MAP6", MAP,        IS_MAP + 60 },
		{"SI_MAP6", MAP,        SI_MAP + 60 },
		{"IR_MAP7", MAP,        IR_MAP + 70 },
		{"IS_MAP7", MAP,        IS_MAP + 70 },
		{"SI_MAP7", MAP,        SI_MAP + 70 },
		{"IR_MAP8", MAP,        IR_MAP + 80 },
		{"IS_MAP8", MAP,        IS_MAP + 80 },
		{"SI_MAP8", MAP,        SI_MAP + 80 },
		{"IR_MAP9", MAP,        IR_MAP + 90 },
		{"IS_MAP9", MAP,        IS_MAP + 90 },
		{"SI_MAP9", MAP,        SI_MAP + 90 },
		{"",        0,          0           }
};

const char * User_Program::data_type [] = {
	"Integer",
	"Decimal",
	"String",
	"Structure"
};

//---------------------------------------------------------
//	Keyword
//---------------------------------------------------------

User_Program::Token_Data * User_Program::Keyword (string &token)
{
	for (token_ptr = keywords; !token_ptr->name.empty (); token_ptr++) {
		if (token_ptr->name.Equals (token)) {
			return (token_ptr);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Symbol
//---------------------------------------------------------

User_Program::Token_Data * User_Program::Symbol (string &token)
{
	for (token_ptr = symbols; !token_ptr->name.empty (); token_ptr++) {
		if (token_ptr->name.Equals (token)) {
			return (token_ptr);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Special
//---------------------------------------------------------

User_Program::Token_Data * User_Program::Special (string &token)
{
	for (token_ptr = specials; !token_ptr->name.empty (); token_ptr++) {
		if (token_ptr->name.Equals (token)) {
			return (token_ptr);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Variable
//---------------------------------------------------------

User_Program::Token_Data * User_Program::Variable (string &token)
{
	Token_Itr itr;

	for (itr = variables.begin (); itr != variables.end (); itr++) {
		if (itr->name.Equals (token)) {
			token_ptr = &(*itr);
			return (token_ptr);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Token_Name
//---------------------------------------------------------

string User_Program::Token_Name (int type, int code)
{
	for (token_ptr = keywords; !token_ptr->name.empty (); token_ptr++) {
		if (token_ptr->type == type && token_ptr->code == code) {
			return (token_ptr->name);
		}
	}
	for (token_ptr = specials; !token_ptr->name.empty (); token_ptr++) {
		if (token_ptr->type == type && token_ptr->code == code) {
			return (token_ptr->name);
		}
	}
	for (token_ptr = symbols; !token_ptr->name.empty (); token_ptr++) {
		if (token_ptr->type == type && token_ptr->code == code) {
			return (token_ptr->name);
		}
	}
	return ("");
}

//---------------------------------------------------------
//	Variable_Name
//---------------------------------------------------------

string User_Program::Variable_Name (int type, int code)
{
	Token_Itr itr;

	for (itr = variables.begin (); itr != variables.end (); itr++) {
		if (itr->type == type && itr->code == code) {
			return (itr->name);
		}
	}
	return ("");
}
