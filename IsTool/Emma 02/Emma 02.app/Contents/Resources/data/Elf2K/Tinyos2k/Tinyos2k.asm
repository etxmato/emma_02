; Netronic TinyBasic For The ELF/OS operating system.
; TINYOS2K By Richard Peters, Richard11092000@cox.net
; Special THANKS to Mike Riley, Tom Pittan,Lee Hart 
; and Dave Ruske. This version can be named anything 
; on the ELF/OS system, But if Referenceing it to 
; me please call it TINYOS2K. There was many versions of 
; this. In the Attempt to get it working.
;
; Assembled With QELFEXE V2.5 Multiformat Assembler
; Designed to run In RAM Starting at 2000
; This version is designed to run, with ELF/OS operating
; system by Mike Riley, LOAD and SAVE has been 
; adapted to Load and Save From Hard Drive/CF currently
; Supported By Elf/os.
; additional commands are: 
; SYSTEM - Restarts,Elf/os
; MEM - give memory remaining
; NEW - Clone for CLEAR
; Load and Save syntac LOAD NAME, SAVE NAME
; space is required before NAME or path name.  
; 
; Do to the Different Cold Start address,
; some Tinybasic program that use USR calls may
; need to be changed, Adding an Offset of 8192
; may correct problem.
; Last update 03/27/2015
; This version has had all I/O striped out and
; Now uses ELF2K's BIOS functions. Baud is assumed already set
; 
; This also stops the PLOT Function from affecting
; the coursor location, but will still output a character
; Assembler has been modified to generate error numbers
; for IL code. You will see this in the listing.
;
; Added functions: FRE,16 bit DPEEK and 16 bit DPOKE
; Added INPUT "TEXT"A abilities
; Added Operators AND,OR,XOR
; 
; IL Language additions:
;AN      25      AND
;OR      26      OR
;XR      28      XOR
;    Bitwize AND,OR,XOR of the two numbers represented by the top four
;    bytes of the expression stack, and replace them with the two-byte
;    result. Stack underflow results in an error stop.
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
;               RE.1=  USED FOR INPUT,OUTPUT
X          EQU     15        BASIC: SCRATCH REGISTER
;
LDI0       EQU     8EH       GLO RE - CLEAR ACCUM. MACRO
FECH       EQU     0D7H      SEP R7 - PAGE 0 MACRO
;
; DISPLAY BUFFER EQU
;
BUFF       EQU     0x1DB0     ONLY CHANGE PAGE, UNLESS YOU
BUFE       EQU     BUFF+344   WONT TO CHANGE INTERUPT ROUTINE
BUFX       EQU     BUFE+56    ALSO LIMITED TO 1DB0 BY PLOT
;
;
RSTACK     EQU     #80        RESERVE STACK
           ORG     #2000
PAGE       LBR     COLDV
Z165       PLO     R7
           LDI     0     
           LBR     Z1165
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
Z1165      PLO     RE         I/O ROUTINES CLEAR LDI0
           LBDF    PEND       GOTO WARM START
;
; BIOS ADDRESS 
;
SERINPUT   EQU     #FF06
SEROUTPUT  EQU     #FF03
INITR      LDI     0x56       
           CALL    TYPEV      OUTPUT 0C  CLEARSCREEN
           LDI     0x31
           CALL    TYPEV      OUTPUT VERSION NUMBER
           LBR     CLEAR
BRKTST     ADI     0          BREAK TEST
           BN4     Z157
           SMI     0
Z157       RETURN
GRINPUT    CALL    INITCR
           CALL    SERINPUT
           PHI     RF         SAVE D
           BR      GRREGFIX
GROUTPUT   CALL    INITCR
           GHI     RF         RECOVER D  
           CALL    SEROUTPUT
GRREGFIX   LDI     HIGH CALL  RESTORE CALL ROUTINES
           PHI     R4
           PHI     R5
           PHI     R7
           LDI     CALL
           PLO     R4
           LDI     RETURN
           PLO     R5
           LDI     FETCH
           PLO     R7
           INC     R2
           LDA     R2          RELOAD R6 FROM STACK
           PHI     R6
           LDN     R2
           PLO     R6
           LDI     0           RESET LDI0 REGISTER
           PLO     RE
           GHI     RF          RECOVER D
           RETURN
           ORG     #2100
COLDV      NOP                COLD START
           BR      COLD
           LBR     WARM       WARM START ENTRY
KEYV       LBR     GRINPUT    BRANCH TO CHARATER INPUT
TYPEV      LBR     GROUTPUT   BRANCH TO CHARATER OUPUT
BREAKV     LBR     BRKTST     BRANCH TO BREAK TEST
; DEFAULTS LOADED TO DIRECT PAGE
           DB      0x08       BACKSPACE CODE
           DB      0x1B       LINE CANCEL CODE
           DB      0x00       PAD CHARATER
           DB      0x00       TAPE MODE ENABLE FLAG 80=ENABLED
           DB      RSTACK     SPARE STACK SIZE
DPOKE      GHI     RA         DPOKE
           STR     R8
           INC     R8
           GLO     RA
ILPOKE     STR     R8         POKE
           RETURN
           DW      :STRT      ADDRESS OF IL PROGRAM START
CONST      DW      END        DEFAULT START OF PROGRAM SPACE
           DB      0x76        END MEM STOP
           DB      HIGH PAGE
; END DEFAULTS
DPEEK      LDA     R8         DOUBLE PEEK ENTRY
           SKP
ILPEEK     DB      LDI0       PEEK ENTRY
           PHI     RA
           LDA     R8
           RETURN
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
           DW      IXOR
           DW      GETLN
           DW      IOR
           DW      IAND
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
           DB      LDI0       WITH LINE# = 0
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
           ANDI    #7F       MASK HIGH BIT
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
ILINPOUT   STXD              PUSH OUT BYTE
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
; ADDED LOGIC OPERATORS      
IAND       CALL    APOP      Load AC With First #
           SEX     PZ
           INC     PZ        Move To End Next #
           GLO     AC
           AND               And Low Bytes
           STXD
           GHI     AC         
           AND               And High Bytes
           STR     PZ
           RETURN
IOR        CALL    APOP      Load AC With First #
           SEX     PZ
           INC     PZ        Move To End Next #
           GLO     AC
           OR                Or Low Bytes
           STXD
           GHI     AC        
           OR                Or High Bytes
           STR     PZ
           RETURN
IXOR       CALL    APOP      Load AC With First #
           SEX     PZ
           INC     PZ        Move To End Next #
           GLO     AC
           XOR               Xor Low Bytes
           STXD
           GHI     AC        
           XOR               Xor High Bytes
           STR     PZ
           RETURN
;
;
:STRT      PC                'TB:'  Start Of IL Program
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
           BV      * 
           BC      *         '='

:LET       JS      :EXPR
           BE      * 
           SV
           NX

:GOTO      BC      :PRNT     'GO'
           BC      :GOSB     'TO'
           JS      :EXPR
           BE      * 
           SB
           RB
           GO

:GOSB      BC      *         'SUB'
           JS      :EXPR
           BE      *  
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
Z233       BE      * 
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
           BC      Z242      '"'
           PQ
Z242       BV      * 
           SB
           BE      Z238
Z239       PC                '?'
           GL
           BE      Z238
           BR      Z239
Z238       BC      Z240      ','
Z240       JS      :EXPR
           SV
           RB
           BC      Z241      ','
           BR      Z242
Z241       BE      * 
           NX

:RETN      BC      :END      'RETURN'
           BE      * 
           RS
           NX

:END       BC      :LIST     'END'
           BE      * 
           WS

:LIST      BC      :RUN      'LIST'
           BE      Z243
           LN      #0001
           LN      #7FFF
           BR      Z244
Z243       JS      :EXPR
           JS      Z245
           BE      * 
Z244       LS
           NX

:RUN       BC      :MEM      'RUN'
           J       :XEC

:MEM       BC      :CLER     'MEM'
           JS      :FRE1
           PN
           NL
           NX       

:CLER      BC      :NEW      'CLEAR'
           MT

:NEW       BC      :PLOT     'NEW'
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

Z250       LN      #DB0<<3   
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
           LN      BUFF-#DB0  ADD OFFSET For Higher Address.
           AD
           SV                 STORE NEW CURSOR
           BC      Z252      ','
           JS      :EXPR
Z251       BE      * 
           LN      TYPEV
           SX 2
           SX 1
           SX 3
           SX 1
           DS
           US                CALL TYPEV AND OUTPUT BYTE
           SP                POP RETURNED VALUE
           NX                NEXT STATEMENT
Z252       BE      * 
           NX

:POKE      BC      :DPOKE    'POKE'
           LN      ILPOKE
:POKE1     JS      :EXPR
           BR      Z253

:DPOKE     BC      :OUT      'DPOKE'
           LN      DPOKE
           BR      :POKE1

:OUT       BC      :SAVE     'OUT'
           LN      ILINPOUT
           LN      8
           JS      Z249      GET EXPR AND COMP >0 AND <8
Z253       JS      Z245      CHECK FOR ,AND GET EXPR
           BE      * 
           US                CALL ILINPOUT OR ILPOKE OR DPOKE
           SP                POP RETURNED VALUE
           NX                NEXT STATEMENT

:SAVE      BC      :LOAD     'SAVE'
           LN      SAVEDISK
           LB      MEND
           FV
           LB      BASIC
           FV
           SU                END PROGRAM-BEGIN PROGRAM
           LN      RSTACK-1
           SU                -RESERVE STACK
           LB      BASIC
           FV
           US                CALL ILSAVE
           BR      *         SAVEDISK SKIPS THIS IF NO ERROR
           WS                WARM START

:LOAD      BC      :SYSTEM   'LOAD'
           LB      MEND
           LN      LOADDISK
           LN      1
           LB      BASIC
           FV
           US                CALL LOADDISK
           BR      *         LOADDISK SKIPS THIS IF NO ERROR
           LN      RSTACK-1  SPARE STACK SIZE-1,DOES NOT GET IT?
           AD                ADD TO RETURN VALUE
           SV                SAVE MEM END
           WS                WARM START

:SYSTEM    BC      Z258      'SYSTEM'
           LN      #300      SYSTEM COLD RESTART
           DS
           DS  
           US

Z258       BC      :DFLT     'REM'
           NX

:DFLT      BV      * 
           BC      *         '='
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
Z265       BC      :AND      '/'
           JS      :RND
           DV
           BR      Z266

;:AND       BC      :OR       '&'

:AND       BC      :OR       'AND'
           JS      :RND
           AN
:AND1      BR      Z266

;:OR        BC      :XOR      '^'    This is to = #5E + #80

:OR        BC      :XOR      'OR'    
           JS      :RND
           OR 
           BR      Z266

;:XOR       BC      Z267      '|'

:XOR       BC      Z267      'XOR'
           JS      :RND
           XR
           BR      :AND1
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
           BC      *         ')'
           US
           RT

:INP       BC      :FLG      'INP('
           LN      ILINPOUT
           LN      8
           JS      Z249      GET EXPR AND COMP >0 AND <8
           LN      8         ADD 8 TO OP FOR INPUT
           AD
           BR      :FLG1

:FLG       BC      :FRE      'FLG('
           LN      ILFLG     FLG WAS NEVER DOCUMENTED
           LN      5
           JS      Z249      GET EXPR AND COMP >0 AND <5
           LN      1
           SU                SUB 1
:FLG1      BR      Z271

:FRE       BC      :PEEK     'FRE'
:FRE1      LB      STACK
           FV
           LB      MEND
           FV
           SU
           RT

:PEEK      BC      :DPEEK    'PEEK('
           LN      ILPEEK
:PEEK1     JS      :EXPR
Z271       BC      *         ')'
           DS
           US
           RT

:DPEEK     BC      Z272      'DPEEK('
           LN      DPEEK
           BR      :PEEK1
Z272       BV      Z273
           FV
           RT
Z273       BN      Z274
           RT

Z274       BC      *         '('
Z269       JS      :EXPR
           BC      *         ')'
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
Z277       BC      *         '>'
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
           BR      *  
           LB      6
           LN      0
           CP
           BR      * 
:ENDIL     RT                 End Of IL Program

ILFLG      ORI     0x34      MAKE FLG BRANCH
           PHI     RF        SAVE HIGH F
           DEC     RD
           SEX     RD        X=D
           GLO     RD        GLO D TO BRANCH TOO
           PLO     RF        SAVE IN LOW F
           LDI     0xD5      STORE RETURN
           STXD
           LDI     LDI0      STORE CLEAR
           STXD
           GLO     RF        STORE BRANCH TOO ADDRESS
           STXD
           GHI     RF        STORE BRANCH INSTUCTION
           STR     RD
           LDI     1         LOAD 1
           SEP     RD        EXAMPLE 34XX  90 D5 BRANCH SKIPS CLEAR
;
;  ELF/OS SYSTEM CALLS
;
INITCR     EQU     #FF3F
O_OPEN     EQU     #0306
O_READ     EQU     #0309
O_WRITE    EQU     #030C
O_CLOSE    EQU     #0312
;
; OPENFILE USED BY LOADDISK AND SAVEDISK  
;
OPENFILE   INC     RB             MOVE PAST SPACE 
           GHI     RB
           PHI     RF
           GLO     RB
           PLO     RF
LOOP1      LDA     RB             FINED CR
           SMI     33
           BDF     LOOP1
           DEC     RB             WAS CR BACK UP
           LDI     0
           STR     RB             ZERO CR
           LDI     HIGH FILED
           PHI     RD
           LDI     FILED
           PLO     RD
           CALL    O_OPEN
           RETURN
FILED      DW      0
           DW      0
           DW      DTA
           ORG     *+15
;
; SAVE TO DISK ROUTINE
; 
; CALLED BY IL WITH COMMAND LINE POINTER RB 
; AND RA = PROGRAM START
; AND R8 = NUMBER OF BYTES TO WRITE.
;
SAVEDISK   CALL    INITCR         SET TO ELFOS CALLING
           LDI     3
           PLO     R7             FILE OPEN FLAG
           CALL    OPENFILE
           BDF     DISKEXIT       BRANCH ON ERROR
           GHI     RA             MOVE START ADDRESS
           PHI     RF
           GLO     RA
           PLO     RF
           GHI     R8             MOVE NUMBER OF BYTE TO SAVE
           PHI     RC
           GLO     R8           
           PLO     RC
           CALL    O_WRITE
           BR      CLOSEFILE
;
; LOAD FROM DISK ROUTINE
;
; CALLED BY IL WITH COMMAND LINE POINTER RB 
; AND RA = PROGRAM START
; AND RETURNS NEW PROGRAM END IN RA
; 
;
LOADDISK   CALL    INITCR         SET TO ELFOS CALLING
           LDI     0              SET FLAG TO OPEN
           PLO     R7
           CALL    OPENFILE
           BDF     DISKEXIT
           GHI     RA             MOVE START ADDRESS
           PHI     RF
           GLO     RA
           PLO     RF
           LDI     #4F            LOAD #4F4F BYTES MAX.
           PHI     RC             OR TO END OF FILE 
           PLO     RC
           CALL    O_READ
CLOSEFILE  CALL    O_CLOSE
           INC     R9          SKIP ERROR IN IL CODE
           GHI     RF
           PHI     RA          SET HIGH RETURN VALUE
DISKEXIT   LDI     HIGH CALL      RESTORE CALL ROUTINES
           PHI     R4
           PHI     R5
           PHI     R7
           LDI     CALL
           PLO     R4
           LDI     RETURN
           PLO     R5
           LDI     FETCH
           PLO     R7
           INC     R2
           LDA     R2          RELOAD R6 FROM STACK
           PHI     R6
           LDN     R2
           PLO     R6
           LDI     0           RESET LDI0 REGISTER
           PLO     RE
           GLO     RF          D=LOW RETURN VALUE
           RETURN    
DTA        DB      0
           ORG     *+512
END        EQU     *
