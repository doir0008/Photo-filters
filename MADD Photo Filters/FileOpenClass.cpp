// Tony Davidson 3 Jan 2017
// 
#include "FileOpenClass.h"

FileOpen::FileOpen ( HWND windowHandle, wstring title  ) : m_WindowHandle ( windowHandle ), m_Title (title )
{
}

wstring FileOpen::GetFileName ( )
{
	HRESULT hr = 0;
	wstring fileName = L"";
	vector <COMDLG_FILTERSPEC> vFileTypes = { { L"Bitmap files", L"*.bmp" }, { L"All files", L"*.*" } };
	CComPtr <IFileOpenDialog> pDlg = nullptr;
  
	hr = pDlg.CoCreateInstance ( __uuidof( FileOpenDialog ) );	 // Create the File Open dialog COM object

	if ( FAILED ( hr ) )
	{
		return fileName;
	}

	pDlg->SetTitle ( m_Title.c_str() );
	pDlg->SetFileTypes ( vFileTypes.size (), &vFileTypes.front () );

	hr = pDlg->Show ( m_WindowHandle );	// Show the file open dialog

	if ( SUCCEEDED ( hr ) )	   // If a file was selected return the full path and file name in a wstring
	{
		CComPtr<IShellItem> pItem;
		hr = pDlg->GetResult ( &pItem );

		if ( SUCCEEDED ( hr ) )
		{
			LPOLESTR pwsz = nullptr;
			hr = pItem->GetDisplayName ( SIGDN_FILESYSPATH, &pwsz );
			if ( SUCCEEDED ( hr ) )
			{
				fileName = pwsz;
				CoTaskMemFree ( pwsz );	   // clean up dynamically created COM string
			}
		}
	}

	return fileName;
}