#pragma once

/// SYS API includes
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <set>

/// Resources
#include "resource.h"

/// Headers includes
#include "Utils/Utils.h"
#include "FileDlg/FileDlg.h"

extern "C" {
#include "ps2classic/ps2classic.h"
}

/// Libs calls
#pragma comment(lib, "ComCtl32.lib")

/// Files Format
#define ISO_FORMAT ".ISO"
#define BIN_FORMAT ".BIN"
#define ENC_FORMAT ".BIN.ENC"
#define DEFAULT_CONTENTID "2P0001-PS2U10000_00-0000111122223333"

/// Win32 Dialog vars
HWND hDlg;
MSG msg;
BOOL multi_files, ret;
HINSTANCE hInstance;
HANDLE hThread, hBarThread;
INT current_image, total_image;
TCHAR current_directory[MAX_PATH];

CUtils* Utils = new CUtils();