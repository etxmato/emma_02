;=====================================================================
; SL.ASM - STAR LEGION for the COMX-35 (1802 machine code, A18 syntax)
; An original space shooter in four missions.  Start: CALL(@4500).
;=====================================================================
PAGE    EQU     0F800H
CRAM    EQU     0F400H
R3ON    EQU     000C0H
R3OFF   EQU     000D0H
R4OFF   EQU     00080H
R5NORM  EQU     08080H
R5CMEM  EQU     08081H
LDCELL  EQU     00089H
K_UP    EQU     00082H
K_RIGHT EQU     00083H
K_LEFT  EQU     00084H
K_DOWN  EQU     00085H
K_ESC   EQU     00081H
K_CR    EQU     00080H
BGBUF   EQU     0A000H
OMAP    EQU     0A400H
QBUF    EQU     0A900H
CSAVE   EQU     09C00H
EX      EQU     0A800H
EY      EQU     0A820H
ET      EQU     0A840H
EA      EQU     0A860H
EB      EQU     0A880H
EOX     EQU     0A8A0H
EOY     EQU     0A8C0H
EON     EQU     0A8E0H
SAVESP  EQU     0AA00H
RND     EQU     0AA02H
LASTK   EQU     0AA04H
RPT     EQU     0AA05H
QN      EQU     0AA06H
QR      EQU     0AA07H
QRO     EQU     0AA08H
SNDT    EQU     0AA09H
SNDS    EQU     0AA0AH
SNDW    EQU     0AA0BH
NSDT    EQU     0AA0DH
MUST    EQU     0AA0EH
MUSP    EQU     0AA0FH
MUTE    EQU     0AA11H
GOVER   EQU     0AA12H
SCORE   EQU     0AA13H
HISC    EQU     0AA19H
LIVES   EQU     0AA1FH
MISSION EQU     0AA20H
CYCLE   EQU     0AA21H
HUDF    EQU     0AA22H
FRM     EQU     0AA23H
PX      EQU     0AA25H
POX     EQU     0AA26H
PON     EQU     0AA27H
HELD    EQU     0AA28H
AUTO    EQU     0AA29H
SX      EQU     0AA2AH
SY      EQU     0AA2BH
SON     EQU     0AA2CH
DEAD    EQU     0AA2DH
DTMR    EQU     0AA2EH
NALIVE  EQU     0AA2FH
GX      EQU     0AA30H
GY      EQU     0AA31H
GDX     EQU     0AA32H
GROW    EQU     0AA33H
GTMR    EQU     0AA34H
GSPD    EQU     0AA35H
MTMR    EQU     0AA36H
FTMR    EQU     0AA37H
BEAMT   EQU     0AA38H
BEAMX   EQU     0AA39H
BEAMS   EQU     0AA3AH
WVN     EQU     0AA3BH
WVT     EQU     0AA3CH
VANG    EQU     0AA3DH
CMX     EQU     0AA3EH
CMDX    EQU     0AA3FH
CMTMR   EQU     0AA40H
CMSH    EQU     0AA41H
WIN     EQU     0AA48H
WTMR    EQU     0AA49H
TMPA    EQU     0AA4AH
TMPB    EQU     0AA4BH
KCNT    EQU     0AA4CH
FIRET   EQU     0AA4DH
ANIM    EQU     0AA4EH
BX      EQU     0AA4FH
BY      EQU     0AA55H
BON     EQU     0AA5BH
BOX     EQU     0AA61H
BOY     EQU     0AA67H
EXX     EQU     0AA6DH
EXY     EQU     0AA71H
EXT     EQU     0AA75H
        ORG     4500H
START   SEX     R2
        GLO     R1
        STXD
        GHI     R1
        STXD
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
        LDI     HIGH (QP1)
        PHI     R1
        LDI     LOW (QP1)
        PLO     R1
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
        SEP     R5
VIDOFF  SEX     R3
        OUT     3
        BYTE    R3OFF
        SEX     R2
        SEP     R5
; WAITVB - return at the start of the non-display period
WAITVB
WV1     BN1     WV1
WV2     B1      WV2
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
; CLRSCR - blank the page (display off)
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
; POS - RE.1 = row, RE.0 = col -> RD = page address (uses RA)
POS     GHI     RE
        SHL
        ADI     LOW ROWTAB
        PLO     RA
        LDI     HIGH ROWTAB
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     RD
        LDN     RA
        STR     R2
        GLO     RE
        ADD
        PLO     RD
        GHI     RD
        ADCI    0
        PHI     RD
        SEP     R5
; PRT - RC = text, RD = page address, RF.0 = OR mask (80H = PCB).  Writes directly (display off)
PRT     LDA     RC
        LBZ     PRX
        STR     R2
        GLO     RF
        OR
        STR     RD
        INC     RD
        LBR     PRT
PRX     SEP     R5
; RAND - D = next pseudo-random byte (16-bit LFSR)
RAND
        LDI     HIGH (RND)
        PHI     RA
        LDI     LOW (RND)
        PLO     RA
        LDA     RA
        PHI     R7
        LDN     RA
        PLO     R7
        GHI     R7
        SHR
        PHI     R7
        GLO     R7
        SHRC
        PLO     R7
        LBNF    RD1
        GHI     R7
        XRI     0B4H
        PHI     R7
RD1     DEC     RA
        GHI     R7
        STR     RA
        INC     RA
        GLO     R7
        STR     RA
        SEP     R5
GETKEY  BN3     GKN
        LBR     GK1
GKN
        LDI     HIGH (LASTK)
        PHI     RA
        LDI     LOW (LASTK)
        PLO     RA
        LDN     RA
        LBZ     GKX
        B2      GKH
        LDI     0
        STR     RA
        LBR     GKX
GKH
        LDI     HIGH (RPT)
        PHI     RB
        LDI     LOW (RPT)
        PLO     RB
        LDN     RB
        SMI     1
        STR     RB
        LBNZ    GKX
        LDI     3
        STR     RB
        LDN     RA
        SEP     R5
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
        LDI     10
        STR     RB
GK3     GLO     R7
        SEP     R5
; SOUND - RC = tone word, D = frames, SNDS = sweep step
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
; NOISE - RC = noise word (high byte, low byte 80H), D = frames
NOISE
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
SNDTICK
        LDI     HIGH (SNDT)
        PHI     RA
        LDI     LOW (SNDT)
        PLO     RA
        LDN     RA
        LBZ     ST3
        SMI     1
        STR     RA
        LBNZ    ST2
        LDI     HIGH (R4OFF)
        PHI     RF
        LDI     LOW (R4OFF)
        PLO     RF
        SEX     RF
        OUT     4
        SEX     R2
        LBR     ST3
ST2
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDN     RA
        LBZ     ST3
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
ST3
        LDI     HIGH (NSDT)
        PHI     RA
        LDI     LOW (NSDT)
        PLO     RA
        LDN     RA
        LBZ     ST4
        SMI     1
        STR     RA
        LBNZ    ST4
        LDI     HIGH (R5NORM)
        PHI     RF
        LDI     LOW (R5NORM)
        PLO     RF
        SEX     RF
        OUT     5
        SEX     R2
ST4     SEP     R5
SILENCE
        LDI     HIGH (SNDT)
        PHI     RA
        LDI     LOW (SNDT)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (NSDT)
        PHI     RA
        LDI     LOW (NSDT)
        PLO     RA
        LDI     0
        STR     RA
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
        SEX     R2
        SEP     R5
QPRET   SEP     R3
QP1     PLO     R7
        GHI     RE
        SHL
        ADI     LOW ROWTAB
        PLO     RC
        LDI     HIGH ROWTAB
        ADCI    0
        PHI     RC
        LDA     RC
        PHI     RD
        LDN     RC
        STR     R2
        GLO     RE
        ADD
        PLO     RD
        GHI     RD
        ADCI    0
        PHI     RD
        GLO     R7
        LBNZ    QP2
        GLO     RD
        PLO     RC
        GHI     RD
        ADI     0A8H
        PHI     RC
        LDN     RC
        PLO     R7
QP2
        LDI     HIGH (QN)
        PHI     RA
        LDI     LOW (QN)
        PLO     RA
        LDN     RA
        SMI     78
        LBDF    QPRET
        LDN     RA
        ADI     1
        STR     RA
        SMI     1
        STR     R2
        SHL
        ADD
        PLO     RA
        LDI     HIGH QBUF
        PHI     RA
        GHI     RD
        STR     RA
        INC     RA
        GLO     RD
        STR     RA
        INC     RA
        GLO     R7
        STR     RA
        LBR     QPRET
VBDRAW
        LDI     HIGH (QR)
        PHI     RA
        LDI     LOW (QR)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (QN)
        PHI     RA
        LDI     LOW (QN)
        PLO     RA
        LDN     RA
        SM
        LBZ     VBE
        PLO     R8
        LDI     HIGH (QRO)
        PHI     RA
        LDI     LOW (QRO)
        PLO     RA
        LDN     RA
        PLO     RA
        LDI     HIGH QBUF
        PHI     RA
VBQ     LDA     RA
        PHI     RD
        LDA     RA
        PLO     RD
        LDA     RA
        STR     RD
        BN1     VBQ1
        DEC     RA
        DEC     RA
        DEC     RA
        LBR     VBS
VBQ1    DEC     R8
        GLO     R8
        LBNZ    VBQ
VBE
        LDI     HIGH (QN)
        PHI     RA
        LDI     LOW (QN)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QR)
        PHI     RA
        LDI     LOW (QR)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QRO)
        PHI     RA
        LDI     LOW (QRO)
        PLO     RA
        LDI     0
        STR     RA
        SEP     R5
VBS     GLO     RA
        PLO     R7
        LDI     HIGH (QRO)
        PHI     RA
        LDI     LOW (QRO)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (QN)
        PHI     RA
        LDI     LOW (QN)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R8
        SD
        PLO     R7
        LDI     HIGH (QR)
        PHI     RA
        LDI     LOW (QR)
        PLO     RA
        GLO     R7
        STR     RA
        SEP     R5
OMAPA   GHI     RE
        SHL
        ADI     LOW ROWTAB
        PLO     RC
        LDI     HIGH ROWTAB
        ADCI    0
        PHI     RC
        LDA     RC
        PHI     R7
        LDN     RC
        STR     R2
        GLO     RE
        ADD
        PLO     RC
        GHI     R7
        ADCI    0ACH
        PHI     RC
        SEP     R5
; ESHOW - (re)draw enemy R8 at EY/EX
ESHOW
        GLO     R8
        ADI     LOW EON
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     ES2
        SEP     R4
        WORD    EERASE
ES2
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     R9
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PHI     RE
        SMI     22
        LBDF    ESX
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RE
        SMI     40
        LBDF    ESX
        GLO     R9
        SHL
        STR     R2
        LDI     HIGH (ANIM)
        PHI     RA
        LDI     LOW (ANIM)
        PLO     RA
        LDN     RA
        ANI     1
        ADD
        ADI     LOW (CHART-2)
        PLO     RC
        LDI     HIGH (CHART-2)
        ADCI    0
        PHI     RC
        LDN     RC
        SEP     R1
        SEP     R4
        WORD    OMAPA
        GLO     R8
        ADI     1
        STR     RC
        GLO     R9
        ADI     LOW (CHART2-1)
        PLO     RC
        LDI     HIGH (CHART2-1)
        ADCI    0
        PHI     RC
        LDN     RC
        LBZ     ES3
        INC     RE
        SEP     R1
        SEP     R4
        WORD    OMAPA
        GLO     R8
        ADI     1
        STR     RC
        DEC     RE
ES3
        GLO     R8
        ADI     LOW EOX
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     RE
        STR     RA
        GLO     R8
        ADI     LOW EOY
        PLO     RA
        LDI     0A8H
        PHI     RA
        GHI     RE
        STR     RA
        GLO     R8
        ADI     LOW EON
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     1
        STR     RA
ESX     SEP     R5
EERASE
        GLO     R8
        ADI     LOW EON
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     EEX
        LDI     0
        STR     RA
        GLO     R8
        ADI     LOW EOY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PHI     RE
        GLO     R8
        ADI     LOW EOX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     0
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0
        STR     RC
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     LOW (EWIDT-1)
        PLO     RC
        LDI     HIGH (EWIDT-1)
        ADCI    0
        PHI     RC
        LDN     RC
        XRI     2
        LBNZ    EEX
        INC     RE
        LDI     0
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0
        STR     RC
EEX     SEP     R5
EKILL
        SEP     R4
        WORD    EERASE
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     R9
        LDI     0
        STR     RA
        GLO     R9
        XRI     4
        LBZ     EK0
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
EK0     GLO     R9
        ADI     LOW (ESCT-1)
        PLO     RC
        LDI     HIGH (ESCT-1)
        ADCI    0
        PHI     RC
        LDN     RC
        SEP     R4
        WORD    ADDSC
EK1
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PHI     RE
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RE
        SEP     R4
        WORD    EFFECT
        LDI     HIGH (3F80H)
        PHI     RC
        LDI     LOW (3F80H)
        PLO     RC
        LDI     12
        SEP     R4
        WORD    NOISE
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     3
        STR     RA
        LDI     HIGH (201FH)
        PHI     RC
        LDI     LOW (201FH)
        PLO     RC
        LDI     10
        SEP     R4
        WORD    SOUND
        SEP     R5
EFFECT  LDI     0
        PLO     RB
EF1L    GLO     RB
        ADI     LOW EXT
        PLO     RA
        LDI     HIGH EXT
        PHI     RA
        LDN     RA
        LBZ     EF2
        INC     RB
        GLO     RB
        XRI     4
        LBNZ    EF1L
        SEP     R5
EF2     LDI     12
        STR     RA
        GLO     RB
        ADI     LOW EXX
        PLO     RA
        GLO     RE
        STR     RA
        GLO     RB
        ADI     LOW EXY
        PLO     RA
        GHI     RE
        STR     RA
        LDI     231
        SEP     R1
        SEP     R5
EFFECTS LDI     0
        PLO     RB
EFL     GLO     RB
        ADI     LOW EXT
        PLO     RA
        LDI     HIGH EXT
        PHI     RA
        LDN     RA
        LBZ     EFN
        SMI     1
        STR     RA
        PLO     R9
        GLO     RB
        ADI     LOW EXX
        PLO     RA
        LDN     RA
        PLO     RE
        GLO     RB
        ADI     LOW EXY
        PLO     RA
        LDN     RA
        PHI     RE
        GLO     R9
        LBNZ    EFL2
        SEP     R1
        LBR     EFN
EFL2    GLO     R9
        XRI     6
        LBNZ    EFN
        LDI     232
        SEP     R1
EFN     INC     RB
        GLO     RB
        XRI     4
        LBNZ    EFL
        SEP     R5
ADDSC   LBZ     ASX
        PLO     R9
AS1
        LDI     HIGH (SCORE+4)
        PHI     RA
        LDI     LOW (SCORE+4)
        PLO     RA
AS2     LDN     RA
        ADI     1
        STR     RA
        SMI     10
        LBNF    AS3
        STR     RA
        DEC     RA
        GLO     RA
        XRI     LOW (SCORE-1)
        LBNZ    AS2
AS3     DEC     R9
        GLO     R9
        LBNZ    AS1
        LDI     HIGH (HUDF)
        PHI     RA
        LDI     LOW (HUDF)
        PLO     RA
        LDI     1
        STR     RA
ASX     SEP     R5
HUDSC
        LDI     HIGH (HUDF)
        PHI     RA
        LDI     LOW (HUDF)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (SCORE)
        PHI     RB
        LDI     LOW (SCORE)
        PLO     RB
        LDI     HIGH (6)
        PHI     RE
        LDI     LOW (6)
        PLO     RE
HS1     LDA     RB
        ADI     0B0H
        SEP     R1
        INC     RE
        GLO     RE
        XRI     12
        LBNZ    HS1
        SEP     R5
; BFIRE - enemy bullet from RE (row,col)
BFIRE   GLO     RE
        SMI     40
        LBNF    BF0
        SEP     R5
BF0     LDI     0
        PLO     RB
BF1
        GLO     RB
        ADI     LOW BON
        PLO     RA
        LDI     HIGH BON
        PHI     RA
        LDN     RA
        LBZ     BF2
        INC     RB
        GLO     RB
        XRI     6
        LBNZ    BF1
        SEP     R5
BF2     LDI     2
        STR     RA
        GLO     RB
        ADI     LOW BX
        PLO     RA
        LDI     HIGH BX
        PHI     RA
        GLO     RE
        STR     RA
        GLO     RB
        ADI     LOW BY
        PLO     RA
        LDI     HIGH BY
        PHI     RA
        GHI     RE
        STR     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (3A18H)
        PHI     RC
        LDI     LOW (3A18H)
        PLO     RC
        LDI     3
        SEP     R4
        WORD    SOUND
        SEP     R5
; BULLETS - move every 2nd frame
BULLETS
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        ANI     1
        LBNZ    BUX
        LDI     0
        PLO     R8
BU0
        GLO     R8
        ADI     LOW BON
        PLO     RA
        LDI     HIGH BON
        PHI     RA
        LDN     RA
        LBZ     BUN
        XRI     2
        LBZ     BU1
        GLO     R8
        ADI     LOW BY
        PLO     RA
        LDI     HIGH BY
        PHI     RA
        LDN     RA
        PHI     RE
        GLO     R8
        ADI     LOW BX
        PLO     RA
        LDI     HIGH BX
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     0
        SEP     R1
        GLO     R8
        ADI     LOW BY
        PLO     RA
        LDI     HIGH BY
        PHI     RA
        LDN     RA
        ADI     1
        STR     RA
BU1
        GLO     R8
        ADI     LOW BON
        PLO     RA
        LDI     HIGH BON
        PHI     RA
        LDI     1
        STR     RA
        GLO     R8
        ADI     LOW BY
        PLO     RA
        LDI     HIGH BY
        PHI     RA
        LDN     RA
        PHI     RE
        SMI     22
        LBDF    BUK
        GLO     R8
        ADI     LOW BX
        PLO     RA
        LDI     HIGH BX
        PHI     RA
        LDN     RA
        PLO     RE
        GHI     RE
        XRI     21
        LBNZ    BU2
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     RE
        SM
        ADI     1
        SMI     3
        LBDF    BU2
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDN     RA
        LBNZ    BU2
        SEP     R4
        WORD    KILLP
        LBR     BUK
BU2
        SEP     R4
        WORD    OMAPA
        LDN     RC
        XRI     0F0H
        LBNZ    BU3
        SEP     R4
        WORD    FDAMAGE
        LBR     BUK
BU3
        GLO     R8
        ADI     LOW BY
        PLO     RA
        LDI     HIGH BY
        PHI     RA
        LDN     RA
        PHI     RE
        GLO     R8
        ADI     LOW BX
        PLO     RA
        LDI     HIGH BX
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     229
        SEP     R1
        LBR     BUN
BUK
        GLO     R8
        ADI     LOW BON
        PLO     RA
        LDI     HIGH BON
        PHI     RA
        LDI     0
        STR     RA
BUN     INC     R8
        GLO     R8
        XRI     6
        LBNZ    BU0
BUX     SEP     R5
FDAMAGE GLO     RC
        PLO     RB
        GHI     RC
        SMI     4
        PHI     RB
        LDN     RB
        XRI     141
        LBZ     FD1
        LDI     20H
        STR     RB
        LDI     0
        STR     RC
        LDI     20H
        SEP     R1
        SEP     R5
FD1     LDI     142
        STR     RB
        SEP     R1
        SEP     R5
; PLAYER - keys, movement, shot
PLAYER  BN3     PL1
        SEX     R2
        INP     3
        PLO     R9
        SMI     61H
        LBNF    PL0
        GLO     R9
        SMI     7BH
        LBDF    PL0
        GLO     R9
        SMI     20H
        PLO     R9
PL0     GLO     R9
        XRI     K_LEFT
        LBZ     PLH
        GLO     R9
        XRI     K_RIGHT
        LBZ     PLH
        GLO     R9
        XRI     K_ESC
        LBNZ    PL0A
        LDI     HIGH (GOVER)
        PHI     RA
        LDI     LOW (GOVER)
        PLO     RA
        LDI     2
        STR     RA
        SEP     R5
PL0A    GLO     R9
        XRI     'A'
        LBNZ    PL0B
        LDI     HIGH (AUTO)
        PHI     RA
        LDI     LOW (AUTO)
        PLO     RA
        LDN     RA
        XRI     1
        STR     RA
        LBR     PL1
PL0B
        SEP     R4
        WORD    FIRE
        LBR     PL1
PLH     GLO     R9
        PLO     R7
        LDI     HIGH (HELD)
        PHI     RA
        LDI     LOW (HELD)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (KCNT)
        PHI     RA
        LDI     LOW (KCNT)
        PLO     RA
        LDI     0
        STR     RA
PL1
        LDI     HIGH (HELD)
        PHI     RA
        LDI     LOW (HELD)
        PLO     RA
        LDN     RA
        LBZ     PL5
        B2      PL2
        LDI     HIGH (HELD)
        PHI     RA
        LDI     LOW (HELD)
        PLO     RA
        LDI     0
        STR     RA
        LBR     PL5
PL2
        LDI     HIGH (KCNT)
        PHI     RA
        LDI     LOW (KCNT)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        ANI     1
        LBZ     PL5
        LDI     HIGH (HELD)
        PHI     RA
        LDI     LOW (HELD)
        PLO     RA
        LDN     RA
        XRI     K_LEFT
        LBNZ    PL3
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        SMI     2
        LBNF    PL5
        ADI     1
        STR     RA
        LBR     PL5
PL3
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        SMI     37
        LBDF    PL5
        ADI     38
        STR     RA
PL5
        LDI     HIGH (AUTO)
        PHI     RA
        LDI     LOW (AUTO)
        PLO     RA
        LDN     RA
        LBZ     PL6
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDN     RA
        LBNZ    PL6
        SEP     R4
        WORD    FIRE
PL6
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (POX)
        PHI     RA
        LDI     LOW (POX)
        PLO     RA
        LDN     RA
        XOR
        LBNZ    PL7
        LDI     HIGH (PON)
        PHI     RA
        LDI     LOW (PON)
        PLO     RA
        LDN     RA
        LBNZ    PLX
PL7
        LDI     HIGH (PON)
        PHI     RA
        LDI     LOW (PON)
        PLO     RA
        LDN     RA
        LBZ     PL8
        LDI     HIGH (POX)
        PHI     RA
        LDI     LOW (POX)
        PLO     RA
        LDN     RA
        SMI     1
        PLO     RE
        LDI     21
        PHI     RE
        LDI     0
        SEP     R1
        INC     RE
        LDI     0
        SEP     R1
        INC     RE
        LDI     0
        SEP     R1
        INC     RE
PL8
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        STR     R2
        PLO     R7
        LDI     HIGH (POX)
        PHI     RA
        LDI     LOW (POX)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (PON)
        PHI     RA
        LDI     LOW (PON)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        SMI     1
        PLO     RE
        LDI     21
        PHI     RE
        LDI     129
        SEP     R1
        INC     RE
        LDI     130
        SEP     R1
        INC     RE
        LDI     131
        SEP     R1
        INC     RE
PLX     SEP     R5
FIRE
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDN     RA
        LBNZ    FIX
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDN     RA
        LBZ     FI1
        LDI     HIGH (SY)
        PHI     RA
        LDI     LOW (SY)
        PLO     RA
        LDN     RA
        PHI     RE
        LDI     HIGH (SX)
        PHI     RA
        LDI     LOW (SX)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     0
        SEP     R1
FI1
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        PLO     R7
        LDI     HIGH (SX)
        PHI     RA
        LDI     LOW (SX)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (SY)
        PHI     RA
        LDI     LOW (SY)
        PLO     RA
        LDI     21
        STR     RA
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDI     2
        STR     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (0A3FH)
        PHI     RC
        LDI     LOW (0A3FH)
        PLO     RC
        LDI     6
        SEP     R4
        WORD    SOUND
FIX     SEP     R5
SHOT
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDN     RA
        LBZ     SHX
        XRI     2
        LBZ     SH1
        LDI     HIGH (SY)
        PHI     RA
        LDI     LOW (SY)
        PLO     RA
        LDN     RA
        PHI     RE
        LDI     HIGH (SX)
        PHI     RA
        LDI     LOW (SX)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     0
        SEP     R1
SH1
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (SY)
        PHI     RA
        LDI     LOW (SY)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBZ     SHK
        PHI     RE
        LDI     HIGH (SX)
        PHI     RA
        LDI     LOW (SX)
        PLO     RA
        LDN     RA
        PLO     RE
        SEP     R4
        WORD    OMAPA
        LDN     RC
        LBZ     SH9
        PLO     R8
        SMI     0F0H
        LBZ     SH9
        GLO     R8
        SMI     0E0H
        LBDF    SHC
        DEC     R8
        SEP     R4
        WORD    EKILL
        LBR     SHK
SHC
        SEP     R4
        WORD    CMDHIT
        LBR     SHK
SH9     LDI     132
        SEP     R1
        SEP     R5
SHK
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDI     0
        STR     RA
SHX     SEP     R5
KILLP
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDN     RA
        LBNZ    KPX
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (DTMR)
        PHI     RA
        LDI     LOW (DTMR)
        PLO     RA
        LDI     100
        STR     RA
        LDI     HIGH (1F80H)
        PHI     RC
        LDI     LOW (1F80H)
        PLO     RC
        LDI     60
        SEP     R4
        WORD    NOISE
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     2
        STR     RA
        LDI     HIGH (101FH)
        PHI     RC
        LDI     LOW (101FH)
        PLO     RC
        LDI     50
        SEP     R4
        WORD    SOUND
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDN     RA
        LBZ     KP1
        LDI     HIGH (SY)
        PHI     RA
        LDI     LOW (SY)
        PLO     RA
        LDN     RA
        PHI     RE
        LDI     HIGH (SX)
        PHI     RA
        LDI     LOW (SX)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     0
        SEP     R1
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDI     0
        STR     RA
KP1
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     21
        PHI     RE
        SEP     R4
        WORD    EFFECT
        DEC     RE
        SEP     R4
        WORD    EFFECT
        INC     RE
        INC     RE
        SEP     R4
        WORD    EFFECT
KPX     SEP     R5
DEATH
        LDI     HIGH (DTMR)
        PHI     RA
        LDI     LOW (DTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    DTX
        LDI     HIGH (POX)
        PHI     RA
        LDI     LOW (POX)
        PLO     RA
        LDN     RA
        SMI     1
        PLO     RE
        LDI     21
        PHI     RE
        LDI     0
        SEP     R1
        INC     RE
        LDI     0
        SEP     R1
        INC     RE
        LDI     0
        SEP     R1
        INC     RE
        LDI     HIGH (PON)
        PHI     RA
        LDI     LOW (PON)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (LIVES)
        PHI     RA
        LDI     LOW (LIVES)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    DT1
        LDI     HIGH (GOVER)
        PHI     RA
        LDI     LOW (GOVER)
        PLO     RA
        LDI     1
        STR     RA
        SEP     R5
DT1
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDI     20
        STR     RA
        SEP     R4
        WORD    LIVESHOW
        SEP     R4
        WORD    CLRBUL
DTX     SEP     R5
CLRBUL  LDI     0
        PLO     R8
CB1
        GLO     R8
        ADI     LOW BON
        PLO     RA
        LDI     HIGH BON
        PHI     RA
        LDN     RA
        LBZ     CB2
        LDI     0
        STR     RA
        GLO     R8
        ADI     LOW BY
        PLO     RA
        LDI     HIGH BY
        PHI     RA
        LDN     RA
        PHI     RE
        GLO     R8
        ADI     LOW BX
        PLO     RA
        LDI     HIGH BX
        PHI     RA
        LDN     RA
        PLO     RE
        LDI     0
        SEP     R1
CB2     INC     R8
        GLO     R8
        XRI     6
        LBNZ    CB1
        SEP     R5
LIVESHOW
        LDI     HIGH (36)
        PHI     RE
        LDI     LOW (36)
        PLO     RE
        LDI     HIGH (LIVES)
        PHI     RA
        LDI     LOW (LIVES)
        PLO     RA
        LDN     RA
        PLO     R9
        LDI     4
        PLO     RB
LS1     GLO     R9
        SMI     2
        LBNF    LS2
        DEC     R9
        LDI     235
        LBR     LS3
LS2     LDI     20H
LS3     SEP     R1
        INC     RE
        DEC     RB
        GLO     RB
        LBNZ    LS1
        SEP     R5
RANDN   PLO     RB
        SEP     R4
        WORD    RAND
        STR     R2
RN1     GLO     RB
        SD
        LBNF    RN2
        STR     R2
        LBR     RN1
RN2     LDN     R2
        SEP     R5
M1INIT
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDI     28
        STR     RA
        LDI     HIGH (GX)
        PHI     RA
        LDI     LOW (GX)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (GY)
        PHI     RA
        LDI     LOW (GY)
        PLO     RA
        LDI     3
        STR     RA
        LDI     HIGH (GDX)
        PHI     RA
        LDI     LOW (GDX)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (GROW)
        PHI     RA
        LDI     LOW (GROW)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (GTMR)
        PHI     RA
        LDI     LOW (GTMR)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (MTMR)
        PHI     RA
        LDI     LOW (MTMR)
        PLO     RA
        LDI     250
        STR     RA
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDI     60
        STR     RA
        LDI     0
        PLO     R8
M1I1
        GLO     R8
        ADI     LOW GTYPE
        PLO     RC
        LDI     HIGH GTYPE
        ADCI    0
        PHI     RC
        LDN     RC
        PLO     R9
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        GLO     R8
        ADI     LOW GROWT
        PLO     RC
        LDI     HIGH GROWT
        ADCI    0
        PHI     RC
        LDN     RC
        PLO     R9
        GLO     R8
        ADI     LOW EA
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        SHL
        ADI     3
        PLO     R9
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        GLO     R8
        ADI     LOW GCOLT
        PLO     RC
        LDI     HIGH GCOLT
        ADCI    0
        PHI     RC
        LDN     RC
        PLO     R9
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        SHL
        SHL
        ADI     4
        PLO     R9
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        SEP     R4
        WORD    ESHOW
        INC     R8
        GLO     R8
        XRI     28
        LBNZ    M1I1
        LDI     HIGH (BGBUF+17*40+8)
        PHI     RA
        LDI     LOW (BGBUF+17*40+8)
        PLO     RA
        LDI     HIGH (OMAP+17*40+8)
        PHI     RC
        LDI     LOW (OMAP+17*40+8)
        PLO     RC
        LDI     24
        PLO     R9
M1I2    LDI     141
        STR     RA
        INC     RA
        LDI     0F0H
        STR     RC
        INC     RC
        DEC     R9
        GLO     R9
        LBNZ    M1I2
        SEP     R5
M1STEP
        LDI     HIGH (GX)
        PHI     RA
        LDI     LOW (GX)
        PLO     RA
        LDN     RA
        ANI     1
        PLO     R7
        LDI     HIGH (ANIM)
        PHI     RA
        LDI     LOW (ANIM)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (GTMR)
        PHI     RA
        LDI     LOW (GTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M1F
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDN     RA
        SDI     4
        LBNF    M1S0
        LBNZ    M1S1
M1S0    LDI     1
M1S1    PLO     R9
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDN     RA
        SMI     8
        LBDF    M1S2
        LDI     1
        PLO     R9
M1S2    GLO     R9
        PLO     R7
        LDI     HIGH (GTMR)
        PHI     RA
        LDI     LOW (GTMR)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (GROW)
        PHI     RA
        LDI     LOW (GROW)
        PLO     RA
        LDN     RA
        PLO     RB
        SHL
        SHL
        SHL
        STR     R2
        GLO     RB
        SD
        PLO     R8
        LDI     7
        PHI     RB
M1R1
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M1R2
        LDI     HIGH (GX)
        PHI     RA
        LDI     LOW (GX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (GDX)
        PHI     RA
        LDI     LOW (GDX)
        PLO     RA
        LDN     RA
        ADD
        STR     R2
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        SHL
        SHL
        ADD
        PLO     R9
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        LDI     HIGH (GY)
        PHI     RA
        LDI     LOW (GY)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R8
        ADI     LOW EA
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        SHL
        ADD
        PLO     R9
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        GHI     RB
        STXD
        SEP     R4
        WORD    ESHOW
        IRX
        LDX
        PHI     RB
M1R2    INC     R8
        GHI     RB
        SMI     1
        PHI     RB
        LBNZ    M1R1
        LDI     HIGH (GROW)
        PHI     RA
        LDI     LOW (GROW)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        XRI     4
        LBNZ    M1F
        LDI     HIGH (GROW)
        PHI     RA
        LDI     LOW (GROW)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (GDX)
        PHI     RA
        LDI     LOW (GDX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (GX)
        PHI     RA
        LDI     LOW (GX)
        PLO     RA
        LDN     RA
        ADD
        STR     RA
        PLO     R9
        LDI     HIGH (GDX)
        PHI     RA
        LDI     LOW (GDX)
        PLO     RA
        LDN     RA
        XRI     1
        LBNZ    M1E1
        GLO     R9
        SMI     14
        LBNF    M1F
        LBR     M1E2
M1E1    GLO     R9
        LBNZ    M1F
M1E2
        LDI     HIGH (GDX)
        PHI     RA
        LDI     LOW (GDX)
        PLO     RA
        LDN     RA
        SDI     0
        STR     RA
        LDI     HIGH (GY)
        PHI     RA
        LDI     LOW (GY)
        PLO     RA
        LDN     RA
        SMI     10
        LBDF    M1F
        LDI     HIGH (GY)
        PHI     RA
        LDI     LOW (GY)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
M1F
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M1M
        SEP     R4
        WORD    RAND
        ANI     31
        ADI     12
        STR     R2
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDN     RA
        SHL
        SHL
        SD
        LBDF    M1F1
        LDI     6
M1F1
        PLO     R7
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     28
        SEP     R4
        WORD    RANDN
        PLO     R8
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M1M
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     1
        PHI     RE
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RE
        SEP     R4
        WORD    BFIRE
M1M
        LDI     HIGH (ET+28)
        PHI     RA
        LDI     LOW (ET+28)
        PLO     RA
        LDN     RA
        LBNZ    M1M2
        LDI     HIGH (MTMR)
        PHI     RA
        LDI     LOW (MTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M1X
        LDI     HIGH (MTMR)
        PHI     RA
        LDI     LOW (MTMR)
        PLO     RA
        LDI     250
        STR     RA
        LDI     HIGH (ET+28)
        PHI     RA
        LDI     LOW (ET+28)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (EX+28)
        PHI     RA
        LDI     LOW (EX+28)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (EY+28)
        PHI     RA
        LDI     LOW (EY+28)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (1018H)
        PHI     RC
        LDI     LOW (1018H)
        PLO     RC
        LDI     30
        SEP     R4
        WORD    SOUND
M1M2
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        ANI     3
        LBNZ    M1X
        LDI     HIGH (EX+28)
        PHI     RA
        LDI     LOW (EX+28)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        LDI     28
        PLO     R8
        GLO     RA
        LDN     RA
        SMI     38
        LBNF    M1M3
        SEP     R4
        WORD    EERASE
        LDI     HIGH (ET+28)
        PHI     RA
        LDI     LOW (ET+28)
        PLO     RA
        LDI     0
        STR     RA
        LBR     M1X
M1M3
        SEP     R4
        WORD    ESHOW
M1X     SEP     R5
M2INIT
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDI     10
        STR     RA
        LDI     HIGH (BEAMT)
        PHI     RA
        LDI     LOW (BEAMT)
        PLO     RA
        LDI     100
        STR     RA
        LDI     HIGH (BEAMS)
        PHI     RA
        LDI     LOW (BEAMS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDI     40
        STR     RA
        LDI     HIGH (ET+0)
        PHI     RA
        LDI     LOW (ET+0)
        PLO     RA
        LDI     6
        STR     RA
        LDI     HIGH (EX+0)
        PHI     RA
        LDI     LOW (EX+0)
        PLO     RA
        LDI     8
        STR     RA
        LDI     HIGH (EY+0)
        PHI     RA
        LDI     LOW (EY+0)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (EA+0)
        PHI     RA
        LDI     LOW (EA+0)
        PLO     RA
        LDI     1
        STR     RA
        LDI     0
        PLO     R8
        SEP     R4
        WORD    ESHOW
        LDI     HIGH (ET+1)
        PHI     RA
        LDI     LOW (ET+1)
        PLO     RA
        LDI     6
        STR     RA
        LDI     HIGH (EX+1)
        PHI     RA
        LDI     LOW (EX+1)
        PLO     RA
        LDI     28
        STR     RA
        LDI     HIGH (EY+1)
        PHI     RA
        LDI     LOW (EY+1)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (EA+1)
        PHI     RA
        LDI     LOW (EA+1)
        PLO     RA
        LDI     255
        STR     RA
        LDI     1
        PLO     R8
        SEP     R4
        WORD    ESHOW
        LDI     HIGH (ET+2)
        PHI     RA
        LDI     LOW (ET+2)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+2)
        PHI     RA
        LDI     LOW (EA+2)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (EB+2)
        PHI     RA
        LDI     LOW (EB+2)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (ET+3)
        PHI     RA
        LDI     LOW (ET+3)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+3)
        PHI     RA
        LDI     LOW (EA+3)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (EB+3)
        PHI     RA
        LDI     LOW (EB+3)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (ET+4)
        PHI     RA
        LDI     LOW (ET+4)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+4)
        PHI     RA
        LDI     LOW (EA+4)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (EB+4)
        PHI     RA
        LDI     LOW (EB+4)
        PLO     RA
        LDI     2
        STR     RA
        LDI     HIGH (ET+5)
        PHI     RA
        LDI     LOW (ET+5)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+5)
        PHI     RA
        LDI     LOW (EA+5)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (EB+5)
        PHI     RA
        LDI     LOW (EB+5)
        PLO     RA
        LDI     3
        STR     RA
        LDI     HIGH (ET+6)
        PHI     RA
        LDI     LOW (ET+6)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+6)
        PHI     RA
        LDI     LOW (EA+6)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (EB+6)
        PHI     RA
        LDI     LOW (EB+6)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (ET+7)
        PHI     RA
        LDI     LOW (ET+7)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+7)
        PHI     RA
        LDI     LOW (EA+7)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (EB+7)
        PHI     RA
        LDI     LOW (EB+7)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (ET+8)
        PHI     RA
        LDI     LOW (ET+8)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+8)
        PHI     RA
        LDI     LOW (EA+8)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (EB+8)
        PHI     RA
        LDI     LOW (EB+8)
        PLO     RA
        LDI     6
        STR     RA
        LDI     HIGH (ET+9)
        PHI     RA
        LDI     LOW (ET+9)
        PLO     RA
        LDI     5
        STR     RA
        LDI     HIGH (EA+9)
        PHI     RA
        LDI     LOW (EA+9)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (EB+9)
        PHI     RA
        LDI     LOW (EB+9)
        PLO     RA
        LDI     7
        STR     RA
        SEP     R5
M2STEP
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        SHR
        SHR
        SHR
        ANI     1
        PLO     R7
        LDI     HIGH (ANIM)
        PHI     RA
        LDI     LOW (ANIM)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        ANI     7
        LBNZ    M2E
        LDI     0
        PLO     R8
M2L1
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M2L3
        GLO     R8
        ADI     LOW EA
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        STR     R2
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADD
        STR     RA
        SMI     4
        LBNF    M2L2
        SMI     31
        LBNF    M2L2A
M2L2
        GLO     R8
        ADI     LOW EA
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        SDI     0
        STR     RA
M2L2A
        SEP     R4
        WORD    ESHOW
M2L3    INC     R8
        GLO     R8
        XRI     2
        LBNZ    M2L1
M2E
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        ANI     3
        LBNZ    M2B
        LDI     2
        PLO     R8
M2E1
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M2E9
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ANI     80H
        LBNZ    M2D
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     1
        ANI     7
        STR     RA
        PLO     R9
        GLO     R8
        ADI     LOW EA
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RB
        GLO     RB
        ADI     LOW ET
        PLO     RA
        LDN     RA
        LBZ     M2DS
        SEP     R4
        WORD    RAND
        ANI     3FH
        LBZ     M2DS
        GLO     RB
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        STR     R2
        GLO     R9
        ADI     LOW OFSX
        PLO     RC
        LDI     HIGH OFSX
        ADCI    0
        PHI     RC
        LDN     RC
        ADD
        PLO     R7
        SMI     40
        LBNF    M2CX
        LDI     39
        PLO     R7
M2CX
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R7
        STR     RA
        GLO     RB
        ADI     LOW EY
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R9
        ADI     LOW OFSY
        PLO     RC
        LDI     HIGH OFSY
        ADCI    0
        PHI     RC
        LDN     RC
        ADD
        PLO     R7
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R7
        STR     RA
        LBR     M2E8
M2DS
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ORI     80H
        STR     RA
M2D
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     1
        STR     RA
        SMI     21
        LBNF    M2D2
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     2
        STR     RA
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ANI     7
        STR     RA
        LBR     M2E8
M2D2
        SEP     R4
        WORD    STEER
        SEP     R4
        WORD    HITP
M2E8
        SEP     R4
        WORD    ESHOW
M2E9    INC     R8
        GLO     R8
        XRI     10
        LBNZ    M2E1
M2B
        LDI     HIGH (BEAMS)
        PHI     RA
        LDI     LOW (BEAMS)
        PLO     RA
        LDN     RA
        LBNZ    M2B5
        LDI     HIGH (BEAMT)
        PHI     RA
        LDI     LOW (BEAMT)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M2F
        SEP     R4
        WORD    RAND
        ANI     1
        PLO     R8
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBNZ    M2B1
        GLO     R8
        XRI     1
        PLO     R8
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M2F
M2B1
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     1
        PLO     R7
        LDI     HIGH (BEAMX)
        PHI     RA
        LDI     LOW (BEAMX)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (BEAMS)
        PHI     RA
        LDI     LOW (BEAMS)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (BEAMT)
        PHI     RA
        LDI     LOW (BEAMT)
        PLO     RA
        LDI     40
        STR     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (0818H)
        PHI     RC
        LDI     LOW (0818H)
        PLO     RC
        LDI     40
        SEP     R4
        WORD    SOUND
        LDI     HIGH (BEAMX)
        PHI     RA
        LDI     LOW (BEAMX)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     5
        PHI     RE
M2B2    LDI     147
        SEP     R1
        GHI     RE
        ADI     1
        PHI     RE
        XRI     21
        LBNZ    M2B2
        LBR     M2F
M2B5
        LDI     HIGH (BEAMX)
        PHI     RA
        LDI     LOW (BEAMX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        SM
        ADI     1
        SMI     3
        LBDF    M2B6
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDN     RA
        LBNZ    M2B6
        SEP     R4
        WORD    KILLP
M2B6
        LDI     HIGH (BEAMT)
        PHI     RA
        LDI     LOW (BEAMT)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M2F
        LDI     HIGH (BEAMS)
        PHI     RA
        LDI     LOW (BEAMS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDN     RA
        SHL
        SHL
        SHL
        SHL
        SDI     150
        PLO     R7
        LDI     HIGH (BEAMT)
        PHI     RA
        LDI     LOW (BEAMT)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (BEAMX)
        PHI     RA
        LDI     LOW (BEAMX)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     5
        PHI     RE
M2B7    LDI     0
        SEP     R1
        GHI     RE
        ADI     1
        PHI     RE
        XRI     21
        LBNZ    M2B7
M2F
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M2X
        SEP     R4
        WORD    RAND
        ANI     31
        ADI     15
        PLO     R7
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     8
        SEP     R4
        WORD    RANDN
        ADI     2
        PLO     R8
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M2X
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     1
        PHI     RE
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RE
        SEP     R4
        WORD    BFIRE
M2X     SEP     R5
STEER
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        SM
        LBZ     STX
        LBNF    ST1
        LDN     RA
        SMI     1
        STR     RA
        SEP     R5
ST1     LDN     RA
        ADI     1
        STR     RA
STX     SEP     R5
HITP
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        XRI     21
        LBNZ    HPX
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        SM
        ADI     1
        SMI     3
        LBDF    HPX
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDN     RA
        LBNZ    HPX
        SEP     R4
        WORD    KILLP
HPX     SEP     R5
M3INIT
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDI     12
        STR     RA
        LDI     HIGH (WVN)
        PHI     RA
        LDI     LOW (WVN)
        PLO     RA
        LDI     12
        STR     RA
        LDI     HIGH (VANG)
        PHI     RA
        LDI     LOW (VANG)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDI     50
        STR     RA
        LDI     HIGH (TMPA)
        PHI     RA
        LDI     LOW (TMPA)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (BGBUF+10*40+19)
        PHI     RA
        LDI     LOW (BGBUF+10*40+19)
        PLO     RA
        LDI     150
        STR     RA
        SEP     R5
M3STEP
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        SHR
        SHR
        ANI     1
        PLO     R7
        LDI     HIGH (ANIM)
        PHI     RA
        LDI     LOW (ANIM)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (TMPA)
        PHI     RA
        LDI     LOW (TMPA)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M3M
        LDI     HIGH (TMPA)
        PHI     RA
        LDI     LOW (TMPA)
        PLO     RA
        LDI     30
        STR     RA
        LDI     HIGH (WVN)
        PHI     RA
        LDI     LOW (WVN)
        PLO     RA
        LDN     RA
        LBZ     M3M
        LDI     0
        PLO     R8
M3S1
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M3S2
        INC     R8
        GLO     R8
        XRI     6
        LBNZ    M3S1
        LBR     M3M
M3S2    LDI     7
        STR     RA
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     0
        STR     RA
        LDI     HIGH (VANG)
        PHI     RA
        LDI     LOW (VANG)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        ANI     7
        PLO     R9
        GLO     R8
        ADI     LOW EA
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R9
        STR     RA
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     19
        STR     RA
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     10
        STR     RA
        LDI     HIGH (WVN)
        PHI     RA
        LDI     LOW (WVN)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     126
        STR     RA
        LDI     HIGH (1C2FH)
        PHI     RC
        LDI     LOW (1C2FH)
        PLO     RC
        LDI     20
        SEP     R4
        WORD    SOUND
M3M
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        ANI     3
        LBNZ    M3F
        LDI     0
        PLO     R8
M3M1
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M3M9
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        SMI     24
        LBDF    M3D
        GLO     R8
        ADI     LOW EA
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     R9
        GLO     R9
        SHL
        SHL
        SHL
        STR     R2
        GLO     R9
        SHL
        SHL
        SHL
        SHL
        ADD
        STR     R2
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RB
        ADD
        PLO     R9
        GLO     RB
        ADI     1
        STR     RA
        GLO     R9
        ADI     LOW PATHX
        PLO     RC
        LDI     HIGH PATHX
        ADCI    0
        PHI     RC
        LDN     RC
        PLO     R7
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R7
        STR     RA
        GLO     R9
        ADI     LOW PATHY
        PLO     RC
        LDI     HIGH PATHY
        ADCI    0
        PHI     RC
        LDN     RC
        PLO     R7
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        GLO     R7
        STR     RA
        LBR     M3M8
M3D
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     1
        STR     RA
        SMI     21
        LBNF    M3D2
        GLO     R8
        ADI     LOW EB
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     0
        STR     RA
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     19
        STR     RA
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDI     10
        STR     RA
        LBR     M3M8
M3D2
        SEP     R4
        WORD    STEER
        SEP     R4
        WORD    HITP
M3M8
        SEP     R4
        WORD    ESHOW
M3M9    INC     R8
        GLO     R8
        XRI     6
        LBNZ    M3M1
M3F
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M3X
        SEP     R4
        WORD    RAND
        ANI     31
        ADI     12
        PLO     R7
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     6
        SEP     R4
        WORD    RANDN
        PLO     R8
        GLO     R8
        ADI     LOW ET
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        LBZ     M3X
        GLO     R8
        ADI     LOW EY
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        ADI     1
        PHI     RE
        GLO     R8
        ADI     LOW EX
        PLO     RA
        LDI     0A8H
        PHI     RA
        LDN     RA
        PLO     RE
        SEP     R4
        WORD    BFIRE
M3X     SEP     R5
M4INIT
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDI     16
        STR     RA
        LDI     HIGH (CMDX)
        PHI     RA
        LDI     LOW (CMDX)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (CMTMR)
        PHI     RA
        LDI     LOW (CMTMR)
        PLO     RA
        LDI     8
        STR     RA
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDI     40
        STR     RA
        LDI     HIGH (CMSH)
        PHI     RA
        LDI     LOW (CMSH)
        PLO     RA
        LDI     2
        STR     RA
        INC     RA
        STR     RA
        INC     RA
        STR     RA
        INC     RA
        STR     RA
        INC     RA
        STR     RA
        INC     RA
        STR     RA
        INC     RA
        STR     RA
        INC     RA
        SEP     R4
        WORD    CMDDRAW
        SEP     R5
CMDDRAW LDI     0
        PLO     RB
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     0
        PLO     RE
        LDI     2
        PHI     RE
        LDI     151
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     1
        PLO     RE
        LDI     2
        PHI     RE
        LDI     152
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     RE
        LDI     2
        PHI     RE
        LDI     153
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     3
        PLO     RE
        LDI     2
        PHI     RE
        LDI     154
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     4
        PLO     RE
        LDI     2
        PHI     RE
        LDI     155
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     5
        PLO     RE
        LDI     2
        PHI     RE
        LDI     156
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     6
        PLO     RE
        LDI     2
        PHI     RE
        LDI     157
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     0
        PLO     RE
        LDI     3
        PHI     RE
        LDI     158
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     1
        PLO     RE
        LDI     3
        PHI     RE
        LDI     159
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     RE
        LDI     3
        PHI     RE
        LDI     219
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     3
        PLO     RE
        LDI     3
        PHI     RE
        LDI     227
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F2H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     4
        PLO     RE
        LDI     3
        PHI     RE
        LDI     221
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     5
        PLO     RE
        LDI     3
        PHI     RE
        LDI     222
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     6
        PLO     RE
        LDI     3
        PHI     RE
        LDI     223
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     0
        PLO     RE
        LDI     4
        PHI     RE
        LDI     224
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     1
        PLO     RE
        LDI     4
        PHI     RE
        LDI     225
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     RE
        LDI     4
        PHI     RE
        LDI     226
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     3
        PLO     RE
        LDI     4
        PHI     RE
        LDI     225
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     4
        PLO     RE
        LDI     4
        PHI     RE
        LDI     226
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     5
        PLO     RE
        LDI     4
        PHI     RE
        LDI     225
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0F3H
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     6
        PLO     RE
        LDI     4
        PHI     RE
        LDI     0
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0
        STR     RC
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     0
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    CSHDRAW
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     1
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    CSHDRAW
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    CSHDRAW
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     3
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    CSHDRAW
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     4
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    CSHDRAW
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     5
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    CSHDRAW
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     6
        PLO     RE
        LDI     5
        PHI     RE
        SEP     R4
        WORD    CSHDRAW
        SEP     R5
CSHDRAW
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        STR     R2
        GLO     RE
        SM
        PLO     R9
        ADI     LOW CMSH
        PLO     RA
        LDI     HIGH CMSH
        PHI     RA
        LDN     RA
        LBZ     CSD0
        XRI     2
        LBZ     CSD2
        LDI     142
        LBR     CSD3
CSD2    LDI     228
CSD3    SEP     R1
        SEP     R4
        WORD    OMAPA
        GLO     R9
        ADI     0E0H
        STR     RC
        SEP     R5
CSD0    LDI     0
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0
        STR     RC
        SEP     R5
M4STEP
        LDI     HIGH (CMTMR)
        PHI     RA
        LDI     LOW (CMTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M4F
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDN     RA
        SDI     8
        LBNF    M4T0
        SMI     3
        LBDF    M4T1
M4T0    LDI     0
M4T1    ADI     3
        PLO     R7
        LDI     HIGH (CMTMR)
        PHI     RA
        LDI     LOW (CMTMR)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     HIGH (CMDX)
        PHI     RA
        LDI     LOW (CMDX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADD
        LBZ     M4R
        SMI     33
        LBNF    M4V
M4R
        LDI     HIGH (CMDX)
        PHI     RA
        LDI     LOW (CMDX)
        PLO     RA
        LDN     RA
        SDI     0
        STR     RA
        LBR     M4F
M4V
        LDI     HIGH (CMDX)
        PHI     RA
        LDI     LOW (CMDX)
        PLO     RA
        LDN     RA
        XRI     1
        LBNZ    M4V1
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        LBR     M4V2
M4V1
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     6
M4V2    PLO     RE
        LDI     2
        PHI     RE
M4V3    LDI     0
        SEP     R1
        SEP     R4
        WORD    OMAPA
        LDI     0
        STR     RC
        GHI     RE
        ADI     1
        PHI     RE
        XRI     6
        LBNZ    M4V3
        LDI     HIGH (CMDX)
        PHI     RA
        LDI     LOW (CMDX)
        PLO     RA
        LDN     RA
        STR     R2
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADD
        STR     RA
        SEP     R4
        WORD    CMDDRAW
M4F
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    M4X
        SEP     R4
        WORD    RAND
        ANI     15
        ADI     14
        PLO     R7
        LDI     HIGH (FTMR)
        PHI     RA
        LDI     LOW (FTMR)
        PLO     RA
        GLO     R7
        STR     RA
        LDI     7
        SEP     R4
        WORD    RANDN
        STR     R2
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADD
        PLO     RE
        LDI     6
        PHI     RE
        SEP     R4
        WORD    BFIRE
M4X     SEP     R5
CMDHIT  GLO     R8
        XRI     0F2H
        LBZ     CH5
        GLO     R8
        SMI     0F0H
        LBDF    CHX
        GLO     R8
        SMI     0E0H
        ADI     LOW CMSH
        PLO     RA
        LDI     HIGH CMSH
        PHI     RA
        LDN     RA
        LBZ     CHX
        SMI     1
        STR     RA
        SEP     R4
        WORD    CSHDRAW
        LDI     1
        SEP     R4
        WORD    ADDSC
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (203FH)
        PHI     RC
        LDI     LOW (203FH)
        PLO     RC
        LDI     4
        SEP     R4
        WORD    SOUND
CHX     SEP     R5
CH5
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDI     0
        STR     RA
        LDI     100
        SEP     R4
        WORD    ADDSC
        LDI     HIGH (0F80H)
        PHI     RC
        LDI     LOW (0F80H)
        PLO     RC
        LDI     90
        SEP     R4
        WORD    NOISE
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (301FH)
        PHI     RC
        LDI     LOW (301FH)
        PLO     RC
        LDI     80
        SEP     R4
        WORD    SOUND
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     3
        PLO     RE
        LDI     3
        PHI     RE
        SEP     R4
        WORD    EFFECT
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     1
        PLO     RE
        LDI     3
        PHI     RE
        SEP     R4
        WORD    EFFECT
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     5
        PLO     RE
        LDI     3
        PHI     RE
        SEP     R4
        WORD    EFFECT
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     2
        PLO     RE
        LDI     4
        PHI     RE
        SEP     R4
        WORD    EFFECT
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     4
        PLO     RE
        LDI     4
        PHI     RE
        SEP     R4
        WORD    EFFECT
        LDI     HIGH (CMX)
        PHI     RA
        LDI     LOW (CMX)
        PLO     RA
        LDN     RA
        ADI     3
        PLO     RE
        LDI     2
        PHI     RE
        SEP     R4
        WORD    EFFECT
        SEP     R5
MINITX
        LDI     HIGH (MISSION)
        PHI     RA
        LDI     LOW (MISSION)
        PLO     RA
        LDN     RA
        LBZ     M1INIT
        SMI     1
        LBZ     M2INIT
        SMI     1
        LBZ     M3INIT
        LBR     M4INIT
MSTEPX
        LDI     HIGH (MISSION)
        PHI     RA
        LDI     LOW (MISSION)
        PLO     RA
        LDN     RA
        LBZ     M1STEP
        SMI     1
        LBZ     M2STEP
        SMI     1
        LBZ     M3STEP
        LBR     M4STEP
QFLUSH
        LDI     HIGH (QN)
        PHI     RA
        LDI     LOW (QN)
        PLO     RA
        LDN     RA
        LBZ     QFX
        PLO     R8
        LDI     HIGH (QBUF)
        PHI     RA
        LDI     LOW (QBUF)
        PLO     RA
QF1     LDA     RA
        PHI     RD
        LDA     RA
        PLO     RD
        LDA     RA
        STR     RD
        DEC     R8
        GLO     R8
        LBNZ    QF1
QFX
        LDI     HIGH (QN)
        PHI     RA
        LDI     LOW (QN)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QR)
        PHI     RA
        LDI     LOW (QR)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QRO)
        PHI     RA
        LDI     LOW (QRO)
        PLO     RA
        LDI     0
        STR     RA
        SEP     R5
COPYMEM LDA     RC
        STR     RD
        INC     RD
        DEC     RE
        GLO     RE
        LBNZ    COPYMEM
        GHI     RE
        LBNZ    COPYMEM
        SEP     R5
FILL    PLO     R7
FL1     GLO     R7
        STR     RD
        INC     RD
        DEC     RE
        GLO     RE
        LBNZ    FL1
        GHI     RE
        LBNZ    FL1
        SEP     R5
WAITN   PLO     R9
WN1
        SEP     R4
        WORD    WAITVB
        SEP     R4
        WORD    SNDTICK
        DEC     R9
        GLO     R9
        LBNZ    WN1
        SEP     R5
QFLUSHW LDI     4
        PLO     R9
QW1
        SEP     R4
        WORD    WAITVB
        SEP     R4
        WORD    VBDRAW
        DEC     R9
        GLO     R9
        LBNZ    QW1
        SEP     R5
MUSIC
        LDI     HIGH (MUST)
        PHI     RA
        LDI     LOW (MUST)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        LBNZ    MUX
        LDI     HIGH (MUSP)
        PHI     RA
        LDI     LOW (MUSP)
        PLO     RA
        LDA     RA
        PHI     RB
        LDN     RA
        PLO     RB
        LDA     RB
        LBNZ    MU1
        LDI     HIGH (TUNE)
        PHI     RB
        LDI     LOW (TUNE)
        PLO     RB
        LDA     RB
MU1     PHI     RC
        LDI     28H
        PLO     RC
        LDA     RB
        PLO     R7
        LDI     HIGH (MUST)
        PHI     RA
        LDI     LOW (MUST)
        PLO     RA
        GLO     R7
        STR     RA
        PLO     R9
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        GHI     RC
        XRI     7FH
        LBZ     MU2
        GLO     R9
        SMI     1
        SEP     R4
        WORD    SOUND
MU2
        LDI     HIGH (MUSP)
        PHI     RA
        LDI     LOW (MUSP)
        PLO     RA
        GHI     RB
        STR     RA
        INC     RA
        GLO     RB
        STR     RA
MUX     SEP     R5
EXIT
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    SILENCE
        SEP     R4
        WORD    CHREST
        SEP     R4
        WORD    CLRSCR
        LDI     HIGH (8088H)
        PHI     RF
        LDI     LOW (8088H)
        PLO     RF
        SEX     RF
        OUT     5
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
        SEX     R3
        OUT     3
        BYTE    80H
        SEX     R2
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
        LDXA
        PLO     R4
        LDXA
        PHI     R1
        LDX
        PLO     R1
        SEP     R5
SBUF    EQU     09600H
MSGQ    LDA     RC
        LBZ     MQX
        ORI     80H
        PLO     RF
        GLO     RC
        STXD
        GHI     RC
        STXD
        GLO     RF
        SEP     R1
        IRX
        LDXA
        PHI     RC
        LDX
        PLO     RC
        INC     RE
        LBR     MSGQ
MQX     SEP     R5
INIT
        SEP     R4
        WORD    VIDINIT
        SEP     R4
        WORD    CHSAVE
        SEP     R4
        WORD    CHLOAD
        LDI     HIGH (RND)
        PHI     RA
        LDI     LOW (RND)
        PLO     RA
        LDI     5AH
        STR     RA
        INC     RA
        LDI     0A5H
        STR     RA
        LDI     HIGH (HISC)
        PHI     RD
        LDI     LOW (HISC)
        PLO     RD
        LDI     HIGH (6)
        PHI     RE
        LDI     LOW (6)
        PLO     RE
        LDI     0
        SEP     R4
        WORD    FILL
        LDI     HIGH (HISC+1)
        PHI     RD
        LDI     LOW (HISC+1)
        PLO     RD
        LDI     1
        STR     RD
        LDI     HIGH (SBUF)
        PHI     RD
        LDI     LOW (SBUF)
        PLO     RD
        LDI     HIGH (960)
        PHI     RE
        LDI     LOW (960)
        PLO     RE
        LDI     20H
        SEP     R4
        WORD    FILL
        LDI     HIGH (SBUF+40)
        PHI     RD
        LDI     LOW (SBUF+40)
        PLO     RD
        LDI     HIGH (840)
        PHI     RE
        LDI     LOW (840)
        PLO     RE
IS1
        SEP     R4
        WORD    RAND
        SMI     10
        LBDF    IS4
        SEP     R4
        WORD    RAND
        ANI     1
        LBZ     IS3
        LDI     233
        STR     RD
        LBR     IS4
IS3     LDI     106
        STR     RD
IS4     INC     RD
        DEC     RE
        GLO     RE
        LBNZ    IS1
        GHI     RE
        LBNZ    IS1
        LDI     HIGH (AUTO)
        PHI     RA
        LDI     LOW (AUTO)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (MUTE)
        PHI     RA
        LDI     LOW (MUTE)
        PLO     RA
        LDI     0
        STR     RA
TITLE
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    SILENCE
        SEP     R4
        WORD    CLRSCR
        LDI     HIGH (SBUF+40)
        PHI     RC
        LDI     LOW (SBUF+40)
        PLO     RC
        LDI     HIGH (PAGE+40)
        PHI     RD
        LDI     LOW (PAGE+40)
        PLO     RD
        LDI     HIGH (800)
        PHI     RE
        LDI     LOW (800)
        PLO     RE
        SEP     R4
        WORD    COPYMEM
        LDI     HIGH (783)
        PHI     RE
        LDI     LOW (783)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX1)
        PHI     RC
        LDI     LOW (TX1)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (1290)
        PHI     RE
        LDI     LOW (1290)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX2)
        PHI     RC
        LDI     LOW (TX2)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (2311)
        PHI     RE
        LDI     LOW (2311)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX3)
        PHI     RC
        LDI     LOW (TX3)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (2823)
        PHI     RE
        LDI     LOW (2823)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX4)
        PHI     RC
        LDI     LOW (TX4)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (3079)
        PHI     RE
        LDI     LOW (3079)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX5)
        PHI     RC
        LDI     LOW (TX5)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (3844)
        PHI     RE
        LDI     LOW (3844)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX6)
        PHI     RC
        LDI     LOW (TX6)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (4100)
        PHI     RE
        LDI     LOW (4100)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX7)
        PHI     RC
        LDI     LOW (TX7)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (4356)
        PHI     RE
        LDI     LOW (4356)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX8)
        PHI     RC
        LDI     LOW (TX8)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (4878)
        PHI     RE
        LDI     LOW (4878)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX9)
        PHI     RC
        LDI     LOW (TX9)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (4881)
        PHI     RE
        LDI     LOW (4881)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (HISC)
        PHI     RB
        LDI     LOW (HISC)
        PLO     RB
        LDI     6
        PLO     R9
TI1     LDA     RB
        ADI     0B0H
        STR     RD
        INC     RD
        DEC     R9
        GLO     R9
        LBNZ    TI1
        LDI     HIGH (PAGE+858)
        PHI     RD
        LDI     LOW (PAGE+858)
        PLO     RD
        LDI     129
        STR     RD
        LDI     HIGH (PAGE+859)
        PHI     RD
        LDI     LOW (PAGE+859)
        PLO     RD
        LDI     130
        STR     RD
        LDI     HIGH (PAGE+860)
        PHI     RD
        LDI     LOW (PAGE+860)
        PLO     RD
        LDI     131
        STR     RD
        LDI     HIGH (PAGE+22*40)
        PHI     RD
        LDI     LOW (PAGE+22*40)
        PLO     RD
        LDI     HIGH (40)
        PHI     RE
        LDI     LOW (40)
        PLO     RE
        LDI     236
        SEP     R4
        WORD    FILL
        SEP     R4
        WORD    VIDON
        LDI     HIGH (TUNE)
        PHI     RC
        LDI     LOW (TUNE)
        PLO     RC
        LDI     HIGH (MUSP)
        PHI     RA
        LDI     LOW (MUSP)
        PLO     RA
        GHI     RC
        STR     RA
        INC     RA
        GLO     RC
        STR     RA
        LDI     HIGH (MUST)
        PHI     RA
        LDI     LOW (MUST)
        PLO     RA
        LDI     1
        STR     RA
TW1
        SEP     R4
        WORD    WAITVB
        SEP     R4
        WORD    SNDTICK
        LDI     HIGH (MUTE)
        PHI     RA
        LDI     LOW (MUTE)
        PLO     RA
        LDN     RA
        LBNZ    TW2
        SEP     R4
        WORD    MUSIC
TW2
        SEP     R4
        WORD    GETKEY
        LBZ     TW1
        PLO     R9
        XRI     'M'
        LBNZ    TW3
        LDI     HIGH (MUTE)
        PHI     RA
        LDI     LOW (MUTE)
        PLO     RA
        LDN     RA
        XRI     1
        STR     RA
        SEP     R4
        WORD    SILENCE
        LDI     HIGH (MUST)
        PHI     RA
        LDI     LOW (MUST)
        PLO     RA
        LDI     1
        STR     RA
        LBR     TW1
TW3     GLO     R9
        XRI     K_ESC
        LBZ     EXIT
        GLO     R9
        XRI     5FH
        LBZ     NEWGAME
        GLO     R9
        XRI     20H
        LBZ     NEWGAME
        GLO     R9
        XRI     '1'
        LBZ     NEWGAME
        LBR     TW1
NEWGAME
        LDI     HIGH (SCORE)
        PHI     RD
        LDI     LOW (SCORE)
        PLO     RD
        LDI     HIGH (6)
        PHI     RE
        LDI     LOW (6)
        PLO     RE
        LDI     0
        SEP     R4
        WORD    FILL
        LDI     HIGH (LIVES)
        PHI     RA
        LDI     LOW (LIVES)
        PLO     RA
        LDI     4
        STR     RA
        LDI     HIGH (MISSION)
        PHI     RA
        LDI     LOW (MISSION)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDI     0
        STR     RA
NEXTM
        SEP     R4
        WORD    MSETUP
MAIN
MW1     BN1     MW1
MW2     B1      MW2
        SEP     R4
        WORD    VBDRAW
        LDI     HIGH (SNDT)
        PHI     RA
        LDI     LOW (SNDT)
        PLO     RA
        LDN     RA
        LBNZ    MXS1
        LDI     HIGH (NSDT)
        PHI     RA
        LDI     LOW (NSDT)
        PLO     RA
        LDN     RA
        LBZ     MXS2
MXS1
        SEP     R4
        WORD    SNDTICK
MXS2
        LDI     HIGH (FRM+1)
        PHI     RA
        LDI     LOW (FRM+1)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        DEC     RA
        LDN     RA
        ADCI    0
        STR     RA
        LDI     HIGH (WIN)
        PHI     RA
        LDI     LOW (WIN)
        PLO     RA
        LDN     RA
        LBNZ    MWIN
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDN     RA
        LBZ     MXP
        SEP     R4
        WORD    DEATH
        LBR     MXQ
MXP
        SEP     R4
        WORD    PLAYER
        SEP     R4
        WORD    SHOT
MXQ
        SEP     R4
        WORD    MSTEPX
        SEP     R4
        WORD    BULLETS
        SEP     R4
        WORD    EFFECTS
        LDI     HIGH (HUDF)
        PHI     RA
        LDI     LOW (HUDF)
        PLO     RA
        LDN     RA
        LBZ     MXH
        SEP     R4
        WORD    HUDSC
MXH
        LDI     HIGH (GOVER)
        PHI     RA
        LDI     LOW (GOVER)
        PLO     RA
        LDN     RA
        LBZ     MXG
        XRI     1
        LBZ     GAMEOVER
        LBR     GAMEOVER
MXG
        LDI     HIGH (NALIVE)
        PHI     RA
        LDI     LOW (NALIVE)
        PLO     RA
        LDN     RA
        LBNZ    MAIN
        LDI     HIGH (WIN)
        PHI     RA
        LDI     LOW (WIN)
        PLO     RA
        LDI     1
        STR     RA
        LDI     HIGH (WTMR)
        PHI     RA
        LDI     LOW (WTMR)
        PLO     RA
        LDI     150
        STR     RA
        SEP     R4
        WORD    CLRBUL
        LDI     HIGH (BEAMS)
        PHI     RA
        LDI     LOW (BEAMS)
        PLO     RA
        LDN     RA
        LBZ     MW0
        LDI     HIGH (BEAMS)
        PHI     RA
        LDI     LOW (BEAMS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (BEAMX)
        PHI     RA
        LDI     LOW (BEAMX)
        PLO     RA
        LDN     RA
        PLO     RE
        LDI     5
        PHI     RE
MWB     LDI     0
        SEP     R1
        GHI     RE
        ADI     1
        PHI     RE
        XRI     21
        LBNZ    MWB
MW0
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     127
        STR     RA
        LDI     HIGH (1A1FH)
        PHI     RC
        LDI     LOW (1A1FH)
        PLO     RC
        LDI     30
        SEP     R4
        WORD    SOUND
        LBR     MAIN
MWIN
        SEP     R4
        WORD    EFFECTS
        LDI     HIGH (WTMR)
        PHI     RA
        LDI     LOW (WTMR)
        PLO     RA
        LDN     RA
        SMI     1
        STR     RA
        XRI     130
        LBNZ    MWI1
        LDI     HIGH (3084)
        PHI     RE
        LDI     LOW (3084)
        PLO     RE
        LDI     HIGH (TX10)
        PHI     RC
        LDI     LOW (TX10)
        PLO     RC
        SEP     R4
        WORD    MSGQ
        LBR     MAIN
MWI1
        LDI     HIGH (WTMR)
        PHI     RA
        LDI     LOW (WTMR)
        PLO     RA
        LDN     RA
        LBNZ    MAIN
        LDI     HIGH (WIN)
        PHI     RA
        LDI     LOW (WIN)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (MISSION)
        PHI     RA
        LDI     LOW (MISSION)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        XRI     4
        LBNZ    NEXTM
        LDI     HIGH (MISSION)
        PHI     RA
        LDI     LOW (MISSION)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (LIVES)
        PHI     RA
        LDI     LOW (LIVES)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDN     RA
        XRI     4
        LBZ     NEXTM
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDN     RA
        ADI     1
        STR     RA
        LBR     NEXTM
MSETUP
        SEP     R4
        WORD    VIDOFF
        SEP     R4
        WORD    SILENCE
        LDI     HIGH (OMAP)
        PHI     RD
        LDI     LOW (OMAP)
        PLO     RD
        LDI     HIGH (960)
        PHI     RE
        LDI     LOW (960)
        PLO     RE
        LDI     0
        SEP     R4
        WORD    FILL
        LDI     HIGH (SBUF)
        PHI     RC
        LDI     LOW (SBUF)
        PLO     RC
        LDI     HIGH (BGBUF)
        PHI     RD
        LDI     LOW (BGBUF)
        PLO     RD
        LDI     HIGH (960)
        PHI     RE
        LDI     LOW (960)
        PLO     RE
        SEP     R4
        WORD    COPYMEM
        LDI     HIGH (EX)
        PHI     RD
        LDI     LOW (EX)
        PLO     RD
        LDI     HIGH (256)
        PHI     RE
        LDI     LOW (256)
        PLO     RE
        LDI     0
        SEP     R4
        WORD    FILL
        LDI     HIGH (BON)
        PHI     RD
        LDI     LOW (BON)
        PLO     RD
        LDI     HIGH (6)
        PHI     RE
        LDI     LOW (6)
        PLO     RE
        LDI     0
        SEP     R4
        WORD    FILL
        LDI     HIGH (EXT)
        PHI     RD
        LDI     LOW (EXT)
        PLO     RD
        LDI     HIGH (4)
        PHI     RE
        LDI     LOW (4)
        PLO     RE
        LDI     0
        SEP     R4
        WORD    FILL
        LDI     HIGH (SON)
        PHI     RA
        LDI     LOW (SON)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (PON)
        PHI     RA
        LDI     LOW (PON)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (PX)
        PHI     RA
        LDI     LOW (PX)
        PLO     RA
        LDI     20
        STR     RA
        LDI     HIGH (DEAD)
        PHI     RA
        LDI     LOW (DEAD)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (WIN)
        PHI     RA
        LDI     LOW (WIN)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QN)
        PHI     RA
        LDI     LOW (QN)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QR)
        PHI     RA
        LDI     LOW (QR)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (QRO)
        PHI     RA
        LDI     LOW (QRO)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (GOVER)
        PHI     RA
        LDI     LOW (GOVER)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (HELD)
        PHI     RA
        LDI     LOW (HELD)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (BEAMS)
        PHI     RA
        LDI     LOW (BEAMS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (ANIM)
        PHI     RA
        LDI     LOW (ANIM)
        PLO     RA
        LDI     0
        STR     RA
        SEP     R4
        WORD    MINITX
        LDI     HIGH (BGBUF)
        PHI     RC
        LDI     LOW (BGBUF)
        PLO     RC
        LDI     HIGH (PAGE)
        PHI     RD
        LDI     LOW (PAGE)
        PLO     RD
        LDI     HIGH (960)
        PHI     RE
        LDI     LOW (960)
        PLO     RE
        SEP     R4
        WORD    COPYMEM
        LDI     HIGH (0)
        PHI     RE
        LDI     LOW (0)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX11)
        PHI     RC
        LDI     LOW (TX11)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (14)
        PHI     RE
        LDI     LOW (14)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX12)
        PHI     RC
        LDI     LOW (TX12)
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (17)
        PHI     RE
        LDI     LOW (17)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (HISC)
        PHI     RB
        LDI     LOW (HISC)
        PLO     RB
        LDI     6
        PLO     R9
MS1     LDA     RB
        ADI     30H
        STR     RD
        INC     RD
        DEC     R9
        GLO     R9
        LBNZ    MS1
        LDI     HIGH (PAGE+22*40)
        PHI     RD
        LDI     LOW (PAGE+22*40)
        PLO     RD
        LDI     HIGH (40)
        PHI     RE
        LDI     LOW (40)
        PLO     RE
        LDI     236
        SEP     R4
        WORD    FILL
        LDI     HIGH (5888)
        PHI     RE
        LDI     LOW (5888)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (MISSION)
        PHI     RA
        LDI     LOW (MISSION)
        PLO     RA
        LDN     RA
        SHL
        ADI     LOW MNT
        PLO     RA
        LDI     HIGH MNT
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     RC
        LDN     RA
        PLO     RC
        LDI     0
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (5914)
        PHI     RE
        LDI     LOW (5914)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (CYCLE)
        PHI     RA
        LDI     LOW (CYCLE)
        PLO     RA
        LDN     RA
        SHL
        ADI     LOW RNT
        PLO     RA
        LDI     HIGH RNT
        ADCI    0
        PHI     RA
        LDA     RA
        PHI     RC
        LDN     RA
        PLO     RC
        LDI     80H
        PLO     RF
        SEP     R4
        WORD    PRT
        LDI     HIGH (HUDF)
        PHI     RA
        LDI     LOW (HUDF)
        PLO     RA
        LDI     1
        STR     RA
        SEP     R4
        WORD    HUDSC
        SEP     R4
        WORD    LIVESHOW
        SEP     R4
        WORD    QFLUSH
        SEP     R4
        WORD    VIDON
        LDI     HIGH (SNDS)
        PHI     RA
        LDI     LOW (SNDS)
        PLO     RA
        LDI     0
        STR     RA
        LDI     HIGH (1A1FH)
        PHI     RC
        LDI     LOW (1A1FH)
        PLO     RC
        LDI     20
        SEP     R4
        WORD    SOUND
        LDI     60
        SEP     R4
        WORD    WAITN
        SEP     R5
GAMEOVER
        SEP     R4
        WORD    SILENCE
        SEP     R4
        WORD    QFLUSHW
        LDI     HIGH (SCORE)
        PHI     RB
        LDI     LOW (SCORE)
        PLO     RB
        LDI     HIGH (HISC)
        PHI     RC
        LDI     LOW (HISC)
        PLO     RC
        LDI     6
        PLO     R9
GO1     LDA     RC
        STR     R2
        LDA     RB
        SM
        LBNF    GO3
        LBNZ    GO2
        DEC     R9
        GLO     R9
        LBNZ    GO1
        LBR     GO3
GO2
        LDI     HIGH (SCORE)
        PHI     RC
        LDI     LOW (SCORE)
        PLO     RC
        LDI     HIGH (HISC)
        PHI     RD
        LDI     LOW (HISC)
        PLO     RD
        LDI     HIGH (6)
        PHI     RE
        LDI     LOW (6)
        PLO     RE
        SEP     R4
        WORD    COPYMEM
GO3
        LDI     HIGH (GOVER)
        PHI     RA
        LDI     LOW (GOVER)
        PLO     RA
        LDN     RA
        XRI     2
        LBZ     TITLE
        SEP     R4
        WORD    VIDOFF
        LDI     HIGH (3087)
        PHI     RE
        LDI     LOW (3087)
        PLO     RE
        SEP     R4
        WORD    POS
        LDI     HIGH (TX13)
        PHI     RC
        LDI     LOW (TX13)
        PLO     RC
        LDI     128
        PLO     RF
        SEP     R4
        WORD    PRT
        SEP     R4
        WORD    VIDON
        LDI     HIGH (1F80H)
        PHI     RC
        LDI     LOW (1F80H)
        PLO     RC
        LDI     40
        SEP     R4
        WORD    NOISE
        LDI     200
        SEP     R4
        WORD    WAITN
        LBR     TITLE
ROWTAB
        WORD    PAGE+0
        WORD    PAGE+40
        WORD    PAGE+80
        WORD    PAGE+120
        WORD    PAGE+160
        WORD    PAGE+200
        WORD    PAGE+240
        WORD    PAGE+280
        WORD    PAGE+320
        WORD    PAGE+360
        WORD    PAGE+400
        WORD    PAGE+440
        WORD    PAGE+480
        WORD    PAGE+520
        WORD    PAGE+560
        WORD    PAGE+600
        WORD    PAGE+640
        WORD    PAGE+680
        WORD    PAGE+720
        WORD    PAGE+760
        WORD    PAGE+800
        WORD    PAGE+840
        WORD    PAGE+880
        WORD    PAGE+920
CHART   BYTE    085H, 086H, 087H, 088H, 089H, 08AH, 08BH, 08BH, 091H, 092H, 08FH, 08FH, 094H, 095H
CHART2  BYTE    000H, 000H, 000H, 08CH, 000H, 090H, 000H
EWIDT   BYTE    001H, 001H, 001H, 002H, 001H, 002H, 001H
ESCT    BYTE    002H, 003H, 004H, 00FH, 005H, 014H, 006H
GTYPE   BYTE    003H, 003H, 003H, 003H, 003H, 003H, 003H, 002H, 002H, 002H, 002H, 002H, 002H, 002H, 001H, 001H
        BYTE    001H, 001H, 001H, 001H, 001H, 001H, 001H, 001H, 001H, 001H, 001H, 001H
GROWT   BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 001H, 001H, 001H, 001H, 001H, 001H, 001H, 002H, 002H
        BYTE    002H, 002H, 002H, 002H, 002H, 003H, 003H, 003H, 003H, 003H, 003H, 003H
GCOLT   BYTE    000H, 001H, 002H, 003H, 004H, 005H, 006H, 000H, 001H, 002H, 003H, 004H, 005H, 006H, 000H, 001H
        BYTE    002H, 003H, 004H, 005H, 006H, 000H, 001H, 002H, 003H, 004H, 005H, 006H
OFSX    BYTE    003H, 002H, 000H, 0FEH, 0FDH, 0FEH, 000H, 002H
OFSY    BYTE    000H, 001H, 002H, 001H, 000H, 0FFH, 0FEH, 0FFH
PATHX   BYTE    015H, 015H, 014H, 013H, 010H, 00EH, 00DH, 00EH, 011H, 015H, 01AH, 01DH, 01EH, 01BH, 015H, 00EH
        BYTE    007H, 004H, 005H, 00AH, 013H, 01DH, 024H, 025H, 013H, 011H, 011H, 00FH, 00FH, 011H, 015H, 017H
        BYTE    01BH, 01BH, 019H, 015H, 00FH, 009H, 007H, 007H, 00BH, 011H, 01BH, 021H, 023H, 023H, 01BH, 011H
        BYTE    011H, 011H, 012H, 013H, 016H, 018H, 019H, 018H, 015H, 011H, 00CH, 009H, 008H, 00BH, 011H, 018H
        BYTE    01FH, 022H, 021H, 01CH, 013H, 009H, 002H, 001H, 013H, 015H, 015H, 017H, 017H, 015H, 011H, 00FH
        BYTE    00BH, 00BH, 00DH, 011H, 017H, 01DH, 01FH, 01FH, 01BH, 015H, 00BH, 005H, 003H, 003H, 00BH, 015H
        BYTE    011H, 011H, 012H, 013H, 016H, 018H, 019H, 018H, 015H, 011H, 00CH, 009H, 008H, 00BH, 011H, 018H
        BYTE    01FH, 022H, 021H, 01CH, 013H, 009H, 002H, 001H, 013H, 015H, 015H, 017H, 017H, 015H, 011H, 00FH
        BYTE    00BH, 00BH, 00DH, 011H, 017H, 01DH, 01FH, 01FH, 01BH, 015H, 00BH, 005H, 003H, 003H, 00BH, 015H
        BYTE    015H, 015H, 014H, 013H, 010H, 00EH, 00DH, 00EH, 011H, 015H, 01AH, 01DH, 01EH, 01BH, 015H, 00EH
        BYTE    007H, 004H, 005H, 00AH, 013H, 01DH, 024H, 025H, 013H, 011H, 011H, 00FH, 00FH, 011H, 015H, 017H
        BYTE    01BH, 01BH, 019H, 015H, 00FH, 009H, 007H, 007H, 00BH, 011H, 01BH, 021H, 023H, 023H, 01BH, 011H
PATHY   BYTE    00AH, 00BH, 00BH, 00CH, 00CH, 00BH, 009H, 008H, 006H, 006H, 007H, 009H, 00CH, 00FH, 010H, 010H
        BYTE    00EH, 00BH, 006H, 003H, 002H, 002H, 006H, 00BH, 00BH, 00BH, 00AH, 00AH, 008H, 007H, 007H, 007H
        BYTE    009H, 00BH, 00DH, 00FH, 00FH, 00EH, 00BH, 007H, 004H, 002H, 003H, 005H, 00AH, 00FH, 012H, 013H
        BYTE    00AH, 009H, 009H, 008H, 008H, 009H, 00BH, 00CH, 00EH, 00EH, 00DH, 00BH, 008H, 005H, 004H, 004H
        BYTE    006H, 009H, 00EH, 011H, 012H, 012H, 00EH, 009H, 009H, 009H, 009H, 00AH, 00BH, 00CH, 00DH, 00CH
        BYTE    00BH, 009H, 006H, 005H, 004H, 006H, 009H, 00CH, 010H, 011H, 011H, 00EH, 00AH, 005H, 002H, 002H
        BYTE    00AH, 00BH, 00BH, 00CH, 00CH, 00BH, 009H, 008H, 006H, 006H, 007H, 009H, 00CH, 00FH, 010H, 010H
        BYTE    00EH, 00BH, 006H, 003H, 002H, 002H, 006H, 00BH, 00BH, 00BH, 00AH, 00AH, 008H, 007H, 007H, 007H
        BYTE    009H, 00BH, 00DH, 00FH, 00FH, 00EH, 00BH, 007H, 004H, 002H, 003H, 005H, 00AH, 00FH, 012H, 013H
        BYTE    00AH, 009H, 009H, 008H, 008H, 009H, 00BH, 00CH, 00EH, 00EH, 00DH, 00BH, 008H, 005H, 004H, 004H
        BYTE    006H, 009H, 00EH, 011H, 012H, 012H, 00EH, 009H, 009H, 009H, 009H, 00AH, 00BH, 00CH, 00DH, 00CH
        BYTE    00BH, 009H, 006H, 005H, 004H, 006H, 009H, 00CH, 010H, 011H, 011H, 00EH, 00AH, 005H, 002H, 002H
MNT
        WORD    MN0
        WORD    MN1
        WORD    MN2
        WORD    MN3
RNT
        WORD    RK0
        WORD    RK1
        WORD    RK2
        WORD    RK3
        WORD    RK4
MN0      TEXT    "MISSION 1  ASTRO GRID"
        BYTE    0
MN1      TEXT    "MISSION 2  LASER FLEET"
        BYTE    0
MN2      TEXT    "MISSION 3  VORTEX"
        BYTE    0
MN3      TEXT    "MISSION 4  COMMAND SHIP"
        BYTE    0
RK0      TEXT    "RANK RECRUIT"
        BYTE    0
RK1      TEXT    "RANK PILOT"
        BYTE    0
RK2      TEXT    "RANK COMMANDER"
        BYTE    0
RK3      TEXT    "RANK ADMIRAL"
        BYTE    0
RK4      TEXT    "RANK LEGION ACE"
        BYTE    0
TUNE    BYTE    01BH, 008H, 014H, 008H, 010H, 008H, 00DH, 010H, 010H, 008H, 014H, 010H, 012H, 008H, 015H, 008H
        BYTE    01BH, 010H, 07FH, 008H, 018H, 008H, 014H, 008H, 010H, 010H, 012H, 008H, 014H, 008H, 015H, 008H
        BYTE    01BH, 008H, 014H, 020H, 07FH, 078H, 07FH, 078H, 000H
TX1      TEXT    "STAR LEGION"
        BYTE    0
TX2      TEXT    "DEFEND THE OUTER WORLDS"
        BYTE    0
TX3      TEXT    "SPACE OR 1   START"
        BYTE    0
TX4      TEXT    "ESC          BACK TO BASIC"
        BYTE    0
TX5      TEXT    "M            MUSIC ON/OFF"
        BYTE    0
TX6      TEXT    "LEFT/RIGHT  MOVE (HOLD)"
        BYTE    0
TX7      TEXT    "SPACE       FIRE"
        BYTE    0
TX8      TEXT    "A           AUTO-FIRE ON/OFF"
        BYTE    0
TX9      TEXT    "HI"
        BYTE    0
TX10     TEXT    "MISSION COMPLETE"
        BYTE    0
TX11     TEXT    "SCORE"
        BYTE    0
TX12     TEXT    "HI"
        BYTE    0
TX13     TEXT    "GAME OVER"
        BYTE    0
CHTAB
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 0C0H, 0C0H, 0C0H, 0C4H, 0C5H, 0CFH, 0DFH, 0F9H
        BYTE    0CCH, 0CCH, 0DEH, 0FFH, 0FFH, 0FFH, 0FFH, 0E1H, 0C0H, 0C0H, 0C0H, 0C8H, 0E8H, 0FCH, 0FEH, 0E7H
        BYTE    0C8H, 0C8H, 0C8H, 0C8H, 0C8H, 0C8H, 0C0H, 0C0H, 080H, 092H, 0BFH, 0ADH, 0BFH, 092H, 0A1H, 080H
        BYTE    080H, 092H, 0BFH, 0ADH, 0BFH, 092H, 092H, 080H, 040H, 04CH, 05EH, 075H, 05EH, 06DH, 061H, 040H
        BYTE    040H, 04CH, 05EH, 06BH, 05EH, 052H, 061H, 040H, 000H, 01EH, 02DH, 03FH, 00CH, 012H, 021H, 000H
        BYTE    000H, 01EH, 02DH, 03FH, 00CH, 012H, 012H, 000H, 000H, 00FH, 01BH, 03FH, 009H, 010H, 000H, 000H
        BYTE    000H, 03CH, 01BH, 03FH, 024H, 002H, 000H, 000H, 040H, 040H, 07FH, 06AH, 055H, 07FH, 040H, 040H
        BYTE    040H, 040H, 069H, 04AH, 051H, 065H, 040H, 040H, 000H, 01FH, 03FH, 037H, 03FH, 012H, 020H, 000H
        BYTE    000H, 03EH, 03FH, 03DH, 03FH, 012H, 001H, 000H, 0C0H, 0E1H, 0F3H, 0DEH, 0CCH, 0D2H, 0C0H, 0C0H
        BYTE    0C0H, 0CCH, 0D2H, 0FFH, 0D2H, 0CCH, 0C0H, 0C0H, 008H, 00CH, 008H, 004H, 008H, 00CH, 008H, 004H
        BYTE    080H, 088H, 09CH, 0B6H, 09CH, 088H, 080H, 080H, 080H, 084H, 08EH, 09BH, 08EH, 084H, 080H, 080H
        BYTE    048H, 054H, 062H, 054H, 048H, 040H, 040H, 040H, 0C0H, 0C0H, 0C0H, 0C0H, 0C1H, 0C7H, 0DFH, 0FFH
        BYTE    0C0H, 0C0H, 0C0H, 0CFH, 0FFH, 0E7H, 0FFH, 0FFH, 0C0H, 0C0H, 0DFH, 0FFH, 0FFH, 0E7H, 0FFH, 0FFH
        BYTE    0CCH, 0FFH, 0FFH, 0FFH, 0FFH, 0E7H, 0FFH, 0FFH, 0C0H, 0C0H, 0FEH, 0FFH, 0FFH, 0E7H, 0FFH, 0FFH
        BYTE    0C0H, 0C0H, 0C0H, 0FCH, 0FFH, 0E7H, 0FFH, 0FFH, 0C0H, 0C0H, 0C0H, 0C0H, 0E0H, 0E0H, 0F8H, 0FFH
        BYTE    0FFH, 0DFH, 0C7H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0DFH, 0FFH, 0FFH, 0FFH, 0C3H, 0C0H, 0C0H, 0C0H
        BYTE    0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C4H, 0C4H, 0C4H, 0C4H, 0C4H, 0C0H, 0C4H, 0C0H
        BYTE    0CAH, 0CAH, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0CAH, 0CAH, 0DFH, 0CAH, 0DFH, 0CAH, 0CAH, 0C0H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 0D8H, 0D9H, 0C2H, 0C4H, 0C8H, 0D3H, 0C3H, 0C0H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 0C4H, 0C4H, 0C8H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H
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
        BYTE    0DFH, 0C1H, 0C2H, 0C4H, 0C8H, 0D0H, 0DFH, 0C0H, 0F7H, 0FFH, 0FFH, 0FFH, 0FFH, 0CFH, 0C0H, 0C0H
        BYTE    0FDH, 0FFH, 0FFH, 0FFH, 0FFH, 0FCH, 0C0H, 0C0H, 0FFH, 0FFH, 0FFH, 0FFH, 0F0H, 0C0H, 0C0H, 0C0H
        BYTE    0DFH, 0FFH, 0F8H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0F7H, 0F8H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    0C0H, 0C1H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 0E2H, 0F7H, 0E2H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    0C8H, 0DDH, 0C8H, 0C0H, 0C0H, 0C0H, 0C0H, 0C0H, 00CH, 01EH, 03FH, 03FH, 01EH, 00CH, 000H, 000H
        BYTE    0BFH, 0ADH, 0BFH, 080H, 080H, 080H, 080H, 080H, 000H, 000H, 00CH, 00CH, 008H, 00CH, 000H, 000H
        BYTE    080H, 080H, 092H, 08CH, 08CH, 092H, 080H, 080H, 080H, 088H, 095H, 08EH, 0AAH, 08AH, 080H, 080H
        BYTE    025H, 010H, 022H, 004H, 024H, 009H, 022H, 000H, 0C0H, 0C0H, 0C0H, 0C8H, 0C0H, 0C0H, 0C0H, 0C0H
        BYTE    040H, 040H, 040H, 040H, 040H, 042H, 040H, 040H, 0C0H, 0C0H, 0CCH, 0DEH, 0FFH, 0E1H, 0C0H, 0C0H
        BYTE    07FH, 06AH, 055H, 040H, 040H, 040H, 040H, 040H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        BYTE    000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H
        END
