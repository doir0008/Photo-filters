#include "FileSaveClass.h"

FileSave::FileSave ( HWND windowHandle, wstring title ) : m_WindowHandle ( windowHandle ), m_Title ( title )
{
}

wstring FileSave::GetFileName ()
{
	HRESULT hr = 0;
	wstring fileName = L"";
	vector <COMDLG_FILTERSPEC> vFileTypes = { { L"Bitmap files", L"*.bmp" }, { L"All files", L"*.*" } };
	CComPtr <IFileSaveDialog> pDlg = nullptr;

	hr = pDlg.CoCreateInstance ( __uuidof( FileSaveDialog ) );	 // Create the Save Open dialog COM object

	if ( FAILED ( hr ) )
	{
		return fileName;
	}

	pDlg->SetTitle ( m_Title.c_str () );
	pDlg->SetDefaultExtension ( L"bmp" );
	pDlg->SetFileTypes ( vFileTypes.size (), &vFileTypes.front () );

	hr = pDlg->Show ( m_WindowHandle );	// Show the file save dialog

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