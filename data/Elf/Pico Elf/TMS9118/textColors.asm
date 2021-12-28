; -------------------------------------------------------------------
;                        textColors
; 
; Text demo program for PicoElf / MicroELF with the TMS9118 Color 
; Board published by Michael H Riley:
;
;      http://www.elf-emulation.com/hardware/tms9918.png
;
; This program uses the TMS9118 mode 0 for 40x24 text
;
; This software is Copyright (C) 2021 by Glenn Jolly
;
; You have permission to use, modify, copy, and distribute
; this software for non commercial uses.  Please notify me 
; on the COSMAC ELF Group at https://groups.io/g/cosmacelf of any
; improvements and/or corrections.
;
; -------------------------------------------------------------------
; *** Uses BIOS calls from software written by Michael H Riley
; *** Original author copyright notice:
; -------------------------------------------------------------------
; *** This software is copyright 2004 by Michael H Riley          ***
; *** You have permission to use, modify, copy, and distribute    ***
; *** this software so long as this copyright notice is retained. ***
; *** This software may not be used in commercial applications    ***
; *** without express written permission from the author.         ***
; -------------------------------------------------------------------
;
;                  TMS9118
;            Text mode memory map
;            +-----------------+ 0000h
;            |  Pattern Table  |
;            +-----------------+ 0800h 2048 (can define 256 codes)
;            |    Name Table   |
;            +-----------------+ 0BC0h 3008 (40x24 960 chars)
;            |     Unused      |
;            +-----------------+ 3FFFh (16k VRAM)
;

include    bios.inc

#define cr  13
#define lf  10

; Port assignments
#define VDP_DAT_P    1   ; Data port
#define VDP_REG_P    5   ; Register and address control port
#define UART_SELECT  6   ; UART register select I/O port
#define UART_DATA    7   ; UART data I/O port

; Font used - must be only one defined
;#define TI99_CHARSET
#define CP437_CHARSET

;#define ASM02

#ifdef ASM02 
; RCASM compatible pseudo ops
.op "PUSH","N","8$1 73 9$1 73"
.op "POP","N","60 72 B$1 F0 A$1"
.op "CALL","W","D4 H1 L1"
.op "RTN","","D5"
.op "MOV","NR","8$2 A$1 9$2 B$1"
.op "MOV","NW","F8 L2 A$1 F8 H2 B$1"
#endif

; TI VDP colors - not used - for reference only
; COLOR_TRANSPARENT:  equ 00h
; COLOR_BLACK:        equ 01h
; COLOR_GREEN:        equ 02h
; COLOR_LIGHT_GREEN:  equ 03h
; COLOR_BLUE:         equ 04h
; COLOR_LIGHT_BLUE:   equ 05h
; COLOR_DARK_RED:     equ 06h
; COLOR_CYAN:         equ 07h
; COLOR_RED:          equ 08h
; COLOR_LIGHT_RED:    equ 09h
; COLOR_YELLOW:       equ 0ah
; COLOR_LIGHT_YELLOW: equ 0bh
; COLOR_DARK_GREEN:   equ 0ch
; COLOR_MAGENTA:      equ 0dh
; COLOR_GRAY:         equ 0eh
; COLOR_WHITE:        equ 0fh

            org 2000H

MAIN:       call INIT_VREG     ; start with white text on blue background
            call CLEAR_VRAM    ; clear VRAM
            call INIT_CHARACTER_SET
            
            call GOTOXY
            db   10, 0
            mov  rf, TITLE_STR 
            call TYPE_STRING

            call GOTOXY
            db   8, 2
            mov  rf, CHAR_STR 
            call TYPE_STRING

            call GOTOXY
            db   0, 4
            call DISP_CHARSET

            call GOTOXY
            db   13,19
            mov  rf, C4_STR 
            call TYPE_STRING

            call GOTOXY
            db   0, 23
            mov  rf, PROMPT_STR 
            call TYPE_STRING


; -------------------------------------------------------------------
;          Display various textcolor settings
; -------------------------------------------------------------------

            mov  rc, TXTCOLORS+4 ; pointer to default white on blue
COLOR_LOOP: call SET_TEXTCOLOR 

            mov  rf, 0h
            call INKEY
            glo  rf
            smi  078h
            lbz  QUIT          ; exit if 'x' typed
            
            glo  rf
            lbz  NO_NUM

            call isnum         ; DF=1 if numeric ascii '0'..'9'
            lbdf NUM_OK

NO_NUM:     call DELAY100MS
            lbr  COLOR_LOOP
 
NUM_OK:     mov  rd, 0
            glo  rf
            smi  30h
            plo  r7            ; save number index
            plo  rd
            mov  rf, TXTCOLORS
            call ADD16  
            mov  rc,rd         ; rc points to color scheme F/B byte
            call GOTOXY
            db   13,19
            
            mov  rd, 0
            glo  r7            ; get index back
            plo  rd
            shl                ; multiply by 16
            shl
            shl
            shl
            plo  rd            ; 16*index
            
            mov  rf, C0_STR 
            call ADD16         ; rd is address of color string message
            mov  rf,rd         ; rf <- C0_STR + 16*index

            call TYPE_STRING
            call DELAY100MS
            lbr  COLOR_LOOP

QUIT:       rtn                ; return to monitor


; -------------------------------------------------------------------
;            Initialize the 8 TMS9118 VDP Registers
; -------------------------------------------------------------------

INIT_VREG:  mov  rf, VREG_SET
            ldi  80h
            plo  r7
NEXTREG:    lda  rf
            str  r2
            out  VDP_REG_P
            dec  r2
            glo  r7
            str  r2
            out  VDP_REG_P
            dec  r2
            inc  r7
            glo  r7
            smi  88h
            lbnz NEXTREG
            rtn

; -----------------------------------------------------------
;         Select VDP destination address for sending
; -----------------------------------------------------------
; Note:   Selected VDP address must have bit 14 set,
;         so add 4000h to any VDP address selection.
;         e.g. VDP colortable address 2000h-> 6000h  
; -----------------------------------------------------------
SELECT_VDP_ADDR:
            lda  r6
            phi  rf
            lda  r6
            plo  rf            ; rf has address from linkage
            str  r2
            out  VDP_REG_P     ; send low byte of address
            dec  r2
            ghi  rf
            str  r2
            out  VDP_REG_P     ; and then high byte
            dec  r2
            rtn


; -------------------------------------------------------------------
;          Copy text data to vram Pattern Table @ 0000h
; -------------------------------------------------------------------
INIT_CHARACTER_SET:
            call SELECT_VDP_ADDR
            dw   4000h         ; set VDP write address to 0000h
     
            ; now we copy data to 0000h (Pattern table)
            mov  rf, START_CHARACTER_SET
            
            ; r7 is number of bytes to transfer
            mov  r7, END_CHARACTER_SET-START_CHARACTER_SET 
NEXT_BYTE:  lda  rf
            str  r2
            out  VDP_DAT_P     ; VDP will autoincrement VRAM address
            dec  r2
            dec  r7
            glo  r7
            lbnz NEXT_BYTE
            ghi  r7
            lbnz NEXT_BYTE
            rtn


; -------------------------------------------------------------------
;             Display selected character set
; -------------------------------------------------------------------

DISP_CHARSET:
#ifdef TI99_CHARSET
            mov  r7, 190       ; number of chars to write (2*sizeof(charset))
#endif

#ifdef CP437_CHARSET
            mov  r7, 512       ; number of chars to write (2*sizeof(charset))
#endif
            ldi  0             ; starting index
            plo  r8
CHAR_IDX:   glo  r8
            str  r2
            out  VDP_DAT_P
            dec  r2
            inc  r8
            
            ; put blank space between each character
            ldi  0
            str  r2
            out  VDP_DAT_P
            dec  r2
            dec  r7
            dec  r7   ; two decrements since we have sent 2 chars
            glo  r7
            lbnz CHAR_IDX
            ghi  r7
            lbnz CHAR_IDX
            rtn

; -------------------------------------------------------------------
;        Position 'cursor' to x,y for subsequent printing
; -------------------------------------------------------------------
            ; multiply column (y) and add row (x) to get address offset
            ; ofs = 40*y + x since we have 40 chars per line
GOTOXY:     ldi  0
            phi  rf
            lda  r6
            plo  rf         ; rf = x
            push rf
            ldi  40
            phi  rf
            lda  r6
            plo  rf         ; rf.1 = 40;  rf.0 = y
            
            call MULT8      ; rd = rf.O x rf.1 i.e. rd = 40*y
            pop  rf         ; rf = x
            call ADD16      ; rd = 40*y + x
            
            ; add VDP offset 
            mov  rf, 4800h  ; 4800h is VDP address 800h
            call ADD16      ; rd = 40*y + x + 4800h
            
            ; send address to VDP
            glo  rd
            str  r2
            out  VDP_REG_P
            dec  r2
            ghi  rd
            str  r2
            out  VDP_REG_P
            dec  r2
            rtn

; -----------------------------------------------------------
;        Type asciiz string at positioned cursor
; -----------------------------------------------------------

TYPE_STRING:lda  rf
            lbz   EXITTYPE
#ifdef TI99_CHARSET
            smi  32
#endif
            str  r2
            out  VDP_DAT_P
            dec  r2
            lbr  TYPE_STRING  
EXITTYPE:   rtn


; -------------------------------------------------------------------
;           Set textcolor foreground/background
; -------------------------------------------------------------------

SET_TEXTCOLOR:  
            ldn  rc            ; rc points to color data
            str  r2
            out  VDP_REG_P
            dec  r2
            ldi  87h           ; send to VREG7
            str  r2
            out  VDP_REG_P
            dec  r2
            rtn


; -----------------------------------------------------------
;                     Clear VDP memory
; -----------------------------------------------------------

CLEAR_VRAM: call SELECT_VDP_ADDR
            dw   4000h         ; set VDP write address to 0000h

            mov  r7, 4000h     ; 16k memory
CLEAR_NXT:  ldi  0
            str  r2
            out  VDP_DAT_P     ; VDP performs autoincrement of VRAM address  
            dec  r2
            dec  r7
            glo  r7
            lbnz CLEAR_NXT
            ghi  r7
            lbnz CLEAR_NXT
            rtn
; -----------------------------------------------------------




; measured 99.994mS on 4mHz Pico Elf 2  22 June 2021
DELAY100MS: ldi  01Bh
            phi  r7
            ldi  0C7h
            plo  r7
D100:       dec  r7
            glo  r7
            lbnz D100
            ghi  r7
            lbnz D100
            rtn


;--------------------------------------------------------------
;          Math functions adapted from Tom Swan
;--------------------------------------------------------------

;--------------------------------------------------------------
; ADD16:       DOUBLE PRECISION ADDITION
; Author:      Tom Swan
;--------------------------------------------------------------
; INPUT:   --  RD = operand #1
;              RF = operand #2
;
; OUTPUT:  --  RD <-- RD + RF using double precision
;              DF indicates if overflow occurred
;
; CHANGES: --  RD
;--------------------------------------------------------------

ADD16:      glo  rd          ;Get low byte operand #1
            str  r2          ;Push onto stack @ M(R(2))
            glo  rf          ;Get low byte operand #2
            add              ;Add to byte on stack via R(X)
            plo  rd          ;Put result in RD.0
            ghi  rd          ;Get high byte operand #1
            str  r2          ;Push onto stack @ M(R(2))
            ghi  rf          ;Get high byte operand #2
            adc              ;Add with possible carry to M(R(X))
            phi  rd          ;Put result in RD.1
            rtn              ;Return from subroutine


;--------------------------------------------------------------
;  MULT8:       MULTIPLY ROUTINE
;  Author:      Tom Swan
;--------------------------------------------------------------
;  INPUT:       RF.1 = multiplier
;               RF.0 = multiplicand
;
;  OUTPUT:   -- RD = RF.O x RF.1 using bit shifting
;
;  CHANGES:  -- RD RF
;--------------------------------------------------------------

MULT8:      ldi  0             ; Load 00 byte into D
            phi  rd            ; Put in RD.1 to initialize answer 
            glo  rf            ; Get multiplicand in RF.O
            str  r2            ; Push onto stack @ M(R(2))
            ldi  8             ; Load 08 into D
            plo  rf            ; Put in RF.O as a loop count (old 
                               ; RF.O on stack)
L1:         ghi  rf            ; Get multiplier from RF.1
            shr                ; Shift LSB into DF

            phi  rf            ; Put shifted value back in RF.1 
            ghi  rd            ; Get high byte of answer into D 
            lbnf SKIP          ; If DF = 0 then jump to skip 
                               ; next instruction
            add                ; Add multiplicand on stack to 
                               ; D on DF =1
SKIP:       shrc               ; Shift D right with carry
            phi  rd            ; Put in RD.2, high byte of answer 
            glo  rd            ; Get low byte of answer from RE.0 
            shrc               ; Shift with possible carry
            plo  rd            ; Put in RD.0 now double precision 
                               ; shifted right
            dec  rf            ; Decrement loop count in RF.O 
            glo  rf            ; Get loop count to test value 
            lbnz L1            ; If !=0, loop to test all eight 
                               ; bits of multiplier
            rtn                ; Return. 16 bit answer in RD


; *************************************
; *** Check if character is numeric ***
; *** D - char to check             ***
; *** Returns DF=1 if numeric       ***
; ***         DF=0 if not           ***
; *** from M H Riley BIOS source    ***
; *************************************
isnum:     plo     re                  ; save a copy
           smi     '0'                 ; check for below zero
           bnf     fails               ; jump if below
           smi     10                  ; see if above
           bdf     fails               ; fails if so
passes:    smi     0                   ; signal success
           lskp
fails:     adi     0                   ; signal failure
           glo     re                  ; recover character
           sep     sret                ; and return

;--------------- End of Math functions ------------------------


;--------------------------------------------------------------
;    Read byte from UART if char available  
;    return in rf.0 - else return null
;
;    from original bios code of Bob Armstrong
;    modified for non-blocking console input
;--------------------------------------------------------------
INKEY:  ldi     015h            ; need UART line status register
        str     r2              ; prepare for out
        out     UART_SELECT     ; write to register select port
        dec     r2              ; correct for inc on out
        inp     UART_DATA       ; read line status register
        ani     1               ; mask for data ready bit
        lbz     NOKEY           ; return if no bytes to read
        ldi     010h            ; select data register
        str     r2              ; prepare for out
        out     UART_SELECT     ; write to register select port
        dec     r2              ; back to free spot
        inp     UART_DATA       ; read UART data register
        plo     rf
        rtn
        
NOKEY:  ldi     0h
        plo     rf
        rtn

        org 2300h

           ; default VDP register settings for textmode
VREG_SET:   db  0      ; VR0 text mode, no ext video
            db  0d0h   ; VR1 16k vram, display enabled, interrupts disabled
            db  2h     ; VR2 Name table address 0800h
            db  0      ; VR3 No color table - see VR7
            db  0      ; VR4 Pattern table address 0000h
            db  20h    ; VR5 Sprite attribute table address 1000h - ignored
            db  0      ; VR6 Sprite pattern table address 0h - ignored
            db  0f4h   ; VR7 White text (F) on blue background (4)

TXTCOLORS:  db  0fdh   ; VR7 White text (F) on magenta background (D)
            db  0f6h   ; VR7 White text (F) on dark red background (6)
            db  0f1h   ; VR7 White text (F) on black background (1)
            db  0fch   ; VR7 White text (F) on dark green background (C)
            db  0f4h   ; VR7 White text (F) on blue background (4)
            db  01fh   ; VR7 Black text (1) on white background (F)
            db  012h   ; VR7 Black text (1) on green background (2)
            db  017h   ; VR7 Black text (1) on cyan background (7)
            db  01dh   ; VR7 Black text (1) on magenta background (D)
            db  019h   ; VR7 Black text (F) on lt red background (9)

            ; lesser quality combinations
            ;db  074h   ; VR7 Cyan text (1) on blue background (4)
            ;db  04fh   ; VR7 Blue text (4) on white background (F)
            ;db  0a6h   ; VR7 Yellow text (A) on dark red (6)
            ;db  0a1h   ; VR7 Yellow text (A) on black background (1)
            ;db  0a4h   ; VR7 Yellow text (A) on blue background (4) **very poor**

TITLE_STR:  db 'TMS9118 Text Mode', 0       ; 17 chars

#ifdef CP437_CHARSET
CHAR_STR:   db 'CP437 Character Set:', 0    ; 20 chars
#endif

#ifdef TI99_CHARSET
CHAR_STR:   db 'TI99/4a Character Set:', 0  ; 22 chars
#endif

PROMPT_STR: db 'Type 0..9 to change color - x to exit', 0 ; 37 chars

C0_STR:     db 'White / Magenta', 0       ; 16 chars including null
C1_STR:     db 'White / Red    ', 0
C2_STR:     db 'White / Black  ', 0
C3_STR:     db 'White / Dk Grn ', 0
C4_STR:     db 'White / Blue   ', 0
C5_STR:     db 'Black / White  ', 0
C6_STR:     db 'Black / Green  ', 0
C7_STR:     db 'Black / Cyan   ', 0
C8_STR:     db 'Black / Magenta', 0
C9_STR:     db 'Black / Lt Red ', 0


        org 2420h
START_CHARACTER_SET:
#ifdef TI99_CHARSET
               ; TI99 symbols and upper case                 ; note index is -32!
            db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  ; Char000  space
            db 010h, 010h, 010h, 010h, 010h, 000h, 010h, 000h,  ; Char001  !
            db 028h, 028h, 028h, 000h, 000h, 000h, 000h, 000h,  ; Char002  dbl quotes
            db 028h, 028h, 07ch, 028h, 07ch, 028h, 028h, 000h,  ; Char003  #
            db 038h, 054h, 050h, 038h, 014h, 054h, 038h, 000h,  ; Char004  $
            db 060h, 064h, 008h, 010h, 020h, 04ch, 00ch, 000h,  ; Char005  %
            db 020h, 050h, 050h, 020h, 054h, 048h, 034h, 000h,  ; Char006  &
            db 008h, 008h, 010h, 000h, 000h, 000h, 000h, 000h,  ; Char007  '
            db 008h, 010h, 020h, 020h, 020h, 010h, 008h, 000h,  ; Char008  (
            db 020h, 010h, 008h, 008h, 008h, 010h, 020h, 000h,  ; Char009  )
            db 000h, 028h, 010h, 07ch, 010h, 028h, 000h, 000h,  ; Char010  *
            db 000h, 010h, 010h, 07ch, 010h, 010h, 000h, 000h,  ; Char011  +
            db 000h, 000h, 000h, 000h, 030h, 010h, 020h, 000h,  ; Char012  ,
            db 000h, 000h, 000h, 07ch, 000h, 000h, 000h, 000h,  ; Char013  -
            db 000h, 000h, 000h, 000h, 000h, 030h, 030h, 000h,  ; Char014  .
            db 000h, 004h, 008h, 010h, 020h, 040h, 000h, 000h,  ; Char015  /
            db 038h, 044h, 044h, 044h, 044h, 044h, 038h, 000h,  ; Char016  0
            db 010h, 030h, 010h, 010h, 010h, 010h, 038h, 000h,  ; Char017  1
            db 038h, 044h, 004h, 008h, 010h, 020h, 07ch, 000h,  ; Char018  2
            db 038h, 044h, 004h, 018h, 004h, 044h, 038h, 000h,  ; Char019  3
            db 008h, 018h, 028h, 048h, 07ch, 008h, 008h, 000h,  ; Char020  4
            db 07ch, 040h, 078h, 004h, 004h, 044h, 038h, 000h,  ; Char021  5
            db 018h, 020h, 040h, 078h, 044h, 044h, 038h, 000h,  ; Char022  6
            db 07ch, 004h, 008h, 010h, 020h, 020h, 020h, 000h,  ; Char023  7
            db 038h, 044h, 044h, 038h, 044h, 044h, 038h, 000h,  ; Char024  8
            db 038h, 044h, 044h, 03ch, 004h, 008h, 030h, 000h,  ; Char025  9
            db 000h, 030h, 030h, 000h, 030h, 030h, 000h, 000h,  ; Char026  :
            db 000h, 030h, 030h, 000h, 030h, 010h, 020h, 000h,  ; Char027  ;
            db 008h, 010h, 020h, 040h, 020h, 010h, 008h, 000h,  ; Char028  <
            db 000h, 000h, 07ch, 000h, 07ch, 000h, 000h, 000h,  ; Char029  =
            db 020h, 010h, 008h, 004h, 008h, 010h, 020h, 000h,  ; Char030  >
            db 038h, 044h, 004h, 008h, 010h, 000h, 010h, 000h,  ; Char031  ?
            db 038h, 044h, 05ch, 054h, 05ch, 040h, 038h, 000h,  ; Char032  @
            db 038h, 044h, 044h, 07ch, 044h, 044h, 044h, 000h,  ; Char033  A
            db 078h, 024h, 024h, 038h, 024h, 024h, 078h, 000h,  ; Char034  B
            db 038h, 044h, 040h, 040h, 040h, 044h, 038h, 000h,  ; Char035  C
            db 078h, 024h, 024h, 024h, 024h, 024h, 078h, 000h,  ; Char036  D
            db 07ch, 040h, 040h, 078h, 040h, 040h, 07ch, 000h,  ; Char037  E
            db 07ch, 040h, 040h, 078h, 040h, 040h, 040h, 000h,  ; Char038  F
            db 03ch, 040h, 040h, 05ch, 044h, 044h, 038h, 000h,  ; Char039  G
            db 044h, 044h, 044h, 07ch, 044h, 044h, 044h, 000h,  ; Char040  H
            db 038h, 010h, 010h, 010h, 010h, 010h, 038h, 000h,  ; Char041  I
            db 004h, 004h, 004h, 004h, 004h, 044h, 038h, 000h,  ; Char042  J
            db 044h, 048h, 050h, 060h, 050h, 048h, 044h, 000h,  ; Char043  K
            db 040h, 040h, 040h, 040h, 040h, 040h, 07ch, 000h,  ; Char044  L
            db 044h, 06ch, 054h, 054h, 044h, 044h, 044h, 000h,  ; Char045  M
            db 044h, 064h, 064h, 054h, 04ch, 04ch, 044h, 000h,  ; Char046  N
            db 07ch, 044h, 044h, 044h, 044h, 044h, 07ch, 000h,  ; Char047  O
            db 078h, 044h, 044h, 078h, 040h, 040h, 040h, 000h,  ; Char048  P
            db 038h, 044h, 044h, 044h, 054h, 048h, 034h, 000h,  ; Char049  Q
            db 078h, 044h, 044h, 078h, 050h, 048h, 044h, 000h,  ; Char050  R
            db 038h, 044h, 040h, 038h, 004h, 044h, 038h, 000h,  ; Char051  S
            db 07ch, 010h, 010h, 010h, 010h, 010h, 010h, 000h,  ; Char052  T
            db 044h, 044h, 044h, 044h, 044h, 044h, 038h, 000h,  ; Char053  U
            db 044h, 044h, 044h, 028h, 028h, 010h, 010h, 000h,  ; Char054  V
            db 044h, 044h, 044h, 054h, 054h, 054h, 028h, 000h,  ; Char055  W
            db 044h, 044h, 028h, 010h, 028h, 044h, 044h, 000h,  ; Char056  X
            db 044h, 044h, 028h, 010h, 010h, 010h, 010h, 000h,  ; Char057  Y
            db 07ch, 004h, 008h, 010h, 020h, 040h, 07ch, 000h,  ; Char058  Z
            db 038h, 020h, 020h, 020h, 020h, 020h, 038h, 000h,  ; Char059  [
            db 000h, 040h, 020h, 010h, 008h, 004h, 000h, 000h,  ; Char060  \
            db 038h, 008h, 008h, 008h, 008h, 008h, 038h, 000h,  ; Char061  ]
            db 000h, 010h, 028h, 044h, 000h, 000h, 000h, 000h,  ; Char062  ^
            db 000h, 000h, 000h, 000h, 000h, 000h, 07ch, 000h,  ; Char063  _
            db 000h, 020h, 010h, 008h, 000h, 000h, 000h, 000h,  ; Char064  `
                ; TI99 lower case
            db 000h, 000h, 038h, 044h, 07ch, 044h, 044h, 000h,  ; Char065  a
            db 000h, 000h, 078h, 024h, 038h, 024h, 078h, 000h,  ; Char066  b
            db 000h, 000h, 03ch, 040h, 040h, 040h, 03ch, 000h,  ; Char067  c
            db 000h, 000h, 078h, 024h, 024h, 024h, 078h, 000h,  ; Char068  d
            db 000h, 000h, 07ch, 040h, 078h, 040h, 07ch, 000h,  ; Char069  e
            db 000h, 000h, 07ch, 040h, 078h, 040h, 040h, 000h,  ; Char070  f
            db 000h, 000h, 03ch, 040h, 05ch, 044h, 038h, 000h,  ; Char071  g
            db 000h, 000h, 044h, 044h, 07ch, 044h, 044h, 000h,  ; Char072  h
            db 000h, 000h, 038h, 010h, 010h, 010h, 038h, 000h,  ; Char073  i
            db 000h, 000h, 008h, 008h, 008h, 048h, 030h, 000h,  ; Char074  j
            db 000h, 000h, 024h, 028h, 030h, 028h, 024h, 000h,  ; Char075  k
            db 000h, 000h, 040h, 040h, 040h, 040h, 07ch, 000h,  ; Char076  l
            db 000h, 000h, 044h, 06ch, 054h, 044h, 044h, 000h,  ; Char077  m
            db 000h, 000h, 044h, 064h, 054h, 04ch, 044h, 000h,  ; Char078  n
            db 000h, 000h, 07ch, 044h, 044h, 044h, 07ch, 000h,  ; Char079  o
            db 000h, 000h, 078h, 044h, 078h, 040h, 040h, 000h,  ; Char080  p
            db 000h, 000h, 038h, 044h, 054h, 048h, 034h, 000h,  ; Char081  q
            db 000h, 000h, 078h, 044h, 078h, 048h, 044h, 000h,  ; Char082  r
            db 000h, 000h, 03ch, 040h, 038h, 004h, 078h, 000h,  ; Char083  s
            db 000h, 000h, 07ch, 010h, 010h, 010h, 010h, 000h,  ; Char084  t
            db 000h, 000h, 044h, 044h, 044h, 044h, 038h, 000h,  ; Char085  u
            db 000h, 000h, 044h, 044h, 028h, 028h, 010h, 000h,  ; Char086  v
            db 000h, 000h, 044h, 044h, 054h, 054h, 028h, 000h,  ; Char087  w
            db 000h, 000h, 044h, 028h, 010h, 028h, 044h, 000h,  ; Char088  x
            db 000h, 000h, 044h, 028h, 010h, 010h, 010h, 000h,  ; Char089  y
            db 000h, 000h, 07ch, 008h, 010h, 020h, 07ch, 000h,  ; Char090  z
            db 018h, 020h, 020h, 040h, 020h, 020h, 018h, 000h,  ; Char091  {
            db 010h, 010h, 010h, 000h, 010h, 010h, 010h, 000h,  ; Char092  |
            db 030h, 008h, 008h, 004h, 008h, 008h, 030h, 000h,  ; Char093  }
            db 000h, 020h, 054h, 008h, 000h, 000h, 000h, 000h   ; Char094  ~
#endif

#ifdef CP437_CHARSET
               ; CP437 character set
            db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  ; Char000  null
            db 038h, 044h, 06ch, 044h, 054h, 044h, 038h, 000h,  ; Char001
            db 038h, 07ch, 054h, 07ch, 044h, 07ch, 038h, 000h,  ; Char002
            db 000h, 028h, 07ch, 07ch, 07ch, 038h, 010h, 000h,  ; Char003
            db 000h, 010h, 038h, 07ch, 07ch, 038h, 010h, 000h,  ; Char004
            db 010h, 038h, 038h, 010h, 07ch, 07ch, 010h, 000h,  ; Char005
            db 000h, 010h, 038h, 07ch, 07ch, 010h, 038h, 000h,  ; Char006
            db 000h, 000h, 000h, 030h, 030h, 000h, 000h, 000h,  ; Char007
            db 0fch, 0fch, 0fch, 0cch, 0cch, 0fch, 0fch, 0fch,  ; Char008
            db 000h, 000h, 078h, 048h, 048h, 078h, 000h, 000h,  ; Char009
            db 0fch, 0fch, 084h, 0b4h, 0b4h, 084h, 0fch, 0fch,  ; Char010
            db 000h, 01ch, 00ch, 034h, 048h, 048h, 030h, 000h,  ; Char011
            db 038h, 044h, 044h, 038h, 010h, 038h, 010h, 000h,  ; Char012
            db 010h, 018h, 014h, 010h, 030h, 070h, 060h, 000h,  ; Char013
            db 00ch, 034h, 02ch, 034h, 02ch, 06ch, 060h, 000h,  ; Char014
            db 000h, 054h, 038h, 06ch, 038h, 054h, 000h, 000h,  ; Char015
            db 020h, 030h, 038h, 03ch, 038h, 030h, 020h, 000h,  ; Char016
            db 008h, 018h, 038h, 078h, 038h, 018h, 008h, 000h,  ; Char017
            db 010h, 038h, 07ch, 010h, 07ch, 038h, 010h, 000h,  ; Char018
            db 028h, 028h, 028h, 028h, 028h, 000h, 028h, 000h,  ; Char019
            db 03ch, 054h, 054h, 034h, 014h, 014h, 014h, 000h,  ; Char020
            db 038h, 044h, 030h, 028h, 018h, 044h, 038h, 000h,  ; Char021
            db 000h, 000h, 000h, 000h, 000h, 078h, 078h, 000h,  ; Char022
            db 010h, 038h, 07ch, 010h, 07ch, 038h, 010h, 038h,  ; Char023
            db 010h, 038h, 07ch, 010h, 010h, 010h, 010h, 000h,  ; Char024
            db 010h, 010h, 010h, 010h, 07ch, 038h, 010h, 000h,  ; Char025
            db 000h, 010h, 018h, 07ch, 018h, 010h, 000h, 000h,  ; Char026
            db 000h, 010h, 030h, 07ch, 030h, 010h, 000h, 000h,  ; Char027
            db 000h, 000h, 000h, 040h, 040h, 040h, 07ch, 000h,  ; Char028
            db 000h, 028h, 028h, 07ch, 028h, 028h, 000h, 000h,  ; Char029
            db 010h, 010h, 038h, 038h, 07ch, 07ch, 000h, 000h,  ; Char030
            db 07ch, 07ch, 038h, 038h, 010h, 010h, 000h, 000h,  ; Char031
            db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  ; Char032
            db 010h, 038h, 038h, 010h, 010h, 000h, 010h, 000h,  ; Char033
            db 06ch, 06ch, 048h, 000h, 000h, 000h, 000h, 000h,  ; Char034
            db 000h, 028h, 07ch, 028h, 028h, 07ch, 028h, 000h,  ; Char035
            db 020h, 038h, 040h, 030h, 008h, 070h, 010h, 000h,  ; Char036
            db 064h, 064h, 008h, 010h, 020h, 04ch, 04ch, 000h,  ; Char037
            db 020h, 050h, 050h, 020h, 054h, 048h, 034h, 000h,  ; Char038
            db 030h, 030h, 020h, 000h, 000h, 000h, 000h, 000h,  ; Char039
            db 010h, 020h, 020h, 020h, 020h, 020h, 010h, 000h,  ; Char040
            db 020h, 010h, 010h, 010h, 010h, 010h, 020h, 000h,  ; Char041
            db 000h, 028h, 038h, 07ch, 038h, 028h, 000h, 000h,  ; Char042
            db 000h, 010h, 010h, 07ch, 010h, 010h, 000h, 000h,  ; Char043
            db 000h, 000h, 000h, 000h, 000h, 030h, 030h, 020h,  ; Char044
            db 000h, 000h, 000h, 07ch, 000h, 000h, 000h, 000h,  ; Char045
            db 000h, 000h, 000h, 000h, 000h, 030h, 030h, 000h,  ; Char046
            db 000h, 004h, 008h, 010h, 020h, 040h, 000h, 000h,  ; Char047
            db 038h, 044h, 04ch, 054h, 064h, 044h, 038h, 000h,  ; Char048
            db 010h, 030h, 010h, 010h, 010h, 010h, 038h, 000h,  ; Char049
            db 038h, 044h, 004h, 018h, 020h, 040h, 07ch, 000h,  ; Char050
            db 038h, 044h, 004h, 038h, 004h, 044h, 038h, 000h,  ; Char051
            db 008h, 018h, 028h, 048h, 07ch, 008h, 008h, 000h,  ; Char052
            db 07ch, 040h, 040h, 078h, 004h, 044h, 038h, 000h,  ; Char053
            db 018h, 020h, 040h, 078h, 044h, 044h, 038h, 000h,  ; Char054
            db 07ch, 004h, 008h, 010h, 020h, 020h, 020h, 000h,  ; Char055
            db 038h, 044h, 044h, 038h, 044h, 044h, 038h, 000h,  ; Char056
            db 038h, 044h, 044h, 03ch, 004h, 008h, 030h, 000h,  ; Char057
            db 000h, 000h, 030h, 030h, 000h, 030h, 030h, 000h,  ; Char058
            db 000h, 000h, 030h, 030h, 000h, 030h, 030h, 020h,  ; Char059
            db 008h, 010h, 020h, 040h, 020h, 010h, 008h, 000h,  ; Char060
            db 000h, 000h, 07ch, 000h, 000h, 07ch, 000h, 000h,  ; Char061
            db 020h, 010h, 008h, 004h, 008h, 010h, 020h, 000h,  ; Char062
            db 038h, 044h, 004h, 018h, 010h, 000h, 010h, 000h,  ; Char063
            db 038h, 044h, 05ch, 054h, 05ch, 040h, 038h, 000h,  ; Char064
            db 038h, 044h, 044h, 044h, 07ch, 044h, 044h, 000h,  ; Char065
            db 078h, 044h, 044h, 078h, 044h, 044h, 078h, 000h,  ; Char066
            db 038h, 044h, 040h, 040h, 040h, 044h, 038h, 000h,  ; Char067
            db 078h, 044h, 044h, 044h, 044h, 044h, 078h, 000h,  ; Char068
            db 07ch, 040h, 040h, 078h, 040h, 040h, 07ch, 000h,  ; Char069
            db 07ch, 040h, 040h, 078h, 040h, 040h, 040h, 000h,  ; Char070
            db 038h, 044h, 040h, 05ch, 044h, 044h, 03ch, 000h,  ; Char071
            db 044h, 044h, 044h, 07ch, 044h, 044h, 044h, 000h,  ; Char072
            db 038h, 010h, 010h, 010h, 010h, 010h, 038h, 000h,  ; Char073
            db 004h, 004h, 004h, 004h, 044h, 044h, 038h, 000h,  ; Char074
            db 044h, 048h, 050h, 060h, 050h, 048h, 044h, 000h,  ; Char075
            db 040h, 040h, 040h, 040h, 040h, 040h, 07ch, 000h,  ; Char076
            db 044h, 06ch, 054h, 044h, 044h, 044h, 044h, 000h,  ; Char077
            db 044h, 064h, 054h, 04ch, 044h, 044h, 044h, 000h,  ; Char078
            db 038h, 044h, 044h, 044h, 044h, 044h, 038h, 000h,  ; Char079
            db 078h, 044h, 044h, 078h, 040h, 040h, 040h, 000h,  ; Char080
            db 038h, 044h, 044h, 044h, 054h, 048h, 034h, 000h,  ; Char081
            db 078h, 044h, 044h, 078h, 048h, 044h, 044h, 000h,  ; Char082
            db 038h, 044h, 040h, 038h, 004h, 044h, 038h, 000h,  ; Char083
            db 07ch, 010h, 010h, 010h, 010h, 010h, 010h, 000h,  ; Char084
            db 044h, 044h, 044h, 044h, 044h, 044h, 038h, 000h,  ; Char085
            db 044h, 044h, 044h, 044h, 044h, 028h, 010h, 000h,  ; Char086
            db 044h, 044h, 054h, 054h, 054h, 054h, 028h, 000h,  ; Char087
            db 044h, 044h, 028h, 010h, 028h, 044h, 044h, 000h,  ; Char088
            db 044h, 044h, 044h, 028h, 010h, 010h, 010h, 000h,  ; Char089
            db 078h, 008h, 010h, 020h, 040h, 040h, 078h, 000h,  ; Char090
            db 038h, 020h, 020h, 020h, 020h, 020h, 038h, 000h,  ; Char091
            db 000h, 040h, 020h, 010h, 008h, 004h, 000h, 000h,  ; Char092
            db 038h, 008h, 008h, 008h, 008h, 008h, 038h, 000h,  ; Char093
            db 010h, 028h, 044h, 000h, 000h, 000h, 000h, 000h,  ; Char094
            db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0fch,  ; Char095
            db 030h, 030h, 010h, 000h, 000h, 000h, 000h, 000h,  ; Char096
            db 000h, 000h, 038h, 004h, 03ch, 044h, 03ch, 000h,  ; Char097
            db 040h, 040h, 078h, 044h, 044h, 044h, 078h, 000h,  ; Char098
            db 000h, 000h, 038h, 044h, 040h, 044h, 038h, 000h,  ; Char099
            db 004h, 004h, 03ch, 044h, 044h, 044h, 03ch, 000h,  ; Char100
            db 000h, 000h, 038h, 044h, 078h, 040h, 038h, 000h,  ; Char101
            db 018h, 020h, 020h, 078h, 020h, 020h, 020h, 000h,  ; Char102
            db 000h, 000h, 03ch, 044h, 044h, 03ch, 004h, 038h,  ; Char103
            db 040h, 040h, 070h, 048h, 048h, 048h, 048h, 000h,  ; Char104
            db 010h, 000h, 010h, 010h, 010h, 010h, 018h, 000h,  ; Char105
            db 008h, 000h, 018h, 008h, 008h, 008h, 048h, 030h,  ; Char106
            db 040h, 040h, 048h, 050h, 060h, 050h, 048h, 000h,  ; Char107
            db 010h, 010h, 010h, 010h, 010h, 010h, 018h, 000h,  ; Char108
            db 000h, 000h, 068h, 054h, 054h, 044h, 044h, 000h,  ; Char109
            db 000h, 000h, 070h, 048h, 048h, 048h, 048h, 000h,  ; Char110
            db 000h, 000h, 038h, 044h, 044h, 044h, 038h, 000h,  ; Char111
            db 000h, 000h, 078h, 044h, 044h, 044h, 078h, 040h,  ; Char112
            db 000h, 000h, 03ch, 044h, 044h, 044h, 03ch, 004h,  ; Char113
            db 000h, 000h, 058h, 024h, 020h, 020h, 070h, 000h,  ; Char114
            db 000h, 000h, 038h, 040h, 038h, 004h, 038h, 000h,  ; Char115
            db 000h, 020h, 078h, 020h, 020h, 028h, 010h, 000h,  ; Char116
            db 000h, 000h, 048h, 048h, 048h, 058h, 028h, 000h,  ; Char117
            db 000h, 000h, 044h, 044h, 044h, 028h, 010h, 000h,  ; Char118
            db 000h, 000h, 044h, 044h, 054h, 07ch, 028h, 000h,  ; Char119
            db 000h, 000h, 048h, 048h, 030h, 048h, 048h, 000h,  ; Char120
            db 000h, 000h, 048h, 048h, 048h, 038h, 010h, 060h,  ; Char121
            db 000h, 000h, 078h, 008h, 030h, 040h, 078h, 000h,  ; Char122
            db 018h, 020h, 020h, 060h, 020h, 020h, 018h, 000h,  ; Char123
            db 010h, 010h, 010h, 000h, 010h, 010h, 010h, 000h,  ; Char124
            db 030h, 008h, 008h, 00ch, 008h, 008h, 030h, 000h,  ; Char125
            db 028h, 050h, 000h, 000h, 000h, 000h, 000h, 000h,  ; Char126
            db 010h, 038h, 06ch, 044h, 044h, 07ch, 000h, 000h,  ; Char127
            db 038h, 044h, 040h, 040h, 044h, 038h, 010h, 030h,  ; Char128
            db 048h, 000h, 048h, 048h, 048h, 058h, 028h, 000h,  ; Char129
            db 00ch, 000h, 038h, 044h, 078h, 040h, 038h, 000h,  ; Char130
            db 038h, 000h, 038h, 004h, 03ch, 044h, 03ch, 000h,  ; Char131
            db 028h, 000h, 038h, 004h, 03ch, 044h, 03ch, 000h,  ; Char132
            db 030h, 000h, 038h, 004h, 03ch, 044h, 03ch, 000h,  ; Char133
            db 038h, 028h, 038h, 004h, 03ch, 044h, 03ch, 000h,  ; Char134
            db 000h, 038h, 044h, 040h, 044h, 038h, 010h, 030h,  ; Char135
            db 038h, 000h, 038h, 044h, 078h, 040h, 038h, 000h,  ; Char136
            db 028h, 000h, 038h, 044h, 078h, 040h, 038h, 000h,  ; Char137
            db 030h, 000h, 038h, 044h, 078h, 040h, 038h, 000h,  ; Char138
            db 028h, 000h, 010h, 010h, 010h, 010h, 018h, 000h,  ; Char139
            db 010h, 028h, 000h, 010h, 010h, 010h, 018h, 000h,  ; Char140
            db 020h, 000h, 010h, 010h, 010h, 010h, 018h, 000h,  ; Char141
            db 028h, 000h, 010h, 028h, 044h, 07ch, 044h, 000h,  ; Char142
            db 038h, 028h, 038h, 06ch, 044h, 07ch, 044h, 000h,  ; Char143
            db 00ch, 000h, 07ch, 040h, 078h, 040h, 07ch, 000h,  ; Char144
            db 000h, 000h, 078h, 014h, 07ch, 050h, 03ch, 000h,  ; Char145
            db 03ch, 050h, 050h, 07ch, 050h, 050h, 05ch, 000h,  ; Char146
            db 038h, 000h, 030h, 048h, 048h, 048h, 030h, 000h,  ; Char147
            db 028h, 000h, 030h, 048h, 048h, 048h, 030h, 000h,  ; Char148
            db 060h, 000h, 030h, 048h, 048h, 048h, 030h, 000h,  ; Char149
            db 038h, 000h, 048h, 048h, 048h, 058h, 028h, 000h,  ; Char150
            db 060h, 000h, 048h, 048h, 048h, 058h, 028h, 000h,  ; Char151
            db 028h, 000h, 048h, 048h, 048h, 038h, 010h, 060h,  ; Char152
            db 048h, 030h, 048h, 048h, 048h, 048h, 030h, 000h,  ; Char153
            db 028h, 000h, 048h, 048h, 048h, 048h, 030h, 000h,  ; Char154
            db 000h, 010h, 038h, 040h, 040h, 038h, 010h, 000h,  ; Char155
            db 018h, 024h, 020h, 078h, 020h, 024h, 05ch, 000h,  ; Char156
            db 044h, 028h, 010h, 07ch, 010h, 07ch, 010h, 000h,  ; Char157
            db 060h, 050h, 050h, 068h, 05ch, 048h, 048h, 000h,  ; Char158
            db 008h, 014h, 010h, 038h, 010h, 010h, 050h, 020h,  ; Char159
            db 018h, 000h, 038h, 004h, 03ch, 044h, 03ch, 000h,  ; Char160
            db 018h, 000h, 010h, 010h, 010h, 010h, 018h, 000h,  ; Char161
            db 018h, 000h, 030h, 048h, 048h, 048h, 030h, 000h,  ; Char162
            db 018h, 000h, 048h, 048h, 048h, 058h, 028h, 000h,  ; Char163
            db 028h, 050h, 000h, 070h, 048h, 048h, 048h, 000h,  ; Char164
            db 028h, 050h, 000h, 048h, 068h, 058h, 048h, 000h,  ; Char165
            db 038h, 004h, 03ch, 044h, 03ch, 000h, 03ch, 000h,  ; Char166
            db 030h, 048h, 048h, 048h, 030h, 000h, 078h, 000h,  ; Char167
            db 010h, 000h, 010h, 030h, 040h, 044h, 038h, 000h,  ; Char168
            db 000h, 000h, 07ch, 040h, 040h, 040h, 000h, 000h,  ; Char169
            db 000h, 000h, 0fch, 004h, 004h, 000h, 000h, 000h,  ; Char170
            db 040h, 048h, 050h, 038h, 044h, 008h, 01ch, 000h,  ; Char171
            db 040h, 048h, 050h, 02ch, 054h, 01ch, 004h, 000h,  ; Char172
            db 010h, 000h, 010h, 010h, 038h, 038h, 010h, 000h,  ; Char173
            db 000h, 000h, 024h, 048h, 024h, 000h, 000h, 000h,  ; Char174
            db 000h, 000h, 048h, 024h, 048h, 000h, 000h, 000h,  ; Char175
            db 054h, 000h, 0a8h, 000h, 054h, 000h, 0a8h, 000h,  ; Char176
            db 054h, 0a8h, 054h, 0a8h, 054h, 0a8h, 054h, 0a8h,  ; Char177
            db 0a8h, 0fch, 054h, 0fch, 0a8h, 0fch, 054h, 0fch,  ; Char178
            db 010h, 010h, 010h, 010h, 010h, 010h, 010h, 010h,  ; Char179
            db 010h, 010h, 010h, 0f0h, 010h, 010h, 010h, 010h,  ; Char180
            db 010h, 0f0h, 010h, 0f0h, 010h, 010h, 010h, 010h,  ; Char181
            db 050h, 050h, 050h, 0d0h, 050h, 050h, 050h, 050h,  ; Char182
            db 000h, 000h, 000h, 0f0h, 050h, 050h, 050h, 050h,  ; Char183
            db 000h, 0f0h, 010h, 0f0h, 010h, 010h, 010h, 010h,  ; Char184
            db 050h, 0d0h, 010h, 0d0h, 050h, 050h, 050h, 050h,  ; Char185
            db 050h, 050h, 050h, 050h, 050h, 050h, 050h, 050h,  ; Char186
            db 000h, 0f0h, 010h, 0d0h, 050h, 050h, 050h, 050h,  ; Char187
            db 050h, 0d0h, 010h, 0f0h, 000h, 000h, 000h, 000h,  ; Char188
            db 050h, 050h, 050h, 0f0h, 000h, 000h, 000h, 000h,  ; Char189
            db 010h, 0f0h, 010h, 0f0h, 000h, 000h, 000h, 000h,  ; Char190
            db 000h, 000h, 000h, 0f0h, 010h, 010h, 010h, 010h,  ; Char191
            db 010h, 010h, 010h, 01ch, 000h, 000h, 000h, 000h,  ; Char192
            db 010h, 010h, 010h, 0fch, 000h, 000h, 000h, 000h,  ; Char193
            db 000h, 000h, 000h, 0fch, 010h, 010h, 010h, 010h,  ; Char194
            db 010h, 010h, 010h, 01ch, 010h, 010h, 010h, 010h,  ; Char195
            db 000h, 000h, 000h, 0fch, 000h, 000h, 000h, 000h,  ; Char196
            db 010h, 010h, 010h, 0fch, 010h, 010h, 010h, 010h,  ; Char197
            db 010h, 01ch, 010h, 01ch, 010h, 010h, 010h, 010h,  ; Char198
            db 050h, 050h, 050h, 05ch, 050h, 050h, 050h, 050h,  ; Char199
            db 050h, 05ch, 040h, 07ch, 000h, 000h, 000h, 000h,  ; Char200
            db 000h, 07ch, 040h, 05ch, 050h, 050h, 050h, 050h,  ; Char201
            db 050h, 0dch, 000h, 0fch, 000h, 000h, 000h, 000h,  ; Char202
            db 000h, 0fch, 000h, 0dch, 050h, 050h, 050h, 050h,  ; Char203
            db 050h, 05ch, 040h, 05ch, 050h, 050h, 050h, 050h,  ; Char204
            db 000h, 0fch, 000h, 0fch, 000h, 000h, 000h, 000h,  ; Char205
            db 050h, 0dch, 000h, 0dch, 050h, 050h, 050h, 050h,  ; Char206
            db 010h, 0fch, 000h, 0fch, 000h, 000h, 000h, 000h,  ; Char207
            db 050h, 050h, 050h, 0fch, 000h, 000h, 000h, 000h,  ; Char208
            db 000h, 0fch, 000h, 0fch, 010h, 010h, 010h, 010h,  ; Char209
            db 000h, 000h, 000h, 0fch, 050h, 050h, 050h, 050h,  ; Char210
            db 050h, 050h, 050h, 07ch, 000h, 000h, 000h, 000h,  ; Char211
            db 010h, 01ch, 010h, 01ch, 000h, 000h, 000h, 000h,  ; Char212
            db 000h, 01ch, 010h, 01ch, 010h, 010h, 010h, 010h,  ; Char213
            db 000h, 000h, 000h, 07ch, 050h, 050h, 050h, 050h,  ; Char214
            db 050h, 050h, 050h, 0dch, 050h, 050h, 050h, 050h,  ; Char215
            db 010h, 0fch, 000h, 0fch, 010h, 010h, 010h, 010h,  ; Char216
            db 010h, 010h, 010h, 0f0h, 000h, 000h, 000h, 000h,  ; Char217
            db 000h, 000h, 000h, 01ch, 010h, 010h, 010h, 010h,  ; Char218
            db 0fch, 0fch, 0fch, 0fch, 0fch, 0fch, 0fch, 0fch,  ; Char219
            db 000h, 000h, 000h, 000h, 0fch, 0fch, 0fch, 0fch,  ; Char220
            db 0e0h, 0e0h, 0e0h, 0e0h, 0e0h, 0e0h, 0e0h, 0e0h,  ; Char221
            db 01ch, 01ch, 01ch, 01ch, 01ch, 01ch, 01ch, 01ch,  ; Char222
            db 0fch, 0fch, 0fch, 0fch, 000h, 000h, 000h, 000h,  ; Char223
            db 000h, 000h, 034h, 048h, 048h, 034h, 000h, 000h,  ; Char224
            db 000h, 070h, 048h, 070h, 048h, 048h, 070h, 040h,  ; Char225
            db 078h, 048h, 040h, 040h, 040h, 040h, 040h, 000h,  ; Char226
            db 000h, 07ch, 028h, 028h, 028h, 028h, 028h, 000h,  ; Char227
            db 078h, 048h, 020h, 010h, 020h, 048h, 078h, 000h,  ; Char228
            db 000h, 000h, 03ch, 048h, 048h, 030h, 000h, 000h,  ; Char229
            db 000h, 000h, 048h, 048h, 048h, 070h, 040h, 040h,  ; Char230
            db 000h, 000h, 028h, 050h, 010h, 010h, 010h, 000h,  ; Char231
            db 038h, 010h, 038h, 044h, 038h, 010h, 038h, 000h,  ; Char232
            db 030h, 048h, 048h, 078h, 048h, 048h, 030h, 000h,  ; Char233
            db 000h, 038h, 044h, 044h, 028h, 028h, 06ch, 000h,  ; Char234
            db 030h, 040h, 020h, 010h, 038h, 048h, 030h, 000h,  ; Char235
            db 000h, 000h, 028h, 054h, 054h, 028h, 000h, 000h,  ; Char236
            db 000h, 010h, 038h, 054h, 054h, 038h, 010h, 000h,  ; Char237
            db 000h, 038h, 040h, 078h, 040h, 038h, 000h, 000h,  ; Char238
            db 000h, 030h, 048h, 048h, 048h, 048h, 000h, 000h,  ; Char239
            db 000h, 078h, 000h, 078h, 000h, 078h, 000h, 000h,  ; Char240
            db 000h, 010h, 038h, 010h, 000h, 038h, 000h, 000h,  ; Char241
            db 040h, 030h, 008h, 030h, 040h, 000h, 078h, 000h,  ; Char242
            db 008h, 030h, 040h, 030h, 008h, 000h, 078h, 000h,  ; Char243
            db 000h, 008h, 014h, 010h, 010h, 010h, 010h, 010h,  ; Char244
            db 010h, 010h, 010h, 010h, 010h, 050h, 020h, 000h,  ; Char245
            db 000h, 010h, 000h, 07ch, 000h, 010h, 000h, 000h,  ; Char246
            db 000h, 028h, 050h, 000h, 028h, 050h, 000h, 000h,  ; Char247
            db 030h, 048h, 048h, 030h, 000h, 000h, 000h, 000h,  ; Char248
            db 000h, 000h, 000h, 030h, 030h, 000h, 000h, 000h,  ; Char249
            db 000h, 000h, 000h, 020h, 000h, 000h, 000h, 000h,  ; Char250
            db 000h, 01ch, 010h, 010h, 050h, 050h, 020h, 000h,  ; Char251
            db 050h, 028h, 028h, 028h, 000h, 000h, 000h, 000h,  ; Char252
            db 060h, 010h, 020h, 070h, 000h, 000h, 000h, 000h,  ; Char253
            db 000h, 000h, 078h, 078h, 078h, 078h, 000h, 000h,  ; Char254
            db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h   ; Char255
#endif  ; CP437 character set

END_CHARACTER_SET:

END: