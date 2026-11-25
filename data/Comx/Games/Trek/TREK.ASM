;=====================================================================
; TREK.ASM - TREK for the COMX-35 in 1802 machine code
; Port of Ian Gray's 1983 C64 BASIC TREK (system names and rules from
; the owner's tape).  Font, glyphs and code are new.  A18 syntax.
; Start: CALL(@4500).  SCRT: SEP R4 / WORD addr = call, SEP R5 = return.
;=====================================================================
PAGE    EQU     0F800H
CRAM    EQU     0F400H
R3ON    EQU     0C0H
R3OFF   EQU     0D0H
R4OFF   EQU     0080H
R5NORM  EQU     8088H
R5CMEM  EQU     8089H
LDCELL  EQU     0089H
K_UP    EQU     82H
K_RIGHT EQU     83H
K_LEFT  EQU     84H
K_DOWN  EQU     85H
K_ESC   EQU     81H
K_CR    EQU     80H
BSS     EQU     0A000H
MAP     EQU     BSS
CSAVE   EQU     0A400H
SAVESP  EQU     0A300H
RNDV    EQU     0A302H
SCEN    EQU     0A304H
LEVEL   EQU     0A305H
SHIPX   EQU     0A306H
SHIPY   EQU     0A307H
SHIPS   EQU     0A308H
CURX    EQU     0A309H
CURY    EQU     0A30AH
UNDER   EQU     0A30BH
PANEL   EQU     0A30CH
LASTK   EQU     0A30DH
RPT     EQU     0A30EH
SEED    EQU     0A30FH
QCNT    EQU     0A310H
QRD     EQU     0A3E8H
VMODE   EQU     0A311H
QUEUE   EQU     0A800H
SYSX    EQU     0A320H
SYSY    EQU     0A340H
SYST    EQU     0A360H
SYSD    EQU     0A380H
SYSN    EQU     0A3A0H
        ORG     4500H
START   SEX     R2
        GLO     R4
        STXD
        GHI     R4
        STXD
        GLO     R5
        STXD
        GHI     R5
        STXD
        GLO     R6
        STXD
        GHI     R6
        STXD
        GLO     R7
        STXD
        GHI     R7
        STXD
        GLO     R8
        STXD
        GHI     R8
        STXD
        GLO     R9
        STXD
        GHI     R9
        STXD
        GLO     RA
        STXD
        GHI     RA
        STXD
        GLO     RB
        STXD
        GHI     RB
        STXD
        GLO     RC
        STXD
        GHI     RC
        STXD
        GLO     RD
        STXD
        GHI     RD
        STXD
        GLO     RE
        STXD
        GHI     RE
        STXD
        GLO     RF
        STXD
        GHI     RF
        STXD
        SEX     R3
        DIS
        BYTE    23H
        LDI     HIGH (SCALL)
        PHI     R4
        LDI     LOW (SCALL)
        PLO     R4
        LDI     HIGH (SRET)
        PHI     R5
        LDI     LOW (SRET)
        PLO     R5
        LDI     HIGH (SAVESP)
        PHI     RA
        LDI     LOW (SAVESP)
        PLO     RA
        GLO     R2
        STR     RA
        INC     RA
        GHI     R2
        STR     RA
        LBR     INIT
        SEP     R3
SCALL   PHI     RF
        SEX     R2
        GHI     R6
        STXD
        GLO     R6
        STXD
        GHI     R3
        PHI     R6
        GLO     R3
        PLO     R6
        LDA     R6
        PHI     R3
        LDA     R6
        PLO     R3
        GHI     RF
        BR      SCALL-1
        SEP     R3
SRET    PHI     RF
        SEX     R2
        GHI     R6
        PHI     R3
        GLO     R6
        PLO     R3
        INC     R2
        LDA     R2
        PLO     R6
        LDN     R2
        PHI     R6
        GHI     RF
        BR      SRET-1
VIDINIT SEX     R3
        OUT     3
        BYTE    R3OFF
        LDI     HIGH (R4OFF)
        PHI     RF
        LDI     LOW (R4OFF)
        PLO     RF
        SEX     RF
        OUT     4
        LDI     HIGH (R5NORM)
        PHI     RF
        LDI     LOW (R5NORM)
        PLO     RF
        OUT     5
        LDI     HIGH (PAGE)
        PHI     RF
        LDI     LOW (PAGE)
        PLO     RF
        OUT     7
        SEX     R2
        SEP     R5
VIDON   SEX     R3
        OUT     3
        BYTE    R3ON
        SEX     R2
        LDI     HIGH (VMODE)
        PHI     RA
        LDI     LOW (VMODE)
        PLO     RA
        LDI     1
        STR     RA
        SEP     R5
VIDOFF  SEX     R3
        OUT     3
        BYTE    R3OFF
        SEX     R2
        LDI     HIGH (VMODE)
        PHI     RA
        LDI     LOW (VMODE)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QCNT)
        PHI     RA
        LDI     LOW (QCNT)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QRD)
        PHI     RA
        LDI     LOW (QRD)
        PLO     RA
        LDI     0
        STR     RA
        SEP     R5
; PUTC - write D to page address RD: direct when display is off, else queued (uses RA, RB)
PUTC    PLO     RB
        LDI     HIGH (VMODE)
        PHI     RA
        LDI     LOW (VMODE)
        PLO     RA
        LDN     RA
        LBZ     PDIR
        LDI     HIGH (QCNT)
        PHI     RA
        LDI     LOW (QCNT)
        PLO     RA
        LDN     RA
        SMI     80
        LBDF    PQF
        LDN     RA
        ADI     1
        STR     RA
        SMI     1
        PHI     RB
        SHL
        STR     R2
        GHI     RB
        ADD
        PLO     RA
        LDI     HIGH QUEUE
        PHI     RA
        GHI     RD
        STR     RA
        INC     RA
        GLO     RD
        STR     RA
        INC     RA
        GLO     RB
        STR     RA
PQX     SEP     R5
PDIR    GLO     RB
        STR     RD
        SEP     R5
PQF     GLO     RB
        STXD
        GHI     RC
        STXD
        GLO     RC
        STXD
        SEP     R4
        WORD    FRAME
        IRX
        LDXA
        PLO     RC
        LDXA
        PHI     RC
        LDX
        LBR     PUTC
; FRAME - wait for non-display, then flush queued writes until the display starts again
FRAME
        SEP     R4
        WORD    WAITFR
        LDI     HIGH (QRD)
        PHI     RA
        LDI     LOW (QRD)
        PLO     RA
        LDN     RA
        PLO     RB
        LDI     HIGH (QCNT)
        PHI     RA
        LDI     LOW (QCNT)
        PLO     RA
        LDN     RA
        PHI     RB
FL0     GLO     RB
        STR     R2
        GHI     RB
        SM
        LBZ     FDONE
        BN1     FL2
        LBR     FSAVE
FL2     GLO     RB
        SHL
        STR     R2
        GLO     RB
        ADD
        PLO     RA
        LDI     HIGH QUEUE
        PHI     RA
        LDA     RA
        PHI     RC
        LDA     RA
        PLO     RC
        LDN     RA
        STR     RC
        INC     RB
        LBR     FL0
FDONE
        LDI     HIGH (QCNT)
        PHI     RA
        LDI     LOW (QCNT)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QRD)
        PHI     RA
        LDI     LOW (QRD)
        PLO     RA
        LDI     0
        STR     RA
FX      SEP     R5
FSAVE
        LDI     HIGH (QRD)
        PHI     RA
        LDI     LOW (QRD)
        PLO     RA
        GLO     RB
        STR     RA
        SEP     R5
WAITFR
WF1     BN1     WF1
WF2     B1      WF2
        SEP     R5
CMEMON
        LDI     HIGH (PAGE+LDCELL)
        PHI     RA
        LDI     LOW (PAGE+LDCELL)
        PLO     RA
        GLO     RE
        STR     RA
        LDI     HIGH (R5CMEM)
        PHI     RF
        LDI     LOW (R5CMEM)
        PLO     RF
        SEX     RF
        OUT     5
        LDI     HIGH (LDCELL)
        PHI     RF
        LDI     LOW (LDCELL)
        PLO     RF
        OUT     6
        SEX     R2
        LDI     HIGH (CRAM)
        PHI     RB
        LDI     LOW (CRAM)
        PLO     RB
        SEP     R5
CMEMOFF
        LDI     HIGH (R5NORM)
        PHI     RF
        LDI     LOW (R5NORM)
        PLO     RF
        SEX     RF
        OUT     5
        SEX     R2
        SEP     R5
CHSAVE
        LDI     HIGH (CSAVE)
        PHI     RC
        LDI     LOW (CSAVE)
        PLO     RC
        LDI     0
        PLO     RE
CSV1
        SEP     R4
        WORD    CMEMON
        LDI     8
        PHI     RE
CSV2    LDA     RB
        STR     RC
        INC     RC
        GHI     RE
        SMI     1
        PHI     RE
        LBNZ    CSV2
        SEP     R4
        WORD    CMEMOFF
        INC     RE
        GLO     RE
        XRI     80H
        LBNZ    CSV1
        SEP     R5
CHREST
        LDI     HIGH (CSAVE)
        PHI     RC
        LDI     LOW (CSAVE)
        PLO     RC
        LBR     CHWR
CHLOAD
        LDI     HIGH (CHTAB)
        PHI     RC
        LDI     LOW (CHTAB)
        PLO     RC
CHWR    LDI     0
        PLO     RE
CLD1
        SEP     R4
        WORD    CMEMON
        LDI     8
        PHI     RE
CLD2    LDA     RC
        STR     RB
        INC     RB
        GHI     RE
        SMI     1
        PHI     RE
        LBNZ    CLD2
        SEP     R4
        WORD    CMEMOFF
        INC     RE
        GLO     RE
        XRI     80H
        LBNZ    CLD1
        SEP     R5
; CLRSCR - spaces over the whole page (display off)
CLRSCR
        LDI     HIGH (PAGE)
        PHI     RA
        LDI     LOW (PAGE)
        PLO     RA
        LDI     HIGH (960)
        PHI     RE
        LDI     LOW (960)
        PLO     RE
CS1     LDI     20H
        STR     RA
        INC     RA
        DEC     RE
        GLO     RE
        LBNZ    CS1
        GHI     RE
        LBNZ    CS1
        SEP     R5
; POS - RE.1 = row, RE.0 = col -> RD = page address
POS
        LDI     HIGH (PAGE)
        PHI     RD
        LDI     LOW (PAGE)
        PLO     RD
        GLO     RE
        STR     R2
        GLO     RD
        ADD
        PLO     RD
        GHI     RD
        ADCI    0
        PHI     RD
PS1     GHI     RE
        LBZ     PS2
        SMI     1
        PHI     RE
        GLO     RD
        ADI     40
        PLO     RD
        GHI     RD
        ADCI    0
        PHI     RD
        LBR     PS1
PS2     SEP     R5
; PRT - RC = text, RD = page addr, RF.0 = 00 cyan / 80H white / 01 green / 81H yellow / 82H red
PRT     LDA     RC
        LBZ     PRX
        PLO     R7
        GLO     RF
        ANI     3
        LBZ     PRN
        XRI     2
        LBZ     PRR
        GLO     R7
        SMI     41H
        LBNF    PRN
        SMI     1AH
        LBDF    PRN
        GLO     R7
        ADI     20H
        PLO     R7
        LBR     PRN
PRR     GLO     R7
        SMI     20H
        LBNF    PRN
        SMI     40H
        LBDF    PRN
        GLO     R7
        ADI     LOW (REDT-20H)
        PLO     RA
        LDI     HIGH (REDT-20H)
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
PRN     GLO     RF
        ANI     80H
        STR     R2
        GLO     R7
        OR
        SEP     R4
        WORD    PUTC
        INC     RD
        LBR     PRT
PRX     SEP     R5
; PNUM - D (0-255) in decimal at RD, colour RF.0, no leading zeros (uses RE)
PNUM    PLO     RE
        LDI     0
        PHI     RE
        LDI     100
        PLO     R8
        SEP     R4
        WORD    PDIG
        LDI     10
        PLO     R8
        SEP     R4
        WORD    PDIG
        LDI     1
        PHI     RE
        LDI     1
        PLO     R8
        SEP     R4
        WORD    PDIG
        SEP     R5
; PDIG - digit of RE.0 for place value RF.1; RE.1 = 'printed' flag
PDIG    LDI     0
        PLO     R7
PD1     GLO     RE
        STR     R2
        GLO     R8
        SD
        LBNF    PD2
        GLO     RE
        STR     R2
        GLO     R8
        SD
        LBDF    PD1X
PD2     GLO     R7
        LBNZ    PD3
        GHI     RE
        LBZ     PDX
PD3     GLO     R7
        ADI     30H
        STR     R2
        GLO     RF
        OR
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     1
        PHI     RE
PDX     SEP     R5
PD1X    GLO     R8
        STR     R2
        GLO     RE
        SM
        PLO     RE
        INC     R7
        LBR     PD1
; RAND - 16-bit LFSR, D = new low byte (uses R7)
RAND
        LDI     HIGH (RNDV)
        PHI     R7
        LDI     LOW (RNDV)
        PLO     R7
        LDA     R7
        SHR
        PHI     R8
        LDN     R7
        SHRC
        PLO     R8
        LBNF    RN1
        GHI     R8
        XRI     0B4H
        PHI     R8
RN1     GLO     R8
        STR     R7
        DEC     R7
        GHI     R8
        STR     R7
        GLO     R8
        SEP     R5
; RANDN - D = N (1-255) -> D = random 0..N-1 (uses R7, R8)
RANDN   PLO     R8
        GLO     R8
        STXD
        SEP     R4
        WORD    RAND
        IRX
        PLO     R7
        LDX
        STR     R2
RD1     GLO     R7
        SM
        LBNF    RD2
        PLO     R7
        LBR     RD1
RD2     GLO     R7
        SEP     R5
MAPA
        LDI     HIGH (MAP)
        PHI     RA
        LDI     LOW (MAP)
        PLO     RA
        GLO     RE
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     RE
        PLO     R7
MA1     GLO     R7
        LBZ     MA2
        DEC     R7
        GLO     RA
        ADI     30
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LBR     MA1
MA2     SEP     R5
INIT
        SEP     R4
        WORD    VIDINIT
        SEP     R4
        WORD    CHSAVE
        SEP     R4
        WORD    CHLOAD
        SEP     R4
        WORD    CLRSCR
        SEP     R4
        WORD    VIDON
        LDI     HIGH (RNDV)
        PHI     RA
        LDI     LOW (RNDV)
        PLO     RA
        LDI     0ACH
        STR     RA
        INC     RA
        LDI     0E1H
        STR     RA
TITLE
        LDI     HIGH (SAVESP)
        PHI     RA
        LDI     LOW (SAVESP)
        PLO     RA
        LDA     RA
        PLO     R2
        LDN     RA
        PHI     R2
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     3
        PHI     RE
        LDI     15
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_TITLE)
        PHI     RC
        LDI     LOW (T_TITLE)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     5
        PHI     RE
        LDI     11
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_SUB)
        PHI     RC
        LDI     LOW (T_SUB)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     7
        PHI     RE
        LDI     12
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_BY)
        PHI     RC
        LDI     LOW (T_BY)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     9
        PHI     RE
        LDI     6
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_M0)
        PHI     RC
        LDI     LOW (T_M0)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     6
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_M1)
        PHI     RC
        LDI     LOW (T_M1)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     13
        PHI     RE
        LDI     6
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_M2)
        PHI     RC
        LDI     LOW (T_M2)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     15
        PHI     RE
        LDI     6
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_M3)
        PHI     RC
        LDI     LOW (T_M3)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     19
        PHI     RE
        LDI     6
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_ESC)
        PHI     RC
        LDI     LOW (T_ESC)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
TM1
        SEP     R4
        WORD    FRAME
        LDI     HIGH (RNDV)
        PHI     RA
        LDI     LOW (RNDV)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        BN3     TM1
        SEX     R2
        INP     3
        PLO     R9
        XRI     K_ESC
        LBZ     EXIT
        GLO     R9
        XRI     '0'
        LBZ     TMH
        GLO     R9
        SMI     31H
        LBNF    TM1
        SMI     3
        LBDF    TM1
        GLO     R9
        SMI     30H
        PLO     R9
        LDI     HIGH (SCEN)
        PHI     RA
        LDI     LOW (SCEN)
        PLO     RA
        GLO     R9
        STR     RA
        LDI     17
        PHI     RE
        LDI     6
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_LEV)
        PHI     RC
        LDI     LOW (T_LEV)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
TM2
        SEP     R4
        WORD    FRAME
        BN3     TM2
        SEX     R2
        INP     3
        PLO     R9
        SMI     31H
        LBNF    TM2
        SMI     5
        LBDF    TM2
        GLO     R9
        SMI     30H
        PLO     R9
        LDI     HIGH (LEVEL)
        PHI     RA
        LDI     LOW (LEVEL)
        PLO     RA
        GLO     R9
        STR     RA
GEN
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     10
        PHI     RE
        LDI     10
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (T_BUILD)
        PHI     RC
        LDI     LOW (T_BUILD)
        PLO     RC
        LDI     80H
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        LDI     HIGH (MAP)
        PHI     RA
        LDI     LOW (MAP)
        PLO     RA
        LDI     HIGH (0300H)
        PHI     RE
        LDI     LOW (0300H)
        PLO     RE
GC1     LDI     0
        STR     RA
        INC     RA
        DEC     RE
        GLO     RE
        LBNZ    GC1
        GHI     RE
        LBNZ    GC1
        LDI     30
        PLO     R9
        LDI     HIGH (SYSX+30)
        PHI     RA
        LDI     LOW (SYSX+30)
        PLO     RA
        LDI     30
        STR     RA
        LDI     HIGH (SYSY+30)
        PHI     RA
        LDI     LOW (SYSY+30)
        PLO     RA
        LDI     22
        STR     RA
        LDI     HIGH (SYST+30)
        PHI     RA
        LDI     LOW (SYST+30)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (SYSD+30)
        PHI     RA
        LDI     LOW (SYSD+30)
        PLO     RA
        LDI     2
        STR     RA
        LDI     HIGH (SYSN+30)
        PHI     RA
        LDI     LOW (SYSN+30)
        PLO     RA
        LDI     9
        STR     RA
        LDI     21
        PHI     RE
        LDI     29
        PLO     RE
        SEP     R4
        WORD    MAPA
        LDI     30
        STR     RA
        LDI     1
        PLO     R9
GS1     LDI     30
        SEP     R4
        WORD    RANDN
        PHI     R9
        LDI     22
        SEP     R4
        WORD    RANDN
        PHI     RE
        GHI     R9
        PLO     RE
        SEP     R4
        WORD    MAPA
        LDN     RA
        LBNZ    GS1
        GLO     R9
        STR     RA
        LDI     HIGH (SYSX)
        PHI     RA
        LDI     LOW (SYSX)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R9
        ADI     1
        STR     RA
        LDI     HIGH (SYSY)
        PHI     RA
        LDI     LOW (SYSY)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     RE
        ADI     1
        STR     RA
        LDI     6
        SEP     R4
        WORD    RANDN
        ADI     1
        PHI     R9
        LDI     HIGH (SYST)
        PHI     RA
        LDI     LOW (SYST)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R9
        STR     RA
        LDI     6
        SEP     R4
        WORD    RANDN
        PHI     R9
        LDI     HIGH (SYSD)
        PHI     RA
        LDI     LOW (SYSD)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R9
        STR     RA
        LDI     11
        SEP     R4
        WORD    RANDN
        ADI     1
        PHI     R9
        LDI     HIGH (SYSN)
        PHI     RA
        LDI     LOW (SYSN)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R9
        STR     RA
        INC     R9
        GLO     R9
        XRI     30
        LBNZ    GS1
        LDI     HIGH (SHIPX)
        PHI     RA
        LDI     LOW (SHIPX)
        PLO     RA
        LDI     30
        STR     RA
        INC     RA
        LDI     22
        STR     RA
        INC     RA
        LDI     30
        STR     RA
        SEP     R4
        WORD    NEWGAME
        LBR     CHART
TMH
        SEP     R4
        WORD    HELP
        LBR     TITLE
SCR     EQU     0A312H
NPL     EQU     0A313H
NAL     EQU     0A314H
ALR     EQU     0A315H
GLY     EQU     0A316H
CO      EQU     0A317H
WPN     EQU     0A318H
BSCR    EQU     0A319H
RKF     EQU     0A31AH
EVT     EQU     0A31BH
SNDT    EQU     0A31CH
APTR    EQU     0A31DH
ORB     EQU     0A31EH
TMP     EQU     0A31FH
FRM     EQU     0A3C0H
T1      EQU     0A3C2H
T2      EQU     0A3C4H
SCORE   EQU     0A3C6H
CMODE   EQU     0A3C8H
CMINX   EQU     0A3C9H
CMAXX   EQU     0A3CAH
CMINY   EQU     0A3CBH
CMAXY   EQU     0A3CCH
PSYS    EQU     0A3CDH
PPL     EQU     0A3CEH
SUMD    EQU     0A3CFH
TMP2    EQU     0A3D0H
NUMV    EQU     0A3D1H
PGI     EQU     0A3D2H
ICNT    EQU     0A3D3H
WARPC   EQU     0A3D4H
DEST    EQU     0A3D6H
SNDS    EQU     0A3E0H
SNDW    EQU     0A3E2H
NSDT    EQU     0A3E4H
SYSF    EQU     0A900H
SYSG    EQU     0A920H
SE      EQU     0A940H
SDM     EQU     0A960H
SAL     EQU     0A970H
SRP     EQU     0A980H
TORP    EQU     0A990H
AEN     EQU     0A9A0H
ASC     EQU     0A9B0H
AROW    EQU     0A9B8H
ACOL    EQU     0A9C0H
BST     EQU     0AA00H
PLAN    EQU     09A00H
; SUB16 - R8 = R8 - RC, DF = 1 when no borrow
SUB16   GLO     RC
        STR     R2
        GLO     R8
        SM
        PLO     R8
        GHI     RC
        STR     R2
        GHI     R8
        SMB
        PHI     R8
        SEP     R5
ADD16   GLO     RC
        STR     R2
        GLO     R8
        ADD
        PLO     R8
        GHI     RC
        STR     R2
        GHI     R8
        ADC
        PHI     R8
        SEP     R5
; CMP16 - DF = 1 when R8 >= RC (R8 kept)
CMP16   GLO     RC
        STR     R2
        GLO     R8
        SM
        GHI     RC
        STR     R2
        GHI     R8
        SMB
        SEP     R5
PNUM16  LDI     0
        PHI     RE
        LDI     HIGH (PV10K)
        PHI     RC
        LDI     LOW (PV10K)
        PLO     RC
PN1     LDA     RC
        PHI     R7
        LDA     RC
        PLO     R7
        LBNZ    PN2
        GHI     R7
        LBZ     PNE
PN2     LDI     0
        PLO     RE
PN3     GLO     R7
        STR     R2
        GLO     R8
        SM
        PHI     RF
        GHI     R7
        STR     R2
        GHI     R8
        SMB
        LBNF    PN4
        PHI     R8
        GHI     RF
        PLO     R8
        INC     RE
        LBR     PN3
PN4     GLO     RE
        LBNZ    PN5
        GHI     RE
        LBNZ    PN5
        GHI     R7
        LBNZ    PN1
        GLO     R7
        XRI     1
        LBNZ    PN1
PN5     GLO     RE
        ADI     30H
        STR     R2
        GLO     RF
        OR
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     1
        PHI     RE
        LBR     PN1
PNE     SEP     R5
PV10K   WORD    10000, 1000, 100, 10, 1, 0
PENRG   GLO     R8
        LBNZ    PE1
        GHI     R8
        LBNZ    PE1
        LDI     30H
        STR     R2
        GLO     RF
        OR
        SEP     R4
        WORD    PUTC
        INC     RD
        SEP     R5
PE1
        SEP     R4
        WORD    PNUM16
        LDI     30H
        STR     R2
        GLO     RF
        OR
        SEP     R4
        WORD    PUTC
        INC     RD
        SEP     R5
PSCORE
        LDI     HIGH (SCORE)
        PHI     RA
        LDI     LOW (SCORE)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        GHI     R8
        ANI     80H
        LBZ     PSC1
        LDI     2DH
        STR     R2
        GLO     RF
        OR
        SEP     R4
        WORD    PUTC
        INC     RD
        GLO     R8
        SDI     0
        PLO     R8
        GHI     R8
        SDBI    0
        PHI     R8
PSC1
        SEP     R4
        WORD    PNUM16
        SEP     R5
ADDSC   GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     HIGH (SCORE)
        PHI     RA
        LDI     LOW (SCORE)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        SEP     R4
        WORD    ADD16
        LDI     HIGH (SCORE)
        PHI     RA
        LDI     LOW (SCORE)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        SEP     R5
; MULT - R8 = RE.0 * RE.1 (uses R7)
MULT    LDI     0
        PLO     R8
        PHI     R8
        GHI     RE
        PLO     R7
MU1     GLO     R7
        LBZ     MUX
        DEC     R7
        GLO     RE
        STR     R2
        GLO     R8
        ADD
        PLO     R8
        GHI     R8
        ADCI    0
        PHI     R8
        LBR     MU1
MUX     SEP     R5
SOUND
        PLO     R7
        LDI     HIGH (SNDT)
        PHI     RA
        LDI     LOW (SNDT)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (SNDW)
        PHI     RA
        LDI     LOW (SNDW)
        PLO     RA
        GHI     RC
        STR     RA
        INC     RA
        GLO     RC
        STR     RA
        SEX     RC
        OUT     4
        SEX     R2
        SEP     R5
NSOUND
        PLO     R7
        LDI     HIGH (NSDT)
        PHI     RA
        LDI     LOW (NSDT)
        PLO     RA
        GLO     R7
        STR     RA
        SEX     RC
        OUT     5
        SEX     R2
        SEP     R5
FRAME2
        SEP     R4
        WORD    FRAME
        LDI     HIGH (FRM)
        PHI     RA
        LDI     LOW (FRM)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        INC     R8
        LDI     HIGH (FRM)
        PHI     RA
        LDI     LOW (FRM)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        LDI     HIGH (SNDT)
        PHI     RA
        LDI     LOW (SNDT)
        PLO     RA
        LDN     RA
        LBZ     F2N
        SMI     1
        STR     RA
        LBNZ    F2S
        LDI     HIGH (R4OFF)
        PHI     RF
        LDI     LOW (R4OFF)
        PLO     RF
        SEX     RF
        OUT     4
        SEX     R2
        LBR     F2N
F2S
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDN     RA
        LBZ     F2N
        STR     R2
        LDI     HIGH (SNDW)
        PHI     RA
        LDI     LOW (SNDW)
        PLO     RA
        LDN     RA
        ADD
        ANI     7FH
        STR     RA
        PHI     RF
        INC     RA
        LDN     RA
        PLO     RF
        SEX     RF
        OUT     4
        SEX     R2
F2N
        LDI     HIGH (NSDT)
        PHI     RA
        LDI     LOW (NSDT)
        PLO     RA
        LDN     RA
        LBZ     F2X
        SMI     1
        STR     RA
        LBNZ    F2X
        LDI     HIGH (R5NORM)
        PHI     RF
        LDI     LOW (R5NORM)
        PLO     RF
        SEX     RF
        OUT     5
        SEX     R2
F2X     SEP     R5
PADDR
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        SMI     1
        PLO     RE
        LDI     11
        PHI     RE
        SEP     R4
        WORD    MULT
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        SMI     1
        STR     R2
        GLO     R8
        ADD
        PLO     R8
        GHI     R8
        ADCI    0
        PHI     R8
        GLO     R8
        SHL
        PLO     R8
        GHI     R8
        SHLC
        PHI     R8
        GLO     R8
        SHL
        PLO     R8
        GHI     R8
        SHLC
        PHI     R8
        GLO     R8
        SHL
        PLO     R8
        GHI     R8
        SHLC
        PHI     R8
        GLO     R8
        PLO     RA
        GHI     R8
        ADI     HIGH PLAN
        PHI     RA
        SEP     R5
PGEN
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSG)
        PHI     RA
        LDI     LOW (SYSG)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBNZ    PGX
        LDI     1
        STR     RA
        LDI     HIGH (TMP2)
        PHI     RA
        LDI     LOW (TMP2)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSN)
        PHI     RA
        LDI     LOW (SYSN)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBZ     PGX
        PLO     R7
        LDI     HIGH (ICNT)
        PHI     RA
        LDI     LOW (ICNT)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDI     1
        STR     RA
PG1
        SEP     R4
        WORD    PADDR
        GLO     RA
        PLO     RD
        GHI     RA
        PHI     RD
        LDI     0
        STR     RD
        INC     RD
        LDI     0
        STR     RD
        INC     RD
        LDI     0
        STR     RD
        INC     RD
        LDI     0
        STR     RD
        INC     RD
        LDI     0
        STR     RD
        INC     RD
        LDI     0
        STR     RD
        INC     RD
        LDI     0
        STR     RD
        INC     RD
        LDI     0
        STR     RD
        INC     RD
        GLO     RD
        SMI     8
        PLO     RD
        GHI     RD
        SMBI    0
        PHI     RD
        LDI     2
        SEP     R4
        WORD    RANDN
        ADI     1
        STR     RD
        INC     RD
        LDI     5
        SEP     R4
        WORD    RANDN
        ADI     1
        STR     RD
        DEC     RD
        LDN     RD
        XRI     2
        LBNZ    PG2
        INC     RD
        INC     RD
        LDI     4
        STR     RD
        INC     RD
        LDI     8
        STR     RD
        LBR     PGN
PG2     INC     RD
        INC     RD
        LDI     3
        SEP     R4
        WORD    RANDN
        ADI     1
        STR     RD
        INC     RD
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYST)
        PHI     RA
        LDI     LOW (SYST)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (STEMP)
        PHI     RA
        LDI     LOW (STEMP)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R7
        ADD
        SMI     3
        LBDF    PG3
        LDI     1
PG3     LBNZ    PG3A
        LDI     1
PG3A    SMI     8
        LBNF    PG3B
        LDI     7
        LBR     PG3C
PG3B    ADI     8
PG3C    STR     RD
        SMI     3
        LBNF    PGN
        LDN     RD
        SMI     6
        LBDF    PGN
        DEC     RD
        DEC     RD
        LDN     RD
        INC     RD
        INC     RD
        SMI     4
        LBDF    PGN
PG4     LDI     8
        SEP     R4
        WORD    RANDN
        PLO     R9
        LDI     0
        PHI     R9
        GLO     R9
        LBZ     PG5
        LDI     5
        SEP     R4
        WORD    RANDN
        ADI     1
        PHI     R9
PG5
        LDI     HIGH (SCEN)
        PHI     RA
        LDI     LOW (SCEN)
        PLO     RA
        LDN     RA
        XRI     2
        LBNZ    PG6
        GHI     R9
        LBZ     PG5A
        LDI     4
        SEP     R4
        WORD    RANDN
        ADI     4
        PLO     R9
PG5A    GHI     R9
        SMI     3
        LBNF    PG4
PG6
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYST)
        PHI     RA
        LDI     LOW (SYST)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     3
        LBDF    PG7
        GLO     R9
        SMI     6
        LBDF    PG7
        LDI     0
        PLO     R9
        PHI     R9
PG7
        LDI     HIGH (SCEN)
        PHI     RA
        LDI     LOW (SCEN)
        PLO     RA
        LDN     RA
        XRI     3
        LBNZ    PG8
        GHI     R9
        LBZ     PG8
        ADI     10
        PHI     R9
PG8     INC     RD
        GLO     R9
        STR     RD
        INC     RD
        GHI     R9
        STR     RD
        GLO     R9
        LBZ     PGN
        LDI     HIGH (TMP2)
        PHI     RA
        LDI     LOW (TMP2)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
PGN
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        LDI     HIGH (ICNT)
        PHI     RA
        LDI     LOW (ICNT)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    PG1
        LDI     HIGH (SCEN)
        PHI     RA
        LDI     LOW (SCEN)
        PLO     RA
        LDN     RA
        XRI     1
        LBNZ    PGX
        LDI     HIGH (TMP2)
        PHI     RA
        LDI     LOW (TMP2)
        PLO     RA
        LDN     RA
        LBNZ    PGX
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSN)
        PHI     RA
        LDI     LOW (SYSN)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (ICNT)
        PHI     RA
        LDI     LOW (ICNT)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDI     1
        STR     RA
PGK
        SEP     R4
        WORD    PADDR
        LDI     10
        SEP     R4
        WORD    RANDN
        SMI     2
        LBDF    PGK1
        LDI     0
PGK1    PLO     R7
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GLO     R7
        STR     RA
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        LDI     HIGH (ICNT)
        PHI     RA
        LDI     LOW (ICNT)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    PGK
PGX     SEP     R5
STEMP   BYTE    0, 1, 2, 3, 3, 4, 5
GETKEY  BN3     GKN
        LBR     GK1
GKN
        LDI     HIGH (LASTK)
        PHI     RA
        LDI     LOW (LASTK)
        PLO     RA
        LDN     RA
        LBZ     GKX
        BN2     GKCLR
        LDI     HIGH (RPT)
        PHI     RB
        LDI     LOW (RPT)
        PLO     RB
        LDN     RB
        SMI     1
        STR     RB
        LBNZ    GKX
        LDI     4
        STR     RB
        LDN     RA
        SEP     R5
GKCLR   LDI     0
        STR     RA
GKX     LDI     0
        SEP     R5
GK1     SEX     R2
        INP     3
        PLO     R7
        SMI     61H
        LBNF    GK2
        GLO     R7
        SMI     7BH
        LBDF    GK2
        GLO     R7
        SMI     20H
        PLO     R7
GK2
        LDI     HIGH (LASTK)
        PHI     RA
        LDI     LOW (LASTK)
        PLO     RA
        LDI     0
        STR     RA
        GLO     R7
        SMI     82H
        LBNF    GK3
        SMI     4
        LBDF    GK3
        GLO     R7
        STR     RA
        LDI     HIGH (RPT)
        PHI     RB
        LDI     LOW (RPT)
        PLO     RB
        LDI     12
        STR     RB
GK3     GLO     R7
        SEP     R5
WKEY
        SEP     R4
        WORD    FRAME2
        SEP     R4
        WORD    GETKEY
        LBZ     WKEY
        SEP     R5
CURGET
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        SM
        PHI     RE
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        SM
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (UNDER)
        PHI     RA
        LDI     LOW (UNDER)
        PLO     RA
        LDN     RD
        STR     RA
        SEP     R5
CURPUT
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        SM
        PHI     RE
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        SM
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (UNDER)
        PHI     RA
        LDI     LOW (UNDER)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PUTC
        SEP     R5
CURDRAW
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        SM
        PHI     RE
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        SM
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     0ABH
        SEP     R4
        WORD    PUTC
        SEP     R5
CMOVE   PLO     R9
        SEP     R4
        WORD    CURPUT
        GLO     R9
        XRI     K_UP
        LBNZ    CMUPN
        LDI     HIGH (CMINY)
        PHI     RA
        LDI     LOW (CMINY)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    CMUPA
        LDI     HIGH (CMAXY)
        PHI     RB
        LDI     LOW (CMAXY)
        PLO     RB
        LDN     RB
        STR     RA
        LBR     CMD
CMUPA   LDN     RA
        SMI     1
        STR     RA
        LBR     CMD
CMUPN
        GLO     R9
        XRI     K_DOWN
        LBNZ    CMDOWNN
        LDI     HIGH (CMAXY)
        PHI     RA
        LDI     LOW (CMAXY)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    CMDOWNA
        LDI     HIGH (CMINY)
        PHI     RB
        LDI     LOW (CMINY)
        PLO     RB
        LDN     RB
        STR     RA
        LBR     CMD
CMDOWNA   LDN     RA
        ADI     1
        STR     RA
        LBR     CMD
CMDOWNN
        GLO     R9
        XRI     K_LEFT
        LBNZ    CMLEFTN
        LDI     HIGH (CMINX)
        PHI     RA
        LDI     LOW (CMINX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    CMLEFTA
        LDI     HIGH (CMAXX)
        PHI     RB
        LDI     LOW (CMAXX)
        PLO     RB
        LDN     RB
        STR     RA
        LBR     CMD
CMLEFTA   LDN     RA
        SMI     1
        STR     RA
        LBR     CMD
CMLEFTN
        GLO     R9
        XRI     K_RIGHT
        LBNZ    CMRIGHTN
        LDI     HIGH (CMAXX)
        PHI     RA
        LDI     LOW (CMAXX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    CMRIGHTA
        LDI     HIGH (CMINX)
        PHI     RB
        LDI     LOW (CMINX)
        PLO     RB
        LDN     RB
        STR     RA
        LBR     CMD
CMRIGHTA   LDN     RA
        ADI     1
        STR     RA
        LBR     CMD
CMRIGHTN
CMD
        SEP     R4
        WORD    CURGET
        SEP     R4
        WORD    CURDRAW
        SEP     R5
BORDER  LDI     0
        PLO     R9
BO1     GLO     R9
        PHI     RE
        LDI     30
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     07H
        STR     RD
        INC     R9
        GLO     R9
        XRI     22
        LBNZ    BO1
        LDI     22
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     40
        PLO     R9
BO2     LDI     08H
        STR     RD
        INC     RD
        DEC     R9
        GLO     R9
        LBNZ    BO2
        LDI     22
        PHI     RE
        LDI     30
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     09H
        STR     RD
        SEP     R5
NAMEPTR
        LDI     HIGH (NAMET)
        PHI     RA
        LDI     LOW (NAMET)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     RC
        LDN     RA
        PLO     RC
        SEP     R5
STRN    PLO     R7
SN1     GLO     R7
        LBZ     SNX
SN2     LDA     RC
        LBNZ    SN2
        DEC     R7
        LBR     SN1
SNX     SEP     R5
CURSYS
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        SMI     1
        PHI     RE
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        SMI     1
        PLO     RE
        SEP     R4
        WORD    MAPA
        LDN     RA
        SEP     R5
PORBIT
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R9
        SEP     R4
        WORD    NAMEPTR
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     0A0H
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (ROMAN)
        PHI     RC
        LDI     LOW (ROMAN)
        PLO     RC
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        SMI     1
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R5
CHART
        LDI     HIGH (SHIPX)
        PHI     RA
        LDI     LOW (SHIPX)
        PLO     RA
        LDI     HIGH (CURX)
        PHI     RB
        LDI     LOW (CURX)
        PLO     RB
        LDA     RA
        STR     RB
        INC     RB
        LDN     RA
        STR     RB
CHDRAW
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        SEP     R4
        WORD    BORDER
        LDI     1
        PLO     R9
CH1
        LDI     HIGH (SYSY)
        PHI     RA
        LDI     LOW (SYSY)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        PHI     RE
        LDI     HIGH (SYSX)
        PHI     RA
        LDI     LOW (SYSX)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (SYST)
        PHI     RA
        LDI     LOW (SYST)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (STARC)
        PHI     RA
        LDI     LOW (STARC)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        STR     RD
        INC     R9
        GLO     R9
        XRI     31
        LBNZ    CH1
        LDI     0
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S0)
        PHI     RC
        LDI     LOW (S0)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     1
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S1)
        PHI     RC
        LDI     LOW (S1)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     3
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S2)
        PHI     RC
        LDI     LOW (S2)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     HIGH (SHIPX)
        PHI     RA
        LDI     LOW (SHIPX)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     0ACH
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (SHIPY)
        PHI     RA
        LDI     LOW (SHIPY)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     6
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S3)
        PHI     RC
        LDI     LOW (S3)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     12
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S4)
        PHI     RC
        LDI     LOW (S4)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     13
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S5)
        PHI     RC
        LDI     LOW (S5)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     14
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S6)
        PHI     RC
        LDI     LOW (S6)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     23
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S7)
        PHI     RC
        LDI     LOW (S7)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBZ     CH2
        LDI     16
        PHI     RE
        LDI     32
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S8)
        PHI     RC
        LDI     LOW (S8)
        PLO     RC
        LDI     130
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     17
        PHI     RE
        LDI     32
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S9)
        PHI     RC
        LDI     LOW (S9)
        PLO     RC
        LDI     130
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     CH2A
CH2
        LDI     16
        PHI     RE
        LDI     32
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S10)
        PHI     RC
        LDI     LOW (S10)
        PLO     RC
        LDI     1
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     17
        PHI     RE
        LDI     32
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S9)
        PHI     RC
        LDI     LOW (S9)
        PLO     RC
        LDI     1
        PLO     RF
        SEP     R4
        WORD    PRT
CH2A
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (CMINX)
        PHI     RA
        LDI     LOW (CMINX)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (CMAXX)
        PHI     RA
        LDI     LOW (CMAXX)
        PLO     RA
        LDI     30
        STR     RA
        LDI     HIGH (CMINY)
        PHI     RA
        LDI     LOW (CMINY)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (CMAXY)
        PHI     RA
        LDI     LOW (CMAXY)
        PLO     RA
        LDI     22
        STR     RA
        SEP     R4
        WORD    CURGET
        SEP     R4
        WORD    CURDRAW
        SEP     R4
        WORD    PANUPD
        SEP     R4
        WORD    VIDON
        LDI     HIGH (LASTK)
        PHI     RA
        LDI     LOW (LASTK)
        PLO     RA
        LDI     0
        STR     RA
CLOOP
        SEP     R4
        WORD    FRAME2
        SEP     R4
        WORD    TICK
        XRI     2
        LBZ     CHDRAW
        SEP     R4
        WORD    GETKEY
        LBZ     CLOOP
        PLO     R9
        ANI     0FCH
        XRI     84H
        LBZ     CLMV
        GLO     R9
        XRI     K_UP
        LBZ     CLMV
        GLO     R9
        XRI     K_RIGHT
        LBZ     CLMV
        GLO     R9
        XRI     'W'
        LBZ     WARP
        GLO     R9
        XRI     K_CR
        LBZ     WARP
        GLO     R9
        XRI     'F'
        LBZ     INFO
        GLO     R9
        XRI     'C'
        LBZ     CHART
        GLO     R9
        XRI     'S'
        LBZ     COMP
        GLO     R9
        XRI     'B'
        LBZ     BATTLE
        GLO     R9
        XRI     'I'
        LBZ     IMPULSE
        GLO     R9
        XRI     'M'
        LBZ     IMPULSE
        LBR     CLOOP
CLMV    GLO     R9
        SEP     R4
        WORD    CMOVE
        SEP     R4
        WORD    PANUPD
        LBR     CLOOP
PANUPD
        LDI     7
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S11)
        PHI     RC
        LDI     LOW (S11)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     7
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     0ACH
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     9
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S11)
        PHI     RC
        LDI     LOW (S11)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S11)
        PHI     RC
        LDI     LOW (S11)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    CURSYS
        LBZ     PUX
        PLO     R9
        LDI     9
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        SEP     R4
        WORD    NAMEPTR
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S12)
        PHI     RC
        LDI     LOW (S12)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SYST)
        PHI     RA
        LDI     LOW (SYST)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (STARL)
        PHI     RA
        LDI     LOW (STARL)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        ORI     80H
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (SYSD)
        PHI     RA
        LDI     LOW (SYSD)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        ADI     0B0H
        SEP     R4
        WORD    PUTC
PUX     SEP     R5
INFO
        SEP     R4
        WORD    CURSYS
        LBZ     CLOOP
        PLO     R9
        PLO     R7
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    PGEN
INF0
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        SEP     R4
        WORD    INFHDR
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSF)
        PHI     RA
        LDI     LOW (SYSF)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBZ     INF1
        LDI     8
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S13)
        PHI     RC
        LDI     LOW (S13)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    INFKEY
        LBR     CHDRAW
INF1
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDI     1
        STR     RA
INF2
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        SEP     R4
        WORD    INFHDR
        LDI     8
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R9
        SEP     R4
        WORD    NAMEPTR
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     0A0H
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (ROMAN)
        PHI     RC
        LDI     LOW (ROMAN)
        PLO     RC
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        SMI     1
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S14)
        PHI     RC
        LDI     LOW (S14)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     0
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        PLO     R9
        LDI     10
        PHI     RE
        LDI     16
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (CATT)
        PHI     RC
        LDI     LOW (CATT)
        PLO     RC
        GLO     R9
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S15)
        PHI     RC
        LDI     LOW (S15)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     16
        PLO     RE
        SEP     R4
        WORD    POS
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     2
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        PLO     R9
        LDI     HIGH (DENT)
        PHI     RC
        LDI     LOW (DENT)
        PLO     RC
        GLO     R9
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     0ACH
        SEP     R4
        WORD    PUTC
        INC     RD
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     1
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        SHL
        PLO     R9
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     0
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        STR     R2
        GLO     R9
        ADD
        PLO     R9
        LDI     HIGH (ATMT)
        PHI     RC
        LDI     LOW (ATMT)
        PLO     RC
        GLO     R9
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     12
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S16)
        PHI     RC
        LDI     LOW (S16)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     12
        PHI     RE
        LDI     16
        PLO     RE
        SEP     R4
        WORD    POS
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     3
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        PLO     R9
        LDI     HIGH (TEMT)
        PHI     RC
        LDI     LOW (TEMT)
        PLO     RC
        GLO     R9
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     0
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     1
        LBNZ    INF3
        LDI     14
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S17)
        PHI     RC
        LDI     LOW (S17)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     14
        PHI     RE
        LDI     16
        PLO     RE
        SEP     R4
        WORD    POS
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R9
        LDI     HIGH (CIVT)
        PHI     RC
        LDI     LOW (CIVT)
        PLO     RC
        GLO     R9
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBZ     INF3
        LDI     15
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S18)
        PHI     RC
        LDI     LOW (S18)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     15
        PHI     RE
        LDI     16
        PLO     RE
        SEP     R4
        WORD    POS
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R9
        SMI     10
        LBNF    INF2A
        LDI     0
        PLO     R9
INF2A
        LDI     HIGH (ATTT)
        PHI     RC
        LDI     LOW (ATTT)
        PLO     RC
        GLO     R9
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
INF3
        SEP     R4
        WORD    INFKEY
        XRI     'C'
        LBZ     CHDRAW
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSN)
        PHI     RA
        LDI     LOW (SYSN)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        SD
        LBDF    INF2
        LBR     CHDRAW
INFHDR
        LDI     1
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S19)
        PHI     RC
        LDI     LOW (S19)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     3
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R9
        SEP     R4
        WORD    NAMEPTR
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (S20)
        PHI     RC
        LDI     LOW (S20)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     5
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S21)
        PHI     RC
        LDI     LOW (S21)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYST)
        PHI     RA
        LDI     LOW (SYST)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (STARL)
        PHI     RA
        LDI     LOW (STARL)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        ORI     80H
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSD)
        PHI     RA
        LDI     LOW (SYSD)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        ADI     0B0H
        SEP     R4
        WORD    PUTC
        LDI     6
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S22)
        PHI     RC
        LDI     LOW (S22)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSN)
        PHI     RA
        LDI     LOW (SYSN)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R8
        LDI     128
        PLO     RF
        GLO     R8
        SEP     R4
        WORD    PNUM
        SEP     R5
INFKEY
        LDI     22
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S23)
        PHI     RC
        LDI     LOW (S23)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    WKEY
        SEP     R5
WARP
        SEP     R4
        WORD    CURSYS
        LBZ     CLOOP
        PLO     R9
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R9
        XOR
        LBZ     CLOOP
        GLO     R9
        PLO     R7
        LDI     HIGH (DEST)
        PHI     RA
        LDI     LOW (DEST)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (SHIPX)
        PHI     RA
        LDI     LOW (SHIPX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        SM
        LBDF    WA1
        SDI     0
WA1     PLO     RE
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     HIGH (SHIPY)
        PHI     RA
        LDI     LOW (SHIPY)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        SM
        LBDF    WA2
        SDI     0
WA2     PLO     RE
        PHI     RE
        SEP     R4
        WORD    MULT
        SEP     R4
        WORD    ADD16
        GLO     R8
        PLO     RD
        GHI     R8
        PHI     RD
        LDI     0
        PLO     R9
WA3     GLO     R9
        ADI     1
        PLO     RE
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     RD
        PLO     RC
        GHI     RD
        PHI     RC
        GLO     RC
        STR     R2
        GLO     R8
        SD
        GHI     RC
        STR     R2
        GHI     R8
        SDB
        LBNF    WA4
        INC     R9
        LBR     WA3
WA4     GLO     R9
        PLO     RE
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        STR     R2
        GLO     RD
        SM
        PLO     RD
        GHI     R8
        STR     R2
        GHI     RD
        SMB
        PHI     RD
        GLO     RD
        PLO     RE
        LDI     100
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        PLO     RD
        GHI     R8
        PHI     RD
        LDI     9
        PHI     R9
WA5     GHI     R9
        PLO     RE
        LDI     20
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        PLO     RE
        GLO     R9
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        GHI     R9
        PLO     RE
        PHI     RE
        SEP     R4
        WORD    MULT
        SEP     R4
        WORD    ADD16
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        GLO     RD
        PLO     R8
        GHI     RD
        PHI     R8
        SEP     R4
        WORD    CMP16
        LBDF    WA6
        GHI     R9
        SMI     1
        PHI     R9
        LBR     WA5
WA6     GLO     R9
        PLO     RE
        LDI     10
        PHI     RE
        SEP     R4
        WORD    MULT
        GHI     R9
        STR     R2
        GLO     R8
        ADD
        PLO     RE
        LDI     20
        PHI     RE
        SEP     R4
        WORD    MULT
        LDI     HIGH (WARPC)
        PHI     RA
        LDI     LOW (WARPC)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     2
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S24)
        PHI     RC
        LDI     LOW (S24)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (DEST)
        PHI     RA
        LDI     LOW (DEST)
        PLO     RA
        LDN     RA
        PLO     R9
        LDI     HIGH (DEST)
        PHI     RA
        LDI     LOW (DEST)
        PLO     RA
        LDN     RA
        STXD
        SEP     R4
        WORD    NAMEPTR
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (S25)
        PHI     RC
        LDI     LOW (S25)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     HIGH (WARPC)
        PHI     RA
        LDI     LOW (WARPC)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        IRX
        LDI     6
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S26)
        PHI     RC
        LDI     LOW (S26)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (WARPC)
        PHI     RA
        LDI     LOW (WARPC)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        SEP     R4
        WORD    PENRG
        LDI     HIGH (S27)
        PHI     RC
        LDI     LOW (S27)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S28)
        PHI     RC
        LDI     LOW (S28)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        SEP     R4
        WORD    PENRG
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        LDI     HIGH (WARPC)
        PHI     RA
        LDI     LOW (WARPC)
        PLO     RA
        LDA     RA
        PHI     RC
        LDN     RA
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBDF    WA7
        LDI     11
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S29)
        PHI     RC
        LDI     LOW (S29)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    WKEY
        LBR     CHDRAW
WA7
        LDI     11
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S30)
        PHI     RC
        LDI     LOW (S30)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
WA8
        SEP     R4
        WORD    WKEY
        PLO     R9
        GLO     R9
        XRI     'N'
        LBZ     CHDRAW
        GLO     R9
        XRI     'Y'
        LBNZ    WA8
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        LDI     HIGH (WARPC)
        PHI     RA
        LDI     LOW (WARPC)
        PLO     RA
        LDA     RA
        PHI     RC
        LDN     RA
        PLO     RC
        SEP     R4
        WORD    SUB16
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     0
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     2
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     6
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     12
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     14
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     16
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     18
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     20
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S31)
        PHI     RC
        LDI     LOW (S31)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        LDI     48
        PLO     R9
WA9
        SEP     R4
        WORD    FRAME2
        GLO     R9
        ANI     07H
        ORI     R3ON
        STR     R2
        OUT     3
        DEC     R2
        GLO     R9
        SHL
        ADI     8
        PHI     RC
        LDI     2AH
        PLO     RC
        SEX     RC
        OUT     4
        SEX     R2
        DEC     R9
        GLO     R9
        LBNZ    WA9
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (0C00H+3FH)
        PHI     RC
        LDI     LOW (0C00H+3FH)
        PLO     RC
        LDI     10
        SEP     R4
        WORD    SOUND
        SEX     R3
        OUT     3
        BYTE    R3ON
        SEX     R2
        LDI     HIGH (DEST)
        PHI     RA
        LDI     LOW (DEST)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDI     HIGH (SHIPX)
        PHI     RB
        LDI     LOW (SHIPX)
        PLO     RB
        LDA     RA
        STR     RB
        INC     RB
        LDN     RA
        STR     RB
        SEP     R4
        WORD    ARRIVE
        LBR     CHART
ARRIVE
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    PGEN
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSN)
        PHI     RA
        LDI     LOW (SYSN)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (NPL)
        PHI     RA
        LDI     LOW (NPL)
        PLO     RA
        GLO     R7
        STR     RA
        SMI     3
        LBNF    AR1
        LDI     3
        LBR     AR2
AR1
        LDI     HIGH (NPL)
        PHI     RA
        LDI     LOW (NPL)
        PLO     RA
        LDN     RA
AR2
        PLO     R7
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SYSF)
        PHI     RA
        LDI     LOW (SYSF)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDI     0
        STR     RA
        SEP     R4
        WORD    NEWORB
        SEP     R4
        WORD    BSTARS
        SEP     R5
NEWORB
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (CO)
        PHI     RA
        LDI     LOW (CO)
        PLO     RA
        LDI     16
        STR     RA
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        LDI     8
        PLO     R7
NO1     LDI     0
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NO1
        SEP     R5
BSTARS
        LDI     HIGH (BST)
        PHI     RD
        LDI     LOW (BST)
        PLO     RD
        LDI     40
        PLO     R9
BS1     LDI     20
        SEP     R4
        WORD    RANDN
        ADI     1
        STR     RD
        INC     RD
        LDI     28
        SEP     R4
        WORD    RANDN
        ADI     1
        STR     RD
        INC     RD
        DEC     R9
        GLO     R9
        LBNZ    BS1
        SEP     R5
IMPULSE
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     1
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S32)
        PHI     RC
        LDI     LOW (S32)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     3
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S33)
        PHI     RC
        LDI     LOW (S33)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    PORBIT
        LDI     5
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S34)
        PHI     RC
        LDI     LOW (S34)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (NPL)
        PHI     RA
        LDI     LOW (NPL)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     7
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S35)
        PHI     RC
        LDI     LOW (S35)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (SE+4)
        PHI     RA
        LDI     LOW (SE+4)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        SEP     R4
        WORD    PENRG
        LDI     9
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S36)
        PHI     RC
        LDI     LOW (S36)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    GETNUM
        LBZ     COMP
        PLO     R9
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R9
        XOR
        LBZ     COMP
        LDI     HIGH (NPL)
        PHI     RA
        LDI     LOW (NPL)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R9
        SD
        LBNF    COMP
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R9
        SM
        LBDF    IM1
        SDI     0
IM1     PLO     RE
        LDI     100
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     HIGH (SE+4)
        PHI     RA
        LDI     LOW (SE+4)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        SEP     R4
        WORD    CMP16
        LBDF    IM2
        LDI     12
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S37)
        PHI     RC
        LDI     LOW (S37)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    WKEY
        LBR     COMP
IM2
        SEP     R4
        WORD    SUB16
        LDI     HIGH (SE+4)
        PHI     RA
        LDI     LOW (SE+4)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        GLO     R9
        PLO     R7
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    NEWORB
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     1
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S38)
        PHI     RC
        LDI     LOW (S38)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S38)
        PHI     RC
        LDI     LOW (S38)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     7
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S38)
        PHI     RC
        LDI     LOW (S38)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S38)
        PHI     RC
        LDI     LOW (S38)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     13
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S38)
        PHI     RC
        LDI     LOW (S38)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     16
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S38)
        PHI     RC
        LDI     LOW (S38)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     19
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S38)
        PHI     RC
        LDI     LOW (S38)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        LDI     30
        PLO     R9
IM3
        SEP     R4
        WORD    FRAME2
        GLO     R9
        SHL
        SHL
        ADI     40H
        PHI     RC
        LDI     1BH
        PLO     RC
        SEX     RC
        OUT     4
        SEX     R2
        DEC     R9
        GLO     R9
        LBNZ    IM3
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (0C00H+1FH)
        PHI     RC
        LDI     LOW (0C00H+1FH)
        PLO     RC
        LDI     6
        SEP     R4
        WORD    SOUND
        LBR     COMP
GETNUM  LDI     0
        PLO     R7
        LDI     HIGH (NUMV)
        PHI     RA
        LDI     LOW (NUMV)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (TMP2)
        PHI     RA
        LDI     LOW (TMP2)
        PLO     RA
        LDI     0
        STR     RA
GN1
        LDI     0DFH
        SEP     R4
        WORD    PUTC
        SEP     R4
        WORD    WKEY
        PLO     R9
        XRI     K_CR
        LBZ     GNX
        GLO     R9
        SMI     30H
        LBNF    GN1
        SMI     10
        LBDF    GN1
        LDI     HIGH (TMP2)
        PHI     RA
        LDI     LOW (TMP2)
        PLO     RA
        LDN     RA
        SMI     3
        LBDF    GN1
        LDI     HIGH (NUMV)
        PHI     RA
        LDI     LOW (NUMV)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     10
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R9
        SMI     30H
        STR     R2
        GLO     R8
        ADD
        PLO     R8
        GHI     R8
        ADCI    0
        LBZ     GN2
        LDI     0FFH
        PLO     R8
GN2     GLO     R8
        PLO     R7
        LDI     HIGH (NUMV)
        PHI     RA
        LDI     LOW (NUMV)
        PLO     RA
        GLO     R7
        STR     RA
        GLO     R9
        ORI     80H
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (TMP2)
        PHI     RA
        LDI     LOW (TMP2)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        LBR     GN1
GNX     LDI     0A0H
        SEP     R4
        WORD    PUTC
        LDI     HIGH (NUMV)
        PHI     RA
        LDI     LOW (NUMV)
        PLO     RA
        LDN     RA
        SEP     R5
TICK
        LDI     HIGH (EVT)
        PHI     RA
        LDI     LOW (EVT)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (FRM)
        PHI     RA
        LDI     LOW (FRM)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        LDI     HIGH (T1)
        PHI     RA
        LDI     LOW (T1)
        PLO     RA
        LDA     RA
        PHI     RC
        LDN     RA
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBNF    TKATT
        LDI     HIGH (1800)
        PHI     RC
        LDI     LOW (1800)
        PLO     RC
        SEP     R4
        WORD    ADD16
        LDI     HIGH (T1)
        PHI     RA
        LDI     LOW (T1)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        LDI     HIGH (EVT)
        PHI     RA
        LDI     LOW (EVT)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (SCEN)
        PHI     RA
        LDI     LOW (SCEN)
        PLO     RA
        LDN     RA
        XRI     1
        LBNZ    TK2
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBNZ    TK2
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RD
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDI     1
        STR     RA
TK1
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        XOR
        LBZ     TK1N
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        GLO     RD
        SDI     7
        PHI     RD
        GLO     R7
        STR     R2
        GHI     RD
        SM
        LBDF    TK1A
        GHI     RD
        LBR     TK1B
TK1A    GLO     R7
TK1B    PHI     RD
        GLO     R7
        STR     R2
        GHI     RD
        SD
        STR     RA
        GHI     RD
        STR     R2
        GLO     RD
        ADD
        PLO     RD
TK1N
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        PLO     R7
        LDI     HIGH (NPL)
        PHI     RA
        LDI     LOW (NPL)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R7
        SD
        LBDF    TK1
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        GLO     RD
        STR     RA
TK2
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        XRI     30
        LBNZ    TK3
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        XRI     3
        LBNZ    TK3
        LDI     HIGH (TORP)
        PHI     RA
        LDI     LOW (TORP)
        PLO     RA
        LDI     20
        STR     RA
        INC     RA
        STR     RA
        LDI     HIGH (SDM+1)
        PHI     RA
        LDI     LOW (SDM+1)
        PLO     RA
        LDI     11
        PLO     R7
TK2A    LDI     0
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    TK2A
TK3
        LDI     HIGH (SUMD)
        PHI     RA
        LDI     LOW (SUMD)
        PLO     RA
        LDI     0
        STR     RA
        LDI     1
        PLO     R9
TK4
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     10
        PHI     RE
        SEP     R4
        WORD    MULT
        LDI     1
        PLO     R7
TK4A    GLO     R8
        SMI     35
        PLO     R8
        GHI     R8
        SMBI    0
        PHI     R8
        LBNF    TK4B
        INC     R7
        LBR     TK4A
TK4B
        LDI     HIGH (SAL)
        PHI     RA
        LDI     LOW (SAL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     10
        PHI     RE
        GLO     R7
        STXD
        SEP     R4
        WORD    MULT
        IRX
        LDX
        PLO     R7
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     0
        PLO     R8
        PHI     R8
TK4C    GLO     R7
        STR     R2
        GLO     RC
        SM
        PLO     RC
        GHI     RC
        SMBI    0
        PHI     RC
        LBNF    TK4D
        INC     R8
        LBR     TK4C
TK4D    GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        SEP     R4
        WORD    ADD16
        GLO     R9
        XRI     5
        LBZ     TK4E
        GLO     R9
        XRI     6
        LBNZ    TK4F
TK4E    LDI     0
        PHI     R8
        LDI     50
        PLO     R8
TK4F
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        XRI     9
        LBNZ    TK4G
        LDI     0
        PLO     R8
        PHI     R8
TK4G
        LDI     HIGH (SUMD)
        PHI     RA
        LDI     LOW (SUMD)
        PLO     RA
        GLO     R7
        STR     R2
        LDN     RA
        ADD
        STR     RA
        GHI     R8
        SMI     27H
        LBNF    TK4H
        LBNZ    TK4I
        GLO     R8
        SMI     0ACH
        LBNF    TK4H
TK4I    LDI     26H
        PHI     R8
        LDI     0ACH
        PLO     R8
TK4H
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        INC     R9
        GLO     R9
        XRI     12
        LBNZ    TK4
        LDI     HIGH (LEVEL)
        PHI     RA
        LDI     LOW (LEVEL)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     10
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        ADI     39
        STR     R2
        LDI     HIGH (SUMD)
        PHI     RA
        LDI     LOW (SUMD)
        PLO     RA
        LDN     RA
        SD
        LBNF    DEAD
TK5
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDI     0
        STR     RA
        LDI     1
        PLO     R9
TK5A
        LDI     HIGH (SRP)
        PHI     RA
        LDI     LOW (SRP)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RD
        LDI     HIGH (SE+22)
        PHI     RA
        LDI     LOW (SE+22)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (60)
        PHI     RC
        LDI     LOW (60)
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBNF    TK6
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     RD
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBZ     TK5B
        XRI     9
        LBZ     TK5B
        LDN     RA
        SMI     1
        STR     RA
        LDI     HIGH (SE+22)
        PHI     RA
        LDI     LOW (SE+22)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (60)
        PHI     RC
        LDI     LOW (60)
        PLO     RC
        SEP     R4
        WORD    SUB16
        LDI     HIGH (SE+22)
        PHI     RA
        LDI     LOW (SE+22)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDI     1
        STR     RA
TK5B    INC     R9
        GLO     R9
        XRI     11
        LBNZ    TK5A
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        LBNZ    TK5
TK6
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     4
        LBNF    TK8
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     10
        LBNF    TK8
        LDI     HIGH (CO)
        PHI     RA
        LDI     LOW (CO)
        PLO     RA
        LDN     RA
        LBZ     TK6A
        SEP     R4
        WORD    RANDN
        ADI     1
        LBR     TK6B
TK6A    LDI     1
TK6B    PLO     R9
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     3
        PLO     RE
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        STR     R2
        GLO     R9
        SD
        LBNF    TKX
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     10
        STR     RA
        XRI     5
        LBZ     TK8
        SEP     R4
        WORD    CONTACT
        LDI     HIGH (EVT)
        PHI     RA
        LDI     LOW (EVT)
        PLO     RA
        LDI     2
        STR     RA
        LBR     TKX
TK8
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBNZ    TK8A
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     4
        LBZ     TK8B
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     5
        LBZ     TK8B
        LBR     TKATT
TK8A
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBNZ    TKATT
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (GLY)
        PHI     RA
        LDI     LOW (GLY)
        PLO     RA
        LDI     133
        STR     RA
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDI     3
        STR     RA
        LBR     TK9
TK8B
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBNZ    TKATT
        LDI     4
        SEP     R4
        WORD    RANDN
        ADI     4
        PLO     R7
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        GLO     R7
        STR     RA
        PLO     R9
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        GLO     R9
        STR     RA
        LDI     HIGH (GLY)
        PHI     RA
        LDI     LOW (GLY)
        PLO     RA
        LDI     134
        STR     RA
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     3
        LBZ     TK8C
        LBNF    TK8C
        LBR     TK8D
TK8C    LDI     3
TK8D
        PLO     R7
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        GLO     R7
        STR     RA
TK9     LDI     0
        PLO     R9
TK9A
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDI     4
        SEP     R4
        WORD    RANDN
        ADI     1
        STR     RA
        LDI     HIGH (AROW)
        PHI     RA
        LDI     LOW (AROW)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDI     18
        SEP     R4
        WORD    RANDN
        ADI     2
        STR     RA
        LDI     HIGH (ACOL)
        PHI     RA
        LDI     LOW (ACOL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDI     27
        SEP     R4
        WORD    RANDN
        ADI     2
        STR     RA
        LDI     50
        SEP     R4
        WORD    RANDN
        ADI     1
        STXD
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     100
        PHI     RE
        SEP     R4
        WORD    MULT
        IRX
        LDX
        STR     R2
        GLO     R8
        ADD
        PLO     R8
        GHI     R8
        ADCI    0
        PHI     R8
        LDI     HIGH (AEN)
        PHI     RA
        LDI     LOW (AEN)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        INC     R9
        GLO     R9
        STR     R2
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        SD
        LBNF    TK9A
        LDI     HIGH (FRM)
        PHI     RA
        LDI     LOW (FRM)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        LDI     HIGH (300)
        PHI     RC
        LDI     LOW (300)
        PLO     RC
        SEP     R4
        WORD    ADD16
        LDI     HIGH (T2)
        PHI     RA
        LDI     LOW (T2)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (1800H+3FH)
        PHI     RC
        LDI     LOW (1800H+3FH)
        PLO     RC
        LDI     30
        SEP     R4
        WORD    SOUND
        LDI     HIGH (EVT)
        PHI     RA
        LDI     LOW (EVT)
        PLO     RA
        LDI     2
        STR     RA
TKATT
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBZ     TKX
        LDI     HIGH (FRM)
        PHI     RA
        LDI     LOW (FRM)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        LDI     HIGH (T2)
        PHI     RA
        LDI     LOW (T2)
        PLO     RA
        LDA     RA
        PHI     RC
        LDN     RA
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBNF    TKX
        LDI     250
        SEP     R4
        WORD    RANDN
        PLO     RE
        LDI     2
        PHI     RE
        SEP     R4
        WORD    MULT
        LDI     HIGH (600)
        PHI     RC
        LDI     LOW (600)
        PLO     RC
        SEP     R4
        WORD    ADD16
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        PLO     R7
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     0
        PLO     R8
        PHI     R8
TA1     GLO     R7
        STR     R2
        GLO     RC
        SM
        PLO     RC
        GHI     RC
        SMBI    0
        PHI     RC
        LBNF    TA2
        INC     R8
        LBR     TA1
TA2     GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     HIGH (FRM)
        PHI     RA
        LDI     LOW (FRM)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        SEP     R4
        WORD    ADD16
        LDI     HIGH (T2)
        PHI     RA
        LDI     LOW (T2)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        LDI     HIGH (EVT)
        PHI     RA
        LDI     LOW (EVT)
        PLO     RA
        LDN     RA
        LBNZ    TA3
        LDI     HIGH (EVT)
        PHI     RA
        LDI     LOW (EVT)
        PLO     RA
        LDI     1
        STR     RA
TA3     LDI     8
        PLO     R7
TA4
        LDI     HIGH (APTR)
        PHI     RA
        LDI     LOW (APTR)
        PLO     RA
        LDN     RA
        ADI     1
        ANI     07H
        STR     RA
        PLO     R9
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBNZ    TA5
        DEC     R7
        GLO     R7
        LBNZ    TA4
        LBR     TKX
TA5     ADI     6
        PLO     R9
        LDI     5
        PLO     R8
        LDI     0
        PHI     R8
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDN     RA
        PLO     R7
TA6     GLO     R8
        SHL
        PLO     R8
        GHI     R8
        SHLC
        PHI     R8
        DEC     R7
        GLO     R7
        LBNZ    TA6
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        SEP     R4
        WORD    RANDN
        STR     R2
        GLO     RC
        ADD
        PLO     RC
        GHI     RC
        ADCI    0
        PHI     RC
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SHL
        PLO     RE
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDN     RA
        PHI     RE
        SEP     R4
        WORD    MULT
        SEP     R4
        WORD    ADD16
        GLO     R8
        PLO     RC
        GHI     R8
        PHI     RC
        LDI     HIGH (LEVEL)
        PHI     RA
        LDI     LOW (LEVEL)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     40
        PHI     RE
        SEP     R4
        WORD    MULT
        GLO     R8
        SEP     R4
        WORD    RANDN
        XRI     4
        LBNZ    TA7
        GLO     RC
        ADI     0E8H
        PLO     RC
        GHI     RC
        ADCI    3
        PHI     RC
TA7
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        SEP     R4
        WORD    SUB16
        LBDF    TA8
        LDI     0
        PLO     R8
        PHI     R8
TA8
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LDI     23
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S39)
        PHI     RC
        LDI     LOW (S39)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     23
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S40)
        PHI     RC
        LDI     LOW (S40)
        PLO     RC
        LDI     130
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        GLO     R9
        SMI     1
        SEP     R4
        WORD    STRN
        LDI     130
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (201)
        PHI     RC
        LDI     LOW (201)
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBNF    TA9
        LDI     HIGH (S41)
        PHI     RC
        LDI     LOW (S41)
        PLO     RC
        LDI     129
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (2800H+1AH)
        PHI     RC
        LDI     LOW (2800H+1AH)
        PLO     RC
        LDI     3
        SEP     R4
        WORD    SOUND
        LBR     TKX
TA9
        LDI     HIGH (S42)
        PHI     RC
        LDI     LOW (S42)
        PLO     RC
        LDI     130
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (3F88H)
        PHI     RC
        LDI     LOW (3F88H)
        PLO     RC
        LDI     10
        SEP     R4
        WORD    NSOUND
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     6
        STR     RA
        LDI     HIGH (281FH)
        PHI     RC
        LDI     LOW (281FH)
        PLO     RC
        LDI     10
        SEP     R4
        WORD    SOUND
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDN     RA
        STR     R2
        LDN     RA
        ADD
        STR     RA
        SMI     10
        LBNF    TA10
        LDI     9
        STR     RA
TA10
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDN     RA
        SHL
        PLO     RD
TA11    LDI     10
        SEP     R4
        WORD    RANDN
        ADI     1
        PLO     R9
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     9
        LBZ     TA12
        LDN     RA
        ADI     1
        STR     RA
TA12    DEC     RD
        GLO     RD
        LBNZ    TA11
TKX
        LDI     HIGH (EVT)
        PHI     RA
        LDI     LOW (EVT)
        PLO     RA
        LDN     RA
        SEP     R5
CONTACT
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     1
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S43)
        PHI     RC
        LDI     LOW (S43)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S44)
        PHI     RC
        LDI     LOW (S44)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     5
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        SEP     R4
        WORD    PORBIT
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     3
        LBDF    CT2
        LDI     8
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S45)
        PHI     RC
        LDI     LOW (S45)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     9
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S46)
        PHI     RC
        LDI     LOW (S46)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S47)
        PHI     RC
        LDI     LOW (S47)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S48)
        PHI     RC
        LDI     LOW (S48)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     100
        PHI     RE
        SEP     R4
        WORD    MULT
        SEP     R4
        WORD    ADDSC
        LBR     CTX
CT2     LBNZ    CT3
        LDI     8
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S49)
        PHI     RC
        LDI     LOW (S49)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     9
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S50)
        PHI     RC
        LDI     LOW (S50)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S51)
        PHI     RC
        LDI     LOW (S51)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S52)
        PHI     RC
        LDI     LOW (S52)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     50
        PHI     RE
        SEP     R4
        WORD    MULT
        SEP     R4
        WORD    ADDSC
        LBR     CTX
CT3
        LDI     8
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S53)
        PHI     RC
        LDI     LOW (S53)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     9
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S54)
        PHI     RC
        LDI     LOW (S54)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S55)
        PHI     RC
        LDI     LOW (S55)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
CTX
        LDI     22
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S56)
        PHI     RC
        LDI     LOW (S56)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (1400H+3CH)
        PHI     RC
        LDI     LOW (1400H+3CH)
        PLO     RC
        LDI     20
        SEP     R4
        WORD    SOUND
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    WKEY
        SEP     R5
BATTLE
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDI     15
        STR     RA
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDI     10
        STR     RA
BDRAW
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        SEP     R4
        WORD    BORDER
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDN     RA
        SMI     1
        SHL
        SHL
        STR     R2
        SHL
        SHL
        ADD
        PLO     R7
        LDI     HIGH (BST)
        PHI     RD
        LDI     LOW (BST)
        PLO     RD
        GLO     R7
        STR     R2
        GLO     RD
        ADD
        PLO     RD
        LDI     10
        PLO     R9
BD1     LDA     RD
        PHI     RE
        LDA     RD
        PLO     RE
        GLO     RD
        STXD
        GHI     RD
        STXD
        SEP     R4
        WORD    POS
        LDI     2EH
        STR     RD
        IRX
        LDXA
        PHI     RD
        LDX
        PLO     RD
        DEC     R9
        GLO     R9
        LBNZ    BD1
        LDI     0
        PLO     R9
BD2
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    BD3
        LDI     HIGH (AROW)
        PHI     RA
        LDI     LOW (AROW)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PHI     RE
        LDI     HIGH (ACOL)
        PHI     RA
        LDI     LOW (ACOL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (GLY)
        PHI     RA
        LDI     LOW (GLY)
        PLO     RA
        LDN     RA
        STR     RD
BD3     INC     R9
        GLO     R9
        XRI     8
        LBNZ    BD2
        LDI     0
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S57)
        PHI     RC
        LDI     LOW (S57)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     1
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S58)
        PHI     RC
        LDI     LOW (S58)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     3
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S59)
        PHI     RC
        LDI     LOW (S59)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     6
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S60)
        PHI     RC
        LDI     LOW (S60)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S61)
        PHI     RC
        LDI     LOW (S61)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     13
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S62)
        PHI     RC
        LDI     LOW (S62)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     17
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S63)
        PHI     RC
        LDI     LOW (S63)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     18
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S64)
        PHI     RC
        LDI     LOW (S64)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     19
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S65)
        PHI     RC
        LDI     LOW (S65)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     20
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S66)
        PHI     RC
        LDI     LOW (S66)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     23
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S67)
        PHI     RC
        LDI     LOW (S67)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     15
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S68)
        PHI     RC
        LDI     LOW (S68)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
BD4
        LDI     HIGH (CMODE)
        PHI     RA
        LDI     LOW (CMODE)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (CMINX)
        PHI     RA
        LDI     LOW (CMINX)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (CMAXX)
        PHI     RA
        LDI     LOW (CMAXX)
        PLO     RA
        LDI     29
        STR     RA
        LDI     HIGH (CMINY)
        PHI     RA
        LDI     LOW (CMINY)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (CMAXY)
        PHI     RA
        LDI     LOW (CMAXY)
        PLO     RA
        LDI     20
        STR     RA
        SEP     R4
        WORD    CURGET
        SEP     R4
        WORD    CURDRAW
        SEP     R4
        WORD    BPANEL
        SEP     R4
        WORD    VIDON
        LDI     HIGH (LASTK)
        PHI     RA
        LDI     LOW (LASTK)
        PLO     RA
        LDI     0
        STR     RA
BLOOP
        SEP     R4
        WORD    FRAME2
        SEP     R4
        WORD    TICK
        PLO     R9
        XRI     2
        LBZ     BDRAW
        GLO     R9
        LBZ     BL1
        SEP     R4
        WORD    BPANEL
BL1
        SEP     R4
        WORD    GETKEY
        LBZ     BLOOP
        PLO     R9
        SMI     82H
        LBNF    BL2
        SMI     4
        LBDF    BL2
        GLO     R9
        SEP     R4
        WORD    CMOVE
        LBR     BLOOP
BL2     GLO     R9
        SMI     31H
        LBNF    BL3
        SMI     4
        LBDF    BL3
        GLO     R9
        SMI     30H
        PLO     R7
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        GLO     R7
        STR     RA
        LBR     BDRAW
BL3
        GLO     R9
        XRI     'G'
        LBZ     BWPN
        GLO     R9
        XRI     K_CR
        LBZ     BWPN
        GLO     R9
        XRI     ' '
        LBZ     FIRE
        GLO     R9
        XRI     5FH
        LBZ     FIRE
        GLO     R9
        XRI     'F'
        LBZ     FIRE
        GLO     R9
        XRI     '+'
        LBZ     BOOST
        GLO     R9
        XRI     '='
        LBZ     BOOST
        GLO     R9
        XRI     5BH
        LBZ     BOOST
        GLO     R9
        XRI     2DH
        LBZ     BOOST
        GLO     R9
        XRI     5CH
        LBZ     DROP
        GLO     R9
        XRI     'C'
        LBZ     CHART
        GLO     R9
        XRI     'S'
        LBZ     COMP
        GLO     R9
        XRI     'I'
        LBZ     IMPULSE
        GLO     R9
        XRI     'M'
        LBZ     IMPULSE
        LBR     BLOOP
BWPN
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        SMI     5
        LBNF    BW1
        LDI     1
        STR     RA
BW1
        SEP     R4
        WORD    BPANEL
        LBR     BLOOP
FMNONE
        LDI     21
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S69)
        PHI     RC
        LDI     LOW (S69)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (202FH)
        PHI     RC
        LDI     LOW (202FH)
        PLO     RC
        LDI     6
        SEP     R4
        WORD    SOUND
        LBR     BLOOP
FMDMG
        LDI     21
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S70)
        PHI     RC
        LDI     LOW (S70)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (202FH)
        PHI     RC
        LDI     LOW (202FH)
        PLO     RC
        LDI     6
        SEP     R4
        WORD    SOUND
        LBR     BLOOP
FMNOE
        LDI     21
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S71)
        PHI     RC
        LDI     LOW (S71)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (202FH)
        PHI     RC
        LDI     LOW (202FH)
        PLO     RC
        LDI     6
        SEP     R4
        WORD    SOUND
        LBR     BLOOP
FMNOT
        LDI     21
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S72)
        PHI     RC
        LDI     LOW (S72)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (202FH)
        PHI     RC
        LDI     LOW (202FH)
        PLO     RC
        LDI     6
        SEP     R4
        WORD    SOUND
        LBR     BLOOP
FMMISS
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        SMI     3
        LBDF    FMM1
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     R9
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (50)
        PHI     RC
        LDI     LOW (50)
        PLO     RC
        SEP     R4
        WORD    SUB16
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LBR     FMM2
FMM1
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (TORP-3)
        PHI     RA
        LDI     LOW (TORP-3)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
FMM2
        SEP     R4
        WORD    BPANEL
        LDI     21
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S73)
        PHI     RC
        LDI     LOW (S73)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     BLOOP
FMHIT
        LDI     21
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S74)
        PHI     RC
        LDI     LOW (S74)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     BLOOP
BOOST
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (251)
        PHI     RC
        LDI     LOW (251)
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBNF    BLOOP
        LDI     HIGH (250)
        PHI     RC
        LDI     LOW (250)
        PLO     RC
        SEP     R4
        WORD    SUB16
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDN     RA
        ADI     6
        PLO     R9
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (50)
        PHI     RC
        LDI     LOW (50)
        PLO     RC
        SEP     R4
        WORD    ADD16
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (1000H+2CH)
        PHI     RC
        LDI     LOW (1000H+2CH)
        PLO     RC
        LDI     3
        SEP     R4
        WORD    SOUND
        SEP     R4
        WORD    BPANEL
        LBR     BLOOP
DROP
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDN     RA
        ADI     6
        PLO     R9
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (50)
        PHI     RC
        LDI     LOW (50)
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBNF    BLOOP
        SEP     R4
        WORD    SUB16
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (250)
        PHI     RC
        LDI     LOW (250)
        PLO     RC
        SEP     R4
        WORD    ADD16
        LDI     HIGH (SE+2)
        PHI     RA
        LDI     LOW (SE+2)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (2000H+2CH)
        PHI     RC
        LDI     LOW (2000H+2CH)
        PLO     RC
        LDI     3
        SEP     R4
        WORD    SOUND
        SEP     R4
        WORD    BPANEL
        LBR     BLOOP
BPANEL
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDN     RA
        ADI     5
        SEP     R4
        WORD    STRN
        LDI     4
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     4
        PLO     R9
BP0     LDA     RC
        ORI     80H
        SEP     R4
        WORD    PUTC
        INC     RD
        DEC     R9
        GLO     R9
        LBNZ    BP0
        LDI     7
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S75)
        PHI     RC
        LDI     LOW (S75)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     1
        SEP     R4
        WORD    STRN
        LDI     7
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     5
        PLO     R9
BP1     LDA     RC
        ORI     80H
        SEP     R4
        WORD    PUTC
        INC     RD
        DEC     R9
        GLO     R9
        LBNZ    BP1
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ANI     1
        SDI     0B2H
        SEP     R4
        WORD    PUTC
        LDI     8
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S75)
        PHI     RC
        LDI     LOW (S75)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        SMI     3
        LBDF    BP2
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     R9
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        SEP     R4
        WORD    PENRG
        LBR     BP3
BP2
        LDI     HIGH (TORP-3)
        PHI     RA
        LDI     LOW (TORP-3)
        PLO     RA
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (TORP-3)
        PHI     RA
        LDI     LOW (TORP-3)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     HIGH (S76)
        PHI     RC
        LDI     LOW (S76)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
BP3
        LDI     11
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S75)
        PHI     RC
        LDI     LOW (S75)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDN     RA
        ADI     6
        PLO     R9
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        SEP     R4
        WORD    PENRG
        LDI     14
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S75)
        PHI     RC
        LDI     LOW (S75)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     14
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     0AFH
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     R9
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     16
        PHI     RE
        LDI     31
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     0
        PHI     R9
        PLO     R9
BPC1
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     1
        LBNZ    BPD1
        GHI     R9
        ADI     1
        PHI     R9
BPD1   INC     R9
        GLO     R9
        XRI     8
        LBNZ    BPC1
        GHI     R9
        ADI     0B0H
        SEP     R4
        WORD    PUTC
        INC     RD
        INC     RD
        LDI     0
        PHI     R9
        PLO     R9
BPC2
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     2
        LBNZ    BPD2
        GHI     R9
        ADI     1
        PHI     R9
BPD2   INC     R9
        GLO     R9
        XRI     8
        LBNZ    BPC2
        GHI     R9
        ADI     0B0H
        SEP     R4
        WORD    PUTC
        INC     RD
        INC     RD
        LDI     0
        PHI     R9
        PLO     R9
BPC3
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     3
        LBNZ    BPD3
        GHI     R9
        ADI     1
        PHI     R9
BPD3   INC     R9
        GLO     R9
        XRI     8
        LBNZ    BPC3
        GHI     R9
        ADI     0B0H
        SEP     R4
        WORD    PUTC
        INC     RD
        INC     RD
        LDI     0
        PHI     R9
        PLO     R9
BPC4
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     4
        LBNZ    BPD4
        GHI     R9
        ADI     1
        PHI     R9
BPD4   INC     R9
        GLO     R9
        XRI     8
        LBNZ    BPC4
        GHI     R9
        ADI     0B0H
        SEP     R4
        WORD    PUTC
        INC     RD
        INC     RD
        SEP     R5
FIRE
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBZ     FMNONE
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     RD
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     RD
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     9
        LBZ     FMDMG
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        SMI     3
        LBDF    FI1
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     R9
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (50)
        PHI     RC
        LDI     LOW (50)
        PLO     RC
        SEP     R4
        WORD    CMP16
        LBNF    FMNOE
        LBR     FI2
FI1
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (TORP-3)
        PHI     RA
        LDI     LOW (TORP-3)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        LBZ     FMNOT
FI2
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        SMI     3
        LBDF    FS1
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     3
        STR     RA
        LDI     HIGH (1C00H+3FH)
        PHI     RC
        LDI     LOW (1C00H+3FH)
        PLO     RC
        LDI     10
        SEP     R4
        WORD    SOUND
        LBR     FS2
FS1
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     6
        STR     RA
        LDI     HIGH (0C00H+4FH)
        PHI     RC
        LDI     LOW (0C00H+4FH)
        PLO     RC
        LDI     14
        SEP     R4
        WORD    SOUND
FS2
        LDI     0FFH
        PHI     R9
        LDI     0
        PLO     R9
FI3
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    FI4
        LDI     HIGH (AROW)
        PHI     RA
        LDI     LOW (AROW)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        SM
        ADI     1
        SMI     3
        LBDF    FI4
        LDI     HIGH (ACOL)
        PHI     RA
        LDI     LOW (ACOL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        SM
        ADI     1
        SMI     3
        LBDF    FI4
        GLO     R9
        PHI     R9
FI4     INC     R9
        GLO     R9
        XRI     8
        LBNZ    FI3
        GHI     R9
        XRI     0FFH
        LBZ     FMMISS
        GHI     R9
        PLO     R9
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        SMI     3
        LBDF    FI6
        GLO     R9
        STXD
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     R9
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     HIGH (50)
        PHI     RC
        LDI     LOW (50)
        PLO     RC
        SEP     R4
        WORD    SUB16
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        IRX
        PHI     RD
        LDX
        PLO     R9
        GHI     RD
        PLO     RE
        LDI     10
        PHI     RE
        SEP     R4
        WORD    MULT
        LDI     50
        PLO     RC
FI5     GLO     R8
        SMI     35
        PLO     R8
        GHI     R8
        SMBI    0
        PHI     R8
        LBNF    FI5A
        GLO     RC
        SHR
        PLO     RC
        LBR     FI5
FI5A    LDI     0
        PHI     RC
        LBR     FI7
FI6
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (TORP-3)
        PHI     RA
        LDI     LOW (TORP-3)
        PLO     RA
        GLO     R7
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LDI     HIGH (350)
        PHI     RC
        LDI     LOW (350)
        PLO     RC
FI7
        LDI     HIGH (AEN)
        PHI     RA
        LDI     LOW (AEN)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        SEP     R4
        WORD    SUB16
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        DEC     RA
        LBNF    FI8
        GLO     R8
        LBNZ    FI9
        GHI     R8
        LBNZ    FI9
        LDI     HIGH (5C88H)
        PHI     RC
        LDI     LOW (5C88H)
        PLO     RC
        LDI     8
        SEP     R4
        WORD    NSOUND
        SEP     R4
        WORD    BPANEL
        LBR     FMHIT
FI9
        LDI     HIGH (5C88H)
        PHI     RC
        LDI     LOW (5C88H)
        PLO     RC
        LDI     8
        SEP     R4
        WORD    NSOUND
        SEP     R4
        WORD    BPANEL
        LBR     FMHIT
FI8
        LDI     HIGH (ASC)
        PHI     RA
        LDI     LOW (ASC)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDI     0
        STR     RA
        LDI     HIGH (AROW)
        PHI     RA
        LDI     LOW (AROW)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PHI     RE
        LDI     HIGH (ACOL)
        PHI     RA
        LDI     LOW (ACOL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     20H
        SEP     R4
        WORD    PUTC
        LDI     HIGH (AROW)
        PHI     RA
        LDI     LOW (AROW)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURY)
        PHI     RA
        LDI     LOW (CURY)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    FI8A
        LDI     HIGH (ACOL)
        PHI     RA
        LDI     LOW (ACOL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CURX)
        PHI     RA
        LDI     LOW (CURX)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    FI8A
        LDI     HIGH (UNDER)
        PHI     RA
        LDI     LOW (UNDER)
        PLO     RA
        LDI     32
        STR     RA
        SEP     R4
        WORD    CURDRAW
FI8A
        LDI     HIGH (2F88H)
        PHI     RC
        LDI     LOW (2F88H)
        PLO     RC
        LDI     30
        SEP     R4
        WORD    NSOUND
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (301FH)
        PHI     RC
        LDI     LOW (301FH)
        PLO     RC
        LDI     20
        SEP     R4
        WORD    SOUND
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LDI     HIGH (ALR)
        PHI     RA
        LDI     LOW (ALR)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     25
        PHI     RE
        SEP     R4
        WORD    MULT
        SEP     R4
        WORD    ADDSC
        SEP     R4
        WORD    BPANEL
        LDI     21
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S77)
        PHI     RC
        LDI     LOW (S77)
        PLO     RC
        LDI     1
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBNZ    BLOOP
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LDI     0
        STR     RA
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     4
        LBZ     FI8B
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     5
        LBNZ    FI8C
FI8B
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LDI     6
        STR     RA
FI8C
        LDI     23
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S78)
        PHI     RC
        LDI     LOW (S78)
        PLO     RC
        LDI     1
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    WKEY
        LBR     STATUS
COMP
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     1
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S79)
        PHI     RC
        LDI     LOW (S79)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S80)
        PHI     RC
        LDI     LOW (S80)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     6
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S81)
        PHI     RC
        LDI     LOW (S81)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S82)
        PHI     RC
        LDI     LOW (S82)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S83)
        PHI     RC
        LDI     LOW (S83)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     12
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S84)
        PHI     RC
        LDI     LOW (S84)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     14
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S85)
        PHI     RC
        LDI     LOW (S85)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     16
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S86)
        PHI     RC
        LDI     LOW (S86)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBZ     CO1
        LDI     19
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S87)
        PHI     RC
        LDI     LOW (S87)
        PLO     RC
        LDI     130
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     CO1A
CO1
        LDI     19
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S88)
        PHI     RC
        LDI     LOW (S88)
        PLO     RC
        LDI     1
        PLO     RF
        SEP     R4
        WORD    PRT
CO1A
        LDI     23
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S89)
        PHI     RC
        LDI     LOW (S89)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
COL1
        SEP     R4
        WORD    FRAME2
        SEP     R4
        WORD    TICK
        XRI     2
        LBZ     COMP
        SEP     R4
        WORD    GETKEY
        LBZ     COL1
        PLO     R9
        GLO     R9
        XRI     'T'
        LBZ     STATUS
        GLO     R9
        XRI     'E'
        LBZ     ALLOC
        GLO     R9
        XRI     'R'
        LBZ     REPAIR
        GLO     R9
        XRI     'D'
        LBZ     DIPLO
        GLO     R9
        XRI     'X'
        LBZ     ENDQ
        GLO     R9
        XRI     'H'
        LBZ     HELPC
        GLO     R9
        XRI     'Z'
        LBZ     DESTQ
        GLO     R9
        XRI     'C'
        LBZ     CHART
        GLO     R9
        XRI     'B'
        LBZ     BATTLE
        GLO     R9
        XRI     'I'
        LBZ     IMPULSE
        GLO     R9
        XRI     'M'
        LBZ     IMPULSE
        GLO     R9
        XRI     'S'
        LBZ     COMP
        LBR     COL1
STATUS
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     0
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S90)
        PHI     RC
        LDI     LOW (S90)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     2
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S33)
        PHI     RC
        LDI     LOW (S33)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    PORBIT
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (NPL)
        PHI     RA
        LDI     LOW (NPL)
        PLO     RA
        LDN     RA
        PLO     R9
ST1
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        STR     R2
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        OR
        STR     RA
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        DEC     R9
        GLO     R9
        LBNZ    ST1
        LDI     3
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S91)
        PHI     RC
        LDI     LOW (S91)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     6
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBNZ    ST3
        LDI     HIGH (NAL)
        PHI     RA
        LDI     LOW (NAL)
        PLO     RA
        LDN     RA
        LBNZ    ST3
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        LBNZ    ST2
        LDI     HIGH (S10)
        PHI     RC
        LDI     LOW (S10)
        PLO     RC
        LDI     1
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     ST4
ST2
        LDI     HIGH (S92)
        PHI     RC
        LDI     LOW (S92)
        PLO     RC
        LDI     129
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     ST4
ST3
        LDI     HIGH (S93)
        PHI     RC
        LDI     LOW (S93)
        PLO     RC
        LDI     130
        PLO     RF
        SEP     R4
        WORD    PRT
ST4
        LDI     3
        PHI     RE
        LDI     20
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S94)
        PHI     RC
        LDI     LOW (S94)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PSCORE
        LDI     5
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S95)
        PHI     RC
        LDI     LOW (S95)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     1
        PLO     R9
ST5
        GLO     R9
        ADI     5
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        GLO     R9
        SMI     1
        SEP     R4
        WORD    STRN
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        GLO     R9
        ADI     5
        PHI     RE
        LDI     18
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        GLO     R9
        SHL
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        DEC     RA
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PENRG
        GLO     R9
        ADI     5
        PHI     RE
        LDI     26
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (DMGT)
        PHI     RC
        LDI     LOW (DMGT)
        PLO     RC
        GLO     R7
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        INC     R9
        GLO     R9
        XRI     12
        LBNZ    ST5
        LDI     18
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S96)
        PHI     RC
        LDI     LOW (S96)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (TORP)
        PHI     RA
        LDI     LOW (TORP)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     HIGH (S97)
        PHI     RC
        LDI     LOW (S97)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (TORP+1)
        PHI     RA
        LDI     LOW (TORP+1)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     23
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S7)
        PHI     RC
        LDI     LOW (S7)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
STL
        SEP     R4
        WORD    FRAME2
        SEP     R4
        WORD    TICK
        LBNZ    STATUS
        SEP     R4
        WORD    GETKEY
        LBZ     STL
        PLO     R9
        GLO     R9
        XRI     'C'
        LBZ     CHART
        GLO     R9
        XRI     'B'
        LBZ     BATTLE
        GLO     R9
        XRI     'I'
        LBZ     IMPULSE
        GLO     R9
        XRI     'M'
        LBZ     IMPULSE
        LBR     COMP
ALLOC
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     0
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S98)
        PHI     RC
        LDI     LOW (S98)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     1
        PLO     R9
AL1
        GLO     R9
        ADI     1
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        GLO     R9
        SMI     1
        SEP     R4
        WORD    STRN
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    ALSHOW
        INC     R9
        GLO     R9
        XRI     12
        LBNZ    AL1
        LDI     14
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S99)
        PHI     RC
        LDI     LOW (S99)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    WKEY
        XRI     'Y'
        LBNZ    COMP
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDI     0
        STR     RA
        LDI     1
        PLO     R9
AL2
        LDI     16
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S39)
        PHI     RC
        LDI     LOW (S39)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     16
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        GLO     R9
        SMI     1
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (S100)
        PHI     RC
        LDI     LOW (S100)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        SDI     100
        SEP     R4
        WORD    PNUM
        LDI     HIGH (S101)
        PHI     RC
        LDI     LOW (S101)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        GLO     R9
        STXD
        SEP     R4
        WORD    GETNUM
        IRX
        PLO     R7
        LDX
        PLO     R9
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        LDN     RA
        SDI     100
        STR     R2
        GLO     R7
        SD
        LBNF    AL2
        LDI     HIGH (SAL)
        PHI     RA
        LDI     LOW (SAL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GLO     R7
        STR     RA
        LDI     HIGH (TMP)
        PHI     RA
        LDI     LOW (TMP)
        PLO     RA
        GLO     R7
        STR     R2
        LDN     RA
        ADD
        STR     RA
        SEP     R4
        WORD    ALSHOW
        INC     R9
        GLO     R9
        XRI     12
        LBNZ    AL2
        LBR     COMP
ALSHOW  GLO     R9
        ADI     1
        PHI     RE
        LDI     20
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S102)
        PHI     RC
        LDI     LOW (S102)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        GLO     R9
        ADI     1
        PHI     RE
        LDI     20
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        LDI     HIGH (SAL)
        PHI     RA
        LDI     LOW (SAL)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        LDI     HIGH (S103)
        PHI     RC
        LDI     LOW (S103)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R5
REPAIR
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     0
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S104)
        PHI     RC
        LDI     LOW (S104)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     0
        PHI     RE
        LDI     24
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S105)
        PHI     RC
        LDI     LOW (S105)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     1
        PLO     R9
RP1
        SEP     R4
        WORD    RPSHOW
        GLO     R9
        ADI     1
        PHI     RE
        LDI     24
        PLO     RE
        SEP     R4
        WORD    POS
        GLO     R9
        XRI     10
        LBNZ    RP1A
        LDI     0B0H
        LBR     RP1B
RP1A    GLO     R9
        ADI     0B0H
RP1B
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     0BDH
        SEP     R4
        WORD    PUTC
        INC     RD
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        GLO     R9
        SMI     1
        SEP     R4
        WORD    STRN
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        INC     R9
        GLO     R9
        XRI     11
        LBNZ    RP1
        LDI     13
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S106)
        PHI     RC
        LDI     LOW (S106)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     14
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S107)
        PHI     RC
        LDI     LOW (S107)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        LDI     1
        PLO     R9
RP2
        LDI     16
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S108)
        PHI     RC
        LDI     LOW (S108)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     16
        PHI     RE
        LDI     9
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        GLO     R9
        SEP     R4
        WORD    PNUM
RP3
        SEP     R4
        WORD    WKEY
        PLO     R7
        SMI     30H
        LBNF    RP3
        SMI     10
        LBDF    RP3
        GLO     R7
        SMI     30H
        LBNZ    RP4
        LDI     10
RP4     PLO     R7
        LDI     HIGH (SRP)
        PHI     RA
        LDI     LOW (SRP)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GLO     R7
        STR     RA
        SEP     R4
        WORD    RPSHOW
        INC     R9
        GLO     R9
        XRI     11
        LBNZ    RP2
        LBR     COMP
RPSHOW  GLO     R9
        ADI     1
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S109)
        PHI     RC
        LDI     LOW (S109)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        GLO     R9
        ADI     1
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     128
        PLO     RF
        GLO     R9
        SEP     R4
        WORD    PNUM
        INC     RD
        LDI     HIGH (SRP)
        PHI     RA
        LDI     LOW (SRP)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     1
        PLO     R7
        LDI     HIGH (SYSNT)
        PHI     RC
        LDI     LOW (SYSNT)
        PLO     RC
        GLO     R7
        SEP     R4
        WORD    STRN
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R5
DIPLO
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     0
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S110)
        PHI     RC
        LDI     LOW (S110)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     2
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S111)
        PHI     RC
        LDI     LOW (S111)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     3
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S112)
        PHI     RC
        LDI     LOW (S112)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S113)
        PHI     RC
        LDI     LOW (S113)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     5
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S114)
        PHI     RC
        LDI     LOW (S114)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     7
        PHI     RE
        LDI     2
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S115)
        PHI     RC
        LDI     LOW (S115)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
DP1
        SEP     R4
        WORD    FRAME2
        SEP     R4
        WORD    TICK
        XRI     2
        LBZ     DIPLO
        SEP     R4
        WORD    GETKEY
        LBZ     DP1
        PLO     R9
        GLO     R9
        XRI     '0'
        LBZ     COMP
        GLO     R9
        XRI     'C'
        LBZ     CHART
        GLO     R9
        XRI     'B'
        LBZ     BATTLE
        GLO     R9
        XRI     'M'
        LBZ     IMPULSE
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (ORB)
        PHI     RA
        LDI     LOW (ORB)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S39)
        PHI     RC
        LDI     LOW (S39)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        GLO     R9
        XRI     '1'
        LBZ     DPC
        GLO     R9
        XRI     '2'
        LBZ     DPR
        GLO     R9
        XRI     '3'
        LBZ     DPW
        LBR     DP1
DPC
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBNZ    DPC1
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S116)
        PHI     RC
        LDI     LOW (S116)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPC1
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     10
        LBDF    DPC2
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S117)
        PHI     RC
        LDI     LOW (S117)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPC2
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     4
        LBDF    DPC3
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S118)
        PHI     RC
        LDI     LOW (S118)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPC3
        LDI     HIGH (CO)
        PHI     RA
        LDI     LOW (CO)
        PLO     RA
        LDI     0
        STR     RA
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S119)
        PHI     RC
        LDI     LOW (S119)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPR
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBNZ    DPR1
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S116)
        PHI     RC
        LDI     LOW (S116)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPR1
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     10
        LBNF    DPR2
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S120)
        PHI     RC
        LDI     LOW (S120)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPR2
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     6
        LBDF    DPR3
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S121)
        PHI     RC
        LDI     LOW (S121)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPR3
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     3
        LBNF    DPR4
        SMI     3
        LBDF    DPR4
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S122)
        PHI     RC
        LDI     LOW (S122)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPR4
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SDI     7
        PLO     RD
        LDI     1
        PLO     R9
DPR5
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        GLO     R9
        STR     R2
        GLO     RA
        ADD
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        GLO     RD
        STR     R2
        LDN     RA
        SM
        LBDF    DPR6
        LDI     0
DPR6    STR     RA
        INC     R9
        GLO     R9
        XRI     12
        LBNZ    DPR5
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S123)
        PHI     RC
        LDI     LOW (S123)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPW
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LBNZ    DPW1
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S116)
        PHI     RC
        LDI     LOW (S116)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPW1
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     10
        LBNF    DPW2
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S120)
        PHI     RC
        LDI     LOW (S120)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPW2
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     4
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     4
        LBDF    DPW3
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S118)
        PHI     RC
        LDI     LOW (S118)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPW3
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        XRI     6
        LBNZ    DPW4
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S124)
        PHI     RC
        LDI     LOW (S124)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPW4
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        SMI     3
        LBDF    DPW5
        LDI     HIGH (SCORE)
        PHI     RA
        LDI     LOW (SCORE)
        PLO     RA
        LDI     0D8H
        STR     RA
        INC     RA
        LDI     0F0H
        STR     RA
DPW5
        SEP     R4
        WORD    PADDR
        GLO     RA
        ADI     5
        PLO     RA
        GHI     RA
        ADCI    0
        PHI     RA
        LDN     RA
        LDI     5
        STR     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (1800H+3FH)
        PHI     RC
        LDI     LOW (1800H+3FH)
        PLO     RC
        LDI     30
        SEP     R4
        WORD    SOUND
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S125)
        PHI     RC
        LDI     LOW (S125)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LBR     DPM
DPM
        LDI     22
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S56)
        PHI     RC
        LDI     LOW (S56)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    WKEY
        LBR     DIPLO
ENDQ
        LDI     21
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S126)
        PHI     RC
        LDI     LOW (S126)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
ENDQ1
        SEP     R4
        WORD    WKEY
        PLO     R9
        GLO     R9
        XRI     'Y'
        LBZ     RANK
        LBR     COMP
DESTQ
        LDI     21
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S127)
        PHI     RC
        LDI     LOW (S127)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
DESTQ1
        SEP     R4
        WORD    WKEY
        PLO     R9
        GLO     R9
        XRI     'Y'
        LBZ     EXPLODE
        LBR     COMP
DEAD
EXPLODE
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        SEP     R4
        WORD    VIDON
        LDI     HIGH (1F88H)
        PHI     RC
        LDI     LOW (1F88H)
        PLO     RC
        LDI     90
        SEP     R4
        WORD    NSOUND
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (201FH)
        PHI     RC
        LDI     LOW (201FH)
        PLO     RC
        LDI     90
        SEP     R4
        WORD    SOUND
        LDI     80
        PLO     R9
EX1
        SEP     R4
        WORD    FRAME2
        LDI     24
        SEP     R4
        WORD    RANDN
        PHI     RE
        LDI     40
        SEP     R4
        WORD    RANDN
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     0AAH
        SEP     R4
        WORD    PUTC
        DEC     R9
        GLO     R9
        LBNZ    EX1
        LDI     11
        PHI     RE
        LDI     10
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S128)
        PHI     RC
        LDI     LOW (S128)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     13
        PHI     RE
        LDI     10
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S129)
        PHI     RC
        LDI     LOW (S129)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    WKEY
RANK
        LDI     HIGH (RKF)
        PHI     RA
        LDI     LOW (RKF)
        PLO     RA
        LDN     RA
        LBNZ    AGAIN
        LDI     HIGH (RKF)
        PHI     RA
        LDI     LOW (RKF)
        PLO     RA
        LDI     1
        STR     RA
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     HIGH (SCORE)
        PHI     RA
        LDI     LOW (SCORE)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        GHI     R8
        ANI     80H
        LBNZ    DISMISS
        LDI     HIGH (1000)
        PHI     RC
        LDI     LOW (1000)
        PLO     RC
        LDI     0
        PLO     R9
RK1
        SEP     R4
        WORD    SUB16
        LBNF    RK2
        INC     R9
        GLO     R9
        XRI     5
        LBNZ    RK1
RK2
        LDI     4
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S130)
        PHI     RC
        LDI     LOW (S130)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PSCORE
        LDI     6
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S131)
        PHI     RC
        LDI     LOW (S131)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (RANKT)
        PHI     RC
        LDI     LOW (RANKT)
        PLO     RC
        GLO     R9
        SEP     R4
        WORD    STRN
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S132)
        PHI     RC
        LDI     LOW (S132)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     128
        PLO     RF
        LDI     HIGH (LEVEL)
        PHI     RA
        LDI     LOW (LEVEL)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    PNUM
        SEP     R4
        WORD    VIDON
        LBR     AGAIN
DISMISS
        LDI     4
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S133)
        PHI     RC
        LDI     LOW (S133)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     5
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S134)
        PHI     RC
        LDI     LOW (S134)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     6
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S135)
        PHI     RC
        LDI     LOW (S135)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S56)
        PHI     RC
        LDI     LOW (S56)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    WKEY
        LBR     EXPLODE
AGAIN
        LDI     14
        PHI     RE
        LDI     4
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S136)
        PHI     RC
        LDI     LOW (S136)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
AG1
        SEP     R4
        WORD    WKEY
        PLO     R9
        GLO     R9
        XRI     'Y'
        LBZ     TITLE
        GLO     R9
        XRI     'N'
        LBZ     EXIT
        LBR     AG1
HELP
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     1
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S137)
        PHI     RC
        LDI     LOW (S137)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     2
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S138)
        PHI     RC
        LDI     LOW (S138)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     3
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S139)
        PHI     RC
        LDI     LOW (S139)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S140)
        PHI     RC
        LDI     LOW (S140)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     5
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S138)
        PHI     RC
        LDI     LOW (S138)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     6
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S141)
        PHI     RC
        LDI     LOW (S141)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     7
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S142)
        PHI     RC
        LDI     LOW (S142)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S143)
        PHI     RC
        LDI     LOW (S143)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     9
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S144)
        PHI     RC
        LDI     LOW (S144)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S145)
        PHI     RC
        LDI     LOW (S145)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S138)
        PHI     RC
        LDI     LOW (S138)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     12
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S146)
        PHI     RC
        LDI     LOW (S146)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     13
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S147)
        PHI     RC
        LDI     LOW (S147)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     22
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S56)
        PHI     RC
        LDI     LOW (S56)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    WKEY
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    CLRSCR
        LDI     1
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S148)
        PHI     RC
        LDI     LOW (S148)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     2
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S149)
        PHI     RC
        LDI     LOW (S149)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     3
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S150)
        PHI     RC
        LDI     LOW (S150)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     4
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S151)
        PHI     RC
        LDI     LOW (S151)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     5
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S138)
        PHI     RC
        LDI     LOW (S138)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     6
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S152)
        PHI     RC
        LDI     LOW (S152)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     7
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S153)
        PHI     RC
        LDI     LOW (S153)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     8
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S154)
        PHI     RC
        LDI     LOW (S154)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     9
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S138)
        PHI     RC
        LDI     LOW (S138)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     10
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S155)
        PHI     RC
        LDI     LOW (S155)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     11
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S156)
        PHI     RC
        LDI     LOW (S156)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     12
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S157)
        PHI     RC
        LDI     LOW (S157)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     13
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S158)
        PHI     RC
        LDI     LOW (S158)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     22
        PHI     RE
        LDI     0
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (S56)
        PHI     RC
        LDI     LOW (S56)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        SEP     R4
        WORD    WKEY
        SEP     R5
HELPC
        SEP     R4
        WORD    HELP
        LBR     COMP
NEWGAME
        LDI     HIGH (SYSF)
        PHI     RA
        LDI     LOW (SYSF)
        PLO     RA
        LDI     64
        PLO     R7
NG1     LDI     0
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NG1
        LDI     HIGH (SCEN)
        PHI     RA
        LDI     LOW (SCEN)
        PLO     RA
        LDN     RA
        XRI     3
        LBNZ    NG3
        LDI     HIGH (SYSF+1)
        PHI     RA
        LDI     LOW (SYSF+1)
        PLO     RA
        LDI     29
        PLO     R7
NG2     LDI     1
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NG2
NG3
        LDI     HIGH (PSYS)
        PHI     RA
        LDI     LOW (PSYS)
        PLO     RA
        LDI     30
        STR     RA
        LDI     HIGH (PPL)
        PHI     RA
        LDI     LOW (PPL)
        PLO     RA
        LDI     1
        STR     RA
        SEP     R4
        WORD    PADDR
        LDI     HIGH (SOLD)
        PHI     RC
        LDI     LOW (SOLD)
        PLO     RC
        LDI     72
        PLO     R7
NG4     LDA     RC
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NG4
        LDI     HIGH (SYSG+30)
        PHI     RA
        LDI     LOW (SYSG+30)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (SE)
        PHI     RA
        LDI     LOW (SE)
        PLO     RA
        LDI     HIGH (EINIT)
        PHI     RC
        LDI     LOW (EINIT)
        PLO     RC
        LDI     24
        PLO     R7
NG5     LDA     RC
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NG5
        LDI     HIGH (SAL)
        PHI     RA
        LDI     LOW (SAL)
        PLO     RA
        LDI     HIGH (AINIT)
        PHI     RC
        LDI     LOW (AINIT)
        PLO     RC
        LDI     12
        PLO     R7
NG6     LDA     RC
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NG6
        LDI     HIGH (SRP)
        PHI     RA
        LDI     LOW (SRP)
        PLO     RA
        LDI     HIGH (PINIT)
        PHI     RC
        LDI     LOW (PINIT)
        PLO     RC
        LDI     11
        PLO     R7
NG7     LDA     RC
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NG7
        LDI     HIGH (SDM)
        PHI     RA
        LDI     LOW (SDM)
        PLO     RA
        LDI     12
        PLO     R7
NG8     LDI     0
        STR     RA
        INC     RA
        DEC     R7
        GLO     R7
        LBNZ    NG8
        LDI     HIGH (TORP)
        PHI     RA
        LDI     LOW (TORP)
        PLO     RA
        LDI     0
        STR     RA
        INC     RA
        STR     RA
        LDI     HIGH (SCORE)
        PHI     RA
        LDI     LOW (SCORE)
        PLO     RA
        LDI     0
        STR     RA
        INC     RA
        STR     RA
        LDI     HIGH (WPN)
        PHI     RA
        LDI     LOW (WPN)
        PLO     RA
        LDI     3
        STR     RA
        LDI     HIGH (SCR)
        PHI     RA
        LDI     LOW (SCR)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (RKF)
        PHI     RA
        LDI     LOW (RKF)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (APTR)
        PHI     RA
        LDI     LOW (APTR)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (SNDT)
        PHI     RA
        LDI     LOW (SNDT)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (FRM)
        PHI     RA
        LDI     LOW (FRM)
        PLO     RA
        LDA     RA
        PHI     R8
        LDN     RA
        PLO     R8
        LDI     HIGH (T1)
        PHI     RA
        LDI     LOW (T1)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        LDI     HIGH (T2)
        PHI     RA
        LDI     LOW (T2)
        PLO     RA
        GHI     R8
        STR     RA
        INC     RA
        GLO     R8
        STR     RA
        LDI     HIGH (SHIPS)
        PHI     RA
        LDI     LOW (SHIPS)
        PLO     RA
        LDN     RA
        SEP     R4
        WORD    ARRIVE
        SEP     R5
ROMAN
        TEXT    "I"
        BYTE    0
        TEXT    "II"
        BYTE    0
        TEXT    "III"
        BYTE    0
        TEXT    "IV"
        BYTE    0
        TEXT    "V"
        BYTE    0
        TEXT    "VI"
        BYTE    0
        TEXT    "VII"
        BYTE    0
        TEXT    "VIII"
        BYTE    0
        TEXT    "IX"
        BYTE    0
        TEXT    "X"
        BYTE    0
        TEXT    "XI"
        BYTE    0
CATT
        TEXT    "TERRESTRIAL"
        BYTE    0
        TEXT    "GAS GIANT"
        BYTE    0
DENT
        TEXT    "V.THIN"
        BYTE    0
        TEXT    "THIN"
        BYTE    0
        TEXT    "DENSE"
        BYTE    0
        TEXT    "V.DENSE"
        BYTE    0
ATMT
        TEXT    "OXYGEN/NITROGEN"
        BYTE    0
        TEXT    "HYDROGEN/HELIUM"
        BYTE    0
        TEXT    "OXYGEN/NITROGEN"
        BYTE    0
        TEXT    "METHANE/AMMONIA"
        BYTE    0
        TEXT    "CHLORINE/HCL"
        BYTE    0
        TEXT    "HELIUM/AMMONIA"
        BYTE    0
        TEXT    "CARBON DIOXIDE"
        BYTE    0
        TEXT    "HYDROGEN/METHANE"
        BYTE    0
        TEXT    "SULPHUR DIOXIDE"
        BYTE    0
        TEXT    "HYDROGEN/AMMONIA"
        BYTE    0
TEMT
        TEXT    "EX.HOT"
        BYTE    0
        TEXT    "V.HOT"
        BYTE    0
        TEXT    "HOT"
        BYTE    0
        TEXT    "TEMPERATE"
        BYTE    0
        TEXT    "COLD"
        BYTE    0
        TEXT    "V.COLD"
        BYTE    0
        TEXT    "EX.COLD"
        BYTE    0
        TEXT    "-----"
        BYTE    0
CIVT
        TEXT    "UNINHABITED"
        BYTE    0
        TEXT    "PRIMITIVE"
        BYTE    0
        TEXT    "AGRICULTURAL"
        BYTE    0
        TEXT    "INDUSTRIAL"
        BYTE    0
        TEXT    "EARLY SPACE TRAVEL"
        BYTE    0
        TEXT    "INTERPLANETARY"
        BYTE    0
        TEXT    "INTERSTELLAR"
        BYTE    0
        TEXT    "SUPER-RACE"
        BYTE    0
ATTT
        TEXT    "UNKNOWN"
        BYTE    0
        TEXT    "V.FRIENDLY"
        BYTE    0
        TEXT    "FRIENDLY"
        BYTE    0
        TEXT    "NEUTRAL"
        BYTE    0
        TEXT    "HOSTILE"
        BYTE    0
        TEXT    "V.HOSTILE"
        BYTE    0
        TEXT    "CONQUERED"
        BYTE    0
SYSNT
        TEXT    "WARP ENGINES"
        BYTE    0
        TEXT    "IMPULSE ENGINES"
        BYTE    0
        TEXT    "PHASER #1"
        BYTE    0
        TEXT    "PHASER #2"
        BYTE    0
        TEXT    "PHOTON TORP #1"
        BYTE    0
        TEXT    "PHOTON TORP #2"
        BYTE    0
        TEXT    "FWD. SHIELD"
        BYTE    0
        TEXT    "STBD SHIELD"
        BYTE    0
        TEXT    "AFT. SHIELD"
        BYTE    0
        TEXT    "PORT SHIELD"
        BYTE    0
        TEXT    "DAMAGE CONTROL"
        BYTE    0
DMGT
        TEXT    "NONE"
        BYTE    0
        TEXT    "MINIMAL"
        BYTE    0
        TEXT    "LOW"
        BYTE    0
        TEXT    "LOW"
        BYTE    0
        TEXT    "MODERATE"
        BYTE    0
        TEXT    "MODERATE"
        BYTE    0
        TEXT    "HEAVY"
        BYTE    0
        TEXT    "SEVERE"
        BYTE    0
        TEXT    "EXTREME"
        BYTE    0
        TEXT    "DESTROYED"
        BYTE    0
RANKT
        TEXT    "CADET"
        BYTE    0
        TEXT    "ENSIGN"
        BYTE    0
        TEXT    "LIEUTENANT"
        BYTE    0
        TEXT    "LT-COMMANDER"
        BYTE    0
        TEXT    "FIRST OFFICER"
        BYTE    0
        TEXT    "STARSHIP CAPTAIN"
        BYTE    0
SOLD
        BYTE    1, 4, 1, 1, 0, 0, 0, 0
        BYTE    1, 5, 4, 1, 0, 0, 0, 0
        BYTE    1, 1, 3, 4, 6, 1, 0, 0
        BYTE    1, 4, 1, 6, 0, 0, 0, 0
        BYTE    2, 1, 4, 8, 0, 0, 0, 0
        BYTE    2, 1, 4, 8, 0, 0, 0, 0
        BYTE    2, 4, 4, 8, 0, 0, 0, 0
        BYTE    2, 4, 4, 8, 0, 0, 0, 0
        BYTE    1, 4, 1, 7, 0, 0, 0, 0
EINIT   WORD    0, 2000, 200, 200, 200, 200, 200, 500, 500, 500, 500, 500
AINIT   BYTE    0, 25, 5, 15, 15, 1, 1, 6, 6, 6, 6, 14
PINIT   BYTE    0, 5, 1, 7, 8, 9, 10, 6, 3, 4, 2
S0       TEXT    "GALACTIC"
        BYTE    0
S1       TEXT    "CHART"
        BYTE    0
S2       TEXT    "SHIP AT"
        BYTE    0
S3       TEXT    "CURSOR"
        BYTE    0
S4       TEXT    "W:WARP"
        BYTE    0
S5       TEXT    "F:INFO"
        BYTE    0
S6       TEXT    "ARROWS"
        BYTE    0
S7       TEXT    "C:CHART S:COMPUTER B:BATTLE I:IMPULSE"
        BYTE    0
S8       TEXT    "RED  "
        BYTE    0
S9       TEXT    "ALERT"
        BYTE    0
S10      TEXT    "GREEN"
        BYTE    0
S11      TEXT    "         "
        BYTE    0
S12      TEXT    "TYPE "
        BYTE    0
S13      TEXT    "NO FURTHER INFORMATION AVAILABLE"
        BYTE    0
S14      TEXT    "CATEGORY    :"
        BYTE    0
S15      TEXT    "ATMOSPHERE  :"
        BYTE    0
S16      TEXT    "SURFACE TEMP:"
        BYTE    0
S17      TEXT    "CIVILISATION:"
        BYTE    0
S18      TEXT    "ATTITUDE    :"
        BYTE    0
S19      TEXT    "GALACTIC INFORMATION"
        BYTE    0
S20      TEXT    " SYSTEM"
        BYTE    0
S21      TEXT    "STAR TYPE : "
        BYTE    0
S22      TEXT    "PLANETS   : "
        BYTE    0
S23      TEXT    "ANY KEY: NEXT   C: CHART"
        BYTE    0
S24      TEXT    "DISTANCE TO "
        BYTE    0
S25      TEXT    " SYSTEM:"
        BYTE    0
S26      TEXT    "ENERGY REQUIRED : "
        BYTE    0
S27      TEXT    " UNITS"
        BYTE    0
S28      TEXT    "WARP ENERGY     : "
        BYTE    0
S29      TEXT    "INSUFFICIENT WARP ENERGY"
        BYTE    0
S30      TEXT    "CONFIRM WARP (Y/N)?"
        BYTE    0
S31      TEXT    "WARP DRIVE   WARP DRIVE   WARP DRIVE"
        BYTE    0
S32      TEXT    "IMPULSE DRIVE"
        BYTE    0
S33      TEXT    "ORBITING "
        BYTE    0
S34      TEXT    "PLANETS IN SYSTEM: "
        BYTE    0
S35      TEXT    "IMPULSE ENERGY   : "
        BYTE    0
S36      TEXT    "PLANET (RETURN = CANCEL)? "
        BYTE    0
S37      TEXT    "INSUFFICIENT IMPULSE ENERGY"
        BYTE    0
S38      TEXT    "IMPULSE      IMPULSE      IMPULSE"
        BYTE    0
S39      TEXT    "                                       "
        BYTE    0
S40      TEXT    "ALIEN FIRE: "
        BYTE    0
S41      TEXT    " HOLDING"
        BYTE    0
S42      TEXT    " DAMAGE!"
        BYTE    0
S43      TEXT    "PRIORITY  PRIORITY  PRIORITY  PRIORITY"
        BYTE    0
S44      TEXT    "MESSAGE RECEIVED FROM"
        BYTE    0
S45      TEXT    "'WELCOME TO OUR STAR SYSTEM,"
        BYTE    0
S46      TEXT    "TRAVELLERS! WE OFFER YOU OUR"
        BYTE    0
S47      TEXT    "HOSPITALITY, AND WILL HELP YOU"
        BYTE    0
S48      TEXT    "IF WE ARE ABLE TO.'"
        BYTE    0
S49      TEXT    "'GREETINGS, ALIENS. WE WILL NOT"
        BYTE    0
S50      TEXT    "HINDER YOUR VOYAGE. IN RETURN WE"
        BYTE    0
S51      TEXT    "ASK THAT YOU HAVE NO FURTHER"
        BYTE    0
S52      TEXT    "CONTACT WITH US.'"
        BYTE    0
S53      TEXT    "'ALIENS, YOU ARE IN ORBIT ABOUT AN"
        BYTE    0
S54      TEXT    "INHABITED WORLD. LEAVE QUICKLY,"
        BYTE    0
S55      TEXT    "OR FACE ATTACK!'"
        BYTE    0
S56      TEXT    "PRESS A KEY"
        BYTE    0
S57      TEXT    "BATTLE"
        BYTE    0
S58      TEXT    "COMPUTER"
        BYTE    0
S59      TEXT    "SCREEN:"
        BYTE    0
S60      TEXT    "WEAPON:"
        BYTE    0
S61      TEXT    "SHIELD:"
        BYTE    0
S62      TEXT    "DMG S/W"
        BYTE    0
S63      TEXT    "+/- SHLD"
        BYTE    0
S64      TEXT    "C:CHART"
        BYTE    0
S65      TEXT    "S:COMP"
        BYTE    0
S66      TEXT    "I:IMPLS"
        BYTE    0
S67      TEXT    "ARROWS AIM 1-4 SCREEN G WPN SPC/F FIRE"
        BYTE    0
S68      TEXT    "ENEMY 1-4"
        BYTE    0
S69      TEXT    "NO ENEMY IN RANGE            "
        BYTE    0
S70      TEXT    "WEAPON DAMAGED - REPAIR IT   "
        BYTE    0
S71      TEXT    "PHASER ENERGY TOO LOW        "
        BYTE    0
S72      TEXT    "NO TORPEDOES - USE G         "
        BYTE    0
S73      TEXT    "MISSED - PUT + ON THE SHIP   "
        BYTE    0
S74      TEXT    "HIT - ENEMY DAMAGED          "
        BYTE    0
S75      TEXT    "        "
        BYTE    0
S76      TEXT    " TP"
        BYTE    0
S77      TEXT    "ENEMY DESTROYED              "
        BYTE    0
S78      TEXT    "ALL ENEMY SHIPS DESTROYED - PRESS A KEY"
        BYTE    0
S79      TEXT    "SHIP'S COMPUTER"
        BYTE    0
S80      TEXT    "T : STATUS"
        BYTE    0
S81      TEXT    "E : ENERGY ALLOCATION"
        BYTE    0
S82      TEXT    "R : REPAIR PRIORITY"
        BYTE    0
S83      TEXT    "D : DIPLOMATIC FUNCTIONS"
        BYTE    0
S84      TEXT    "X : END SCENARIO"
        BYTE    0
S85      TEXT    "H : INSTRUCTIONS"
        BYTE    0
S86      TEXT    "Z : SELF-DESTRUCT"
        BYTE    0
S87      TEXT    "*** RED ALERT ***"
        BYTE    0
S88      TEXT    "CONDITION GREEN"
        BYTE    0
S89      TEXT    "C:CHART B:BATTLE I:IMPULSE"
        BYTE    0
S90      TEXT    "CURRENT STATUS"
        BYTE    0
S91      TEXT    "ALERT: "
        BYTE    0
S92      TEXT    "YELLOW"
        BYTE    0
S93      TEXT    "RED"
        BYTE    0
S94      TEXT    "SCORE: "
        BYTE    0
S95      TEXT    "SYSTEM            ENERGY  DAMAGE"
        BYTE    0
S96      TEXT    "TORPEDOES  TUBE 1: "
        BYTE    0
S97      TEXT    "  TUBE 2: "
        BYTE    0
S98      TEXT    "ENERGY ALLOCATION (%)"
        BYTE    0
S99      TEXT    "CHANGE ALLOCATION (Y/N)?"
        BYTE    0
S100     TEXT    " (0-"
        BYTE    0
S101     TEXT    ")? "
        BYTE    0
S102     TEXT    "    "
        BYTE    0
S103     TEXT    "%"
        BYTE    0
S104     TEXT    "REPAIR PRIORITY"
        BYTE    0
S105     TEXT    "SYSTEMS"
        BYTE    0
S106     TEXT    "SET EACH PRIORITY WITH KEYS 0-9"
        BYTE    0
S107     TEXT    "(0 = SYSTEM 10)"
        BYTE    0
S108     TEXT    "PRIORITY      = ?"
        BYTE    0
S109     TEXT    "                      "
        BYTE    0
S110     TEXT    "DIPLOMATIC FUNCTIONS"
        BYTE    0
S111     TEXT    "1 : CONTACT INHABITANTS"
        BYTE    0
S112     TEXT    "2 : REQUEST REPAIRS"
        BYTE    0
S113     TEXT    "3 : DECLARE WAR"
        BYTE    0
S114     TEXT    "0 : RETURN TO COMPUTER"
        BYTE    0
S115     TEXT    "FUNCTION NO. ?"
        BYTE    0
S116     TEXT    "UNINHABITED PLANET"
        BYTE    0
S117     TEXT    "CONTACT ALREADY ESTABLISHED"
        BYTE    0
S118     TEXT    "LOW LEVEL CIVILISATION: NO INTERFERENCE"
        BYTE    0
S119     TEXT    "MESSAGES BROADCAST: AWAITING RESPONSE"
        BYTE    0
S120     TEXT    "CIVILISATION UNCONTACTED"
        BYTE    0
S121     TEXT    "INSUFFICIENT TECHNOLOGY"
        BYTE    0
S122     TEXT    "REQUEST REFUSED"
        BYTE    0
S123     TEXT    "REPAIRS UNDERWAY"
        BYTE    0
S124     TEXT    "CIVILISATION ALREADY CONQUERED"
        BYTE    0
S125     TEXT    "WAR DECLARED!!"
        BYTE    0
S126     TEXT    "END SCENARIO - ARE YOU SURE (Y/N)?"
        BYTE    0
S127     TEXT    "SELF-DESTRUCT - ARE YOU SURE (Y/N)?"
        BYTE    0
S128     TEXT    "  STARSHIP DESTROYED  "
        BYTE    0
S129     TEXT    "     PRESS A KEY      "
        BYTE    0
S130     TEXT    "FINAL SCORE : "
        BYTE    0
S131     TEXT    "RANK        : "
        BYTE    0
S132     TEXT    "GRADE       : "
        BYTE    0
S133     TEXT    "YOU ARE HEREBY DISMISSED FROM"
        BYTE    0
S134     TEXT    "STARFLEET FOR GENOCIDAL CRIMES"
        BYTE    0
S135     TEXT    "AGAINST AN ALIEN CIVILISATION!"
        BYTE    0
S136     TEXT    "ANOTHER VOYAGE (Y/N)?"
        BYTE    0
S137     TEXT    "TREK - STARSHIP SIMULATOR"
        BYTE    0
S138     TEXT    ""
        BYTE    0
S139     TEXT    "C  GALACTIC CHART     S  SHIP'S COMPUTER"
        BYTE    0
S140     TEXT    "B  BATTLE COMPUTER    I  IMPULSE DRIVE"
        BYTE    0
S141     TEXT    "CHART: ARROWS MOVE THE CURSOR,"
        BYTE    0
S142     TEXT    "W OR RETURN WARPS (2000 UNITS/SECTOR),"
        BYTE    0
S143     TEXT    "F SHOWS SYSTEM AND PLANET DATA."
        BYTE    0
S144     TEXT    "STARS: B BLUE  A WHITE  F,G YELLOW"
        BYTE    0
S145     TEXT    "       K,M RED"
        BYTE    0
S146     TEXT    "10000 UNITS OF ENERGY ARE MADE EVERY"
        BYTE    0
S147     TEXT    "30 SECONDS AND SHARED BY ALLOCATION."
        BYTE    0
S148     TEXT    "BATTLE: ARROWS AIM, 1-4 SCANNERS"
        BYTE    0
S149     TEXT    "(FWD STBD AFT PORT), G OR RETURN"
        BYTE    0
S150     TEXT    "CHANGES WEAPON, SPACE FIRES, + RAISES"
        BYTE    0
S151     TEXT    "THE SHIELD (2500 WARP ENERGY), - LOWERS"
        BYTE    0
S152     TEXT    "SHIELDS UNDER 2000 LET DAMAGE THROUGH."
        BYTE    0
S153     TEXT    "TORPEDOES RELOAD AND DAMAGE IS"
        BYTE    0
S154     TEXT    "REPAIRED IN EARTH ORBIT (SUN III)."
        BYTE    0
S155     TEXT    "1 KLINGON INVASION: CLEAR THE FLEETS"
        BYTE    0
S156     TEXT    "2 GALACTIC WAR: CONQUER THE REBELS"
        BYTE    0
S157     TEXT    "3 FINAL FRONTIER: EXPLORE AND MEET"
        BYTE    0
S158     TEXT    "  NEW CIVILISATIONS."
        BYTE    0
EXIT    SEX     R3
        OUT     3
        BYTE    90H
        SEP     R4
        WORD    CHREST
        SEP     R4
        WORD    CLRSCR
        LDI     HIGH (4195H)
        PHI     RA
        LDI     LOW (4195H)
        PLO     RA
        LDA     RA
        PHI     RF
        LDN     RA
        PLO     RF
        SEX     RF
        OUT     7
        LDI     HIGH (R4OFF)
        PHI     RF
        LDI     LOW (R4OFF)
        PLO     RF
        OUT     4
        LDI     HIGH (R5NORM)
        PHI     RF
        LDI     LOW (R5NORM)
        PLO     RF
        OUT     5
        SEX     R2
        SEP     R4
        WORD    VIDON
        LDI     HIGH (SAVESP)
        PHI     RA
        LDI     LOW (SAVESP)
        PLO     RA
        LDA     RA
        PLO     R2
        LDN     RA
        PHI     R2
        SEX     R3
        RET
        BYTE    23H
        IRX
        LDXA
        PHI     RF
        LDXA
        PLO     RF
        LDXA
        PHI     RE
        LDXA
        PLO     RE
        LDXA
        PHI     RD
        LDXA
        PLO     RD
        LDXA
        PHI     RC
        LDXA
        PLO     RC
        LDXA
        PHI     RB
        LDXA
        PLO     RB
        LDXA
        PHI     RA
        LDXA
        PLO     RA
        LDXA
        PHI     R9
        LDXA
        PLO     R9
        LDXA
        PHI     R8
        LDXA
        PLO     R8
        LDXA
        PHI     R7
        LDXA
        PLO     R7
        LDXA
        PHI     R6
        LDXA
        PLO     R6
        LDXA
        PHI     R5
        LDXA
        PLO     R5
        LDXA
        PHI     R4
        LDX
        PLO     R4
        SEP     R5
STARC   BYTE    0, 01H, 82H, 83H, 83H, 84H, 84H
STARL   BYTE    0, 'B', 'A', 'F', 'G', 'K', 'M'
T_TITLE  TEXT    "STAR TREK"
        BYTE    0
T_SUB    TEXT    "STARSHIP SIMULATOR"
        BYTE    0
T_BY     TEXT    "(C) 1983 I.GRAY"
        BYTE    0
T_M0     TEXT    "0  INSTRUCTIONS"
        BYTE    0
T_M1     TEXT    "1  KLINGON INVASION"
        BYTE    0
T_M2     TEXT    "2  GALACTIC WAR"
        BYTE    0
T_M3     TEXT    "3  THE FINAL FRONTIER"
        BYTE    0
T_ESC    TEXT    "ESC: BACK TO BASIC"
        BYTE    0
T_LEV    TEXT    "LEVEL 1-5 (1 = HARDEST)?"
        BYTE    0
T_BUILD  TEXT    "CONSTRUCTING SCENARIO"
        BYTE    0
T_GAL    TEXT    "GALACTIC"
        BYTE    0
T_CHART  TEXT    "CHART"
        BYTE    0
T_SHIPAT TEXT    "SHIP AT"
        BYTE    0
T_CURS   TEXT    "CURSOR"
        BYTE    0
T_KW     TEXT    "W:WARP"
        BYTE    0
T_KF     TEXT    "F:INFO"
        BYTE    0
T_KA     TEXT    "ARROWS"
        BYTE    0
T_KE     TEXT    "ESC:QUIT"
        BYTE    0
T_BAR    TEXT    "ARROWS MOVE   W WARP   F INFO   ESC QUIT"
        BYTE    0
T_BL9    TEXT    "         "
        BYTE    0
T_TYPE   TEXT    "TYPE "
        BYTE    0
T_WARP   TEXT    "*** WARP DRIVE ***"
        BYTE    0
T_GINF   TEXT    "GALACTIC INFORMATION"
        BYTE    0
T_SYS    TEXT    " SYSTEM"
        BYTE    0
T_STYPE  TEXT    "STAR TYPE : "
        BYTE    0
T_PLAN   TEXT    "PLANETS   : "
        BYTE    0
T_KEY    TEXT    "PRESS A KEY"
        BYTE    0
NAMET   WORD    0
        WORD    N1
        WORD    N2
        WORD    N3
        WORD    N4
        WORD    N5
        WORD    N6
        WORD    N7
        WORD    N8
        WORD    N9
        WORD    N10
        WORD    N11
        WORD    N12
        WORD    N13
        WORD    N14
        WORD    N15
        WORD    N16
        WORD    N17
        WORD    N18
        WORD    N19
        WORD    N20
        WORD    N21
        WORD    N22
        WORD    N23
        WORD    N24
        WORD    N25
        WORD    N26
        WORD    N27
        WORD    N28
        WORD    N29
        WORD    N30
N1       TEXT    "VARGO"
        BYTE    0
N2       TEXT    "MIROS"
        BYTE    0
N3       TEXT    "PRELOS"
        BYTE    0
N4       TEXT    "KAIROS"
        BYTE    0
N5       TEXT    "GALATON"
        BYTE    0
N6       TEXT    "ANDROS"
        BYTE    0
N7       TEXT    "TERRADON"
        BYTE    0
N8       TEXT    "ALGENUS"
        BYTE    0
N9       TEXT    "SKARO"
        BYTE    0
N10      TEXT    "VORTIS"
        BYTE    0
N11      TEXT    "MARINUS"
        BYTE    0
N12      TEXT    "MONDAS"
        BYTE    0
N13      TEXT    "TELOS"
        BYTE    0
N14      TEXT    "AXOS"
        BYTE    0
N15      TEXT    "DAEMOS"
        BYTE    0
N16      TEXT    "PELADON"
        BYTE    0
N17      TEXT    "DRACOS"
        BYTE    0
N18      TEXT    "SPIRODON"
        BYTE    0
N19      TEXT    "SONTAR"
        BYTE    0
N20      TEXT    "REXINDAR"
        BYTE    0
N21      TEXT    "METEBELIS"
        BYTE    0
N22      TEXT    "VOGA"
        BYTE    0
N23      TEXT    "CYBORN"
        BYTE    0
N24      TEXT    "OSIRINIS"
        BYTE    0
N25      TEXT    "RIBOS"
        BYTE    0
N26      TEXT    "CHLORIS"
        BYTE    0
N27      TEXT    "MEGLOS"
        BYTE    0
N28      TEXT    "ALLADAS"
        BYTE    0
N29      TEXT    "MINDORNUS"
        BYTE    0
N30      TEXT    "SUN"
        BYTE    0
REDT
        BYTE    020H, 07CH, 022H, 023H, 024H, 025H, 026H, 027H, 028H, 029H, 060H, 02BH, 02CH, 02DH, 07DH, 02FH
        BYTE    030H, 031H, 032H, 033H, 034H, 035H, 036H, 037H, 038H, 039H, 07BH, 03BH, 03CH, 03DH, 03EH, 03FH
        BYTE    040H, 00AH, 00BH, 00CH, 00DH, 00EH, 00FH, 010H, 011H, 012H, 04AH, 013H, 014H, 015H, 016H, 017H
        BYTE    018H, 051H, 019H, 01AH, 01BH, 01CH, 01DH, 01EH, 058H, 01FH, 05AH, 05BH, 05CH, 05DH, 05EH, 05FH
CHTAB
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 040H, 048H, 06AH, 05CH, 07EH, 05CH, 06AH, 048H
        BYTE    0C0H, 0C8H, 0EAH, 0DCH, 0FEH, 0DCH, 0EAH, 0C8H, 080H, 088H, 0AAH, 09CH, 0BEH, 09CH, 0AAH, 088H
        BYTE    000H, 008H, 02AH, 01CH, 03EH, 01CH, 02AH, 008H, 000H, 021H, 033H, 03FH, 01EH, 00CH, 012H, 000H
        BYTE    040H, 061H, 073H, 07FH, 05EH, 04CH, 052H, 040H, 0C8H, 0C8H, 0C8H, 0C8H, 0C8H, 0C8H, 0C8H, 0C8H
        BYTE    0C0H, 0C0H, 0C0H, 0FFH, 0C0H, 0C0H, 0C0H, 0C0H, 0C8H, 0C8H, 0C8H, 0FFH, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    00EH, 011H, 011H, 01FH, 011H, 011H, 011H, 000H, 01EH, 011H, 011H, 01EH, 011H, 011H, 01EH, 000H
        BYTE    00EH, 011H, 010H, 010H, 010H, 011H, 00EH, 000H, 01EH, 011H, 011H, 011H, 011H, 011H, 01EH, 000H
        BYTE    01FH, 010H, 010H, 01EH, 010H, 010H, 01FH, 000H, 01FH, 010H, 010H, 01EH, 010H, 010H, 010H, 000H
        BYTE    00EH, 011H, 010H, 017H, 011H, 011H, 00FH, 000H, 011H, 011H, 011H, 01FH, 011H, 011H, 011H, 000H
        BYTE    00EH, 004H, 004H, 004H, 004H, 004H, 00EH, 000H, 011H, 012H, 014H, 018H, 014H, 012H, 011H, 000H
        BYTE    010H, 010H, 010H, 010H, 010H, 010H, 01FH, 000H, 011H, 01BH, 015H, 015H, 011H, 011H, 011H, 000H
        BYTE    011H, 019H, 015H, 013H, 011H, 011H, 011H, 000H, 00EH, 011H, 011H, 011H, 011H, 011H, 00EH, 000H
        BYTE    01EH, 011H, 011H, 01EH, 010H, 010H, 010H, 000H, 01EH, 011H, 011H, 01EH, 014H, 012H, 011H, 000H
        BYTE    00FH, 010H, 010H, 00EH, 001H, 001H, 01EH, 000H, 01FH, 004H, 004H, 004H, 004H, 004H, 004H, 000H
        BYTE    011H, 011H, 011H, 011H, 011H, 011H, 00EH, 000H, 011H, 011H, 011H, 011H, 011H, 00AH, 004H, 000H
        BYTE    011H, 011H, 011H, 015H, 015H, 015H, 00AH, 000H, 011H, 011H, 00AH, 004H, 004H, 004H, 004H, 000H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C4H, 0C4H, 0C4H, 0C4H, 0C4H, 0C0H, 0C4H, 0C0H
        BYTE    0CAH, 0CAH, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0CAH, 0CAH, 0DFH, 0CAH, 0DFH, 0CAH, 0CAH, 0C0H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0D8H, 0D9H, 0C2H, 0C4H, 0C8H, 0D3H, 0C3H, 0C0H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C4H, 0C4H, 0C8H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    0C2H, 0C4H, 0C8H, 0C8H, 0C8H, 0C4H, 0C2H, 0C0H, 0C8H, 0C4H, 0C2H, 0C2H, 0C2H, 0C4H, 0C8H, 0C0H
        BYTE    0C0H, 0D5H, 0CEH, 0DFH, 0CEH, 0D5H, 0C0H, 0C0H, 0C0H, 0C4H, 0C4H, 0DFH, 0C4H, 0C4H, 0C0H, 0C0H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0CCH, 0C4H, 0C8H, 0C0H, 0C0H, 0C0H, 0C0H, 0DFH, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0CCH, 0CCH, 0C0H, 0C0H, 0C1H, 0C2H, 0C4H, 0C8H, 0D0H, 0C0H, 0C0H
        BYTE    0CEH, 0D1H, 0D3H, 0D5H, 0D9H, 0D1H, 0CEH, 0C0H, 0C4H, 0CCH, 0C4H, 0C4H, 0C4H, 0C4H, 0CEH, 0C0H
        BYTE    0CEH, 0D1H, 0C1H, 0C2H, 0C4H, 0C8H, 0DFH, 0C0H, 0DFH, 0C2H, 0C4H, 0C2H, 0C1H, 0D1H, 0CEH, 0C0H
        BYTE    0C2H, 0C6H, 0CAH, 0D2H, 0DFH, 0C2H, 0C2H, 0C0H, 0DFH, 0D0H, 0DEH, 0C1H, 0C1H, 0D1H, 0CEH, 0C0H
        BYTE    0C6H, 0C8H, 0D0H, 0DEH, 0D1H, 0D1H, 0CEH, 0C0H, 0DFH, 0C1H, 0C2H, 0C4H, 0C8H, 0C8H, 0C8H, 0C0H
        BYTE    0CEH, 0D1H, 0D1H, 0CEH, 0D1H, 0D1H, 0CEH, 0C0H, 0CEH, 0D1H, 0D1H, 0CFH, 0C1H, 0C2H, 0CCH, 0C0H
        BYTE    0C0H, 0CCH, 0CCH, 0C0H, 0CCH, 0CCH, 0C0H, 0C0H, 0C0H, 0CCH, 0CCH, 0C0H, 0CCH, 0C4H, 0C8H, 0C0H
        BYTE    0C2H, 0C4H, 0C8H, 0D0H, 0C8H, 0C4H, 0C2H, 0C0H, 0C0H, 0C0H, 0DFH, 0C0H, 0DFH, 0C0H, 0C0H, 0C0H
        BYTE    0C8H, 0C4H, 0C2H, 0C1H, 0C2H, 0C4H, 0C8H, 0C0H, 0CEH, 0D1H, 0C1H, 0C2H, 0C4H, 0C0H, 0C4H, 0C0H
        BYTE    0CEH, 0D1H, 0D7H, 0D5H, 0D7H, 0D0H, 0CFH, 0C0H, 0CEH, 0D1H, 0D1H, 0DFH, 0D1H, 0D1H, 0D1H, 0C0H
        BYTE    0DEH, 0D1H, 0D1H, 0DEH, 0D1H, 0D1H, 0DEH, 0C0H, 0CEH, 0D1H, 0D0H, 0D0H, 0D0H, 0D1H, 0CEH, 0C0H
        BYTE    0DEH, 0D1H, 0D1H, 0D1H, 0D1H, 0D1H, 0DEH, 0C0H, 0DFH, 0D0H, 0D0H, 0DEH, 0D0H, 0D0H, 0DFH, 0C0H
        BYTE    0DFH, 0D0H, 0D0H, 0DEH, 0D0H, 0D0H, 0D0H, 0C0H, 0CEH, 0D1H, 0D0H, 0D7H, 0D1H, 0D1H, 0CFH, 0C0H
        BYTE    0D1H, 0D1H, 0D1H, 0DFH, 0D1H, 0D1H, 0D1H, 0C0H, 0CEH, 0C4H, 0C4H, 0C4H, 0C4H, 0C4H, 0CEH, 0C0H
        BYTE    0C7H, 0C2H, 0C2H, 0C2H, 0C2H, 0D2H, 0CCH, 0C0H, 0D1H, 0D2H, 0D4H, 0D8H, 0D4H, 0D2H, 0D1H, 0C0H
        BYTE    0D0H, 0D0H, 0D0H, 0D0H, 0D0H, 0D0H, 0DFH, 0C0H, 0D1H, 0DBH, 0D5H, 0D5H, 0D1H, 0D1H, 0D1H, 0C0H
        BYTE    0D1H, 0D9H, 0D5H, 0D3H, 0D1H, 0D1H, 0D1H, 0C0H, 0CEH, 0D1H, 0D1H, 0D1H, 0D1H, 0D1H, 0CEH, 0C0H
        BYTE    0DEH, 0D1H, 0D1H, 0DEH, 0D0H, 0D0H, 0D0H, 0C0H, 0CEH, 0D1H, 0D1H, 0D1H, 0D5H, 0D2H, 0CDH, 0C0H
        BYTE    0DEH, 0D1H, 0D1H, 0DEH, 0D4H, 0D2H, 0D1H, 0C0H, 0CFH, 0D0H, 0D0H, 0CEH, 0C1H, 0C1H, 0DEH, 0C0H
        BYTE    0DFH, 0C4H, 0C4H, 0C4H, 0C4H, 0C4H, 0C4H, 0C0H, 0D1H, 0D1H, 0D1H, 0D1H, 0D1H, 0D1H, 0CEH, 0C0H
        BYTE    0D1H, 0D1H, 0D1H, 0D1H, 0D1H, 0CAH, 0C4H, 0C0H, 0D1H, 0D1H, 0D1H, 0D5H, 0D5H, 0D5H, 0CAH, 0C0H
        BYTE    0D1H, 0D1H, 0CAH, 0C4H, 0CAH, 0D1H, 0D1H, 0C0H, 0D1H, 0D1H, 0CAH, 0C4H, 0C4H, 0C4H, 0C4H, 0C0H
        BYTE    0DFH, 0C1H, 0C2H, 0C4H, 0C8H, 0D0H, 0DFH, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0DFH, 0C0H
        BYTE    000H, 015H, 00EH, 01FH, 00EH, 015H, 000H, 000H, 08EH, 091H, 091H, 09FH, 091H, 091H, 091H, 080H
        BYTE    09EH, 091H, 091H, 09EH, 091H, 091H, 09EH, 080H, 08EH, 091H, 090H, 090H, 090H, 091H, 08EH, 080H
        BYTE    09EH, 091H, 091H, 091H, 091H, 091H, 09EH, 080H, 09FH, 090H, 090H, 09EH, 090H, 090H, 09FH, 080H
        BYTE    09FH, 090H, 090H, 09EH, 090H, 090H, 090H, 080H, 08EH, 091H, 090H, 097H, 091H, 091H, 08FH, 080H
        BYTE    091H, 091H, 091H, 09FH, 091H, 091H, 091H, 080H, 08EH, 084H, 084H, 084H, 084H, 084H, 08EH, 080H
        BYTE    087H, 082H, 082H, 082H, 082H, 092H, 08CH, 080H, 091H, 092H, 094H, 098H, 094H, 092H, 091H, 080H
        BYTE    090H, 090H, 090H, 090H, 090H, 090H, 09FH, 080H, 091H, 09BH, 095H, 095H, 091H, 091H, 091H, 080H
        BYTE    091H, 099H, 095H, 093H, 091H, 091H, 091H, 080H, 08EH, 091H, 091H, 091H, 091H, 091H, 08EH, 080H
        BYTE    09EH, 091H, 091H, 09EH, 090H, 090H, 090H, 080H, 08EH, 091H, 091H, 091H, 095H, 092H, 08DH, 080H
        BYTE    09EH, 091H, 091H, 09EH, 094H, 092H, 091H, 080H, 08FH, 090H, 090H, 08EH, 081H, 081H, 09EH, 080H
        BYTE    09FH, 084H, 084H, 084H, 084H, 084H, 084H, 080H, 091H, 091H, 091H, 091H, 091H, 091H, 08EH, 080H
        BYTE    091H, 091H, 091H, 091H, 091H, 08AH, 084H, 080H, 091H, 091H, 091H, 095H, 095H, 095H, 08AH, 080H
        BYTE    091H, 091H, 08AH, 084H, 08AH, 091H, 091H, 080H, 091H, 091H, 08AH, 084H, 084H, 084H, 084H, 080H
        BYTE    09FH, 081H, 082H, 084H, 088H, 090H, 09FH, 080H, 000H, 00CH, 00CH, 000H, 00CH, 00CH, 000H, 000H
        BYTE    004H, 004H, 004H, 004H, 004H, 000H, 004H, 000H, 000H, 000H, 000H, 000H, 000H, 00CH, 00CH, 000H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
ENDCODE EQU     $
        END
