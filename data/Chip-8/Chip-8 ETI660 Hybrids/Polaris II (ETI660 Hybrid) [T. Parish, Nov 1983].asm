; Origin set to 00600H, EOF = 008FFH
        ORG  00600H

; CPU Type:
        CPU 1805

; Unused or indirect labels:
; S08B2
; S08C1
; S08D1

; Register Definitions:
R0        EQU 0
R1        EQU 1
R2        EQU 2
R3        EQU 3
R4        EQU 4
R5        EQU 5
R6        EQU 6
R7        EQU 7
R8        EQU 8
R9        EQU 9
RA        EQU 10
RB        EQU 11
RC        EQU 12
RD        EQU 13
RE        EQU 14
RF        EQU 15

; Start code segment
        DB   08H, 0B2H           ;0600: SYS   8B2           
        DB   08H, 0C1H           ;0602: SYS   8C1           
        DB   60H, 01H            ;0604: LD    V0, 01        
        DB   61H, 00H            ;0606: LD    V1, 00        
        DB   62H, 00H            ;0608: LD    V2, 00        
        DB   63H, 08H            ;060A: LD    V3, 08        
        DB   64H, 04H            ;060C: LD    V4, 04        
        DB   28H, 90H            ;060E: CALL  890           
        DB   62H, 04H            ;0610: LD    V2, 04        
        DB   64H, 08H            ;0612: LD    V4, 08        
        DB   60H, 05H            ;0614: LD    V0, 05        
        DB   28H, 90H            ;0616: CALL  890           
        DB   62H, 0CH            ;0618: LD    V2, 0C        
        DB   64H, 01H            ;061A: LD    V4, 01        
        DB   60H, 06H            ;061C: LD    V0, 06        
        DB   28H, 90H            ;061E: CALL  890           
        DB   62H, 0DH            ;0620: LD    V2, 0D        
        DB   64H, 0AH            ;0622: LD    V4, 0A        
        DB   60H, 05H            ;0624: LD    V0, 05        
        DB   28H, 90H            ;0626: CALL  890           
        DB   62H, 17H            ;0628: LD    V2, 17        
        DB   64H, 01H            ;062A: LD    V4, 01        
        DB   60H, 04H            ;062C: LD    V0, 04        
        DB   28H, 90H            ;062E: CALL  890           
        DB   61H, 18H            ;0630: LD    V1, 18        
        DB   60H, 00H            ;0632: LD    V0, 00        
        DB   0A8H, 36H           ;0634: LD    I, 836        
        DB   0D0H, 11H           ;0636: DRW   V0, V1, 1     
        DB   70H, 08H            ;0638: ADD   V0, 08        
        DB   30H, 40H            ;063A: SE    V0, 40        
        DB   16H, 36H            ;063C: JP    636           
        DB   62H, 00H            ;063E: LD    V2, 00        
        DB   68H, 2CH            ;0640: LD    V8, 2C        
        DB   0A7H, 10H           ;0642: LD    I, 710        
        DB   0D2H, 82H           ;0644: DRW   V2, V8, 2     
        DB   61H, 2FH            ;0646: LD    V1, 2F        
        DB   60H, 00H            ;0648: LD    V0, 00        
        DB   0A8H, 30H           ;064A: LD    I, 830        
        DB   0D0H, 11H           ;064C: DRW   V0, V1, 1     
        DB   70H, 08H            ;064E: ADD   V0, 08        
        DB   30H, 40H            ;0650: SE    V0, 40        
        DB   16H, 4CH            ;0652: JP    64C           
        DB   63H, 00H            ;0654: LD    V3, 00        
        DB   61H, 01H            ;0656: LD    V1, 01        
        DB   0A8H, 2EH           ;0658: LD    I, 82E        
        DB   6BH, 20H            ;065A: LD    VB, 20        
        DB   6CH, 20H            ;065C: LD    VC, 20        
        DB   0DBH, 0C2H          ;065E: DRW   VB, VC, 2     
        DB   0A8H, 2CH           ;0660: LD    I, 82C        
        DB   6DH, 00H            ;0662: LD    VD, 00        
        DB   6EH, 02H            ;0664: LD    VE, 02        
        DB   0DDH, 0E2H          ;0666: DRW   VD, VE, 2     
        DB   67H, 00H            ;0668: LD    V7, 00        
        DB   65H, 00H            ;066A: LD    V5, 00        
        DB   6AH, 00H            ;066C: LD    VA, 00        
        DB   69H, 00H            ;066E: LD    V9, 00        
        DB   80H, 0B0H           ;0670: LD    V0, VB        
        DB   80H, 0D5H           ;0672: SUB   V0, VD        
        DB   40H, 09H            ;0674: SNE   V0, 09        
        DB   16H, 7CH            ;0676: JP    67C           
        DB   30H, 08H            ;0678: SE    V0, 08        
        DB   16H, 88H            ;067A: JP    688           
        DB   3AH, 00H            ;067C: SE    VA, 00        
        DB   16H, 88H            ;067E: JP    688           
        DB   62H, 00H            ;0680: LD    V2, 00        
        DB   0A6H, 5DH           ;0682: LD    I, 65D        
        DB   6AH, 04H            ;0684: LD    VA, 04        
        DB   0DDH, 0A1H          ;0686: DRW   VD, VA, 1     
        DB   0A8H, 2CH           ;0688: LD    I, 82C        
        DB   0DDH, 0E2H          ;068A: DRW   VD, VE, 2     
        DB   7DH, 01H            ;068C: ADD   VD, 01        
        DB   3DH, 3EH            ;068E: SE    VD, 3E        
        DB   16H, 94H            ;0690: JP    694           
        DB   6DH, 00H            ;0692: LD    VD, 00        
        DB   0DDH, 0E2H          ;0694: DRW   VD, VE, 2     
        DB   80H, 0D0H           ;0696: LD    V0, VD        
        DB   32H, 00H            ;0698: SE    V2, 00        
        DB   80H, 24H            ;069A: ADD   V0, V2        
        DB   3AH, 18H            ;069C: SE    VA, 18        
        DB   16H, 0A4H           ;069E: JP    6A4           
        DB   89H, 00H            ;06A0: LD    V9, V0        
        DB   79H, 0FFH           ;06A2: ADD   V9, FF        
        DB   4AH, 00H            ;06A4: SNE   VA, 00        
        DB   17H, 1AH            ;06A6: JP    71A           
        DB   0A6H, 5DH           ;06A8: LD    I, 65D        
        DB   39H, 00H            ;06AA: SE    V9, 00        
        DB   16H, 0BAH           ;06AC: JP    6BA           
        DB   70H, 0FFH           ;06AE: ADD   V0, FF        
        DB   0D0H, 0A1H          ;06B0: DRW   V0, VA, 1     
        DB   70H, 01H            ;06B2: ADD   V0, 01        
        DB   7AH, 02H            ;06B4: ADD   VA, 02        
        DB   0D0H, 0A1H          ;06B6: DRW   V0, VA, 1     
        DB   17H, 1AH            ;06B8: JP    71A           
        DB   0D9H, 0A1H          ;06BA: DRW   V9, VA, 1     
        DB   9AH, 0C0H           ;06BC: SNE   VA, VC        
        DB   16H, 0CAH           ;06BE: JP    6CA           
        DB   7AH, 01H            ;06C0: ADD   VA, 01        
        DB   9AH, 0C0H           ;06C2: SNE   VA, VC        
        DB   16H, 0CAH           ;06C4: JP    6CA           
        DB   0D9H, 0A1H          ;06C6: DRW   V9, VA, 1     
        DB   17H, 1AH            ;06C8: JP    71A           
        DB   0A8H, 31H           ;06CA: LD    I, 831        
        DB   8AH, 0E5H           ;06CC: SUB   VA, VE        
        DB   0D9H, 0A5H          ;06CE: DRW   V9, VA, 5     
        DB   80H, 90H            ;06D0: LD    V0, V9        
        DB   70H, 02H            ;06D2: ADD   V0, 02        
        DB   80H, 0B5H           ;06D4: SUB   V0, VB        
        DB   3FH, 01H            ;06D6: SE    VF, 01        
        DB   17H, 14H            ;06D8: JP    714           
        DB   6EH, 06H            ;06DA: LD    VE, 06        
        DB   8EH, 05H            ;06DC: SUB   VE, V0        
        DB   6EH, 02H            ;06DE: LD    VE, 02        
        DB   4FH, 00H            ;06E0: SNE   VF, 00        
        DB   17H, 14H            ;06E2: JP    714           
        DB   60H, 60H            ;06E4: LD    V0, 60        
        DB   0F0H, 00H           ;06E6: LD    P, V0         
        DB   70H, 01H            ;06E8: ADD   V0, 01        
        DB   0F1H, 18H           ;06EA: LD    ST, V1        
        DB   30H, 00H            ;06EC: SE    V0, 00        
        DB   16H, 0E6H           ;06EE: JP    6E6           
        DB   63H, 18H            ;06F0: LD    V3, 18        
        DB   64H, 0AH            ;06F2: LD    V4, 0A        
        DB   0A8H, 38H           ;06F4: LD    I, 838        
        DB   0F5H, 33H           ;06F6: LD    B, V5         
        DB   0F2H, 65H           ;06F8: LD    V2, [I]       
        DB   0F0H, 29H           ;06FA: LD    F, V0         
        DB   0D3H, 45H           ;06FC: DRW   V3, V4, 5     
        DB   0F1H, 29H           ;06FE: LD    F, V1         
        DB   73H, 05H            ;0700: ADD   V3, 05        
        DB   0D3H, 45H           ;0702: DRW   V3, V4, 5     
        DB   0F2H, 29H           ;0704: LD    F, V2         
        DB   73H, 05H            ;0706: ADD   V3, 05        
        DB   0D3H, 45H           ;0708: DRW   V3, V4, 5     
        DB   60H, 08H            ;070A: LD    V0, 08        
        DB   0E0H, 9EH           ;070C: SKP   V0            
        DB   17H, 0AH            ;070E: JP    70A           
        DB   00H, 0E0H           ;0710: CLS                 
        DB   16H, 02H            ;0712: JP    602           
        DB   0D9H, 0A5H          ;0714: DRW   V9, VA, 5     
        DB   6AH, 00H            ;0716: LD    VA, 00        
        DB   89H, 0A0H           ;0718: LD    V9, VA        
        DB   0A8H, 37H           ;071A: LD    I, 837        
        DB   33H, 00H            ;071C: SE    V3, 00        
        DB   17H, 24H            ;071E: JP    724           
        DB   84H, 0C0H           ;0720: LD    V4, VC        
        DB   0D3H, 41H           ;0722: DRW   V3, V4, 1     
        DB   0D3H, 41H           ;0724: DRW   V3, V4, 1     
        DB   73H, 01H            ;0726: ADD   V3, 01        
        DB   0D3H, 41H           ;0728: DRW   V3, V4, 1     
        DB   80H, 30H            ;072A: LD    V0, V3        
        DB   80H, 0B5H           ;072C: SUB   V0, VB        
        DB   3FH, 01H            ;072E: SE    VF, 01        
        DB   17H, 48H            ;0730: JP    748           
        DB   6EH, 06H            ;0732: LD    VE, 06        
        DB   8EH, 05H            ;0734: SUB   VE, V0        
        DB   6EH, 02H            ;0736: LD    VE, 02        
        DB   3FH, 01H            ;0738: SE    VF, 01        
        DB   17H, 48H            ;073A: JP    748           
        DB   94H, 0C0H           ;073C: SNE   V4, VC        
        DB   18H, 24H            ;073E: JP    824           
        DB   80H, 0C0H           ;0740: LD    V0, VC        
        DB   70H, 01H            ;0742: ADD   V0, 01        
        DB   94H, 00H            ;0744: SNE   V4, V0        
        DB   18H, 24H            ;0746: JP    824           
        DB   33H, 3EH            ;0748: SE    V3, 3E        
        DB   17H, 50H            ;074A: JP    750           
        DB   0D3H, 41H           ;074C: DRW   V3, V4, 1     
        DB   63H, 00H            ;074E: LD    V3, 00        
        DB   80H, 0D0H           ;0750: LD    V0, VD        
        DB   6FH, 03H            ;0752: LD    VF, 03        
        DB   80H, 0F2H           ;0754: AND   V0, VF        
        DB   30H, 00H            ;0756: SE    V0, 00        
        DB   17H, 9AH            ;0758: JP    79A           
        DB   0A8H, 2EH           ;075A: LD    I, 82E        
        DB   0DBH, 0C2H          ;075C: DRW   VB, VC, 2     
        DB   60H, 05H            ;075E: LD    V0, 05        
        DB   0E0H, 0A1H          ;0760: SKNP  V0            
        DB   17H, 78H            ;0762: JP    778           
        DB   60H, 07H            ;0764: LD    V0, 07        
        DB   0E0H, 0A1H          ;0766: SKNP  V0            
        DB   17H, 7EH            ;0768: JP    77E           
        DB   60H, 06H            ;076A: LD    V0, 06        
        DB   0E0H, 0A1H          ;076C: SKNP  V0            
        DB   17H, 84H            ;076E: JP    784           
        DB   60H, 0EH            ;0770: LD    V0, 0E        
        DB   0E0H, 0A1H          ;0772: SKNP  V0            
        DB   17H, 8AH            ;0774: JP    78A           
        DB   17H, 98H            ;0776: JP    798           
        DB   3BH, 01H            ;0778: SE    VB, 01        
        DB   7BH, 0FFH           ;077A: ADD   VB, FF        
        DB   17H, 98H            ;077C: JP    798           
        DB   3BH, 38H            ;077E: SE    VB, 38        
        DB   7BH, 01H            ;0780: ADD   VB, 01        
        DB   17H, 98H            ;0782: JP    798           
        DB   3CH, 19H            ;0784: SE    VC, 19        
        DB   7CH, 0FFH           ;0786: ADD   VC, FF        
        DB   17H, 98H            ;0788: JP    798           
        DB   80H, 80H            ;078A: LD    V0, V8        
        DB   9CH, 00H            ;078C: SNE   VC, V0        
        DB   17H, 98H            ;078E: JP    798           
        DB   70H, 0FFH           ;0790: ADD   V0, FF        
        DB   9CH, 00H            ;0792: SNE   VC, V0        
        DB   17H, 98H            ;0794: JP    798           
        DB   7CH, 01H            ;0796: ADD   VC, 01        
        DB   0DBH, 0C2H          ;0798: DRW   VB, VC, 2     
        DB   0A6H, 5DH           ;079A: LD    I, 65D        
        DB   60H, 00H            ;079C: LD    V0, 00        
        DB   0E0H, 9EH           ;079E: SKP   V0            
        DB   17H, 0C0H           ;07A0: JP    7C0           
        DB   37H, 00H            ;07A2: SE    V7, 00        
        DB   17H, 0C0H           ;07A4: JP    7C0           
        DB   60H, 08H            ;07A6: LD    V0, 08        
        DB   0F0H, 00H           ;07A8: LD    P, V0         
        DB   70H, 04H            ;07AA: ADD   V0, 04        
        DB   0F1H, 18H           ;07AC: LD    ST, V1        
        DB   30H, 90H            ;07AE: SE    V0, 90        
        DB   17H, 0A8H           ;07B0: JP    7A8           
        DB   86H, 0B0H           ;07B2: LD    V6, VB        
        DB   87H, 0C0H           ;07B4: LD    V7, VC        
        DB   80H, 0C0H           ;07B6: LD    V0, VC        
        DB   80H, 12H            ;07B8: AND   V0, V1        
        DB   30H, 00H            ;07BA: SE    V0, 00        
        DB   77H, 0FFH           ;07BC: ADD   V7, FF        
        DB   0D6H, 71H           ;07BE: DRW   V6, V7, 1     
        DB   47H, 00H            ;07C0: SNE   V7, 00        
        DB   16H, 70H            ;07C2: JP    670           
        DB   0D6H, 71H           ;07C4: DRW   V6, V7, 1     
        DB   87H, 0E5H           ;07C6: SUB   V7, VE        
        DB   37H, 02H            ;07C8: SE    V7, 02        
        DB   18H, 20H            ;07CA: JP    820           
        DB   0A8H, 31H           ;07CC: LD    I, 831        
        DB   0D6H, 75H           ;07CE: DRW   V6, V7, 5     
        DB   80H, 0D0H           ;07D0: LD    V0, VD        
        DB   70H, 03H            ;07D2: ADD   V0, 03        
        DB   80H, 65H            ;07D4: SUB   V0, V6        
        DB   3FH, 01H            ;07D6: SE    VF, 01        
        DB   18H, 18H            ;07D8: JP    818           
        DB   6EH, 06H            ;07DA: LD    VE, 06        
        DB   8EH, 05H            ;07DC: SUB   VE, V0        
        DB   6EH, 02H            ;07DE: LD    VE, 02        
        DB   4FH, 00H            ;07E0: SNE   VF, 00        
        DB   18H, 18H            ;07E2: JP    818           
        DB   6EH, 00H            ;07E4: LD    VE, 00        
        DB   60H, 30H            ;07E6: LD    V0, 30        
        DB   0F0H, 00H           ;07E8: LD    P, V0         
        DB   70H, 0FEH           ;07EA: ADD   V0, FE        
        DB   0F1H, 18H           ;07EC: LD    ST, V1        
        DB   30H, 10H            ;07EE: SE    V0, 10        
        DB   17H, 0E8H           ;07F0: JP    7E8           
        DB   7EH, 01H            ;07F2: ADD   VE, 01        
        DB   3EH, 04H            ;07F4: SE    VE, 04        
        DB   17H, 0E6H           ;07F6: JP    7E6           
        DB   6EH, 02H            ;07F8: LD    VE, 02        
        DB   0A8H, 2CH           ;07FA: LD    I, 82C        
        DB   0DDH, 0E2H          ;07FC: DRW   VD, VE, 2     
        DB   82H, 0D0H           ;07FE: LD    V2, VD        
        DB   6DH, 00H            ;0800: LD    VD, 00        
        DB   0DDH, 0E2H          ;0802: DRW   VD, VE, 2     
        DB   60H, 01H            ;0804: LD    V0, 01        
        DB   80H, 52H            ;0806: AND   V0, V5        
        DB   30H, 00H            ;0808: SE    V0, 00        
        DB   18H, 16H            ;080A: JP    816           
        DB   0A7H, 10H           ;080C: LD    I, 710        
        DB   0D0H, 82H           ;080E: DRW   V0, V8, 2     
        DB   38H, 1FH            ;0810: SE    V8, 1F        
        DB   78H, 0FFH           ;0812: ADD   V8, FF        
        DB   0D0H, 82H           ;0814: DRW   V0, V8, 2     
        DB   75H, 05H            ;0816: ADD   V5, 05        
        DB   0A8H, 31H           ;0818: LD    I, 831        
        DB   0D6H, 75H           ;081A: DRW   V6, V7, 5     
        DB   67H, 00H            ;081C: LD    V7, 00        
        DB   16H, 70H            ;081E: JP    670           
        DB   0D6H, 71H           ;0820: DRW   V6, V7, 1     
        DB   16H, 70H            ;0822: JP    670           
        DB   84H, 0E5H           ;0824: SUB   V4, VE        
        DB   0A8H, 31H           ;0826: LD    I, 831        
        DB   0D3H, 45H           ;0828: DRW   V3, V4, 5     
        DB   16H, 0E4H           ;082A: JP    6E4           
        DB   80H                 ;082C: 80
        DB   0F0H                ;082D: F0
        DB   08H                 ;082E: 08
        DB   0FEH                ;082F: FE
        DB   0FFH                ;0830: FF
        DB   50H                 ;0831: 50
        DB   0A8H                ;0832: A8
        DB   50H                 ;0833: 50
        DB   0A8H                ;0834: A8
        DB   50H                 ;0835: 50
        DB   0AAH                ;0836: AA
        DB   0C0H                ;0837: C0
        DB   00H                 ;0838: 00
        DB   00H                 ;0839: 00
        DB   00H                 ;083A: 00
        DB   00H                 ;083B: 00
        DB   00H                 ;083C: 00
        DB   00H                 ;083D: 00
        DB   00H                 ;083E: 00
        DB   00H                 ;083F: 00
        DB   00H                 ;0840: 00
        DB   00H                 ;0841: 00
        DB   00H                 ;0842: 00
        DB   00H                 ;0843: 00
        DB   00H                 ;0844: 00
        DB   00H                 ;0845: 00
        DB   00H                 ;0846: 00
        DB   00H                 ;0847: 00
        DB   00H                 ;0848: 00
        DB   00H                 ;0849: 00
        DB   00H                 ;084A: 00
        DB   00H                 ;084B: 00
        DB   00H                 ;084C: 00
        DB   00H                 ;084D: 00
        DB   00H                 ;084E: 00
        DB   00H                 ;084F: 00
        DB   00H                 ;0850: 00
        DB   00H                 ;0851: 00
        DB   00H                 ;0852: 00
        DB   00H                 ;0853: 00
        DB   00H                 ;0854: 00
        DB   00H                 ;0855: 00
        DB   00H                 ;0856: 00
        DB   00H                 ;0857: 00
        DB   00H                 ;0858: 00
        DB   00H                 ;0859: 00
        DB   00H                 ;085A: 00
        DB   00H                 ;085B: 00
        DB   00H                 ;085C: 00
        DB   00H                 ;085D: 00
        DB   00H                 ;085E: 00
        DB   00H                 ;085F: 00
        DB   00H                 ;0860: 00
        DB   00H                 ;0861: 00
        DB   00H                 ;0862: 00
        DB   00H                 ;0863: 00
        DB   00H                 ;0864: 00
        DB   00H                 ;0865: 00
        DB   00H                 ;0866: 00
        DB   00H                 ;0867: 00
        DB   00H                 ;0868: 00
        DB   00H                 ;0869: 00
        DB   00H                 ;086A: 00
        DB   00H                 ;086B: 00
        DB   00H                 ;086C: 00
        DB   00H                 ;086D: 00
        DB   00H                 ;086E: 00
        DB   00H                 ;086F: 00
        DB   00H                 ;0870: 00
        DB   00H                 ;0871: 00
        DB   00H                 ;0872: 00
        DB   00H                 ;0873: 00
        DB   00H                 ;0874: 00
        DB   00H                 ;0875: 00
        DB   00H                 ;0876: 00
        DB   00H                 ;0877: 00
        DB   00H                 ;0878: 00
        DB   00H                 ;0879: 00
        DB   00H                 ;087A: 00
        DB   00H                 ;087B: 00
        DB   00H                 ;087C: 00
        DB   00H                 ;087D: 00
        DB   00H                 ;087E: 00
        DB   00H                 ;087F: 00
        DB   00H                 ;0880: 00
        DB   00H                 ;0881: 00
        DB   00H                 ;0882: 00
        DB   00H                 ;0883: 00
        DB   00H                 ;0884: 00
        DB   00H                 ;0885: 00
        DB   00H                 ;0886: 00
        DB   00H                 ;0887: 00
        DB   00H                 ;0888: 00
        DB   00H                 ;0889: 00
        DB   00H                 ;088A: 00
        DB   00H                 ;088B: 00
        DB   00H                 ;088C: 00
        DB   00H                 ;088D: 00
        DB   00H                 ;088E: 00
        DB   00H                 ;088F: 00
        DB   0A8H, 8BH           ;0890: LD    I, 88B        
        DB   0F4H, 55H           ;0892: LD    [I], V4       
        DB   8EH, 10H            ;0894: LD    VE, V1        
        DB   8DH, 30H            ;0896: LD    VD, V3        
        DB   81H, 0E0H           ;0898: LD    V1, VE        
        DB   28H, 0B8H           ;089A: CALL  8B8           
        DB   7DH, 0FFH           ;089C: ADD   VD, FF        
        DB   71H, 01H            ;089E: ADD   V1, 01        
        DB   3DH, 00H            ;08A0: SE    VD, 00        
        DB   18H, 9AH            ;08A2: JP    89A           
        DB   74H, 0FFH           ;08A4: ADD   V4, FF        
        DB   72H, 01H            ;08A6: ADD   V2, 01        
        DB   34H, 00H            ;08A8: SE    V4, 00        
        DB   18H, 96H            ;08AA: JP    896           
        DB   0A8H, 8BH           ;08AC: LD    I, 88B        
        DB   0F4H, 65H           ;08AE: LD    V4, [I]       
        DB   00H, 0EEH           ;08B0: RETURN              
S08B2
        SEX  R9                  ;08B2: E9          
        OUT  1                   ;08B3: 61          
        SEP  R4                  ;08B4: D4          
        DB   00H                 ;08B5: 00
        DB   00H                 ;08B6: 00
        DB   00H                 ;08B7: 00
        DB   0A8H, 0B5H          ;08B8: LD    I, 8B5        
        DB   0F2H, 55H           ;08BA: LD    [I], V2       
        DB   08H, 0D1H           ;08BC: SYS   8D1           
        DB   00H, 0EEH           ;08BE: RETURN              
        DB   00H                 ;08C0: 00
S08C1
        LDI  39H                 ;08C1: F8 39       
        PLO  RF                  ;08C3: AF          
        GHI  R6                  ;08C4: 96          
        PHI  RF                  ;08C5: BF          
        SEX  RF                  ;08C6: EF          
        LDI  2CH                 ;08C7: F8 2C       
        STR  RF                  ;08C9: 5F          
        OUT  2                   ;08CA: 62          
        DEC  RF                  ;08CB: 2F          
        LDI  20H                 ;08CC: F8 20       
        STR  RF                  ;08CE: 5F          
        OUT  2                   ;08CF: 62          
        SEP  R4                  ;08D0: D4          
S08D1
        LDI  08H                 ;08D1: F8 08       
        PHI  RE                  ;08D3: BE          
        LDI  0B5H                ;08D4: F8 B5       
        PLO  RE                  ;08D6: AE          
        SEX  RE                  ;08D7: EE          
        LDXA                     ;08D8: 72          
        ANI  07H                 ;08D9: FA 07       
        PHI  RF                  ;08DB: BF          
        LDX                      ;08DC: F0          
        ANI  07H                 ;08DD: FA 07       
        STR  RE                  ;08DF: 5E          
        INC  RE                  ;08E0: 1E          
        LDX                      ;08E1: F0          
        ANI  1FH                 ;08E2: FA 1F       
        SHL                      ;08E4: FE          
        SHL                      ;08E5: FE          
        SHL                      ;08E6: FE          
        SHL                      ;08E7: FE          
        STR  RE                  ;08E8: 5E          
        LDI  0CH                 ;08E9: F8 0C       
        ADCI 00H                 ;08EB: 7C 00       
        PHI  RD                  ;08ED: BD          
        LDI  80H                 ;08EE: F8 80       
        ADD                      ;08F0: F4          
        PLO  RF                  ;08F1: AF          
        GHI  RD                  ;08F2: 9D          
        ADCI 00H                 ;08F3: 7C 00       
        PHI  RD                  ;08F5: BD          
        GLO  RF                  ;08F6: 8F          
        DEC  RE                  ;08F7: 2E          
        ADD                      ;08F8: F4          
        PLO  RD                  ;08F9: AD          
        SEX  RD                  ;08FA: ED          
        GHI  RF                  ;08FB: 9F          
        STR  RD                  ;08FC: 5D          
        OUT  3                   ;08FD: 63          
        SEX  R2                  ;08FE: E2          
        SEP  R4                  ;08FF: D4          
        END
