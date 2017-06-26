#include "CBMP.h"

BMPImage::BMPImage(const wstring & fileName)
	: m_BitmapInfo(nullptr), m_ImageDataPtr(nullptr), m_BackupImageDataPtr(nullptr), m_BFH({}), m_BIH({}), m_FileHandle(0), m_FileName(fileName), m_Padding(0), m_Pitch(0)
{
	if (!m_FileName.empty())
	{
		ReadBMPFile();
	}
}

BMPImage::~BMPImage()
{
	if (m_ImageDataPtr != nullptr)
	{
		delete[] m_ImageDataPtr;
		m_ImageDataPtr = nullptr;
	}
	if (m_BackupImageDataPtr != nullptr)
	{
		delete[] m_BackupImageDataPtr;
		m_BackupImageDataPtr = nullptr;
	}
}

void BMPImage::Reset()
{
	m_Error = L"";

	if (m_BackupImageDataPtr != nullptr)
	{
		RestoreImageData();
	}
	else // if there is no original image data then reload the image
	{
		m_BitmapInfo = nullptr;
		ReadBMPFile();
	}
}

#pragma region FILE_INPUT_OUTPUT_METHODS

void BMPImage::ReadBMPFile()
{
	m_BitmapInfo = nullptr;

	if (m_ImageDataPtr != nullptr)
	{
		delete[] m_ImageDataPtr;
		m_ImageDataPtr = nullptr;
	}

	if (ReadBMPFileHeader())
	{
		if (ReadBMPInfoHeader())
		{
			m_ImageDataPtr = new byte[m_BIH.biSizeImage];
			if (m_ImageDataPtr != nullptr)
			{
				if (ReadBMPImageData() != m_BIH.biSizeImage)
				{
					m_Error = L"Unable to read all image data";
				}
				else
				{
					SetupBitmapInfo();
				}
			}
			else
			{
				m_Error = L"Unable to allocate memory for image";
			}
		}
		else
		{
			m_Error = L"Unable to read BMP information";
		}
	}
	else
	{
		m_Error = L"Invalid file format";
	}
}

bool BMPImage::ReadBMPFileHeader()
{
	unsigned long numberOfBytesRead = 0;
	// Open Bitmap file
	m_FileHandle = CreateFile(m_FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, 0);
	// Read Bitmap file header
	ReadFile(m_FileHandle, &m_BFH, sizeof(m_BFH), &numberOfBytesRead, 0);

	if (numberOfBytesRead != sizeof(m_BFH) || m_BFH.bfType != 19778) // 19778 = "BM"
	{
		return false;
	}
	return true;
}

bool BMPImage::ReadBMPInfoHeader()
{
	unsigned long numberOfBytesRead = 0;

	// Read Bitmap info header
	ReadFile(m_FileHandle, &m_BIH, sizeof(m_BIH), &numberOfBytesRead, 0);

	if (numberOfBytesRead != sizeof(m_BIH) || m_BIH.biBitCount != 24)
	{
		m_Error = L"BMP Image Format not supported";
		return false;
	}
	// sometimes the width and height are incorrectly saved as negative values
	if (m_BIH.biHeight < 0)
	{
		m_BIH.biHeight *= -1;
	}
	if (m_BIH.biWidth < 0)
	{
		m_BIH.biWidth *= -1;
	}
	return true;
}


bool BMPImage::BackupImageData()
{
	// do we have any image data?
	if (m_ImageDataPtr == nullptr)
	{
		return false; // no so nothing to do!
	}

	// free up any memory used by the backup image 
	if (m_BackupImageDataPtr != nullptr)
	{
		delete[] m_BackupImageDataPtr;
		m_BackupImageDataPtr = nullptr;
	}
	
	// allocate memory for the backup image
	m_BackupImageDataPtr = new byte[m_BIH.biSizeImage];

	// pointer copy 
	byte * sourcePtr = m_ImageDataPtr;
	byte * destinationPtr = m_BackupImageDataPtr;

	for (unsigned int c = 0; c < m_BIH.biSizeImage; c++)
	{
		*destinationPtr = *sourcePtr;
		destinationPtr++;
		sourcePtr++;
	}
	return true;
}

bool BMPImage::RestoreImageData()
{
	// do we have any backup image data?
	if (m_BackupImageDataPtr == nullptr)
	{
		return false; // no so nothing to do!
	}

	// free up any memory used by the original image 
	if (m_ImageDataPtr != nullptr)
	{
		delete[] m_ImageDataPtr;
		m_ImageDataPtr = nullptr;
	}

	// allocate memory for the backup image
	m_ImageDataPtr = new byte[m_BIH.biSizeImage];

	// pointer copy 
	byte * sourcePtr = m_BackupImageDataPtr;
	byte * destinationPtr = m_ImageDataPtr;

	for (unsigned int c = 0; c < m_BIH.biSizeImage; c++)
	{
		*destinationPtr = *sourcePtr;
		destinationPtr++;
		sourcePtr++;
	}
	return true;
}


unsigned int BMPImage::ReadBMPImageData()
{
	unsigned long numberOfBytesRead = 0;
	// Read Bitmap image data
	ReadFile(m_FileHandle, m_ImageDataPtr, m_BIH.biSizeImage, &numberOfBytesRead, nullptr);
	CloseHandle(m_FileHandle);

	BackupImageData();

	return numberOfBytesRead;
}

unsigned long  BMPImage::WriteBmpFile(wstring fileName)
{
	unsigned long numberOfBytesWritten = 0;

	HANDLE fileHandle = CreateFile(fileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, 0, 0);
	WriteFile(fileHandle, &m_BFH, sizeof(m_BFH), &numberOfBytesWritten, nullptr);
	WriteFile(fileHandle, &m_BIH, sizeof(m_BIH), &numberOfBytesWritten, nullptr);
	WriteFile(fileHandle, m_ImageDataPtr, m_BIH.biSizeImage, &numberOfBytesWritten, nullptr);
	CloseHandle(fileHandle);

	return numberOfBytesWritten;
}

#pragma endregion FILE_INPUT_OUTPUT_METHODS

bool BMPImage::SetupBitmapInfo()
{
	m_BitmapInfo = make_unique<BITMAPINFO>();

	m_BitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BitmapInfo->bmiHeader.biWidth = m_BIH.biWidth;
	m_BitmapInfo->bmiHeader.biHeight = m_BIH.biHeight;
	m_BitmapInfo->bmiHeader.biPlanes = 1;
	m_BitmapInfo->bmiHeader.biBitCount = (WORD)m_BIH.biBitCount;
	m_BitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_BitmapInfo->bmiHeader.biSizeImage = 0;
	m_BitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_BitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_BitmapInfo->bmiHeader.biClrUsed = 0;
	m_BitmapInfo->bmiHeader.biClrImportant = 0;

	// Calculate pitch and padding
	// The number of bytes per row is referred to as the pitch, the number of pixels per row is the width.
	m_Pitch = ((m_BIH.biWidth * m_BIH.biBitCount) + 31) / 32 * 4;
	m_Padding = m_Pitch - (m_BIH.biWidth * 3);   // bytes per pixel

	return true;
}

void BMPImage::PaintBMP(HDC hdc)
{
	if (m_ImageDataPtr != nullptr)
	{
		if (m_BIH.biWidth > 0 && m_BIH.biHeight > 0)
		{
			SetDIBitsToDevice(
				hdc,		    // handle to the device context
				0,				// The x-coordinate, in logical units, of the upper-left corner of the destination rectangle
				0,				// The y-coordinate, in logical units, of the upper-left corner of the destination rectangle
				m_BIH.biWidth,	// The width, in logical units, of the image
				m_BIH.biHeight,	// The height, in logical units, of the image
				0,				// The x-coordinate, in logical units, of the lower-left corner of the image
				0,			    // The y-coordinate, in logical units, of the lower-left corner of the image
				0,				// starting scan line in the image
				m_BIH.biHeight,	// number of DIB scan lines contained in the image data	  
				m_ImageDataPtr, // the image bytes
				m_BitmapInfo.get(),   // the address of the BITMAPINFO struct NOTE: Not the same as a BITMAPINFOHEADER
				DIB_RGB_COLORS); // set for explicit RGB values not palette colours
		}
	}
}

//*******************************************************************************************************************

wstring BMPImage::GetBitmapInformation()
{
	wstringstream sout;
	sout << "File Information:\n";

	
	sout << "\nImage Type = " << m_BFH.bfType;
	sout << "\nImage Size = " << m_BIH.biSizeImage;

	// add code from your assignment 3 here


	sout << L"\nSize: " << m_BFH.bfSize;
	sout << "\nImage Type: " << m_BFH.bfType;
	sout << "\nReserved 1: " << m_BFH.bfReserved1;
	sout << "\nReserved 2: " << m_BFH.bfReserved2;
	sout << "\nOffbits: " << m_BFH.bfOffBits;

	sout << "\n\nImage Infromation: \n";
	sout << "\nSize(bytes): " << m_BIH.biSize;
	sout << "\nImage Size(bytes): " << m_BIH.biSizeImage;
	sout << "\nImage Width(pixels): " << m_BIH.biWidth;
	sout << "\nImage Height(pixels): " << m_BIH.biHeight;
	sout << "\nNumber of bits per pixel: " << m_BIH.biBitCount;
	sout << "\nNumber of required color indexes: " << m_BIH.biClrImportant;
	sout << "\nNumber of color indexes used: " << m_BIH.biClrUsed;
	sout << "\nImage Compression Type: " << m_BIH.biCompression;
	sout << "\nNumber of planes: " << m_BIH.biPlanes;
	sout << "\nHorizontal Resolution of the device(pixels-per-meter): " << m_BIH.biXPelsPerMeter;
	sout << "\nVertical Resolution of the device(pixels-per-meter): " << m_BIH.biYPelsPerMeter;

	sout << "\n\nImage Data: \n";
	sout << "\nImage Data located at address: " << m_ImageDataPtr;

	return sout.str();
}
//*******************************************************************************************************************





#pragma region IMAGE_PROCESSING_METHODS

void BMPImage::BrightenImage(unsigned short amount)
{
	if (amount > 255)
	{
		amount = 255;
	}

	byte * tempImagePtr = m_ImageDataPtr; // ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	for (unsigned int i = 0; i < m_BIH.biSizeImage; i++)
	{
		*tempImagePtr += amount;
		if (*tempImagePtr < amount)
		{
			*tempImagePtr = 255;
		}
		tempImagePtr++;
	}
}

void BMPImage::DarkenImage(unsigned short amount)
{
	if (amount > 255)
	{
		amount = 255;
	}

	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	for (unsigned int i = 0; i < m_BIH.biSizeImage; i++)
	{
		if (*tempImagePtr > amount)
		{
			*tempImagePtr -= amount;
		}
		else
		{
			*tempImagePtr = 0;
		}
		tempImagePtr++;
	}
}

void BMPImage::BlueChannel()
{
	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++)
		{
			tempImagePtr++;   // skip the blue byte
			*tempImagePtr = 0; // clear the green byte
			tempImagePtr++;   // go to red byte
			*tempImagePtr = 0; // clear the red byte
			tempImagePtr++; // ready for next BGR triple
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::GreenChannel()
{
	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++)
		{
			*tempImagePtr = 0; // clear the blue byte
			tempImagePtr += 2; // go to red byte
			*tempImagePtr = 0; // clear the red byte
			tempImagePtr++; // ready for next BGR triple
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::RedChannel()
{
	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++)
		{
			*tempImagePtr = 0; // clear the blue byte
			tempImagePtr++; // go to blue byte
			*tempImagePtr = 0; // clear the blue byte
			tempImagePtr += 2; // ready for next BGR triple
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::RemoveBlueChannel()
{
	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++)
		{
			*tempImagePtr = 0;
			tempImagePtr += 3;
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::RemoveGreenChannel()
{

	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	int counter = 0;
	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		counter = m_Pitch * row;
		for (int col = 0; col < m_BIH.biWidth; col++)
		{
			tempImagePtr++;
			*tempImagePtr = 0;
			tempImagePtr += 2;
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::RemoveRedChannel()
{

	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:
	int counter = 0;
	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		counter = m_Pitch * row;
		for (int col = 0; col < m_BIH.biWidth; col++)
		{
			tempImagePtr += 2;
			*tempImagePtr = 0;
			tempImagePtr++;
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::BlackAndWhite()
{

	byte * tempImagePtr = m_ImageDataPtr;// ALWAYS create a temporary pointer before modifying the image data

	// only use the temporary pointer in the image processing code:

	// Formula for monochrome from RGB = 0.2125 * RED + 0.7154 * GREEN + 0.0721 * BLUE
	float GRAY = 0.0f;
	float RED = 0.0f;
	float GREEN = 0.0f;
	float BLUE = 0.0f;

	byte pixelData = 0;

	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++)	// cycle through the pitch: start at zero so one less than the width
		{
			BLUE = *tempImagePtr;
			tempImagePtr++;
			GREEN = *tempImagePtr;
			tempImagePtr++;
			RED = *tempImagePtr;

			tempImagePtr -= 2; // back to the blue pixel

			GRAY = (0.2125f * RED) + (0.7154f * GREEN) + (0.0721f * BLUE);
			pixelData = (byte)(GRAY + 0.5f); // round up
			*tempImagePtr = pixelData;
			tempImagePtr++;
			*tempImagePtr = pixelData;
			tempImagePtr++;
			*tempImagePtr = pixelData;
			tempImagePtr++;
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::StudentMethod1()
{
	// INVERT PHOTO
	// Invert each color channel
	// This method was demonstrated in class

	byte * tempImagePtr = m_ImageDataPtr; // ALWAYS create a temporary pointer before modifying the image data

	// invert the photo (photo negative), just going through the image one byte at a time
	for (int i = 0; i < m_BIH.biSizeImage; i++)
	{
		*tempImagePtr = 255 - *tempImagePtr;
		tempImagePtr++;
	}

}

void BMPImage::StudentMethod2()
{
	// SEPIA TONE
	// Convert the image to grayscale, then apply the sepia tone
	// Sources:
	// http://stackoverflow.com/questions/1061093/how-is-a-sepia-tone-created
	// http://www.cplusplus.com/forum/general/16974/

	byte * tempImagePtr = m_ImageDataPtr; // ALWAYS create a temporary pointer before modifying the image data

	float GRAY = 0.0f;
	float RED = 0.0f;
	float GREEN = 0.0f;
	float BLUE = 0.0f;

	float outputRed = 0.0f;
	float outputGreen = 0.0f;
	float outputBlue = 0.0f;

	byte pixelData = 0;

	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++)	// cycle through the pitch: start at zero so one less than the width
		{
			BLUE = *tempImagePtr;
			tempImagePtr++;
			GREEN = *tempImagePtr;
			tempImagePtr++;
			RED = *tempImagePtr;

			tempImagePtr -= 2; // back to the blue pixel

			GRAY = (0.2125f * RED) + (0.7154f * GREEN) + (0.0721f * BLUE);

			outputRed = GRAY * ((RED * 0.393f) + (GREEN * 0.769f) + (BLUE * 0.189f)) / 255;
			outputGreen = GRAY * ((RED * 0.349f) + (GREEN * 0.686f) + (BLUE * 0.168f)) / 255;
			outputBlue = GRAY * ((RED * 0.272f) + (GREEN * 0.534f) + (BLUE * 0.131)) / 255;

			if (outputRed > 255)
			{
				outputRed = 255;
			}
			if (outputGreen > 255)
			{
				outputGreen = 255;
			}
			if (outputBlue > 255)
			{
				outputBlue = 255;
			}

			pixelData = (byte)outputBlue;
			*tempImagePtr = pixelData;
			tempImagePtr++;

			pixelData = (byte)outputGreen;
			*tempImagePtr = pixelData;
			tempImagePtr++;

			pixelData = (byte)outputRed;
			*tempImagePtr = pixelData;
			tempImagePtr++;
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
}

void BMPImage::StudentMethod3()
{
	// TEMPATURE ADJUSTMENT
	//
	// Source:
	// http://www.tannerhelland.com/5675/simple-algorithms-adjusting-image-temperature-tint/
	// Given a temperature adjustment on the range -100 to 100,
	// apply the following adjustment to each pixel in the image :
	// r = r + adjustmentValue
	// g = g
	// b = b - adjustmentValue
	// adjustmentValue has been arbitrarily set to 20

	byte * tempImagePtr = m_ImageDataPtr; // ALWAYS create a temporary pointer before modifying the image data

	float RED = 0.0f;
	float GREEN = 0.0f;
	float BLUE = 0.0f;

	float outputRed = 0.0f;
	float outputGreen = 0.0f; 
	float outputBlue = 0.0f;

	byte pixelData = 0;

	for (int row = 0; row < m_BIH.biHeight; row++)  // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++)	// cycle through the pitch: start at zero so one less than the width
		{ 
			BLUE = *tempImagePtr;
			tempImagePtr++;
			GREEN = *tempImagePtr;
			tempImagePtr++;
			RED = *tempImagePtr;

			tempImagePtr -= 2; // back to the blue pixel

			outputRed = RED + 20.0f;
			outputGreen = GREEN;
			outputBlue = BLUE - 20.0f;

			if (outputRed > 255)
			{
				outputRed = 255;
			}
			if (outputGreen > 255)
			{
				outputGreen = 255;
			}
			if (outputBlue > 255)
			{
				outputBlue = 255;
			}

			if (outputRed < 0)
			{
				outputRed = 0;
			}
			if (outputGreen < 0)
			{
				outputGreen = 0;
			}
			if (outputBlue < 0)
			{
				outputBlue = 0;
			}

			pixelData = (byte)outputBlue;
			*tempImagePtr = pixelData;
			tempImagePtr++;

			pixelData = (byte)outputGreen;
			*tempImagePtr = pixelData;
			tempImagePtr++;

			pixelData = (byte)outputRed;
			*tempImagePtr = pixelData;
			tempImagePtr++;
		}
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}

}

void BMPImage::StudentMethod4()
{
	// Flip

	// Starting at the end of tempImage and working backwards,
	// grab each pixel value from the tempImage and store it.
	// Then write the pixel values to the source starting at the beginning.

	byte * tempImagePtr = m_ImageDataPtr; // ALWAYS create a temporary pointer before modifying the image data
	byte * tempImage = new byte[m_BIH.biSizeImage];
	byte * tempImagePtr1 = tempImage;

	byte RED = 0;
	byte GREEN = 0;
	byte BLUE = 0;

	// copy the source image into tempImage
	for (int i = 0; i < m_BIH.biSizeImage; i++)
	{
		*tempImagePtr1 = *tempImagePtr;
		tempImagePtr++;
		tempImagePtr1++;
	}

	// reset ptr position back to beginning
	tempImagePtr -= m_BIH.biSizeImage;

	tempImagePtr1--;

	// backup the ptr position on the tempImage and account for padding
	if (m_Padding > 0)
	{
		tempImagePtr1 -= m_Padding;
	}
	//else
	//{
	//	tempImagePtr1--;
	//}

	for (int row = 0; row < m_BIH.biHeight; row++) // do all rows
	{
		for (int col = 0; col < m_BIH.biWidth; col++) // cycle through the pitch: start at zero so one less than the width
		{
			RED = *tempImagePtr1;
			tempImagePtr1--;
			GREEN = *tempImagePtr1;
			tempImagePtr1--;
			BLUE = *tempImagePtr1;
			tempImagePtr1--;

			*tempImagePtr = BLUE;
			tempImagePtr++;
			*tempImagePtr = GREEN;
			tempImagePtr++;
			*tempImagePtr = RED;
			tempImagePtr++;
		}
		tempImagePtr1 -= m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
		tempImagePtr += m_Padding; // Bitmaps end on 4 byte boundaries therefore we need to add 0,1,2 or 3 bytes to the pointer location so we are at the start of the next row
	}
	// cleanup
	delete[] tempImage;
	tempImage = nullptr;
}

#pragma endregion IMAGE_PROCESSING_METHODS