#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include <Utils.h> 

using std::max;
using std::cout;
using std::vector;
using std::string;
using std::unordered_map;

void Print_Help() {
	constexpr auto RESET = "\033[0m";
	constexpr auto BOLD = "\033[1m";
	constexpr auto CYAN = "\033[36m";
	constexpr auto GREEN = "\033[32m";
	constexpr auto YELLOW = "\033[33m";
	constexpr auto MAGENTA = "\033[35m";
	constexpr auto PURPLE = "\033[38;5;135m";

	cout << BOLD << RESET << MAGENTA;
	cout << "┌────────────────────────────────────────────────────────────────────────────┐\n";
	cout << "│                             " << PURPLE << "FlatDB CLI Help Menu" << RESET << MAGENTA << "                           │\n";
	cout << "├────────────────────────────────────────────────────────────────────────────┤\n";
	cout << "│ " << PURPLE << "Syntax : FlatDB accepts SQL-like commands for managing JSON-based storage." << RESET << MAGENTA << " │\n";
	cout << "│ " << PURPLE << "         All operations are non-relational and work on JSON schema & data." << RESET << MAGENTA << " │\n";
	cout << "├────────────────────────────────────────────────────────────────────────────┤\n";
	cout << "│ " << CYAN << "DATABASE COMMANDS :" << RESET << MAGENTA << "                                                        │\n";
	cout << "│                                                                            │\n";
	cout << "│ " << GREEN << "CREATE DATABASE" << RESET << " <db_name>              Create a new database directory     " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "USE DATABASE" << RESET << " <db_name>                 Switch to an existing database      " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "TRUNCATE DATABASE" << RESET << " <name>               Delete all tables but keep structure" << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "DROP DATABASE" << RESET << " <db_name>                Delete database and all contents    " << RESET << MAGENTA << "│\n";
	cout << "├────────────────────────────────────────────────────────────────────────────┤\n";
	cout << "│ " << CYAN << "TABLE COMMANDS :" << RESET << MAGENTA << "                                                           │\n";
	cout << "│                                                                            │\n";
	cout << "│ " << GREEN << "CREATE TABLE" << RESET << " <tb> " << YELLOW << "FROM" << RESET << " <schema.json>          Create table using schema    " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "INSERT INTO TABLE" << RESET << " <tb> " << YELLOW << "VALUES" << RESET << " (key=val,...)   Insert new record            " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "SELECT *" << YELLOW << " FROM" << RESET << " <tb>                            View all table records       " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "SELECT *" << YELLOW << " FROM" << RESET << " <tb> " << YELLOW << "WHERE" << RESET << " key=val              Filter records               " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "UPDATE" << RESET << " <tb> " << YELLOW << "SET" << RESET << " key=val " << YELLOW << "WHERE" << RESET << " key=val         Update matching records      " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "TRUNCATE TABLE" << RESET << " <tb>                           Delete all records           " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "DROP TABLE" << RESET << " <tb>                               Delete table schema and data " << RESET << MAGENTA << "│\n";
	cout << "├────────────────────────────────────────────────────────────────────────────┤\n";
	cout << "│ " << CYAN << "WHERE CONDITIONS :" << RESET << MAGENTA << "                                                         │\n";
	cout << "│                                                                            │\n";
	cout << "│ " << PURPLE << "Supported Operators :" << YELLOW << " =  !=  <  <=  >  >=" << RESET << MAGENTA << "                                  │\n";
	cout << "│ " << PURPLE << "Logical Connectors  :" << YELLOW << " AND, OR" << RESET << MAGENTA << "                                              │\n";
	cout << "│ " << PURPLE << "Example:" << RESET << MAGENTA << "                                                                   │\n";
	cout << "│   " << GREEN << "SELECT *" << YELLOW << " FROM" << RESET << " tb1 " << YELLOW << "WHERE" << RESET << " name = \"John\" " << YELLOW << "AND" << RESET << " age > 25                       " << RESET << MAGENTA << "│\n";
	cout << "├────────────────────────────────────────────────────────────────────────────┤\n";
	cout << "│ " << CYAN << "MISC COMMANDS :" << RESET << MAGENTA << "                                                            │\n";
	cout << "│                                                                            │\n";
	cout << "│ " << GREEN << "HELP" << RESET << "                                               Show this help screen   " << RESET << MAGENTA << "│\n";
	cout << "│ " << GREEN << "EXIT" << RESET << " or " << GREEN << "QUIT" << RESET << "                                       Exit the CLI application" << RESET << MAGENTA << "│\n";
	cout << "├────────────────────────────────────────────────────────────────────────────┤\n";
	cout << "│ " << CYAN << "NOTE :" << RESET << MAGENTA << "                                                                     │\n";
	cout << "│ " << PURPLE << "- All databases are stored under:" << RESET << MAGENTA << "                                          │\n";
	cout << "│   " << YELLOW << DATABASE_DIRECTORY << RESET << MAGENTA << std::setw(78 - 4 - string(DATABASE_DIRECTORY).length() - 1) << "" << "│\n";
	cout << "│ " << PURPLE << "- Schemas define field types (string, number, array, etc).                 " << RESET << MAGENTA << "│\n";
	cout << "│ " << PURPLE << "- Records are in " << YELLOW << "Data.json" << PURPLE << "; schema in " << YELLOW << "Schema.json" << PURPLE << " per table.               " << RESET << MAGENTA << "│\n";
	cout << "└────────────────────────────────────────────────────────────────────────────┘\n";
	cout << RESET;
}

void Print_Table(const json& records) {
	if (records.empty()) {
		cout << "\nNo Results to show.\n";
		return;
	}

	vector<string> columns;
	for (const auto& [key, val] : records[0].items()) {
		columns.push_back(key);
	}

	unordered_map<string, size_t> hash;
	for (const auto& column : columns) {
		hash[column] = column.size();
		for (const auto& row : records) {
			string val;

			if (row[column].is_string()) {
				val = row[column].get<string>();
			} else if (row[column].is_array()) {
				for (const auto& item : row[column]) {
					if (!val.empty()) val += ", ";
					val += item.is_string() ? item.get<string>() : item.dump();
				}
			} else {
				val = row[column].dump();
			}

			hash[column] = max(hash[column], val.size());
		}
	}

	cout << "+";
	for (const auto& column : columns) {
		cout << string(hash[column] + 2, '-') << "+";
	}
	cout << "\n|";
	for (const auto& column : columns) {
		cout << " " << column << string(hash[column] - column.size() + 1, ' ') << "|";
	}
	cout << "\n+";
	for (const auto& column : columns) {
		cout << string(hash[column] + 2, '-') << "+";
	}
	cout << "\n";

	for (const auto& row : records) {
		cout << "|";
		for (const auto& column : columns) {
			string val;

			if (row[column].is_string()) {
				val = row[column].get<string>();
			} else if (row[column].is_array()) {
				for (const auto& item : row[column]) {
					if (!val.empty()) val += ", ";
					val += item.is_string() ? item.get<string>() : item.dump();
				}
			} else {
				val = row[column].dump();
			}

			cout << " " << val << string(hash[column] - val.size() + 1, ' ') << "|";
		}
		cout << "\n";
	}

	cout << "+";
	for (const auto& column : columns) {
		cout << string(hash[column] + 2, '-') << "+";
	}
	cout << "\n";
	cout << records.size() << " row(s) in set.\n";
}

bool Is_Type_Valid(const json& value, const std::string& expected_type) {
	if (expected_type == "int")    return value.is_number_integer();
	if (expected_type == "float")  return value.is_number_float() || value.is_number_integer(); // allow ints for floats
	if (expected_type == "string") return value.is_string();
	if (expected_type == "char")   return value.is_string() && value.get<std::string>().size() == 1;
	if (expected_type == "array")  return value.is_array();
	return false;
}