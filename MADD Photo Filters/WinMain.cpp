#include <windows.h>
#include <string>
#include <memory>
#include "resource.h"
#include "FileOpenClass.h"
#include "CBMP.h"
#include "FileSaveClass.h"

using std::wstring;
using std::unique_ptr;
using std::make_unique;

#pragma region GLOBAL_VARIABLES
// Store global handles to the main window, application and menu

HWND windowHandle = 0;
HINSTANCE applicationInstance = 0;
HMENU mainMenuHandle = 0;

// Set the initial position of your window
int windowXPosition = 200;
int windowYPosition = 200;

int windowWidth = 800;
int windowHeight = 600;

int windowClientWidth = 0;
int windowClientHeight = 0;

const wstring windowTitle = L"MADD Photo";
const wstring windowClassName = L"Standard Windows Application Template Class";

// global Unicode strings
wstring fileName = L"";

// global booleans for state information
bool displayInformationWhenOpened = false;
bool displayFileName = true;

// Additional Global Variables 
unique_ptr<BMPImage> theBMPImage = nullptr; // our smart pointer

#pragma endregion GLOBAL_VARIABLES

// The windows procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
#pragma region MENU_CODE

	case WM_COMMAND:
		// find the menu item
		switch (LOWORD(wParam))
		{
		case ID_OPTIONS_DISPLAYFILENAME:
			displayFileName = !displayFileName;
			CheckMenuItem(mainMenuHandle, wParam, (displayFileName) ? MF_CHECKED : MF_UNCHECKED);
			if (displayFileName)
			{
				SetWindowText(hwnd, fileName.c_str());
			}
			else
			{
				SetWindowText(hwnd, L"MADD Photo");
			}
			break;

		case ID_OPTIONS_DISPLAYINFORMATIONONOPEN:
			displayInformationWhenOpened = !displayInformationWhenOpened;
			CheckMenuItem(mainMenuHandle, wParam, (displayInformationWhenOpened) ? MF_CHECKED : MF_UNCHECKED);
			break;

		case ID_IMAGE_INFORMATION:
			MessageBox(hwnd, theBMPImage->GetBitmapInformation().c_str(), L"Bitmap Image Information", 0);
			break;

		case ID_FILE_OPEN:
			{
				FileOpen fo(hwnd, L"Open Bitmap File"); // Open the File Open Dialog Window
				fileName = fo.GetFileName();	 // get the selected file name and full path

				if (!fileName.empty())	 // this is empty if the user pressed cancel
				{
					theBMPImage = make_unique<BMPImage>(fileName);

					if (!theBMPImage->GetError().empty()) // not empty means we have an error!
					{
						MessageBox(hwnd, theBMPImage->GetError().c_str(), L"Error Message", 0);
						InvalidateRect(hwnd, 0, true); // paint the empty image so we have a blank screen
						return 0;
					}
					InvalidateRect(hwnd, 0, true); // paint the valid image

					EnableMenuItem(mainMenuHandle, ID_IMAGE_INFORMATION, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_FILE_REVERT, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_BLACKWHITE, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_BLUECHANNEL, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_BRIGHTEN, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_DARKEN, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_GREENCHANNEL, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_REDCHANNEL, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_REMOVEBLUECHANNEL, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_REMOVEGREENCHANNEL, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_REMOVE_REDCHANNEL, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER1, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER2, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER3, MF_ENABLED);
					EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER4, MF_ENABLED);

					if (displayFileName)
					{
						SetWindowText(hwnd, fileName.c_str());
					}

					if (displayInformationWhenOpened)
					{
						MessageBox(hwnd, theBMPImage->GetBitmapInformation().c_str(), L"Bitmap Image Information", 0);
					}
				}
			}
			break;

		case ID_FILE_SAVE:
		{
			FileSave fs(hwnd);  // Create a FileSave
			fileName = fs.GetFileName();  // Get the path to save the file

			if (!fileName.empty())	 // this is empty if the user didn't pressed cancel
			{
				theBMPImage->WriteBmpFile(fileName);  // Write the BMP to save the file

				if (!theBMPImage->GetError().empty()) // If there's an error
				{
					MessageBox(hwnd, theBMPImage->GetError().c_str(), L"Error Message", 0);
					return 0;
				}

				if (displayFileName)
				{
					SetWindowText(hwnd, fileName.c_str());  // Change the window text to the new file name
				}
			}
		}
		break;

		case ID_ADJUSTMENTS_BLACKWHITE:
			if (theBMPImage)
			{
				theBMPImage->BlackAndWhite();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_BRIGHTEN:
			if (theBMPImage)
			{
				theBMPImage->BrightenImage(50);
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_DARKEN:
			if (theBMPImage)
			{
				theBMPImage->DarkenImage(50);
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_BLUECHANNEL:
			if (theBMPImage)
			{
				theBMPImage->BlueChannel();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_GREENCHANNEL:
			if (theBMPImage)
			{
				theBMPImage->GreenChannel();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_REDCHANNEL:
			if (theBMPImage)
			{
				theBMPImage->RedChannel();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_REMOVEBLUECHANNEL:
			if (theBMPImage)
			{
				theBMPImage->RemoveBlueChannel();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_REMOVEGREENCHANNEL:
			if (theBMPImage)
			{
				theBMPImage->RemoveGreenChannel();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_REMOVE_REDCHANNEL:
			if (theBMPImage)
			{
				theBMPImage->RemoveRedChannel();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_STUDENTFILTER1:
			if (theBMPImage)
			{
				theBMPImage->StudentMethod1();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_STUDENTFILTER2:
			if (theBMPImage)
			{
				theBMPImage->StudentMethod2();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_STUDENTFILTER3:
			if (theBMPImage)
			{
				theBMPImage->StudentMethod3();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_ADJUSTMENTS_STUDENTFILTER4:
			if (theBMPImage)
			{
				theBMPImage->StudentMethod4();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_FILE_REVERT:
			if (theBMPImage)
			{
				theBMPImage->Reset();
				InvalidateRect(hwnd, 0, false);
			}
			break;

		case ID_FILE_EXIT:
			DestroyWindow(hwnd);
			break;
		}
		break;


#pragma endregion MENU_CODE	


#pragma region KEYBOARD_CODE	
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_DOWN:
			if (theBMPImage)
			{
				theBMPImage->DarkenImage(1);
				InvalidateRect(hwnd, 0, false);
			}
			return 0;

		case VK_UP:
			if (theBMPImage)
			{
				theBMPImage->BrightenImage(1);
				InvalidateRect(hwnd, 0, false);
			}
			return 0;

		case 'R':
			if (theBMPImage)
			{
				theBMPImage->Reset();
				InvalidateRect(hwnd, 0, false);
			}
			return 0;
		}
		break;

#pragma endregion KEYBOARD_CODE


#pragma region STANDARD_WINDOWS_MESSAGES_CODE

	case WM_PAINT:
		{
			PAINTSTRUCT paintstruct;
			HDC devicecontext = BeginPaint(hwnd, &paintstruct);
			if (theBMPImage && theBMPImage->GetError().empty()) // make sure we have a bitmap image to paint and that there is no error
			{
				theBMPImage->PaintBMP(devicecontext);
				// The function below re-sizes our window to the loaded Bitmap Dimensions
				SetWindowPos(hwnd, HWND_TOP, 0, 0, theBMPImage->GetImageWidth() + windowClientWidth, theBMPImage->GetImageHeight() + windowClientHeight, SWP_NOMOVE);
			}
			EndPaint(hwnd, &paintstruct);
		}
		return 0;

	case WM_CREATE:
		if (mainMenuHandle)	 // set up menu state
		{
			EnableMenuItem(mainMenuHandle, ID_IMAGE_INFORMATION, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_FILE_REVERT, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_BLACKWHITE, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_BLUECHANNEL, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_BRIGHTEN, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_DARKEN, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_GREENCHANNEL, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_REDCHANNEL, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_REMOVEBLUECHANNEL, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_REMOVEGREENCHANNEL, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_REMOVE_REDCHANNEL, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER1, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER2, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER3, MF_GRAYED);
			EnableMenuItem(mainMenuHandle, ID_ADJUSTMENTS_STUDENTFILTER4, MF_GRAYED);

			CheckMenuItem(mainMenuHandle, ID_OPTIONS_DISPLAYFILENAME, (displayFileName) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem(mainMenuHandle, ID_OPTIONS_DISPLAYINFORMATIONONOPEN, (displayInformationWhenOpened) ? MF_CHECKED : MF_UNCHECKED);
		}
		return 0;

	case WM_DESTROY: // Destroy the window must always be included
		PostQuitMessage(0);
		return 0;

#pragma endregion STANDARD_WINDOWS_MESSAGES_CODE

	}
	// send the messages we don't handle to default window procedure
	return DefWindowProc(hwnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int showCmd)
{
	// Save handle to application instance global variable.
	applicationInstance = hInstance;

	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIconSm = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = applicationInstance;

	wc.hIcon = LoadIcon(applicationInstance, MAKEINTRESOURCE(IDI_ICON1));

	wc.hCursor = LoadCursor(0, IDC_ARROW);

	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	wc.lpszMenuName = L"Main Menu";	// Optional, not required

	wc.lpszClassName = windowClassName.c_str();

	RegisterClassEx(&wc);

	// load the menu 
	mainMenuHandle = LoadMenu(applicationInstance, MAKEINTRESOURCE(IDR_MENU1));

	// Create the window
	windowHandle = CreateWindowEx(WS_EX_ACCEPTFILES,
		windowClassName.c_str(),
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		windowXPosition,
		windowYPosition,
		windowWidth,
		windowHeight,
		0,
		mainMenuHandle,
		applicationInstance, nullptr);

	if (!windowHandle)
	{
		MessageBox(0, L"CreateWindow Failed", L"Application Error Message", 0);
		return -1;
	}

	ShowWindow(windowHandle, showCmd);

	UpdateWindow(windowHandle);


	// Get the border and title bar/menu dimensions so we can accuratley resize our window in WM_PAINT
	//*******************************************************************************************************************
	RECT clientRectangle = {};
	RECT windowRectangle = {};
	GetClientRect(windowHandle, &clientRectangle);
	GetWindowRect(windowHandle, &windowRectangle);
	windowClientWidth = (windowRectangle.right - windowRectangle.left) - clientRectangle.right;
	windowClientHeight = (windowRectangle.bottom - windowRectangle.top) - clientRectangle.bottom;
	//*******************************************************************************************************************

	// Enter the message loop - quit when a WM_QUIT message is received
	MSG msg = {};

	int returnValue = 0; //  Used to check if GetMessage failed

	while ((returnValue = GetMessage(&msg, 0, 0, 0)) != 0) // exit loop if we get a quit message
	{
		if (-1 == returnValue)
		{
			// handle the error then break if desired
			MessageBox(windowHandle, L"GetMessage Failed!", L"Error Message", MB_OK);
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Return exit code to Windows
	return (int)msg.wParam;
}


