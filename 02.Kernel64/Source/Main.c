#include "Types.h"
#include "Keyboard.h"

void kPrintString(int iX, int iY, char* pcString);

void Main()
{
    char vcTemp[2] = {0, };
    BYTE bFlags;
    BYTE bTemp;
    int i = 0;

    char pass[] = "Pass";
    char fail[] = "Fail";

    char str[] = "Switch to IA-32e Mode Success!";
    kPrintString(0, 10, str);

    char str2[] = "IA-32e C Kernel Start!";
    kPrintString(0, 11, str2);

    char str3[] = "Keyboard Activate......";
    kPrintString(0, 12, str3);

    if(kActivateKeyboard() == TRUE)
    {
        kPrintString(45, 12, pass);    
        kChangeKeyboardLED(FALSE, FALSE, FALSE);
    }
    else
    {
        kPrintString(45, 12, fail);
        while(1);
    }
   
    while(1)
    {
        // 출력 버퍼(포트 0x60)가 차 있으면 스캔 코드를 읽을 수 있음
        if(kIsOutputBufferFull() == TRUE)
        {
            // 출력 버퍼(포트 0x60)에서 스캔 코드를 읽어서 저장
            bTemp = kGetKeyboardScanCode();

            // 스캔 코드를 ASCII 코드로 변환하는 함수를 호출하여 ASCII 코드와
            // 눌림 또는 떨어짐 정보를 반환
            if(kConvertScanCodeToASCIICode(bTemp, &(vcTemp[0]), &bFlags) == TRUE)
            {
                // 키가 눌러졌으면 키의 ASCII 코드 값을 화면에 출력
                if(bFlags & KEY_FALGS_DOWN)
                {
                    kPrintString(i++, 13, vcTemp);
                }
            }
        }
    }



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




