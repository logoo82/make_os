#include "Page.h"

void kInitializePageTables(void)
{

    PML4ENTRY*  pstPML4Entry;
    PDPTENTRY*  pstPDPTEntry;
    PDENTRY*    pstPDEntry;
    DWORD       dwMappingAddress;
    int i;

    // PML4 -> 페이지 디렉토리 포인터 테이블(PDPT) -> 페이지 디렉토리 테이블(PD)

    //[1단계] PML4 테이블 생성
    //1개의 페이지 디렉토리 포인터 테이블 관리 -> 1개 필요
    //첫 번째 엔트리 외에 나머지는 다 0으로 초기화(안씀)
    //

    //1. 1MB(0x100000)지점을 PML4 테이블로 사용
    pstPML4Entry = (PML4ENTRY*) 0x100000;
    //2.첫 번째 칸 설정 / 상위 테이블의 기준주소(0x101000) / P + RW 설정
    kSetPageEntryData( &pstPML4Entry[0], 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0);

    //3. 나머지 칸(511개) 0으로 초기화
    for(i = 1; i < PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData( &(pstPML4Entry[i]), 0, 0, 0, 0);
    }

    //[2단계] 페이지 디렉토리 포인터 테이블 생성
    //64개의 엔트리를 설정해 64GB까지 매핑

    //1. 0x101000을 PDPT 테이블로 설정
    pstPDPTEntry = (PDPTENTRY*) 0x101000;

    //2. 64개의 엔트리(0~63)를 채움
    //각 엔트리는 서로 다른 페이지 디렉토리(PD)를 가리켜야함
    for(i = 0; i < 64; i++)
    {
        // 0x102000부터 시작해 4KB(페이지 디렉토리 테이블 크기)씩 건너뛰며 연결 -> 상위 테이블 연결
        // 0x103000, 0x104000, ...
        kSetPageEntryData( &(pstPDPTEntry[i]), 0, 0x102000 + (i*PAGE_TABLESIZE), PAGE_FLAGS_DEFAULT, 0);
    }
    //3. 안쓰는 부분은 0으로 초기화
    for(i = 64; i < PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData( &(pstPDPTEntry[i]), 0, 0, 0, 0);
    }

    //[3단계] 페이지 디렉토리 테이블 생성
    //하나의 페이지 디렉토리가 1GB까지 매핑 가능
    //64개를 생성해 총 64GB 지원
    //가상주소 - 물리주소 연결

    //1. 0x102000을 페이지 디렉토리 테이블들의 시작주소로 설정
    pstPDEntry = (PDENTRY*) 0x102000;
    //실제 매핑할 물리 주소(0번지부터 시작)
    dwMappingAddress = 0;

    //64GB 영역을 매핑하는  데 필요한 페이지 디렉토리 수 (512*64)
    for(i = 0; i < PAGE_MAXENTRYCOUNT * 64; i++)
    {
        //보호모드에서는 단일 레지스터로 최대 32비트 값까지만 표현 가능
        //64비트 주소를 전달하기 위해서는 상위 32비트와 하위 32비트를 나눈 후 넘겨줘야함

        //페이지 크기인 2MB를 1MB(20bit)로 나누어 값을 나눈 후(2가 됨), i(반복 변수)와 곱함
        // 즉 2MB 단위를 정수 2로 바꾸고 페이지 번호와 곱함
        //페이지 번호가 2048을 넘어가면 32비트를 초과하게 됨 -> 이때부터 12 shift연산을 했을 때 값이 나옴
        // 이후 계산 결과를 다시 4KB(12bit)로 나누어 상위 32비트의 주소 계산
        kSetPageEntryData( &(pstPDEntry[i]), (i * (PAGE_DEFAULTSIZE >> 20)) >> 12, 
        dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
        
        //다음 페이지를 위해 2MB 증가
        dwMappingAddress += PAGE_DEFAULTSIZE;
    }
}

//엔트리(8byte, 테이블의 구성 요소)에 값을 채워 놓음

void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags)
{   
    // 하위 4바이트 설정
    // OR 연산을 통해 주소와 속성 값을 합침
    // 주소는 4KB 단위(하위 12비트가 0)이라 이 공간에 속성값을 넣음
    pstEntry -> dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
    
    // 상위 4바이트 설정
    // 상위 주소(>32비트)와 EXB 비트 등을 합침

    // 8비트만 페이지 기준 주소로 사용해야 하기 때문에 11111111와 and연산을 해 나머지 값들을 지워버림

    pstEntry -> dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}
