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
/// Bitmap�t�H�[�}�b�g���t�@�C���ɏ�������
/// �Q�lurl:http://www.umekkii.jp/data/computer/file_format/bitmap.cgi
/// </summary>
/// <param name="file"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
int makeBmpHeader(FILE* file, int x, int y)
{
	int i;
	unsigned long int bfOffBits; /* �w�b�_�T�C�Y(byte) */

	if (file == NULL || ferror(file)) {
		return 0;
	}

	// �w�b�_�t�@�C�� Bitmap File Header size [byte]
	int sizeBitmapFileHeader = 14;
	// ���w�b�_ Bitmap Information Header size [byte]
	int sizeBitmapInfoHeaderSize = 40;
	// �p���b�g�f�[�^ Palette Data size [byte]
	int sizePalletaData = 4;

	bfOffBits = sizeBitmapFileHeader + sizeBitmapInfoHeaderSize + sizePalletaData;

	/// �t�@�C���w�b�_
	// bfType �t�@�C���^�C�v 2byte
	fputs("BM", file);
	// bfSize �t�@�C���T�C�Y 4byte
	fputc4Byte(bfOffBits + (unsigned long)x * y, file);
	// bfReserved1 �\��̈�1,�\��̈�2  1byte
	// ���g�̓[��
	fputc2Bytes(0, file);// 1
	fputc2Bytes(0, file);// 2
	// bfOffBits �w�b�_�T�C�Y 4byte
	fputc4Byte(bfOffBits, file);


	/// ���w�b�_
	// biSize ���T�C�Y�T�C�Y 4byte
	fputc4Byte(40, file);
	// biWidth �摜�̕�(�s�N�Z��) 4byte
	fputc4Byte(x, file);
	// biHeight �摜�̍���(�s�N�Z��) 4byte
	fputc4Byte(y, file);
	// biPlanes �v���[���� 2byte ���1
	fputc2Bytes(1, file);
	// biBitCount �F�r�b�g��(�r�b�g) 2byte
	// 24�ɐݒ�A1677���F�r�b�g�}�b�v�炵�� ?
	fputc2Bytes(24, file);
	// biCompression ���k���� 4byte
	fputc4Byte(0, file);
	// biSizeImage �摜�f�[�^�T�C�Y 4byte
	fputc4Byte(0, file);
	// biXPelsPerMeter �����𑜓x(dot/m) 4byte
	fputc4Byte(0, file);
	// biYPelsPerMeter �����𑜓x(dot/m) 4byte
	fputc4Byte(0, file);
	// biClrUsed �i�[�p���b�g�� 4byte
	fputc4Byte(0, file);
	// biClrImportant �d�v�F�� 4byte
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