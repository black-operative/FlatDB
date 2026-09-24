#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Parser.h>
#include <Error.h>
#include <Utils.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::getline;
using std::exception;
using std::transform;
using std::runtime_error;
using std::istringstream;

static void Run_Command(Parse& parser, const std::string& cmd) {
	try {
		parser.Execute(cmd);
	} catch (ERROR_CODE code) {
		cerr << "Caught ERROR_CODE: " << static_cast<int>(code) << " for command: " << cmd << endl;
		cerr << GET_ERROR_MESSAGE(code) << endl;
	}
}

static void Test_Parse_Class() {
	Parse parser;

	// ---- CREATE DATABASE ----
	Run_Command(parser, "CREATE DATABASE TestDB");

	// // ---- USE DATABASE ----
	Run_Command(parser, "USE DATABASE TestDB");

	// ---- CREATE TABLE ----
	// Prepare schema JSON
	std::ofstream schemaFile("test_schema.json");
	schemaFile << R"({
	"name": "string",
	"age": "int",
	"salary": "float"
})";
	schemaFile.close();

	Run_Command(parser, "CREATE TABLE tb1 FROM test_schema.json");

	// ---- INSERT INTO TABLE ----
	Run_Command(parser, R"(INSERT INTO TABLE tb1 VALUES name="Alice" age=30 salary=50000.5)");
	Run_Command(parser, R"(INSERT INTO TABLE tb1 VALUES name="Bob" age=25 salary=42000.25)");

	// ---- SELECT * FROM ----
	Run_Command(parser, "SELECT * FROM tb1");

	// ---- SELECT * FROM WHERE ----
	Run_Command(parser, R"(SELECT * FROM tb1 WHERE age > 26)");

	// ---- UPDATE ----
	Run_Command(parser, R"(UPDATE tb1 SET salary=55000 WHERE name = "Alice")");

	// ---- TRUNCATE TABLE ----
	Run_Command(parser, "TRUNCATE TABLE tb1");

	// ---- DROP TABLE ----
	Run_Command(parser, "DROP TABLE tb1");

	// ---- DROP DATABASE ----
	//Run_Command(parser, "DROP DATABASE TestDB");

	// Clean up schema file
	std::remove("test_schema.json");

	cout << "\nAll Parse commands tested.\n";
}

static int Run_Interactive_Shell() {
	Parse parser;
	string cli_args;

	cout << "FlatDB v" << FLATDB_VERSION << " - type HELP for a list of commands, EXIT to quit.\n";

	while (true) {
		try {
			cout << FLATDB_PROMPT;

			if (!getline(cin, cli_args)) {
				cout << "\nGoodbye.\n";
				break;
			}

			if (cli_args.empty()) continue;

			// Extract the first word the same way Parse::Tokenize does,
			// so leading whitespace doesn't defeat the lookup below.
			istringstream first_line(cli_args);
			string first_word;
			first_line >> first_word;
			transform(first_word.begin(), first_word.end(), first_word.begin(), ::tolower);

			auto op_itr = OP_LOOKUP_TABLE.find(first_word);
			if (op_itr != OP_LOOKUP_TABLE.end()) {
				if (op_itr->second == OPERATION::EXIT) {
					string response;

					cout << "\nAre you sure you want to exit? (Y/n): ";
					getline(cin, response);
					if (
						!response.empty()   &&
						response[0] != 'y'  &&
						response[0] != 'Y'
					) { continue; }

					cout << "Clear Screen is enabled, proceeding will remove all terminal content, proceed? (y/N): ";
					getline(cin, response);
					if (
						!response.empty() &&
						(response[0] == 'y' || response[0] == 'Y')
					) { cout << SCREEN_CLEAR_COMMAND; }

					cout << "Thank you for using FlatDB.\n";
					break;
				} else if (op_itr->second == OPERATION::HELP) {
					Print_Help();
					continue;
				}
			}

			// Parse & execute
			parser.Execute(cli_args);
		} catch (const ERROR_CODE& e) {
			cerr << "[ERROR] " << GET_ERROR_MESSAGE(e) << endl;
		} catch (const runtime_error& e) {
			cerr << "[RUNTIME ERROR] " << e.what() << endl;
		} catch (const exception& e) {
			cerr << "[EXCEPTION] " << e.what() << endl;
		} catch (...) {
			cerr << "[UNKNOWN ERROR] Something went wrong. : " << cli_args << endl;
		}
	}

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc > 1 && string(argv[1]) == "--test") {
		cout << "Starting tests for Parse class...\n";
		Test_Parse_Class();
		cout << "\nAll tests completed successfully.\n";
		return 0;
	}

	return Run_Interactive_Shell();
}
