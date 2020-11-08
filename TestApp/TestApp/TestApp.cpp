// TestApp.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include "TestApp.h"


int main()
{
    std::cout << "Hello World!\n";
	FILE* file = NULL;
	fopen_s(&file, "C:/Users/tobita/Desktop/test.dat", "wb");
	if (file == NULL)
	{
		std::cout << "[!!!] error open file." << std::endl;
		return -1;
	}
	fputs("BM", file);
	for (int i = 0; i < 16; i++)
	{
		if (i % 3 == 0)
		{
			fputs("BM", file);
		}
		fputc4LowHigh((unsigned long)(65000), file);
	}
	fclose(file);
}

int fputc4LowHigh(unsigned long d, FILE* file)
{

	//int f = d & 0xFF;
	//std::cout << f << std::endl;
	//f = (d >> CHAR_BIT) & 0xFF;
	//std::cout << f << std::endl;
	//f = (d >> CHAR_BIT * 2) & 0xFF;
	//std::cout << f << std::endl;
	//f = (d >> CHAR_BIT * 3) & 0xFF;
	//std::cout << f << std::endl;

	putc(d & 0xFF, file);
	putc((d >> CHAR_BIT) & 0xFF, file);
	putc((d >> CHAR_BIT * 2) & 0xFF, file);
	return putc((d >> CHAR_BIT * 3) & 0xFF, file);
}
