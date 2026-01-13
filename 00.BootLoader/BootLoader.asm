[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START    ; CS 세그먼트 레지스터에 0x07C0을 복사하면서, START 레이블로 이동

;;; OS에 관련된 환경 설정 값 ;;;
TOTALSECTORCOUNT: dw 1024      ; 부트 로더를 제외한 OS 이미지의 크기
                            ; 최대 1152 섹터(0x90000byte)까지 가능


;;; 코드 영역 ;;;
START:
    mov ax, 0x07C0  ; 부트 로더의 시작 어드레스(0x7C00)을 세그먼트 레지스터 값으로 변환
    mov ds, ax      ; DS 세그먼트 레지스터에 설정
    mov ax, 0xB800  ; 비디오 메모리의 시작 어드레스(0xB800)을 세그먼트 레지스터 값으로 변환
    mov es, ax      ; ES 세그먼트 레지스터에 (0xB800)설정

    ; 스택을 0x0000:0000 ~ 0x0000:FFFF 영역에 64KB 크기로 생성
    mov ax, 0x0000  ; 스택 세그먼트의 시작 주소(0x0000)를 세그먼트 레지스터 값으로 변환
    mov SS, ax      ; SS 세그먼트 레지스터에 설정
    mov sp, 0xFFFE  ; SP 레지스터의 주소를 0xFFFE로 설정
    mov bp, 0xFFFE  ; BP 레지스터의 주소를 0xFFFE로 설정


    ;;; 화면을 모두 지우고, 녹색으로 설정 ;;;
    mov si, 0       ; SI 레지스터(문자열 원본 인덱스 레지스터)를 초기화

.SCREANCLEARLOOP:   ; 화면을 지우는 루프
    mov byte [ es: si ], 0          ; 비디오 메모리의 문자가 위치하는 어드레스에 0을 복사해 문자 삭제

    mov byte [ es: si + 1 ], 0x0A   ; 비디오 메모리의 속성이 위치하는 어드레스에 0x0A를 복사

    add si, 2       ; 문자와 속성 설정 완료 -> 다음 위치로 이동
    
    cmp si, 80*25*2 ; SI 레지스터가 전체 화면 크기(4000)보다 작다면 아직 지우지 못한 영역이 있음
    
    jl .SCREANCLEARLOOP ; .SCREANCLEANLOOP 레이블로 이동

    ;mov si, 0   ; SI 레지스터(문자열 원본 인덱스 레지스터)를 초기화
    ;mov di, 0   ; DI 레지스터(문자열 대상 인덱스 레지스터)를 초기화

    ;;; 화면 상단에 시작 메시지 출력 ;;;
    push MESSAGE1       ; 출력할 메시지의 주소를 스택에 push
    push 0              ; 화면 Y좌표(0)를 스택에 push
    push 0              ; 화면 X좌표(0)를 스택에 push
    call PRINTMESSAGE   ; PRINTMESSAGE 함수 호출
    add sp, 6           ; 삽입한 파라미터 제거거

    ;;; OS 이미지를 로딩한다는 메시지 출력 ;;;
    push IMAGELOADINGMESSAGE    ; 출력할 메시지의 주소를 스택에 push
    push 1                      ; 화면 Y좌표(1)를 스택에 push
    push 0                      ; 화면 X좌표(0)를 스택에 push
    call PRINTMESSAGE           ; PRINTMESSAGE 함수 호출
    add sp, 6                   ; 삽입한 파라미터 제거

    ;;; 디스크에서 OS 이미지를 로딩 ;;;
    ; 디스크를 읽기 전에 먼저 리셋
RESETDISK:  ; 디스크를 리셋하는 코드의 시작
    ;;; BIOS Reset Function 호출 ;;;
    ; 서비스 번호 = 0, 드라이브 번호(0=Floppy)
    mov ax, 0
    mov dl, 0
    int 0x13
    ; 에러가 발생하면 에러 처리로 이동
    jc HANDLERDISKERROR

    ;;; 디스크에서 섹터를 읽음 ;;;
    ; 디스크의 내용을 메모리로 복사할 어드레스(ES:BX)를 0x10000으로 설정
    mov si, 0x1000          ; OS 이미지를 복사할 주소(0x10000)를 세그먼트 레지스터 값으로 변환
    mov es, si              ; ES 세그먼트 레지스터에 값 설정
    mov bx, 0x0000          ; BX 레지스터에 0x0000을 설정하여 복사할 주소를 0x1000:0000(0x10000)으로 최종 설정

    mov di, word [ TOTALSECTORCOUNT ]   ; 복사할 OS 이미지의 섹터 수를 DI 레지스터에 설정

READDATA:   ; 디스크를 읽는 코드의 시작
    ; 모든 섹터를 다 읽었는 지 확인
    cmp di, 0   ; 복사할 OS 이미지의 섹터 수를 0과 비교
    je READEND  ; 복사할 섹터 수가 0이라면 다 복사했으므로 READEND로 이동
    sub di, 0x1 ; 복사할 섹터 수를 1 감소

    ;;; BIOS READ Function 호출
    mov ah, 0x02                    ; BIOS 서비스 번호 2(Read Sector)
    mov al, 0x01                    ; 읽을 섹터 수는 1
    mov ch, byte [ TRACKNUMBER ]    ; 읽을 트랙 번호 설정
    mov cl, byte [ SECTORNUMBER ]   ; 읽을 섹터 번호 설정
    mov dh, byte [ HEADNUMBER ]     ; 읽을 헤드 번호 설정
    mov dl, 0x00                    ; 읽을 드라이브 번호(0=Floppy) 설정
    int 0x13                        ; 인터럽트 서비스 수행
    jc HANDLERDISKERROR             ; 에러 처리(HANDLERDISKERROR)로 이동

    ;;; 복사할 주소와 트랙, 헤드, 섹터 주소 계산 
    add si, 0x0020      ; 512(0x200)바이트만큼 읽었으므로, 이를 세그먼트 레지스터 값으로 변화나
    mov es, si          ; ES 세그먼트 레지스터에 더해서 주소를 한 섹터 만큼 증가

    ; 한 섹터를  읽었으므로 섹터 번호를 증가시키고 마지막 섹터(18)까지 읽었는 지 판단
    ; 마지막 섹터가 아니면 섹터 읽기로 이동해 다시 섹터 읽기 수행
    mov al, byte [ SECTORNUMBER ]   ; 섹터 번호를 AL 레지스터에 설정
    add al, 0x1                     ; 섹터 번호를 1 증가
    mov byte [ SECTORNUMBER ], al   ; 증가시킨 섹터 번호를 SECTORNUMBER에 다시 설정
    cmp al, 19                      ; 증가시킨 섹터 번호를 19와 비교
    jl READDATA                     ; 섹터 번호 < 19면 READDATA로 이동 -> 디스크 읽기

    ; 마지막 섹터까지 읽었으면(섹터 번호 == 19) 헤드를 Toggle하고, 섹터 번호를 1로 설정
    xor byte [ HEADNUMBER ], 0x01    ; 헤드 번호를 0x01과 xor(Toggle)
    mov byte [ SECTORNUMBER ], 0x01  ; 섹터 번호를 다시 1로 설정

    ; 헤드가 1 -> 0으로 바뀌었으면 양쪽 헤드를 모두 읽은 것이므로 아래로 이동하여 트랙 번호를 1 증가
    cmp byte [ HEADNUMBER ], 0x00   ; 헤드 번호를 0x00과 비교
    jne READDATA                    ; 0이 아니면 READDATA로 이동

    ; 트랙을 1 증가시킨 후 다시 섹터 읽기로 이동
    add byte [ TRACKNUMBER ], 0x01  ; 트랙 번호를 1 증가
    jmp READDATA                    ; READDATA로 이동

READEND:

    ;;; OS 이미지가 완료되었다는 메시지를 출력 ;;;
    push LOADINGCOMPLETEMESSAGE     ; 출력할 메시지의 주소를 스택에 push
    push 1                          ; 출력 Y좌표(1)를 스택에 push
    push 20                         ; 출력 X좌표(20)를 스택에 push
    call PRINTMESSAGE               ; PRINTMESSAGE 함수 호출
    add sp, 6                       ; 삽입한 파라미터 제거

    ;;; 로딩한 가상 OS 이미지 실행 ;;;
    jmp 0x1000:0x0000

;;; 함수 코드 영역 ;;;
; 디스크 에러를 처리하는 함수
HANDLERDISKERROR:
    push DISKERRORMESSAGE   ; 에러 문자열의 주소를 스택에 삽입
    push 1                  ; 화면 Y좌표(1)를 스택에 삽입
    push 20                 ; 화면 X좌표(20)를 스택에 삽입
    call PRINTMESSAGE       ; PRINTMESSAGE 함수 호출

    jmp $                   ; 현재 위치에서 무한 루프

; 메시지를 출력하는 함수
; parameter : x좌표, y좌표, 문자열
PRINTMESSAGE:
    push bp     ; 베이스 포인터 레지스터(BP)를 스택에 삽입
    mov bp, sp  ; 베이스 포인터 레지스터에 스택 포인터 레지스터(SP)의 값 설정
                ; BP를 이용해 파라미터에 접근

    push es     ; ES 세그먼트 레지스터부터 DX 레지스터까지 스택에 삽입
    push si     ; 함수에서 임시로 사용하는 레지스터의 마지막 부분에서
    push di     ; 스택에 삽입된 값을 꺼내 원래 값으로 복원
    push ax
    push cx
    push DX

    ; ES 세그먼트 레지스터에서 비디오 모드 주소 설정
    mov ax, 0xB800   ; 비디오 메모리 시작 주소(0xB800)를 세그먼트 레지스터 값으로 변환
    mov es, ax       ; ES 세그먼트 레지스터에 설정

    ;;; X, Y의 좌표로 비디오 메모리의 주소를 계산 ;;;
    ; Y좌표를 이용해 먼저 라인 주소 계산
    mov ax, word [ bp + 6 ]     ; 파라미터 2(Y좌표)를 AX 레지스터에 설정
    mov si, 160                 ; 한 라인의 바이트 수(2 + 80 컬럼)을 SI 레지스터로 설정
    mul si                      ; AX 레지스터와 SI 레지스터를 곱하여 화면 Y 주소 계산
    mov di, ax                  ; 계산된 화면 Y 주소를 DI 레지스터에 설정

    ; X좌표를 이용해 2를 곱한 후 최종 주소 구하기
    mov ax, word [ bp + 4 ]     ; 파라미터 1(X좌표)를 AX 레지스터에 설정
    mov si, 2                   ; 한 문자를 나타내는 바이트 수(2)를 SI 레지스터에 설정
    mul si                      ; AX 레지스터와 SI 레지스터를 곱하면 화면 X 주소 계산
    add di, ax                  ; 실제 비디오 메모리 주소 계산

    ; 출력할 문자열 주소
    mov si, word [ bp + 8 ]     ; 파라미터 3(출력할 문자열 주소)

.MESSAGELOOP:   ; 메시지를 출력하는 루프
    mov cl, byte [ si ]  ; SI 레지스터가 가리키는 문자열 위치에서 한 문자를 CL 레지스터에 복사
                         ; CL 레지스터는 CX 레지스터의 하위 1바이트를 의미
                         ; 문자열은 1 바이트면 충분하므로 CX 레지스터의 하위 1바이트만 사용
    
    cmp cl, 0       ; 복사된 문자와 0을 비교
    je .MESSAGEEND  ; 복사된 문자의 값이 0이면 문자열이 종료됨, MESSAGEEND로 이동해 문자 출력 종료

    mov byte [ es: di ], cl ; 0이 아니라면 비디오 메모리 어드레스 0xB800:di에 문자를 출력

    add si, 1   ; SI 레지스터에 1을 더하여 다음 문자열로 이동
    add di, 2   ; DI 레지스터에 2를 더하여 비디오 메모리의 다음 문자 위치로 이동
                ; 비디오 메모리는 (문자, 속성)의 쌍으로 구성되므로, 문자만 출력하려면 2를 더해야 함

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop dx  ; 함수에서 사용이 끝난 DX 레지스터부터 ES 레지스터까지를 스택에 삽입된 값을 이용해 복원
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp  ; BP 복원
    ret     ; 함수를 호출한 다음 코드의 위치로 복귀

;;; 데이터 영역 ;;;
; 부트로더 시작 메시지지
MESSAGE1: db 'OS Boot Loader Start!', 0 ;출력할 메시지 정의 / 마지막은 0으로 설정하여 .MESSAGELOOP에서 문자열이 종료되었음을 알 수 있도록 함

DISKERRORMESSAGE: db 'DISK Error!', 0
IMAGELOADINGMESSAGE: db 'OS Image Loading', 0
LOADINGCOMPLETEMESSAGE: db 'Complete', 0

; 디스크 읽기에 관련된 변수들
SECTORNUMBER: db 0x02   ; OS 이미지가 시작하는 섹터 번호를 저장하는 영역
HEADNUMBER: db 0x00     ; OS 이미지가 시작하는 헤드 번호를 저장하는 영역
TRACKNUMBER: db 0x00    ; OS 이미지가 시작하는 트랙 번호를 저장하는 영역

times 510 - ($ - $$) db 0x00

db 0x55
db 0xAA

