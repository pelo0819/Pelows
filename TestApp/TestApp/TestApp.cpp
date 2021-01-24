#include <iostream>
#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <atlimage.h>
//#include <gdiplusimaging.h>

#include "TestApp.h"

int main()
{
	FILE* file = NULL;
	fopen_s(&file, "C:/Users/tobita/Documents/test.bmp", "wb");
	if (file == NULL)
	{
		std::cout << "[!!!] error open file." << std::endl;
		return -1;
	}

	// 画面のサイズを取得
	HWND hWnd = GetDesktopWindow();
	int width = GetSystemMetrics(SM_CXSCREEN) * 1.5f;
	int height = GetSystemMetrics(SM_CYSCREEN) * 1.5f;
	int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int top = GetSystemMetrics(SM_YVIRTUALSCREEN);

	RECT rc;
	//GetClientRect(hWnd, &rc);
	GetWindowRect(hWnd, &rc);

	std::cout << "width:" << width << std::endl;
	std::cout << "height:" << height << std::endl;

	int bmp_data_size = 3 * width * height;

	// 画像データ分のメモリを確保
	unsigned char* pic_data;
	pic_data = (unsigned char*)malloc(bmp_data_size);
	if (pic_data == NULL)
	{
		std::cout << "[!!!] fail to get picture data memory." << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "[*] successful to get picture data." << std::endl;
	}

	// 画面のピクセルをメモリに書き込む
	GeneratePicData(pic_data, width, height, hWnd);

	// BMPヘッダーをファイルに書き込む
	int flag = makeBmpHeader(file, width, height);
	if (!flag) 
	{
		std::cout << "[!!!] fail to write bitmap header." << std::endl;
		fclose(file);
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "[*] successful write bitmap header." << std::endl;
	}

	flag = fwrite(pic_data, sizeof(unsigned char), bmp_data_size, file);
	if (flag != bmp_data_size)
	{
		fclose(file);
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "[*] successful write bitmap file. may be..." << std::endl;
	}

	fclose(file);
}

bool GenerateBitMapCache(HWND hw, BITMAP* bmp)
{

	//HDC hdc = GetWindowDC(hw);
	//RECT rc;
	//GetClientRect(hw, &rc);
	//HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
	//BITMAP bmp;
	//if (!GetObject(hBitmap, sizeof(BITMAP), &bmp))
	//{
	//	std::cout << "[!!!] fail to bitmap cache" << std::endl;
	//	return false;
	//}

	return true;
}

/// <summary>
/// 指定したウィンドハンドルのピクセルデータをバッファに書き込む
/// </summary>
/// <param name="buffer"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="h"></param>
bool GeneratePicData(unsigned char* buffer, int w, int h, HWND hw)
{
	CRect targetRect(0, 0, w, h);
	HDC hdc = GetDC(hw);
	CImage img;
	img.Create(w, h, 24);
	CImageDC imgDC(img);
	/*BitBlt(imgDC, 0, 0, targetRect.Width(), targetRect.Height(),
		hdc, targetRect.left, targetRect.top, SRCCOPY);*/
	BitBlt(imgDC, 0, 0, w, h,
		hdc, targetRect.left, targetRect.top, SRCCOPY);
	ReleaseDC(hw, hdc);
	for (int y = h-1; y >= 0; y--) {
		for (int x = 0; x < w; x++) {
			COLORREF ref = img.GetPixel(x, y);
			unsigned char r = GetRValue(ref);
			unsigned char g = GetGValue(ref);
			unsigned char b = GetBValue(ref);
			*buffer = b;
			buffer++;
			*buffer = g;
			buffer++;
			*buffer = r;
			buffer++;
		}
		std::cout << "scan monitor -> " << y << " / " << h << std::endl;
	}

	return true;
}

/// <summary>
/// Bitmapヘッダーをファイルに書き込む
/// 参考url:http://www.umekkii.jp/data/computer/file_format/bitmap.cgi
/// </summary>
/// <param name="file"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
int makeBmpHeader(FILE* file, int x, int y)
{
	unsigned long int bfOffBits; /* ヘッダサイズ(byte) */

	if (file == NULL || ferror(file)) {
		return 0;
	}

	// ヘッダファイル Bitmap File Header size [byte]
	int sizeBitmapFileHeader = 14;
	// 情報ヘッダ Bitmap Information Header size [byte]
	int sizeBitmapInfoHeaderSize = 40;
	// パレットデータ Palette Data size [byte]
	int sizePalletaData = 4;

	//bfOffBits = sizeBitmapFileHeader + sizeBitmapInfoHeaderSize + sizePalletaData;
	bfOffBits = sizeBitmapFileHeader + sizeBitmapInfoHeaderSize;

	/// ファイルヘッダ
	// bfType ファイルタイプ 2byte
	fputs("BM", file);
	// bfSize ファイルサイズ 4byte
	fputc4Byte(bfOffBits + (unsigned long)x * y, file);
	// bfReserved1 予約領域1,予約領域2  1byte
	// 中身はゼロ
	fputc2Bytes(0, file);// 1
	fputc2Bytes(0, file);// 2
	// bfOffBits ヘッダサイズ 4byte
	fputc4Byte(bfOffBits, file);


	/// 情報ヘッダ
	// biSize 情報サイズサイズ 4byte
	fputc4Byte(40, file);
	// biWidth 画像の幅(ピクセル) 4byte
	fputc4Byte(x, file);
	// biHeight 画像の高さ(ピクセル) 4byte
	fputc4Byte(y, file);
	// biPlanes プレーン数 2byte 常に1
	fputc2Bytes(1, file);
	// biBitCount 色ビット数(ビット) 2byte
	// 24に設定、1677万色ビットマップらしい ?
	fputc2Bytes(24, file);
	// biCompression 圧縮方式 4byte
	fputc4Byte(0, file);
	// biSizeImage 画像データサイズ 4byte
	fputc4Byte(0, file);
	// biXPelsPerMeter 水平解像度(dot/m) 4byte
	fputc4Byte(0, file);
	// biYPelsPerMeter 垂直解像度(dot/m) 4byte
	fputc4Byte(0, file);
	// biClrUsed 格納パレット数 4byte
	fputc4Byte(0, file);
	// biClrImportant 重要色数 4byte
	fputc4Byte(0, file);

	if (ferror(file)) { return 0; }
	return 1;
}

int fputc2Bytes(unsigned short d, FILE* s)
{
	putc(d & 0xFF, s);
	return putc(d >> CHAR_BIT, s);
}

int fputc4Byte(unsigned long d, FILE* file)
{
	putc(d & 0xFF, file);
	putc((d >> CHAR_BIT) & 0xFF, file);
	putc((d >> CHAR_BIT * 2) & 0xFF, file);
	return putc((d >> CHAR_BIT * 3) & 0xFF, file);
}