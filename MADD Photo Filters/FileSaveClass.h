// Tony Davidson 3 Jan 2017
// 
#ifndef FILESAVECLASS_H
#define FILESAVECLASS_H

#include <windows.h>
#include <string>
#include <atlbase.h>	  // for ATL
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <vector>

using std::wstring;
using std::vector;

class FileSave
{
private:
	HWND m_WindowHandle;
	wstring m_Title;

public:
	FileSave ( HWND windowHandle, wstring title = L"Save File" );
	wstring GetFileName ();
};

#endif