#include "FileDlg.h"

/*
	*	FileDlg.h and FileDlg.cpp
	*	Author@ Daniel E
	*	Data: 2012
	*	Link: https://www.gamedev.net/forums/topic/624356-c-and-getopenfilename/?do=findComment&comment=4936796
*/


CeFileDlg::CeFileDlg(void)
{
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.nMaxFile = MAXFILENAMESIZE;
	ofn.lpstrFileTitle = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrFilter = "\0";
}

CeFileDlg::~CeFileDlg(void)
{

}

void CeFileDlg::addFilter(std::string title, std::string extension)
{
	filter.append(title);
	filter.push_back('\0');
	filter.append("*");
	filter.append(extension);
	filter.push_back('\0');

	defExt.clear();
	defExt.append(extension);

	ofn.lpstrDefExt = &extension[0];
	ofn.lpstrFilter = &filter[0];
}

void CeFileDlg::clearFilter()
{
	filter = "";
	ofn.lpstrFilter = "\0";
}

bool CeFileDlg::showOpen(std::string& fname)
{
	return show(fname, true);
}

bool CeFileDlg::showSave(std::string& fname)
{
	return show(fname, false);
}

bool CeFileDlg::show(std::string& fname, bool open)
{
	TCHAR szFile[MAXFILENAMESIZE];

	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = L'\0';

	if (open)
	{
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON;
		GetOpenFileName(&ofn);
	}
	else
	{
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON;
		GetSaveFileName(&ofn);
	}

	fname = ofn.lpstrFile;
	return !fname.empty();
}