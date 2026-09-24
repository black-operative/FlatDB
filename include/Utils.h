#pragma once

#include <string>
#include <nlohmann/json.hpp>

using std::string;

using json = nlohmann::json;

// Resolved at startup to "<directory containing the running binary>/data" — see Utils.cpp.
extern const string DATABASE_DIRECTORY;

constexpr auto FLATDB_VERSION       = "1.0.1";                               // App Version
constexpr auto FLATDB_PROMPT        = "\x1B[31mFlatDB\033[0m >> ";           // Red color text, prompt
constexpr auto SCREEN_CLEAR_COMMAND = "\x1B[2J\x1B[3J\x1B[H";                // Clear screen ANSI escape code

void Print_Help();

void Print_Table(const json&);

bool Is_Type_Valid(const json&, const string&);