#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;

enum class OPERATION {
    CREATE = 0,
    USE,
    TRUNCATE,
    DROP,
    INSERT,
    SELECT,
    UPDATE,
    HELP,
    EXIT
};
const unordered_map<string, OPERATION> OP_LOOKUP_TABLE = {
    { "create",   OPERATION::CREATE   },
    { "use",      OPERATION::USE      },
    { "truncate", OPERATION::TRUNCATE },
    { "drop",     OPERATION::DROP     },
    { "insert",   OPERATION::INSERT   },
    { "select",   OPERATION::SELECT   },
    { "update",   OPERATION::UPDATE   },
    { "help",	  OPERATION::HELP	  },
    { "exit",	  OPERATION::EXIT	  }
};

enum class KEYWORD {
	FROM = 0,
	INTO,
	VALUES,
	WHERE,
	SET
};
const unordered_map<string, KEYWORD> KW_LOOKUP_TABLE = {
	{ "from",   KEYWORD::FROM   },
	{ "into",   KEYWORD::INTO   },
	{ "where",  KEYWORD::WHERE  },
	{ "values", KEYWORD::VALUES },
	{ "set"	  , KEYWORD::SET	}
};

enum class RELATIONAL_OPS {
	EQUALS = 1,
	NOT_EQUALS,
	SMALLER,
	GREATER,
	SMALLER_EQUALS,
	GREATER_EQUALS
};
const unordered_map<string, RELATIONAL_OPS> RE_OPS_LOOKUP_TABLE = {
	{ "=",	RELATIONAL_OPS::EQUALS		   },
	{ "!=", RELATIONAL_OPS::NOT_EQUALS	   },
	{ "<",  RELATIONAL_OPS::SMALLER		   },
	{ ">",  RELATIONAL_OPS::GREATER		   },
	{ "<=", RELATIONAL_OPS::SMALLER_EQUALS },
	{ ">=", RELATIONAL_OPS::GREATER_EQUALS }
};

class Parse {
	private:
		string Current_DB;
		vector<string> Tokens;

		void Tokenize(const string&);

		void Create();
		void Truncate();
		void Drop();

		void Use();

		void Insert();
		void Select();
		void Update();

	public:
		void Execute(const string&);
};
