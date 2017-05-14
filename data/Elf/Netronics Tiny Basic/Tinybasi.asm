; Netronic TinyBasic For The ELF II Recreation
; By Richard Peters, Richard11092000@cox.net
; Special THANKS to TOM PITTMAN for Writing Program,
; To LEE A. HART for Posting Necessary Parts
; And To Dave Ruske for creating COMACELF group
; Which Made This Recreation Possible
; Assembled With QELFEXE V2.0 Multiformat Assembler
; Current Code Running In Tinybasi.zip Emulator
; Requires Giant Board And ROM Monitor
; To Use LOAD And SAVE to Tape
; Designed to run In RAM Starting at 0000
; I Have Done What I Could To Make This Source
; Moveable And To Follow Itself, But I Still Could
; Could Have Missed Something. I Also Tried To Figure
; Out Some Of The IL Code and Add Comments To It.
; If There Is Something To Add Or Change, Let Me Know.
; Code Has Only Been Verified With What I Have
; Hope The Above Changes Soon.
; Last Update 01/29/2004 09:40PM
;
; INTERNAL MACRO DEFINITIONS
;
; CALL   = SEP R4 + DW   SUB LOCATION
; RETURN = SEP R5
; SEP R7 = SEP R7 + DB   LOW LOCATION OF BYTE
;
R0         EQU     0         REGISTER DEFINITION
R1         EQU     1         REGISTER DEFINITION
R2         EQU     2         REGISTER DEFINITION
R3         EQU     3         REGISTER DEFINITION
R4         EQU     4         REGISTER DEFINITION
R5         EQU     5         REGISTER DEFINITION
R6         EQU     6         REGISTER DEFINITION
R7         EQU     7         REGISTER DEFINITION
R8         EQU     8         REGISTER DEFINITION
R9         EQU     9         REGISTER DEFINITION
RA         EQU     10        REGISTER DEFINITION
RB         EQU     11        REGISTER DEFINITION
RC         EQU     12        REGISTER DEFINITION
RD         EQU     13        REGISTER DEFINITION
RE         EQU     14        REGISTER DEFINITION
RF         EQU     15        REGISTER DEFINITION
;
; The Following Register And EQU Assignments Are Not Used
; In Every Part Of Program
;
; REGISTER ASSIGNMENTS:
;
;      0 ; PC (VIA RESET) AT ENTRY
;      1 ; INTERRUPT PROGRAM COUNTER
;      2 ; STACK POINTER
;      3 ; NORMAL PROGRAM COUNTER
;      4 ; BASIC: SCRT "CALL" PC
;      5 ; BASIC: SCRT "RETURN" PC
;      6 ; BASIC: SCRT RETURN ADDR.
;      7 ; BASIC: PC FOR "FECH"
XX         EQU     8         BASIC: WORK REGISTER
PC         EQU     9         IL PROGRAM COUNTER
AC         EQU     10        BASIC: 16-BIT ACCUMULATOR
BP         EQU     11        BASIC POINTER
;     12                     SERIAL AND TAPE ROUTINES
PZ         EQU     13        BASE: PAGE 0 POINTER
;     14 ;      RE.0=BAUD RATE CONSTANT
;            IF RE.0=0 USES 1861 AND KEYBOARD P7,EF3
;               RE.1=  USED FOR INPUT,OUTPUT
X          EQU     15        BASIC: SCRATCH REGISTER
;
;LDI0  ASSUMES THAT BASE PAGE IS ZERO
LDI0       EQU     9DH       GHI RD - CLEAR ACCUM. MACRO
FECH       EQU     0D7H      SEP R7 - PAGE 0 MACRO
;
; DISPLAY BUFFER EQU
;
BUFF       EQU     0xDB0      ONLY CHANGE PAGE, UNLESS YOU
BUFE       EQU     BUFF+344   WONT TO CHANGE INTERUPT ROUTINE
BUFX       EQU     BUFE+56    ALSO LIMITED TO 1DB0 BY PLOT
;
MONITOR    EQU     0xF000     Monitor address
;Putting C8 in first byte allows Monitor To Run instead of Tiny
PAGE       LBR     COLDV
           LBR     MONITOR
           SEP     R0
           IDL
;
;  DATA AREA, COULD BE EQUATES
;
TVXY       DB      0x0F       DISPLAY CURSOR LOCATION
           DB      0x00
           DB      0x00       BIT LOCATION OF CURSOR
           DB      0x00
MASK       DB      0xE0
TIME       DB      0x9A
           DB      0x27
           DB      0x3A
           DB      0x00
           DB      0x00
           DB      0x00
BS         DB      0x08
CAN        DB      0x1B
PAD        DB      0x00
TAPEMODE   DB      0x00
SPARE      DB      0x19
XEQ        DB      0x19
LEND       DB      0x34
AEPTR      DB      0x80
TTYCC      DB      0x00
NXA        DW      0x0773
AIL        DW      0x0766
BASIC      DW      0x0F40     LOWEST ADD. FOR PROGRAM
STACK      DW      0x3FF7     HIGHEST ADD. FOR PROGRAM
MEND       DW      0x109B     PROGRAM END + STACK RESERVE
TOPS       DW      0x3FF7     TOP OF GOSUB STACK
LINO       DW      0x00AA     CURRENT BASIC LINE NUMBER
WORK       DW      0x1083
           DW      0x0034
SP         DW      0x0033
LINE       DW      0x0000     INPUT LINE BUFFER
;
           ORG     PAGE+#80   #00
AESTK      DW      0x0000     RANDOM NUMBER GEN.
           DW      0x0000     VAR. A
           DW      0x0000     VAR. B
           DW      0x0000     VAR. C
           DW      0x0000     VAR. D
           DW      0x0000     VAR. E
           DW      0x0000     VAR. F
           DW      0x0000     VAR. G
           DW      0x0000     VAR. H
           DW      0x0000     VAR. I
           DW      0x0000     VAR. J
           DW      0x0000     VAR. K
           DW      0x0000     VAR. L
           DW      0x0000     VAR. M
           DW      0x0000     VAR. N
           DW      0x0000     VAR. O
           DW      0x0000     VAR. P
           DW      0x0000     VAR. Q
           DW      0x0000     VAR. R
           DW      0x0000     VAR. S
           DW      0x0000     VAR. T
           DW      0x0000     VAR. U
           DW      0x0000     VAR. V
           DW      0x0000     VAR. W
           DW      0x0000     VAR. X
           DW      0x0000     VAR. Y
           DW      0x0000     VAR. Z
Z165       PLO     R7         I/O ROUTINES
           LBDF    PEND       GOTO WARM START
           GHI     RD
Z149       B3      Z148       CHECK FOR KEYBOARD OR SERIAL
           B4      Z149          INPUT
Z150       B3      Z148
           BN4     Z150       FINED TIMING OF SERIAL INPUT
           SEQ
Z153       PLO     RE
           LDI     8
Z151       SMI     1
           BNZ     Z151
           GLO     RE
           ADI     2
           BNQ     Z152
           B4      Z153
           REQ
Z152       BN4     Z153       MUST GOTO #C4
           NOP                      |
           NOP                      |
           SMI     1                |
           BN4     Z154             |
           BNZ     Z152+1     BECAUSE OF THIS
           INC     RE
Z154       GLO     RE
           SMI     6
Z148       PHI     RE
           LDI     0x0C
           CALL    OUTPUTR    OUTPUT 0C  CLEARSCREEN
           LBR     CLEAR
BRKTST     ADI     0          BREAK TEST
           GHI     RE
           BNZ     Z156
           BN3     Z157
           LSKP
Z156       B4      Z157
           SMI     0
Z296       GHI     RE
           ANI     0xFE
Z158       PLO     RE
           LSZ
           DEC     RE
           GLO     RE
           BNZ     Z158
Z157       RETURN
COLDV      NOP                COLD START
           BR      COLD
           LBR     WARM       WARM START ENTRY
KEYV       LBR     INPUTR     BRANCH TO CHARATER INPUT
TYPEV      LBR     OUTPUTR    BRANCH TO CHARATER OUPUT
BREAKV     LBR     BRKTST     BRANCH TO BREAK TEST
; DEFAULTS LOADED TO DIRECT PAGE
           DB      0x08       BACKSPACE CODE
           DB      0x1B       LINE CANCEL CODE
           DB      0x00       PAD CHARATER
           DB      0x00       TAPE MODE ENABLE FLAG 80=ENABLED
           DB      0x19       SPARE STACK SIZE
ILPEEK     BR      PEEK       BRANCH TO PEEK
           DB      0x00
           DB      0x00
ILPOKE     DB      0x58       POKE
           DB      0xD5
           DW      :STRT      ADDRESS OF IL PROGRAM START
CONST      DW      0x0F40     DEFAULT START OF PROGRAM SPACE
           DB      0x7F        END MEM STOP
           DB      0x00
; END DEFAULTS
           LDA     R8         DOUBLE PEEK ENTRY
           SKP
PEEK       GHI     RD         PEEK ENTRY
           PHI     RA
           LDA     R8
           RETURN
ILINPOUT   LBR     IO
           SEP     R3
CALL       PHI     RF         CALL ROUTINE
           SEX     R2
           GLO     R6
           STXD
           GHI     R6
           STXD
           GLO     R3
           PLO     R6
           GHI     R3
           PHI     R6
           LDA     R6
           PHI     R3
           LDA     R6
           PLO     R3
           GHI     RF
           BR      CALL-1
           SEP     R3
RETURN     PHI     RF         RETURN ROUTINE
           SEX     R2
           GHI     R6
           PHI     R3
           GLO     R6
           PLO     R3
           INC     R2
           LDA     R2
           PHI     R6
           LDN     R2
           PLO     R6
           GHI     RF
           BR      RETURN-1
           SEP     R3
FETCH      LDA     R3         LOAD TEMP IMMEDIATE ROUTINE
           PLO     RD
           LDI     HIGH PAGE    MEMORY BASE PAGE
           PHI     RD
           LDA     RD
           SEX     RD         AND SET X TO D AND +
           BR      FETCH-1
TABLE      DW      BACK
           DW      HOP
           DW      MATCH
           DW      TSTV
           DW      TSTN
           DW      TEND
           DW      RTN
           DW      HOOK
           DW      WARM
           DW      XINIT
           DW      CLEAR
           DW      INSRT
           DW      RETN
           DW      RETN
           DW      GETLN
           DW      RETN
           DW      RETN
           DW      STRNG
           DW      CRLF
           DW      TAB
           DW      PRS
           DW      PRN
           DW      LIST
           DW      RETN
           DW      NXT
           DW      CMPR
           DW      IDIV
           DW      IMUL
           DW      ISUB
           DW      IADD
           DW      INEG
           DW      XFER
           DW      RSTR
           DW      SAV
           DW      STORE
           DW      IND
           DW      RSBP
           DW      SVBP
           DW      RETN
           DW      RETN
           DW      BPOP
           DW      APOP
           DW      DUPS
           DW      LITN
           DW      LIT1
           DW      RETN
TBEND      EQU     $         OPCODES BACKWARDS FROM HERE
; COLD & WARM START INITIALIZATION ;
;
; COLD START;
;
COLD       LDI     LOW $+3   CHANGE PROGRAM COUNTER
           PLO     R3        FROM R0 TO R3
           LDI     HIGH $
           PHI     R3
           SEP     R3
; DETERMINE SIZE OF USER RAM
           PHI     AC        GET LOW END ADDR.
           LDI     LOW CONST OF USER PROGRAM
           PLO     AC        RAM (AT "CONST")
           LDA     AC
           PHI     R2        ..AND PUT IN R2
           LDA     AC
           PLO     R2
           LDA     AC        SET PZ TO WRAP POINT
           PHI     PZ        (END OF SEARCH)
           LDI     0xFF
           PLO     PZ
           LDN     PZ        ..AND SAVE BYTE
           PHI     X         NOW AT ADDR. PZ
SCAN       SEX     R2        REPEAT TO SEARCH RAM..
           INC     R2        - GET NEXT BYTE
           LDX
           PLO     X         - SAVE A COPY
           XRI     0xFF      - COMPLEMENT IT
           STR     R2        - STORE IT
           XOR               - SEE IF IT WORKED
           SEX     PZ
           LSNZ              - IF MATCHES, IS RAM
           GHI     X         SET CARRY IF AT
           XOR               WRAP POINT..
           ADI     0xFF      - ELSE IS NOT RAM
           GLO     X         RESTORE ORIGINAL BYTE
           STR     R2
           BNF     SCAN      - ..UNTIL END OR WRAP POINT
           DEC     R2
           LDN     AC        RAM SIZED: SET
           PHI     PZ        POINTER PZ TO
           LDI     STACK+1   WORK AREA
           PLO     PZ
           GLO     R2        STORE RAM END ADDRESS
           STXD
           GHI     R2
           STXD              GET & STORE RAM BEGINNIG
           DEC     AC        REPEAT TO COPY PARAMETERS..
           DEC     AC        - POINT TO NEXT
           LDN     AC        - GET PARAMETER
           STXD              - STORE IN WORK AREA
           GLO     PZ
           XRI     BS-1      - TEST FOR LAST PARAMETER
           BNZ     $-6       - ..UNTIL LAST COPIED
           SHR               SET DF=0 FOR "CLEAR"
           LSKP
;
; WARM START:
;
WARM       SMI     0         SET DF=1 FOR "DON'T CLEAR"
           LDI     $+3
           PLO     R3        BE SURE PROGRAM COUNTER IS R3
           LDI     HIGH $
           PHI     R3
           SEP     R3
           PHI     R4        INITIALIZE R4, R5, R7
           PHI     R5        ASSUMES CALL,RETURN,FETCH
           PHI     R7        IS IN SAME PAGE AS WARM
           LDI     CALL
           PLO     R4
           LDI     RETURN
           PLO     R5
           LDI     FETCH
           LBR     Z165       GOTO #00B6
CLEAR      DB      FECH,BASIC - MARK PROGRAM EMPTY
           PHI     BP
           LDA     PZ
           PLO     BP
           DB      LDI0      WITH LINE# = 0
           STR     BP
           INC     BP
           STR     BP
           DB      FECH,SPARE-1 SET MEND = START + SPARE
           GLO     BP        GET START
           ADD               ADD LOW BYTE OF SPARE
           PHI     X         SAVE TEMPORARILY
           DB      FECH,MEND GET MEND
           GHI     X
           STXD              STORE LOW BYTE OF MEND
           GHI     BP
           ADCI    0         ADD CARRY
           STXD              STORE HIGH BYTE OF MEND
PEND       DB      FECH,STACK SET STACK TO END OF MEMORY
           PHI     R2
           LDA     PZ
           PLO     R2
           DB      FECH,TOPS
           GLO     R2        SET TOPS TO EMPTY
           STXD              (I.E. STACK END)
           GHI     R2
           STXD
           CALL    FORCE     SET TAPE MODE "OFF"
IIL        DB      FECH,AIL  SET IL PC
           PHI     PC
           LDA     PZ
           PLO     PC        CONTINUE INTO "NEXT"
;
; EXECUTE NEXT INTERMEDIATE LANGUAGE (IL) INSTRUCTION
;
NEXT       SEX     R2        GET OPCODE
           LDA     PC
           SMI     0x30      IF JUMP OR BRANCH,
           BDF     TBR       GO HANDLE IT
           SDI     0xD7      IF STACK BYTE EXCHANGE,
           BDF     XCHG      GO HANDLE IT
           SHL               ELSE MULTIPLY BY 2
           ADI     TBEND     TO POINT INTO TABLE
           PLO     R6
           LDI     LOW NEXT  & SET RETURN TO HERE
           DEC     R2        (DUMMY STACK ENTRY)
           DEC     R2
           STXD
           GHI     R3
           STXD
DOIT       GHI     R7        TABLE PAGE
           PHI     R6
           LDA     R6        FETCH SERVICE ADDRESS
           STR     R2
           LDA     R6
           PLO     R6
           LDX
           PHI     R6
           SEP     R5        GO DO IT
;
TBR        SMI     0x10      IF JUMP OR CALL,
           BNF     TJMP      GO DO IT
           PLO     R6        ELSE BRANCH; SAVE OPCODE
           ANI     0x1F      COMPUTE DESTINATION
           BZ      TBERR     IF BRANCH ADDR = 0, GOTO ERROR
           STR     R2        PUSH ADDRESS ONTO STACK
           GLO     PC        ADD RELATIVE OFFSET
           ADD               LOW BYTE
           STXD
           GHI     PC        HIGH BYTE W. CARRY
           ADCI    0
           SKP
TBERR      STXD              STORE 0 FOR ERROR
           STXD
           GLO     R6        NOW COMPUTE SERVICE ADDRESS
           SHR               WHICH IS HIGH 3 BITS
           SHR
           SHR
           SHR
           ANI     0xFE
           ADI     LOW TABLE INDEX INTO TABLE
           PLO     R6
           BR      DOIT
;
TJMP       ADI     8         NOTE IF JUMP IN CARRY
           ANI     7         GET ADDRESS
           PHI     R6
           LDA     PC
           PLO     R6
           BDF     JMP       JUMP
           GLO     PC        PUSH PC
           STXD
           GHI     PC
           STXD
           CALL    STEST     CHECK STACK DEPTH
;
JMP        DB      FECH,AIL  ADD JUMP ADDRESS TO IL BASE
           GLO     R6
           ADD
           PLO     PC
           GHI     R6
           DEC     PZ
           ADC
           PHI     PC
           BR      NEXT
;
XCHG       SDI     7         SAVE OFFSET
           STR     R2
           DB      FECH,AEPTR
           PLO     PZ
           SEX     R2
           ADD
           PLO     R6        R6 IS OTHER POINTER
           GHI     PZ
           PHI     R6
           LDN     PZ        NOW SWAP THEM:
           STR     R2        SAVE OLD TOP
           LDN     R6        GET INNER BYTE
           STR     PZ        PUT ON TOP
           LDN     R2        GET OLD TOP
           STR     R6        PUT IN
           BR      NEXT
;
BACK       GLO     R6        REMOVE OFFSET
           SMI     0x20      FOR BACKWARDS HOP
           PLO     R6
           GHI     R6
           SMBI    0
           SKP
;
HOP        GHI     R6        FORWARD HOP
           LBZ     ERR       IF ZERO, GOTO ERROR
           PHI     PC        ELSE PUT INTO PC
           GLO     R6
           PLO     PC
           BR      NEXT
;
           INC     BP        ADVANCE TO NEXT NON-BLANK CHAR.
NONBL      LDN     BP        GET CHARACTER
           SMI     0x20      IF BLANK,
           BZ      NONBL-1   INCREMENT POINTER AND TRY AGAIN
           SMI     0x10      IF NUMERIC (0-9),
           LSNF
           SDI     9         SET DF=1
NONBX      LDN     BP        GET CHARACTER
           RETURN              AND RETURN
;
STORE      CALL    APOP      GET VARIABLE
           LDA     PZ        GET POINTER
           PLO     PZ
           GHI     AC        STORE THE NUMBER
           STR     PZ
           INC     PZ
           GLO     AC
           STR     PZ
           BR      BPOP      GO POP POINTER
;
           CALL    APOP      POP 4 BYTES
APOP       CALL    BPOP      POP 2 BYTES
           PHI     AC        FIRST BYTE TO AC.1
BPOP       DB      FECH,AEPTR POP 1 BYTE
           DEC     PZ
           ADI     1         INCREMENT
           STR     PZ
           PLO     PZ
           DEC     PZ
           LDA     PZ        LEAVE IT IN D
           PLO     AC        AND AC.0
RETN       RETURN
;
TEND       CALL    NONBL     GET NEXT CHARACTER
           XRI     0x0D      IF CARRIAGE RETURN,
           BZ      NEXT      THEN FALL THRU IN IL
           BR      HOP       ELSE TAKE BRANCH
;
TSTV       CALL    NONBL     GET NEXT CHARACTER
           SMI     0x41      IF LESS THAN 'A',
           BNF     HOP       THEN HOP
           SMI     0x1A      IF GREATER THAN 'Z'
           BDF     HOP       THEN HOP
           INC     BP        ELSE IS LETTER A-Z
           GHI     X         GET SAVED COPY
           SHL               CONVERT TO VARIABLE'S ADDRESS
           CALL    BPUSH     AND PUSH ONTO STACK
           BR      NEXT
;
TSTN       CALL    NONBL     GET NEXT CHARACTER
           BNF     HOP       IF NOT A DIGIT, HOP
           DB      LDI0      ELSE COMPUTE NUMBER
           PHI     AC        INITIALLY 0
           PLO     AC
           CALL    APUSH     PUSH ONTO STACK
NUMB       LDA     BP        GET CHARACTER
           ANI     0x0F      CONVERT FROM ASCII TO NUMBER
           PLO     AC
           DB      LDI0
           PHI     AC
           LDI     10        ADD 10 TIMES THE..
           PLO     X
           SEX     PZ
NM10       INC     PZ
           GLO     AC        ..PREVIOUS VALUE..
           ADD
           PLO     AC
           GHI     AC
           DEC     PZ        ..WHICH IS ON STACK.
           ADC
           PHI     AC
           DEC     X         COUNT THE ITERATIONS
           GLO     X
           BNZ     NM10
           GHI     AC        SAVE NEW VALUE
           STR     PZ
           INC     PZ
           GLO     AC
           STXD
           CALL    NONBL     IF ANY MORE DIGITS,
           LBDF    NUMB      THEN DO IT AGAIN
NHOP       LBR     NEXT      UNTIL DONE
;
MATCH      GHI     BP        SAVE PB IN CASE NO MATCH
           PHI     AC
           GLO     BP
           PLO     AC
MAL        CALL    NONBL     GET A BYTE (IN CAPS)
;
           INC     BP        COMPARE THEM
           STR     R2
           LDA     PC
           XOR
           BZ      MAL       STILL EQUAL
           XRI     80H       END?
           BZ      NHOP      YES
           GHI     AC        NO GOOD
           PHI     BP        PUT POINTER BACK
           GLO     AC
           PLO     BP
JHOP       LBR     HOP       THEN TAKE BRANCH
;
STEST      DB      FECH,MEND POINT TO PROGRAM END
           GLO     R2        COMPARE TO STACK TOP
           SD
           DEC     PZ
           GHI     R2
           SDB
           BDF     ERR       AHA; OVERFLOW
           RETURN              ELSE EXIT
;
LIT1       LDA     PC        ONE BYTE
           BR      BPUSH
LITN       LDA     PC        TWO BYTES
           PHI     AC        FIRST IS HIGH BYTE,
           LDA     PC        THEN LOW BYTE
           BR      APUSH+1   PUSH RESULT ONTO STACK
;
HOOK       CALL    HOOP      GO DO IT, LEAVE EXIT HERE
           BR      APUSH+1   PUSH RESULT ONTO STACK
;
DUPS       CALL    APOP      POP 2 BYTES INTO AC
           CALL    APUSH     THEN PUSH TWICE
APUSH      GLO     AC        PUSH 2 BYTES
           CALL    BPUSH
           GHI     AC
BPUSH      STR     R2        PUSH ONE BYTE (IN D)
           DB      FECH,LEND CHECK FOR OVERFLOW
           SM                COMPARE AEPTR TO LEND
           BDF     ERR       OOPS!
           LDI     1
           SD
           STR     PZ
           PLO     PZ
           LDN     R2        GET SAVED BYTE
           STR     PZ        STORE INTO STACK
SEP5       RETURN              & RETURN
;
IND        CALL    BPOP      GET POINTER
           PLO     PZ
           LDA     PZ        GET VARIABLE
           PHI     AC
           LDA     PZ
           BR      APUSH+1   GO PUSH IT
;
QUOTE      XRI     0x2F      TEST FOR QUOTE
           BZ      SEP5      IF QUOTE, GO EXIT
           XRI     0x22      ELSE RESTORE CHARACTER
           CALL    TYPER
PRS        LDA     BP        GET NEXT BYTE
           XRI     0x0D      IF NOT CARRIAGE RETURN,
           BNZ     QUOTE     THEN CONTINUE
           DEC     PC        ELSE CONTINUE INTO ERROR
;
ERR        DB      FECH,XEQ  ERROR:
           PHI     XX        SAVE XEQ FLAG
           CALL    FORCE     TURN TAPE MODE OFF
           LDI     "!"       PRINT "!" ON NEW LINE
           CALL    TYPER
           DB      FECH,AIL
           GLO     PC        CONVERT IL PC TO ERROR#
           SM                BY SUBTRACTING
           PLO     AC        IL START FROM PC
           GHI     PC
           DEC     PZ        X MUST POINT TO
           SMB               PAGE0 REGISTER PZ=RD
           PHI     AC
           CALL    PRNA      PRINT ERROR#
           GHI     XX        GET XEQ FLAG
           BZ      BELL      IF XEQ SET,
           LDI     LOW ATMSG - THEN TYPE "AT"
           PLO     PC
           GHI     R3
           PHI     PC
           CALL    STRNG
           DB      FECH,LINO - GET LINE NUMBER
           PHI     AC        - AND PRINT IT, TOO
           LDA     PZ
           PLO     AC
           CALL    PRNA
BELL       LDI     7         RING THE BELL
           CALL    TYPEV
           CALL    CRLF      PRINT <CR><LF>
FIN        DB      FECH,TTYCC-1
           DB      LDI0      TURN TAPE MODE OFF
           STR     PZ
EXIT       DB      FECH,TOPS RESET STACK POINTER
           PHI     R2
           LDA     PZ
           PLO     R2
           LBR     IIL       RESTART IL FROM BEGINNING
;
ATMSG      DB      ' ','A','T' ERROR MESSAGE TEMPLATE
           DB      ' ',0A3H
;
TSTR       CALL    TYPER-2   PRINT CHARACTER STRING
STRNG      LDA     PC        GET NEXT CHARACTER OF STRING
           ADI     0x80      IF HI BIT=0,
           BNF     TSTR      THEN GO PRINT & CONTINUE
           BR      TYPER-2   PRINT LAST CHAR AND EXIT
;
FORCE      DB      FECH,AEPTR-1
           LDI     AESTK     CLEAR A.E.STACK
           STXD
           DB      LDI0      SET "NOT EXECUTING"
           STXD              LEND=0 ZERO LINE LENGTH
           STXD              XEQ=0 NOT EXECUTING
           LSKP              CONTINUE TO CRLF
;
CRLF       DB      FECH,TTYCC GET COLUMN COUNT
           SHL               IF IN TAPE MODE (MSB=1),
           BDF     SEP5      THEN JUST EXIT
           DB      FECH,PAD  GET # OF PAD CHARS
           PLO     AC        & SAVE IT
           LDI     0x0D      TYPE <CR>
PADS       CALL    TYPEV
           DB      FECH,TTYCC-1 POINT PZ TO COLUMN COUNTER
           GLO     AC        GET # OF PADS TO GO
           SHL               MSB SELECTS NULL OR DELETE
           BZ      PLF       UNTIL NO MORE PADS..
           DEC     AC        DECREMENT # OF PADS TO GO
           DB      LDI0      PAD=NULL=0 IF MSB=0
           LSNF
           LDI     0xFF      PAD=DELETE=FFH IF MSB=1
           BR      PADS      ..REPEAT
;
PLF        STXD              SET COLUMN COUNTER TTYCC=0
           LDI     0x8A      TYPE <LF>
;
           SMI     0x80      FIX HI BIT
TYPER      PHI     X         SAVE CHAR
           DB      FECH,TTYCC CHECK OUTPUT MODE
           DEC     PZ
           ADI     0x81      INCREMENT COLUMN COUNTER TTYCC
           ADI     0x80      WITHOUT DISTURBING MSB
           BNF     SEP5      IF MSB=1, IN TAPE MODE, NOT PRINTIN
           STR     PZ        ELSE UPDATE COLUMN COUNTER
           GHI     X         GET CHAR
           LBR     TYPEV     AND GO TYPE IT
;
TAB        LDI     0x20
           CALL    TYPER
           DB      FECH,TTYCC GET COLUMN COUNT
TABS       ANI     7         LOW 3 BITS
           BNZ     TAB
           RETURN
           CALL    TYPER
           DEC     AC        DECREMENT SPACES TO GO
           BR      TABS      ...REPEAT
;
PRNA       CALL    APUSH     NUMBER IN AC
PRN        DB      FECH,AEPTR CHECK SIGN
           PLO     PZ
           CALL    DNEG      IF NEGATIVE,
           BNF     PRP
           LDI     '-'       PRINT '-'
           CALL    TYPER
PRP        DB      LDI0      PUSH ZERO FLAG
           STXD              WHICH MARKS NUMBER END
           PHI     AC        PUSH 10 (=DIVISOR)
           LDI     10
           CALL    APUSH+1
           INC     PZ
PDVL       CALL    PDIV      DIVIDE BY 10
           GLO     AC        REMAINDER IS NEXT DIGIT
           SHR               BUT DOUBLED; HALVE IT
           ORI     0x30      CONVERT TO ASCII
           STXD              PUSH IT
           INC     PZ        IS QUOTIENT=0?
           LDA     PZ
           SEX     PZ
           OR
           DEC     PZ        RESTORE POINTER
           DEC     PZ
           BNZ     PDVL      ..REPEAT
PRNL       INC     R2        NOW, TO PRINT IT
           LDN     R2        GET CHAR
           LBZ     APOP-3    UNTIL ZERO (END FLAG)..
           CALL    TYPER     PRINT IT
           BR      PRNL      ..REPEAT
;
RSBP       DB      FECH,SP   GET SP
           SKP
SVBP       GHI     BP        GET BP
           XRI     HIGH LINE IN THE LINE?
           BNZ     SWAP      NO, NOT IN SAME PAGE
           GLO     BP
           STR     R2
           LDX
           SMI     LOW AESTK
           BDF     SWAP      NO, BEYOND ITS END
           DB      FECH,SP
           GLO     BP        YES, JUST COPY BP TO SP
           STXD
           GHI     BP
           STR     PZ
TYX        RETURN
;
SWAP       DB      FECH,SP   EXCHANGE BP AND SP
           PHI     XX        PUT SP IN TEMP
           LDN     PZ
           PLO     XX
           GLO     BP        STORE BP IN SP
           STXD
           GHI     BP
           STR     PZ
           GHI     XX        STORE TEMP IN BP
           PHI     BP
           GLO     XX
           PLO     BP
           RETURN
;
CMPR       CALL    APOP      GET FIRST NUMBER
           GHI     AC        PUSH ONTO STACK WITH BIAS
           XRI     0x80      (FOR 2'S COMPLEMENT)
           STXD              (BACKWARDS)
           GLO     AC
           STXD
           CALL    BPOP      GET AND SAVE
           PLO     X         COMPARE BITS
           CALL    APOP      GET SECOND NUMBER
           INC     R2
           GLO     AC        COMARE THEM
           SM                LOW BYTE
           PLO     AC
           INC     R2
           GHI     AC        HIGH BYTE
           XRI     0x80      BIAS: 0 TO 65535 INSTEAD
           SMB               OF -32768 TO +32767
           STR     R2
           BNF     CLT       LESS IF NO CARRY OUT
           GLO     AC
           OR
           BZ      CEQ       EQUAL IF BOTH BYTES 0
           GLO     X         ELSE GREATER
           SHR               MOVE PROPER BIT
           SKP
CEQ        GLO     X         (BIT 1)
           SHR
           SKP
CLT        GLO     X         (BIT 0)
           SHR               TO CARRY
           LSNF
           NOP
SKIP       INC     PC        SKIP ONE BYTE IF TRUE
           RETURN
;
ISUB       CALL    INEG      SUBTRACT IS ADD NEGATIVE
IADD       CALL    APOP      PUT ADDEND IN AC
           SEX     PZ
           INC     PZ        ADD TO AUGEND
           GLO     AC
           ADD
           STXD
           GHI     AC        CARRY INTO HIGH BYTE
           ADC
           STR     PZ
           RETURN
;
IMUL       CALL    APOP      MULTIPLIER IN AC
           LDI     0x10      BIT COUNTER IN X
           PLO     X
           LDA     PZ        MULTIPLICAND IN XX
           PHI     XX
           LDN     PZ
           PLO     XX
MULL       LDN     PZ        SHIFT PRODUCT LEFT
           SHL               (ON STACK)
           STR     PZ
           DEC     PZ
           LDN     PZ
           SHLC              DISCARD HIGH 16 BITS
           STR     PZ
           CALL    SHAL      GET A BIT
           BNF     MULC      NOT THIS TIME
           SEX     PZ        IF MULTIPLIER BIT=1,
           INC     PZ
           GLO     XX        ADD MULTIPLICAND
           ADD
           STXD
           GHI     XX
           ADC
           STR     PZ
MULC       DEC     X         REPEAT 16 TIMES
           GLO     X
           INC     PZ
           BNZ     MULL
           RETURN
;
IDIV       CALL    APOP      GET DIVISOR
           GHI     AC
           STR     R2        CHECK FOR DIVIDE BY ZERO
           GLO     AC
           OR
           LBZ     ERR       IF YES, FORGET IT
           LDN     PZ        COMPARE SIGN OF DIVISOR
           XOR
           STXD              SAVE FOR LATER
           CALL    DNEG      MAKE DIVEDEND POSITIVE
           DEC     PZ        SAME FOR DIVISOR
           DEC     PZ
           CALL    DNEG
           INC     PZ
           DB      LDI0
           LSKP
PDIV       DB      LDI0      MARK "NO SIGN CHANGE"
           STXD              FOR PRN ENTRY
           PLO     AC        CLEAR HIGH END
           PHI     AC        OF DIVIDEND IN AC
           LDI     17        COUNTER TO X
           PLO     X
DIVL       SEX     PZ        DO TRIAL SUBTRACT
           GLO     AC
           SM
           STR     R2        HOLD LOW BYTE FOR NOW
           DEC     PZ
           GHI     AC
           SMB
           BNF     $+5       IF NEGATIVE, CANCEL  IT
           PHI     AC        IF POSITIVE, MAKE IT REAL
           LDN     R2
           PLO     AC
           INC     PZ        SHIFT EVERYTHING LEFT
           INC     PZ
           INC     PZ
           LDX
           SHLC
           STXD
           LDX
           SHLC
           STXD
           GLO     AC        HIGH 16
           SHLC
           CALL    SHCL
           DEC     X         DO IT 16 TIMES MORE
           GLO     X
           LBNZ    DIVL
           INC     R2        CHECK SIGN OF QUOTIENT
           LDN     R2
           SHL
           BNF     NEGX      POSITIVE IS DONE
INEG       DB      FECH,AEPTR POINT TO STACK
           PLO     PZ
           BR      NEG
DNEG       SEX     PZ
           LDX               FOR DIVIDE,
           SHL               TEST SIGN
           BNF     NEGX      IF POSITIVE, LEAVE IT ALONE
NEG        INC     PZ        IF NEGATIVE,
           DB      LDI0      SUBTRACT IT FROM 0
           SM
           STXD
           DB      LDI0
           SMB
           STR     PZ
           SMI     0         AND SET CARRY=1
NEGX       RETURN
;
SHAL       GLO     AC        USED BY MULTIPLY
           SHL
SHCL       PLO     AC        AND DIVIDE
           GHI     AC
           SHLC
           PHI     AC
           RETURN
;
NXT        DB      FECH,XEQ  IF DIRECT EXECUTION
           LBZ     FIN       QUIT WITH DF=0
           LDA     BP        ELSE SCAN TO NEXT <CR>
           XRI     0x0D
           BNZ     $-3
           CALL    GLINO     GET LINE NUMBER
           BZ      BERR      ZERO IS ERROR
CONT       CALL    BREAKV    TEST FOR BREAK
           BDF     BREAK     IF BREAK,
           DB      FECH,NXA  RECOVER RESTART POINT
           PHI     PC        WHICH WAS SAVED BY INIT
           LDA     PZ
           PLO     PC
RUN        DB      FECH,XEQ-1 TURN OFF RUN MODE
           STR     PZ        (NON-ZERO)
           RETURN
;
BREAK      DB      FECH,AIL  SET BREAK ADDR=0
           PHI     PC        I.E. PC=IL START
           LDA     PZ
           PLO     PC
BERR       LBR     ERR
;
XINIT      DB      FECH,BASIC POINT TO START OF BASIC PROGRAM
           PHI     BP
           LDA     PZ
           PLO     BP
           CALL    GLINO     GET LINE NUMBER
           BZ      BERR      IF 0, IS ERROR (NO PROGRAM)
           DB      FECH,NXA  SAVE STATEMENT
           GLO     PC        ANALYZER ADDRESS
           STXD
           GHI     PC
           STR     PZ
           BR      RUN       GO START UP
;
XFER       CALL    FIND      GET THE LINE
           BZ      CONT      IF WE GOT IT, GO CONTINUE
GOAL       DB      FECH,LINO ELSE FAILED
           GLO     AC        MARK DESTINATION
           STXD
           GHI     AC
           STR     PZ
           BR      BERR      GO HANDLE ERROR
;
RSTR       CALL    TTOP      CHECK FOR UNDERFLOW
           LDA     R2        GET THE NUMBER
           PHI     AC        FROM STACK INTO AC
           LDN     R2
           PLO     AC
           DB      FECH,TOPS
           GLO     R2        RESET TOPS FROM R2
           STXD
           GHI     R2
           STXD
           CALL    FIND+3    POINT TO THIS LINE
           BNZ     GOAL      NOT THERE ANY MORE
           BR      BNEXT     OK
;
RTN        CALL    TTOP      CHECK FOR UNDERFLOW
           LDA     R2        (2 ALREADY INCLUDED)
           PHI     PC        PIP ADDRESS TO PC
           LDN     R2
           PLO     PC
BNEXT      LBR     NEXT
;
TTOP       DB      FECH,STACK GET TOP OF STACK
           INC     R2
           INC     R2
           GLO     R2        MATCH TO STACK POINTER
           ADI     2         (ADJUSTED FOR RETURN)
           XOR
           DEC     PZ
           BNZ     TTOK      NOT EQUAL
           GHI     R2
           ADCI    0
           XOR
           BZ      BERR      MATCH IS EMPTY STACK
;
TTOK       INC     R2        (ONCE HERE SAVES TWICE)
           RETURN
;
TAPE       DB      FECH,PAD+1 TURN OFF TYPEOUT
           SKP
NTAPE      DB      LDI0      TURN ON TYPEOUT
           SHL               (FLAG TO CARRY)
           DB      FECH,TTYCC-1
           DB      LDI0
           SHRC              00 OR 80H
           STR     PZ
           BR      KLOOP
GETLN      LDI     LOW LINE  POINT TO LINE
           PLO     BP
           CALL    APUSH     MARK STACK LIMIT
           GHI     PZ
           PHI     BP
KLOOP      CALL    KEYV      GET AN ECHOED BYTE
           ANI     7FH       SET HIGH BIT TO 0
           BZ      KLOOP     IGNORE NULL
           STR     R2
           XRI     0x7F
           BZ      KLOOP     IGNORE DELETE
           XRI     0x75      IF <LF>,
           BZ      TAPE      THEN TURN TAPE MODE ON
           XRI     0x19      IF <XOFF> (DC3=13H),
           BZ      NTAPE     THEN TURN TAPE MODE OFF
           DB      FECH,CAN-1
           LDN     R2
           XOR               IF CANCEL,
           BZ      CANCL     THEN GO TO CANCEL
           DEC     PZ
           LDN     R2
           XOR
           BNZ     STOK      NO
           DEC     BP        YES
           GLO     BP
           SMI     LOW LINE  ANYTHING LEFT?
           BDF     KLOOP     YES
CANCL      LDI     LOW LINE  IF NO, CANCEL THIS LINE
           PLO     BP
           LDI     0x0D      BY FORCING A <CR>
           SKP
STOK       LDN     R2        STORE CHARACTER IN LINE
           STR     BP
           DB      FECH,AEPTR-1
           GLO     BP        CHECK FOR OVERFLOW
           SM
           BNF     CHIN      OK
           LDI     7         IF NOT, RING BELL
           CALL    TYPER
           LDN     BP        NOW LOOK AT CHAR
           SKP
CHIN       LDA     BP        INCREMENT POINTER
           XRI     0x0D      IF NOT <CR>,
           BNZ     KLOOP     THEN GET ANOTHER
           CALL    CRLF      ELSE ECHO <LF>
           DB      FECH,LEND-1 AND MARK END
           GLO     BP
           STR     PZ
           LDI     LOW LINE  RESET BP TO FRONT
           PLO     BP
           LBR     APOP      AND GO POP DUMMY
;
FIND       CALL    APOP      GET LINE NUMBER
           GLO     AC
           STR     R2        CHECK FOR ZERO
           GHI     AC
           OR
           LBZ     ERR       IF 0, GO TO ERROR
FINDX      DB      FECH,BASIC START AT FRONT
           PHI     BP
           LDA     PZ
           PLO     BP
FLINE      CALL    GLINO     GET LINE NUMBER
           LSNZ              NOT THER IF 0
           GLO     PZ        SET NON-ZERO,
FEND       RETURN              AND RETURN
           SEX     PZ
           GLO     AC        COMPARE THEM
           SD
           STR     R2        (SAVE LOW BYTE OF DIFFERENCE)
           GHI     AC
           DEC     PZ
           SDB
           SEX     R2
           OR                (D=0 IF EQUAL)
           BDF     FEND      LESS OR EQUAL IS END
           LDA     BP        NOT THERE YET
           XRI     0x0D      SCAN TO NEXT <CR>
           BNZ     $-3
           BR      FLINE
;
HOOP       CALL    HOOP+3    ADJUST STACK
           CALL    APOP      SET UP PARAMETERS:
           LDA     PZ        AC
           PHI     XX        MIDDLE ARGUMENT TO XX
           LDA     PZ
           PLO     XX
           LDA     PZ        SUBROUTINE ADDRESS BECOMES
           PHI     R6        "RETURN ADDRESS"
           LDA     PZ
           PLO     R6
           GLO     PZ        FIX STACK POINTER
           STR     R2
           DB      FECH,AEPTR-1
           LDN     R2        BY PUTTING CURRENT VALUE
           STR     PZ        VALUE BACK INTO IT
           PLO     PZ        LEAVE PZ AT STACK TOP
           GLO     AC        LEAVE AC.0 IN D
           RETURN              GO DO IT
;
LIST       DB      FECH,WORK+2
           GLO     BP        SAVE POINTERS
           STXD
           GHI     BP
           STR     PZ
           CALL    FIND      GET LIST LIMITS
           DB      FECH,WORK SAVE UPPER
           GLO     BP
           STXD
           GHI     BP
           STXD
           CALL    FIND      TWO ITEMS MARK BOUNDS
           DEC     BP        BACK UP OVER LINE#
           DEC     BP
LLINE      DB      FECH,WORK END?
           GLO     BP
           SM
           DEC     PZ
           GHI     BP
           SMB
           BDF     LIX       SO IF BP>BOUNDS,
           LDA     BP        GET LINE#
           PHI     AC
           LDA     BP
           PLO     AC
           BNZ     $+5
           GHI     AC
           BZ      LIX       QUIT IF ZERO (PROGRAM END)
           CALL    PRNA      ELSE PRINT LINE#
           LDI     0x2D      THEN A SPACE
LLOOP      XRI     0x0D      (RESTORE BITS FROM <CR> TEST)
           CALL    TYPER
           CALL    BREAKV    TEST FOR BREAK
           BDF     LIX       IF YES, THEN QUIT
           LDA     BP        NOW PRINT TEXT
           XRI     0x0D      UNTIL <CR>
           BNZ     LLOOP
           CALL    CRLF      END LINE WITH <CR><LF>
           BR      LLINE     ..REPEAT UNTIL DONE
;
LIX        DB      FECH,WORK+2 RESTORE BP
           PHI     BP
           LDA     PZ
           PLO     BP
           RETURN
;
SAV        DB      FECH,TOPS ADJUST STACK TOP
           GLO     R2
           STXD
           GHI     R2
           STR     PZ
           DB      FECH,XEQ  IF NOT EXECUTING
           DEC     PZ
           LSZ               USE ZERO INSTEAD
           DB      FECH,LINO
           PLO     AC        HOLD HIGH BYTE
           LDA     PZ        GET LOW BYTE
           INC     R2
           INC     R2
           SEX     R2
           STXD              PUSH ONTO STACK
           GLO     AC        NOW THE HIGH BYTE
           STXD
           LBR     NEXT
;
GLINO      DB      FECH,LINO-1 SETUP POINTER
           LDA     BP        GET 1ST BYTE
           STR     PZ        STORE IN RAM
           INC     PZ
           LDA     BP        2ND BYTE
           STXD
           OR                D=0 IF LINE#=0
           INC     PZ
           RETURN
;
INSRT      CALL    SWAP      SAVE POINTER IN NEW LINE
           CALL    FIND      FIND INSERT POINT
           ADI     0xFF      IF DONE, SET DF
           DB      LDI0
           PLO     X         X IS SIZE DIFFERENCE
           BDF     NEW
           GHI     BP        SAVE INSERT POINT
           PHI     PZ
           GLO     BP
           PLO     PZ
           DEC     X         MEASURE OLD LINE LENGTH
           DEC     X         -3 FOR LINE# AND <CR>
           DEC     X         REPEAT..
           LDA     PZ        -1 FOR EACH BYTE OF TEXT
           XRI     0x0D      ..UNTIL <CR>
           BNZ     $-4
NEW        DEC     BP        BACK OVER LINE#
           DEC     BP
           CALL    SWAP      TRADE LINE POINTERS
           DB      FECH,LINO
           LDN     BP
           XRI     0x0D      IF NEW LINE IS NULL,
           STXD
           STR     PZ
           BZ      HMUCH     THEN GO MARK IT
           GHI     AC        ELSE SAVE LINE NUMBER
           STR     PZ
           INC     PZ
           GLO     AC
           STR     PZ
           GHI     BP        MEASURE ITS LENGTH
           PHI     AC
           GLO     BP
           PLO     AC
           INC     X         LINE#
           INC     X         ENDING <CR>
           INC     X
           LDA     AC
           XRI     0x0D      AND ALL CHARS UNTIL FINAL <CR>
           BNZ     $-4
HMUCH      DB      FECH,SP   FIGURE AMOUNT OF MOVE
           PHI     AC
           LDA     PZ
           PLO     AC
           DB      FECH,MEND =DISTANCE FROM INSERT
           GLO     AC        TO END OF PROGRAM
           SM
           PLO     AC        LEAVE IT IN AC, NEGATIVE
           DEC     PZ
           GHI     AC
           SMB
           PHI     AC
           INC     PZ
           GLO     X         NOW COMPUTE NEW MEND,
           ADD               WHICH IS SUM OF OFFSET,
           PHI     X
           GLO     X
           ANI     0x80      WITH SIGN EXTEND,
           LSZ
           LDI     0xFF
           DEC     PZ
           ADC               PLUS OLD MEND
           SEX     R2
           STXD              PUSH ONTO STACK
           PHI     XX
           GHI     X
           STXD              (BACKWARDS)
           STR     R2        CHECK FOR OVERFLOW
           GLO     R2
           SD
           GHI     XX
           STR     R2
           GHI     R2
           SDB
           LBDF    ERR-1     IF YES, THEN QUIT
           GLO     X         ELSE NO, PREPARE TO MOVE
           BZ      STUFF     NO MOVE NEEDED
           STR     R2
           SHL
           BNF     MORE      ADD SOME SPACE
           DB      FECH,SP   DELETE SOME
           PHI     X         X IS DESTINATION
           LDA     PZ
           PLO     X
           SEX     R2
           SM
           PLO     XX        XX IS SOURCE
           GHI     X
           ADCI    0
           PHI     XX
           LDA     XX        NOW MOVE IT
           STR     X
           INC     X
           INC     AC
           GHI     AC
           BNZ     $-5
           BR      STUFF
MORE       GHI     X         SET UP POINTERS
           PLO     X         X IS DESTINATION
           GHI     XX
           PHI     X
           DB      FECH,MEND
           PHI     XX
           LDA     PZ
           PLO     XX        XX IS SOURCE
           DEC     AC
           SEX     X         NOW MOVE IT
           LDN     XX
           DEC     XX
           STXD
           INC     AC
           GHI     AC
           BNZ     $-5
STUFF      DB      FECH,MEND UPDATE MEND
           INC     R2
           LDA     R2
           STXD
           LDN     R2
           STR     PZ
           DB      FECH,SP   POINT INTO PROGRAM
           PHI     AC
           LDA     PZ
           PLO     AC
           DB      FECH,LINO INSERT NEW LINE
           PLO     X
           OR                IF THERE IS ONE
           BZ      INSX      NO, EXIT
           GLO     X         ELSE INSERT LINE NUMBER
           STR     AC
           INC     AC
           LDA     PZ
           STR     AC
           INC     AC
           LDA     BP        NOW REST OF LINE
           STR     AC
           XRI     0x0D      TO <CR>
           BNZ     $-5
INSX       LBR     EXIT
IO         STXD              PUSH OUT BYTE
           STR     R2
           DB      LDI0      CLEAR AC
           PHI     AC
           DEC     PZ
           LDA     R3        STORE RETURN IN RAM
           SEP     R5        (THIS IS NOT EXECUTED)
           STR     PZ
           DEC     PZ
           GLO     XX        MAKE IO INSTRUCTION
           ANI     0x0F
           ORI     0x60
           STR     PZ
           ANI     8
           LSZ
           NOP               INPUT, SO
           INC     R2        DO INCREMENT NOW
           SEP     PZ        GO EXECUTE, RESULT IN D
;
;
:STRT      PC                ':Q^'  Start Of IL Program
           GL
           SB
           BE      :LO
           BR      :STRT
:LO        BN      :STMT
           IL
           BR      :STRT
:XEC       SB
           RB
           XQ
:STMT      BC      :GOTO     'LET'
; !18 In Following Is Not Required It Was Generated By My
; Dissassembler And Indicates The Error Code Produced
; At This Point, The Assembler Assumes This Is A Remark
           BV      * !18
           BC      * !20     '='
:LET       JS      :EXPR
           BE      * !23
           SV
           NX
:GOTO      BC      :PRNT     'GO'
           BC      :GOSB     'TO'
           JS      :EXPR
           BE      * !34
           SB
           RB
           GO
:GOSB      BC      * !39     'SUB'
           JS      :EXPR
           BE      * !44
           GS
           GO
:PRNT      BC      :SKIP     'PR'
           BC      :P0       'INT'
:P0        BE      :P3
           BR      Z233
:P1        BC      Z234      ';'
:P2        BE      :P3
           NX
:P3        BC      Z235      '"'
           PQ
           BR      :P1
:SKIP      BR      :IF
Z234       BC      Z236      ','
           PT
           BR      :P2
Z236       BC      Z233      ':'
           PC                'S^'
Z233       BE      * !73
           NL
           NX
Z235       JS      :EXPR
           PN
           BR      :P1
:IF        BC      :INPT     'IF'
           JS      :EXPR
           JS      Z237
           JS      :EXPR
           BC      :I1       'THEN'
:I1        CP
           NX
           J       :STMT
:INPT      BC      :RETN     'INPUT'
Z242       BV      * !104
           SB
           BE      Z238
Z239       PC                '? Q^'
           GL
           BE      Z238
           BR      Z239
Z238       BC      Z240      ','
Z240       JS      :EXPR
           SV
           RB
           BC      Z241      ','
           BR      Z242
Z241       BE      * !123
           NX
:RETN      BC      :END      'RETURN'
           BE      * !132
           RS
           NX
:END       BC      :LIST     'END'
           BE      * !139
           WS
:LIST      BC      :RUN      'LIST'
           BE      Z243
           LN      #0001
           LN      #7FFF
           BR      Z244
Z243       JS      :EXPR
           JS      Z245
           BE      * !158
Z244       PC                '@^@^@^@^J^@^'
           LS
           NX
:RUN       BC      :CLER     'RUN'
           J       :XEC
:CLER      BC      :PLOT     'CLEAR'
           MT
:PLOT      BC      Z246      'PLOT'
           JS      :EXPR
           BC      Z247      ','
           DS
           DS
           LN      42
           JS      Z248      COMPARE >0 AND <42
           LN      64        MULTIPLY BY 64
           MP
           LN      64
           JS      Z249      GET NEXT EXPR AND COMP >0 AND <64
           AD                ADD TOGETHER (X*64+Y)
           BR      Z250      SKIP JUMPS
Z246       J       :POKE
Z247       J       Z251
Z250       LN      BUFF<<3         BUFF*8
           AD                ADD (BUFF*8)+(X*64+Y)
           DS
           LN      8
           DV                ((BUFF*8)+(X*64+Y))/8
           DS
           SX 4
           SX 2
           SX 5
           SX 3
           SX 5
           LN      8
           MP                 MULTIPLY BY 8
           SU                 GET REMANDER
           LB      TVXY+2
           SX 2
           SV                 STORE NEW BIT POINTER
           LB      TVXY
           SX 2
           SX 1
           SV                 STORE NEW CURSOR
           BC      Z252      ','
           JS      :EXPR
Z251       BE      * !245
           LN      TYPEV
           SX 2
           SX 1
           SX 3
           SX 1
           DS
           US                CALL TYPEV AND OUTPUT BYTE
           SP                POP RETURNED VALUE
           NX                NEXT STATEMENT
Z252       BE      * !257
           NX
:POKE      BC      :OUT      'POKE'
           LN      ILPOKE
           JS      :EXPR
           BR      Z253
:OUT       BC      :SAVE     'OUT'
           LN      ILINPOUT
           LN      8
           JS      Z249      GET EXPR AND COMP >0 AND <8
Z253       JS      Z245      CHECK FOR ,AND GET EXPR
           BE      * !284
           US                CALL ILINPOUT OR ILPOKE
           SP                POP RETURNED VALUE
           NX                NEXT STATEMENT
:SAVE      BC      Z254      'SAVE'
           BE      * !293
           PC                'TURN ON RECORD'
           NL
           PC                'HIT KEY'
           BR      Z255
Z254       J       :LOAD
Z255       LN      KEYV
           DS
           DS
           US                CALL KEY INPUT
           SP                POP RETURNED VALUE
           NL                NEW LINE
           LN      ILSAVE
           LB      MEND
           FV
           LB      BASIC
           FV
           SU                END PROGRAM-BEGIN PROGRAM
           LN      256
           AD                ADD 256 ????
           LB      BASIC
           FV
           US                CALL ILSAVE
           SP                POP RETURNED VALUE
           NX                NEXT STATEMENT
:LOAD      BC      Z256      'LOAD'
           BE      * !354
           BR      Z257
Z256       J       Z258
Z257       LB      MEND
           LN      ILLOAD
           LN      1
           LB      BASIC
           FV
           US                CALL ILLOAD
           BR      Z259      ILLOAD SKIPS THIS IF NO ERROR
           LN      0x18      SPARE STACK SIZE-1,DOES NOT GET IT?
           AD                ADD TO RETURN VALUE
           SV                SAVE MEM END
           WS                WARM START
Z259       NL
           PC                'TAPE ERROR'
           MT
Z258       BC      :DFLT     'REM'
           NX
:DFLT      BV      * !395
           BC      * !397    '='
           J       :LET
:EXPR      BC      Z260      '-'
           JS      :TERM
           NE
           BR      Z261
Z260       BC      Z262      '+'
Z262       JS      :TERM
Z261       BC      Z263      '+'
           JS      :TERM
           AD
           BR      Z261
Z263       BC      Z264      '-'
           JS      :TERM
           SU
           BR      Z261
Z264       RT
:TERM      JS      :RND
Z266       BC      Z265      '*'
           JS      :RND
           MP
           BR      Z266
Z265       BC      Z267      '/'
           JS      :RND
           DV
           BR      Z266
Z267       RT
:RND       BC      Z268      'RND('
           LN      0x8080
           FV
           LN      0x0929
           MP
           LN      0x1A85
           AD
           SV
           LB      0x80
           FV
           SX 1
           DS
           JS      Z269
           BR      Z270
Z268       BR      :USR
Z270       DS
           SX 4
           SX 2
           SX 3
           SX 5
           SX 3
           DV
           MP
           SU
           DS
           LB      6
           LN      0
           CP
           NE
           RT
:USR       BC      :INP      'USR('
           JS      :EXPR
           JS      Z245
           JS      Z245
           BC      * !495    ')'
           US
           RT
:INP       BC      :FLG      'INP('
           LN      ILINPOUT
           LN      8
           JS      Z249      GET EXPR AND COMP >0 AND <8
           LN      8         ADD 8 TO OP FOR INPUT
           AD
           BR      Z271
:FLG       BC      :PEEK     'FLG('
           LN      ILFLG     FLG WAS NEVER DOCUMENTED
           LN      5
           JS      Z249      GET EXPR AND COMP >0 AND <5
           LN      1
           SU                SUB 1
           BR      Z271
:PEEK      BC      Z272      'PEEK('
           LN      ILPEEK
           JS      :EXPR
Z271       BC      * !546    ')'
           DS
           US
           RT
Z272       BV      Z273
           FV
           RT
Z273       BN      Z274
           RT
Z274       BC      * !556    '('
Z269       JS      :EXPR
           BC      * !560    ')'
           RT
Z245       BC      Z275      ','
           J       :EXPR
Z275       DS
           RT
Z237       BC      Z276      '='
           LB      2
           RT
Z276       BC      Z277      '<'
           BC      Z278      '='
           LB      3
           RT
Z278       BC      Z279      '>'
           LB      5
           RT
Z279       LB      1
           RT
Z277       BC      * !589    '>'
           BC      Z280      '='
           LB      6
           RT
Z280       BC      Z281      '<'
           LB      5
           RT
Z281       LB      4
           RT
Z249       JS      :EXPR
           DS
           DS
           SX 6
           SX 1
           SX 7
           SX 1
Z248       LB      1
           SX 2
           SX 1
           CP
           BR      * !616
           LB      6
           LN      0
           CP
           BR      * !623
:ENDIL     RT                 End Of IL Program
           IDL
           IDL
TVON       LDI     INTERUPT  SETUP INTERRUPT ROUTINE
           PLO     R1
           LDI     HIGH INTERUPT
           PHI     R1
           B1      *         LOOP UNTIL EF1 GOES FALSE
;                  (EF1 brackets the 1861 interrupt request)
           INP     1         TURN ON 1861
           SEX     R3
           RET               ENABLE INTERRUPTS
           RETURN
Z283       ORI     0x34      MAKE FLG BRANCH
           PHI     RF        SAVE HIGH F
           DEC     RD
           SEX     RD        X=D
           GLO     RD        GLO D TO BRANCH TOO
           PLO     RF        SAVE IN LOW F
           LDI     0xD5       STORE RETURN
           STXD
           LDI     0x9D       LDI0      STORE CLEAR
           STXD
           GLO     RF        STORE BRANCH TOO ADDRESS
           STXD
           GHI     RF        STORE BRANCH INSTUCTION
           STR     RD
           LDI     1         LOAD 1
           SEP     RD        EXAMPLE  34XX  9D D5 BRANCH SKIPS C
ILFLG      BR      Z283
ILLOAD     LBR     Z284
ILSAVE     LDI     0xF0        SAVE TO TAPE
           PHI     RC
           LDI     0x65
           PLO     RC
           LDI     0x80
           PHI     RD
Z285       SMI     0
           SEP     RC         GOSUB #F065 ROM CALLS
           GHI     RD
           BNZ     Z285
Z287       SEQ
           LDA     RA
           PHI     RF
           LDI     9
           PLO     RF
           PLO     RD
           SHL
Z286       SEP     RC
           DEC     RF
           GHI     RF
           SHL
           PHI     RF
           GLO     RF
           BNZ     Z286
           GLO     RD
           SHR
           SEP     RC
           DEC     R8
           GHI     R8
           BNZ     Z287
           SEP     RC
           SEP     RC
           SEP     RC
           SEP     RC
           REQ
           RETURN
Z284       LDI     0xF0        LOAD FROM TAPE
           PHI     RC
           LDI     0xBA
           PLO     RC
Z288       LDI     0xF9
           PHI     RD
Z289       SEP     RC         GOSUB #F0BA ROM CALLS
           BNF     Z288
           GHI     RD
           BNZ     Z289
Z290       SEP     RC
           BDF     Z290
           LDI     1
           PHI     RD
           PLO     RD
Z291       SEP     RC
           GHI     RD
           SHLC
           PHI     RD
           BNF     Z291
           SEP     RC
           GLO     RD
           SHR
           BDF     Z292
           GHI     RD
           STR     RA
           SEX     RA
           OUT     4
           ADI     0xFF
           GLO     R8
           SHLC
           PLO     R8
           ANI     0x03
           BNZ     Z290
           INC     R9
           GLO     RA
Z292       RETURN
INPUTR     GHI     RE         INPUT FROM KEYBOARD ROUTINE
           BZ      KEYIN
Z293       B4      Z293       SERIAL INPUT ROUTINE
           SHR
           CALL    Z158
           B4      INPUTR
           LDI     0x7F
Z298       PLO     RF
           GHI     RE
           SHR
           B4      Z294
           BNF     Z295
           SEQ
           SKP
Z294       REQ
Z295       CALL    Z296
           GLO     RF
           SHR
           PLO     RF
           BN4     Z297
           ORI     0x80
Z297       BDF     Z298
           REQ
           CALL    Z296
           GLO     RF
           RETURN
OUTPUTR    PLO     RC         OUTPUT TO SCREEN ROUTINE
           PHI     RC
           GHI     RE
           BNZ     Z299
           GLO     RC
           BR      DISP
Z299       LDI     0x0A        SERIAL OUTPUT ROUTINE
           PLO     RF
           CALL    Z296
           ADI     0
Z300       CALL    Z296
           LSNF
           REQ
           SKP
           SEQ
           GHI     RC
           SMI     0
           SHRC
           PHI     RC
           DEC     RF
           GLO     RF
           BNZ     Z300
           GLO     RC
           RETURN
BLINK      SEP R7  TIME+2    LOOK AT TIMER
           SHL
           SHL
           SHL
           BNZ     Z301
           SHLC               EVERY HALF SECOND
           CALL    TVD        TOGGLE CURSOR
KEYIN      CALL    TVON       TURN DISPLAY ON
Z301       BN3     BLINK      WAIT FOR KEYIN
           GHI     RD
           CALL    TVD        TURN CURSOR OFF
           INP     7          GET KEYIN
DISP       CALL    TVD        DISPLAY CHAR
           PLO     RE
           XRI     0x0A        IF <LF>
           BZ      HOLD       YES
           B4      HOLD       ALSO HOLD ON EF4
           GLO     RE
           RETURN
HOLD       CALL    TVON       TURN DISPLAY ON
TVOFF      LDI     0x0C        TV OFF AND DELAY
           PHI     RF
           DEC     RF
           GHI     RF
           BNZ     *-2
           B4      *          THEN WAIT FOR /4
           SEX     R3
           OUT     1          TURN DISPLAY OFF
           IDL
           GLO     RE
           DIS
           RETURN
;
;       (ORG in last 40 bytes of page)
;
; Character Formatter - ASCII character in ACC.
;
TVD        ANI     0x7F       SET HIGH BIT TO 0
           PLO     RE         SAVE FOR EXIT
           SMI     0x60       CHECK FOR UPPER CASE
           GLO     RE
           BNF     Z304       IF NOT JUMP
           SMI     0x20       CONVERT TO UPPERCASE
           PLO     RE
Z304       SEX     R2
           GLO     RA         SAVE RA ON STACK
           STXD
           GHI     RA
           STXD
           GLO     R9         SAVE R9 ON STACK
           STXD
           GHI     R9
           STXD
           GLO     R8         SAVE R8 ON STACK
           STXD
           GHI     R8
           STXD
           LDI     SHFT       SET UP SHIFT PC
           PLO     RA
           LDI     HIGH SHFT
           PHI     RA
           SEP R7  TVXY       GET POINTER R8 = *0008-9
           PHI     R8         WHICH IS CURSOR
           LDA     RD
           PLO     R8
           LDA     RD         D = *000A AND BIT POINTER
           ANI     7          ONLY WONT LOW 3 BITS
           PHI     R9
           SEP R7  BS         IS THIS CANCEL
           GLO     RE
           XOR                AT BS+1=CANCEL
           BZ      DOTON
           GLO     RE
           SMI     0x7F        IGNORE <DEL>=7F
           BZ      EXIT2
           ADI     0x5F
           BDF     CHAR       IS IT PRINTABLE
           ADI     0x13
           BZ      ODBYTE     IS IT <CR>
           ADI     1
           BZ      OCBYTE     IS IT CLEARSCREEN
           ADI     2
           BZ      OABYTE     IS IF <LF>
           ADI     9
           BZ      DOTON      1 = TURN DOT ON
           BNF     DOTOFF     0 = TURN DOT OFF
EXIT2      SEP R7  0x1A        RD = #001B
           GHI     R9
           ANI     7
           PHI     R9
           ADI     0xFE
           GLO     R8
           SHLC
           XOR
           ANI     7
           XOR
           STR     RD
           SEP R7  TVXY+1     RD = #000A
           GHI     R9
           STXD               STORE BIT POINTER
           GLO     R8
           STXD               STORE DISPLAY LOCATION
           GHI     R8
           STXD
           INC     R2         RESTORE R8
           LDA     R2
           PHI     R8
           LDA     R2
           PLO     R8
           LDA     R2         RESTORE R9
           PHI     R9
           LDA     R2
           PLO     R9
           LDA     R2         RESTORE RA
           PHI     RA
           LDN     R2
           PLO     RA
           GLO     RE         GET SAVED CHARACTER
           RETURN
DOTOFF     LDI     0x80        POINT TO BIT
           SEP     RA
           XRI     0xFF        MAKE AND MASK
           AND
           BR      DOTON+4
DOTON      LDI     0x80
           SEP     RA
           OR
           STR     R8
           BR      EXIT2
OABYTE     GLO     R8         0A ROUTINE   DOWN LINE
           ADI     0x30
           PLO     R8
           GHI     R8
           ADCI    0
           PHI     R8
           BR      Z306
OCBYTE     LDI     BUFX-1     0C ROUTINE   CLEAR SCREEN
           PLO     R8
           LDI     HIGH BUFX-1
           PHI     R8
           SEX     R8         REPEAT...
CLRS       GHI     RD         CLEAR BYTE
           STXD               DECREMENT POINTER
           GLO     R8
           SMI     BUFF       HAS POINTER REACHED
           GHI     R8         START OF BUFFER
           SMBI    HIGH BUFF
           BDF     CLRS       ...UNTIL DONE
           IRX                CONTINUE TO <CR>
ODBYTE     GHI     RD         0D ROUTINE   CARRAGE RETURN
           PHI     R9         LEFT END OF LINE
           GLO     R8
           ANI     0xF8       OF BYTE * 8
           PLO     R8
Z306       GLO     R8         CHEAK FOR BOTTOM OF SCREEN
           SMI     BUFE
           GHI     R8
           SMBI    HIGH BUFE
           BNF     EXIT2
           LDI     BUFF
           PLO     RF
           GLO     R8
           ADI     BUFF
           ANI     0xF8
           PLO     RA
           LDI     HIGH BUFF
           PHI     RF
           ADCI    0
           PHI     RA
Z308       LDA     RA         SCROLL SCREEN
Z309       STR     RF
           INC     RF
           GLO     RF
           SMI     BUFE
           GHI     RF
           SMBI    HIGH BUFE
           BNF     Z308
           GLO     RF
           SMI     BUFX
           GHI     RD
           BNF     Z309
           GLO     R8
           ANI     7
           PLO     R8
           LDI     HIGH BUFE-8
           PHI     R8
           BR      EXIT2
CHAR       GLO     RE         20 BYTE - 5A BYTE ROUTINE
           SHL                INDEX INTO CHARACTER TABLE
           ADI     CTBL-64
           PLO     RF
           GHI     RD
           ADCI    HIGH CTBL-64
           PHI     RF
           SEP R7  MASK-1
           LDA     RF         GET BIT MASK
           STR     RD         SAVE IT
           LDA     RF
           ADI     DOTS
           PLO     RF         POINT TO DOT MATRIX
           GHI     RD
           ADCI    HIGH DOTS
           PHI     RF
           GHI     R9         SAVE CURSOR POSITION
           STR     R2
           DEC     R2
           LDN     RD         POSITION BIT MASK
           ANI     7
           PHI     R9
           LDN     RD
           ANI     0xF8       IT IS LEFT 5 BITS
           SEP     RA
           INC     RA         CANCEL 2ND CO-CALL
           INC     RA
           PLO     R9         SAVE MASK
           INC     R2
           SEX     R2         PREPARE TO FIND RELATIVE SHIFT
           LDN     RD
           ANI     7
           SD                 (X=2)
           PHI     R9
           GLO     R9         SAVE NEW MASK
           STR     RD
CHRL       LDA     RF         GET SOME DOTS
           SEX     RD
           AND                MASK INTO THIS CHARACTER
           SEP     RA         SHIFT IT
           OR                 INSERT INTO BUFFER
           STR     R8
           SEP     RA         UP TO NEXT LINE
           BDF     CHRL
           LDN     RD         CHECK FOR SPLIT WORD
           SEP     RA
           INC     RA         DON'T WANT COUNTER
           INC     RA
           BNF     Z310       NOT SPLIT
           GLO     RF         BACK UP DOT POINTER
           SMI     6
           PLO     RF
           INC     R8         POINT TO NEXT WORD
           GLO     R8
           ANI     7
           BNZ     Z311
           GHI     R9         OOPS,NEXT LINE
           ANI     0x87
           ORI     0x50
           PHI     R9
           DEC     RA
           DEC     RA
           SEP     RA
Z311       GHI     R9         CONVERT TO LEFT SHIFT
           ORI     0xF8
           PHI     R9
           BR      CHRL       GO DO IT AGAIN
Z310       SEX     R2
           ORI     0x80
           SKP                FIND RIGHT EDGE
Z312       INC     R9
           SHR
           BNF     Z312       OF MASK
           GLO     R9
           SDI     9          ANY LEFT?
           PHI     R9
           SMI     8
           BNF     CSTK       YES
           PHI     R9         NO, ADVANCE WORD
           INC     R8
           GLO     R8
           ANI     7
           BNZ     CSTK
           LDI     0x50
           PHI     R9
           DEC     RA
           DEC     RA
           SEP     RA
CSTK       NOP
           LBR     Z306
Z324       SEP     R3         < EXIT
SHFT       STR     R2         >ENTER SAVE BITS TO SHIFT
           GLO     R8         NOTE IF ADDRESS IS OUTSIDE DISPLAY
           SMI     BUFF
           GHI     R8         ONLY WORRY ABOUT TOO LOW
           SMBI    HIGH BUFF
           GHI     RD         IF SO, RETURN 0
           BNF     Z314       SO PROGRAM ISN'T DESTROYED
           GHI     R9         LOOK AT COUNTER
           ANI     0x87       MASK OUT WORD COUNTER
           PLO     R9
           BZ      Z315       NO SHIFT
           SHL
           BDF     Z316       LEFT
Z317       LDN     R2         SHIFT RIGHT ONCE
           SHR
           STR     R2
           DEC     R9
           GLO     R9
           BNZ     Z317       REPEAT N TIMES
           BR      Z318       DONE
Z316       SHR                LEFT SHIFT
           SDI     0x08       SET UP COUNTER
           PLO     R9
Z319       LDN     R2         DO IT
           SHL
           STR     R2
           DEC     R9
           GLO     R9
           BNZ     Z319       REPEAT
Z315       SHL                CLEAR CARRY
Z318       LDN     R2         GET BITS
Z314       SEX     R8
           SEP     R3         EXIT, C=SHIFT OFF RIGHT
           BR      Z320       SECOND ENTRY POINT
           BR      SHFT       OPTIONAL RERUNS
;
; Count words, moving pointer up
;
Z320       GHI     R9
           ADI     0x18       CONVERT IF NEGATIVE, ADD 1
           BNF     Z321
           ORI     0x80       (RESTORE SIGN)
Z321       ANI     0xF7
           PHI     R9
           SHL                SIGN IN C
           ANI     0xE0       IS THIS END?
           XRI     0xC0
           BNZ     Z322       NO
           GHI     R9         YES, RESTORE ORIGINAL
           ANI     7
           BNF     Z323
           ORI     0xF8       (NEGATIVE)
Z323       PHI     R9
           GLO     R8         BUMP R8 BACK TO BOTTOM
           ADI     0x28
           PLO     R8
           GHI     R8
           ADCI    0
           PHI     R8
           BR      Z324       C=0
Z322       GLO     R8         GO TO NEXT LINE UP
           SMI     8
           PLO     R8
           GHI     R8
           SMBI    0
           PHI     R8
           BR      Z324       C=1
;
; Interrupt service routine for 1861
;
Z327       LDI     3
           PLO     R0
           SEX     R2
           LDA     R2
           SHL
           LDA     R2         RECOVER D
           RET                < EXIT
INTERUPT   NOP                > ENTRY DISPLAY INT. ROUTINE
           DEC     R2
           SAV                SAVE T
           DEC     R2
           STXD               SAVE D
           LDI     HIGH BUFF
           PHI     R0         SET UP R0 FOR DMA
           LDI     BUFF
           PLO     R0
           B1      *          SYNCHRONIZE
Z326       GLO     R0
           DEC     R0
           PLO     R0
           SEX     R0         NOT A NOP
           DEC     R0
           PLO     R0         THREE LINES PER PIXEL
           GHI     R0         LAST LINE
           XRI     HIGH BUFE  IS NEW PAGE
           BNZ     Z326
           PHI     R0
           LDI     TIME+2     NOW UPDATE CLOCK
           PLO     R0
           SHRC               SAVE CARRY
           STR     R2
           LDX
           ADI     1          INCREMENT FRAME COUNT
           STR     R0
           SMI     0x3D       ONE SECOND
           BNF     Z327       NOT YET
           STXD               IF YES,
           LDX                    BUMP SECONDS
           ADI     1
           STR     R0
           BR      Z327
CTBL       DW      0x8608     SP MASK BYTE AND DATA POINTER
           DW      0x820A     ! MASK BYTE AND DATA POINTER
           DW      0xE508     " MASK BYTE AND DATA POINTER
           DW      0xF823     # MASK BYTE AND DATA POINTER
           DW      0xE435     $ MASK BYTE AND DATA POINTER
           DW      0xE55A     % MASK BYTE AND DATA POINTER
           DW      0xF423     & MASK BYTE AND DATA POINTER
           DW      0xC200     ' MASK BYTE AND DATA POINTER
           DW      0xC111     ( MASK BYTE AND DATA POINTER
           DW      0xC211     ) MASK BYTE AND DATA POINTER
           DW      0xE03C     * MASK BYTE AND DATA POINTER
           DW      0xE547     + MASK BYTE AND DATA POINTER
           DW      0xC307     , MASK BYTE AND DATA POINTER
           DW      0xC441     - MASK BYTE AND DATA POINTER
           DW      0x8407     . MASK BYTE AND DATA POINTER
           DW      0xE529     / MASK BYTE AND DATA POINTER
           DW      0xE111     0 MASK BYTE AND DATA POINTER
           DW      0xE243     1 MASK BYTE AND DATA POINTER
           DW      0xE44F     2 MASK BYTE AND DATA POINTER
           DW      0xE103     3 MASK BYTE AND DATA POINTER
           DW      0xE01D     4 MASK BYTE AND DATA POINTER
           DW      0xE042     5 MASK BYTE AND DATA POINTER
           DW      0xE249     6 MASK BYTE AND DATA POINTER
           DW      0xE073     7 MASK BYTE AND DATA POINTER
           DW      0xE303     8 MASK BYTE AND DATA POINTER
           DW      0xE049     9 MASK BYTE AND DATA POINTER
           DW      0x8304     : MASK BYTE AND DATA POINTER
           DW      0xC335     ; MASK BYTE AND DATA POINTER
           DW      0xE541     < MASK BYTE AND DATA POINTER
           DW      0xE503     = MASK BYTE AND DATA POINTER
           DW      0xE517     > MASK BYTE AND DATA POINTER
           DW      0xE05A     ? MASK BYTE AND DATA POINTER
           DW      0xE079     @ MASK BYTE AND DATA POINTER
           DW      0xE12F     A MASK BYTE AND DATA POINTER
           DW      0xE56D     B MASK BYTE AND DATA POINTER
           DW      0xE417     C MASK BYTE AND DATA POINTER
           DW      0xE217     D MASK BYTE AND DATA POINTER
           DW      0xE560     E MASK BYTE AND DATA POINTER
           DW      0xE534     F MASK BYTE AND DATA POINTER
           DW      0xE24F     G MASK BYTE AND DATA POINTER
           DW      0xE33B     H MASK BYTE AND DATA POINTER
           DW      0x8217     I MASK BYTE AND DATA POINTER
           DW      0xE155     J MASK BYTE AND DATA POINTER
           DW      0xE060     K MASK BYTE AND DATA POINTER
           DW      0xE343     L MASK BYTE AND DATA POINTER
           DW      0xF96D     M MASK BYTE AND DATA POINTER
           DW      0xF41D     N MASK BYTE AND DATA POINTER
           DW      0xE017     O MASK BYTE AND DATA POINTER
           DW      0xE573     P MASK BYTE AND DATA POINTER
           DW      0xE00B     Q MASK BYTE AND DATA POINTER
           DW      0xE53B     R MASK BYTE AND DATA POINTER
           DW      0xE029     S MASK BYTE AND DATA POINTER
           DW      0xE066     T MASK BYTE AND DATA POINTER
           DW      0xE21D     U MASK BYTE AND DATA POINTER
           DW      0xE379     V MASK BYTE AND DATA POINTER
           DW      0xFB2E     W MASK BYTE AND DATA POINTER
           DW      0xE260     X MASK BYTE AND DATA POINTER
           DW      0xE273     Y MASK BYTE AND DATA POINTER
           DW      0xE035     Z MASK BYTE AND DATA POINTER
           DW      0xC017     [ MASK BYTE AND DATA POINTER
           DW      0xE329     \ MASK BYTE AND DATA POINTER
           DW      0xC117     ] MASK BYTE AND DATA POINTER
           DW      0xE200     ^ MASK BYTE AND DATA POINTER
           DW      0xE507     _ MASK BYTE AND DATA POINTER
DOTS       DW      0x0000     START OF DOT TABLE
           DW      0x8080
           DW      0xE897
           DW      0xA897
           DW      0x6840
           DW      0x4020
           DW      0x40AD
           DW      0xB6AD
           DW      0x4404
           DW      0x2056
           DW      0xDD57
           DW      0x2000
           DW      0xF4AA
           DW      0xA9AA
           DW      0xF400
           DW      0x39E9
           DW      0xABAD
           DW      0x2900
           DW      0x55FA
           DW      0x54F8
           DW      0x5400
           DW      0xC024
           DW      0x4A91
           DW      0x600A
           DW      0x5575
           DW      0x5151
           DW      0x2014
           DW      0xEC86
           DW      0x4C27
           DW      0xE400
           DW      0x1515
           DW      0xBE55
           DW      0xB600
           DW      0x01C2
           DW      0x3CD2
           DW      0x91F0
           DW      0x1002
           DW      0x572A
           DW      0x70A0
           DW      0x5800
           DW      0xDE68
           DW      0xA462
           DW      0x1C06
           DW      0x2355
           DW      0x1518
           DW      0x1051
           DW      0x0422
           DW      0xA144
           DW      0x00AF
           DW      0xACD6
           DW      0xACAF
           DW      0x0042
           DW      0x425F
           DW      0x52F9
           DW      0x1000
           DW      0x4645
           DW      0x566D
           DW      0x4600
           DW      0x9494
           DW      0x562D
           DW      0xEE00
           DW      0x6894
           DW      0xB4B4
           DW      0x5400
           SEP     RF
