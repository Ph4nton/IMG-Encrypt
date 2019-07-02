#include "WinMain.h"

/// Enable visual style
#pragma comment(linker,"\"/manifestdependency:type='Win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

struct ThreadParamrs
{
	INT mode;
	std::string content_id, key_path;
	std::vector<std::string> images_array, out_images_array;
} thread_paramrs;

void UpdateProgressBar(int nIDDlgItem, int porcentage, int max = 100)
{
	HWND progress_bar = GetDlgItem(hDlg, nIDDlgItem);

	if (!progress_bar) return;

	SendMessage(progress_bar, PBM_SETRANGE, 0, MAKELPARAM(0, max));

	SendMessage(progress_bar, PBM_SETPOS, porcentage, 0);
}

void HandleFiles(WPARAM wParam, std::vector<std::string>& vec)
{
	// DragQueryFile() takes a LPWSTR for the name so we need a TCHAR string
	TCHAR szName[MAX_PATH];

	// Here we cast the wParam as a HDROP handle to pass into the next functions
	HDROP hDrop = (HDROP)wParam;

	// This functions has a couple functionalities.  If you pass in 0xFFFFFFFF in
	// the second parameter then it returns the count of how many filers were drag
	// and dropped.  Otherwise, the function fills in the szName string array with
	// the current file being queried.
	int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

	// Here we go through all the files that were drag and dropped then display them
	for (int i = 0; i < count; i++)
	{
		// Grab the name of the file associated with index "i" in the list of files dropped.
		// Be sure you know that the name is attached to the FULL path of the file.
		DragQueryFile(hDrop, i, szName, MAX_PATH);

		/// Add item to a array
		vec.push_back(szName);

		/// Add item to a list
		SendMessage(GetDlgItem(hDlg, IDC_PATH_LIST), LB_ADDSTRING, 0, (LPARAM)szName);
	}

	// Finally, we destroy the HDROP handle so the extra memory
	// allocated by the application is released.
	DragFinish(hDrop);
}

void encrypt_thread(LPVOID lpParameter)
{
	time_t prg_begin, prg_end;

	ThreadParamrs *paramrs = (ThreadParamrs*)lpParameter;

	total_image = paramrs->images_array.size();

	/// set game key licensee
	ps2_set_keylicensee(paramrs->key_path.data());

	/// time start
	prg_begin = clock();

	/// start encrypt images
	for (size_t i = 0; i < paramrs->images_array.size(); i++)
	{
		current_image = i + 1;

		/// call ps2 encrypt function
		ps2_encrypt_image
		(
			paramrs->mode, 
			(char*)paramrs->images_array.at(i).data(), 
			(char*)paramrs->out_images_array.at(i).data(), 
			(char*)paramrs->out_images_array.at(i).data(),
			(char*)paramrs->content_id.data()
		);
	}
	
	/// time end
	prg_end = clock();

	/// Wait for finish bar animated
	WaitForSingleObject(hBarThread, INFINITE);

	/// Finish
	MessageBeep(MB_OK);

	/// Show finish menssage
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_FINISH), SW_SHOW);

	/// calcule diference
	double enlapsed_time = (double)(prg_end - prg_begin) / (double)CLK_TCK;

	/// Set finish enlapsed time text
	SetDlgItemText(hDlg, IDC_STATIC_FINISH, Utils->text_format(TEXT("Finish %.2f sec"), enlapsed_time));
}

void decrypt_thread(LPVOID lpParameter)
{
	time_t prg_begin, prg_end;

	ThreadParamrs *paramrs = (ThreadParamrs*)lpParameter;

	total_image = paramrs->images_array.size();

	/// set game key licensee
	ps2_set_keylicensee(paramrs->key_path.data());

	/// time start
	prg_begin = clock();

	/// start encrypt images
	for (size_t i = 0; i < paramrs->images_array.size(); i++)
	{
		current_image = i + 1;

		/// call ps2 encrypt function
		ps2_decrypt_image
		(
			paramrs->mode,
			(char*)paramrs->images_array.at(i).data(),
			(char*)paramrs->out_images_array.at(i).data(),
			(char*)paramrs->out_images_array.at(i).data()
		);
	}

	/// time end
	prg_end = clock();

	/// Wait for finish bar animated
	WaitForSingleObject(hBarThread, INFINITE);

	/// Finish
	MessageBeep(MB_OK);

	/// Show finish menssage
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_FINISH), SW_SHOW);

	/// calcule diference
	double enlapsed_time = (double)(prg_end - prg_begin) / (double)CLK_TCK;

	/// Set finish enlapsed time text
	SetDlgItemText(hDlg, IDC_STATIC_FINISH, Utils->text_format(TEXT("Finish %.2f sec"), enlapsed_time));
}

void update_dialog()
{
	/// set progress bar to start
	UpdateProgressBar(IDC_PROGRESS1, 0);

	/// Show progress menssage
	ShowWindow(GetDlgItem(hDlg, IDC_PROGRESS_TEXT), SW_SHOW);

	while (true)
	{
		/// update progress bar
		UpdateProgressBar(IDC_PROGRESS1, porcentage);

		/// Set finish enlapsed time text
		SetDlgItemText(hDlg, IDC_PROGRESS_TEXT, Utils->text_format(TEXT("%i of %i"), current_image, total_image));

		/// if finish, break loop
		if (porcentage == 100 && current_image == total_image)
			break;

		/// Reduce cpu usage
		Sleep(100);
	}

	/// Wait progress animate
	Sleep(500);
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static INT mode;
	static std::string content_id, key_path;

	switch (uMsg)
	{
	///Initialize dialog
	case WM_INITDIALOG:
	{
		/// Set CEX Radio Button as true
		SendMessage(GetDlgItem(hDlg, IDC_CEX_RADIO), BM_SETCHECK, TRUE, 0);

		/// Set content ID to as default
		SetDlgItemText(hDlg, IDC_CONTENTID_EDIT, DEFAULT_CONTENTID);

		/// Update files number
		SetDlgItemText(hDlg, IDC_FILE_COUNT, Utils->text_format(TEXT("%i Files"), thread_paramrs.images_array.size()));

		/// Add images to a list
		if(!thread_paramrs.images_array.empty())
			for (size_t i = 0; i < thread_paramrs.images_array.size(); i++)
				SendMessage(GetDlgItem(hDlg, IDC_PATH_LIST), LB_ADDSTRING, 0, (LPARAM)thread_paramrs.images_array.at(i).data());

		/// Load bitmap from resource
		HBITMAP hBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 30, 30, LR_DEFAULTCOLOR);

		/// Set bitmap to a button
		SendDlgItemMessage(hDlg, IDC_IMGPATH_BUTTON, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

		/// release bitmap
		DeleteObject((HBITMAP)hBitmap);

		/// Set key licensee path
		key_path = std::string(current_directory) + TEXT("\\ps2.key");

		return TRUE;
	}
	case WM_DROPFILES:
	{
		/// Drag&Drop handle
		HandleFiles(wParam, thread_paramrs.images_array);

		/// Update files number
		SetDlgItemText(hDlg, IDC_FILE_COUNT, Utils->text_format(TEXT("%i Files"), thread_paramrs.images_array.size()));

		break;
	}
	case WM_COMMAND:
	{
		/*if (IsDlgButtonChecked(hDlg, IDC_CEX_RADIO) == BST_CHECKED)*/

		switch (LOWORD(wParam))
		{
		case IDC_IMGPATH_BUTTON:
		{
			std::string image_path;
			CeFileDlg img_file_dialog;

			/// create dialog filters
			img_file_dialog.addFilter(TEXT("All supported files"), TEXT(".ISO;*.BIN;*.BIN.ENC"));
			img_file_dialog.addFilter(TEXT("ISO File (*.ISO)"), ISO_FORMAT);
			img_file_dialog.addFilter(TEXT("BIN/CUE File (*.BIN)"), BIN_FORMAT);
			img_file_dialog.addFilter(TEXT("Encrypt File (*.BIN.ENC)"), ENC_FORMAT);
			img_file_dialog.addFilter(TEXT("All"), TEXT(".*"));

			/// if cancel dialog, return
			if (!img_file_dialog.showOpen(image_path))
				return TRUE;

			/// add image to array
			thread_paramrs.images_array.push_back(image_path);

			/// Add path to a list
			SendMessage(GetDlgItem(hDlg, IDC_PATH_LIST), LB_ADDSTRING, 0, (LPARAM)image_path.data());

			/// Update files number
			SetDlgItemText(hDlg, IDC_FILE_COUNT, Utils->text_format(TEXT("%i Files"), thread_paramrs.images_array.size()));

			return TRUE;
		}
		case IDC_CEX_RADIO:
		{
			mode = CEX;
			return TRUE;
		}
		case IDC_DEX_RADIO:
		{
			mode = DEX;
			return TRUE;
		}
		case IDC_CONTENTID_EDIT:
		{
			char sz_buffer[MAX_PATH];
			GetDlgItemText(hDlg, IDC_CONTENTID_EDIT, sz_buffer, _countof(sz_buffer));
			content_id = std::string(sz_buffer);
			return TRUE;
		}
		case IDC_ENCRYPT_BUTTON:
		{
			/// Hide finish menssage
			ShowWindow(GetDlgItem(hDlg, IDC_STATIC_FINISH), SW_HIDE);

			/// Check if image is selected
			if (thread_paramrs.images_array.empty()) {
				MessageBox(hDlg, TEXT("Select a image (.BIN.ENC) and try again"), TEXT("Image not selected!"), MB_ICONINFORMATION);
				return TRUE;
			}

			/// Remove duplicate strings if exist
			Utils->remove_duplicates_strings(thread_paramrs.images_array);

			/// Check if file exists
			for (size_t i = 0; i < thread_paramrs.images_array.size(); i++)
			{
				/// Get file name
				std::string file_extension = Utils->get_file_extension(thread_paramrs.images_array.at(i));

				/// Supported_files
				std::set<std::string> supported_files = { ISO_FORMAT, BIN_FORMAT, ".iso", ".bin"};

				/// Check if iterator it is valid
				if (std::count(supported_files.begin(), supported_files.end(), file_extension) == 0)
				{
					/// want to continue?
					if (MessageBox(hDlg, Utils->text_format(TEXT("Outpuf file \"%s\" is not a .ISO or .BIN\nDo you want to continue anyway?"), Utils->get_file_name(thread_paramrs.images_array.at(i)).data()),
						TEXT("File format not supported!"), MB_ICONWARNING | MB_YESNO) == IDNO)
						return TRUE;
				}

				printf("%s\n", thread_paramrs.images_array.at(i).data());

				/// check if image exist
				if (Utils->is_file_exists(thread_paramrs.images_array.at(i)))
				{
					/// create a output file name
					std::string output_file_image = Utils->remove_file_extension(thread_paramrs.images_array.at(i)) += ENC_FORMAT;

					/// check if output file already exists
					if (Utils->is_file_exists(output_file_image))
					{
						/// if select 'No' continue to the next item
						if (MessageBox(hDlg, 
							Utils->text_format(TEXT("Outpuf file \"%s\" already exists!\nDo you want to replace?"), Utils->get_file_name(output_file_image).data()), 
							TEXT("File Already Exists!"), MB_ICONWARNING | MB_YESNO) == IDNO)
							return TRUE;
					}

					/// push back output path to array
					thread_paramrs.out_images_array.push_back(output_file_image);
				}
				else
				{
					/// Error if file not exist
					MessageBox(hDlg, std::string("The file " + thread_paramrs.images_array.at(i) + " not found!").data(), "Image Error", MB_ICONERROR);
					return TRUE;
				}
			}

			/// Check content id
			if (content_id.empty())
			{
				MessageBox(hDlg, TEXT("Content ID can not be null!"), TEXT("Content ID is Null"), MB_ICONINFORMATION);
				return TRUE;
			}

			/// Check key licensee
			if (!Utils->is_file_exists(key_path))
			{
				MessageBox(hDlg, TEXT("Key Licensee not found!\nMake sure the \"ps2.key\" file is in this directory"), TEXT("Key Licensee Error"), MB_ICONERROR);
				return TRUE;
			}

			/// Create parameters
			thread_paramrs.mode = mode;
			thread_paramrs.content_id = content_id;
			thread_paramrs.key_path = key_path;

			//Ps. you can only use one thread, but this affects the performance
			/// Create encrypt thread
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)encrypt_thread, &thread_paramrs, 0, 0);

			/// Create update progrress bar thread
			hBarThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)update_dialog, NULL, 0, 0);

			/// Set hThread to Highest 
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);

			return TRUE;
		}
		case IDC_DECRYPT_BUTTON:
		{
			/// Hide finish menssage
			ShowWindow(GetDlgItem(hDlg, IDC_STATIC_FINISH), SW_HIDE);

			/// Check if image is selected
			if (thread_paramrs.images_array.empty()) {
				MessageBox(hDlg, TEXT("Select a image (.ISO or .BIN) and try again"), TEXT("Image not selected!"), MB_ICONINFORMATION);
				return TRUE;
			}

			/// Remove duplicate strings if exist
			Utils->remove_duplicates_strings(thread_paramrs.images_array);

			/// Check if file exists
			for (size_t i = 0; i < thread_paramrs.images_array.size(); i++)
			{
				/// Get file name
				std::string file_extension = Utils->get_file_extension(thread_paramrs.images_array.at(i));

				/// Supported_files
				std::set<std::string> supported_files = { ".ENC", ".enc" };

				/// Check if iterator it is valid
				if (std::count(supported_files.begin(), supported_files.end(), file_extension) == 0)
				{
					/// want to continue?
					if (MessageBox(hDlg, Utils->text_format(TEXT("Outpuf file \"%s\" is not a .BIN.ENC\nDo you want to continue anyway?"), Utils->get_file_name(thread_paramrs.images_array.at(i)).data()),
						TEXT("File format not supported!"), MB_ICONWARNING | MB_YESNO) == IDNO)
						return TRUE;
				}

				/// check if image exist
				if (Utils->is_file_exists(thread_paramrs.images_array.at(i)))
				{
					/// create a output file name
					std::string output_file_image = Utils->remove_file_extension(thread_paramrs.images_array.at(i));
					output_file_image = Utils->remove_file_extension(output_file_image) += ISO_FORMAT;

					/// check if output file already exists
					if (Utils->is_file_exists(output_file_image))
					{
						/// if select 'No' continue to the next item
						if (MessageBox(hDlg,
							Utils->text_format(TEXT("Outpuf file \"%s\" already exists!\nDo you want to replace?"), Utils->get_file_name(output_file_image).data()),
							TEXT("File Already Exists!"), MB_ICONWARNING | MB_YESNO) == IDNO)
							return TRUE;
					}

					/// push back output path to array
					thread_paramrs.out_images_array.push_back(output_file_image);
				}
				else
				{
					/// Error if file not exist
					MessageBox(hDlg, std::string("The file " + thread_paramrs.images_array.at(i) + " not found!").data(), TEXT("Image Error"), MB_ICONERROR);
					return TRUE;
				}
			}

			/// Check key licensee
			if (!Utils->is_file_exists(key_path))
			{
				MessageBox(hDlg, TEXT("Key Licensee not found!\nMake sure the \"ps2.key\" file is in this directory"), TEXT("Key Licensee Error"), MB_ICONERROR);
				return TRUE;
			}

			/// Create parameters
			thread_paramrs.mode = mode;
			thread_paramrs.content_id = '\0';
			thread_paramrs.key_path = key_path;

			//Ps. you can only use one thread, but this affects the performance
			/// Create encrypt thread
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)decrypt_thread, &thread_paramrs, 0, 0);

			/// Create update progrress bar thread
			hBarThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)update_dialog, NULL, 0, 0);

			/// Set hThread to Highest 
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);

			return TRUE;
		}
		break;
		}
		break;
	}
	case WM_CLOSE:
		DestroyWindow(hDlg);
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}

std::streampos file_size(const std::string& file)
{
	std::ifstream iffile(file.c_str(), std::ios::binary | std::ios::ate);
	return iffile.tellg();
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
	int argCount;
	LPWSTR *szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);

	if (szArgList[1])
	{
		if (wcscmp(szArgList[1], L"-debug") == 0) {
			AllocConsole();
			freopen("conout$", "w", stdout);
		}

		for (int i = 1; i < argCount; i++)
		{
			std::string Arg_path = Utils->to_string(szArgList[i]);

			if (Utils->is_file_exists(Arg_path)) {
				thread_paramrs.images_array.push_back(Arg_path);
				printf("File Size: %x\n", file_size(Arg_path));
			}
		}

		LocalFree(szArgList);
	}

	/*cex ps2.key Game.ISO ISO.BIN.ENC ISO.BIN.ENC 2P0001-PS2U10000_00-0000111122223333*/

	hInstance = hInst;

	GetCurrentDirectory(MAX_PATH, current_directory);

	InitCommonControls();

	hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_MAIN), 0, DialogProc, 0);

	ShowWindow(hDlg, nCmdShow);

	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret == -1)
			return -1;

		if (!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}