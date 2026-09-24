#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

#include <Utils.h>
#include <Parser.h>
#include <Error.h>
#include <JSONDB.h>

using std::cin;
using std::cout;
using std::tolower;
using std::transform;
using std::stringstream;

fs::path Resolve_DB_Path(
	const string& base_dir, 
	const string& key
) {
    if (key.empty() || key.find("..") != std::string::npos) {
        throw ERROR_CODE::DIRECTORY_CREATE; // or a more specific code
    }
    fs::path base 	   = fs::weakly_canonical(base_dir);
    fs::path candidate = fs::weakly_canonical(base / key);

    auto [baseEnd, _] = std::mismatch(
		base.begin(), base.end(), 
		candidate.begin(), candidate.end()
	);

    if (baseEnd != base.end()) { throw ERROR_CODE::DIRECTORY_CREATE; }
    
	return candidate;
}

string To_Lower(const string& s) {
		string out = s;
		transform(
			out.begin(), 
			out.end(), 
			out.begin(),
			[](unsigned char c) { return std::tolower(c); }
		);
		return out;
	}

void Parse::Tokenize(const string& command) {
	if (command.empty()) { return; }
	
	vector<string> T_Tokens;

	stringstream SS(command);
	string word;
	while (SS >> word) { T_Tokens.push_back(word); }

	Tokens = T_Tokens;
}

// Common Functions
void Parse::Create() {
	if (Tokens.empty()) { return; }

	string type = To_Lower(Tokens[1]);

	if (type == "database") {
		if (Tokens.size() < 3) { throw ERROR_CODE::DIRECTORY_EMPTY; }
		cout << "\nCreating Database....\n";
		
		if (
			Tokens[2].empty() || 
			Tokens[2].find("..") != std::string::npos
		) { throw ERROR_CODE::DIRECTORY_CREATE; }

		fs::path candidate = Resolve_DB_Path(DATABASE_DIRECTORY, Tokens[2]);
		if (
			!fs::is_directory(candidate) &&
			!fs::create_directories(candidate)
		) { throw ERROR_CODE::DIRECTORY_CREATE; }

		cout << "\nDatabase Created. Would you like to switch to " << Tokens[2] << " ? [y/N] : ";
		string response;
		getline(cin, response);
		if (
			!response.empty() &&
			(response[0] == 'Y' || response[0] == 'y')
		) {
			this->Current_DB  = candidate; // store the resolved path
			this->DB_Selected = true;
			cout << "\nSwitched to database " << Tokens[2] << "\n";
		}

	} else if (type == "table") {
		if (!this->DB_Selected) throw ERROR_CODE::DATABASE_NSELECT;

		if (Tokens.size() < 3) { throw ERROR_CODE::TABLE_EMPTY;     }
		if (Tokens.size() < 5) { throw ERROR_CODE::FILE_PATH_EMPTY; }

		auto itr = KW_LOOKUP_TABLE.find(Tokens[3]);
		if (
			itr == KW_LOOKUP_TABLE.end() || 
			itr->second != KEYWORD::FROM
		) { throw ERROR_CODE::SYNTAX_FROM; }

		cout << "\nATTENTTION : THIS WILL CREATE THE TABLE IN THE FOLLOWING DATABASE : " << Current_DB
			 << "\nIf you wish to change your target database, use : "
			 << "\nUSE DATABASE <Database Name>"
			 << "\n\nChange DB ? [Y/n] : ";
		string response;
		getline(cin, response);
		if (
			response.empty()   || 
			response[0] == 'Y' || 
			response[0] == 'y'
		) { return; }

		fs::path candidate = Resolve_DB_Path(DATABASE_DIRECTORY, Tokens[2]);
		JSON_DB Database(candidate);
		if (
			!Database.Create_Table(
				Database.Read_JSON(Tokens[4])
			)
		) { throw ERROR_CODE::TABLE_CREATE; }

		cout << "\nTable Created.\n";
	} else {
		throw ERROR_CODE::SYNTAX_CREATE;
	}
}

void Parse::Truncate() {
	if (!this->DB_Selected) throw ERROR_CODE::DATABASE_NSELECT;

	if (Tokens.size() < 3) throw ERROR_CODE::SYNTAX_TYPE;

	string type = To_Lower(Tokens[1]);
	if (type != "database" && type != "table") { throw ERROR_CODE::SYNTAX_TYPE; }

	if (type == "database") {
		fs::path db_path = Resolve_DB_Path(DATABASE_DIRECTORY, Tokens[2]);
		if (!fs::is_directory(db_path)) { throw ERROR_CODE::DIRECTORY_WHERE; }

		for (const auto& directory_entry : fs::directory_iterator(db_path)) {
			if (directory_entry.is_directory()) {
				fs::remove_all(directory_entry.path());
			}
		}

		cout << "\nDatabase : [" << db_path << "] truncated.\n";
	} else if (type == "table") {
		fs::path t_path = Resolve_DB_Path(Current_DB, Tokens[2]);
		if (!fs::is_directory(t_path)) { throw ERROR_CODE::TABLE_WHERE; }

		JSON_DB Database(t_path);
		Database.Write_JSON(Database.Get_Data_File(), json::array());

		cout << "\nTable : [" << Tokens[2] << "] <- [" << Current_DB << "] truncated.\n";
	}
}

void Parse::Drop() {
	if (!this->DB_Selected) throw ERROR_CODE::DATABASE_NSELECT;

	if (Tokens.size() < 3) throw ERROR_CODE::SYNTAX_TYPE;

	string type = To_Lower(Tokens[1]);
	if (type != "database" && type != "table") { throw ERROR_CODE::SYNTAX_TYPE; }

	if (type == "database") {
		fs::path db_path = Resolve_DB_Path(DATABASE_DIRECTORY, Tokens[2]);
		if (!fs::is_directory(db_path)) { throw ERROR_CODE::DIRECTORY_WHERE; }

		fs::remove_all(db_path);
		cout << "\nDatabase : [" << db_path << "] dropped.\n";

		if (Current_DB == db_path) {
			Current_DB.clear();
			DB_Selected = false;
		}

	} else if (type == "table") {
		fs::path t_path = Resolve_DB_Path(Current_DB, Tokens[2]);
		if (!fs::is_directory(t_path)) { throw ERROR_CODE::TABLE_WHERE; }

		fs::remove_all(t_path);

		cout << "\nTable : [" << Tokens[2] << "] <- [" << Current_DB << "] dropped.\n";
	}
}

// Database ONLY Function
void Parse::Use() {
	if (Tokens.size() < 3) { throw ERROR_CODE::SYNTAX_TYPE; }

	string type = To_Lower(Tokens[1]);
	if (type != "database") { throw ERROR_CODE::SYNTAX_TYPE; }

	fs::path db_path = Resolve_DB_Path(DATABASE_DIRECTORY, Tokens[2]);
	if (!fs::is_directory(db_path)) { throw ERROR_CODE::DIRECTORY_WHERE; }

	this->Current_DB  = db_path;
	this->DB_Selected = true;
	cout << "\nSwitched to database: " << Current_DB << "\n";
}

// Table ONLY Function
void Parse::Insert() {
	if (!this->DB_Selected) throw ERROR_CODE::DATABASE_NSELECT;

	if (Tokens.size() < 6) throw ERROR_CODE::SYNTAX_INSERT;

	if (
		To_Lower(Tokens[1]) != "into" || 
		To_Lower(Tokens[2]) != "table"
	) { throw ERROR_CODE::SYNTAX_TYPE; }

	if (KW_LOOKUP_TABLE.at(Tokens[4]) != KEYWORD::VALUES)
		throw ERROR_CODE::SYNTAX_VALUE;

	string table_name = Tokens[3];
	json record;
	for (size_t i = 5; i < Tokens.size(); ++i) {
		auto pos = Tokens[i].find('=');
		if (pos == string::npos) continue;

		string key = Tokens[i].substr(0, pos);
		string val = Tokens[i].substr(pos + 1);

		if (
			!val.empty() && 
			val.front() == '"' && 
			val.back() == '"'
		) { val = val.substr(1, val.length() - 2); }

		try {
			if (val.find('.') != string::npos)
				record[key] = std::stof(val);
			else
				record[key] = std::stoi(val);
		} catch (...) {
			record[key] = val;
		}
	}
	
	JSON_DB Database(Resolve_DB_Path(Current_DB, table_name));
	if (!Database.Insert_Record(record)) throw ERROR_CODE::TABLE_INSERT;
	cout << "\nRecord inserted into " << table_name << ".\n";
}

void Parse::Select() {
	if (!this->DB_Selected) throw ERROR_CODE::DATABASE_NSELECT;

	if (Tokens.size() < 4 || Tokens[1] != "*") { throw ERROR_CODE::SYNTAX_SELECT; }

	auto kw_itr = KW_LOOKUP_TABLE.find(To_Lower(Tokens[2]));
	if (
		kw_itr 		   == KW_LOOKUP_TABLE.end() || 
		kw_itr->second != KEYWORD::FROM
	) { throw ERROR_CODE::SYNTAX_SELECT; }

	string t_name = Tokens[3];
	JSON_DB Database(Resolve_DB_Path(Current_DB, t_name));
	auto data = Database.Read_Records();

	// Simple SELECT * FROM tb1
	if (Tokens.size() == 4) {
		Print_Table(data);
		return;
	}

	// SELECT * FROM tb1 WHERE key <op> value
	kw_itr = KW_LOOKUP_TABLE.find(To_Lower(Tokens[4]));
	if (
		Tokens.size()  >= 8 					&& 
		kw_itr		   != KW_LOOKUP_TABLE.end() && 
		kw_itr->second == KEYWORD::WHERE
	) {
		string key = Tokens[5];
		string ops = Tokens[6];
		string val = Tokens[7];

		// Strip quotes if value is quoted
		if (
			!val.empty() && 
			val.front() == '"' && 
			val.back() == '"'
		) { val = val.substr(1, val.size() - 2); }

		auto ops_itr = RE_OPS_LOOKUP_TABLE.find(ops);
		if (ops_itr == RE_OPS_LOOKUP_TABLE.end()) throw ERROR_CODE::SYNTAX_OPERATOR;

		json filtered = json::array();
		for (const auto& record : data) {
			if (!record.contains(key)) continue;

			string actual = record[key].is_string()
				? record[key].get<string>()
				: record[key].dump();  

			bool match = false;

			RELATIONAL_OPS RE_OP = RE_OPS_LOOKUP_TABLE.at(ops);
			switch (RE_OP) {
				case RELATIONAL_OPS::EQUALS:         match = (actual == val); break;
				case RELATIONAL_OPS::NOT_EQUALS:     match = (actual != val); break;
				case RELATIONAL_OPS::SMALLER:        match = (actual < val);  break;
				case RELATIONAL_OPS::GREATER:        match = (actual > val);  break;
				case RELATIONAL_OPS::SMALLER_EQUALS: match = (actual <= val); break;
				case RELATIONAL_OPS::GREATER_EQUALS: match = (actual >= val); break;
				default: throw ERROR_CODE::SYNTAX_OPERATOR;
			}

			if (match) filtered.push_back(record);
		}

		Print_Table(filtered);
	} else {
		throw ERROR_CODE::SYNTAX_WHERE;
	}
}

void Parse::Update() {
	if (!this->DB_Selected) throw ERROR_CODE::DATABASE_NSELECT;

	if (Tokens.size() < 8) { throw ERROR_CODE::SYNTAX_UPDATE; }

	auto kw_itr = KW_LOOKUP_TABLE.find(To_Lower(Tokens[2]));
	if (
		kw_itr 		   == KW_LOOKUP_TABLE.end() || 
		kw_itr->second != KEYWORD::SET
	) { throw ERROR_CODE::SYNTAX_UPDATE; }

	string t_name   = Tokens[1];
	string set_expr = Tokens[3]; // Example: key="value"

	size_t eq_pos = set_expr.find('=');
	if (eq_pos == string::npos) throw ERROR_CODE::SYNTAX_SET;

	string set_key = set_expr.substr(0, eq_pos);
	string set_val = set_expr.substr(eq_pos + 1);

	if (
		!set_val.empty() &&
		set_val.front() == '"' &&
		set_val.back() == '"'
		) {
		set_val = set_val.substr(1, set_val.size() - 2);
	}

    // WHERE clause
	kw_itr = KW_LOOKUP_TABLE.find(To_Lower(Tokens[4]));
	if (
		kw_itr == KW_LOOKUP_TABLE.end() || 
		kw_itr->second != KEYWORD::WHERE
	) { throw ERROR_CODE::SYNTAX_WHERE; }

	string cond_key = Tokens[5];
	string cond_op = Tokens[6];
	string cond_val = Tokens[7];

	if (
		!cond_val.empty() &&
		cond_val.front() == '"' &&
		cond_val.back() == '"'
		) {
		cond_val = cond_val.substr(1, cond_val.size() - 2);
	}

	auto ops_itr = RE_OPS_LOOKUP_TABLE.find(cond_op);
	if (ops_itr == RE_OPS_LOOKUP_TABLE.end()) throw ERROR_CODE::SYNTAX_OPERATOR;

	// Begin update
	JSON_DB Database(Resolve_DB_Path(Current_DB, t_name));
	json data = Database.Read_Records();

	size_t updated_count = 0;
	for (auto& record : data) {
		if (!record.contains(cond_key)) continue;

		string actual = record[cond_key].is_string()
			? record[cond_key].get<string>()
			: record[cond_key].dump();

		bool match = false;
		switch (ops_itr->second) {
			case RELATIONAL_OPS::EQUALS:         match = (actual == cond_val); break;
			case RELATIONAL_OPS::NOT_EQUALS:     match = (actual != cond_val); break;
			case RELATIONAL_OPS::SMALLER:        match = (actual < cond_val);  break;
			case RELATIONAL_OPS::GREATER:        match = (actual > cond_val);  break;
			case RELATIONAL_OPS::SMALLER_EQUALS: match = (actual <= cond_val); break;
			case RELATIONAL_OPS::GREATER_EQUALS: match = (actual >= cond_val); break;
			default: throw ERROR_CODE::SYNTAX_OPERATOR;
		}

		if (match) {
			// numeric coercion, matching Insert()'s behavior
			try {
				if (set_val.find('.') != string::npos)
					record[set_key] = std::stof(set_val);
				else
					record[set_key] = std::stoi(set_val);
			} catch (...) {
				record[set_key] = set_val;
			}
			updated_count++;
		}
	}

	Database.Write_JSON(Database.Get_Data_File(), data);
	cout << "\n" << updated_count << " record(s) updated.\n";
}

// API method
void Parse::Execute(const string& command) {
	if (command.empty()) { return; }

	Tokenize(command);
	
	if (Tokens.empty()) { return; }

	string action = To_Lower(Tokens[0]);
	auto itr = OP_LOOKUP_TABLE.find(action);
	if (itr == OP_LOOKUP_TABLE.end()) throw ERROR_CODE::SYNTAX_UNKNOWN;

	switch (itr->second) {
		case OPERATION::CREATE:
			Create();
			break;

		case OPERATION::USE:
			Use();
			break;

		case OPERATION::TRUNCATE:
			Truncate();
			break;

		case OPERATION::DROP:
			Drop();
			break;

		case OPERATION::INSERT:
			Insert();
			break;
		case OPERATION::SELECT:
			Select();
			break;

		case OPERATION::UPDATE:
			Update();
			break;

		default:
			throw ERROR_CODE::SYNTAX_UNKNOWN;
			break;
	}
}
