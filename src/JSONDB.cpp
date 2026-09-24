#include <fstream>
#include <ios>
#include <iostream>
#include <filesystem>
#include <system_error>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
#endif

#include <Error.h>
#include <JSONDB.h>

using std::endl;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::lock_guard;

namespace fs = std::filesystem;

void JSON_DB::Sync_File(const string& path) {
	#ifdef _WIN32
		HANDLE h = CreateFileA(
			path.c_str(),
			GENERIC_WRITE,
			FILE_SHARE_READ| FILE_SHARE_WRITE,
			nullptr, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			nullptr
		);

		if (h == INVALID_HANDLE_VALUE) throw ERROR_CODE::FILE_CANNOT_OPEN;
		
		if (!FlushFileBuffers(h)) { 
			CloseHandle(h); 
			throw ERROR_CODE::FILE_CANNOT_OPEN; 
		}
		
		CloseHandle(h);
	
	#else
		int fd = open(path.c_str(), O_RDWR);
		
		if (fd == - 1) throw ERROR_CODE::FILE_CANNOT_OPEN;
		
		if (fsync(fd) != 0) {
			close(fd);
			throw ERROR_CODE::FILE_CANNOT_OPEN;
		}

		close(fd);

	#endif	
}

void JSON_DB::Sync_Directory(const string& path) {
	#ifndef _WIN32
		int fd = open(path.c_str(), O_RDONLY);

		if (fd == -1) return;

		fsync(fd);
		close(fd);

	#endif
}

// Read JSON from file
json JSON_DB::Read_JSON(const string& file_name) const {
	if (file_name.empty()) throw ERROR_CODE::FILE_PATH_EMPTY;

	if (!fs::exists(file_name)) {
		cerr << "\nWarning: FILE NOT FOUND [" << file_name << "]\n";
		return json::array();
	}

	ifstream file(file_name);
	if (!file.is_open()) throw ERROR_CODE::FILE_CANNOT_OPEN;

	json retval;
	file >> retval;
	file.close();

	return retval;
}

// Write JSON to file
void JSON_DB::Write_JSON(const string& file_name, const json& json_data) {
	if (file_name.empty()) throw ERROR_CODE::FILE_PATH_EMPTY;

	Ensure_Directory_Exists();

	fs::path target = file_name;
	fs::path temp_path = target;
	temp_path += ".tmp";
	{
		ofstream file(temp_path, std::ios::binary | std::ios::trunc);
		if (!file.is_open()) throw ERROR_CODE::FILE_CANNOT_OPEN;	
		
		file << json_data.dump(4);
		file.flush();
		if (!file.good()) throw ERROR_CODE::FILE_CANNOT_OPEN;
	}
	
	Sync_File(temp_path);

	std::error_code code;
	fs::rename(temp_path, target, code);
	if (code) throw ERROR_CODE::FILE_CANNOT_OPEN;

	Sync_Directory(
		target.parent_path().empty()
			? "."
			: target.parent_path().string()
	);
}

// Create directories if they don't exist
void JSON_DB::Ensure_Directory_Exists() const {
	if (!fs::exists(Table_Path)) {
		fs::create_directories(Table_Path);
	}
}

// Create a new table: schema + empty data file
bool JSON_DB::Create_Table(const json& schema) {
	lock_guard<mutex> lock(File_Mutex);

	Ensure_Directory_Exists();
	Write_JSON(Get_Schema_File(), schema);
	Write_JSON(Get_Data_File(), json::array());

	return true;
}

// Insert a record into the table
bool JSON_DB::Insert_Record(const json& record) {
	lock_guard<mutex> lock(File_Mutex);

	json schema = Read_JSON(Get_Schema_File());
	json data   = Read_JSON(Get_Data_File());

	for (auto& [key, type] : schema.items()) {
		if (!record.contains(key) || !Is_Type_Valid(record[key], type)) {
			cerr << "\nInvalid Data For: " << key << endl;
			return false;
		}
	}

	data.push_back(record);
	Write_JSON(Get_Data_File(), data);

	return true;
}

// Read all records from the table
json JSON_DB::Read_Records() const {
	lock_guard<mutex> lock(File_Mutex);
	return Read_JSON(Get_Data_File());
}
