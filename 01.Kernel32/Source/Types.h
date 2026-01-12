//__TYPES_H__가 정의되어있지 않을 경우 아래 코드 실행
#ifndef __TYPES_H__
#define __TYPES_H__

// BYTE = 1byte, WORD = 2byte, DWORD = 4byte, QWORD = 8byte

// #define BYTE unsigned char
// #define WORD unsigned short
// #define DWORD unsigned int
// #define QWORD unsigned long
// #define BOOL unsigned char

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int        DWORD;
typedef unsigned long long  QWORD;
typedef int                 BOOL;

#define TRUE 1
#define FALSE 0
#define NULL 0

//1byte씩 정렬 선언
//#pragma pack (push, 1)

typedef struct k_character_struct
{
    //문자
    BYTE b_character;
    //속성값
    BYTE b_attribute;
} __attribute__((packed)) CHARACTER;

//#pragma pack(pop)
#endif /*__TYPES_H__*/

