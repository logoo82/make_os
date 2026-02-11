[BITS 32]

global kSwitchAndExecute64bitKernel

SECTION .text

kSwitchAndExecute64bitKernel:
    ;;; cr4 레지스터의 PAE 비트를 1로 설정 ;;;
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    ;;; cr3 레지스터에 페이지 테이블 주소 저장 ;;;
    mov eax, 0x100000
    mov cr3, eax

    ;;; IA32-EFER 레지스터 설정 ;;;
    mov ecx, 0xc0000080
    rdmsr

    or eax, 0x0100
    wrmsr

    ;;; cr0 레지스터의 nw = 0, cd = 0, pg = 1 비트 설정
    mov eax, cr0
    or eax, 0xE0000000
    xor eax, 0x60000000
    mov cr0, eax

    jmp 0x08:0x200000   ; cs 세그먼트 레지스터를ㄹ IA-32e 모드용 코드 세그먼트 디스크립터로 교체
                        ; 0x200000(2MB) 주소로 이동
    
    jmp $