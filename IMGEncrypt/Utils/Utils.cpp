#include "Utils.h"

CUtils::CUtils()
{

}

CUtils::~CUtils()
{

}

char* CUtils::text_format(const char* formart, ...)
{
	char buffer[1024] = { 0 };

	//Variadic function
	va_list va_argList;
	va_start(va_argList, formart);
	_vsnprintf_s(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), 1024, formart, va_argList);
	va_end(va_argList);

	return buffer;
}

int64_t CUtils::Porcentage(int64_t current, int64_t max)
{
	current *= 100; //100%
	return (current / max);
}

bool CUtils::is_file_exists(const std::string& filename) {
	std::ifstream ifile(filename.c_str());
	return (bool)ifile;
}

std::string CUtils::to_string(const std::wstring& str, const std::locale& loc)
{
	std::vector<char> buf(str.size());
	std::use_facet<std::ctype<wchar_t>>(loc).narrow(str.data(), str.data() + str.size(), '?', buf.data());

	return std::string(buf.data(), buf.size());
}

std::string CUtils::get_file_name(const std::string& fullfilepath)
{
	//eg. "C:/Windows/System/File.dll", Result: "File.dll"

	const size_t lastSlashIndex = fullfilepath.find_last_of("/\\");
	return fullfilepath.substr(lastSlashIndex + 1);
}

std::string CUtils::get_file_extension(const std::string& fullfilename)
{
	//eg. "C:/Windows/System/File.dll", Result: ".dll"

	size_t lastindex = fullfilename.find_last_of(".");
	return fullfilename.substr(lastindex, fullfilename.length());
}

std::string CUtils::remove_file_extension(const std::string& fullfilename)
{
	//eg. "C:/Windows/System/File.dll", Result: "C:/Windows/System/File"

	size_t lastindex = fullfilename.find_last_of(".");
	return fullfilename.substr(0, lastindex);
}

std::string CUtils::remove_file_from_path(const std::string& fullfilepath)
{
	//eg. "C:/Windows/System/File.dll", Result: "C:/Windows/System/"

	return fullfilepath.substr(0, fullfilepath.find_last_of("\\/"));
}

void CUtils::remove_duplicates_strings(std::vector<std::string>& vec)
{
	std::sort(vec.begin(), vec.end());
	vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}