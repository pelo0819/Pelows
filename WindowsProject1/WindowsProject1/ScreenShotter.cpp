#include "KeyHook.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>

#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))

void Shot()
{
	FILE* file = NULL;
	fopen_s(&file, "C:/Users/tobita/Desktop/test.bmp", "wb");
	if (file == NULL)
	{
		std::cout << "[!!!] error open file." << std::endl;
		return;
	}
	fputs("BM", file);
	fclose(file);

	HWND hWnd = GetDesktopWindow();
	int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int top = GetSystemMetrics(SM_YVIRTUALSCREEN);

	HDC hdc = GetWindowDC(hWnd);
	//CreateDCFrom

	//CreateBitmap()
	HDC hMdc = CreateCompatibleDC(hdc);
	RECT rc;
	GetClientRect(hWnd, &rc);

	COLORREF ref = GetPixel(hdc, 0, 0);
	int r = GetRValue(ref);
	int g = GetGValue(ref);
	int b = GetBValue(ref);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);

	SelectObject(hMdc, hBitmap);

	BitBlt(hMdc, 0, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY);

	DeleteDC(hdc);
}

#if true

/// <summary>
/// Bitmapフォーマットをファイルに書き込む
/// 参考url:http://www.umekkii.jp/data/computer/file_format/bitmap.cgi
/// </summary>
/// <param name="file"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
int makeBmpHeader(FILE* file, int x, int y)
{
	int i;
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

	bfOffBits = sizeBitmapFileHeader + sizeBitmapInfoHeaderSize + sizePalletaData;

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
#endif

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