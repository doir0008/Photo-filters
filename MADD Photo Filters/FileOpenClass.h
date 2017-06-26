// Tony Davidson 3 Jan 2017
// 
#ifndef FILEOPENCLASS_H
#define FILEOPENCLASS_H

#include <windows.h>
#include <string>
#include <atlbase.h>	  // for ATL
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <vector>

using std::wstring;
using std::vector;

class FileOpen
{
private:
	HWND m_WindowHandle;
	wstring m_Title;

public:
	FileOpen (HWND windowHandle, wstring title = L"Open File");
	wstring GetFileName ();
};

#endif