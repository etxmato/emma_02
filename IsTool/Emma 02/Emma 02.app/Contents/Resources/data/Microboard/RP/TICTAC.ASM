.. 100  REM TIC-TAC-TOE, YOU (X) VS. THE COMPUTER (O)..
  ORG 0
  NOP;LBR RESET
  ORG 00100H
ES EQU 13
ZZ EQU ES
AC EQU 10
XX EQU 8
S EQU 11
L EQU 9
..
RESET  NOP;BR INIT;LBR SAVER
GETCH  LBR CHIN
PUTCH  LBR CHOUT
INIT  GHI 0;PHI 3;PHI 4;PHI 5
  LDI CALL;PLO 4;LDI RETN;PLO 5
  LDI MAIN;PLO 3;SEP 3;DC A.0(000H),00CFFH
  SEP 3
CALL  PLO RE;GHI 6;SEX 2;STXD;GLO 6;STXD
  GLO 3;PLO 6;GHI 3;PHI 6
  LDA 6;PHI 3;LDA 6;PLO 3;GLO RE;BR CALL-1
  SEP 3
RETN  PLO RE;SEX 2;GHI 6;PHI 3;GLO 6;PLO 3;INC 2
  LDA 2;PLO 6;LDN 2;PHI 6;GLO RE;BR RETN-1
SAVER  PLO AC;SEX 2;GLO S;STXD;GHI S;STXD
  GLO L;STXD;GHI L;STXD;GLO AC;ADI 0
  CALL RUN
  PLO AC;INC 2;LDA 2;PHI L;LDA 2;PLO L
  LDA 2;PHI S;LDN 2;PLO S;GLO AC;EXIT
MAIN  LDA 0;PHI ES;LDA 0;PHI 2;LDA 0;PLO 2
  LDI VARS;PLO ES;SMI 0
RUN  LDI STORE;PLO S;LDI A.1(STORE);PHI S
  LDI 0;PHI AC;PLO AC;LSNF;SEP S;DC A.0(TTYCC)
  LDI LINE;PHI AC;LSNF;SEP S;DC A.0(INPTR)
  LDI LOAD;PLO L;LDI A.1(LOAD);PHI L
..
TTYCC EQU 27
INPTR EQU 0002FH
CDENT EQU 1EH
UCALL EQU 0B453H
LINE EQU 00030H
VARS EQU 00080H
..
L100
..
.. 200  REM..
L200
..
.. 205  REM BASIC1 COMPILER VERSION 1..
L205
..
.. 206  REM USE START ADD=256,VAR=46,STACK = #0CFF..
L206
..
.. 207  REM DO NOT RUN IN INTERPRETER  ..
L207
..
.. 209  REM  ..
L209
..
.. 210  PRINT "TIC-TAC-TOE"..
L210
  CALL STRNG
  DC 'TIC-TAC-TOE'
  DC 9FH
  CALL CRLF
..
.. 220  PRINT "DO YOU NEED"..
L220
  CALL STRNG
  DC 'DO YOU NEED'
  DC 9FH
  CALL CRLF
..
.. 230  PRINT "INSTRUCTIONS"..
L230
  CALL STRNG
  DC 'INSTRUCTIONS'
  DC 9FH
  CALL CRLF
..
.. 240  A=0..
L240
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VA)
..
.. 250  B=0..
L250
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VB)
..
.. 260  C=0..
L260
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VC)
..
.. 270  D=0..
L270
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VD)
..
.. 280  E=0..
L280
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VE)
..
.. 290  F=0..
L290
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VF)
..
.. 293  REM BOARD ARRAY LOCATION..
L293
..
.. 295  H=#0C00..
L295
  LDI A.0(00CH);PHI AC
  GHI ZZ;  PLO AC
  SEP S;DC A.0(VH)
..
.. 300  Y=0..
L300
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VY)
..
.. 320  PRINT "(Y,N)";..
L320
  CALL STRNG
  DC '(Y,N)'
  DC 9FH
..
.. 330  INPUT I..
L330
  CALL GETLN
  CALL INPUT,A.0(VI)
..
.. 340  IF I<>Y GOTO 1000..
L340
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VY)
  CALL IF0T
  LBZ IF1
  LBR L1000
IF1
..
.. 350  PRINT..
L350
  CALL CRLF
..
.. 355  PRINT "PLEASE ANSWER"..
L355
  CALL STRNG
  DC 'PLEASE ANSWER'
  DC 9FH
  CALL CRLF
..
.. 360  PRINT "0 FOR YES,"..
L360
  CALL STRNG
  DC '0 FOR YES,'
  DC 9FH
  CALL CRLF
..
.. 365  PRINT "1 FOR NO."..
L365
  CALL STRNG
  DC '1 FOR NO.'
  DC 9FH
  CALL CRLF
..
.. 370  PRINT "UNDERSTAND";..
L370
  CALL STRNG
  DC 'UNDERSTAND'
  DC 9FH
..
.. 380  INPUT I..
L380
  CALL GETLN
  CALL INPUT,A.0(VI)
..
.. 390  IF I<>Y GOTO 350..
L390
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VY)
  CALL IF0T
  LBZ IF2
  LBR L350
IF2
..
.. 400  PRINT..
L400
  CALL CRLF
..
.. 410  PRINT "YOU PLAY X BY"..
L410
  CALL STRNG
  DC 'YOU PLAY X BY'
  DC 9FH
  CALL CRLF
..
.. 415  PRINT "TYPING A NUMBER"..
L415
  CALL STRNG
  DC 'TYPING A NUMBER'
  DC 9FH
  CALL CRLF
..
.. 420  PRINT "OF A SQUARE.  I"..
L420
  CALL STRNG
  DC 'OF A SQUARE.  I'
  DC 9FH
  CALL CRLF
..
.. 425  PRINT "PLAY O. TO START"..
L425
  CALL STRNG
  DC 'PLAY O. TO START'
  DC 9FH
  CALL CRLF
..
.. 430  PRINT "OVER, TYPE THE"..
L430
  CALL STRNG
  DC 'OVER, TYPE THE'
  DC 9FH
  CALL CRLF
..
.. 435  PRINT "NUMBER 0.  OK";..
L435
  CALL STRNG
  DC 'NUMBER 0.  OK'
  DC 9FH
..
.. 440  INPUT I..
L440
  CALL GETLN
  CALL INPUT,A.0(VI)
..
.. 450  IF I<>Y GOTO 220..
L450
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VY)
  CALL IF0T
  LBZ IF3
  LBR L220
IF3
..
.. 455  PRINT..
L455
  CALL CRLF
..
.. 460  PRINT "PLEASE BE PATIENT"..
L460
  CALL STRNG
  DC 'PLEASE BE PATIENT'
  DC 9FH
  CALL CRLF
..
.. 465  PRINT "WITH ME; I THINK"..
L465
  CALL STRNG
  DC 'WITH ME; I THINK'
  DC 9FH
  CALL CRLF
..
.. 470  PRINT "VERY SLOWLY."..
L470
  CALL STRNG
  DC 'VERY SLOWLY.'
  DC 9FH
  CALL CRLF
..
.. 495  PRINT..
L495
  CALL CRLF
..
.. 500  REM---ON WITH THE SHOW.....
L500
..
.. 1000 LET F=1..
L1000
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  SEP S;DC A.0(VF)
..
.. 1010 PRINT "  NEW GAME";..
L1010
  CALL STRNG
  DC '  NEW GAME'
  DC 9FH
..
.. 1020 INPUT I..
L1020
  CALL GETLN
  CALL INPUT,A.0(VI)
..
.. 1030 IF I<>Y THEN 6540..
L1030
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VY)
  CALL IF0T
  LBNZ L6540
..
.. 1100 LET I=H+1..
L1100
  SEP L;DC A.0(VH)
  INC AC
  SEP S;DC A.0(VI)
..
.. 1110 LET @I=0..
L1110
  SEP L;DC A.0(VI)
  GHI ZZ;STR AC
..
.. 1115 LET I=I+1..
L1115
  SEP L;DC A.0(VI)
  INC AC
  SEP S;DC A.0(VI)
..
.. 1120 IF I<H+10 GOTO 1110..
L1120
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 00AH;  PLO AC
  CALL ADD
  CALL IF1T
  LBDF IF4
  LBR L1110
IF4
..
.. 1130 LET U=9..
L1130
  GHI ZZ;PHI AC
  LDI 009H;  PLO AC
  SEP S;DC A.0(VU)
..
.. 1140 LET Z=0..
L1140
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VZ)
..
.. 1150 IF F=0 THEN GOTO 4010..
L1150
  SEP L;DC A.0(VF)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF5
  LBR L4010
IF5
..
.. 1160 GOTO 2010..
L1160
  LBR L2010
..
.. 1500 REM X WON...
L1500
..
.. 1510 LET Z=1..
L1510
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  SEP S;DC A.0(VZ)
..
.. 1520 LET F=0..
L1520
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VF)
..
.. 2000 REM PRINT GAME STATE..
L2000
..
.. 2010 REM PLOT 12..
L2010
..
.. 2015 PRINT..
L2015
  CALL CRLF
..
.. 2020 LET I=1..
L2020
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  SEP S;DC A.0(VI)
..
.. 2030 X=@(I+H)..
L2030
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  LDN AC;  PLO AC
  GHI ZZ;PHI AC
  SEP S;DC A.0(VX)
..
.. 2040 IF X=0 PRINT I;..
L2040
  SEP L;DC A.0(VX)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF6
  SEP L;DC A.0(VI)
  CALL PRN
IF6
..
.. 2050 IF X=1 PRINT "X";..
L2050
  SEP L;DC A.0(VX)
  GLO AC;SMI 001H;LSNZ
  GHI AC;GHI AC;  LBNZ IF7
  CALL STRNG
  DC 'X'
  DC 9FH
IF7
..
.. 2060 IF X=3 PRINT "O";..
L2060
  SEP L;DC A.0(VX)
  GLO AC;SMI 003H;LSNZ
  GHI AC;GHI AC;  LBNZ IF8
  CALL STRNG
  DC 'O'
  DC 9FH
IF8
..
.. 2070 IF I/3*3<>I PRINT " : ";..
L2070
  SEP L;DC A.0(VI)
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 003H;  PLO AC
  CALL DIV
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 003H;  PLO AC
  CALL MPY
  CALL PUSH
  SEP L;DC A.0(VI)
  CALL IF0T
  LBZ IF9
  CALL STRNG
  DC ' : '
  DC 9FH
IF9
..
.. 2080 IF I<>3 GOTO 2110..
L2080
  SEP L;DC A.0(VI)
  GLO AC;SMI 003H;LSNZ
  GHI AC;GHI AC;  LBZ IF10
  LBR L2110
IF10
..
.. 2090 PRINT ..
L2090
  CALL CRLF
..
.. 2100 PRINT "---------"..
L2100
  CALL STRNG
  DC '---------'
  DC 9FH
  CALL CRLF
..
.. 2110 IF I<>6 GOTO 2140 ..
L2110
  SEP L;DC A.0(VI)
  GLO AC;SMI 006H;LSNZ
  GHI AC;GHI AC;  LBZ IF11
  LBR L2140
IF11
..
.. 2120 PRINT..
L2120
  CALL CRLF
..
.. 2130 PRINT "---------"..
L2130
  CALL STRNG
  DC '---------'
  DC 9FH
  CALL CRLF
..
.. 2140 I=I+1..
L2140
  SEP L;DC A.0(VI)
  INC AC
  SEP S;DC A.0(VI)
..
.. 2150 IF I<10 GOTO 2030..
L2150
  SEP L;DC A.0(VI)
  GLO AC;SMI 00AH;GHI AC;XRI 80H
  SMBI A.0(080H);  LBDF IF12
  LBR L2030
IF12
..
.. 2160 PRINT..
L2160
  CALL CRLF
..
.. 3000 IF Z=0 GOTO 3100..
L3000
  SEP L;DC A.0(VZ)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF13
  LBR L3100
IF13
..
.. 3010 REM THE GAME IS OVER...
L3010
..
.. 3020 IF F=1 GOTO 3050..
L3020
  SEP L;DC A.0(VF)
  GLO AC;SMI 001H;LSNZ
  GHI AC;GHI AC;  LBNZ IF14
  LBR L3050
IF14
..
.. 3030 PRINT "YOU WIN. MORE";..
L3030
  CALL STRNG
  DC 'YOU WIN. MORE'
  DC 9FH
..
.. 3040 GOTO 1020..
L3040
  LBR L1020
..
.. 3050 PRINT "I WIN.  AGAIN";..
L3050
  CALL STRNG
  DC 'I WIN.  AGAIN'
  DC 9FH
..
.. 3060 GOTO 1020..
L3060
  LBR L1020
..
.. 3100 IF U>0 GOTO 3210..
L3100
  SEP L;DC A.0(VU)
  GLO AC;SDI 000H;GHI AC;XRI 80H
  SDBI A.0(080H);  LBDF IF15
  LBR L3210
IF15
..
.. 3110 PRINT "CAT'S GAME";..
L3110
  CALL STRNG
  DC 'CAT''S GAME'
  DC 9FH
..
.. 3120 LET F=1-F..
L3120
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  CALL PUSH
  SEP L;DC A.0(VF)
  CALL SUB
  SEP S;DC A.0(VF)
..
.. 3130 GOTO 1020..
L3130
  LBR L1020
..
.. 3140 REM PLOT 41,0..
L3140
..
.. 3150 F=1-F..
L3150
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  CALL PUSH
  SEP L;DC A.0(VF)
  CALL SUB
  SEP S;DC A.0(VF)
..
.. 3160 GOTO 1010..
L3160
  LBR L1010
..
.. 3200 REM INPUT NEXT PLAY...
L3200
..
.. 3210 REM PLOT 5,35..
L3210
..
.. 3212 PRINT "YOUR ";..
L3212
  CALL STRNG
  DC 'YOUR '
  DC 9FH
..
.. 3214 REM PLOT 11,35..
L3214
..
.. 3216 PRINT "PLAY";..
L3216
  CALL STRNG
  DC 'PLAY'
  DC 9FH
..
.. 3220 INPUT I..
L3220
  CALL GETLN
  CALL INPUT,A.0(VI)
..
.. 3230 IF I>0 IF I<10 GOTO 3270..
L3230
  SEP L;DC A.0(VI)
  GLO AC;SDI 000H;GHI AC;XRI 80H
  SDBI A.0(080H);  LBDF IF16
  GLO AC;SMI 00AH;GHI AC;XRI 80H
  SMBI A.0(080H);  LBDF IF16
  LBR L3270
IF16
..
.. 3232 IF I=0 GOTO 3140..
L3232
  SEP L;DC A.0(VI)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF17
  LBR L3140
IF17
..
.. 3236 REM PLOT 12..
L3236
..
.. 3238 PRINT..
L3238
  CALL CRLF
..
.. 3240 PRINT "PLEASE"..
L3240
  CALL STRNG
  DC 'PLEASE'
  DC 9FH
  CALL CRLF
..
.. 3242 PRINT "PLAY A"..
L3242
  CALL STRNG
  DC 'PLAY A'
  DC 9FH
  CALL CRLF
..
.. 3244 PRINT "NUMBER"..
L3244
  CALL STRNG
  DC 'NUMBER'
  DC 9FH
  CALL CRLF
..
.. 3246 PRINT "1 - 9"..
L3246
  CALL STRNG
  DC '1 - 9'
  DC 9FH
  CALL CRLF
..
.. 3248 PRINT "FOR YOUR"..
L3248
  CALL STRNG
  DC 'FOR YOUR'
  DC 9FH
  CALL CRLF
..
.. 3250 PRINT "PLAY."..
L3250
  CALL STRNG
  DC 'PLAY.'
  DC 9FH
  CALL CRLF
..
.. 3255 IF I=0 GOTO 2000..
L3255
  SEP L;DC A.0(VI)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF18
  LBR L2000
IF18
..
.. 3260 GOTO 3210..
L3260
  LBR L3210
..
.. 3270 IF @(I+H)=0 GOTO 3310..
L3270
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  LDN AC;  PLO AC
  GHI ZZ;PHI AC
  GLO AC;  LBNZ IF19
  LBR L3310
IF19
..
.. 3272 REM PLOT 12..
L3272
..
.. 3274 PRINT..
L3274
  CALL CRLF
..
.. 3276 PRINT "SQUARE"..
L3276
  CALL STRNG
  DC 'SQUARE'
  DC 9FH
  CALL CRLF
..
.. 3278 PRINT "# ";I;" IS"..
L3278
  CALL STRNG
  DC '# '
  DC 9FH
  SEP L;DC A.0(VI)
  CALL PRN
  CALL STRNG
  DC ' IS'
  DC 9FH
  CALL CRLF
..
.. 3280 PRINT "ALREADY"..
L3280
  CALL STRNG
  DC 'ALREADY'
  DC 9FH
  CALL CRLF
..
.. 3282 PRINT "TAKEN."..
L3282
  CALL STRNG
  DC 'TAKEN.'
  DC 9FH
  CALL CRLF
..
.. 3290 GOTO 3210..
L3290
  LBR L3210
..
.. 3300 REM CHECK IF X WON...
L3300
..
.. 3310 LET @(I+H)=1..
L3310
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  LDI 001H;STR AC
..
.. 3315 LET U=U-1..
L3315
  SEP L;DC A.0(VU)
  DEC AC
  SEP S;DC A.0(VU)
..
.. 3320 LET W=6100..
L3320
  LDI A.0(017H);PHI AC
  LDI 0D4H;  PLO AC
  SEP S;DC A.0(VW)
..
.. 3330 IF W=6100 GOSUB 6100..
L3330
  SEP L;DC A.0(VW)
  GLO AC;SMI 0D4H;LBNZ $+6
  GHI AC;SMI A.0(017H);  LBNZ IF20
  CALL L6100
IF20
..
.. 3332 IF W=6200 GOSUB 6200..
L3332
  SEP L;DC A.0(VW)
  GLO AC;SMI 038H;LBNZ $+6
  GHI AC;SMI A.0(018H);  LBNZ IF21
  CALL L6200
IF21
..
.. 3334 IF W=6300 GOSUB 6300..
L3334
  SEP L;DC A.0(VW)
  GLO AC;SMI 09CH;LBNZ $+6
  GHI AC;SMI A.0(018H);  LBNZ IF22
  CALL L6300
IF22
..
.. 3336 IF W=6400 GOSUB 6400..
L3336
  SEP L;DC A.0(VW)
  GLO AC;LBNZ $+6
  GHI AC;SMI A.0(019H);  LBNZ IF23
  CALL L6400
IF23
..
.. 3340 IF J>0 IF L*M*N=1 GOTO 1510..
L3340
  SEP L;DC A.0(VJ)
  GLO AC;SDI 000H;GHI AC;XRI 80H
  SDBI A.0(080H);  LBDF IF24
  SEP L;DC A.0(VL)
  CALL PUSH
  SEP L;DC A.0(VM)
  CALL MPY
  CALL PUSH
  SEP L;DC A.0(VN)
  CALL MPY
  GLO AC;SMI 001H;LSNZ
  GHI AC;GHI AC;  LBNZ IF24
  LBR L1510
IF24
..
.. 3350 LET W=W+100..
L3350
  SEP L;DC A.0(VW)
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 064H;  PLO AC
  CALL ADD
  SEP S;DC A.0(VW)
..
.. 3360 IF W<6500 GOTO 3330..
L3360
  SEP L;DC A.0(VW)
  GLO AC;SMI 064H;GHI AC;XRI 80H
  SMBI A.0(099H);  LBDF IF25
  LBR L3330
IF25
..
.. 3400 REM CHECK IF CATS GAME..
L3400
..
.. 3410 IF U=0 GOTO 2010..
L3410
  SEP L;DC A.0(VU)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF26
  LBR L2010
IF26
..
.. 4000 REM FIND BEST "O" PLAY..
L4000
..
.. 4010 LET I=1..
L4010
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  SEP S;DC A.0(VI)
..
.. 4020 LET T=-1..
L4020
  LDI A.0(0FFH);PHI AC
  PLO AC
  SEP S;DC A.0(VT)
..
.. 4290 REM EVALUATE I'TH SQUARE..
L4290
..
.. 4300 LET S=0..
L4300
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VS)
..
.. 4310 IF @(I+H)>0 GOTO 4480..
L4310
  SEP L;DC A.0(VI)
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  LDN AC;  PLO AC
  GHI ZZ;PHI AC
  GLO AC;SDI 000H;  LBDF IF27
  LBR L4480
IF27
..
.. 4320 LET W=6100..
L4320
  LDI A.0(017H);PHI AC
  LDI 0D4H;  PLO AC
  SEP S;DC A.0(VW)
..
.. 4330 IF W=6100 GOSUB 6100..
L4330
  SEP L;DC A.0(VW)
  GLO AC;SMI 0D4H;LBNZ $+6
  GHI AC;SMI A.0(017H);  LBNZ IF28
  CALL L6100
IF28
..
.. 4332 IF W=6200 GOSUB 6200..
L4332
  SEP L;DC A.0(VW)
  GLO AC;SMI 038H;LBNZ $+6
  GHI AC;SMI A.0(018H);  LBNZ IF29
  CALL L6200
IF29
..
.. 4334 IF W=6300 GOSUB 6300..
L4334
  SEP L;DC A.0(VW)
  GLO AC;SMI 09CH;LBNZ $+6
  GHI AC;SMI A.0(018H);  LBNZ IF30
  CALL L6300
IF30
..
.. 4336 IF W=6400 GOSUB 6400..
L4336
  SEP L;DC A.0(VW)
  GLO AC;LBNZ $+6
  GHI AC;SMI A.0(019H);  LBNZ IF31
  CALL L6400
IF31
..
.. 4340 IF J=0 GOTO 4410..
L4340
  SEP L;DC A.0(VJ)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF32
  LBR L4410
IF32
..
.. 4350 LET J=L+M+N..
L4350
  SEP L;DC A.0(VL)
  CALL PUSH
  SEP L;DC A.0(VM)
  CALL ADD
  CALL PUSH
  SEP L;DC A.0(VN)
  CALL ADD
  SEP S;DC A.0(VJ)
..
.. 4360 IF J=4 THEN GOTO 4410..
L4360
  SEP L;DC A.0(VJ)
  GLO AC;SMI 004H;LSNZ
  GHI AC;GHI AC;  LBNZ IF33
  LBR L4410
IF33
..
.. 4370 IF J=2 THEN LET S=S+20..
L4370
  SEP L;DC A.0(VJ)
  GLO AC;SMI 002H;LSNZ
  GHI AC;GHI AC;  LBNZ IF34
  SEP L;DC A.0(VS)
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 014H;  PLO AC
  CALL ADD
  SEP S;DC A.0(VS)
IF34
..
.. 4380 IF J=6 THEN LET S=S+100..
L4380
  SEP L;DC A.0(VJ)
  GLO AC;SMI 006H;LSNZ
  GHI AC;GHI AC;  LBNZ IF35
  SEP L;DC A.0(VS)
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 064H;  PLO AC
  CALL ADD
  SEP S;DC A.0(VS)
IF35
..
.. 4390 IF J=0 THEN LET S=S+2..
L4390
  SEP L;DC A.0(VJ)
  GHI AC;LSNZ;GLO AC;  GLO AC;  LBNZ IF36
  SEP L;DC A.0(VS)
  INC AC;INC AC
  SEP S;DC A.0(VS)
IF36
..
.. 4400 LET S=S+J..
L4400
  SEP L;DC A.0(VS)
  CALL PUSH
  SEP L;DC A.0(VJ)
  CALL ADD
  SEP S;DC A.0(VS)
..
.. 4410 LET W=W+100..
L4410
  SEP L;DC A.0(VW)
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 064H;  PLO AC
  CALL ADD
  SEP S;DC A.0(VW)
..
.. 4420 IF W<6500 GOTO 4330..
L4420
  SEP L;DC A.0(VW)
  GLO AC;SMI 064H;GHI AC;XRI 80H
  SMBI A.0(099H);  LBDF IF37
  LBR L4330
IF37
..
.. 4430 IF S<T THEN GOTO 4470..
L4430
  SEP L;DC A.0(VS)
  CALL PUSH
  SEP L;DC A.0(VT)
  CALL IF1T
  LBDF IF38
  LBR L4470
IF38
..
.. 4440 LET T=S..
L4440
  SEP L;DC A.0(VS)
  SEP S;DC A.0(VT)
..
.. 4450 LET B=I..
L4450
  SEP L;DC A.0(VI)
  SEP S;DC A.0(VB)
..
.. 4460 REM SAY SOMETHING, SO IT WONT SEEM SO LONG...
L4460
..
.. 4470 REM PLOT 10,40,10..
L4470
..
.. 4480 LET I=I+1..
L4480
  SEP L;DC A.0(VI)
  INC AC
  SEP S;DC A.0(VI)
..
.. 4490 IF I<10 GOTO 4300..
L4490
  SEP L;DC A.0(VI)
  GLO AC;SMI 00AH;GHI AC;XRI 80H
  SMBI A.0(080H);  LBDF IF39
  LBR L4300
IF39
..
.. 4500 REM PLOT 12..
L4500
..
.. 4501 PRINT..
L4501
  CALL CRLF
..
.. 4502 REM PLOT 24,35..
L4502
..
.. 4504 PRINT "I PLAYED ";B..
L4504
  CALL STRNG
  DC 'I PLAYED '
  DC 9FH
  SEP L;DC A.0(VB)
  CALL PRN
  CALL CRLF
..
.. 4506 REM PLOT B+48..
L4506
..
.. 4510 PRINT..
L4510
  CALL CRLF
..
.. 4520 LET @(B+H)=3..
L4520
  SEP L;DC A.0(VB)
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  LDI 003H;STR AC
..
.. 4530 LET U=U-1..
L4530
  SEP L;DC A.0(VU)
  DEC AC
  SEP S;DC A.0(VU)
..
.. 4540 IF T<100 THEN GOTO 2020..
L4540
  SEP L;DC A.0(VT)
  GLO AC;SMI 064H;GHI AC;XRI 80H
  SMBI A.0(080H);  LBDF IF40
  LBR L2020
IF40
..
.. 4550 REM I WON I WON I WON..
L4550
..
.. 4560 F=1..
L4560
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  SEP S;DC A.0(VF)
..
.. 4570 Z=1..
L4570
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  SEP S;DC A.0(VZ)
..
.. 4580 GOTO 2010..
L4580
  LBR L2010
..
.. 6000 REM SUBROUTINE TO LOOK AT ONE ROW, COL, OR DIAG..
L6000
..
.. 6010 REM I IS THE POSITION OF REFERENCE..
L6010
..
.. 6020 REM L,M,N ARE RETURNED WITH CONTENTS OF THE THREE SQUARES..
L6020
..
.. 6030 REM ENTER AT 6100,6200,6300, OR 6400.....
L6030
..
.. 6040 REM  TO EXAMINE ROW,COLUMN,DOWN DIAGONAL OR UP DIAGONAL..
L6040
..
.. 6090 REM W=HORIZONTAL..
L6090
..
.. 6100 LET J=(I-1)/3*3+8..
L6100
  SEP L;DC A.0(VI)
  DEC AC
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 003H;  PLO AC
  CALL DIV
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 003H;  PLO AC
  CALL MPY
  INC AC;INC AC;INC AC;INC AC;INC AC;INC AC;INC AC;INC AC
  SEP S;DC A.0(VJ)
..
.. 6110 LET D=1..
L6110
  GHI ZZ;PHI AC
  LDI 001H;  PLO AC
  SEP S;DC A.0(VD)
..
.. 6120 GOTO 6500..
L6120
  LBR L6500
..
.. 6190 REM W=VERTICAL..
L6190
..
.. 6200 LET J=I-(I-1)/3*3+9..
L6200
  SEP L;DC A.0(VI)
  CALL PUSH
  DEC AC
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 003H;  PLO AC
  CALL DIV
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 003H;  PLO AC
  CALL MPY
  CALL SUB
  INC AC;INC AC;INC AC;INC AC;INC AC;INC AC;INC AC;INC AC;INC AC
  SEP S;DC A.0(VJ)
..
.. 6210 LET D=3..
L6210
  GHI ZZ;PHI AC
  LDI 003H;  PLO AC
  SEP S;DC A.0(VD)
..
.. 6220 GOTO 6500..
L6220
  LBR L6500
..
.. 6290 REM W=DOWN DIAGONAL..
L6290
..
.. 6300 IF I-1<>(I-1)/4*4 GOTO 6440..
L6300
  SEP L;DC A.0(VI)
  DEC AC
  CALL PUSH
  SEP L;DC A.0(VI)
  DEC AC
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 004H;  PLO AC
  CALL DIV
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 004H;  PLO AC
  CALL MPY
  CALL IF0T
  LBZ IF41
  LBR L6440
IF41
..
.. 6310 LET D=4..
L6310
  GHI ZZ;PHI AC
  LDI 004H;  PLO AC
  SEP S;DC A.0(VD)
..
.. 6320 REM BOTH DIAGONALS GO THRU CENTER..
L6320
..
.. 6330 LET J=11..
L6330
  GHI ZZ;PHI AC
  LDI 00BH;  PLO AC
  SEP S;DC A.0(VJ)
..
.. 6340 GOTO 6500..
L6340
  LBR L6500
..
.. 6390 REM W=UP DIAGONAL..
L6390
..
.. 6400 LET D=2..
L6400
  GHI ZZ;PHI AC
  LDI 002H;  PLO AC
  SEP S;DC A.0(VD)
..
.. 6410 IF I>1 IF I<9 IF I=I/2*2+1 GOTO 6330..
L6410
  SEP L;DC A.0(VI)
  GLO AC;SDI 001H;GHI AC;XRI 80H
  SDBI A.0(080H);  LBDF IF42
  GLO AC;SMI 009H;GHI AC;XRI 80H
  SMBI A.0(080H);  LBDF IF42
  CALL PUSH
  CALL PUSH
  GHI ZZ;PHI AC
  LDI 002H;  PLO AC
  CALL DIV
  GLO AC;SHL;PLO AC;GHI AC;SHLC;PHI AC
  INC AC
  CALL IF0T
  LBNZ IF42
  LBR L6330
IF42
..
.. 6430 REM A DIAGONAL DOES NOT GO THRU THIS SQUARE..
L6430
..
.. 6440 LET J=0..
L6440
  GHI ZZ;PHI AC
  PLO AC
  SEP S;DC A.0(VJ)
..
.. 6450 RETURN..
L6450
  EXIT
..
.. 6490 REM NOW WE KNOW CENTER OF THIS THREE AND +/- OFFSET..
L6490
..
.. 6500 LET L=@(J-D+H-6)..
L6500
  SEP L;DC A.0(VJ)
  CALL PUSH
  SEP L;DC A.0(VD)
  CALL SUB
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  DEC AC;DEC AC;DEC AC;DEC AC;DEC AC;DEC AC
  LDN AC;  PLO AC
  GHI ZZ;PHI AC
  SEP S;DC A.0(VL)
..
.. 6510 LET M=@(J+H-6)..
L6510
  SEP L;DC A.0(VJ)
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  DEC AC;DEC AC;DEC AC;DEC AC;DEC AC;DEC AC
  LDN AC;  PLO AC
  GHI ZZ;PHI AC
  SEP S;DC A.0(VM)
..
.. 6520 LET N=@(J+D+H-6)..
L6520
  SEP L;DC A.0(VJ)
  CALL PUSH
  SEP L;DC A.0(VD)
  CALL ADD
  CALL PUSH
  SEP L;DC A.0(VH)
  CALL ADD
  DEC AC;DEC AC;DEC AC;DEC AC;DEC AC;DEC AC
  LDN AC;  PLO AC
  GHI ZZ;PHI AC
  SEP S;DC A.0(VN)
..
.. 6530 RETURN..
L6530
  EXIT
..
.. 6540 END..
L6540
  CALL UCALL,A.0(CDENT)
..

..
..  BASIC1 SUBROUTINE LIBRARY VERSION 2.0 FOR ASM8 ON MCDOS
..
..  ABS FUNCTION
..
ABS      GHI AC;SHL;LSDF;EXIT;NOP
..
..  SUBTRACT AC FROM 0000
..
NEG      GLO AC;SDI 0;PLO AC
         GHI AC;SDBI 0;PHI AC;EXIT
..
..  ADD/SUB STACK TO AC
..
SUB      CALL NEG
ADD      SEX ES;GLO AC;ADD;PLO AC;INC ES
         GHI AC;ADC;PHI AC;INC ES;EXIT
..
..  MULTIPLY AC * ES STACK
..
MPY      LDI 16;PLO RF;LDI 0;PHI XX;PLO XX
         SEX ES
MPYL     GLO AC;SHR;LBNF MPYS
         GLO XX;ADD;PLO XX;INC ES
         GHI XX;ADC;DEC ES;SKP
MPYS     GHI XX;SHRC;PHI XX;GLO XX;SHRC;PLO XX
         GHI AC;SHRC;PHI AC;GLO AC;SHRC;PLO AC
         DEC RF;GLO RF;LBNZ MPYL
         INC ES;INC ES;EXIT
..
..  DIVIDE ES STACK/AC
..
DIV      GHI AC;STR 2;GLO AC;OR;ADI 0FFH
         INC ES;LDA ES;LBDF DOK;CALL UCALL,A.0(CDENT);EXIT
DOK      XOR;STXD;DEC ES;DEC ES;CALL ABS
         GHI AC;PHI XX;GLO AC;PLO XX;LDA ES
         PLO AC;LDN ES;PHI AC;CALL ABS
         SEX ES;LDI 0;STXD;STR ES
         LDI 17;PLO RF
DIVL     GLO XX;SD;STR 2;INC ES
         GHI XX;SDB;LBNF DIVS
         STXD;LDN 2;STR ES;SKP
DIVS     DEC ES;GLO AC;SHLC;PLO AC
         GHI AC;SHLC;PHI AC
         DEC RF;GLO RF;LBZ DIVX
         LDN ES;SHLC;STR ES;INC ES
         LDN ES;SHLC;STXD;LBR DIVL
DIVX     INC ES;INC ES;INC 2;LDN 2
MODX     SHL;LBDF NEG;EXIT
..
..  PRINT STRING
..
STRNL    XRI 09FH;CALL OUCH
STRNG    LDA 6;XRI 09FH;LBNZ STRNL;EXIT
..
..  PRINT END LINE (CRLF)
..
LEND     LDI 6;PHI RF
         DEC RF;GHI RF;LBNZ $-2
CRLF     CALL STRNG,0DH
         DC 0A9FH;LDN XX;ANI 0E0H;STR XX;EXIT
..
..  PRINT TAB
..
TAB      LDI 020H;CALL OUCH
         LDA XX;ANI 7;LBNZ TAB;EXIT
..
..  OUTPUT A CHARACTER
..
OUCH     PHI RF;LDI A.1(TTYCC);PHI XX;LDI TTYCC;PLO XX
         LDN XX;ADI 1;ANI 0EFH;STR XX;SHL
         GHI RF;LBNF PUTCH;EXIT
..
..  PRINT A NUMBER
..
PRN      GHI AC;SHL;LBNF PRP;LDI 02DH
         CALL OUCH;CALL NEG
PRP      LDI 0;STXD
PRPL     CALL PUSH
         LDI 0;PHI AC;LDI 10;PLO AC
         CALL DIV;DEC ES;DEC ES
         LDA ES;INC ES;ORI 030H;STXD
         GHI AC;STR 2;GLO AC;OR;LBNZ PRPL
PRNP     INC 2;LDN 2;LSNZ;EXIT;NOP
         CALL OUCH;LBR PRNP
..
..  INPUT NEW LINE
..
RUBOUT   DEC XX;GLO XX;.XOR.A.0(LINE)
         LBZ CANC;@XX->RF.1;XX.1->@-;XX.0->@-;RF.1
         CALL CHOUT;INC R2;@!->XX.0;@R2->XX.1;LBR INCH
GETLF    LDI 080H;LSKP
GETXF    LDI 0;STR 2;LDI TTYCC;PLO XX
         LDN XX;ANI 07FH;OR;STR XX
         SMI 0;LBR GETN
BACKS    XX.1->@-;XX.0->@-
         CALL STRNG;DC 2008H,9FH          ..ASCII SPACE,BS
         INC R2;@!->XX.0;@R2->XX.1
         DEC XX;GLO XX;SMI LINE;LBDF INCH
CANC     CALL LEND
GETLN    CALL STRNG,3F20H,919FH
         ADI 0
GETN     LDI A.1(INPTR);PHI XX;LDI INPTR;PLO XX
         LDI LINE;LSNF;LDA XX;SKP;STR XX;PLO XX
INCH     CALL GETCH;->@R2
         .XOR.7FH;LBZ RUBOUT              ..ASCII RUBOUT
         .XOR.(7FH.XOR.0AH);LBZ GETLF     ..ASCII LINE FEED
         .XOR.(0AH.XOR.13H);LBZ GETXF     ..ASCII DC3
         .XOR.(13H.XOR.3);LBZ CANC        ..ASCII CANCEL (CNTRL X)
         .XOR.(3.XOR.8);LBZ BACKS         ..ASCII BACKSPACE
         @R2;->@XX;.XOR.0DH               ..ASCII CARRIAGE RETURN
         LBZ LEND;INC XX;GLO XX;SMI A.0(VARS-8)
         LSNF;DEC XX;NOP;LBR INCH
..
..  INPUT NUMBER TO AC
..
         CALL GETLN
INPUT    SEP L;DC A.0(INPTR-1);PLO XX;LDI 0;PLO AC;PHI AC
         PLO RF;PHI RF;SKP;DEC RF
INPUL    LDA XX;LSZ;XRI 00DH;LBZ INPUT-3
         XRI 020H;LBZ INPUL-1
         XRI 001H;LBZ INPUL
         XRI 00FH;LBZ IHEX
         XRI 003H;LBZ INPUL
INPUN    XRI 020H;SMI 03AH;LBDF INPUX
         ADI 10;LBNF INPUX;STXD
         GLO XX;STXD;DEC ES;LDI 0;STR ES
         DEC ES;LDI 10;STR ES;CALL MPY
         LDI A.1(LINE);PHI XX;INC 2;LDA 2;PLO XX
         GLO AC;ADD;PLO AC;GHI AC;ADCI 0;PHI AC
INPUS    LDA XX;XRI 020H;LBZ INPUS
         LBR INPUN
IHEX     LDA XX;SMI 020H;LBZ IHEX;LSNF
         SMI 010H;LBNF INPUX;STR 2
         SMI 15;ORI 8;LSNF;STR 2;NOP
         LDI 4;PLO RF
IHEX4    GLO AC;SHL;PLO AC;GHI AC;SHLC;PHI AC
         DEC RF;GLO RF;LBNZ IHEX4
         GLO AC;OR;PLO AC;LBR IHEX
INPUX    DEC XX;GLO XX;STR 2;LDI INPTR;PLO XX
         LDN 2;STR XX;GHI RF;LBZ ISTOR
         CALL NEG
ISTOR    LDA 6;INC S;SEP S;EXIT
..
..  STORE VARIABLE FROM AC
..
         SEP 3
STORE    LDA 3;PLO XX;LDI A.1(VARS);PHI XX
         GHI AC;STR XX;INC XX;GLO AC;STR XX
         LBR STORE-1
..
..  LOAD VARIABLE TO AC
..
         SEP 3
LOAD     LDA 3;PLO XX;LDI A.1(VARS);PHI XX
         LDA XX;PHI AC;LDN XX;PLO AC;LBR LOAD-1
..
..  PUSH AC INTO STACK
..
PUSH     DEC ES;GHI AC;STR ES
         DEC ES;GLO AC;STR ES;EXIT
..
..  COMPARE AC TO ES STACK
..
IF0T     LDA ES; STR 2;GLO AC;SM;STXD
         LDA ES;XRI 080H;STR 2;GHI AC;XRI 080H
         SMB;INC 2;OR;EXIT
IF1T     LDA ES; STR 2;GLO AC;SD;STXD
         LDA ES;XRI 080H;STR 2;GHI AC;XRI 080H
         SDB;INC 2;OR;EXIT
..
..  I/O SELECTION
..
CHIN     EQU $
         LBR 0813EH
CHOUT    EQU $
         SEX 2;STXD
         CALL 81A5H;INC 2;LDN 2
         EXIT..
CEND
..
  ORG VARS+2
VA  DC 0000H
VB  DC 0000H
VC  DC 0000H
VD  DC 0000H
VE  DC 0000H
VF  DC 0000H
VG  DC 0000H
VH  DC 0000H
VI  DC 0000H
VJ  DC 0000H
VK  DC 0000H
VL  DC 0000H
VM  DC 0000H
VN  DC 0000H
VO  DC 0000H
VP  DC 0000H
VQ  DC 0000H
VR  DC 0000H
VS  DC 0000H
VT  DC 0000H
VU  DC 0000H
VV  DC 0000H
VW  DC 0000H
VX  DC 0000H
VY  DC 0000H
VZ  DC 0000H
..
LAST EQU CEND
..
  END
EH
CHOUT    EQU $
         SEX 2;STXD
         CALL 81A5H;INC 2;LDN 2
         EXIT..
CEND
..
  ORG VARS+2
VA  DC 0000H
VB  DC 0000H
VC  DC 0000H
VD  DC 0000H
VE  DC 0000H
VF  DC 0000H
VG  DC 0000H
VH  DC 0000H
VI  DC 0000H
VJ  DC 0000H
VK  DC 0000H
VL  DC 0000H
VM  DC 0000H
VN  DC 0000H
VO  DC 0000H
VP  DC 0000H
VQ  DC 0000H
VR  DC 0000H
VS  DC 0000H
VT  DC 0000H
VU  DC 0000H
VV  DC 0000H
VW  DC 0000H
VX  DC 0000H
VY  DC 0000H
VZ  DC 0000H
..
LAST EQU CEND
..
  E