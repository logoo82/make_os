#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, char* pcString);
BOOL kInitializeKernel64Area(void);
void kCopyKernel64ImageTo2Mbyte(void);

void Main()
{
    DWORD i;

    //배열을 인자로 넘겨야 배열이 스택에 들어가기 때문에 정상적으로 작동
    char str[] = "C Kernel Start,,,,,,,,,,Pass";
    kPrintString(0, 4, str);

    //kInitializeKernel64Area();
    char str2[] = "IA-32e Kernel Area initialize,,,,,,,,,,";
    kPrintString(0, 5, str2);
    if(kInitializeKernel64Area() == FALSE)
    {
        char fail[] = "Fail";
        kPrintString(45, 5, fail);
    }
    char pass[] = "Pass";
    kPrintString(45, 5, pass);
    char IA32[] = "IA-32e Page Tables Initialize,,,,,,,,,,";
    kPrintString(0, 6, IA32);
    kInitializePageTables();
    kPrintString(45, 6, pass);

    char m2[] = "Copy IA-32e Kernel To 2M Address,,,,,,,,";
    // IA-32e 모드 커널을 0x200000(2Mbyte) 주소로 이동
    kPrintString(0, 9, m2);
    kCopyKernel64ImageTo2Mbyte();
    kPrintString(45, 9, pass);

    char switching[] = "Switch To IA-32e Mode";
    kPrintString(0, 10, switching);

    kSwitchAndExecute64bitKernel();
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

void kCopyKernel64ImageTo2Mbyte(void)
{
    WORD wKernel32SectorCount, wTotalKernelSectorCount;
    DWORD* pdwSourceAddress, *pdwDestinationAddress;
    int i;

    // 0x7C05에 총 커널 섹터 수, 0x7C07에 보호 모드 커널 섹터 수가 들어 있음
    wTotalKernelSectorCount = *((WORD*) 0x7C05);
    wKernel32SectorCount = *((WORD*)0x7C07);
    
    pdwSourceAddress = (DWORD*)(0x10000 + (wKernel32SectorCount * 512));
    pdwDestinationAddress = (DWORD*) 0x200000;
    // IA-32e 모드 커널 섹터 크기만큼 복사
    for(i = 0; i < 512 * (wTotalKernelSectorCount - wKernel32SectorCount) / 4; i++)
    {
        *pdwDestinationAddress = *pdwSourceAddress;
        pdwDestinationAddress++;
        pdwSourceAddress++;
    }
}




