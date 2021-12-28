; -------------------------------------------------------------------
;
;                     PLOT plotPixel demo
;
; Plot pixel demo for PicoElf / MicroELF with the TMS9118 Color Board
; published by Michael H Riley:
;
;      http://www.elf-emulation.com/hardware/tms9918.png
;
; This program uses the TMS9118 graphics mode 2 for plotting pixels
; in 256x192 two color mode. 
;
; This software is copyright (C) 2021 by Glenn Jolly
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
;
;            Graphics 2 memory map
;            +-----------------+ 0000h
;            |  Pattern Table  |
;            |   6144 bytes    |
;            +-----------------+ 1800h
;            | Sprite Patterns |
;            |    512 bytes    |
;            +-----------------+ 2000h
;            |   Color Table   |
;            |   6144 bytes    |
;            +-----------------+ 3800h 
;            |    Name Table   |
;            |    768 bytes    |
;            +-----------------+ 3B00h 
;            |Sprite Attributes|
;            |    256 bytes    |
;            +-----------------+ 3C00h 
;            |     Unused      |
;            +-----------------+ 3FFFh 


;#define ASM02       ; uncomment for ASM02 assembler by M H Riley 
                     ; some changes may be needed in bios.inc

include    bios.inc

#define cr  13
#define lf  10

#define VDP_DAT_P    1         ; data port
#define VDP_REG_P    5         ; register and address port

;#define TI99_CHARSET           ; comment out or undef for CP437 font

#ifdef ASM02
; RCASM compatible pseudo ops
.op "PUSH","N","8$1 73 9$1 73"
.op "POP","N","60 72 B$1 F0 A$1"
.op "CALL","W","D4 H1 L1"
.op "RTN","","D5"
.op "MOV","NR","8$2 A$1 9$2 B$1"
.op "MOV","NW","F8 L2 A$1 F8 H2 B$1"
#endif

            org 2000H

START:      call INIT_VREG
            call CLEAR_VRAM
            call SEND_VDP_COLORS
            call SEND_VDP_NAME

            ; init plot data size
            mov  r7, END_PIXEL_DATA-PIXEL_DATA

            ; report number of data pairs (x,y points)
            mov  rd,r7
            call RSHIFT16      ; divide by 2
            call TYPE_NUM
            sep  scall
            dw   f_inmsg
            db   ' points read in for plot', cr, lf, 0

            mov  r8, PIXEL_DATA
            ldi  0
            phi  ra            ; treated below as 8 bit register
            phi  rb            ;          ..   
LPIX:       lda  r8            ; get coordinate pair ra.0, rb.0
            plo  ra            ; X
            lda  r8
            plo  rb            ; Y

            call CALC_PIXEL    ; rd has address, rc.0 the byte mask
            ldn  rd            ; get existing pixel pattern in memory
            str  2             ; save for 'or' operation
            glo  rc            ; get new pixel pattern
            or                 ; 'or' with original
            str  rd            ; save in memory device context
            
            ; check if done
            dec  r7
            dec  r7            ; since we have pairs of data
            glo  r7
            bnz  LPIX
            ghi  r7
            bnz  LPIX

            call SEND_VDP_PATTERN  ; blit the plot to screen

            ; Annotate plot
            call XY2GM2        ; convert coords from char to graphics address
            db   2, 3
            mov  rc, TITLE_BUF
            call TYPE_STRING

            call XY2GM2
            db   21, 3
            mov  rc, PLOT_NAME
            call TYPE_STRING

            call XY2GM2
            db   21, 4
            mov  rc, PLOT_RANGE
            call TYPE_STRING

            rtn

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
            plo  rf          ; rf has address from linkage
            str  r2
            out  VDP_REG_P   ; send low byte of address
            dec  r2
            ghi  rf
            str  r2
            out  VDP_REG_P   ; and then high byte
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
;        Copy pattern data to vram Pattern Table @ 0000h
; -----------------------------------------------------------
SEND_VDP_PATTERN:
            call SELECT_VDP_ADDR
            dw   4000h         ; set VDP write address to 0000h

            ; now we copy data to vram 0000h (Pattern table)
            mov  rf, START_BITMAP
            mov  r7, END_BITMAP-START_BITMAP  ; 6144 bytes
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


; -----------------------------------------------------------
;           Copy color data to vram @ 2000h (Color table)
; -----------------------------------------------------------
SEND_VDP_COLORS:
            call SELECT_VDP_ADDR
            dw   6000h         ; set VDP write address to 2000h

            ; now copy data
            mov  rf, START_COLORTABLE
            mov  r7, END_COLORTABLE-START_COLORTABLE  ; also 6144 bytes
NEXT_CLR:   lda  rf
            ;default is white on black stored in colortable (0f1h)
            ;ldi  0fch          ; temp test of white on dark green
            ;ldi  0f6h          ; temp test of white on dark red
            ;ldi  0f4h          ; temp test of white on blue
            str  r2
            out  VDP_DAT_P     ; VDP will autoincrement VRAM address
            dec  r2
            dec  r7
            glo  r7
            lbnz NEXT_CLR
            ghi  r7
            lbnz NEXT_CLR
            rtn
            
; -----------------------------------------------------------
; Set name table entries of vram @ 3800h (Name table)
; -----------------------------------------------------------
SEND_VDP_NAME:            
            call SELECT_VDP_ADDR
            dw   7800h         ; set VDP write address to 3800h

            ; fill with triplet series 0..255, 0..255, 0..255
            mov  r7, 768       ; number of entries to write
            ldi  0             ; starting index
            plo  r8
NAME_IDX:   glo  r8
            str  r2
            out  VDP_DAT_P
            dec  r2
            inc  r8
            dec  r7
            glo  r7
            lbnz  NAME_IDX
            ghi  r7
            lbnz  NAME_IDX
            rtn
            
;----------------------------------------------------------------------
;             Get VRAM address for setting pixel
; From TI VDP programming guide:
; Byte address = 8(INT(X/8)) + 256(INT(Y/8)) + R(Y/8)
; Where R(Y/8) is the remainder of (Y/8) and the INT operator truncates
; the divide by 8 for X & Y.  Data to write is obtained by taking R(X/8) 
; as an index to mask data in table:
; {080h, 040h, 020h, 010h, 008h, 004h, 002h, 001h} ; pixel to be set ON
;
; On entry X & Y are given by ra.0 & rb.0, high bytes already zeroed
; Returns byte address in rd and pixel bit mask in rc.0
; Calling function will bitwise 'or' the mask with any existing data.
;----------------------------------------------------------------------
CALC_PIXEL: push ra
            push rb
            push r8
            glo  rb        ; Y coord
            shr            ; div 8
            shr
            shr
            phi  rb        ; effective multiply by 256
            glo  rb
            ani  7         ; R(Y/8)
            plo  rb        
            mov  rd,rb     ; rd is now 256(INT(Y/8)) + R(Y/8)
            mov  rf, 0h
            glo  ra        ; X coord
            ani  0F8h      ; d == 8(INT(X/8))
            plo  rf

            call ADD16     ; rd is now 8(INT(X/8)) + 256(INT(Y/8)) + R(Y/8)
            mov  r8,rd     ; temp store address 

            ; get bitmask from pixelTable 
            mov  rf, 0h
            glo  ra
            ani  7         ; R(X/8)  our pixel index to be set (see above)
            plo  rf
            mov  rd, pixelTable
            call ADD16     ; rd+rf -> rd is now pointing to bit mask

            ldn  rd        ; retrieve bit mask rd points to
            plo  rc        ; rc returns bit mask

            mov  rf,r8     ; rf is address stored above
            mov  rd, START_BITMAP ; memory device context (MDC)
            call ADD16     ; offset added -> rd returns address
            pop  r8
            pop  rb
            pop  ra
            rtn


;--------------------------------------------------------------
;                 Math function primitives
;--------------------------------------------------------------

;--------------------------------------------------------------
; ADD16:       DOUBLE PRECISION ADDITION
; Author:      Tom Swan
;--------------------------------------------------------------
; INPUT:   --  RD, RF
;
; OUTPUT:  --  RD <-- RD + RF using double precision
;              DF indicates if overflow occurred
;--------------------------------------------------------------
ADD16:      glo  rd            ; Get low byte operand #1
            str  r2            ; Push onto stack @ M(R(2))
            glo  rf            ; Get low byte operand #2
            add                ; Add to byte on stack via R(X)
            plo  rd            ; Put result in RD.0
            ghi  rd            ; Get high byte operand #1
            str  r2            ; Push onto stack @ M(R(2))
            ghi  rf            ; Get high byte operand #2
            adc                ; Add with possible carry to M(R(X))
            phi  rd            ; Put result in RD.1
            rtn                ; Return from subroutine


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


;--------------------------------------------------------------
;    Shift 16 bit register, simple divide and multiply by 2
;    Glenn Jolly 5 July 2018
;--------------------------------------------------------------

; shift rd right one bit  rd = rd >> 1
RSHIFT16:   ghi  rd
            shr                ; shift the high byte
            phi  rd
            glo  rd
            shrc               ; shift the low byte with carry
            plo  rd            ; and finish divide by 2
            rtn

; shift rd left one bit  rd = rd << 1
LSHIFT16    glo  rd
            shl                ; shift the low byte
            plo  rd
            ghi  rd
            shlc               ; shift high byte with carry
            phi  rd            ; and finish multiply by 2
            rtn
            
;--------------- End of math function primitives ------------------------

; print number rd
TYPE_NUM:   push rd
            push rf
            mov  rf, textBuff
            sep  scall
            dw   f_uintout     ; consumes rd
            ldi  0             ; write terminator
            str  rf

            mov  rf, textBuff
            sep  scall
            dw   f_msg
            pop  rf
            pop  rd            ; restore rd
            rtn


;----------------------------------------------------------------------
;     Character coords (1..31,0..23) to graphics x,y position
;----------------------------------------------------------------------
            ; multiply column (y) and add row (x) to get address offset
            ; ofs = 256*y + 8*x since we have 32 8bit chars per line
XY2GM2:     push ra
            lda  r6
            plo  ra            ; X
            lda  r6
            phi  ra            ; Y
            
            mov  rf, 0
            mov  rd, mpy256Table
            ghi  ra            ; the Y coord
            shl                ; dw (16 bit) values in table; skip by 2
            plo  rf
            call ADD16         ; rd now points to address of 256*Y
            lda  rd
            phi  rf
            ldn  rd
            plo  rf            ; rf is 256*Y

            mov  rd, 0
            glo  ra            ; the X coord
            shl
            shl
            shl                ; 8*X
            plo  rd
            call ADD16         ; rd is address 256*y + 8*x

            ; add VDP bit 14 offset 
            mov  rf, 4000h     ; 4000h is VDP address 0000h
            call ADD16         ; rd = 256*y + 8*x + 4000h

            ; send address rd to VDP 
            glo  rd
            str  r2
            out  VDP_REG_P
            dec  r2
            ghi  rd
            str  r2
            out  VDP_REG_P
            dec  r2
            pop  ra
            rtn


TYPE_STRING:push rd
NEXTCHAR:   mov  rd, 0
            lda  rc            ; ascii code charIdx
            lbz  EXITTYPE
#ifdef TI99_CHARSET
            smi  32            ; TI99 character offset
#endif
            ; calc address of character data
            ; address is A0 + charIdx * 8
            plo  rf            ; rf.0 has charIdx
            ldi  8
            phi  rf            ; rf.1 is 8
            call MULT8         ; rd has charIdx * 8

            ; start of character block A0
#ifdef TI99_CHARSET
            mov  rf, TI99_FONT
#else
            mov  rf, CP437_FONT
#endif
            call ADD16         ; rd <- rd+rf

            mov  r7, 8         ; number of bytes per character
L_NEXT:     lda  rd            ; get byte and advance
            str  r2
            out  VDP_DAT_P
            dec  r2
            dec  r7
            glo  r7
            lbnz L_NEXT
            lbr  NEXTCHAR  
EXITTYPE:   pop  rd
            rtn

        org 2300h
textBuff:   db  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

TITLE_BUF:  db  'Plot Demo', 0
PLOT_NAME:  db  'Sin(X)/X', 0
#ifdef TI99_CHARSET
PLOT_RANGE: db  'X+-3.5pi', 0
#else
; 224 and 227 are the ascii codes for the +- and pi symbols
PLOT_RANGE: db  'X  ', 241, '3.5', 227, 0
#endif

        org 2500h
            ; default VDP register settings for graphics II mode
VREG_SET:   db  2       ; VR0 graphics 2 mode, no ext video
            db  0C2h    ; VR1 16k vram, display enabled, intr dis; 16x16 sprites
            db  0Eh     ; VR2 Name table address 3800h
            db  0FFh    ; VR3 Color table address 2000h
            db  3       ; VR4 Pattern table address 0000h
            db  76h     ; VR5 Sprite attribute table address 3B00h
            db  3       ; VR6 Sprite pattern table address 1800h
            db  01h     ; Backdrop color black
            
pixelTable: db  080h, 040h, 020h, 010h, 008h, 004h, 002h, 001h ; pixel to be set ON
mpy256Table:    ;  starting address of rows 0..23
            dw     0,  256,  512,  768, 1024, 1280, 1536, 1792,
            dw  2048, 2304, 2560, 2816, 3072, 3328, 3584, 3840,
            dw  4096, 4352, 4608, 4864, 5120, 5376, 5632, 5888

        org 3000h

START_BITMAP:
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
            db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 
END_BITMAP:

START_COLORTABLE:
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
            db  0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 0f1h, 
END_COLORTABLE:

PIXEL_DATA:
            db 128,    0
            db 128,    1
            db 128,    2
            db 128,    3
            db 128,    4
            db 128,    5
            db 128,    6
            db 128,    7
            db 128,    8
            db 128,    9
            db 128,   10
            db 128,   11
            db 128,   12
            db 128,   13
            db 128,   14
            db 128,   15
            db 128,   16
            db 128,   17
            db 128,   18
            db 128,   19
            db 128,   20
            db 128,   21
            db 128,   22
            db 128,   23
            db 128,   24
            db 128,   25
            db 128,   26
            db 128,   27
            db 128,   28
            db 128,   29
            db 128,   30
            db 128,   31
            db 128,   32
            db 128,   33
            db 128,   34
            db 128,   35
            db 128,   36
            db 128,   37
            db 128,   38
            db 128,   39
            db 128,   40
            db 128,   41
            db 128,   42
            db 128,   43
            db 128,   44
            db 128,   45
            db 128,   46
            db 128,   47
            db 128,   48
            db 128,   49
            db 128,   50
            db 128,   51
            db 128,   52
            db 128,   53
            db 128,   54
            db 128,   55
            db 128,   56
            db 128,   57
            db 128,   58
            db 128,   59
            db 128,   60
            db 128,   61
            db 128,   62
            db 128,   63
            db 128,   64
            db 128,   65
            db 128,   66
            db 128,   67
            db 128,   68
            db 128,   69
            db 128,   70
            db 128,   71
            db 128,   72
            db 128,   73
            db 128,   74
            db 128,   75
            db 128,   76
            db 128,   77
            db 128,   78
            db 128,   79
            db 128,   80
            db 128,   81
            db 128,   82
            db 128,   83
            db 128,   84
            db 128,   85
            db 128,   86
            db 128,   87
            db 128,   88
            db 128,   89
            db 128,   90
            db 128,   91
            db 128,   92
            db 128,   93
            db 128,   94
            db 128,   95
            db 128,   96
            db 128,   97
            db 128,   98
            db 128,   99
            db 128,  100
            db 128,  101
            db 128,  102
            db 128,  103
            db 128,  104
            db 128,  105
            db 128,  106
            db 128,  107
            db 128,  108
            db 128,  109
            db 128,  110
            db 128,  111
            db 128,  112
            db 128,  113
            db 128,  114
            db 128,  115
            db 128,  116
            db 128,  117
            db 128,  118
            db 128,  119
            db 128,  120
            db 128,  121
            db 128,  122
            db 128,  123
            db 128,  124
            db 128,  125
            db 128,  126
            db 128,  127
            db 128,  128
            db 128,  129
            db 128,  130
            db 128,  131
            db 128,  132
            db 128,  133
            db 128,  134
            db 128,  135
            db 128,  136
            db 128,  137
            db 128,  138
            db 128,  139
            db 128,  140
            db 128,  141
            db 128,  142
            db 128,  143
            db 128,  144
            db 128,  145
            db 128,  146
            db 128,  147
            db 128,  148
            db 128,  149
            db 128,  150
            db 128,  151
            db 128,  152
            db 128,  153
            db 128,  154
            db 128,  155
            db 128,  156
            db 128,  157
            db 128,  158
            db 128,  159
            db 128,  160
            db 128,  161
            db 128,  162
            db 128,  163
            db 128,  164
            db 128,  165
            db 128,  166
            db 128,  167
            db 128,  168
            db 128,  169
            db 128,  170
            db 128,  171
            db 128,  172
            db 128,  173
            db 128,  174
            db 128,  175
            db 128,  176
            db 128,  177
            db 128,  178
            db 128,  179
            db 128,  180
            db 128,  181
            db 128,  182
            db 128,  183
            db 128,  184
            db 128,  185
            db 128,  186
            db 128,  187
            db 128,  188
            db 128,  189
            db 128,  190
            db 128,  191
            db   0,  148
            db   1,  148
            db   2,  148
            db   3,  148
            db   4,  148
            db   5,  148
            db   6,  148
            db   7,  148
            db   8,  148
            db   9,  148
            db  10,  148
            db  11,  148
            db  12,  148
            db  13,  148
            db  14,  148
            db  15,  148
            db  16,  148
            db  17,  148
            db  18,  148
            db  19,  148
            db  20,  148
            db  21,  148
            db  22,  148
            db  23,  148
            db  24,  148
            db  25,  148
            db  26,  148
            db  27,  148
            db  28,  148
            db  29,  148
            db  30,  148
            db  31,  148
            db  32,  148
            db  33,  148
            db  34,  148
            db  35,  148
            db  36,  148
            db  37,  148
            db  38,  148
            db  39,  148
            db  40,  148
            db  41,  148
            db  42,  148
            db  43,  148
            db  44,  148
            db  45,  148
            db  46,  148
            db  47,  148
            db  48,  148
            db  49,  148
            db  50,  148
            db  51,  148
            db  52,  148
            db  53,  148
            db  54,  148
            db  55,  148
            db  56,  148
            db  57,  148
            db  58,  148
            db  59,  148
            db  60,  148
            db  61,  148
            db  62,  148
            db  63,  148
            db  64,  148
            db  65,  148
            db  66,  148
            db  67,  148
            db  68,  148
            db  69,  148
            db  70,  148
            db  71,  148
            db  72,  148
            db  73,  148
            db  74,  148
            db  75,  148
            db  76,  148
            db  77,  148
            db  78,  148
            db  79,  148
            db  80,  148
            db  81,  148
            db  82,  148
            db  83,  148
            db  84,  148
            db  85,  148
            db  86,  148
            db  87,  148
            db  88,  148
            db  89,  148
            db  90,  148
            db  91,  148
            db  92,  148
            db  93,  148
            db  94,  148
            db  95,  148
            db  96,  148
            db  97,  148
            db  98,  148
            db  99,  148
            db 100,  148
            db 101,  148
            db 102,  148
            db 103,  148
            db 104,  148
            db 105,  148
            db 106,  148
            db 107,  148
            db 108,  148
            db 109,  148
            db 110,  148
            db 111,  148
            db 112,  148
            db 113,  148
            db 114,  148
            db 115,  148
            db 116,  148
            db 117,  148
            db 118,  148
            db 119,  148
            db 120,  148
            db 121,  148
            db 122,  148
            db 123,  148
            db 124,  148
            db 125,  148
            db 126,  148
            db 127,  148
            db 128,  148
            db 129,  148
            db 130,  148
            db 131,  148
            db 132,  148
            db 133,  148
            db 134,  148
            db 135,  148
            db 136,  148
            db 137,  148
            db 138,  148
            db 139,  148
            db 140,  148
            db 141,  148
            db 142,  148
            db 143,  148
            db 144,  148
            db 145,  148
            db 146,  148
            db 147,  148
            db 148,  148
            db 149,  148
            db 150,  148
            db 151,  148
            db 152,  148
            db 153,  148
            db 154,  148
            db 155,  148
            db 156,  148
            db 157,  148
            db 158,  148
            db 159,  148
            db 160,  148
            db 161,  148
            db 162,  148
            db 163,  148
            db 164,  148
            db 165,  148
            db 166,  148
            db 167,  148
            db 168,  148
            db 169,  148
            db 170,  148
            db 171,  148
            db 172,  148
            db 173,  148
            db 174,  148
            db 175,  148
            db 176,  148
            db 177,  148
            db 178,  148
            db 179,  148
            db 180,  148
            db 181,  148
            db 182,  148
            db 183,  148
            db 184,  148
            db 185,  148
            db 186,  148
            db 187,  148
            db 188,  148
            db 189,  148
            db 190,  148
            db 191,  148
            db 192,  148
            db 193,  148
            db 194,  148
            db 195,  148
            db 196,  148
            db 197,  148
            db 198,  148
            db 199,  148
            db 200,  148
            db 201,  148
            db 202,  148
            db 203,  148
            db 204,  148
            db 205,  148
            db 206,  148
            db 207,  148
            db 208,  148
            db 209,  148
            db 210,  148
            db 211,  148
            db 212,  148
            db 213,  148
            db 214,  148
            db 215,  148
            db 216,  148
            db 217,  148
            db 218,  148
            db 219,  148
            db 220,  148
            db 221,  148
            db 222,  148
            db 223,  148
            db 224,  148
            db 225,  148
            db 226,  148
            db 227,  148
            db 228,  148
            db 229,  148
            db 230,  148
            db 231,  148
            db 232,  148
            db 233,  148
            db 234,  148
            db 235,  148
            db 236,  148
            db 237,  148
            db 238,  148
            db 239,  148
            db 240,  148
            db 241,  148
            db 242,  148
            db 243,  148
            db 244,  148
            db 245,  148
            db 246,  148
            db 247,  148
            db 248,  148
            db 249,  148
            db 250,  148
            db 251,  148
            db 252,  148
            db 253,  148
            db 254,  148
            db 255,  148
            db   0,  158
            db   0,  158
            db   1,  158
            db   1,  158
            db   1,  158
            db   1,  158
            db   2,  158
            db   2,  158
            db   2,  158
            db   3,  158
            db   3,  158
            db   3,  158
            db   4,  158
            db   4,  158
            db   4,  158
            db   4,  158
            db   5,  158
            db   5,  158
            db   5,  158
            db   6,  158
            db   6,  158
            db   6,  157
            db   6,  157
            db   7,  157
            db   7,  157
            db   7,  157
            db   8,  157
            db   8,  157
            db   8,  157
            db   8,  156
            db   9,  156
            db   9,  156
            db   9,  156
            db  10,  156
            db  10,  155
            db  10,  155
            db  11,  155
            db  11,  155
            db  11,  155
            db  11,  154
            db  12,  154
            db  12,  154
            db  12,  154
            db  13,  153
            db  13,  153
            db  13,  153
            db  13,  153
            db  14,  152
            db  14,  152
            db  14,  152
            db  15,  152
            db  15,  151
            db  15,  151
            db  15,  151
            db  16,  151
            db  16,  150
            db  16,  150
            db  17,  150
            db  17,  149
            db  17,  149
            db  18,  149
            db  18,  148
            db  18,  148
            db  18,  148
            db  19,  148
            db  19,  147
            db  19,  147
            db  20,  147
            db  20,  146
            db  20,  146
            db  20,  146
            db  21,  145
            db  21,  145
            db  21,  145
            db  22,  144
            db  22,  144
            db  22,  144
            db  22,  143
            db  23,  143
            db  23,  143
            db  23,  143
            db  24,  142
            db  24,  142
            db  24,  142
            db  25,  141
            db  25,  141
            db  25,  141
            db  25,  140
            db  26,  140
            db  26,  140
            db  26,  140
            db  27,  139
            db  27,  139
            db  27,  139
            db  27,  138
            db  28,  138
            db  28,  138
            db  28,  138
            db  29,  137
            db  29,  137
            db  29,  137
            db  29,  137
            db  30,  137
            db  30,  136
            db  30,  136
            db  31,  136
            db  31,  136
            db  31,  136
            db  32,  135
            db  32,  135
            db  32,  135
            db  32,  135
            db  33,  135
            db  33,  134
            db  33,  134
            db  34,  134
            db  34,  134
            db  34,  134
            db  34,  134
            db  35,  134
            db  35,  134
            db  35,  134
            db  36,  133
            db  36,  133
            db  36,  133
            db  36,  133
            db  37,  133
            db  37,  133
            db  37,  133
            db  38,  133
            db  38,  133
            db  38,  133
            db  39,  133
            db  39,  133
            db  39,  133
            db  39,  133
            db  40,  133
            db  40,  133
            db  40,  133
            db  41,  134
            db  41,  134
            db  41,  134
            db  41,  134
            db  42,  134
            db  42,  134
            db  42,  134
            db  43,  134
            db  43,  135
            db  43,  135
            db  43,  135
            db  44,  135
            db  44,  135
            db  44,  135
            db  45,  136
            db  45,  136
            db  45,  136
            db  46,  136
            db  46,  137
            db  46,  137
            db  46,  137
            db  47,  137
            db  47,  138
            db  47,  138
            db  48,  138
            db  48,  139
            db  48,  139
            db  48,  139
            db  49,  140
            db  49,  140
            db  49,  140
            db  50,  141
            db  50,  141
            db  50,  141
            db  50,  142
            db  51,  142
            db  51,  143
            db  51,  143
            db  52,  143
            db  52,  144
            db  52,  144
            db  53,  145
            db  53,  145
            db  53,  146
            db  53,  146
            db  54,  146
            db  54,  147
            db  54,  147
            db  55,  148
            db  55,  148
            db  55,  149
            db  55,  149
            db  56,  150
            db  56,  150
            db  56,  151
            db  57,  151
            db  57,  152
            db  57,  152
            db  57,  152
            db  58,  153
            db  58,  153
            db  58,  154
            db  59,  154
            db  59,  155
            db  59,  155
            db  60,  156
            db  60,  156
            db  60,  157
            db  60,  157
            db  61,  158
            db  61,  158
            db  61,  159
            db  62,  159
            db  62,  160
            db  62,  160
            db  62,  161
            db  63,  161
            db  63,  162
            db  63,  162
            db  64,  162
            db  64,  163
            db  64,  163
            db  64,  164
            db  65,  164
            db  65,  165
            db  65,  165
            db  66,  165
            db  66,  166
            db  66,  166
            db  67,  167
            db  67,  167
            db  67,  167
            db  67,  168
            db  68,  168
            db  68,  168
            db  68,  169
            db  69,  169
            db  69,  169
            db  69,  170
            db  69,  170
            db  70,  170
            db  70,  170
            db  70,  171
            db  71,  171
            db  71,  171
            db  71,  171
            db  71,  172
            db  72,  172
            db  72,  172
            db  72,  172
            db  73,  172
            db  73,  172
            db  73,  173
            db  74,  173
            db  74,  173
            db  74,  173
            db  74,  173
            db  75,  173
            db  75,  173
            db  75,  173
            db  76,  173
            db  76,  173
            db  76,  173
            db  76,  173
            db  77,  173
            db  77,  173
            db  77,  173
            db  78,  173
            db  78,  172
            db  78,  172
            db  78,  172
            db  79,  172
            db  79,  172
            db  79,  172
            db  80,  171
            db  80,  171
            db  80,  171
            db  81,  171
            db  81,  170
            db  81,  170
            db  81,  170
            db  82,  169
            db  82,  169
            db  82,  168
            db  83,  168
            db  83,  168
            db  83,  167
            db  83,  167
            db  84,  166
            db  84,  166
            db  84,  165
            db  85,  165
            db  85,  164
            db  85,  163
            db  85,  163
            db  86,  162
            db  86,  162
            db  86,  161
            db  87,  160
            db  87,  160
            db  87,  159
            db  88,  158
            db  88,  158
            db  88,  157
            db  88,  156
            db  89,  155
            db  89,  154
            db  89,  154
            db  90,  153
            db  90,  152
            db  90,  151
            db  90,  150
            db  91,  149
            db  91,  148
            db  91,  148
            db  92,  147
            db  92,  146
            db  92,  145
            db  92,  144
            db  93,  143
            db  93,  142
            db  93,  141
            db  94,  140
            db  94,  139
            db  94,  138
            db  95,  136
            db  95,  135
            db  95,  134
            db  95,  133
            db  96,  132
            db  96,  131
            db  96,  130
            db  97,  129
            db  97,  128
            db  97,  126
            db  97,  125
            db  98,  124
            db  98,  123
            db  98,  122
            db  99,  120
            db  99,  119
            db  99,  118
            db  99,  117
            db 100,  116
            db 100,  114
            db 100,  113
            db 101,  112
            db 101,  111
            db 101,  109
            db 102,  108
            db 102,  107
            db 102,  106
            db 102,  104
            db 103,  103
            db 103,  102
            db 103,  101
            db 104,   99
            db 104,   98
            db 104,   97
            db 104,   96
            db 105,   94
            db 105,   93
            db 105,   92
            db 106,   90
            db 106,   89
            db 106,   88
            db 106,   87
            db 107,   86
            db 107,   84
            db 107,   83
            db 108,   82
            db 108,   81
            db 108,   79
            db 109,   78
            db 109,   77
            db 109,   76
            db 109,   75
            db 110,   73
            db 110,   72
            db 110,   71
            db 111,   70
            db 111,   69
            db 111,   68
            db 111,   67
            db 112,   66
            db 112,   64
            db 112,   63
            db 113,   62
            db 113,   61
            db 113,   60
            db 113,   59
            db 114,   58
            db 114,   57
            db 114,   56
            db 115,   55
            db 115,   54
            db 115,   53
            db 116,   53
            db 116,   52
            db 116,   51
            db 116,   50
            db 117,   49
            db 117,   48
            db 117,   47
            db 118,   47
            db 118,   46
            db 118,   45
            db 118,   44
            db 119,   44
            db 119,   43
            db 119,   42
            db 120,   42
            db 120,   41
            db 120,   41
            db 120,   40
            db 121,   39
            db 121,   39
            db 121,   38
            db 122,   38
            db 122,   37
            db 122,   37
            db 123,   36
            db 123,   36
            db 123,   36
            db 123,   35
            db 124,   35
            db 124,   35
            db 124,   34
            db 125,   34
            db 125,   34
            db 125,   34
            db 125,   33
            db 126,   33
            db 126,   33
            db 126,   33
            db 127,   33
            db 127,   33
            db 127,   33
            db 127,   33
            db 128,   33
            db 128,   33
            db 128,   33
            db 129,   33
            db 129,   33
            db 129,   33
            db 130,   33
            db 130,   33
            db 130,   34
            db 130,   34
            db 131,   34
            db 131,   34
            db 131,   35
            db 132,   35
            db 132,   35
            db 132,   36
            db 132,   36
            db 133,   36
            db 133,   37
            db 133,   37
            db 134,   38
            db 134,   38
            db 134,   39
            db 135,   39
            db 135,   40
            db 135,   41
            db 135,   41
            db 136,   42
            db 136,   42
            db 136,   43
            db 137,   44
            db 137,   44
            db 137,   45
            db 137,   46
            db 138,   47
            db 138,   47
            db 138,   48
            db 139,   49
            db 139,   50
            db 139,   51
            db 139,   52
            db 140,   53
            db 140,   53
            db 140,   54
            db 141,   55
            db 141,   56
            db 141,   57
            db 142,   58
            db 142,   59
            db 142,   60
            db 142,   61
            db 143,   62
            db 143,   63
            db 143,   64
            db 144,   66
            db 144,   67
            db 144,   68
            db 144,   69
            db 145,   70
            db 145,   71
            db 145,   72
            db 146,   73
            db 146,   75
            db 146,   76
            db 146,   77
            db 147,   78
            db 147,   79
            db 147,   81
            db 148,   82
            db 148,   83
            db 148,   84
            db 149,   86
            db 149,   87
            db 149,   88
            db 149,   89
            db 150,   90
            db 150,   92
            db 150,   93
            db 151,   94
            db 151,   96
            db 151,   97
            db 151,   98
            db 152,   99
            db 152,  101
            db 152,  102
            db 153,  103
            db 153,  104
            db 153,  106
            db 153,  107
            db 154,  108
            db 154,  109
            db 154,  111
            db 155,  112
            db 155,  113
            db 155,  114
            db 156,  116
            db 156,  117
            db 156,  118
            db 156,  119
            db 157,  120
            db 157,  122
            db 157,  123
            db 158,  124
            db 158,  125
            db 158,  126
            db 158,  128
            db 159,  129
            db 159,  130
            db 159,  131
            db 160,  132
            db 160,  133
            db 160,  134
            db 160,  135
            db 161,  136
            db 161,  138
            db 161,  139
            db 162,  140
            db 162,  141
            db 162,  142
            db 163,  143
            db 163,  144
            db 163,  145
            db 163,  146
            db 164,  147
            db 164,  148
            db 164,  148
            db 165,  149
            db 165,  150
            db 165,  151
            db 165,  152
            db 166,  153
            db 166,  154
            db 166,  154
            db 167,  155
            db 167,  156
            db 167,  157
            db 167,  158
            db 168,  158
            db 168,  159
            db 168,  160
            db 169,  160
            db 169,  161
            db 169,  162
            db 170,  162
            db 170,  163
            db 170,  163
            db 170,  164
            db 171,  165
            db 171,  165
            db 171,  166
            db 172,  166
            db 172,  167
            db 172,  167
            db 172,  168
            db 173,  168
            db 173,  168
            db 173,  169
            db 174,  169
            db 174,  170
            db 174,  170
            db 174,  170
            db 175,  171
            db 175,  171
            db 175,  171
            db 176,  171
            db 176,  172
            db 176,  172
            db 177,  172
            db 177,  172
            db 177,  172
            db 177,  172
            db 178,  173
            db 178,  173
            db 178,  173
            db 179,  173
            db 179,  173
            db 179,  173
            db 179,  173
            db 180,  173
            db 180,  173
            db 180,  173
            db 181,  173
            db 181,  173
            db 181,  173
            db 181,  173
            db 182,  173
            db 182,  173
            db 182,  172
            db 183,  172
            db 183,  172
            db 183,  172
            db 184,  172
            db 184,  172
            db 184,  171
            db 184,  171
            db 185,  171
            db 185,  171
            db 185,  170
            db 186,  170
            db 186,  170
            db 186,  170
            db 186,  169
            db 187,  169
            db 187,  169
            db 187,  168
            db 188,  168
            db 188,  168
            db 188,  167
            db 188,  167
            db 189,  167
            db 189,  166
            db 189,  166
            db 190,  165
            db 190,  165
            db 190,  165
            db 191,  164
            db 191,  164
            db 191,  163
            db 191,  163
            db 192,  162
            db 192,  162
            db 192,  162
            db 193,  161
            db 193,  161
            db 193,  160
            db 193,  160
            db 194,  159
            db 194,  159
            db 194,  158
            db 195,  158
            db 195,  157
            db 195,  157
            db 195,  156
            db 196,  156
            db 196,  155
            db 196,  155
            db 197,  154
            db 197,  154
            db 197,  153
            db 198,  153
            db 198,  152
            db 198,  152
            db 198,  152
            db 199,  151
            db 199,  151
            db 199,  150
            db 200,  150
            db 200,  149
            db 200,  149
            db 200,  148
            db 201,  148
            db 201,  147
            db 201,  147
            db 202,  146
            db 202,  146
            db 202,  146
            db 202,  145
            db 203,  145
            db 203,  144
            db 203,  144
            db 204,  143
            db 204,  143
            db 204,  143
            db 205,  142
            db 205,  142
            db 205,  141
            db 205,  141
            db 206,  141
            db 206,  140
            db 206,  140
            db 207,  140
            db 207,  139
            db 207,  139
            db 207,  139
            db 208,  138
            db 208,  138
            db 208,  138
            db 209,  137
            db 209,  137
            db 209,  137
            db 209,  137
            db 210,  136
            db 210,  136
            db 210,  136
            db 211,  136
            db 211,  135
            db 211,  135
            db 212,  135
            db 212,  135
            db 212,  135
            db 212,  135
            db 213,  134
            db 213,  134
            db 213,  134
            db 214,  134
            db 214,  134
            db 214,  134
            db 214,  134
            db 215,  134
            db 215,  133
            db 215,  133
            db 216,  133
            db 216,  133
            db 216,  133
            db 216,  133
            db 217,  133
            db 217,  133
            db 217,  133
            db 218,  133
            db 218,  133
            db 218,  133
            db 219,  133
            db 219,  133
            db 219,  133
            db 219,  133
            db 220,  133
            db 220,  134
            db 220,  134
            db 221,  134
            db 221,  134
            db 221,  134
            db 221,  134
            db 222,  134
            db 222,  134
            db 222,  134
            db 223,  135
            db 223,  135
            db 223,  135
            db 223,  135
            db 224,  135
            db 224,  136
            db 224,  136
            db 225,  136
            db 225,  136
            db 225,  136
            db 226,  137
            db 226,  137
            db 226,  137
            db 226,  137
            db 227,  137
            db 227,  138
            db 227,  138
            db 228,  138
            db 228,  138
            db 228,  139
            db 228,  139
            db 229,  139
            db 229,  140
            db 229,  140
            db 230,  140
            db 230,  140
            db 230,  141
            db 230,  141
            db 231,  141
            db 231,  142
            db 231,  142
            db 232,  142
            db 232,  143
            db 232,  143
            db 233,  143
            db 233,  143
            db 233,  144
            db 233,  144
            db 234,  144
            db 234,  145
            db 234,  145
            db 235,  145
            db 235,  146
            db 235,  146
            db 235,  146
            db 236,  147
            db 236,  147
            db 236,  147
            db 237,  148
            db 237,  148
            db 237,  148
            db 237,  148
            db 238,  149
            db 238,  149
            db 238,  149
            db 239,  150
            db 239,  150
            db 239,  150
            db 240,  151
            db 240,  151
            db 240,  151
            db 240,  151
            db 241,  152
            db 241,  152
            db 241,  152
            db 242,  152
            db 242,  153
            db 242,  153
            db 242,  153
            db 243,  153
            db 243,  154
            db 243,  154
            db 244,  154
            db 244,  154
            db 244,  155
            db 244,  155
            db 245,  155
            db 245,  155
            db 245,  155
            db 246,  156
            db 246,  156
            db 246,  156
            db 247,  156
            db 247,  156
            db 247,  157
            db 247,  157
            db 248,  157
            db 248,  157
            db 248,  157
            db 249,  157
            db 249,  157
            db 249,  157
            db 249,  158
            db 250,  158
            db 250,  158
            db 250,  158
            db 251,  158
            db 251,  158
            db 251,  158
            db 251,  158
            db 252,  158
            db 252,  158
            db 252,  158
            db 253,  158
            db 253,  158
            db 253,  158
            db 254,  158
            db 254,  158
            db 254,  158
            db 254,  158
            db 255,  158
            db 255,  158
            db 255,  158
END_PIXEL_DATA:


#ifdef TI99_CHARSET
TI99_FONT:
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

#else
CP437_FONT:
               ; CP437 character set
            db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  ; Char000
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
; CP437 character set
#endif

END:
