#include "Page.h"

void kInitializePageTables(void)
{
    PML4ENTRY*  pstPML4Entry;
    PDPTENTRY*  pstPDPTEntry;
    PDENTRY*    pstPDEntry;
    DWORD       dwMappingAddress;
    int i;

    //PML4 테이블 생성
    //첫 번째 엔트리 외에 나머지는 다 0으로 초기화(안씀)
    pstPML4Entry = (PML4ENTRY*) 0x100000;
    kSetPageEntryData( &pstPML4Entry[0], 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0);
    for(i = 1; i < PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData( &(pstPML4Entry[i]), 0, 0, 0, 0);
    }

    //페이지 디렉토리 포인터 테이블 생성
    //64개의 엔트리를 설정해 64GB까지 매핑
    pstPDPTEntry = (PDPTENTRY*) 0x101000;
    for(i = 0; i < 64; i++)
    {
        kSetPageEntryData( &(pstPDPTEntry[i]), 0, 0x102000 + (i*PAGE_TABLESIZE), PAGE_FLAGS_DEFAULT, 0);
    }
    for(i = 0; i < PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData( &(pstPDPTEntry[i]), 0, 0, 0, 0);
    }

    //페이지 디렉토리 테이블 생성
    //하나의 페이지 디렉토리가 1GB까지 매핑 가능
    //64개를 생성해 총 64GB 지원
    pstPDEntry = (PDPTENTRY*) 0x102000;
    dwMappingAddress = 0;
    for(i = 0; i < PAGE_MAXENTRYCOUNT * 64; i++)
    {
        kSetPageEntryData( &(pstPDPTEntry[i]), (i * (PAGE_DEFAULTSIZE >> 20)) >> 12, 
        dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
        dwMappingAddress += PAGE_DEFAULTSIZE;
    }
}

void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags)
{
    pstEntry -> dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
    pstEntry -> dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}