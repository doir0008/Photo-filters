#include <windows.h>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

using std::wstring;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::wstringstream;

using byte = unsigned char;

class BMPImage
{
private:
	unique_ptr<BITMAPINFO> m_BitmapInfo;
	byte * m_ImageDataPtr;
	byte * m_BackupImageDataPtr;
	
	BITMAPFILEHEADER m_BFH;
	BITMAPINFOHEADER m_BIH;

	HANDLE m_FileHandle;
	wstring m_FileName;
	wstring m_Error;

	unsigned int m_Padding;
	unsigned int m_Pitch;

	bool SetupBitmapInfo ();
	void ReadBMPFile ();
	bool ReadBMPFileHeader();
	bool ReadBMPInfoHeader();
	unsigned int ReadBMPImageData();

	bool BackupImageData();
	bool RestoreImageData();

public:
	BMPImage( const wstring & fileName);
	~BMPImage();

	void PaintBMP (HDC hdc);

	//*******************************************************************************************************************

	wstring GetBitmapInformation();
	//*******************************************************************************************************************

	
	//*******************************************************************************************************************

	unsigned long WriteBmpFile( wstring fileName);
	//*******************************************************************************************************************
	
	
	
	// Image Processing Methods
	void BrightenImage(unsigned short amount);
	void DarkenImage(unsigned short amount);

	void Reset ();
	void BlackAndWhite ();
	void BlueChannel ();
	void GreenChannel ( );
	void RedChannel ( );
	void RemoveBlueChannel ();
	void RemoveGreenChannel ( );
	void RemoveRedChannel ( );

	
	//*******************************************************************************************************************
	// Student method declarations for image processing:
	// Make sure to rename these 4 methods with descriptive names

	void StudentMethod1(); // add arguments if needed
	void StudentMethod2(); // add arguments if needed
	void StudentMethod3(); // add arguments if needed
	void StudentMethod4(); // add arguments if needed

	//*******************************************************************************************************************
	
	
	
	//*******************************************************************************************************************
	//  Get methods for ALL the missing BITMAPINFOHEADER and BITMAPFILEHEADER data members
	//*******************************************************************************************************************
	
	// Inline get methods for BITMAPFILEHEADER 
	unsigned long GetImageFileSize ( ) { return m_BFH.bfSize; }
	
	// Inline get methods for BITMAPINFOHEADER 
	long GetImageWidth() { return m_BIH.biWidth; }
	long GetImageHeight() { return m_BIH.biHeight; }



	// Inline get methods for BITMAPFILEHEADER 
	unsigned long GetImageFileType() { return m_BFH.bfType; }
	unsigned long GetImageFileOffbits() { return m_BFH.bfOffBits; }
	unsigned long GetImageFileReserved1() { return m_BFH.bfReserved1; }
	unsigned long GetImageFileReserved2() { return m_BFH.bfReserved2; }


	// Inline get methods for BITMAPINFOHEADER 
	long GetImageSize() { return m_BIH.biSize; }
	long GetImageSizeImage() { return m_BIH.biSizeImage; }
	long GetImageBitCount() { return m_BIH.biBitCount; }
	long GetImageClrImportant() { return m_BIH.biClrImportant; }
	long GetImageClrUsed() { return m_BIH.biClrUsed; }
	long GetImageCompression() { return m_BIH.biCompression; }
	long GetImagePlanes() { return m_BIH.biPlanes; }
	long GetImageXPlesPerMeter() { return m_BIH.biXPelsPerMeter; }
	long GetImageYPelsPerMeter() { return m_BIH.biYPelsPerMeter; }

	//*******************************************************************************************************************


	// Additional Inline get methods
	// How do you get a pointer to the first element in an std::vector?
	// You can use &mv_vec[0] or &my_vec.front () however...
	// C++ 11 has vector.data () which has the benefit that the call is valid even if the vector is empty:
	unsigned char * GetImageDataPointer() { return m_ImageDataPtr; }
	wstring GetFileName() { return m_FileName; }
	wstring GetError() { return m_Error; }
};

