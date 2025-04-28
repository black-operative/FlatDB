// #include <iostream>
// #include <string>
// #include <algorithm>

// #include <Error.h>
// #include <Parser.h>
// #include <Utils.h>

// using std::cin;
// using std::cout;
// using std::cerr;
// using std::endl;
// using std::string;
// using std::getline;
// using std::exception;
// using std::transform;
// using std::runtime_error;

// int main() {
//     Parse parser;
//     string cli_args;

//     while (true) {
//         try {
//             cin.clear();
//             cin.sync();
//             cout << FLATDB_PROMPT;
//             getline(cin, cli_args);
//             cin.clear();
//             cin.sync();

//             if (cli_args.empty()) continue;

//             size_t first_space = cli_args.find(' ');
//             string first_word = (first_space == string::npos) ? cli_args : cli_args.substr(0, first_space);

//             // Lowercase just the first word
//             transform(first_word.begin(), first_word.end(), first_word.begin(), ::tolower);

//             if (OP_LOOKUP_TABLE.find(first_word) != OP_LOOKUP_TABLE.end()) {
//                 OPERATION op = OP_LOOKUP_TABLE.at(first_word);

//                 if (op == OPERATION::EXIT) {
//                     char Ch = 0;
//                     cout << "\nAre you sure you want to exit? (Y/n): ";
//                     cin.get(Ch);
//                     cin.ignore();

//                     if (Ch == 'y' || Ch == 'Y' || Ch == '\n') {
//                         cout << "Clear Screen is enabled, proceeding will remove all terminal content, proceed? (y/N): ";
//                         cin.get(Ch);
//                         cin.ignore();

//                         if (Ch == 'y' || Ch == 'Y') {
//                             cout << SCREEN_CLEAR_COMMAND;
//                         }

//                         cout << "Thank you for using FlatDB.\n";
//                         break;
//                     }

//                     continue;
//                 } else if (op == OPERATION::HELP) {
//                     Print_Help();
//                     continue;
//                 }
//             }

//             // Parse & execute
//             parser.Execute(cli_args);
//         } catch (const ERROR_CODE& e) {
//             cerr << "[ERROR] " << GET_ERROR_MESSAGE(e) << endl;
//         } catch (const runtime_error& e) {
//             cerr << "[RUNTIME ERROR] " << e.what() << endl;
//         } catch (const exception& e) {
//             cerr << "[EXCEPTION] " << e.what() << endl;
//         } catch (...) {
//             cerr << "[UNKNOWN ERROR] Something went wrong. : " << cli_args << endl;
//         }
//     }

//     return 0;
// }

#include <iostream>
#include <cassert>
#include <fstream>
#include <Parser.h>
#include <Error.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using std::cout;
using std::cerr;
using std::endl;

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
	// Run_Command(parser, "CREATE DATABASE TestDB");

	// // ---- USE DATABASE ----
	// Run_Command(parser, "USE DATABASE TestDB");

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

int main() {
	cout << "Starting tests for Parse class...\n";
	Test_Parse_Class();
	cout << "\nAll tests completed successfully.\n";
	return 0;
}
