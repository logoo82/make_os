#include "Types.h"

void kPrintString(int iX, int iY, char* pcString);
BOOL kInitializeKernel64Area(void);

// void Main()
// {
//     kPrintString(0, 5, "C Kernel!!");

//     kInitializeKernel64Area();
//     kPrintString(0, 6, "IA-32e Kernel Area initialized complete");

//     while(1);
// }

void Main()     
{
    int line = 4;
    int size = sizeof(CHARACTER);
    char msg[] = "SIZE=";
    char* video = (char*)(0xB8000 + 160*line);

    *video++ = 'S';
    *video++ = 0x0A;
    *video++ = 'I';
    *video++ = 0x0A;
    *video++ = 'Z';
    *video++ = 0x0A;
    *video++ = 'E';
    *video++ = 0x0A;
    *video++ = '=';
    *video++ = 0x0A;
    *video++ = '0' + sizeof(CHARACTER);
    *video++ = 0x0A;
    char str[] = "C Kernel!!";
    kPrintString(0, 8, str);

    kPrintString(0,9,"FIXED!!");

    while(1);

}

//문자열 출력 함수
void kPrintString(int iX, int iY, char* pcString)
{
    CHARACTER* pstScreen = (CHARACTER*) 0xB8000;
    pstScreen += (iY*80) + iX;

    int i;
    for(i = 0; pcString[i] != 0; i++)
    {
        pstScreen[i].b_character = pcString[i];
        pstScreen[i].b_attribute = 0x0A;

        // *pstScreen++ = pcString[i];
        // *pstScreen++ = 0x0A;
    }
}

BOOL kInitializeKernel64Area(void)
{
    DWORD* pdwCurrentAddress;

    // 초기화 시작 주소인 0x100000(1MB)를 설정
    pdwCurrentAddress = (DWORD*) 0x100000;

    // 마지막 주소인 0x600000(6MB)까지 4바이트씩 0으로 채움
    while((DWORD)pdwCurrentAddress < 0x600000)
    {
        *pdwCurrentAddress = 0x00;

        //0으로 채운 후 다시 읽었을 때 0이 아닌 경우
        //해당 주소에 문제가 생김 -> 종료
        if(*pdwCurrentAddress != 0)
            return FALSE;
        
        //다음 주소로 이동
        pdwCurrentAddress++;
    }

    return TRUE;
}




