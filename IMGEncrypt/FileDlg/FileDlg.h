#pragma once
#include <Windows.h>
#include <string>

/*
	*	FileDlg.h and FileDlg.cpp
	*	Author@ Daniel E
	*	Data: 2012
	*	Link: https://www.gamedev.net/forums/topic/624356-c-and-getopenfilename/?do=findComment&comment=4936796
*/

#define MAXFILENAMESIZE 1024

class CeFileDlg
{
public:
	CeFileDlg(void);
	~CeFileDlg(void);

	bool showOpen(std::string& fname);

	bool showSave(std::string& fname);

	void addFilter(std::string title, std::string extension);

	void clearFilter();

private:
	bool show(std::string& fname, bool open);

	OPENFILENAMEA ofn;

	std::string	filter;

	std::string	defExt;
};