#pragma once
#include <cctype>
#include <string>
#include <vector>
#include <stdarg.h>
#include <algorithm>
#include <fstream>

class CUtils
{
public:
	CUtils();
	~CUtils();

	char* text_format(const char* formart, ...);

	int64_t Porcentage(int64_t current, int64_t max);

	bool is_file_exists(const std::string& filename);

	std::string to_string(const std::wstring& str, const std::locale& loc = std::locale{});

	std::string get_file_name(const std::string& fullfilepath);

	std::string get_file_extension(const std::string& fullfilename);

	std::string remove_file_extension(const std::string& fullfilename);

	std::string remove_file_from_path(const std::string& fullfilepath);

	void remove_duplicates_strings(std::vector<std::string>& vec);
private:

};