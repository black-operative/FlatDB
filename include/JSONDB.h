#pragma once

#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include <Utils.h>

using std::mutex;
using std::string;
using std::vector;
using std::weak_ptr;
using std::shared_ptr;
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

		// JSON_DB Mutex Registry
		explicit JSON_DB(const string& path);

		static mutex Registry_Mutex;
		static unordered_map<string, weak_ptr<JSON_DB>> Registry;

	public:
		JSON_DB(const JSON_DB&)            = delete;
		JSON_DB& operator=(const JSON_DB&) = delete;

		inline string Get_Schema_File() const { return (fs::path(Table_Path) / "Schema.json"); }
		inline string Get_Data_File()   const { return (fs::path(Table_Path) / "Data.json");   }

		json Read_JSON  (const string&) const;
		void Write_JSON (const string&, const json&);
		
		bool Create_Table(const json&);
		bool Insert_Record(const json&);
		json Read_Records() const;

		// JSON_DB Mutex Registry
		static shared_ptr<JSON_DB> Get(const fs::path& resolved_path);
};
