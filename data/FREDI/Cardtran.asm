; The Sarnoff Collection at TCNJ, S.572.93, S.572.91, S.572.64, S.572.50, S.572.32, S.572.23,

; Origin set to 00000H, EOF = 003FFH
		ORG  00000H

; CPU Type:
		SYSTEM 00

; CARDTRAN definition
;
; Instruction       Code        Operation
; ===========       ====        =========
; SKIP              0XXX        Skip
; INPUT [2AA]       10AA        Wait for 4 digit word input and put word -> M[2AA]
; JP 2AA            20AA        JumP to 2AA
; JZ DR, 2AA        21AA        Jump to 2AA if DR = 0
; JNZ DR, 2AA       22AA        Jump to 2AA if DR != 0
; LD DR, [2AA]      30AA        M[2AA] -> DR
; ADD [2AA], DR     40AA        M[2AA] + DR -> M[2AA], 000C -> DR
; SUB [2AA], DR     50AA        M[2AA] - DR -> M[2AA], 000B -> DR
; LD [2AA], DR      60AA        DR -> M[2AA]
; DISP0 [2AA]       70AA        Display word on M[2AA] on TV(0) display position
; DISP1 [2AA]       71AA        Display word on M[2AA] on TV(1) display position
; RSH [2AA]         80AA        Ring SHift M[2AA] right 1 digit
; LDD [2AA], DR     90AA        LSD of DR -> LSD of M[2AA]
;
; DR is a 4 digit variable stored at location 02E0/02E1
; AA is 2 digits
;
; Both DR and AA are decimal only, making the max value of AA 99. As each 'address' takes 2 bytes for an
; instruction or 4 digit word, the user area is max 200 bytes, i.e. 0200-02C7
;
; TV display:
; ===========
; TV(0) TV(1)
; TV(2)
;
; CONTROL codes:
; ==============
; F0 -  Load program mode @ M[200].
;       Cardtran interpreter will wait for for a 4 digit word which will be stored at location M[200]
;       next word at M[202] etc. TV(2) position will show the 4 digiti instruction after 4 digits are received.
; F1 -  Execute CARDTRAN program at M[200]
; F2 -  Input AA, DDDD after which DDDD is stored at M[2AA]
; F3 -  Show value of DR on TV(2) display position
; F4 -  Input AA after which 4 digit word stored on M[2AA] is shown on TV(2) display position
; F5 -  Halt execution (except 70/71)
; F6 -  Resume execution
; F7 -  Execute next instruction only

; Labels:
R001C	EQU 001CH
R0036	EQU 0036H
R0043	EQU 0043H
R004A	EQU 004AH
R0053	EQU 0053H
R005C	EQU 005CH
R0068	EQU 0068H
R0077	EQU 0077H
R00A1	EQU 00A1H
R00A5	EQU 00A5H
R00BB	EQU 00BBH
R00BF	EQU 00BFH
R00CC	EQU 00CCH
R00D0	EQU 00D0H
R00DA	EQU 00DAH
R00DE	EQU 00DEH
R00ED	EQU 00EDH
R02EA	EQU 02EAH
S02ED	EQU 02EDH
S02F7	EQU 02F7H
S02FA	EQU 02FAH
S0311	EQU 0311H
R0315	EQU 0315H
R0329	EQU 0329H
S032C	EQU 032CH
R033A	EQU 033AH
R0343	EQU 0343H
R0360	EQU 0360H
R0363	EQU 0363H
R03AE	EQU 03AEH
R03BB	EQU 03BBH
R03BC	EQU 03BCH
R03CC	EQU 03CCH
R03CD	EQU 03CDH
R03D0	EQU 03D0H
R03D6	EQU 03D6H
R03D7	EQU 03D7H
R03E0	EQU 03E0H
S03ED	EQU 03EDH

; Unused or indirect labels:
; S002B
; S009D
; S00CA
; S00F4
; S02E2
; S0320
; S0383
; S03F2
; S03F3
; S03F6
; S03F7

; Register Definitions:
R0		EQU 0  ; Display pointer
R1		EQU 1  ; Interrupt program counter
R2		EQU 2  ; Interrupt save pointer
R3		EQU 3  ; Main program counter
R4		EQU 4  ; Subroutine program counter
R5		EQU 5  ; First operand pointer to unpacked fields
R6		EQU 6  ; Second operand pointer to unpacked fields
R7		EQU 7  ; R7.1: Program status identifier (00 = not execution, 01 = execution)
			   ; R7.0: Counter and utility use
R8		EQU 8  ; Display modification pointer
R9		EQU 9  ; Points at location 0000 for temporary storage
RA		EQU 10 ; Cardtan memory modification pointer
RB		EQU 11 ; Pattern area pointer
RC		EQU 12 ; General purpose work register
RD		EQU 13 ; RD.1: Cardtran instruction register 
			   ; RD.0: Used for temporary address storage
RE		EQU 14 ; Points at two byte work area starting at 0300
RF		EQU 15 ; Cardtran program counter

; 0000-0019
; Initialize registers and call main program
; ===============================================================
;
		DB   00H             ;0000: 00
		GHI  R0              ;0001: 90          
		PLO  R9              ;0002: A9          
		PHI  R9              ;0003: B9          
		PHI  R3              ;0004: B3          
		PHI  R1              ;0005: B1          
		PHI  R2              ;0006: B2          
		PHI  R7              ;0007: B7          
		PLO  RE              ;0008: AE          
		LDA  R0              ;0009: 40          
		DB   01H             ;000A: 01
		PHI  RB              ;000B: BB          
		LDA  R0              ;000C: 40          
		DB   02H             ;000D: 02
		PHI  RA              ;000E: BA          
		PHI  RF              ;000F: BF          
		PHI  R5              ;0010: B5          
		PHI  R6              ;0011: B6          
		LDA  R0              ;0012: 40          
		DB   03H             ;0013: 03
		PHI  R4              ;0014: B4          
		PHI  RE              ;0015: BE          
		LDA  R0              ;0016: 40          
		DB   2BH             ;0017: 2B
		PLO  R3              ;0018: A3          
		SEP  R3              ;0019: D3   

; used for some storage?       
		DB   94H             ;001A: 94
		DB   E0H             ;001B: E0

; 001C-0028
; Interrupt routine
; =================
;
R001C
		LDX                  ;001C: F0          
		DEC  R2              ;001D: 22          
		RET                  ;001E: 70          
		STR  R2              ;001F: 52          
		DEC  R2              ;0020: 22          
		SAV                  ;0021: 78          
		LDA  R1              ;0022: 41          
		DB   80H             ;0023: 80
		PLO  R0              ;0024: A0          
		GHI  RB              ;0025: 9B          
		PHI  R0              ;0026: B0          
		INC  R2              ;0027: 12          
		BR   R001C           ;0028: 30 1C      
		
; 002A-0049
; Initialize interrupt pointers, TV and display area	 
; ==================================================
;
		DB   0E3H            ;002A: E3
S002B
		LDA  R3              ;002B: 43          
		DB   1FH             ;002C: 1F
		PLO  R1              ;002D: A1          
		LDA  R3              ;002E: 43          
		DB   1BH             ;002F: 1B
		PLO  R2              ;0030: A2          
		GHI  RB              ;0031: 9B          
		PHI  R8              ;0032: B8          
		LDA  R3              ;0033: 43          
		DB   0FFH            ;0034: FF
		PLO  R8              ;0035: A8          

; Clear display area 01FF-0180, starting with 01FF
R0036
		GHI  R9              ;0036: 99          
		STR  R8              ;0037: 58          
		GLO  R8              ;0038: 88          
		STR  R9              ;0039: 59          
		LDA  R3              ;003A: 43          
		DB   80H             ;003B: 80
		SEX  R9              ;003C: E9          
		XOR                  ;003D: F3          
		BZ   R0043           ;003E: 32 43       
		DEC  R8              ;0040: 28          
		BR   R0036           ;0041: 30 36       
R0043
		SEX  R3              ;0043: E3          
		OUT  1               ;0044: 61          
		DB   02H             ;0045: 02		; Select TV I/O
		OUT  2               ;0046: 62          
		DB   02H             ;0047: 02		; TV on 64x16
		RET                  ;0048: 70          
		DB   33H             ;0049: 33

; 004A-005B 
; Initialize card reader, read & test input byte for control code
; ===============================================================
;
R004A
		SEX  R3              ;004A: E3          
		OUT  1               ;004B: 61          
		DB   01H             ;004C: 01		; Select card reader
		OUT  2               ;004D: 62          
		DB   01H             ;004E: 01		; Select program mode
		B1   R0053           ;004F: 34 53       
		BR   R0068           ;0051: 30 68       
R0053										; Key pressed
		SEX  R9              ;0053: E9          
		INP                  ;0054: 68          
		LDA  R3              ;0055: 43          
		DB   0F0H            ;0056: F0
		SD                   ;0057: F5          
		BDF  R005C           ;0058: 33 5C	; Branch to 005C if card F0-F7 is used       
		BR   R0068           ;005A: 30 68
		
; 005C-0067
; Determine correct control address and branch
; ============================================       
R005C
		STR  R9              ;005C: 59          
		LDA  R3              ;005D: 43          
		DB   08H             ;005E: 08
		AND                  ;005F: F2          
		BNZ  R004A           ;0060: 31 4A	; For F8-F9, back to check for control code       
		LDA  R3              ;0062: 43          
		DB   75H             ;0063: 75
		ADD                  ;0064: F4          
		PLO  RB              ;0065: AB          
		LDA  RB              ;0066: 4B          
		PLO  R3              ;0067: A3          

; 0068-009C
; Staticize CARDTAN instruction and call respective subroutine
; ============================================================
;
R0068
		GHI  R7              ;0068: 97          
		BZ   R004A           ;0069: 32 4A	; Not executing, back to check for control code      
		LDA  RF              ;006B: 4F          
		PHI  RD              ;006C: BD          
		SHR                  ;006D: F6          
		SHR                  ;006E: F6          
		SHR                  ;006F: F6          
		SHR                  ;0070: F6          
		STR  R9              ;0071: 59          
		LDA  RF              ;0072: 4F          
		STR  RE              ;0073: 5E          
		LDA  R3              ;0074: 43          
		DB   92H             ;0075: 92
		PHI  RC              ;0076: BC          
R0077
		LDA  R3              ;0077: 43          
		DB   0FH             ;0078: 0F
		SEX  RE              ;0079: EE          
		AND                  ;007A: F2          
		INC  RE              ;007B: 1E          
		STR  RE              ;007C: 5E          
		ADD                  ;007D: F4          
		STR  RE              ;007E: 5E          
		DEC  RE              ;007F: 2E          
		LDX                  ;0080: F0          
		SHR                  ;0081: F6          
		SHR                  ;0082: F6          
		SHR                  ;0083: F6          
		SHR                  ;0084: F6          
		STR  RE              ;0085: 5E          
		LDA  R3              ;0086: 43          
		DB   10H             ;0087: 10
		OR                   ;0088: F1          
		PLO  RB              ;0089: AB          
		INC  RE              ;008A: 1E          
		LDX                  ;008B: F0          
		SEX  RB              ;008C: EB          
		ADD                  ;008D: F4          
		PLO  RA              ;008E: AA          
		DEC  RE              ;008F: 2E          
		GHI  RC              ;0090: 9C          
		PLO  R3              ;0091: A3          
		LDA  R3              ;0092: 43          
		DB   6BH             ;0093: 6B
		SEX  R9              ;0094: E9          
		ADD                  ;0095: F4          
		PLO  RB              ;0096: AB          
		LDA  RB              ;0097: 4B          
		PLO  R4              ;0098: A4          
		LDA  R3              ;0099: 43          
		DB   0E0H            ;009A: E0
		PLO  R5              ;009B: A5          
		SEP  R4              ;009C: D4          
S009D
		BR   R004A           ;009D: 30 4A       

; 009F-00FF
; Control codes (including calls to CARDTRAN display, input and D hex routines
; ============================================================================
;
		GHI  R9              ;009F: 99          
		PLO  RF              ;00A0: AF          
R00A1
		B1   R00A5           ;00A1: 34 A5       
		BR   R00A1           ;00A3: 30 A1       
R00A5
		SEX  R9              ;00A5: E9          
		INP                  ;00A6: 68          
		LDA  R3              ;00A7: 43          
		DB   0F0H            ;00A8: F0
		SD                   ;00A9: F5          
		BDF  R005C           ;00AA: 33 5C       
		LDX                  ;00AC: F0          
		STR  RF              ;00AD: 5F          
		INC  RF              ;00AE: 1F          
		GLO  RF              ;00AF: 8F          
		PLO  RA              ;00B0: AA          
		SHR                  ;00B1: F6          
		BDF  R00A1           ;00B2: 33 A1       
		DEC  RA              ;00B4: 2A          
		DEC  RA              ;00B5: 2A          
		LDA  R3              ;00B6: 43          
		DB   0A1H            ;00B7: A1
		PLO  RD              ;00B8: AD          
		BR   R00ED           ;00B9: 30 ED       
R00BB
		B1   R00BF           ;00BB: 34 BF       
		BR   R00BB           ;00BD: 30 BB       
R00BF
		SEX  RE              ;00BF: EE          
		INP                  ;00C0: 68          
		LDA  R3              ;00C1: 43          
		DB   0C6H            ;00C2: C6
		PHI  RC              ;00C3: BC          
		BR   R0077           ;00C4: 30 77       
		LDA  R3              ;00C6: 43          
		DB   11H             ;00C7: 11
		PLO  R4              ;00C8: A4          
		SEP  R4              ;00C9: D4          
S00CA
		BR   R004A           ;00CA: 30 4A       
R00CC
		B1   R00D0           ;00CC: 34 D0       
		BR   R00CC           ;00CE: 30 CC       
R00D0
		SEX  RE              ;00D0: EE          
		INP                  ;00D1: 68          
		LDA  R3              ;00D2: 43          
		DB   0EDH            ;00D3: ED
		PHI  RC              ;00D4: BC          
		LDA  R3              ;00D5: 43          
		DB   0DAH            ;00D6: DA
		PLO  RD              ;00D7: AD          
		BR   R0077           ;00D8: 30 77       
R00DA
		B1   R00DE           ;00DA: 34 DE       
		BR   R00DA           ;00DC: 30 DA       
R00DE
		SEX  R9              ;00DE: E9          
		INP                  ;00DF: 68          
		LDA  R3              ;00E0: 43          
		DB   0F0H            ;00E1: F0
		SD                   ;00E2: F5          
		BDF  R005C           ;00E3: 33 5C       
		BR   R00ED           ;00E5: 30 ED       
		LDA  R3              ;00E7: 43          
		DB   0E0H            ;00E8: E0
		PLO  RA              ;00E9: AA          
		LDA  R3              ;00EA: 43          
		DB   4AH             ;00EB: 4A
		PLO  RD              ;00EC: AD          
R00ED
		LDA  R3              ;00ED: 43          
		DB   0CBH            ;00EE: CB
		PLO  R8              ;00EF: A8          
		LDA  R3              ;00F0: 43          
		DB   2CH             ;00F1: 2C
		PLO  R4              ;00F2: A4          
		SEP  R4              ;00F3: D4          
S00F4
		GLO  RD              ;00F4: 8D          
		PLO  R3              ;00F5: A3          
		GHI  R9              ;00F6: 99          
		PHI  R7              ;00F7: B7          
		BR   R004A           ;00F8: 30 4A       
		GHI  R9              ;00FA: 99          
		PLO  RF              ;00FB: AF          
		GHI  RB              ;00FC: 9B          
		PHI  R7              ;00FD: B7          
		BR   R004A           ;00FE: 30 4A 
		
; 0100-016A
; Patter tables and pointer lists for patter clusters
; ===================================================
;
		DB   2CH             ;0100: 2C points to pattern '0' on 012C
		DB   1AH             ;0101: 1A '1'
		DB   44H             ;0102: 44 '2'
		DB   26H             ;0103: 26 '3'
		DB   51H             ;0104: 51 '4'
		DB   20H             ;0105: 20 '5'
		DB   3EH             ;0106: 3E '6'
		DB   4AH             ;0107: 4A '7'
		DB   32H             ;0108: 32 '8'
		DB   38H             ;0109: 38 '9'
		DB   5FH             ;010A: 5F 'X'
		DB   64H             ;010B: 64 'Y'
		DB   58H             ;010C: 58 '+'
		DB   0C0H            ;010D: C0 space
		DB   0C0H            ;010E: C0 space
		DB   0C0H            ;010F: C0 space
		DB   00H             ;0110: 00
		DB   14H             ;0111: 14 
		DB   28H             ;0112: 28 
		DB   3CH             ;0113: 3C 
		DB   50H             ;0114: 50 
		DB   64H             ;0115: 64
		DB   78H             ;0116: 78
		DB   8CH             ;0117: 8C
		DB   0A0H            ;0118: A0
		DB   0B4H            ;0119: B4
		DB   08H             ;011A: 08 ..X... '1'
		DB   18H             ;011B: 18 X.X...
		DB   08H             ;011C: 08 ..X...
		DB   08H             ;011D: 08 ..X...
		DB   08H             ;011E: 08 ..X...
		DB   08H             ;011F: 08 ..X...
		DB   3EH             ;0120: 3E XXXXX. '5'
		DB   20H             ;0121: 20 X.....
		DB   3CH             ;0122: 3C XXXX..
		DB   02H             ;0123: 02 ....X.
		DB   02H             ;0124: 02 ....X.
		DB   22H             ;0125: 22 X...X.
		DB   1CH             ;0126: 1C .XXX.. '3'
		DB   22H             ;0127: 22 .X..X.
		DB   02H             ;0128: 02 ....X.
		DB   0CH             ;0129: 0C ..XX..
		DB   02H             ;012A: 02 ....X.
		DB   22H             ;012B: 22 .X..X.
		DB   1CH             ;012C: 1C .XXX.. '0'
		DB   22H             ;012D: 22 X...X.
		DB   22H             ;012E: 22 X...X.
		DB   22H             ;012F: 22 X...X.
		DB   22H             ;0130: 22 X...X.
		DB   22H             ;0131: 22 X...X.
		DB   1CH             ;0132: 1C .XXX.. '8'
		DB   22H             ;0133: 22 X...X.
		DB   22H             ;0134: 22 X...X.
		DB   1CH             ;0135: 1C .XXX..
		DB   22H             ;0136: 22 X...X.
		DB   22H             ;0137: 22 X...X.
		DB   1CH             ;0138: 1C .XXX.. '9'
		DB   22H             ;0139: 22 X...X.
		DB   22H             ;013A: 22 X...X.
		DB   1EH             ;013B: 1E .XXXX.
		DB   02H             ;013C: 02 ....X.
		DB   22H             ;013D: 22 X...X.
		DB   1CH             ;013E: 1C .XXX.. '6'
		DB   22H             ;013F: 22 X...X.
		DB   20H             ;0140: 20 X.....
		DB   3CH             ;0141: 3C XXXX..
		DB   22H             ;0142: 22 X...X.
		DB   22H             ;0143: 22 X...X.
		DB   1CH             ;0144: 1C .XXX.. '2'
		DB   22H             ;0145: 22 X...X.
		DB   02H             ;0146: 02 ....X.
		DB   04H             ;0147: 04 ...X..
		DB   08H             ;0148: 08 ..X...
		DB   10H             ;0149: 10 .X....
		DB   3EH             ;014A: 3E XXXXX. '7'
		DB   02H             ;014B: 02 ....X.
		DB   04H             ;014C: 04 ...X..
		DB   08H             ;014D: 08 ..X...
		DB   10H             ;014E: 10 .X....
		DB   20H             ;014F: 20 X.....
		DB   20H             ;0150: 20 X.....
		DB   04H             ;0151: 04 ...X.. '4'
		DB   0CH             ;0152: 0C ..XX..
		DB   14H             ;0153: 14 .X.X..
		DB   24H             ;0154: 24 X..X..
		DB   3EH             ;0155: 3E XXXXX.
		DB   04H             ;0156: 04 ...X..
		DB   04H             ;0157: 04 ...X..
		DB   00H             ;0158: 00 ...... '+'
		DB   08H             ;0159: 08 ..X...
		DB   08H             ;015A: 08 ..X...
		DB   3EH             ;015B: 3E XXXXX.
		DB   08H             ;015C: 08 ..X...
		DB   08H             ;015D: 08 ..X...
		DB   00H             ;015E: 00 ......
		DB   22H             ;015F: 22 X...X. 'X'
		DB   22H             ;0160: 22 X...X.
		DB   14H             ;0161: 14 .X.X..
		DB   08H             ;0162: 08 ..X...
		DB   14H             ;0163: 14 .X.X..
		DB   22H             ;0164: 22 X...X. 'Y'
		DB   22H             ;0165: 22 X...X.
		DB   14H             ;0166: 14 .X.X..
		DB   08H             ;0167: 08 ..X...
		DB   08H             ;0168: 08 ..X...
		DB   08H             ;0169: 08 ..X...
		DB   08H             ;016A: 08 ..X...
		
		DB   0F2H            ;016B: F2
		DB   11H             ;016C: 11
		DB   0F3H            ;016D: F3
		DB   0EDH            ;016E: ED
		DB   7EH             ;016F: 7E
		DB   83H             ;0170: 83
		DB   0F7H            ;0171: F7
		DB   20H             ;0172: 20
		DB   59H             ;0173: 59
		DB   02H             ;0174: 02
		DB   9FH             ;0175: 9F
		DB   0FAH            ;0176: FA
		DB   0BBH            ;0177: BB
		DB   0E7H            ;0178: E7
		DB   0CCH            ;0179: CC
		DB   0F6H            ;017A: F6
		DB   0FCH            ;017B: FC
		DB   6BH             ;017C: 6B
		DB   47H             ;017D: 47
		DB   0F0H            ;017E: F0
		DB   90H             ;017F: 90
		
; 0180-01FF
; Display area
; ============	
;	
		DB   00H             ;0180: 00
		DB   00H             ;0181: 00
		DB   00H             ;0182: 00
		DB   00H             ;0183: 00
		DB   1CH             ;0184: 1C
		DB   1CH             ;0185: 1C
		DB   1CH             ;0186: 1C
		DB   1CH             ;0187: 1C
		DB   00H             ;0188: 00
		DB   00H             ;0189: 00
		DB   00H             ;018A: 00
		DB   00H             ;018B: 00
		DB   22H             ;018C: 22
		DB   22H             ;018D: 22
		DB   22H             ;018E: 22
		DB   22H             ;018F: 22
		DB   00H             ;0190: 00
		DB   00H             ;0191: 00
		DB   00H             ;0192: 00
		DB   00H             ;0193: 00
		DB   02H             ;0194: 02
		DB   02H             ;0195: 02
		DB   02H             ;0196: 02
		DB   02H             ;0197: 02
		DB   00H             ;0198: 00
		DB   00H             ;0199: 00
		DB   00H             ;019A: 00
		DB   00H             ;019B: 00
		DB   04H             ;019C: 04
		DB   04H             ;019D: 04
		DB   04H             ;019E: 04
		DB   04H             ;019F: 04
		DB   00H             ;01A0: 00
		DB   00H             ;01A1: 00
		DB   00H             ;01A2: 00
		DB   00H             ;01A3: 00
		DB   08H             ;01A4: 08
		DB   08H             ;01A5: 08
		DB   08H             ;01A6: 08
		DB   08H             ;01A7: 08
		DB   00H             ;01A8: 00
		DB   00H             ;01A9: 00
		DB   00H             ;01AA: 00
		DB   00H             ;01AB: 00
		DB   10H             ;01AC: 10
		DB   10H             ;01AD: 10
		DB   10H             ;01AE: 10
		DB   10H             ;01AF: 10
		DB   00H             ;01B0: 00
		DB   00H             ;01B1: 00
		DB   00H             ;01B2: 00
		DB   00H             ;01B3: 00
		DB   3EH             ;01B4: 3E
		DB   3EH             ;01B5: 3E
		DB   3EH             ;01B6: 3E
		DB   3EH             ;01B7: 3E
		DB   00H             ;01B8: 00
		DB   00H             ;01B9: 00
		DB   00H             ;01BA: 00
		DB   00H             ;01BB: 00
		DB   00H             ;01BC: 00
		DB   00H             ;01BD: 00
		DB   00H             ;01BE: 00
		DB   00H             ;01BF: 00
		DB   00H             ;01C0: 00 ..... 'space'
		DB   00H             ;01C1: 00 .....
		DB   00H             ;01C2: 00 .....
		DB   00H             ;01C3: 00 .....
		DB   00H             ;01C4: 00 .....
		DB   00H             ;01C5: 00 .....
		DB   00H             ;01C6: 00 .....
		DB   00H             ;01C7: 00
		DB   3EH             ;01C8: 3E
		DB   1CH             ;01C9: 1C
		DB   3EH             ;01CA: 3E
		DB   3EH             ;01CB: 3E
		DB   00H             ;01CC: 00
		DB   00H             ;01CD: 00
		DB   00H             ;01CE: 00
		DB   00H             ;01CF: 00
		DB   02H             ;01D0: 02
		DB   22H             ;01D1: 22
		DB   20H             ;01D2: 20
		DB   20H             ;01D3: 20
		DB   00H             ;01D4: 00
		DB   00H             ;01D5: 00
		DB   00H             ;01D6: 00
		DB   00H             ;01D7: 00
		DB   04H             ;01D8: 04
		DB   22H             ;01D9: 22
		DB   3CH             ;01DA: 3C
		DB   3CH             ;01DB: 3C
		DB   00H             ;01DC: 00
		DB   00H             ;01DD: 00
		DB   00H             ;01DE: 00
		DB   00H             ;01DF: 00
		DB   08H             ;01E0: 08
		DB   22H             ;01E1: 22
		DB   02H             ;01E2: 02
		DB   02H             ;01E3: 02
		DB   00H             ;01E4: 00
		DB   00H             ;01E5: 00
		DB   00H             ;01E6: 00
		DB   00H             ;01E7: 00
		DB   10H             ;01E8: 10
		DB   22H             ;01E9: 22
		DB   02H             ;01EA: 02
		DB   02H             ;01EB: 02
		DB   00H             ;01EC: 00
		DB   00H             ;01ED: 00
		DB   00H             ;01EE: 00
		DB   00H             ;01EF: 00
		DB   20H             ;01F0: 20
		DB   22H             ;01F1: 22
		DB   22H             ;01F2: 22
		DB   22H             ;01F3: 22
		DB   00H             ;01F4: 00
		DB   00H             ;01F5: 00
		DB   00H             ;01F6: 00
		DB   00H             ;01F7: 00
		DB   20H             ;01F8: 20
		DB   1CH             ;01F9: 1C
		DB   1CH             ;01FA: 1C
		DB   1CH             ;01FB: 1C
		DB   00H             ;01FC: 00
		DB   00H             ;01FD: 00
		DB   00H             ;01FE: 00
		DB   00H             ;01FF: 00

; 0200-02C7
; CARDTRAN user memory area
; =========================
; AA is 0 to 99 and points to location 0200-02C7: 0200 AA = 0, 0202 AA = 1 etc.
;
; 0200-0267
; CARDTRAN example code
; =====================
; Note disassembly is using actual hexadecimal addresses instead of decimal CARDTRAN code
;
; This program wait for two four digit numbers, when the numbers are given it will multiply them and show
; a counter counting up to the result. At the end it will wait for two new numbers
; If no input is given I think some random numbers are chosen.
;
        DB   10H, 55H        ;0200: INPUT [26E]
        DB   70H, 55H        ;0202: DISP0 [26E]
        DB   10H, 57H        ;0204: INPUT [272]
        DB   71H, 57H        ;0206: DISP1 [272]
        DB   20H, 07H        ;0208: JP    20E
        DB   00H             ;020A: 00
        DB   01H             ;020B: 01
        DB   03H             ;020C: 03
        DB   50H             ;020D: 50
        DB   30H, 06H        ;020E: LD    DR, [20C]
        DB   60H, 59H        ;0210: LD    [276], DR
        DB   30H, 05H        ;0212: LD    DR, [20A]
        DB   50H, 59H        ;0214: SUB   [276], DR
        DB   30H, 59H        ;0216: LD    DR, [276]
        DB   22H, 09H        ;0218: JNZ   DR, 212
        DB   30H, 55H        ;021A: LD    DR, [26E]
        DB   21H, 17H        ;021C: JZ    DR, 222
        DB   30H, 57H        ;021E: LD    DR, [272]
        DB   22H, 21H        ;0220: JNZ   DR, 22A
        DB   60H, 55H        ;0222: LD    [26E], DR
        DB   70H, 55H        ;0224: DISP0 [26E]
        DB   71H, 55H        ;0226: DISP1 [26E]
        DB   20H, 00H        ;0228: JP    200
        DB   20H, 40H        ;022A: JP    250
        DB   69H             ;022C: 69
        DB   95H             ;022D: 95
        DB   00H             ;022E: 00
        DB   00H             ;022F: 00
        DB   30H, 22H        ;0230: LD    DR, [22C]
        DB   60H, 52H        ;0232: LD    [268], DR
        DB   30H, 57H        ;0234: LD    DR, [272]
        DB   60H, 53H        ;0236: LD    [26A], DR
        DB   70H, 52H        ;0238: DISP0 [268]
        DB   71H, 53H        ;023A: DISP1 [26A]
        DB   30H, 05H        ;023C: LD    DR, [20A]
        DB   50H, 55H        ;023E: SUB   [26E], DR
        DB   30H, 55H        ;0240: LD    DR, [26E]
        DB   21H, 00H        ;0242: JZ    DR, 200
        DB   30H, 57H        ;0244: LD    DR, [272]
        DB   40H, 53H        ;0246: ADD   [26A], DR
        DB   40H, 52H        ;0248: ADD   [268], DR
        DB   20H, 28H        ;024A: JP    238
        DB   00H             ;024C: 00
        DB   00H             ;024D: 00
        DB   00H             ;024E: 00
        DB   00H             ;024F: 00
        DB   30H, 57H        ;0250: LD    DR, [272]
        DB   60H, 56H        ;0252: LD    [270], DR
        DB   30H, 55H        ;0254: LD    DR, [26E]
        DB   50H, 56H        ;0256: SUB   [270], DR
        DB   21H, 24H        ;0258: JZ    DR, 230
        DB   30H, 57H        ;025A: LD    DR, [272]
        DB   60H, 56H        ;025C: LD    [270], DR
        DB   30H, 55H        ;025E: LD    DR, [26E]
        DB   60H, 57H        ;0260: LD    [272], DR
        DB   30H, 56H        ;0262: LD    DR, [270]
        DB   60H, 55H        ;0264: LD    [26E], DR
        DB   20H, 24H        ;0266: JP    230

        DB   01H             ;0268: 01
		DB   23H             ;0269: 23
		DB   43H             ;026A: 43
		DB   21H             ;026B: 21
		DB   0FFH            ;026C: FF
		DB   0FFH            ;026D: FF
		DB   0F3H            ;026E: F3
		DB   0F3H            ;026F: F3
		DB   00H             ;0270: 00
		DB   00H             ;0271: 00
		DB   22H             ;0272: 22
		DB   22H             ;0273: 22
		DB   0FFH            ;0274: FF
		DB   0FFH            ;0275: FF
		DB   03H             ;0276: 03
		DB   50H             ;0277: 50
		DB   0FFH            ;0278: FF
		DB   0FFH            ;0279: FF
		DB   0FFH            ;027A: FF
		DB   0FFH            ;027B: FF
		DB   0FFH            ;027C: FF
		DB   0FFH            ;027D: FF
		DB   0FFH            ;027E: FF
		DB   0FFH            ;027F: FF
		DB   00H             ;0280: 00
		DB   90H             ;0281: 90
		DB   0B3H            ;0282: B3
		DB   0B4H            ;0283: B4
		DB   0B5H            ;0284: B5
		DB   0B6H            ;0285: B6
		DB   0B7H            ;0286: B7
		DB   0E0H            ;0287: E0
		DB   80H             ;0288: 80
		DB   1DH             ;0289: 1D
		DB   0F4H            ;028A: F4
		DB   2DH             ;028B: 2D
		DB   0A5H            ;028C: A5
		DB   0A6H            ;028D: A6
		DB   16H             ;028E: 16
		DB   16H             ;028F: 16
		DB   16H             ;0290: 16
		DB   85H             ;0291: 85
		DB   56H             ;0292: 56
		DB   16H             ;0293: 16
		DB   16H             ;0294: 16
		DB   16H             ;0295: 16
		DB   16H             ;0296: 16
		DB   16H             ;0297: 16
		DB   86H             ;0298: 86
		DB   0A7H            ;0299: A7
		DB   16H             ;029A: 16
		DB   16H             ;029B: 16
		DB   16H             ;029C: 16
		DB   85H             ;029D: 85
		DB   56H             ;029E: 56
		DB   16H             ;029F: 16
		DB   86H             ;02A0: 86
		DB   57H             ;02A1: 57
		DB   16H             ;02A2: 16
		DB   16H             ;02A3: 16
		DB   16H             ;02A4: 16
		DB   16H             ;02A5: 16
		DB   86H             ;02A6: 86
		DB   0A4H            ;02A7: A4
		DB   80H             ;02A8: 80
		DB   0A3H            ;02A9: A3
		DB   13H             ;02AA: 13
		DB   0D3H            ;02AB: D3
		DB   43H             ;02AC: 43
		DB   00H             ;02AD: 00
		DB   0A0H            ;02AE: A0
		DB   0B0H            ;02AF: B0
		DB   0E3H            ;02B0: E3
		DB   61H             ;02B1: 61
		DB   03H             ;02B2: 03
		DB   62H             ;02B3: 62
		DB   40H             ;02B4: 40
		DB   0E4H            ;02B5: E4
		DB   90H             ;02B6: 90
		DB   0F3H            ;02B7: F3
		DB   31H             ;02B8: 31
		DB   0B6H            ;02B9: B6
		DB   14H             ;02BA: 14
		DB   80H             ;02BB: 80
		DB   0F3H            ;02BC: F3
		DB   32H             ;02BD: 32
		DB   0C2H            ;02BE: C2
		DB   24H             ;02BF: 24
		DB   30H             ;02C0: 30
		DB   0B6H            ;02C1: B6
		DB   0E3H            ;02C2: E3
		DB   62H             ;02C3: 62
		DB   00H             ;02C4: 00
		DB   00H             ;02C5: 00
		DB   03H             ;02C6: 03
		DB   0FFH            ;02C7: FF
		
; 02C8-02E1
; Add-subtract work area, CARDTAN register and byte inversion table
; =================================================================
;
		DB   02H             ;02C8: 02
		DB   02H             ;02C9: 02
		DB   02H             ;02CA: 02
		DB   02H             ;02CB: 02
		DB   06H             ;02CC: 06
		DB   09H             ;02CD: 09
		DB   09H             ;02CE: 09
		DB   05H             ;02CF: 05
		DB   00H             ;02D0: 00
		DB   10H             ;02D1: 10
		DB   20H             ;02D2: 20
		DB   30H             ;02D3: 30
		DB   40H             ;02D4: 40
		DB   50H             ;02D5: 50
		DB   60H             ;02D6: 60
		DB   70H             ;02D7: 70
		DB   80H             ;02D8: 80
		DB   90H             ;02D9: 90
		DB   0A0H            ;02DA: A0
		DB   0B0H            ;02DB: B0
		DB   0C0H            ;02DC: C0
		DB   0D0H            ;02DD: D0
		DB   0E0H            ;02DE: E0
		DB   0F0H            ;02DF: F0
		
; 02E0-02E1
; DR storage
; ==========
;
		DB   03H             ;02E0: 03
		DB   50H             ;02E1: 50

; 02E2-02FD
; CARDTRAN branch subroutine
; ==========================
;	
S02E2
		GHI  RD              ;02E2: 9D          
		STR  R9              ;02E3: 59          
		LDA  R6              ;02E4: 46          
		DB   0FH             ;02E5: 0F
		SEX  R9              ;02E6: E9          
		AND                  ;02E7: F2          
		BNZ  S02ED           ;02E8: 31 ED       
R02EA
		GLO  RA              ;02EA: 8A          
		PLO  RF              ;02EB: AF          
		SEP  R3              ;02EC: D3          
S02ED
		SHR                  ;02ED: F6          
		LDA  R5              ;02EE: 45          
		BDF  S02F7           ;02EF: 33 F7       
		BNZ  R02EA           ;02F1: 31 EA       
		LDA  R5              ;02F3: 45          
		BNZ  R02EA           ;02F4: 31 EA       
		SEP  R3              ;02F6: D3          
S02F7
		BZ   S02FA           ;02F7: 32 FA       
		SEP  R3              ;02F9: D3          
S02FA
		LDA  R5              ;02FA: 45          
		BZ   R02EA           ;02FB: 32 EA       
		SEP  R3              ;02FD: D3 
		         
		DB   47H             ;02FE: 47
		DB   0F0H            ;02FF: F0

; 0300-0301
; Data storage, used by CARDTRAN interpreter
; ==========================================
;
		DB   00H             ;0300: 00
		DB   12H             ;0301: 12
		
; 0302-0310
; CARDTRAN store digit subroutine
; ===============================
;
		INC  RA              ;0302: 1A          
		INC  R5              ;0303: 15          
		LDA  R5              ;0304: 45          
		PNI  R9              ;0305: C9          
		LDA  R4              ;0306: 44          
		LDX                  ;0307: F0          
		SEX  RA              ;0308: EA          
		AND                  ;0309: F2          
		STR  RA              ;030A: 5A          
		GLO  R9              ;030B: 89          
		OR                   ;030C: F1          
		STR  RA              ;030D: 5A          
		GHI  R9              ;030E: 99          
		PLO  R9              ;030F: A9          
		SEP  R3              ;0310: D3          

; 0311-031F
; CARDTRAN input subroutine (also shared with control code)
; =========================================================
;
S0311
		B1   R0315           ;0311: 34 15       
		BR   S0311           ;0313: 30 11       
R0315
		SEX  RA              ;0315: EA          
		INP                  ;0316: 68          
		INC  RA              ;0317: 1A          
		GLO  RA              ;0318: 8A          
		STR  R9              ;0319: 59          
		GHI  RB              ;031A: 9B          
		SEX  R9              ;031B: E9          
		AND                  ;031C: F2          
		BNZ  S0311           ;031D: 31 11       
		SEP  R3              ;031F: D3  
		
; 0320-0358
; ? CARDTRAN display subroutine (also shared with control code) ?
; ===============================================================
;        
S0320
		GHI  RD              ;0320: 9D          
		SHR                  ;0321: F6          
		BDF  R0329           ;0322: 33 29       
		LDA  R4              ;0324: 44          
		DB   83H             ;0325: 83
		PLO  R8              ;0326: A8          
		BR   S032C           ;0327: 30 2C       
R0329
		LDA  R4              ;0329: 44          
		DB   87H             ;032A: 87
		PLO  R8              ;032B: A8          
S032C
		LDA  R4              ;032C: 44          
		DB   08H             ;032D: 08
		STR  R9              ;032E: 59          
		LDA  R4              ;032F: 44          
		DB   0C8H            ;0330: C8
		PLO  R5              ;0331: A5          
		LDA  R4              ;0332: 44          
		DB   37H             ;0333: 37
		PHI  RC              ;0334: BC          
		BR   R0360           ;0335: 30 60       
		LDA  R4              ;0337: 44          
		DB   04H             ;0338: 04
		PLO  RC              ;0339: AC          
R033A
		LDA  R4              ;033A: 44          
		DB   07H             ;033B: 07
		PLO  R7              ;033C: A7          
		DEC  R5              ;033D: 25          
		SEX  R5              ;033E: E5          
		LDX                  ;033F: F0          
		PLO  RB              ;0340: AB          
		LDA  RB              ;0341: 4B          
		PLO  RB              ;0342: AB          
R0343
		LDA  RB              ;0343: 4B          
		STR  R8              ;0344: 58          
		GLO  R8              ;0345: 88          
		SEX  R9              ;0346: E9          
		ADD                  ;0347: F4          
		PLO  R8              ;0348: A8          
		DEC  R7              ;0349: 27          
		GLO  R7              ;034A: 87          
		BNZ  R0343           ;034B: 31 43       
		GLO  R8              ;034D: 88          
		STR  RE              ;034E: 5E          
		LDA  R4              ;034F: 44          
		DB   39H             ;0350: 39
		SEX  RE              ;0351: EE          
		SD                   ;0352: F5          
		PLO  R8              ;0353: A8          
		DEC  RC              ;0354: 2C          
		GLO  RC              ;0355: 8C          
		BNZ  R033A           ;0356: 31 3A       
		SEP  R3              ;0358: D3       
		   
		DB   44H             ;0359: 44
		DB   0CCH            ;035A: CC
		DB   0A5H            ;035B: A5
		DB   0A6H            ;035C: A6
		DB   44H             ;035D: 44
		DB   76H             ;035E: 76
		DB   0BCH            ;035F: BC

R0360
		GHI  RF              ;0360: 9F          
		PLO  RC              ;0361: AC          
		SEX  RA              ;0362: EA          
R0363
		LDX                  ;0363: F0          
		SHR                  ;0364: F6          
		SHR                  ;0365: F6          
		SHR                  ;0366: F6          
		SHR                  ;0367: F6          
		STR  R5              ;0368: 55          
		INC  R5              ;0369: 15          
		LDA  R4              ;036A: 44          
		DB   0FH             ;036B: 0F
		AND                  ;036C: F2          
		STR  R5              ;036D: 55          
		INC  RA              ;036E: 1A          
		DEC  RC              ;036F: 2C          
		INC  R5              ;0370: 15          
		GLO  RC              ;0371: 8C          
		BNZ  R0363           ;0372: 31 63       
		GHI  RC              ;0374: 9C          
		PLO  R4              ;0375: A4   
		       
		DB   2AH             ;0376: 2A
		DB   2AH             ;0377: 2A
		DB   25H             ;0378: 25
		DB   26H             ;0379: 26
		DB   45H             ;037A: 45
		DB   56H             ;037B: 56
		DB   30H             ;037C: 30
		DB   0E1H            ;037D: E1
		DB   44H             ;037E: 44
		DB   0F4H            ;037F: F4
		DB   59H             ;0380: 59
		DB   30H             ;0381: 30
		DB   86H             ;0382: 86
S0383
		LDA  R4              ;0383: 44          
		DB   0F5H            ;0384: F5
		STR  R9              ;0385: 59          
		LDA  R4              ;0386: 44          
		DB   0B2H            ;0387: B2
		PLO  RE              ;0388: AE          
		SEX  R9              ;0389: E9          
		LDX                  ;038A: F0          
		STR  RE              ;038B: 5E          
		LDA  R4              ;038C: 44          
		DB   0B6H            ;038D: B6
		PLO  RE              ;038E: AE          
		LDX                  ;038F: F0          
		STR  RE              ;0390: 5E          
		GHI  R9              ;0391: 99          
		PLO  RE              ;0392: AE          
		LDA  R4              ;0393: 44          
		DB   0CCH            ;0394: CC
		PLO  R5              ;0395: A5          
		LDA  R4              ;0396: 44          
		DB   9DH             ;0397: 9D
		PHI  RC              ;0398: BC          
		GLO  RA              ;0399: 8A          
		PLO  R7              ;039A: A7          
		BR   R0360           ;039B: 30 60       
		DEC  R5              ;039D: 25          
		GLO  R5              ;039E: 85          
		PLO  R6              ;039F: A6          
		LDA  R4              ;03A0: 44          
		DB   0C8H            ;03A1: C8
		PLO  R5              ;03A2: A5          
		LDA  R4              ;03A3: 44          
		DB   0E0H            ;03A4: E0
		PLO  RA              ;03A5: AA          
		LDA  R4              ;03A6: 44          
		DB   0ABH            ;03A7: AB
		PHI  RC              ;03A8: BC          
		BR   R0360           ;03A9: 30 60     
		  
		DEC  R5              ;03AB: 25          
		GHI  R9              ;03AC: 99          
		STR  R9              ;03AD: 59          
R03AE
		LDA  R5              ;03AE: 45          
		SEX  R9              ;03AF: E9          
		ADD                  ;03B0: F4          
		SEX  R6              ;03B1: E6          
		SD                   ;03B2: F5          
		STR  R6              ;03B3: 56          
		LDA  R4              ;03B4: 44          
		DB   0F6H            ;03B5: F6
		SD                   ;03B6: F5          
		BDF  R03BB           ;03B7: 33 BB       
		BR   R03BC           ;03B9: 30 BC       
R03BB
		STR  R6              ;03BB: 56          
R03BC
		DEC  R6              ;03BC: 26          
		GLO  R6              ;03BD: 86          
		STR  R9              ;03BE: 59          
		LDA  R4              ;03BF: 44          
		DB   0CBH            ;03C0: CB
		SEX  R9              ;03C1: E9          
		XOR                  ;03C2: F3          
		BZ   R03D0           ;03C3: 32 D0       
		DEC  R5              ;03C5: 25          
		DEC  R5              ;03C6: 25          
		BDF  R03CC           ;03C7: 33 CC       
		GHI  R9              ;03C9: 99          
		BR   R03CD           ;03CA: 30 CD       
R03CC
		GHI  RB              ;03CC: 9B          
R03CD
		STR  R9              ;03CD: 59          
		BR   R03AE           ;03CE: 30 AE       
R03D0
		DEC  RA              ;03D0: 2A          
		BDF  R03D6           ;03D1: 33 D6       
		GHI  R9              ;03D3: 99          
		BR   R03D7           ;03D4: 30 D7       
R03D6
		GHI  RB              ;03D6: 9B          
R03D7
		STR  RA              ;03D7: 5A          
		DEC  RA              ;03D8: 2A          
		GHI  R9              ;03D9: 99          
		STR  RA              ;03DA: 5A          
		GLO  R7              ;03DB: 87          
		PLO  RA              ;03DC: AA          
		LDA  R4              ;03DD: 44          
		DB   0D0H            ;03DE: D0
		PLO  R5              ;03DF: A5          
R03E0
		INC  R6              ;03E0: 16          
		LDA  R6              ;03E1: 46          
		PNI  R5              ;03E2: C5          
		LDA  R5              ;03E3: 45          
		SEX  R6              ;03E4: E6          
		OR                   ;03E5: F1          
		STR  RA              ;03E6: 5A          
		INC  RA              ;03E7: 1A          
		GLO  RA              ;03E8: 8A          
		SHR                  ;03E9: F6          
		BDF  R03E0           ;03EA: 33 E0       
		SEP  R3              ;03EC: D3          
S03ED
		LDA  RA              ;03ED: 4A          
		STR  R5              ;03EE: 55          
		INC  R5              ;03EF: 15          
		LDA  RA              ;03F0: 4A          
		STR  R5              ;03F1: 55          
S03F2
		SEP  R3              ;03F2: D3          
S03F3
		LDA  R4              ;03F3: 44          
		DB   0E2H            ;03F4: E2
		PLO  R6              ;03F5: A6          
S03F6
		SEP  R6              ;03F6: D6          
S03F7
		GLO  RA              ;03F7: 8A          
		PLO  R5              ;03F8: A5          
		LDA  R4              ;03F9: 44          
		DB   0E0H            ;03FA: E0
		PLO  RA              ;03FB: AA          
		BR   S03ED           ;03FC: 30 ED       
		DB   00H             ;03FE: 00
		DB   00H             ;03FF: 00
		END
