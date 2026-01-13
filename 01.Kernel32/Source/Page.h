#ifndef __PAGH_H__
#define __PAGH_H__

#include "Types.h"

//매크로 설정
//속성 필드 값에 대한 정의

/*
PCD, PWT, U/S, EXB, Avail, A, PAT, D, G = 0
R/W, P = 1
*/

//하위 32비트
#define PAGE_FLAGS_P    0x00000001  //P, Present
#define PAGE_FLAGS_RW   0x00000002  //Read/Write
#define PAGE_FLAGS_US   0x00000004  //User/Supervisor
#define PAGE_FLAGS_PWT  0x00000008  //캐시 정책
#define PAGE_FLAGS_PCD  0x00000010  //캐시 활성화 여부
#define PAGE_FLAGS_A    0x00000020  //Accessed
#define PAGE_FLAGS_D    0x00000040  //Dirty
#define PAGE_FLAGS_PS   0X00000080  //Page Size(페이지 크기) - 페이지 디렉토리 엔트리
#define PAGE_FLAGS_G    0x00000100  //Global
#define PAGE_FLAGS_PAT  0x00001000  //Page Attribute Table Index

//상위 32비트
#define PAGE_FLAGS_EXB  0x80000000  //데이터 전용
#define PAGE_FLAGS_DEFAULT  ( PAGE_FLAGS_P | PAGE_FLAGS_RW)

//페이지 테이블 크기: 4KB
#define PAGE_TABLESIZE       0x1000
#define PAGE_MAXENTRYCOUNT  512
//2MB 페이지
#define PAGE_DEFAULTSIZE    0x200000

//구조체
#pragma pack(push, 1)
//페이지 엔트리 구조체
//PML4와 페이지 디렉토리 포인터 테이블 엔트리
//하위 32비트 -> 1bit: P, RW, US, PWT, PCD, A / 3bit: 예약된 주소, Avail / 20bit: 상위 레벨 기준 주소
//상위 32비트: 8bit: 상위 레벨 기준 주소 / 12bit: 예약된 주소 / 11bit: Avail / 1bit:EXB

//페이지 디렉토리 엔트리
//하위 32비트 -> 1bit: P, RW, US, PWT, PCD, A, D, PS, G / 3bit: Avail / 1bit: PAT / 8bit: 예약된 주소, Avail / 10bit: 상위 레벨 기준 주소
//상위 32비트: 8bit: 상위 레벨 기준 주소 / 12bit: 예약된 주소 / 11bit: Avail / 1bit:EXB

typedef struct pageTableEntryStruct
{
	DWORD dwAttributeAndLowerBaseAddress; //엔트리의 하위 4바이트
	DWORD dwUpperBaseAddressAndEXB;       //엔트리의 상위 4바이트
} PML4ENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack(pop)

//함수
void kInitializePageTables(void);
void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);

#endif /*__PAGH_H__*/