#pragma once

#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))

bool GenerateBitMapCache(HWND hw, BITMAP* bmp);
bool GeneratePicData(unsigned char* buffer, BITMAP* bmp);
bool GeneratePicData(unsigned char* buffer, int w, int h, HWND hw);
int makeBmpHeader(FILE* file, int x, int y);
int fputc2Bytes(unsigned short d, FILE* s);
int fputc4Byte(unsigned long d, FILE* file);
