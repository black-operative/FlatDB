#pragma once

#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <filesystem>

#include <Utils.h>

using std::mutex;
using std::string;
using std::vector;
using std::unordered_map;

using json = nlohmann::json;

namespace fs = std::filesystem;

class JSON_DB {
	private:
		string		  Table_Path;
		mutable mutex File_Mutex;

		void Ensure_Directory_Exists() const;

		static void Sync_File 	  (const string& path);
		static void Sync_Directory(const string& path);

	public:
		inline string Get_Schema_File() const { return (fs::path(Table_Path) / "Schema.json"); }
		inline string Get_Data_File()   const { return (fs::path(Table_Path) / "Data.json");   }

		json Read_JSON  (const string&) const;
		void Write_JSON (const string&, const json&);
		
		JSON_DB(const string& path) : Table_Path(path) {}

		bool Create_Table(const json&);

		bool Insert_Record(const json&);

		json Read_Records() const;
};
