#include "Types.h"

void kPrintString(int iX, int iY, const char* pcString);

void Main()
{
    kPrintString(0, 4, "C Kernel!!");

    while(1);
}

//문자열 출력 함수
void kPrintString(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = (CHARACTER*) (0xB8000 + (iY*160) + (iX*2));


    int i;

        for(i = 0; pcString[i] != 0; i++)
    {
        pstScreen[i].b_character = pcString[i];
        pstScreen[i].b_attribute = 0x0A;
    }
}