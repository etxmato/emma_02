;
; FFFF  I   GGG        FFFF   OO   RRRR   TTTTT  H   H
; F     I  G           F     O  O  R  R     T    H   H
; FFF   I  G GGG   XX  FFF   O  O  RRR      T    HHHHH
; F     I  G   G       F     O  O  R  R     T    H   H
; F     I   GGG        F      OO   R   R    T    H   H
;
;               1   8888    00   2222
;               1   8  8   0  0     2
;               1   8888   0  0   22
;               1   8  8   0  0  2
;               1   8888    00   2222
;
;
;       ALL PUBLICATIONS OF THE FORTH INTREST GROUP
;       ARE PUBLIC DOMAIN.  THEY MAY BE FURTHER
;       DISTRIBUTED BY INCLUSION OF THIS CREDIT
;       NOTICE:
;
;       THIS PUBLICATION HAS BEEN MADE AVAILABLE BY THE
;                       FORTH INTREST GROUP
;                       P. O. BOX 1105
;                       SAN CARLOS, CA 94070
;
;       IMPLEMENTAION BY:
;               GARY R. BRADSHAW
;               RFD 1 BOX 80
;               GIDLEY ROAD
;               ESPERANCE, NY  12066
;
;       MODIFIED BY:
;               GORDEN FLEMMING
;               13490 SIMSHAW ST.
;               SYLMAR, CA  91342
;
;               JIM MCDANIEL
;               1109 POINCIANA DR.
;               SUNNYVALE, CA 94086
;
;   ADDRESS COMMENTS & CORRECTIONS TO  JIM MCDANIEL.
;
;       ACKNOWLEDGEMENTS:
;
;               KEN MANTEI
;
;               FIG INSTALLATION MANUAL
;
;               FIG 8080 ASSEMBLY SOURCE LISTING..;
;
;
;               THIS LISTING TYPED     PRINTED 3/18/81
;
;
;       THE I/O VECTORS FOR DISC  ARE POINTING TO
;       ROUTINES FOR THE RCA CDP18S007, CDP18S008
;       OR THE CDP 18SOO5..
;       FOR OTHER SYSTEMS YOU WILL NEED TO
;       CHANGE THE POINTERS AND WRITE YOUR
;       OWN ROUTINES.;
;
; THE USER VARIABLE  DV IS 3 BYTES LONG
; (USER AREA OFFSET 32H 33H 34H) AND IS USED
; TO PASS VARIABLES TO THE RCA ROM UTILITY
; WHEN CALLING FOR DISK I/O.  THE RESIDENT ROM
; UTILITIES PRODUCE THEIR OWN ERROR MESSAGES.
;
; THIS VERSION ASSEMBLES WITH START UP CONSTANTS THAT
; ASSUME 28K OF RAM. DISC BUFFERS ARE
; SET FOR 1K (THIS CAN BE EASILY CHANGED
; BY CHANGING FIRST, LIMIT, B/BUF AND B/SCR).
; BLOCK 0 BEGINS AT TRACK 0 SECTOR 1.
;
;
; DR1 (SET DRIVE TO 1) IS IMPLEMENTED AS:
;  : DR1 B/SCR 250 * OFFSET ! ;
; THERFORE B/SCR AND B/BUF CAN BE CHANGED WITHOUT
; HAVING TO REWRITE
; DR1.
; THE TERMINAL I/O ASSUMES THE USE OF AN
; RCA CDP1854 UART CONFIGURED IN ONE OF THE
; ABOVE MENTIONED SYSTEMS.  THE UART IS DRIVEN
; DIRECTLY WITHOUT CALLING RCA ROM UTILITY
; ROUTINES.
;
;
; THE FORTH WORD   MON   EXITS FORTH AND RETURNS
; TO THE RESIDENT ROM UTILITY.
;
; THE FORTH WORD  BYE  IS DEFINED AS:
; : BYE  FLUSH  MON ;;
;
;
;       REGISTER ALLOCATIONS FOR THIS VERSION
;
;               R2   RETURNS STACK POINTER  R0
;                    GROWS DOWN LEFT POINTING TO
;                    FREE LOCATION
;
;               R3   PC FOR I/O AND PRIMITIVES
;
;               R7, R8  TEMPORARY ACCUMULATORS
;
;               R9   COMPUTATION STACK POINTER   S0
;                    GROWS UPWARD
;                    LEFT POINTING AT HIGH BYTE
;
;               RA   FORTH "I" REGISTER   IP
;
;               RB   FORTH "W" REGISTER   WP
;
;               RC   PC FOR INNER INTERPRETER
;
;               RD   USER POINTER         UP
;
;               RF   DISC I/O
;
;               OTHER REGISTERS ARE LEFT AVAILABLE
;               EXCEPT THAT RF.0 IS ZERO AFTER COLD
;               OR WARM STARTS
;
;
;             MEMORY MAP
;
;       ---------------------- LIMIT
;
;            RAM BUFFERS
;
;       ---------------------- FIRST
;
;             USER AREA
;
;       ---------------------- UP
;
;            RETURN STACK      R0
;
;       ---------------------- 
;
;          TERMINAL BUFFER
;
;       ---------------------- TIB
;
;         COMPUTATION STACK
;
;       ---------------------- S0
;
;             FREE SPACE
;
;       ----------------------
;
;            TEXT BUFFER
;
;       ---------------------- PAD
;
;             WORD BUFFER
;
;       ---------------------- DP
;
;             DICTIONARY
;
;       ----------------------
;
;         BOOT UP PARAMETERS
;
;       ---------------------- ORGIN 005E
;
;          ANY REQUIRED I/O
;           INITIALIZATION
;
;       ---------------------- 0000
;
;
        .EQU FIRSTB, H'4000 ; ADDRESS OF FIRST DISK BUFFER
        .EQU LIMITB, H'6C2C ; END OF DISK BUFFER AREA
        .EQU CSTACK, 9
        .EQU RSTACK, 2
	;
        .ORG H'0000
        ;
        ; SET-UP ROUTINES
        ;
        DIS
        .DB H'00
        OUT 1               ; SET LEVEL 1 I/O
        .DB H'01
        OUT 3               ; SET UP UART
        .DB H'1D
        LDI H'00
        PHI 3
        LDI H'5E
        PLO 3
        LDI H'2F
        PHI 2
        LDI H'FF
        PLO 2
        SEP 3
	;
	;
        ;
        .ORG H'005E
	;
START:  NOP
        LBR COLD            ; COLD START
        NOP
        LBR WARM            ; WARM START
        .DW H'070A          ; CPU NUMBER
        .DW H'0001          ; REVISION NUMBER
        .DW TASK - 7        ; TOPMOST PRGM IN FORTH VOCABULARY
        .DW H'0008          ; BACKSPACE
        .DW H'2000          ; INITIAL USER AREA         UP
        .DW H'1F00          ; INITAL STACK              S0
        .DW H'1FFF          ; INITAL RETURN STACK       R0
        .DW H'1F80          ; TERMINAL BUFFER           TIB
        .DW H'001F          ; NAME FIELD WIDTH          WIDTH
                            ;   (31 DECIMAL)
        .DW H'0000          ; WARNING                   WARNING
        .DW LEND            ; FENCE                     FENCE
        .DW LEND            ; INIT DICTIONARY POINTER   DP
        .DW FRTH + 16       ; INIT VOCAB                VOC-LINK
	;
	;
        .DB H'83,"LI",H'D4 ; LIT
        .DW H'0000
LIT:    .DW * + 2
        INC R9
        INC R9
        LDA RA
        STR R9
        INC R9
        LDA RA
        STR R9
        DEC R9
        SEP RC
	;
        ;
        ; NEXT INNER INTERPRETER
        SEP R3              ; LEAVE RC AT NEXT
NEXT:   LDA RA
        PHI RB
        LDA RA
        PLO RB
WBR:    LDA RB
        PHI R3
        LDA RB
        PLO R3
        BR NEXT - 1
        ; EXECUTE
        .DB H'87,"EXECUT",H'C5 ; EXECUTE
	.DW LIT - 6
EXE:    .DW * + 2
        LDA R9
        PHI RB
        LDN R9              ; LOAD W FROM STACK              
        PLO RB
        DEC R9
        DEC R9
        DEC R9
        INC RC
        INC RC
        INC RC              ; POINT TO WBR
        INC RC
        SEP RC
	;
	;
        .DB H'86,"BRANC",H'C8 ; BRANCH
        .DW EXE - 10
BRCH:   .DW * + 2
BRANCH: LDA RA
        STR R2
        LDA RA
        PLO RA
        LDN R2
        PHI RA
        SEP RC
	;
	;
        .DB H'87,"0BRANC",H'C8 ; 0BRANCH
	.DW BRCH - 9
ZBRCH:  .DW * + 2
        LDA R9
        BNZ NO
        LDN R9
        BNZ NO
        DEC R9
        DEC R9
        DEC R9
        BR BRANCH
NO:     INC RA
        INC RA
        DEC R9
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'86,"(LOOP",H'A9 ; (LOOP)
        .DW ZBRCH - 10
LUPE:   .DW * + 2
        INC R2
        GHI R2
        PHI R8
        PHI R7
        GLO R2
        PLO R8
        PLO R7
        DEC R2
        LDN R8
        ADI H'01
        STR R8
        INC R8
        LDN R8
        ADCI H'00
        NOP                 ; TO NEW PAGE
        NOP
COMP:   STR R8
        INC R8
        SEX R7
        LDA R8
        SD
        INC R7
        LDN R8
        SDB
        ANI H'80
        BZ CEND
        LDA RA
        STR R2
        LDN RA
        PLO RA
        LDN R2
        PHI RA
        SEP RC
CEND:   INC RA
        INC RA
        INC R2
        INC R2
        INC R2
        INC R2
        SEP RC
	;
        .DB H'87,"(+LOOP",H'A9 ; (+LOOP)
	.DW LUPE - 9
PLUPE:  .DW * + 2
        INC R2
        GHI R2
        PHI R8
        PHI R7
        GLO R2
        PLO R8
        PLO R7
        DEC R2
        SEX R9
        INC R9
        LDN R8
        ADD
        STR R8
        INC R8
        DEC R9
        LDN R8
        ADC
        STR R8
        LDN R9
        SHL
        DEC R9
        DEC R9
        BDF LUPE1
        BR COMP +1
LUPE1:  INC R8
        SEX R7
        LDA R8
        SM
        INC R7
        LDN R8
        SMB
        BR COMP + 8
	;
	;
        .DB H'84,"(DO",H'A9 ; (DO)
        .DW PLUPE - 10
PDO:    .DW * + 2
        DEC R9
        DEC R9
        SEX R2
        LDA R9
        STXD
        LDA R9
        STXD
        LDA R9
        STXD
        LDN R9
        STXD
        DEC R9
        DEC R9
        DEC R9
        DEC R9
        DEC R9
        SEP RC
	;
	;
        .DB H'85,"DIGI",H'D4 ; DIGIT
        .DW PDO - 7
DGT:    .DW * + 2
        SEX R9
        DEC R9
        LDN R9
        SMI H'30
        BNF BAD
        SMI H'11
        BDF DOK
        SMI H'F9
        BDF BAD
DOK:    ADI H'0A
        STR R9
        INC R9
        INC R9
        SM
        BDF BAD2
        DEC R9
        SEP RC
BAD2:   DEC R9
        DEC R9
BAD:    LDI H'00
        STXD
        STR R9
        SEP RC
	;
        .DB H'86,"(FIND",H'A9 ; (FIND)
        .DW DGT - 8
FIND:   .DW * + 2
        DEC R9
        DEC R9
        LDA R9
        PHI R8
        LDA R9
        PLO R8
        LDA R9
        PHI R7
        LDN R9
        PLO R7
        DEC R9
        DEC R9
LOOP1:  SEX R7              ; SAVE LENGTH BYTE
        LDN R7
        STR R2
        LDA R8              ; COMPARE LENGTH BYTES
        XOR
        ANI H'3F
        BNZ BADLEN
NEXCHR: INC R7
        LDA R8              ; COMPARE NEXT CHARACTER
        XOR
        SHL
        BNZ BADCHR          ; NO MATCH ON 7 BITS
        SHLC
        BZ NEXCHR           ; IF NOT LAST CHARACTER
LOOP2:  LDA R7              ;   ELSE END OF STRING
        ANI H'80
        BZ LOOP2
        SEX R9              ; END OF DICT NAME
        GLO R7
        ADI H'04
        STXD
        GHI R7
        ADCI H'00
        STR R9              ; LEAVE PFA
        INC R9
        INC R9
        LDI H'00
        STR R9
        INC R9
        LDN R2              ; GET LENGTH BYTE
        STR R9
        INC R9
        LDI H'FF
        STR R9
        INC R9
        STXD                ; AND TRUE FLAG
        SEP RC
BADLEN: INC R7
BADCHR: LDA R7
        ANI H'80
        BZ BADCHR
        LDN R7
        BNZ BOK
        INC R7
        LDN R7
        DEC R7
        BNZ BOK
        LDI H'00            ; LINK=0 RETURN FALSE
        STR R9
        DEC R9
        STR R9
        SEP RC
BOK:    LDA R7
        STR R2
        LDN R7
        PLO R7
        LDN R2
        PHI R7
        LDN R9
        PLO R8
        DEC R9
        LDA R9
        PHI R8
        BR LOOP1
	;
	;
        .DB H'87,"ENCLOS",H'C5 ; ENCLOSURE
	.DW FIND -9
ENCL:   .DW * + 2
        DEC R9
        DEC R9
        LDA R9
        PHI R8
        LDA R9
        PLO R8
        INC R9
        LDI H'00            ; R7.0 IS OFFSET
        PLO R7
        LDN R9              ; SAVE DELIM
        STR R2
        SEX R2
LOP1:   LDN R8
        SM
        BNZ FRST            ; FIND FIRST NON-
        INC R8              ;   DELIM CHAR
        INC R7
        BR LOP1
FRST:   GLO R7              ; SAVE OFFSET TO
        STR R9
        PHI R7
        INC R9              ;   FIRST CHARACTER
        LDI H'00
        STR R9
        INC R9
        INC R9
        STR R9
        INC R9
LOP2:   LDN R8
        BZ NULL             ; EQUAL NULL ?
        SM                  ; SUBTRACT DELIMIN
        BZ DELIM
        INC R8
        INC R7
        BR LOP2
NULL:   GLO R7              ; LEAVE OFFSET
        SEX R9
        STR R9
        GHI R7              ; TO NEXT CHARACTER
        SM
        BNZ SKIP
        INC R7
SKIP:   GLO R7              ; LAST CHARACTER IN
        DEC R9
        DEC R9
        STR R9              ; WORD
        INC R9
        SEP RC
DELIM:  INC R7
        GLO R7
        STR R9
        DEC R7
        BR SKIP
	;
        .DB H'85,"CMOV",H'C5 ; CMOVE
        .DW ENCL - 10
CMOVE:  .DW * + 2
        LDA R9
        ADI H'01
        PHI R7
        LDN R9
        PLO R7
        DEC R7
        DEC R9
        DEC R9
        DEC R9
        GHI RA              ; PUSH RA
        STR R2
        DEC R2
        GLO RA
        STR R2
        LDA R9              ; RA IS "TO"
        PHI RA
        LDN R9
        PLO RA
        DEC R9
        DEC R9
        DEC R9
        LDA R9              ; R8 IS "FROM"
        PHI R8
        LDN R9
        PLO R8
        DEC R9
        DEC R9
        DEC R9
LUUP:   GHI R7
        BZ END2
        LDA R8
        STR RA
        INC RA
        DEC R7
        BR LUUP
END2:   LDA R2              ; POP RA
        PLO RA
        LDN R2
        PHI RA
        SEP RC
	;
        .DB H'82,H'55,H'AA  ; U*
                            ; UNSIGNED 16 X 16 BIT MULTIPLY
        .DW CMOVE - 8
USTAR:  .DW * + 2
        SEX R9
        LDI R0
        PLO R7              ; R7 IS LOW 2 BYTES
        PHI R7
        LDI H'10            ; OF PRODUCT
LP7B:   STR R2              ; MEM(2) IS LOOP COUNT
        GLO R7
        SHL
        PLO R7
        GHI R7
        SHLC
        PHI R7
        INC R9              ; DOUBLE THE PRODUCT AND
        LDN R9              ;   TEST HIGH BIT
        SHLC
        STXD
        LDN R9              ;   OF OP2
        SHLC
        STR R9
        BNF SKP9A
        DEC R9
        GLO R7
        ADD
        PLO R7
        DEC R9              ;   ADD OP1
        GHI R7
        ADC
        PHI R7
        INC R9              ; TO 24 BIT PRODUCT
        INC R9
        INC R9
        LDI H'00
        ADC
        STXD
SKP9A:  LDN R2
        SMI H'01
        BNZ LP7B
UOUT:   DEC R9              ; MOVE REST OF
        GLO R7
        STXD
        GHI R7              ; PRODUCT TO STACK
        STR R9
        INC R9
        INC R9
        SEP RC
	;
        .DB H'82,H'55,H'AF  ; U/ UNSIGNED DIVIDE
        .DW USTAR - 5
USLSH:  .DW * + 2
        SEX R9
        DEC R9
        DEC R9
        LDA R9
        PHI R7
        LDN R9
        PLO R7
        DEC R9
        DEC R9
        LDA R9
        SHL
        INC R9
        STXD
        DEC R9
        DEC R9
        LDA R9
        SHLC
        INC R9
        STR R9
        INC R9
        LDI H'10
        PLO R8
LPC5:   GLO R7
        SHLC
        PLO R7
        GHI R7
        SHLC
        PHI R7
        INC R9
        INC R9
        GLO R7
        SM
        PHI R8
        DEC R9
        GHI R7
        SMB
        BNF SKPD8
        PHI R7
        GHI R8
        PLO R7
SKPD8:  DEC R9
        LDN R9
        SHLC
        STXD
        LDN R9
        SHLC
        STR R9
        INC R9
        DEC R8
        GLO R8
        BNZ LPC5
        DEC R9
        BR UOUT
	;
        .DB H'83,"AN",H'C4  ; AND
        .DW USLSH - 5
FAND:   .DW * + 2
        GLO R9
        PLO R8
        GHI R9
        PHI R8
        DEC R8
        SEX R8
        INC R9
        LDN R9
        AND
        STXD
        DEC R9
        LDN R9
        AND
        STR R8
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'82,H'4F,H'D2  ; OR
        .DW FAND - 6
FFOR:   .DW * + 2
        GLO R9
        PLO R8
        GHI R9
        PHI R8
        DEC R8
        SEX R8
        INC R9
        LDN R9
        OR
        STXD
        DEC R9
        LDN R9
        OR
        STR R8
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'83,"XO",H'D2   ; XOR
        .DW FFOR - 5
FXOR:   .DW * + 2
        GLO R9
        PLO R8
        GHI R9
        PHI R8
        DEC R8
        SEX R8
        INC R9
        LDN R9
        XOR
        STXD
        DEC R9
        LDN R9
        XOR
        STR R8
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'83,"SP",H'C0  ; SP@
        .DW FXOR - 6
FSPAT:  .DW * + 2
        GHI R9
        STR R2
        GLO R9
        INC R9
        INC R9
        INC R9
        STR R9
        DEC R9
        LDN R2
        STR R9
        SEP RC
	;
        .DB H'83,"SP",H'A1  ; SP!
                            ;   stack pointer store
        .DW FSPAT - 6
SP1:    .DW * + 2
        GLO RD
        ADI H'06
        PLO R8
        GHI RD
        ADCI H'00
        PHI R8
        LDA R8
        PHI R9
        LDN R8
        PLO R9
        SEP RC
	;
        .DB H'83,"RP",H'A1  ; RP!
                            ; RETURN STACK POINTER STORE
        .DW SP1 - 6
RP1:    .DW * + 2
        GLO RD
        ADI R8
        PLO R8
        GHI RD
        ADCI H'00
        PHI R8
        LDA R8
        PHI R2
        LDN R8
        PLO R2
        SEP RC
	;
        .DB H'82,H'3B,H'D3  ; S (UNEST)
        .DW RP1 - 6
SEMIS:  .DW * + 2
        INC R2
        LDA R2
        PLO RA
        LDN R2
        PHI RA
        SEP RC

        .DB H'85,"LEAV",H'C5 ; LEAVE
        .DW SEMIS - 5
LVE:    .DW * + 2
        GHI R2
        PHI R8
        GLO R2
        PLO R8
        INC R8
        LDA R8
        INC R8
        STR R8
        DEC R8
        LDA R8
        INC R8
        STR R8
        SEP RC
	;
        .DB H'82,H'3E,H'D2  ; >R   TO R
        .DW LVE - 8
GR:     .DW * + 2
        SEX R2
        LDA R9
        STXD
        LDN R9
        STXD
        DEC R9
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'82,H'52,H'BE  ; R>   FROM R
        .DW GR - 5
RG:     .DW * + 2
        INC R9
        INC R9
        INC R9
        INC R2
        LDA R2
        STR R9
        DEC R9
        LDN R2
        STR R9
        SEP RC
	;
        .DW H'81D2          ; R   COPY TOP OF RETN
        .DW RG - 5          ; STACK TO TOP OF
R:      .DW * + 2           ; COMPUTATION STACK
        GLO R2
        PLO R8
        GHI R2
        PHI R8
        INC R8
        INC R9
        INC R9
        INC R9
        LDA R8
        STR R9
        DEC R9
        LDN R8
        STR R9
        SEP RC
	;
        .DB H'82,H'30,H'BD  ; 0=
        .DW R - 4
ZEQAL:  .DW * + 2
        LDA R9
        BNZ NONE
        LDN R9
        BNZ NONE
ZONE:   LDI H'01
        BR STOR
NONE:   LDI H'00
STOR:   STR R9
        DEC R9
        LDI H'00
        STR R9
        SEP RC
	;
        .DB H'82,H'30,H'BC  ; 0<
        .DW ZEQAL - 5
ZLESS:  .DW * + 2
        LDA R9
        SHL
        BDF ZONE
        BR NONE
	;
        .DW H'81AB          ; +
        .DW ZLESS - 5
PLUS:   .DW * + 2
        GLO R9
        PLO R8
        GHI R9
        PHI R8
        DEC R8
        SEX R8
        INC R9
        LDN R9
        ADD
        STXD
        DEC R9
        LDN R9
        ADC
        STR R8
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'85,"MINU",H'D3 ; MINUS
        .DW PLUS - 4
MINUS:  .DW * + 2
        SMI H'00            ; SET CARRY
MINOS:  INC R9
        LDN R9
        XRI H'FF
        ADCI H'00
        STR R9
        DEC R9
        LDN R9
        XRI H'FF
        ADCI H'00
        STR R9
        SEP RC
	;
        .DB H'82,H'44,H'AB  ; D+ DBL PRCN INTEGERS
        .DW MINUS - 8       ; ARE STORED HIGH 16 BITS TOP
DPLUS:  .DW * + 2           ; LOW 16 BITS BENEATH
        GLO R9
        SMI H'05
        PLO R8
        GHI R9
        SMBI H'00
        PHI R8
        DEC R9
        SEX R8
        LDN R9
        ADD
        STXD
        DEC R9
        LDN R9
        ADC
        STR R8
        INC R8
        INC R8
        INC R8
        INC R9
        INC R9
        INC R9
        LDN R9
        ADC
        STXD
        DEC R9
        LDN R9
        ADC
        STR R8
        DEC R9
        DEC R9
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'86,"DMINU",H'D3 ; DMINUS
        .DW DPLUS - 5
DMIN:   .DW * + 2
        SEX R9
        DEC R9
        LDN R9
        XRI H'FF
        ADI H'01
        STXD
        LDN R9
        XRI H'FF
        ADCI H'00
        STR R9
        INC R9
        INC R9
        BR MINOS
	;
        .DB H'84,"OVE",H'D2 ; OVER
        .DW DMIN - 9
OVER:   .DW * + 2
        DEC R9
        DEC R9
        LDA R9
        INC R9
        INC R9
        INC R9
        STR R9
        DEC R9
        DEC R9
        DEC R9
        LDA R9
        INC R9
        INC R9
        INC R9
        STR R9
        DEC R9
        SEP RC
	;
        .DB H'84,"DRO",H'D0 ; DROP
        .DW OVER - 7
DROP:   .DW * + 2
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'84,"SWA",H'D0 ; SWAP
        .DW DROP - 7
SWAP:   .DW * + 2
        GLO R9
        PLO R8
        GHI R9
        PHI R8
        DEC R8
        LDN R8
        STR R2
        INC R9
        LDN R9
        STR R8
        LDN R2
        STR R9
        DEC R9
        DEC R8
        LDN R8
        STR R2
        LDN R9
        STR R8
        LDN R2
        STR R9
        SEP RC
	;
        .DB H'83,"DU",H'D0  ; DUP
        .DW SWAP - 7
DUP:    .DW * + 2
        LDA R9
        INC R9
        STR R9
        DEC R9
        LDA R9
        INC R9
        STR R9
        DEC R9
        SEP RC
	;
        .DB H'82,H'2B,H'A1  ; +!
        .DW DUP - 6
PLUSS:  .DW * + 2
        LDA R9
        PHI R8
        LDN R9
        PLO R8
        DEC R9
        DEC R9
        INC R8
        SEX R8
        LDN R9
        ADD
        STXD
        DEC R9
        LDN R9
        ADC
        STR R8
POP:    DEC R9
        DEC R9
        SEP RC
	;
        .DB H'86,"TOGGL",H'C5 ; TOGGLE
        .DW PLUSS - 5
TGLE:   .DW * + 2
        INC R9
        LDN R9
        PLO R7
        DEC R9
        DEC R9
        DEC R9
        LDA R9
        PHI R8
        LDN R9
        PLO R8
        SEX R8
        GLO R7
        XOR
        STR R8
        DEC R9
        BR POP
        ;        
        .DW H'81C0          ; @
        .DW TGLE - 9
AT:     .DW * + 2
        LDA R9
        PHI R8
        LDN R9
        PLO R8
        DEC R9
        LDA R8
        STR R9
        INC R9
        LDN R8
        STR R9
        DEC R9
        SEP RC
	;
        .DB H'82,H'43,H'C0  ; C@
        .DW AT - 4
CAT:    .DW * + 2
        LDA R9
        PHI R8
        LDN R9
        PLO R8
        LDN R8
        STR R9
        DEC R9
        LDI H'00
        STR R9
        SEP RC
	;
        .DW H'81A1          ; !   STORE
        .DW CAT - 5
EX:     .DW * + 2
        LDA R9
        PHI R8
        LDN R9
        PLO R8
        DEC R9
        DEC R9
        DEC R9
        LDA R9
        STR R8
        INC R8
        LDN R9
        STR R8
        DEC R9
        DEC R9
        DEC R9
        SEP RC
	;
        .DB H'82,H'43,H'A1  ; C!   C STORE
        .DW EX - 4
CEX:    .DW * + 2
        LDA R9
        PHI R8
        LDN R9
        PLO R8
        DEC R9
        DEC R9
        LDN R9
        STR R8
        DEC R9
        DEC R9
        DEC R9
        SEP RC
;
;
; THE FOLLOWING FOUR ROUTINES ARE USER DEFINED
; FOR THE CONSULE INPUT AND OUTPUT       
        ;
        ; SEND ASCII TO TERMINAL
        ;
        .DB H'84,"EMI",H'D4 ; EMIT
        .DW CEX - 5
EMIT:   .DW NEST            ; OUTPUT CHAR TO
                            ; TERMINAL AND INCREMENT
                            ; "OUT"
        .DW FOUT            ; GET ADDRESS OF OUT
        .DW CSEND
        .DW SEMIS
CSEND:  .DW * + 2
; INCREMENT USER VARIABLE "OUT"
; WHOSE ADDRESS IN ON THE COMPUTATION
; STACK
        LDA R9
        PHI R8
        LDN R9
        PLO R8
        DEC R9
        DEC R9
        DEC R9
        INC R8
        LDN R8
        ADI H'01
        STR R8
        DEC R8
        LDN R8
        ADCI H'00
        STR R8
; WAIT UNTIL UART FREE AND
; SEND OUT CHARACTER WHICH IS ON
; THE COMP.STACK
CSEND1: SEX R2
        INP R3              ; GET UART STATUS
        SHL                 ; GET THRE FLAG
        BNF CSEND1
        SEX R9
        INC R9
        OUT 2
        DEC R9
        DEC R9
        DEC R9
        DEC R9
        SEP RC
	;
        ;
        .DB H'83,"KE",H'D9  ; KEY   READ KEYBOARD
        .DW EMIT - 7
KEY:    .DW * + 2
        SEX CSTACK
        IRX
        IRX
        IRX
KEY1:   INP R3              ; GET DA BIT FROM
        SHR					; UART STATUS BYTE
        BNF KEY1            ; WAIT 'TILL DATA AVAIL.
        INP 2               ; GET DATA
        ANI H'7F            ; STRIP PARITY BIT
        STR CSTACK
        DEC CSTACK
        LDI H'00
        STR CSTACK
        SEP RC
        ;
        ; TEST FOR BREAK
        ;
        .DB H'89,"?TERMINA",H'CC ; ?TERMINAL
	.DW KEY - 6
; ASSUME THAT A FRAMING ERROR INDICATES
; THAT THE BREAK KEY IS OR WAS PRESSED.
QTERM:  .DW * +2
        SEX CSTACK
        IRX
        IRX
        IRX
        INP 3               ; GET NEW STATUS BYTE
        ANI H'08            ; FET FRAMING ERROR BIT
        STXD
        INP 2               ; DO A DUMMY READ TO
; CLEAR THE DATA AVAILABLE SIGNAL
        LDI H'00
        STR CSTACK
        SEP RC
        ;
        ; SEND CR/LF TO TERMINAL
        ;
        .DB H'82,H'43,H'D2  ; CR
        .DW QTERM - 12
CR:     .DW * + 2
        SEX RSTACK
CR1:    INP 3               ; GET THRE BIT
        SHL
        BNF CR1
        LDI H'0D
        STR RSTACK
        OUT 2
        DEC RSTACK
CR2:    INP 3
        SHL
        BNF CR2
        LDI H'0A
        STR RSTACK
        OUT 2
        DEC RSTACK
        SEX R9
        SEP RC
        ;
        ;
NEST:   GHI RA
        STR R2
        DEC R2
        GLO RA
        STR R2
        DEC R2
        GHI RB
        PHI RA
        GLO RB
        PLO RA
        SEP RC
        ;
VAR:    INC R9
        INC R9
        GHI RB
        STR R9
        INC R9
        GLO RB
        STR R9
        DEC R9
        SEP RC
        ;
CONST:  INC R9
        INC R9
        LDA RB
        STR R9
        INC R9
        LDA RB
        STR R9
        DEC R9
        SEP RC
        ;
USER:   INC R9
        INC R9
        SEX R9
        LDA RB
        STR R9
        INC R9
        LDA RB
        STR R9
        GLO RD
        ADD
        STXD
        GHI RD
        ADC
        STR R9
        SEP RC
;
; FROM HERE ON THE SOURCE IS GENERALLY
; DEFINED WITH FORTH WORD ADDRESSES
;
        .DW H'81B0          ; 0
        .DW CR - 5
ZERO:   .DW CONST
        .DW H'0000
        ;
        .DW H'81B1          ; 1
        .DW ZERO - 4
ONE:    .DW CONST
        .DW H'0001
        ;
        .DW H'81B2          ; 2
        .DW ONE - 4
TWO:    .DW CONST
        .DW H'0002
        ;
        .DB H'82,H'42,H'CC  ; BL
        .DW TWO - 4         ; CONSTANT ASCII BLANK
BL:     .DW CONST
        .DW H'0020
        ;
        .DB H'83,H'43,H'2F,H'CC ; C/L
                                ; CHARACTERS PER LINE
        .DW BL - 5
CL:     .DW CONST
        .DW H'0040          ; 64 (DECIMAL)
        ;
        .DB H'85,"FIRS",H'D4 ; FIRST
        .DW CL - 6
FIRST:  .DW CONST
        .DW FIRSTB
        ;
        .DB H'85,"LIMI",H'D4 ; LIMIT
        .DW FIRST - 8
LIMIT:  .DW CONST
        .DW LIMITB
        ;
        .DB H'85,"B/BU",H'C6 ; B/BUF
	                       ; BYTES PER BUFFER
        .DW LIMIT - 8
BBUF:   .DW CONST
        .DW H'0400          ; 1024 BYTES/BUFFER
        ;
        .DB H'85,"B/SC",H'D2 ; B/SCR
	                       ; BUFFERS/SCREEN
        .DW BBUF - 8
BSCR:   .DW CONST
        .DW H'0001
        ;
        .DB H'86,"ORIGI",H'CE ; ORIGIN
        .DW BSCR - 8
ORGN:   .DW CONST
        .DW H'005E
        ;
        .DB H'87,"+ORIGI",H'CE ; +ORIGIN

        .DW ORGN - 9
PORGN:  .DW NEST
        .DW ORGN
        .DW PLUS
        .DW SEMIS
        ;
        ; USER VARIABLES
        ;
        .DB H'82,H'53,H'B0  ; S0
        .DW PORGN - 10
SO:     .DW USER
        .DW H'0006
        ;
        .DB H'82,H'52,H'B0  ; R0
        .DW SO - 5
RO:     .DW USER
        .DW H'0008
        ;
        .DB H'83,"TI",H'C2  ; TIB
        .DW RO - 5
TIB:    .DW USER
        .DW H'000A
        ;
        .DB H'85,"WIDT",H'C8 ; WIDTH
        .DW TIB - 6
WIDTH:  .DW USER
        .DW H'000C
        ;
        .DB H'87,"WARNIN",H'C7 ; WARNING
	.DW WIDTH - 8
WRNG:   .DW USER
        .DW H'000E
        ;
        .DB H'85,"FENC",H'C5 ; FENCE   FORGET BOUNDRY
        .DW WRNG - 10
FNCE:   .DW USER
        .DW H'0010
        ;
        .DB H'82,H'44,H'D0  ; DP
        .DW FNCE - 8
DP:     .DW USER
        .DW H'0012
        ;
        .DB H'88,"VOC-LIN",H'CB ; VOC-LINK
	.DW DP - 5
VL:     .DW USER
        .DW H'0014
        ;
        .DB H'83,H'42,H'4C,H'CB ; BLK
        .DW VL - 11
BLK:    .DW USER
        .DW H'0016
        ;
        .DB H'82,H'49,H'CE  ; IN
        .DW BLK - 6
FIN:    .DW USER
        .DW H'0018
        ;
        .DB H'83,"OU",H'D4  ; OUT
        .DW FIN - 5
FOUT:   .DW USER
        .DW H'001A
        ;
        .DB H'83,"SC",H'D2  ; SCR
        .DW FOUT - 6
FSCR:   .DW USER
        .DW H'001C
        ;
        .DB H'86,"OFFSE",H'D4 ; OFFSET
        .DW FSCR - 6
OFST:   .DW USER
        .DW H'001E
        ;
        .DB H'87,"CONTEX",H'D4 ; CONTEXT
	.DW OFST - 9
CNTX:   .DW USER
        .DW H'0020
        ;
        .DB H'87,"CURREN",H'D4 ; CURRENT
	.DW CNTX - 10
CRNT:   .DW USER
        .DW H'0022
        ;
        .DB H'85,"STAT",H'C5 ; STATE
        .DW CRNT - 10
STT:    .DW USER
        .DW H'0024
        ;
        .DB H'84,"BAS",H'C5 ; BASE
        .DW STT - 8
BASE:   .DW USER
        .DW H'0026
        ;
        .DB H'83,H'44,H'50,H'CC ; DPL
        .DW BASE - 7
DPL:    .DW USER
        .DW H'0028
        ;
        .DB H'83,H'46,H'4C,H'C4 ; FLD
        .DW DPL - 6
FLD:    .DW USER
        .DW H'002A
        ;
        .DB H'83,H'43,H'53,H'D0 ; CSP
        .DW FLD - 6
CSP:    .DW USER
        .DW H'002C
        ;
        .DB H'82,H'52,H'A3  ; R#
        .DW CSP - 6
RNU:    .DW USER
        .DW H'002E
        ;
        .DB H'83,H'48,H'4C,H'C4 ; HLD
        .DW RNU - 5
HLD:    .DW USER
        .DW H'0030
        .DB H'82,H'44,H'D6  ; DV
        .DW HLD - 6         ; 3 BYTE VECTOR AREA
; USED DURING DISK OPERATIONS
DV:     .DW USER
        .DW H'0032
        ;
        ; END OF USER VARIABLES
        ;
        .DB H'82,H'31,H'AB  ; 1+
        .DW DV - 5
PLUS1:  .DW NEST
        .DW ONE
        .DW PLUS
        .DW SEMIS
        ;
        .DB H'82,H'32,H'AB  ; 2+
        .DW PLUS1 - 5
PLUS2:  .DW NEST
        .DW TWO
        .DW PLUS
        .DW SEMIS
        ;
        .DB H'84,"HER",H'C5 ; HERE
        .DW PLUS2 - 5
HERE:   .DW NEST
        .DW DP
        .DW AT
        .DW SEMIS
        ;
        .DB H'85,"ALLO",H'D4 ; ALLOT
        .DW HERE - 7
ALLOT:  .DW NEST
        .DW DP
        .DW PLUSS
        .DW SEMIS
        ;
        .DW H'81AC          ; , (COMMA)
        .DW ALLOT - 8
COMMA:  .DW NEST
        .DW HERE
        .DW EX
        .DW TWO
        .DW ALLOT
        .DW SEMIS
        ;
        .DB H'82,H'43,H'AC  ; C,
        .DW COMMA - 4
CCMA:   .DW NEST
        .DW HERE
        .DW CEX
        .DW ONE
        .DW ALLOT
        .DW SEMIS
        ;
        .DW H'81AD          ; - (MINUS SIGN)
        .DW CCMA - 5
MINS:   .DW NEST
        .DW MINUS
        .DW PLUS
        .DW SEMIS
        ;
        .DW H'81BD          ; = (EQUAL SIGN)
        .DW MINS - 4
EQL:    .DW NEST
        .DW MINS
        .DW ZEQAL
        .DW SEMIS
        ;
        .DW H'81BC          ; < (LESS THAN SIGN)
        .DW EQL - 4
LESS:   .DW NEST
        .DW MINS
        .DW ZLESS
        .DW SEMIS
        ;
        .DW H'81BE          ; > (GTR THAN SIGN)
        .DW LESS - 4
GTR:    .DW NEST
        .DW SWAP
        .DW LESS
        .DW SEMIS
        ;
        .DB H'83,H'52,H'4F,H'D4 ; ROT
        .DW GTR - 4
ROT:    .DW NEST
        .DW GR
        .DW SWAP
        .DW RG
        .DW SWAP
        .DW SEMIS
        ;
        .DB H'85,"SPAC",H'C5 ; SPACE
        .DW ROT - 6
SPC:    .DW NEST
        .DW BL
        .DW EMIT
        .DW SEMIS
        ;
        .DB H'84,"-DU",H'D0 ; -DUP
        .DW SPC - 8
MDUP:   .DW NEST
        .DW DUP
        .DW ZBRCH
        .DW * + 4
        .DW DUP
        .DW SEMIS
        ;
        .DB H'88,"TRAVERS",H'C5 ; TRAVERSE
	.DW MDUP - 7
TRVS:   .DW NEST
        .DW SWAP
TR1:    .DW OVER
        .DW PLUS
        .DW LIT
        .DW H'007F
        .DW OVER
        .DW CAT
        .DW LESS
        .DW ZBRCH
        .DW TR1
        .DW SWAP
        .DW DROP
        .DW SEMIS
        ;
        ;
        .DB H'86,"LATES",H'D4 ; LATEST
        .DW TRVS - 11
LTST:   .DW NEST
        .DW CRNT
        .DW AT
        .DW AT
        .DW SEMIS
        ;
        ;
        .DB H'83,H'4C,H'46,H'C1 ; LFA
        .DW LTST - 9        ; LINK FIELD ADDRESS
LFA:    .DW NEST
        .DW LIT
        .DW H'0004
        .DW MINS
        .DW SEMIS
        ;
        .DB H'83,H'43,H'46,H'C1 ; CFA
        .DW LFA - 6         ; CODE FIELD ADDRESS
CFA:    .DW NEST
        .DW TWO
        .DW MINS
        .DW SEMIS
        ;
        ;
        .DB H'83,H'4E,H'46,H'C1 ; NFA
        .DW CFA - 6         ; NAME FIELD ADDRESS
NFA:    .DW NEST
        .DW LIT
        .DW H'0005
        .DW MINS
        .DW LIT
        .DW H'FFFF
        .DW TRVS
        .DW SEMIS
        ;
        ;
        .DB H'83,H'50,H'46,H'C1 ; PFA
        .DW NFA - 6         ; PARAMETER FIELD ADDRESS
PFA:    .DW NEST
        .DW ONE
        .DW TRVS
        .DW LIT
        .DW H'0005
        .DW PLUS
        .DW SEMIS
        ;
        ;
        .DB H'84,H'21,H'43,H'53,H'D0 ; !CSP
        .DW PFA - 6
DCSP:   .DW NEST
        .DW FSPAT
        .DW CSP
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'86,"?ERRO",H'D2 ; ?ERROR
        .DW DCSP - 7
QERR:   .DW NEST
        .DW SWAP
        .DW ZBRCH
        .DW * + 8
        .DW ERROR
        .DW BRCH
        .DW * + 4
        .DW DROP
        .DW SEMIS
        ;
        ;
        .DB H'85,"?COM",H'D0 ; ?COMP
        .DW QERR - 9
QCMP:   .DW NEST
        .DW STT
        .DW AT
        .DW ZEQAL
        .DW LIT
        .DW H'0011
        .DW QERR
        .DW SEMIS
        ;
        ;
        .DB H'85,"?EXE",H'C3 ; ?EXEC
        .DW QCMP - 8
EXC:    .DW NEST
        .DW STT
        .DW AT
        .DW LIT
        .DW H'0012
        .DW QERR
        .DW SEMIS
        ;
        ;
        .DB H'86,"?PAIR",H'D3 ; ?PAIRS
        .DW EXC - 8
QPR:    .DW NEST
        .DW MINS
        .DW LIT
        .DW H'0013
        .DW QERR
        .DW SEMIS
        ;
        ;
        .DB H'84,H'3F,H'43,H'53,H'D0 ; ?CSP
        .DW QPR - 9
QCSP:   .DW NEST
        .DW FSPAT
        .DW CSP
        .DW AT
        .DW MINS
        .DW LIT
        .DW H'0014
        .DW QERR
        .DW SEMIS
        ;
        ;
        .DB H'88,"?LOADIN",H'C7 ; ?LOADING

        .DW QCSP - 7
QLDG:   .DW NEST
        .DW BLK
        .DW AT
        .DW ZEQAL
        .DW LIT
        .DW H'0016
        .DW QERR
        .DW SEMIS
        ;
        ;
        .DB H'87,"COMPIL",H'C5 ; COMPILE
	.DW QLDG - 11
CMPL:   .DW NEST
        .DW QCMP
        .DW RG
        .DW DUP
        .DW PLUS2
        .DW GR
        .DW AT
        .DW COMMA
        .DW SEMIS
        ;
	;
        .DW H'C1DB          ; [   LEFT BRACKET
        .DW CMPL - 10
LB:     .DW NEST
        .DW ZERO
        .DW STT
        .DW EX
        .DW SEMIS
        ;
        ;
        .DW H'81DD          ; ]   RIGHT BRACKET
        .DW LB - 4
RBK:    .DW NEST
        .DW LIT
        .DW H'00C0
        .DW STT
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'86,"SMUDG",H'C5 ; SMUDGE
        .DW RBK - 4
SMDG:   .DW NEST
        .DW LTST
        .DW LIT
        .DW H'0020
        .DW TGLE
        .DW SEMIS
        ;
        ;
        .DB H'83,H'48,H'45,H'D8 ; HEX
        .DW SMDG - 9
MHEX:   .DW NEST
        .DW LIT
        .DW H'0010
        .DW BASE
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'87,"DECIMA",H'CC ; DECIMAL
        .DW MHEX - 6
MDCML:  .DW NEST
        .DW LIT
        .DW H'000A
        .DW BASE
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'87,"(;CODE",H'A9 ; (;CODE)
        .DW MDCML - 10
PCODE:  .DW NEST
        .DW RG
        .DW LTST
        .DW PFA
        .DW CFA
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'C5,";COD",H'C5 ; ;CODE (IMMEDIATE)
        .DW PCODE - 10
CODE:   .DW NEST
        .DW QCSP
        .DW CMPL
        .DW PCODE
        .DW LB
        .DW SMDG
        .DW SEMIS
        ;
        ;
        .DB H'85,"COUN",H'D4 ; COUNT
        .DW CODE - 8
CNT:    .DW NEST
        .DW DUP
        .DW PLUS1
        .DW SWAP
        .DW CAT
        .DW SEMIS
        ;
        ;
        .DB H'84,"TYP",H'C5 ; TYPE
        .DW CNT - 8
TYPE:   .DW NEST
        .DW MDUP
        .DW ZBRCH
        .DW * + 24
        .DW OVER
        .DW PLUS
        .DW SWAP
        .DW PDO
TYP1:   .DW R
        .DW CAT
        .DW EMIT
        .DW LUPE
        .DW TYP1
        .DW BRCH
        .DW * + 4
        .DW DROP
        .DW SEMIS
        ;
        ;
        .DB H'89,"-TRAILIN",H'C7 ; -TRAILING
        .DW TYPE - 7
TRLG:   .DW NEST
        .DW DUP
        .DW ZERO
        .DW PDO
TRL1:   .DW OVER
        .DW OVER
        .DW PLUS
        .DW ONE
        .DW MINS
        .DW CAT
        .DW BL
        .DW MINS
        .DW ZBRCH
        .DW * + 8
        .DW LVE
        .DW BRCH
        .DW * + 6
        .DW ONE        
        .DW MINS
        .DW LUPE
        .DW TRL1
        .DW SEMIS
        ;
        ;
        .DB H'84,H'28,H'2E,H'22,H'A9 ; (.")
        .DW TRLG - 12
PDQ:    .DW NEST
        .DW R
        .DW CNT
        .DW DUP
        .DW PLUS1
        .DW RG
        .DW PLUS
        .DW GR
        .DW TYPE
        .DW SEMIS
        ;
        ;
        .DB H'86,"EXPEC",H'D4 ; EXPECT
        .DW PDQ - 7
EXPT:   .DW NEST
        .DW OVER
        .DW PLUS
        .DW OVER
        .DW PDO
EXPT4:  .DW KEY
        .DW DUP
        .DW LIT
        .DW H'000E
        .DW PORGN
        .DW AT
        .DW EQL
        .DW ZBRCH
        .DW EXPT1
        .DW DROP
        .DW LIT
        .DW H'0008
        .DW OVER
        .DW R
        .DW EQL
        .DW DUP
        .DW RG
        .DW TWO
        .DW MINS
        .DW PLUS
        .DW GR
        .DW MINS
        .DW BRCH
        .DW EXPT2
EXPT1:  .DW DUP
        .DW LIT
        .DW H'000D
        .DW EQL
        .DW ZBRCH
        .DW EXPT3
        .DW LVE
        .DW DROP
        .DW BL
        .DW ZERO
        .DW BRCH
        .DW EXPT5
EXPT3:  .DW DUP
EXPT5:  .DW R
        .DW CEX
        .DW ZERO
        .DW R
        .DW PLUS1
        .DW EX
EXPT2:  .DW EMIT
        .DW LUPE
        .DW EXPT4
        .DW DROP
        .DW SEMIS
        ;
        ;
        .DB H'85,"QUER",H'D9 ; QUERY
        .DW EXPT - 9         ; INPUT LINE OF TEXT
QUER:   .DW NEST
        .DW TIB
        .DW AT
        .DW LIT
        .DW H'0050
        .DW EXPT
        .DW ZERO
        .DW FIN
        .DW EX
        .DW SEMIS
        ;
        ;
        .DW H'C180           ; X   (IMMEDIATE)
        .DW QUER - 8
X:      .DW NEST
        .DW BLK
        .DW AT
        .DW ZBRCH
        .DW X2
        .DW ONE
        .DW BLK
        .DW PLUSS
        .DW ZERO
        .DW FIN
        .DW EX
        .DW BLK
        .DW AT
        .DW LIT
        .DW H'0007
        .DW FAND
        .DW ZEQAL
        .DW ZBRCH
        .DW X1
        .DW EXC
        .DW RG
        .DW DROP
X1:     .DW BRCH
        .DW XEND
X2:     .DW RG
        .DW DROP
XEND:   .DW SEMIS
        ;
        ;
        .DB H'84,"FIL",H'CC ; FILL   FILL MEMORY
        .DW X - 4
FILL:   .DW NEST
        .DW SWAP
        .DW GR
        .DW OVER
        .DW CEX
        .DW DUP
        .DW PLUS1
        .DW RG
        .DW ONE
        .DW MINS
        .DW CMOVE
        .DW SEMIS
        ;
        ;
        .DB H'85,"ERAS",H'C5 ; ERASE   ZERO MEMORY
        .DW FILL - 7
ERS:    .DW NEST
        .DW ZERO
        .DW FILL
        .DW SEMIS
        ;
        ;
        .DB H'86,"BLANK",H'D3 ; BLANKS
        .DW ERS - 8           ; FILL MEMORY WITH
BLNK:   .DW NEST              ; ASCII BLANKS
        .DW BL
        .DW FILL
        .DW SEMIS
        ;
        ;
        .DB H'84,"HOL",H'C4 ; HOLD
        .DW BLNK - 9
HOLD:   .DW NEST
        .DW LIT
        .DW H'FFFF          ; -1
        .DW HLD
        .DW PLUSS
        .DW HLD
        .DW AT
        .DW CEX
        .DW SEMIS
        ;
        ;
        .DB H'83,"PA",H'C4  ; PAD
        .DW HOLD - 7
PAD:    .DW NEST
        .DW HERE
        .DW LIT
        .DW H'0044
        .DW PLUS
        .DW SEMIS
        ;
        ;
        .DB H'84,"WOR",H'C4 ; WORD
        .DW PAD - 6
WORD:   .DW NEST
        .DW BLK
        .DW AT
        .DW ZBRCH
        .DW WD1
        .DW BLK
        .DW AT
        .DW BLOCK
        .DW BRCH
        .DW WD2
WD1:    .DW TIB
        .DW AT
WD2:    .DW FIN
        .DW AT
        .DW PLUS
        .DW SWAP
        .DW ENCL
        .DW HERE
        .DW LIT
        .DW H'0022
        .DW BLNK
        .DW FIN
        .DW PLUSS
        .DW OVER
        .DW MINS
        .DW GR
        .DW R
        .DW HERE
        .DW CEX
        .DW PLUS
        .DW HERE
        .DW PLUS1
        .DW RG
        .DW CMOVE
        .DW SEMIS
        ;
        ;
        .DB H'88,"(NUMBER",H'A9 ; (NUMBER)
        .DW WORD - 7
PNMBR:  .DW NEST
        .DW PLUS1
        .DW DUP
        .DW GR
        .DW CAT
        .DW BASE
        .DW AT
        .DW DGT
        .DW ZBRCH
        .DW PNM2
        .DW SWAP
        .DW BASE
        .DW AT
        .DW USTAR
        .DW DROP
        .DW ROT
        .DW BASE
        .DW AT
        .DW USTAR
        .DW DPLUS
        .DW DPL
        .DW AT
        .DW PLUS1
        .DW ZBRCH
        .DW PNM1
        .DW ONE
        .DW DPL
        .DW PLUSS
PNM1:   .DW RG
        .DW BRCH
        .DW PNMBR + 2
PNM2:   .DW RG
        .DW SEMIS
        ;
        ;
        ;
        .DB H'86,"NUMBE",H'D2 ; NUMBER
        .DW PNMBR - 11
NMBR:   .DW NEST
        .DW ZERO
        .DW ZERO
        .DW ROT
        .DW DUP
        .DW PLUS1
        .DW CAT
        .DW LIT
        .DW H'002D
        .DW EQL
        .DW DUP
        .DW GR
        .DW PLUS
        .DW LIT
        .DW H'FFFF          ; -1
NMB1:   .DW DPL
        .DW EX
        .DW PNMBR
        .DW DUP
        .DW CAT
        .DW BL
        .DW MINS
        .DW ZBRCH
        .DW NMB2
        .DW DUP
        .DW CAT
        .DW LIT
        .DW H'002E
        .DW MINS
        .DW ZERO
        .DW QERR
        .DW ZERO
        .DW BRCH
        .DW NMB1
NMB2:   .DW DROP
        .DW RG
        .DW ZBRCH
        .DW NMB3
        .DW DMIN
NMB3:   .DW SEMIS
        ;
        ;
        .DB H'85,"-FIN",H'C4 ; -FIND
        .DW NMBR - 9
MFIND:  .DW NEST
        .DW BL
        .DW WORD
        .DW HERE
        .DW CNTX
        .DW AT
        .DW AT
        .DW FIND
        .DW DUP
        .DW ZEQAL
        .DW ZBRCH
        .DW MF1
        .DW DROP
        .DW HERE
        .DW LTST
        .DW FIND
MF1:    .DW SEMIS
        ;
        ;
        .DB H'87,"(ABORT",H'A9 ; (ABORT)
        .DW MFIND - 8
PABRT:  .DW NEST
        .DW ABORT
        .DW SEMIS
        ;
        .DB H'85,"ERRO",H'D2 ; ERROR
        .DW PABRT - 10
ERROR:  .DW NEST
        .DW WRNG
        .DW AT
        .DW ZLESS
        .DW ZBRCH
        .DW ERR1
        .DW PABRT
ERR1:   .DW HERE
        .DW CNT
        .DW TYPE
        .DW PDQ
        .DB H'03,"  ?"
        .DW MSG
        .DW SP1
        .DW FIN
        .DW AT
        .DW BLK
        .DW AT
        .DW QUIT
        .DW SEMIS
        ;
        .DB H'83,"MI",H'CE  ; MIN
        .DW ERROR - 8
MIN:    .DW NEST
        .DW OVER
        .DW OVER
        .DW GTR
        .DW ZBRCH
        .DW MN1
        .DW SWAP
MN1:    .DW DROP
        .DW SEMIS
        ;
        .DB H'83,"ID",H'AE  ; ID.
        .DW MIN - 6
ID:     .DW NEST
        .DW PAD
        .DW LIT
        .DW H'0020
        .DW LIT
        .DW H'005F
        .DW FILL
        .DW DUP
        .DW PFA
        .DW LFA
        .DW OVER
        .DW MINS
        .DW PAD
        .DW SWAP
        .DW CMOVE
        .DW PAD
        .DW CNT
        .DW LIT
        .DW H'001F
        .DW FAND
        .DW TYPE
        .DW SPC
        .DW SEMIS
        ;
        ;
        .DB H'86,"CREAT",H'C5 ; CREATE
        .DW ID - 6
CRTE:   .DW NEST
        .DW FSPAT
        .DW HERE
        .DW LIT
        .DW H'00A0
        .DW PLUS
        .DW LESS
        .DW TWO
        .DW QERR
        .DW MFIND
        .DW ZBRCH
        .DW CRT1
        .DW DROP
        .DW NFA
        .DW ID
        .DW LIT
        .DW H'0004
        .DW MSG
        .DW SPC
CRT1:   .DW HERE
        .DW DUP
        .DW CAT
        .DW WIDTH
        .DW AT
        .DW MIN
        .DW PLUS1
        .DW ALLOT
        .DW DUP
        .DW LIT
        .DW H'00A0
        .DW TGLE
        .DW HERE
        .DW ONE
        .DW MINS
        .DW LIT
        .DW H'0080
        .DW TGLE
        .DW LTST
        .DW COMMA
        .DW CRNT
        .DW AT
        .DW EX
        .DW HERE
        .DW PLUS2
        .DW COMMA
        .DW SEMIS
        ;
        ;
        .DW H'C1BA          ; :   (IMMEDIATE)
        .DW CRTE - 9
COLON:  .DW NEST
        .DW EXC
        .DW DCSP
        .DW CRNT
        .DW AT
        .DW CNTX
        .DW EX
        .DW CRTE
        .DW RBK
        .DW LIT
        .DW H'FFFE          ; -2
        .DW DP
        .DW PLUSS
        .DW CMPL
        .DW NEST
        .DW SEMIS
        ;
        ;
        .DB H'85,"!COD",H'C5 ; !CODE
        .DW COLON - 4
DCODE:  .DW NEST
        .DW CRTE
        .DW SMDG
        .DW LTST
        .DW PFA
        .DW CFA
        .DW EX
        .DW COMMA
        .DW SEMIS
        ;
        ;
        .DB H'88,"CONSTAN",H'D4 ; CONSTANT
        .DW DCODE - 8
CNST:   .DW NEST
        .DW LIT
        .DW CONST
        .DW DCODE
        .DW SEMIS
        ;
        ;
        .DB H'88,"VARIABL",H'C5 ; VARIABLE
        .DW CNST - 11
VARB:   .DW NEST
        .DW LIT
        .DW VAR
        .DW DCODE
        .DW SEMIS
        ;
        ;
        .DB H'84,"USE",H'D2 ; USER
        .DW VARB - 11
USR:    .DW NEST
        .DW LIT
        .DW USER
        .DW DCODE
        .DW SEMIS
        ;
        ;
        .DB H'87,"<BUILD",H'D3 ; <BUILDS
        .DW USR - 7
LBLD:   .DW NEST
        .DW ZERO
        .DW CNST
        .DW SEMIS
        ;
        ;
        .DB H'85,"DOES",H'BE ; DOES>
        .DW LBLD - 10
DOSEG:  .DW NEST
        .DW RG
        .DW LTST
        .DW PFA
        .DW EX
        .DW PCODE
DUZ1:   SEX R2
        GHI RA
        STXD
        GLO RA
        STXD
        LDA RB
        PHI RA
        LDA RB
        PLO RA
        INC R9
        INC R9
        GHI RB
        STR R9
        INC R9
        GLO RB
        STR R9
        DEC R9
        SEP RC
        ;
        ;
        .DB H'C7,"LITERA",H'CC ; LITERAL (IMMEDIATE)
        .DW DOSEG - 8
LTL:    .DW NEST
        .DW STT
        .DW AT
        .DW ZBRCH
        .DW LT1
        .DW CMPL
        .DW LIT
        .DW COMMA
LT1:    .DW SEMIS
        ;
        ;
        .DB H'C8,"DLITERA",H'CC ; DLITERAL (IMMEDIATE)
        .DW LTL - 10
DLTL:   .DW NEST
        .DW STT
        .DW AT
        .DW ZBRCH
        .DW DLTL1
        .DW SWAP
        .DW LTL
        .DW LTL
DLTL1:  .DW SEMIS
        ;
        ;
        .DB H'86,"?STAC",H'CB ; ?STACK
        .DW DLTL - 11
QSTK:   .DW NEST
        .DW SO
        .DW AT
        .DW DUP
        .DW FSPAT
        .DW GTR
        .DW ONE
        .DW QERR
        .DW LIT
        .DW H'0100
        .DW PLUS
        .DW FSPAT
        .DW LESS
        .DW LIT
        .DW H'0007
        .DW QERR
        .DW SEMIS
        ;
        ;
        .DB H'89,"INTERPRE",H'D4 ; INTERPRET
        .DW QSTK - 9
INPT:   .DW NEST
        .DW MFIND
        .DW ZBRCH
        .DW PT1
        .DW STT
        .DW AT
        .DW LESS
        .DW ZBRCH
        .DW PT2
        .DW CFA
        .DW COMMA
        .DW BRCH
        .DW PT3
PT2:    .DW CFA
        .DW EXE
PT3:    .DW QSTK
        .DW BRCH
        .DW PT4
PT1:    .DW HERE
        .DW NMBR
        .DW DPL
        .DW AT
        .DW PLUS1
        .DW ZBRCH
        .DW PT5
        .DW DLTL
        .DW BRCH
        .DW PT6
PT5:    .DW DROP
        .DW LTL
PT6:    .DW QSTK
PT4:    .DW BRCH
        .DW INPT + 2
        .DW SEMIS
        ;
        ;
        .DB H'8A,"VOCABULAR",H'D9 ; VOCABULARY
        .DW INPT - 12
VBLY:   .DW NEST
        .DW LBLD
        .DW LIT
        .DW H'81A0
        .DW COMMA
        .DW CRNT
        .DW AT
        .DW CFA
        .DW COMMA
        .DW HERE
        .DW VL
        .DW AT
        .DW COMMA
        .DW VL
        .DW EX
        .DW DOSEG
VB1:    .DW PLUS2
        .DW CNTX
        .DW EX
        .DW SEMIS
        ;
        ;
FRTH:   .DB H'C5,"FORT",H'C8 ; FORTH (IMMEDIATE)
        .DW VBLY - 13
        .DW DUZ1
        .DW VB1
        .DW H'81A0
        .DW TASK - 7        ; DICTION LINK
        .DW H'0000          ; NOTE THIS MUST BE CHANGED
                            ; TO REFLECT THE LINK TO THE
                            ; LAST DICT WORD IN THE
                            ; FORTH VOCAB
        ;
        ;
        .DB H'8B,"DEFINITION",H'D3 ; DEFINITIONS
        .DW FRTH
DFN:    .DW NEST
        .DW CNTX
        .DW AT
        .DW CRNT
        .DW EX
        .DW SEMIS
        ;
        .DB H'84,"QUI",H'D4 ; QUIT
        .DW DFN - 14
QUIT:   .DW NEST
        .DW ZERO
        .DW BLK
        .DW EX
        .DW LB
Q2:     .DW RP1
        .DW CR
        .DW QUER
        .DW INPT
        .DW STT
        .DW AT
        .DW ZEQAL
        .DW ZBRCH
        .DW Q1
        .DW PDQ
        .DB H'04,"  OK"
Q1:     .DW BRCH
        .DW Q2
        .DW SEMIS
        ;
        ;
        .DB H'85,"ABOR",H'D4 ; ABORT
        .DW QUIT - 7
ABORT:  .DW NEST
        .DW SP1
        .DW MDCML
        .DW CR
        .DW PDQ
        .DB H'1C,"1802 FIG-FORTH R0.4  3/16/81"
	.DW DRZER
        .DW MTBUF
        .DW FIRST
        .DW DUP
        .DW PREV
        .DW EX
        .DW USE
        .DW EX
        .DW FRTH + 8
        .DW DFN
        .DW QUIT
        .DW SEMIS
        ;
        ;
        .DW H'C1BB          ; ;   (IMMEDIATE)
        .DW ABORT - 8
SEMIC:  .DW NEST
        .DW QCSP
        .DW CMPL
        .DW SEMIS
        .DW SMDG
        .DW LB
        .DW SEMIS
        ;
        ;
        ;
        .DB H'C2,H'2E,H'A2  ; ."   (IMMEDIATE)
        .DW SEMIC - 4
DOTQ:   .DW NEST
        .DW LIT
        .DW H'0022
        .DW STT
        .DW AT
        .DW ZBRCH
        .DW DOTQ1
        .DW CMPL
        .DW PDQ
        .DW WORD
        .DW HERE
        .DW CAT
        .DW PLUS1
        .DW ALLOT
        .DW BRCH
        .DW DOTQ2
DOTQ1:  .DW WORD
        .DW HERE
        .DW CNT
        .DW TYPE
DOTQ2:  .DW SEMIS
        ;
        ;
        .DB H'C9,"[COMPILE",H'DD ; [COMPILE]
        .DW DOTQ - 5             ; (IMMEDIATE)
BCOMP:  .DW NEST
        .DW MFIND
        .DW ZEQAL
        .DW ZERO
        .DW QERR
        .DW DROP
        .DW CFA
        .DW COMMA
        .DW SEMIS
        ;
        ;
        .DB H'89,"IMMEDIAT",H'C5 ; IMMEDIATE
        .DW BCOMP - 12
IMMED:  .DW NEST
        .DW LTST
        .DW LIT
        .DW H'0040
        .DW TGLE
        .DW SEMIS
        ;
        ;
        .DW H'C1A8          ; (   (IMMEDIATE)
        .DW IMMED - 12
PAREN:  .DW NEST
        .DW LIT
        .DW H'0029
        .DW WORD
        .DW SEMIS
        ;
        ;
        .DW H'81B3          ; 3
        .DW PAREN - 4
THREE:  .DW CONST
        .DW H'0003
        ;
        ;
        .DW H'C1A7          ; '   (TICK)   (IMMEDIATE)
        .DW THREE - 4
TICK:   .DW NEST
        .DW MFIND
        .DW ZEQAL
        .DW ZERO
        .DW QERR
        .DW DROP
        .DW LTL
        .DW SEMIS
        ;
        ;
        .DB H'86,"FORGE",H'D4 ; FORGET
        .DW TICK - 4
FORG:   .DW NEST
        .DW CRNT
        .DW AT
        .DW CNTX
        .DW AT
        .DW MINS
        .DW LIT
        .DW H'0018
        .DW QERR
        .DW TICK
        .DW DUP
        .DW FNCE
        .DW AT
        .DW LESS
        .DW LIT
        .DW H'0015
        .DW QERR
        .DW DUP
        .DW NFA
        .DW DP
        .DW EX
        .DW LFA
        .DW AT
        .DW CNTX
        .DW AT
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'82,H'2B,H'AD  ; +-
        .DW FORG - 9
PM:     .DW NEST
        .DW ZLESS
        .DW ZBRCH
        .DW PM1
        .DW MINUS
PM1:    .DW SEMIS
        ;
        .DB H'83,H'44,H'2B,H'AD ; D+-
        .DW PM - 5
DPM:    .DW NEST
        .DW ZLESS
        .DW ZBRCH
        .DW DPM1
        .DW DMIN
DPM1:   .DW SEMIS
        ;
        .DB H'83,H'41,H'42,H'D3 ; ABS
        .DW DPM - 6
ABS:    .DW NEST
        .DW DUP
        .DW PM
        .DW SEMIS
        ;
        .DB H'84,"DAB",H'D3 ; DABS
        .DW ABS - 6
DABS:   .DW NEST
        .DW DUP
        .DW DPM
        .DW SEMIS
        ;
        .DB H'83,H'4D,H'41,H'D8 ; MAX
        .DW DABS - 7
MAX:    .DW NEST
        .DW OVER
        .DW OVER
        .DW LESS
        .DW ZBRCH
        .DW MAX1
        .DW SWAP
MAX1:   .DW DROP
        .DW SEMIS
        ;
        .DB H'82,H'4D,H'AA  ; M*
        .DW MAX - 6
MSTAR:  .DW NEST
        .DW OVER
        .DW OVER
        .DW FXOR
        .DW GR
        .DW ABS
        .DW SWAP
        .DW ABS
        .DW USTAR
        .DW RG
        .DW DPM
        .DW SEMIS
        ;
        .DB H'82,H'4D,H'AF  ; M/
        .DW MSTAR - 5
MSLAS:  .DW NEST
        .DW OVER
        .DW GR
        .DW GR
        .DW DABS
        .DW R
        .DW ABS
        .DW USLSH
        .DW RG
        .DW R
        .DW FXOR
        .DW PM
        .DW SWAP
        .DW RG
        .DW PM
        .DW SWAP
        .DW SEMIS
        ;
        .DW H'81AA          ; *
        .DW MSLAS - 5
STAR:   .DW NEST
        .DW MSTAR
        .DW DROP
        .DW SEMIS
        ;
        .DB H'84,H'2F,H'4D,H'4F,H'C4 ; /MOD
        .DW STAR - 4
SLMOD:  .DW NEST
        .DW GR
        .DW STOD
        .DW RG
        .DW MSLAS
        .DW SEMIS
        ;
        .DW H'81AF          ; /
        .DW SLMOD - 7
SLASH:  .DW NEST
        .DW SLMOD
        .DW SWAP
        .DW DROP
        .DW SEMIS
        ;
        .DB H'83,H'4D,H'4F,H'C4 ; MOD
        .DW SLASH - 4
MODD:   .DW NEST
        .DW SLMOD
        .DW DROP
        .DW SEMIS
        ;
        .DB H'85,"*/MO",H'C4 ; */MOD
        .DW MODD - 6
SSMOD:  .DW NEST
        .DW GR
        .DW MSTAR
        .DW RG
        .DW MSLAS
        .DW SEMIS
        ;
        .DB H'82,H'2A,H'AF ; */
        .DW SSMOD - 8
SSLA:   .DW NEST
        .DW SSMOD
        .DW SWAP
        .DW DROP
        .DW SEMIS
        ;
        .DB H'85,"M/MO",H'C4 ; M/MOD
        .DW SSLA - 5
MSMOD:  .DW NEST
        .DW GR
        .DW ZERO
        .DW R
        .DW USLSH
        .DW RG
        .DW SWAP
        .DW GR
        .DW USLSH
        .DW RG
        .DW SEMIS
        ;
        ;
        .DB H'83,"MO",H'CE  ; MON
        .DW MSMOD - 8       ; RETURN TO MONITOR
MON:    .DW * + 2           ; AT 8000 HEX
        LDI H'80
        PHI R0
        LDI H'00
        PLO R0
        SEX R0
        SEP R0
        ;
        ;
        .DB H'83,"BY",H'C5  ; BYE
        .DW MON - 6
BYE:    .DW NEST
        .DW FLUSH
        .DW MON
        ;
        ;
        .DB H'84,"BAC",H'CB ; BACK
        .DW BYE - 6
BACK:   .DW NEST
        .DW COMMA
        .DW SEMIS
        ;
        .DB H'C5,"BEGI",H'CE ; BEGIN
        .DW BACK - 7
BEGIN:  .DW NEST
        .DW QCMP
        .DW HERE
        .DW ONE
        .DW SEMIS
        ;
        .DB H'C5,"ENDI",H'C6 ; ENDIF
        .DW BEGIN - 8
ENDIFF: .DW NEST
        .DW QCMP
        .DW TWO
        .DW QPR
        .DW HERE
        .DW SWAP
        .DW EX
        .DW SEMIS
        ;
        .DB H'C4,"THE",H'CE ; THEN
        .DW ENDIFF - 8
THEN:   .DW NEST
        .DW ENDIFF
        .DW SEMIS
        ;
        .DB H'C2,H'44,H'CF  ; DO
        .DW THEN - 7
DO:     .DW NEST
        .DW CMPL
        .DW PDO
        .DW HERE
        .DW THREE
        .DW SEMIS
        ;
        .DB H'C4,H'4C,H'4F,H'4F,H'D0 ; LOOP
        .DW DO - 5
LOOP:   .DW NEST
        .DW THREE
        .DW QPR
        .DW CMPL
        .DW LUPE
        .DW BACK
        .DW SEMIS
        ;
        .DB H'C5,"+LOO",H'D0 ; +LOOP
        .DW LOOP - 7
PLOOP:  .DW NEST
        .DW THREE
        .DW QPR
        .DW CMPL
        .DW PLUPE
        .DW BACK
        .DW SEMIS
        ;
        .DB H'C5,"UNTI",H'CC ; UNTIL
        .DW PLOOP - 8
UNTIL:  .DW NEST
        .DW ONE
        .DW QPR
        .DW CMPL
        .DW ZBRCH
        .DW BACK
        .DW SEMIS
        ;
        .DB H'C3,H'45,H'4E,H'C4 ; END
        .DW UNTIL - 8
ENDD:   .DW NEST
        .DW UNTIL
        .DW SEMIS
        ;
        .DB H'C5,"AGAI",H'CE ; AGAIN
        .DW ENDD - 6
AGAIN:  .DW NEST
        .DW ONE
        .DW QPR
        .DW CMPL
        .DW BRCH
        .DW BACK
        .DW SEMIS
        ;
        .DB H'C6,"REPEA",H'D4 ; REPEAT
        .DW AGAIN - 8
REPEA:  .DW NEST
        .DW GR
        .DW GR
        .DW AGAIN
        .DW RG
        .DW RG
        .DW TWO
        .DW MINS
        .DW ENDIFF
        .DW SEMIS
        ;
        .DB H'C2,H'49,H'C6 ; IF
        .DW REPEA - 9
IFF:    .DW NEST
        .DW CMPL
        .DW ZBRCH
        .DW HERE
        .DW ZERO
        .DW COMMA
        .DW TWO
        .DW SEMIS
        ;
        .DB H'C4,"ELS",H'C5 ; ELSE
        .DW IFF - 5
ELSEE:  .DW NEST
        .DW TWO
        .DW QPR
        .DW CMPL
        .DW BRCH
        .DW HERE
        .DW ZERO
        .DW COMMA
        .DW SWAP
        .DW TWO
        .DW ENDIFF
        .DW TWO
        .DW SEMIS
        ;
        .DB H'C5,"WHIL",H'C5 ; WHILE
        .DW ELSEE - 7
WHILE:  .DW NEST
        .DW IFF
        .DW PLUS2
        .DW SEMIS
        ;
        .DB H'86,"SPACE",H'D3 ; SPACES
        .DW WHILE - 8
SPACS:  .DW NEST
        .DW ZERO
        .DW MAX
        .DW MDUP
        .DW ZBRCH
        .DW SPAX1
        .DW ZERO
        .DW PDO
SPAX2:  .DW SPC
        .DW LUPE
        .DW SPAX2
SPAX1:  .DW SEMIS
        ;
        .DB H'82,H'3C,H'A3  ; <#
        .DW SPACS - 9
BDIGS:  .DW NEST
        .DW PAD
        .DW HLD
        .DW EX
        .DW SEMIS
        ;
        .DB H'82,H'23,H'BE  ; #>
        .DW BDIGS - 5
EDIGS:  .DW NEST
        .DW DROP
        .DW DROP
        .DW HLD
        .DW AT
        .DW PAD
        .DW OVER
        .DW MINS
        .DW SEMIS
        ;
        .DB H'84,"SIG",H'CE ; SIGN
        .DW EDIGS - 5
SIGN:   .DW NEST
        .DW ROT
        .DW ZLESS
        .DW ZBRCH
        .DW SIGN1
        .DW LIT
        .DW H'002D
        .DW HOLD
SIGN1:  .DW SEMIS
        ;
        .DW H'81A3          ; #
        .DW SIGN - 7
DIG:    .DW NEST
        .DW BASE
        .DW AT
        .DW MSMOD
        .DW ROT
        .DW LIT
        .DW H'0009
        .DW OVER
        .DW LESS
        .DW ZBRCH
        .DW DIG1
        .DW LIT
        .DW H'0007
        .DW PLUS
DIG1:   .DW LIT
        .DW H'0030
        .DW PLUS
        .DW HOLD
        .DW SEMIS
        ;
        .DB H'82,H'23,H'D3  ; #S
        .DW DIG - 4
DIGS:   .DW NEST
DIGS1:  .DW DIG
        .DW OVER
        .DW OVER
        .DW FFOR
        .DW ZEQAL
        .DW ZBRCH
        .DW DIGS1
        .DW SEMIS
        ;
        .DB H'83,H'44,H'2E,H'D2 ; D.R
        .DW DIGS - 5
DDOTR:  .DW NEST
        .DW GR
        .DW SWAP
        .DW OVER
        .DW DABS
        .DW BDIGS
        .DW DIGS
        .DW SIGN
        .DW EDIGS
        .DW RG
        .DW OVER
        .DW MINS
        .DW SPACS
        .DW TYPE
        .DW SEMIS
        ;
        .DB H'82,H'2E,H'D2  ; .R
        .DW DDOTR - 6
DOTR:   .DW NEST
        .DW GR
        .DW STOD
        .DW RG
        .DW DDOTR
        .DW SEMIS
        ;
        .DB H'82,H'44,H'AE  ; D.
        .DW DOTR - 5
DDOT:   .DW NEST
        .DW ZERO
        .DW DDOTR
        .DW SPC
        .DW SEMIS
        ;
        .DW H'81AE          ; .   (DOT)
        .DW DDOT - 5
DOT:    .DW NEST
        .DW STOD
        .DW DDOT
        .DW SEMIS
        ;
        .DW H'81BF          ; ?
        .DW DOT - 4
QUES:   .DW NEST
        .DW AT
        .DW DOT
        .DW SEMIS
        ;
        .DB H'82,H'55,H'AE  ; U.
        .DW QUES - 4
UDOT:   .DW NEST
        .DW ZERO
        .DW DDOT
        .DW SEMIS
        ;
        .DB H'85,"VLIS",H'D4 ; VLIST
        .DW UDOT - 5
VLIST:  .DW NEST
        .DW CR
        .DW LIT
        .DW H'0080
        .DW FOUT
        .DW EX
        .DW CNTX
        .DW AT
        .DW AT
VLIS1:  .DW FOUT
        .DW AT
        .DW CL
        .DW GTR
        .DW ZBRCH
        .DW VLIS2
        .DW CR
        .DW ZERO
        .DW FOUT
        .DW EX
VLIS2:  .DW DUP
        .DW ID
        .DW SPC
        .DW SPC
        .DW PFA
        .DW LFA
        .DW AT
        .DW DUP
        .DW ZEQAL
        .DW QTERM
        .DW FFOR
        .DW ZBRCH
        .DW VLIS1
        .DW DROP
        .DW SEMIS
        ;
        ;
        .DB H'87,"MESSAG",H'C5 ; MESSAGE
        .DW VLIST - 8
MSG:    .DW NEST
        .DW WRNG
        .DW AT
        .DW ZBRCH
        .DW MESS1
        .DW MDUP
        .DW ZBRCH
        .DW MESS2
        .DW LIT
        .DW H'0004
        .DW OFST
        .DW AT
        .DW BSCR
        .DW SLASH
        .DW MINS
        .DW DLINE
        .DW SPC
MESS2:  .DW BRCH
        .DW MESS3
MESS1:  .DW PDQ
        .DB H'07," MSG # "
        .DW DOT
MESS3:  .DW SEMIS
        ;
        ;
        .DW H'81C9          ; I
        .DW MSG - 10
I:      .DW * + 2
        INC R2
        INC R9
        INC R9
        INC R9
        LDA R2
        STR R9
        DEC R9
        LDN R2
        STR R9
        DEC R2
        DEC R2
        SEP RC
        ;
        .DB H'84,"WAR",H'CD ; WARM
        .DW I - 4
WRM:    .DW * + 2
        LBR WARM
        ;
        ;
        .DB H'84,"COL",H'C4 ; COLD
        .DW WRM - 7
CLD:    .DW * + 2
        LBR COLD
        ;
        ;
        .DB H'84,"S->",H'C4 ; S->D
        .DW CLD - 7
STOD:   .DW * + 2
        LDA R9
        SHL
        BDF SNEG
        LDI H'00
        BR SSKP
SNEG:   LDI H'FF
SSKP:   INC R9
        STR R9
        INC R9
        STR R9
        DEC R9
        SEP RC
        ;
        .DB H'86,"(LINE",H'A9 ; (LINE)
        .DW STOD - 7
PLINE:  .DW NEST
        .DW GR
        .DW LIT
        .DW H'0040
        .DW BBUF
        .DW SSMOD
        .DW RG
        .DW BSCR
        .DW STAR
        .DW PLUS
        .DW BLOCK
        .DW PLUS
        .DW LIT
        .DW H'0040
        .DW SEMIS
        ;
        .DB H'85,".LIN",H'C5 ; .LINE
        .DW PLINE - 9
DLINE:  .DW NEST
        .DW PLINE
        .DW TRLG
        .DW TYPE
        .DW SEMIS
        ;
        .DB H'83,"US",H'C5  ; USE (ADDR OF
        .DW DLINE - 8       ; NEXT BUFFER TO USE)
USE:    .DW VAR
        .DW FIRSTB
        ;
        .DB H'84,"PRE",H'D6 ; PREV   (ADDR OF
        .DW USE - 6         ; PREVIOUSLY USED BUFFER)
PREV:   .DW VAR
        .DW FIRSTB
        ;
        .DB H'84,"+BU",H'C6 ; +BUF
        .DW PREV - 7        ; ADVANCE
PBUF:   .DW NEST            ;    BUFFER
        .DW BBUF
        .DW LIT
        .DW H'0004
        .DW PLUS
        .DW PLUS
        .DW DUP
        .DW LIMIT
        .DW EQL
        .DW ZBRCH
        .DW PBUF1
        .DW DROP
        .DW FIRST
PBUF1:  .DW DUP
        .DW PREV
        .DW AT
        .DW MINS
        .DW SEMIS
        ;
        .DB H'86,"UPDAT",H'C5 ; UPDATE
        .DW PBUF - 7
UPDAT:  .DW NEST
        .DW PREV
        .DW AT
        .DW AT
        .DW LIT
        .DW H'8000
        .DW FFOR
        .DW PREV
        .DW AT
        .DW EX
        .DW SEMIS
        ;
        .DB H'8D,"EMPTY-BUFFER",H'D3 ; EMPTY-BUFFER
        .DW UPDAT - 9
MTBUF:  .DW NEST
        .DW FIRST
        .DW LIMIT
        .DW OVER
        .DW MINS
        .DW ERS
        .DW SEMIS
        ;
        ;
        .DB H'86,"BUFFE",H'D2 ; BUFFER
        .DW MTBUF - 16
BUFFE:  .DW NEST
        .DW USE
        .DW AT
        .DW DUP
        .DW GR
BUFF1:  .DW PBUF
        .DW ZBRCH
        .DW BUFF1
        .DW USE
        .DW EX
        .DW R
        .DW AT
        .DW ZLESS
        .DW ZBRCH
        .DW BUFF2
        .DW R
        .DW PLUS2
        .DW R
        .DW AT
        .DW LIT
        .DW H'7FFF
        .DW FAND
        .DW ZERO
        .DW RSLW
BUFF2:  .DW R
        .DW EX
        .DW R
        .DW PREV
        .DW EX
        .DW RG
        .DW PLUS2
        .DW SEMIS
        ;
        .DB H'85,"BLOC",H'CB ; BLOCK
        .DW BUFFE - 9
BLOCK:  .DW NEST
        .DW OFST
        .DW AT
        .DW PLUS
        .DW GR
        .DW PREV
        .DW AT
        .DW DUP
        .DW AT
        .DW R
        .DW MINS
        .DW DUP
        .DW PLUS
        .DW ZBRCH
        .DW BLOC1
BLOC2:  .DW PBUF
        .DW ZEQAL
        .DW ZBRCH
        .DW BLOC3
        .DW DROP
        .DW R
        .DW BUFFE
        .DW DUP
        .DW R
        .DW ONE
        .DW RSLW
        .DW TWO
        .DW MINS
BLOC3:  .DW DUP
        .DW AT
        .DW R
        .DW MINS
        .DW DUP
        .DW PLUS
        .DW ZEQAL
        .DW ZBRCH
        .DW BLOC2
        .DW DUP
        .DW PREV
        .DW EX
BLOC1:  .DW RG
        .DW DROP
        .DW PLUS2
        .DW SEMIS
        ;
        .DB H'83,H'52,H'2F,H'D7 ; R/W
        .DW BLOCK - 8
RSLW:   .DW NEST
        .DW SWAP
        .DW LIT
        .DW H'00FA
        .DW SLMOD
        .DW DUP
        .DW LIT
        .DW H'0003
        .DW GTR
        .DW LIT
        .DW H'0005
        .DW QERR
        .DW SWAP
        .DW LIT
        .DW H'0008
        .DW STAR
        .DW LIT
        .DW H'0001
        .DW PLUS
        .DW LIT
        .DW H'001A
        .DW SLMOD
        .DW DV
        .DW CEX
        .DW ONE
        .DW MINS
        .DW SWAP
        .DW LIT
        .DW H'0040
        .DW STAR
        .DW PLUS
        .DW DV
        .DW PLUS1
        .DW CEX
        .DW ZERO
        .DW DV
        .DW PLUS2
        .DW CEX
        .DW DV
        .DW BBUF
        .DW ROT
        .DW ZBRCH
        .DW RWELSE
        .DW BLKRD
        .DW BRCH
        .DW RWEND
RWELSE: .DW BLKWT
RWEND:  .DW SEMIS
        .DB H'0A,"BLOCK-REA",H'C4 ; BLOCKREAD
        .DW RSLW - 6
BLKRD:  .DW * + 2
        LDI H'83
        PHI R4
        PHI R5
        LDI H'64
        PLO R4
        LDI H'74
        PLO R5
        ;
        SEX R2
        GHI RC
        STXD
        GLO RC
        STXD
        LDA R9
        PHI R7
        LDN R9
        PLO R7
        DEC R9
        DEC R9
        LDN R9
        PLO RC
        DEC R9
        LDN R9
        PHI RC
        INC RC
        INC RC
        DEC R9
        LDN R9
        PLO R8
        DEC R9
        LDN R9
        PHI R8
        DEC R9
        DEC R9
BLKRD2: SEP R4
        .DW H'8502
        GHI RF
        STR R8
        INC R8
        DEC R7
        GHI R7
        LBNZ BLKRD2
        GLO R7
        LBNZ BLKRD2
        ;
        SEX R2
        IRX
        LDXA
        PLO RC
        LDX
        PHI RC
        SEP RC
        ;
        .DB H'0B,"BLOCK-WRIT",H'C5 ; BLOCK-WRITE
        .DW BLKRD - 13
BLKWT:  .DW * + 2
        LDI H'83
        PHI R4
        PHI R5
        LDI H'64
        PLO R4
        LDI H'74
        PLO R5
        ;
        SEX R2
        GHI RC
        STXD
        GLO RC
        STXD
        LDA R9
        PHI R7
        LDN R9
        PLO R7
        DEC R9
        DEC R9
        LDN R9
        PLO RC
        DEC R9
        LDN R9
        PHI RC
        INC RC
        INC RC
        DEC R9
        LDN R9
        PLO R8
        DEC R9
        LDN R9
        PHI R8
        DEC R9
        DEC R9
BLKWT2: LDA R8
        PHI RF
        SEP R4
        .DW H'8500
        DEC R7
        GHI R7
        LBNZ BLKWT2
        GLO R7
        LBNZ BLKWT2
        ;
        SEX R2
        IRX
        LDXA
        PLO RC
        LDX
        PHI RC
        SEP RC
        ;
        .DB H'84,"LOA",H'C4 ; LOAD
        .DW BLKWT - 14
LOAD:   .DW NEST
        .DW BLK
        .DW AT
        .DW GR
        .DW FIN
        .DW AT
        .DW GR
        .DW ZERO
        .DW FIN
        .DW EX
        .DW BSCR
        .DW STAR
        .DW BLK
        .DW EX
        .DW INPT
        .DW RG
        .DW FIN
        .DW EX
        .DW RG
        .DW BLK
        .DW EX
        .DW SEMIS
        ;
        .DB H'C3,"--",H'BE ; -->
        .DW LOAD - 7
ARROW:  .DW NEST      
        .DW QLDG
        .DW ZERO
        .DW FIN
        .DW EX
        .DW BSCR
        .DW BLK
        .DW AT
        .DW OVER
        .DW MODD
        .DW MINS
        .DW BLK
        .DW PLUSS
        .DW SEMIS
        ;
        ;
        .DB H'83,H'44,H'52,H'B0 ; DR0
        .DW ARROW - 6
DRZER:  .DW NEST
        .DW ZERO
        .DW OFST
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'83,H'44,H'52,H'B1 ; DR1
        .DW DRZER - 6
DRONE:  .DW NEST
        .DW BSCR
        .DW LIT             ; 250 SCREENS/DISK
        .DW H'00FA
        .DW STAR
        .DW OFST
        .DW EX
        .DW SEMIS
        ;
        ;
        .DB H'84,"LIS",H'D4 ; LIST
        .DW DRONE - 6
LIST:   .DW NEST
        .DW MDCML
        .DW CR
        .DW DUP
        .DW FSCR
        .DW EX
        .DW PDQ
        .DB H'06,"SCR # "
        .DW DOT
        .DW LIT
        .DW H'0010
        .DW ZERO
        .DW PDO
LIST1:  .DW CR
        .DW I
        .DW LIT
        .DW H'0003
        .DW DOTR
        .DW SPC
        .DW I
        .DW FSCR
        .DW AT
        .DW DLINE
        .DW QTERM
        .DW ZBRCH
        .DW LIST2
        .DW LVE
LIST2:  .DW LUPE
        .DW LIST1
        .DW CR
        .DW SEMIS
        ;
        ;
        .DB H'85,"INDE",H'D8 ; INDEX
        .DW LIST - 7
INDEX:  .DW NEST
        .DW CR
        .DW PLUS1
        .DW SWAP
        .DW PDO
INDE1:  .DW CR      
        .DW I
        .DW LIT
        .DW H'0003
        .DW DOTR
        .DW SPC
        .DW ZERO
        .DW I
        .DW DLINE
        .DW QTERM
        .DW ZBRCH
        .DW INDE2
        .DW LVE
INDE2:  .DW LUPE
        .DW INDE1
        .DW SEMIS
        ;
        .DB H'85,"TRIA",H'C4 ; TRIAD
        .DW INDEX - 8
TRIAD:  .DW NEST
        .DW CR
        .DW LIT
        .DW H'0003
        .DW SLASH
        .DW LIT
        .DW H'0003
        .DW STAR
        .DW LIT
        .DW H'0003
        .DW OVER
        .DW PLUS
        .DW SWAP
        .DW PDO
TRIA1:  .DW CR
        .DW I
        .DW LIST
        .DW QTERM
        .DW ZBRCH
        .DW TRIA2
        .DW LVE
TRIA2:  .DW LUPE
        .DW TRIA1
        .DW CR
        .DW LIT
        .DW H'000F
        .DW MSG
        .DW CR
        .DW SEMIS
        ;
        ;
        .DB H'85,"FLUS",H'C8 ; FLUSH
        .DW TRIAD - 8
FLUSH:  .DW NEST
        .DW LIMIT
        .DW FIRST
        .DW MINS
        .DW BBUF
        .DW LIT
        .DW H'0004
        .DW PLUS
        .DW SLASH
        .DW ZERO
        .DW PDO
FL1:    .DW LIT
        .DW H'7FFF
        .DW BUFFE
        .DW DROP
        .DW LUPE
        .DW FL1
        .DW SEMIS
        ;
        ;
        .DB H'84,"TAS",H'CB ; TASK
        .DW FLUSH - 8
TASK:   .DW NEST              
        .DW SEMIS
        ;
        ;
        .PAGE
        ;
        ;        
COLD:   LDI (START + 12) >> 8
        PHI R7
        LDI (START + 12)
        PLO R7
        LDI (FRTH + 14) >> 8
        PHI R8
        LDI ((FRTH + 14) & H'00FF)
        PLO R8
        LDA R7
        STR R8
        INC R8
        LDA R7
        STR R8
        LDI H'16
        BR PUTF
WARM:   LDI H'10
PUTF:   PLO RF
        LDI (START + H'10) >> 8
        PHI R7
        LDI (START + H'10)
        PLO R7
        LDA R7
        PHI RD
        PHI R8
        LDN R7
        PLO RD
        PLO R8
        LDI (START + 12)
        PLO R7
WRMLP:  LDA R7
        STR R8
        INC R8
        DEC RF
        GLO RF
        BNZ WRMLP
        LDI NEXT >> 8
        PHI RC
        LDI NEXT
        PLO RC
        LDI ((ABORT + 2) & H'00FF)
        PLO RA
        LDI (ABORT + 2) >> 8
        PHI RA
        LBR (RP1 + 2)
        ;
LEND:   NOP                 ; INITIAL FENCE IS HERE
        ;
        ; TO EXTEND THIS PORTION TO INCLUDE
        ; NEW WORDS, FIRST USE
        ;    HERE.  TO FIND END OF YOUR NEW VERSION
        ; THEN USE THE FOLLOWING:
        ;
        ;    LATEST 12 +ORIGIN !
        ;    HERE  28 +ORIGIN !
        ;    HERE  30 +ORIGIN !
        ;    HERE FENCE !
        ;
        ; THEN USE   BYE TO GET BACK TO YOUR MONITOR
        ; THEN USE YOUR MONITOR ROUTINES TO SAVE MEMORY
        ; FROM 0000 TO END ADDRESS
        ;
        ; THIS PROCEDURE WILL ALLOW YOU TO CONTINUE
        ; BUILDING ON YOUR FORTH VOCABULARY WITHOUT THE
        ; DISC INTERFACE
        ;
        ;
        ;
        ;
        ;
        .END
        