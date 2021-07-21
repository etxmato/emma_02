; *******************************************************************
; *** This software is copyright 2005 by Michael H Riley          ***
; *** You have permission to use, modify, copy, and distribute    ***
; *** this software so long as this copyright notice is retained. ***
; *** This software may not be used in commercial applications    ***
; *** without express written permission from the author.         ***
; *******************************************************************

include    bios.inc

; RA - text buffer pointer
; R8 - Reg1 (line number)
; R9 - Reg2 (count)

stack:     equ     07effh
ILRUN:     equ     08000h

; ***************************
; *** In memory data page ***
; ***************************
           org     02000h
lineaddr:  dw      0
linenum:   dw      0
curline:   dw      0
lines:     dw      0
char:      db      0
lastsym:   dw      0
pass:      db      0
startaddr: dw      0
baud:      db      0
compjumps: db      0
numbuf:    db      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

buffer:    ds      256
tokens:    ds      256

; text buffer format
; byte size of line (0 if end of buffer)
textbuf:   db      0

; ************************
; *** End of data page ***
; ************************

           org     08600h
           lbr     cold
           ldi     high restart        ; set restart address
           phi     r6
           ldi     low restart
           plo     r6
           ldi     high stack          ; set initial stack frame
           phi     r2
           ldi     low stack
           plo     r2
           sex     r2
           lbr     f_initcall

cold:      ldi     high start          ; get start address
           phi     r6
           ldi     low start
           plo     r6
           ldi     high stack          ; set initial stack frame
           phi     r2
           ldi     low stack
           plo     r2
           sex     r2
           lbr     f_initcall

; *** Check if character is uppercase alpha
; *** D - char to check
; *** Returns DF=1 if numeric
; ***         DF=0 if not
isuc:      sex     r2                  ; be sure x points to stack
           stxd                        ; save number
           smi     'A'                 ; check for bottom of numbers
           bnf     isnotuc             ; jump if not a number
           smi     27                  ; check high end
           bdf     isnotuc             ; jump if not a number
           ldi     1                   ; numer is numeric
           lskp
isnotuc:   ldi     0                   ; signal not a number
           shr                         ; shift result into DF
           irx                         ; recover original value
           ldx
           sep     sret                ; and return to caller

; *** Check if character is lowercase alpha
; *** D - char to check
; *** Returns DF=1 if numeric
; ***         DF=0 if not
islc:      sex     r2                  ; be sure x points to stack
           stxd                        ; save number
           smi     'a'                 ; check for bottom of numbers
           bnf     isnotlc             ; jump if not a number
           smi     27                  ; check high end
           bdf     isnotlc             ; jump if not a number
           ldi     1                   ; numer is numeric
           lskp
isnotlc:   ldi     0                   ; signal not a number
           shr                         ; shift result into DF
           irx                         ; recover original value
           ldx
           sep     sret                ; and return to caller

; **********************************
; *** check D if numeric         ***
; *** Returns DF=0 - numeric     ***
; ***         DF=1 - non-numeric ***
; **********************************
isnum:     plo     re                  ; save original D
           smi     '0'                 ; check for below zero
           lbnf    isnumno             ; jump if not
           smi     10                  ; see if above
           lbdf    isnumno             ; jump if so
           adi     0                   ; signal is numeric
           glo     re                  ; recover D
           sep     sret
isnumno:   smi     0                   ; signal nonnumber
           glo     re                  ; recover D
           sep     sret                ; and return


; *** rf - pointer to ascii string
; *** returns: rf - first non-numeric character
; ***          RD - number
; ***          DF = 1 if first character non-numeric
atoi:      ldi     0                   ; clear answer
           phi     rd
           plo     rd
           ldn     rf                  ; get first value
           sep     scall               ; check if numeric
           dw      isnum
           bnf     atoicnt             ; jump if so
           xri     '-'                 ; check for minus
           bz      atoicnt             ; jump if so
           ldi     1                   ; signal number error
           shr
           sep     sret                ; return to caller
atoicnt:   sex     r2                  ; make sure x points to stack
           glo     rc                  ; save consumed registers
           stxd
           ghi     rc
           stxd
           glo     re
           stxd
           plo     re                  ; signify positive number
           ldn     rf                  ; get first bytr
           xri     '-'                 ; check for negative
           bnz     atoilp              ; jump if not negative
           ldi     1                   ; signify negative number
           plo     re
           inc     rf                  ; move past - sign
atoilp:    ldn     rf                  ; get byte from input
           smi     '0'                 ; convert to binary
           bnf     atoidn              ; jump if below numbers
           smi     10                  ; check for above numbers
           bdf     atoidn              ; jump if above numbers
           glo     rd                  ; multiply by 2
           plo     rc                  ; keep a copy
           shl
           plo     rd
           ghi     rd
           phi     rc
           shlc
           phi     rd
           glo     rd                  ; multiply by 4
           shl
           plo     rd
           ghi     rd
           shlc
           phi     rd
           glo     rc                  ; multiply by 5
           str     r2
           glo     rd
           add
           plo     rd
           ghi     rc
           str     r2
           ghi     rd
           add
           phi     rd
           glo     rd                  ; multiply by 10
           shl
           plo     rd
           ghi     rd
           shlc
           phi     rd
           lda     rf                  ; get byte from buffer
           smi     '0'                 ; convert to binary
           str     r2                  ; prepare for addition
           glo     rd                  ; add in new digit
           add
           plo     rd
           ghi     rd
           adci    0
           phi     rd
           br      atoilp              ; loop back for next character
atoidn:    nop
           irx                         ; recover consumed registers
           ldxa
           plo     re
           ldxa
           phi     rc
           ldx
           plo     rc
           ldi     0                   ; signal valid number
           shr
           sep     sret                ; return to caller

start:
           sep     scall               ; set terminal baud rate
           dw      f_setbd
           ldi     high baud           ; point to baud storage
           phi     rf
           ldi     low baud
           plo     rf
           ghi     re                  ; save baud value
           str     rf
           ldi     high textbuf        ; point to text buffer
           phi     rf
           ldi     low textbuf
           plo     rf
           ldi     0                   ; need terminator
           str     rf
           lbr     new                 ; jump if file does not exist

restart:   ldi     high baud           ; point to baud storage
           phi     rf
           ldi     low baud
           plo     rf
           ldn     rf                  ; get baud value
           phi     re
           lbr     mainlp              ; back to editor

mainlp:    sep     scall               ; get current line number
           dw      getcurln
           inc     r8
           sep     scall               ; show current line
           dw      printnum
           ldi     high prompt         ; display the prompt
           phi     rf
           ldi     low prompt
           plo     rf
           sep     scall
           dw      f_msg
           ldi     high buffer         ; get input from user
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall
           dw      f_input
           sep     scall               ; do a cr/lf
           dw      docrlf
           ldi     0                   ; clear registers
           phi     r8
           plo     r8 
           phi     r9
           plo     r9
           inc     r9                  ; set count to 1
           sep     scall               ; set R8 to current line number
           dw      getcurln
           ldi     high buffer         ; point back to buffer
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; try to get a number
           dw      atoi
           lbdf    arg1non             ; jump if not numeric
           ghi     rd                  ; transfer to reg1
           phi     r8
           glo     rd
           plo     r8
           dec     r8                  ; origin to zero
arg1non:   ldn     rf                  ; get character in buffer
           xri     ','                 ; look for a comma
           lbnz    noarg2              ; jump if no second arg
           inc     rf                  ; point to next character
           sep     scall               ; and convert argument
           dw      atoi
           lbdf    noarg2              ; jump if no number supplied
           ghi     rd                  ; transfer argument to reg 2
           phi     r9
           glo     rd
           plo     r9
noarg2:    ldn     rf                  ; get command
           smi     'C'                 ; check for compile command
           lbz     doasm
           ldn     rf
           smi     'R'                 ; check for run command
           lbz     run
           ldn     rf
           smi     'D'                 ; check for down command
           lbz     down
           ldn     rf                  ; get command
           smi     'N'                 ; check for new command
           lbz     new
           ldn     rf                  ; get command
           smi     'U'                 ; check for up command
           lbz     up
           ldn     rf                  ; get command
           smi     'P'                 ; check for print command
           lbz     print
           ldn     rf                  ; get command
           smi     'I'                 ; check for insert command
           lbz     insert
           ldn     rf
           smi     'T'                 ; check for top command
           lbz     top
           ldn     rf                  ; get command
           smi     'B'                 ; check for bottom command
           lbz     bottom
           ldn     rf                  ; get command
           smi     'K'                 ; check for kill command
           lbz     kill
           ldn     rf                  ; get command
           smi     'G'                 ; check for go command
           lbz     go


           ldi     high errmsg         ; display error
           phi     rf
           ldi     low errmsg
           plo     rf
           sep     scall               ; display message
           dw      f_msg
           lbr     mainlp
  
new:       ldi     high textbuf        ; need to set terminator in text buffer
           phi     rf
           ldi     low textbuf
           plo     rf
           ldi     0
           str     rf
           ldi     high curline        ; point to current line
           phi     rf
           ldi     low curline
           plo     rf
           ldi     0                   ; set at beginning
           str     rf
           inc     rf
           str     rf
           lbr     newfile             ; then display as new

doasm:     sep     scall               ; invoke the compiler
           dw      bascom
           lbr     mainlp              ; then back to main loop

run:       sep     scall
           dw      ILRUN               ; call runtime
           lbr     mainlp              ; then back to main loop

      ldi     high startaddr      ; get start address of program
           phi     rf
           ldi     low startaddr
           plo     rf
           lda     rf                  ; set start address in R0
           phi     r0
           lda     rf
           plo     r0
           sep     r0                  ; transfer control to program
           lbr     mainlp              ; back to main

; ****************************
; *** Go to specified line ***
; *** R8 - line to go to   ***
; ****************************
go:        sep     scall               ; set line number
           dw      setcurln
           sep     scall               ; find line 
           dw      findline
           lbnf    gocont              ; jump if a valid line
           sep     scall               ; find end of buffer
           dw      findend
           sep     scall               ; set as current line
           dw      setcurln
gocont:    sep     scall               ; print the line
           dw      printit
           lbr     mainlp              ; return to main loop

; ***************************************
; *** Delete current line from buffer ***
; ***************************************
kill:      sep     scall               ; move to specified line
           dw      setcurln
killmnlp:  glo     r9                  ; check count
           lbnz    killgo
           ghi     r9
           lbnz    killgo
           sep     scall               ; display new line
           dw      printit
           lbr     mainlp              ; and back to main
killgo:    sep     scall               ; get current line number
           dw      getcurln
           sep     scall               ; get address for line
           dw      findline
           ldn     ra                  ; get length to next line
           adi     1
           str     r2                  ; prepare to add it
           plo     rc
           glo     ra                  ; add it to line address
           add
           plo     rd
           ghi     ra
           adci    0
           phi     rd
killlp:    glo     rc                  ; get count
           lbz     killnxt             ; jump if done copying line
           lda     rd                  ; get source byte
           str     ra                  ; place into destintion
           inc     ra
           dec     rc                  ; decrement count
           lbr     killlp              ; loop until line is done
killnxt:   dec     ra                  ; move back to length bytes
           dec     rd
           ldn     ra                  ; get length
           lbz     killdn              ; jump if at end of buffer
           adi     1                   ; prepare count
           plo     rc
           lbr     killlp              ; and loop for next line
killdn:    dec     r9                  ; decrement count
           lbr     killmnlp            ; and check for more

; ********************
; *** Return to OS ***
; ********************

; ********************************
; *** Move to bottom of buffer ***
; ********************************
bottom:    sep     scall               ; get last line number
           dw      findend
           lbr     topset              ; and set it

; *****************************
; *** Move to top of buffer ***
; *****************************
top:       ldi     0                   ; set line couunter to first line
           phi     r8
           plo     r8
topset:    sep     scall
           dw      setcurln
           sep     scall               ; display top line
           dw      printit
           lbr     mainlp              ; then back to main loop

; *******************************
; *** Insert text into buffer ***
; *******************************
insert:    inc     rf                  ; point to text to insert
           sep     scall               ; set current line
           dw      setcurln
           ldn     rf                  ; see if multi-line insert
           lbnz    insert1             ; only 1 line to insert
insertm:   sep     scall               ; get current line number
           dw      getcurln
           inc     r8
           sep     scall               ; show current line
           dw      printnum
           ldi     ':'                 ; print a colon
           sep     scall 
           dw      f_type
           ldi     high buffer         ; get input from user
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall
           dw      f_input
           shlc                        ; save return flag
           stxd
           sep     scall               ; do a cr/lf
           dw      docrlf
           irx                         ; recover return flag
           ldx
           shr
           lbdf    mainlp              ; back to main of <CTRL><C> pressed
           ldi     high buffer         ; point to input
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; insert the line
           dw      insertln
           lbr     insertm

insert1:   sep     scall               ; insert the line
           dw      insertln
           lbr     mainlp              ; back to main loop


insertln:  ldi     0                   ; setup count
           plo     rc
           phi     rc
           glo     rf                  ; save buffer position
           stxd
           ghi     rf
           stxd
insertlp1: inc     rc                  ; increment count
           lda     rf                  ; get next byte
           smi     32                  ; loof for anything less than a space
           lbdf    insertlp1
           glo     rc                  ; get count
           stxd                        ; and save it
           inc     rc
           inc     rc
           sep     scall               ; find end of buffer
           dw      findend
           glo     rc                  ; add in count to get destination
           str     r2
           glo     ra
           plo     r9
           add
           plo     rd
           ghi     rc
           str     r2
           ghi     ra
           phi     r9
           adc
           phi     rd
           ghi     rd
           adi     1
           phi     rd
           ghi     r9
           adi     1
           phi     r9
           sep     scall               ; get current line number
           dw      getcurln
           sep     scall               ; find address of line
           dw      findline
insertlp2: ldn     r9                  ; read source byte
           str     rd                  ; place into destination
           glo     ra                  ; check for completion
           str     r2
           glo     r9
           sm
           lbnz    inslp2c
           ghi     ra                  ; check for completion
           str     r2
           ghi     r9
           sm
           lbnz    inslp2c
           lbr     inslp2d
inslp2c:   dec     r9                  ; decrement positions
           dec     rd
           lbr     insertlp2
inslp2d:   sep     scall               ; get current line number
           dw      getcurln
           sep     scall               ; find address of line
           dw      findline
           irx                         ; recover count
           ldxa
           adi     1
           str     ra                  ; store into buffer
           smi     1
           inc     ra
           plo     rc                  ; put into count
           dec     rc                  ; subract out length byte
           ldxa                        ; recover input buffer
           phi     rf
           ldx
           plo     rf
insertlp3: glo     rc                  ; get count
           lbz     insertdn            ; jump if done
           lda     rf                  ; get byte from input
           str     ra                  ; store into text buffer
           inc     ra
           dec     rc                  ; decrement count
           lbr     insertlp3           ; loop back until done
insertdn:  ldi     13                  ; place in a cr/lf
           str     ra
           inc     ra
           ldi     10
           str     ra
           sep     scall               ; get current line number
           dw      getcurln
           inc     r8                  ; increment it
           sep     scall               ; and write it back
           dw      setcurln
           sep     sret                ; return to caller

; *******************************
; *** Print lines from buffer ***
; *******************************
print:     glo     r9                  ; check count
           lbnz    printgo             ; jump if more lines
           ghi     r9                  ; check high byte as well
           lbnz    printgo
           lbr     mainlp              ; done, so return to main loop
printgo:   sep     scall               ; print current line
           dw      printit
           inc     r8                  ; increment line number
           dec     r9                  ; decrement count
           lbr     print               ; loop back for more
          
; *****************************
; *** Move up in the buffer ***
; *****************************
up:        glo     r9                  ; check count
           lbnz    upgo1
           ghi     r9
           lbnz    upgo1
           sep     scall               ; print new line
           dw      printit
           lbr     mainlp
upgo1:     sep     scall               ; get current line number
           dw      getcurln
           glo     r8                  ; make sure it is not already 0
           lbnz    upgo                ; jump if good
           ghi     r8
           lbnz    upgo
           ldi     high topmsg         ; display top of buffer message
           phi     rf
           ldi     low topmsg
           plo     rf
           sep     scall               ; display message
           dw      f_msg
           lbr     mainlp
upgo:      dec     r8                  ; decrement line count
           sep     scall               ; write new line number
           dw      setcurln
           dec     r9                  ; decrement count
           lbr     up                  ; and loop back for more

; *******************************
; *** Move down in the buffer ***
; *******************************
down:      glo     r9                  ; check count
           lbnz    downgo
           ghi     r9
           lbnz    downgo
           sep     scall               ; print new line
           dw      printit
           lbr     mainlp
downgo:    sep     scall               ; get current line number
           dw      getcurln
           inc     r8                  ; add 1 to count
           sep     scall               ; see if it is valid
           dw      findline
           lbdf    eoberror            ; jump if it moves past end
           sep     scall               ; write new line number
           dw      setcurln
           dec     r9                  ; decrement count
           lbr     down                ; loop back for more

; *********************************
; *** Print specified line      ***
; *** R8 - Line number to print ***
; *********************************
printit:   sep     scall               ; set buffer position
           dw      findline
           lbnf    printitgo           ; jump if line exists
           sep     sret                ; otherwise just return
printitgo: inc     r8                  ; output origin is 1
           sep     scall               ; print the line number
           dw      printnum
           dec     r8                  ; reorigin to zero
           ldi     high colon          ; now the colon after the number
           phi     rf
           ldi     low colon
           plo     rf
           sep     scall
           dw      f_msg
           lda     ra                  ; get byte count
           plo     rc                  ; place into count register
           lbz     printend            ; jump if have last line of buffer
printlp:   glo     rc                  ; see if done
           lbz     printdn             ; jump if so
           lda     ra                  ; otherwise get byte
           sep     scall               ; and display it
           dw      f_type
           dec     rc                  ; decrement count
           lbr     printlp
printdn:   sep     sret                ; return to caller
printend:  sep     scall               ; print a final CR/LF
           dw      docrlf
           sep     sret                ; and return to caller

; *****************************************
; *** Get current line number           ***
; *** Returns: R8 - current line number ***
; *****************************************
getcurln:  ldi     high curline        ; point to current line
           phi     rf
           ldi     low curline
           plo     rf
           lda     rf                  ; get current line number
           phi     r8
           lda     rf
           plo     r8
           sep     sret                ; and return

; *******************************************
; *** Set current line to specified value ***
; *** R8 - Line number to set as current  ***
; *******************************************
setcurln:  glo     rf                  ; save consumed register
           stxd
           ghi     rf
           stxd
           ldi     high curline        ; point to current line
           phi     rf
           ldi     low curline
           plo     rf
           ghi     r8                  ; write new current line
           str     rf
           inc     rf
           glo     r8
           str     rf
           irx                         ; recover consumed register
           ldxa
           phi     rf
           ldx
           plo     rf
           sep     sret                ; and return
 
; ***********************************
; *** Display end of buffer error ***
; ***********************************
eoberror:  ldi     high endmsg         ; point to end of buffer message
           phi     rf
           ldi     low endmsg
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           lbr     mainlp              ; then back to main loop

; ********************************
; *** Display new file message ***
; ********************************
newfile:   ldi     high newmsg         ; display message indicating new file
           phi     rf
           ldi     low newmsg
           plo     rf
           sep     scall               ; display the message
           dw      f_msg
           lbr     mainlp              ; branch to main loop

; *************************************
; *** Find line in text buffer      ***
; *** R8 - line number              ***
; *** Returns: RA - pointer to line ***
; *************************************
findline:  ldi     high textbuf        ; point to text buffer
           phi     ra
           ldi     low textbuf
           plo     ra
           ghi     r8                  ; get line number
           phi     rc
           glo     r8
           plo     rc
findlp:    glo     rc                  ; see if count is zero
           lbz     found               ; jump if there
           lda     ra
           lbz     fnderr              ; jump if end of buffer was reached
           str     r2                  ; prepare for add
           glo     ra                  ; add to address
           add
           plo     ra
           ghi     ra
           adci    0
           phi     ra
           dec     rc                  ; decrement count
           lbr     findlp              ; and check line
found:     adi     0                   ; signal line found
           shr
           sep     sret                ; and return to caller
fnderr:    smi     0                   ; signal end of buffer reached
           sep     sret                ; return to caller

; *************************************
; *** Find end of buffer            ***
; *** Returns: R8 - Line number     ***
; ***          RA - pointer to line ***
; *************************************
findend:   ldi     high textbuf        ; get text buffer
           phi     ra
           ldi     low textbuf
           plo     ra
           ldi     0                   ; setup count
           phi     r8
           plo     r8
findendlp: lda     ra                  ; get count
           lbz     findenddn           ; jump if end was found
           str     r2
           glo     ra
           add
           plo     ra
           ghi     ra
           adci    0
           phi     ra
           inc     r8                  ; increment count
           lbr     findendlp
findenddn: dec     ra                  ; move back to count byte
           sep     sret                ; and return

; ****************************
; *** Print number         ***
; *** R8 - Number to print ***
; ****************************
printnum:  ghi     r8                  ; transfer number to RD
           phi     rd
           glo     r8
           plo     rd
           ldi     high numbuf         ; setup buffer
           phi     rf
           ldi     low numbuf
           plo     rf
           sep     scall               ; convert number
           dw      f_uintout
           ldi     0                   ; terminate the string
           str     rf
           ldi     high numbuf         ; point back to number
           phi     rf
           ldi     low numbuf
           plo     rf
           sep     scall               ; display the number
           dw      f_msg
           sep     sret                ; and return to caller


; *** Modify this routine to read from the editor ***
; *** Use RD as editor pointer                    ***
readln:     ldi     0                   ; set byte count
            phi     rc
            plo     rc
            ldn     rd                  ; get byte from edit buffer
            lbz     readeof             ; jump if at end
            smi     2                   ; prevent copy of CR LF
            plo     re                  ; write count byte
            inc     rd                  ; move past count byte
readlnlp:   glo     re                  ; see if done
            lbz     doneline            ; jump if end of line
            lda     rd                  ; read byte from edit buffer
            str     rf                  ; store into output buffer
            inc     rf
            inc     rc                  ; increment byte count
            dec     re                  ; decrement character count
            lbr     readlnlp            ; loop until done
doneline:   ldi     0                   ; write terminator
            str     rf
            inc     rd                  ; move past CR LF
            inc     rd
            adi     0                   ; signal valid read
            sep     sret                ; and return
readeof:    smi     0                   ; signal eof by setting DF
            str     rf                  ; place terminator into buffer
            sep     sret                ; and then return

; *****************************
; *** Now the compiler code ***
; *****************************


; *******************************************************************
; *** Basic compiler                                              ***
; *******************************************************************

; *** Register usage
; *** RA - Address pointer
; *** RB - Data base page
; *** RF - buffers

TKN_USTR:  equ     0fch
TKN_QSTR:  equ     0fdh
TKN_NUM:   equ     0feh
TKN_TERM:  equ     0ffh

ERR_DIRECT: equ    1
ERR_SYN:    equ    2
ERR_NOLN:   equ    3

OP_SX1:    equ     001h
OP_SX2:    equ     002h
OP_SX3:    equ     003h
OP_SX4:    equ     004h
OP_SX5:    equ     005h
OP_SX6:    equ     006h
OP_SX7:    equ     007h
OP_LB:     equ     009h
OP_LN:     equ     00ah
OP_DS:     equ     00bh
OP_SP:     equ     00ch
OP_PE:     equ     00eh
OP_PO:     equ     00fh
OP_IO:     equ     010h
OP_FG:     equ     011h
OP_FV:     equ     012h
OP_SV:     equ     013h
OP_DE:     equ     014h
OP_DO:     equ     015h
OP_PL:     equ     016h
OP_NE:     equ     017h
OP_AD:     equ     018h
OP_SU:     equ     019h
OP_MP:     equ     01ah
OP_DV:     equ     01bh
OP_CP:     equ     01ch
OP_TS:     equ     01dh
OP_TJ:     equ     01eh
OP_PN:     equ     020h
OP_PT:     equ     022h
OP_NL:     equ     023h
OP_PC:     equ     024h
OP_AN:     equ     025h
OP_OR:     equ     026h
OP_GL:     equ     027h
OP_XR:     equ     028h
OP_WS:     equ     02dh
OP_US:     equ     02eh
OP_RT:     equ     02fh
OP_JS:     equ     030h
OP_J:      equ     080h


bascom:    ldi     high pass           ; setup base page
           phi     rb
           ldi     low pass            ; point to pass variable
           plo     rb
           ldi     0                   ; indicate first pass
           str     rb
           ldi     low compjumps       ; indicate no computed jumps
           plo     rb
           ldi     0
           str     rb
           ldi     high textbuf        ; setup RD to point to edit buffer
           phi     rd
           ldi     low textbuf
           plo     rd
           sep     scall               ; find end of text buffer
           dw      findend
           inc     ra                  ; move past any count bytes
           inc     ra                  ; and buffer terminator
           ldi     low lines           ; need to set start address
           plo     rb
           ghi     ra
           str     rb
           inc     rb
           glo     ra
           str     rb
           ldi     low lineaddr        ; point to line addresses
           plo     rb
           ghi     ra                  ; set address prointer
           str     rb
           inc     rb
           glo     ra
           str     rb
           ldi     0                   ; setup address pointer
           phi     ra                  ; set address pointer to origin
           plo     ra
           sep     scall               ; invoke a compiler pass
           dw      compile
           lbdf    nogood              ; jump on compile error
; **************************************************
; *** Now re-open input file to process 2nd pass ***
; **************************************************
           ldi     0                   ; reset address
           phi     ra
           plo     ra
           ldi     low pass            ; point to pass variable
           plo     rb
           ldi     1                   ; set as second pass
           str     rb
           ldi     low compjumps       ; need to check for computed jumps
           plo     rb
           ldn     rb
           lbz     nocomps             ; jump of no computed jumps
           sep     scall               ; output the line address table
           dw      lineout
nocomps:   ldi     high textbuf        ; setup RD to point to edit buffer
           phi     rd
           ldi     low textbuf
           plo     rd
           sep     scall               ; invoke a compiler pass
           dw      compile
           lbdf    nogood              ; jump on compile error
nogood:    sep     sret                ; and return to caller

; **************************************
; *** This is the main compiler loop ***
; **************************************
compile:   ldi     high buffer         ; point to buffer
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; read in next line
           dw      readln
           glo     rc                  ; make sure bytes were read
           lbnz    compgo
           ghi     rc
           lbnz    compgo
           ldi     0ffh                ; put address of exit into address table
           phi     rf
           plo     rf
           sep     scall               ; add it
           dw      add_line
           ldi     OP_WS               ; write a final exit
           sep     scall
           dw      output
           adi     0                   ; indicate successful compile
           sep     sret                ; return to caller
           

compgo:    glo     rd                  ; save file descriptor
           stxd
           ghi     rd
           stxd
           ldi     low pass            ; need to dertermine pass
           plo     rb
           ldn     rb                  ; get pass
           lbz     pass1               ; jump if on first pass
           ldi     high buffer         ; back to beginning of buffer
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; display the line
           dw      f_msg
           sep     scall               ; and a carriage return
           dw      docrlf
pass1:     ldi     high buffer
           phi     rf
           ldi     low buffer
           plo     rf
           ldi     high tokens         ; point to token buffer
           phi     r9
           ldi     low tokens
           plo     r9
           sep     scall               ; tokenize the line
           dw      tokenize
           ldi     high tokens         ; point to token buffer
           phi     r9
           ldi     low tokens
           plo     r9
           sep     scall               ; process the tokens
           dw      process
           lbdf    failure             ; jump on failure
           irx                         ; recover source file descriptor
           ldxa
           phi     rd
           ldx
           plo     rd
           lbr     compile             ; and loop back for next line

failure:   ldi     high buffer         ; want to print errant line
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           sep     scall               ; now a crlf
           dw      docrlf
           ldi     high err_msg        ; point to error message
           phi     rf
           ldi     low err_msg
           plo     rf
           sep     scall               ; and display it
           dw      f_msg
           irx                         ; recover source file descriptor
           ldxa
           phi     rd
           ldx
           plo     rd
           smi     0                   ; indicate compile error
           sep     sret                ; and return

; **************************************
; *** Append inline text to a buffer ***
; *** RF - buffer to append to       ***
; **************************************
append:    lda     r6                  ; read next byte
           str     rf                  ; store into buffer
           inc     rf                  ; point to next character
           lbnz    append              ; loop back if more
           sep     sret                ; return to caller

docrlf:    ldi     high crlf
           phi     rf
           ldi     low crlf
           plo     rf
           sep     scall
           dw      f_msg
           sep     sret

; *************************************
; *** Tokenize a string             ***
; *** R9 - Buffer for tokens        ***
; *** RF - Ascii string to tokenize ***
; *** Returns: RC - token count     ***
; ***          DF=1 - error         ***
; *************************************
tokenize:  ldi     0                   ; set initial token count to zero
           phi     rc
           plo     rc
tokenlp:   sep     scall               ; move past any whitespace
           dw      f_ltrim
           ldn     rf                  ; get pointed at character
           lbz     tokendn             ; jump if terminator
           smi     34                  ; check for string
           lbz     charstr             ; jump if so
           sep     scall               ; see if numeric
           dw      f_idnum
           lbnf    tokennum            ; jump if numeric
           ldi     high functable      ; point to function table
           phi     r7
           ldi     low functable
           plo     r7
           sep     scall               ; check for token
           dw      f_findtkn
           lbdf    tokenfunc           ; jump if function

           ldn     rf                  ; get next character
           sep     scall               ; see if alnum
           dw      f_isalnum
           lbdf    ustrlpa             ; jump if it is
           lda     rf                  ; otherwise it is a single character
           str     r9
           inc     r9
           lbr     tokenlp             ; and keep looking for tokens

ustrlpa:   ldi     TKN_USTR            ; token for unquoted strings
ustrlp:    str     r9                  ; store into token stream
           inc     r9
           lda     rf                  ; get next byte
           sep     scall               ; see if alphanumeric
           dw      f_isalnum
           lbdf    ustrlp              ; loop back if so, store, and keep going
           dec     rf                  ; move back to non-string char
           lbr     strdn               ; terminate string 
charstr:   inc     rf                  ; move past opening quote
           ldi     TKN_QSTR            ; token for quoted string
charstrlp: str     r9                  ; store it
           inc     r9
           lda     rf                  ; get next byte of string
           plo     re                  ; save a copy
           lbz     strdn               ; jump if end
           smi     34                  ; check for ending quote
           lbz     strdn               ; also done
           glo     re                  ; recover byte
           lbr     charstrlp           ; and loop back til done
strdn:     ldi     TKN_TERM            ; terminate the string
           str     r9
           inc     r9
           lbr     tokenlp             ; and keep looking for tokens
tokennum:  lbz     numisdec            ; jump if decimal number
           sep     scall               ; convert hex number
           dw      f_hexin
           lbr     numcont             ; continue processing number
numisdec:  sep     scall               ; convert number
           dw      f_atoi
numcont:   ldi     TKN_NUM             ; get token for number
           str     r9                  ; place into token stream
           inc     r9 
           ghi     rd                  ; now write number
           str     r9
           inc     r9
           glo     rd                  ; and low byte
           str     r9
           inc     r9
           lbr     tokenlp             ; loop back for more tokens
tokenfunc: glo     rd                  ; get token number
           ori     080h                ; set high bit
           str     r9                  ; store into token stream
           inc     r9                  ; point to next free space
           lbr     tokenlp             ; and keep searching for tokens
tokendn:   ldi     0                   ; need to terminate token sequence
           str     r9
           adi     0                   ; signal no error
           sep     sret                ; and return to caller

; *********************************
; *** Find address for a line   ***
; *** RF - line number          ***
; *** Returns: R7 - entry       ***
; ***          DF=1 - not found ***
; *********************************
linentry:  ldi     low lines           ; need to get start of lines
           plo     rb
           lda     rb                  ; retrieve address
           phi     r7
           ldn     rb
           plo     r7
           ldi     low lineaddr        ; get end of table
           plo     rb
           lda     rb
           phi     r8                  ; place into r8
           lda     rb
           plo     r8
fndlnlp:   lda     r7                  ; get msb line byte from table
           str     r2                  ; setup for compare
           ghi     rf                  ; get msb of requested line
           sm                          ; check for a match
           lbnz    fndno               ; jump if not a match
           ldn     r7                  ; now to check lsb
           str     r2
           glo     rf                  ; lsb of requested line
           sm                          ; check for match
           lbnz    fndno               ; jump if not
           adi     0                   ; signal success
           sep     sret                ; return postion to caller
fndno:     inc     r7                  ; move to next entry
           inc     r7
           inc     r7
           glo     r8                  ; check against end of table
           str     r2
           glo     r7
           sm
           lbnz    fndlnlp             ; not at end, loop to check next entry
           ghi     r8                  ; check high byte
           str     r2
           ghi     r7
           sm
           lbnz    fndlnlp             ; jump if more entries to check
notfnd:    smi     0                   ; indicate line was not found
           sep     sret                ; and return


; *********************************
; *** Find address for a line   ***
; *** RF - line number          ***
; *** Returns: RF - Address     ***
; ***          DF=1 - not found ***
; *********************************
findlnadd: sep     scall               ; find entry for line
           dw      linentry
           lbdf    notfnd              ; jump if not found
           inc     r7                  ; found, so point to address
           lda     r7                  ; retrieve address
           phi     rf                  ; and put into RF
           lda     r7
           plo     rf
           adi     0                   ; indicate line was found
           sep     sret                ; return to caller

; **********************************
; *** Find address for next line ***
; *** RF - line number           ***
; *** Returns: RF - Address      ***
; ***          DF=1 - not found  ***
; **********************************
findnext:  sep     scall               ; find entry for line
           dw      linentry
           lbdf    notfnd              ; jump if not found
           inc     r7                  ; move to following entry
           inc     r7
           inc     r7
           inc     r7
           inc     r7                  ; found, so point to address
           lda     r7                  ; retrieve address
           phi     rf                  ; and put into RF
           lda     r7
           plo     rf
           adi     0                   ; indicate line was found
           sep     sret                ; return to caller

; ******************************
; *** Output a compiled byte ***
; *** D - byte to output     ***
; ******************************
output:    plo     re                  ; save byte to output
           ghi     ra                  ; get pointer
           adi     1                   ; il offset is 100h
           phi     ra
           glo     re                  ; recover byte
           str     ra                  ; write memory
           ghi     ra                  ; remove offset
           smi     1
           phi     ra
           inc     ra                  ; increment address
           plo     re                  ; save a copy
           sep     sret                ; return to caller on first pass

; *****************************************************
; *** Add line number and address to lineaddr table ***
; *** RF - line number                              ***
; *** RA - address                                  ***
; *****************************************************
add_line:  ldi     low pass            ; can only add during first pass
           plo     rb                  ; put into base page
           ldn     rb                  ; get current pass
           lbnz    add_no              ; do not allow on pass 1
           ldi     low lineaddr        ; point to next line address
           plo     rb                  ; put into base page
           lda     rb                  ; retrieve pointer
           phi     r7                  ; into r7
           ldn     rb
           plo     r7
           ghi     rf                  ; write line number to table
           str     r7
           inc     r7
           glo     rf
           str     r7
           inc     r7
           ghi     ra                  ; write address to table
           str     r7
           inc     r7
           glo     ra
           str     r7
           inc     r7
           glo     r7                  ; write new pointer back to data area
           str     rb
           dec     rb
           ghi     r7
           str     rb
add_no:    sep     sret                ; return

; **********************************
; *** Process a token stream     ***
; *** R9 - pointer to tokens     ***
; *** Returns: DF=1 on error     ***
; ***             D - error code ***
; **********************************
process:   lda     r9                  ; get first token from stream
           smi     TKN_NUM             ; must be a number (line number)
           lbz     process_1           ; jump if line number found
           ldi     ERR_DIRECT          ; indicate direct command error
           smi     0                   ; set DF
           sep     sret                ; and return
process_1: ldi     low linenum         ; point to current line number
           plo     rb
           lda     r9                  ; get msb of line number
           str     rb
           inc     rb
           ldn     r9                  ; get lsb of line number
           str     rb
           dec     r9                  ; move back to msb
           lda     r9                  ; get line number
           phi     rf
           lda     r9
           plo     rf
           sep     scall               ; and add into line address table
           dw      add_line
process_3: lda     r9                  ; get next token
           ani     07fh                ; strip high bit
           plo     re                  ; keep a copy of it
           smi     14                  ; check for PRINT
           lbz     c_print             ; jump if so
           lbnf    syn_err             ; jump on syntax error
           smi     1                   ; check for GOTO
           lbz     c_goto
           smi     1                   ; check for END
           lbz     c_end
           smi     1                   ; check for GOSUB
           lbz     c_gosub
           smi     1                   ; check for RETURN
           lbz     c_return
           smi     1                   ; check for LET
           lbz     c_let
           smi     1                   ; check for IF
           lbz     c_if
           smi     1                   ; check for INPUT
           lbz     c_input
           smi     1                   ; check for REM
           lbz     good                ; ignore this line
           smi     2                   ; check for POKE
           lbz     c_poke
           smi     1                   ; check for DPOKE
           lbz     c_dpoke
           smi     9                   ; check for OUT
           lbz     c_out
           smi     4                   ; check for PLOT
           lbz     c_plot

           dec     r9                  ; retrieve base token
           lda     r9
           smi     31+80h              ; see if USR
           lbz     c_usr               ; jump if so

           dec     r9                  ; move back to token
           lbr     c_let               ; treat as let
           lbr     syn_err             ; not found is syntax error

; *****************************
; *** Process PLOT statment ***
; *****************************
c_plot:    ldi     OP_PL               ; load graphic address
           sep     scall               ; output it
           dw      output
           sep     scall               ; get first argument
           dw      expr
           lbdf    syn_err             ; jump on expression error
           ldn     r9                  ; get next token
           lbz     plot_a1             ; if terminater then single argument
           smi     37+80h              ; also check for :
           lbz     plot_a1
           lda     r9                  ; need to verify a comma
           smi     13+80h              ; check for ,
           lbnz    syn_err             ; jump on error
           sep     scall               ; evaluate 2nd argument
           dw      expr
           lbdf    syn_err             ; jump if expression error
           ldn     r9                  ; get next token
           lbz     syn_err             ; cannot have only 2 args
           smi     37+80h              ; also check for :
           lbz     syn_err
           lda     r9                  ; need to verify a comma
           smi     13+80h              ; check for ,
           lbnz    syn_err             ; jump if not
           ldi     OP_SP               ; discard values for now
           sep     scall               ; output it
           dw      output
           ldi     OP_SP               ; discard values for now
           sep     scall               ; output it
           dw      output
           sep     scall               ; evaluate 3rd argument
           dw      expr
           lbdf    syn_err             ; jump if expression error
; *** 1 argument version
plot_a1:   ldi     OP_DS               ; duplicate top os stack
           sep     scall               ; output it
           dw      output
           ldi     OP_US               ; call USR function
           sep     scall               ; output it
           dw      output
           ldi     OP_SP               ; discard return value
           sep     scall               ; output it
           dw      output
           lbr     stmtend             ; good compile
 
          

; ****************************
; *** Process OUT statment ***
; ****************************
c_out:     ldi     OP_IO               ; get i/o vector
           sep     scall               ; and output it
           dw      output
           sep     scall               ; evaluate 
           dw      expr
           lbdf    syn_err             ; jump on error
           lda     r9                  ; get next token
           smi     13+80h              ; must be a comma
           lbnz    syn_err             ; jump if not 
           sep     scall               ; evaluate 
           dw      expr
           lbdf    syn_err             ; jump on error
           ldi     OP_US               ; now a USR call
           sep     scall               ; and output it
           dw      output
           ldi     OP_SP               ; pop off return value
           sep     scall               ; and output it
           dw      output
stmtend:   lda     r9                  ; get next token
           lbz     good                ; jump if at terminator
           smi     37+80h              ; check for :
           lbz     process_3           ; continue processing line if :
           lbr     syn_err             ; jump if not terminator

; ******************************
; *** Process DPOKE statment ***
; ******************************
c_dpoke:   ldi     OP_DO               ; need DO opcode
           sep     scall               ; output it
           dw      output
           sep     scall               ; get address
           dw      expr
           lbdf    syn_err             ; jump on error
           lda     r9                  ; get next symbol
           smi     13+80h              ; must be a comma
           lbnz    syn_err
           sep     scall               ; get value to poke
           dw      expr
           lbdf    syn_err             ; jump on expression error
           ldi     OP_US               ; usr function
           sep     scall               ; output it
           dw      output
           ldi     OP_SP               ; pop off result of USR
           sep     scall               ; output it
           dw      output
           lbr     stmtend             ; good compiler

; *****************************
; *** Process POKE statment ***
; *****************************
c_poke:    sep     scall               ; get address
           dw      expr
           lbdf    syn_err             ; jump on error
           lda     r9                  ; get next symbol
           smi     13+80h              ; must be a comma
           lbnz    syn_err
           sep     scall               ; get value to poke
           dw      expr
           lbdf    syn_err             ; jump on expression error
           ldi     OP_PO               ; poke function
           sep     scall               ; output it
           dw      output
           lbr     stmtend             ; good compiler

; ****************************
; *** Process USR statment ***
; ****************************
c_usr:     dec     r9                  ; prepare for later increment
           sep     scall               ; call f_usr code
           dw      f_usr
           lbdf    syn_err             ; jump if error
           ldi     OP_SP               ; discard result
           sep     scall               ; output it
           dw      output
           lbr     stmtend             ; good compiler

; ******************************
; *** Process INPUT statment ***
; ******************************
c_input:   ldn     r9                  ; get next token
           smi     TKN_QSTR            ; see if it is a quoted string
           lbnz    input_1             ; jump if not
           ldi     OP_PC               ; need to print the prompt
           sep     scall               ; output the code
           dw      output
           inc     r9                  ; point to first byte
input_l1:  lda     r9                  ; get next character
           plo     re                  ; save for a moment
           ldn     r9                  ; get following byte
           xri     0ffh                ; see if terminator
           lbz     input_2             ; jump if so
           glo     re                  ; recover character
           sep     scall               ; output it
           dw      output
           lbr     input_l1            ; loop back for more
input_2:   glo     re                  ; recover character
           ori     80h                 ; set high bit
           sep     scall               ; output it
           dw      output
           inc     r9                  ; move past terminatoer
input_c:   lda     r9                  ; get next token
           smi     13+80h              ; must be a comma
           lbnz    syn_err             ; otherwise syntax error
input_1:   lda     r9                  ; get next token
           smi     TKN_USTR            ; must be unquoted string (variable)
           lbnz    syn_err             ; jump if not
           ldn     r9                  ; get variable name
           plo     re                  ; keep a copy
           smi     'a'                 ; check for lowercase
           lbnf    input_3             ; jump if already uc
           ldn     r9                  ; recover variable name
           smi     32                  ; convert to uc
           plo     re                  ; and store
input_3:   glo     re                  ; get variable
           shl                         ; multiply by 2
           stxd                        ; save for a momen
           ldi     OP_LB               ; need to load a byte
           sep     scall               ; output it
           dw      output
           irx                         ; recover variable name
           ldx
           sep     scall               ; and output it
           dw      output
           ldi     OP_GL               ; function to get input
           sep     scall               ; output it
           dw      output
           ldi     OP_SV               ; save into variable
           sep     scall               ; output it
           dw      output
input_4:   lda     r9                  ; move past rest of variable name
           xri     0ffh                ; see if terminator
           lbnz    input_4             ; loop until found
           ldn     r9                  ; get next token
           lbz     stmtend             ; jump if terminator
           smi     37+80h              ; check for colon
           lbz     stmtend             ; jump if so
           lbr     input_c             ; jump if not terminator


; ***************************
; *** Process IF statment ***
; ***************************
c_if:      sep     scall               ; evaluate expression following IF
           dw      expr
           lbdf    syn_err             ; jump if evaluation error
           lda     r9                  ; get comparison
           smi     086h                ; check for =
           lbz     if_eq               ; jump if so
           smi     1                   ; check for <=
           lbz     if_le
           smi     1                   ; check for >=
           lbz     if_ge
           smi     1                   ; check for <>
           lbz     if_ne
           smi     1                   ; check for <
           lbz     if_lt
           smi     1                   ; check for >
           lbz     if_gt
           lbr     syn_err             ; invalid comparison
if_eq:     ldi     2                   ; setup comparison
           lskp
if_le:     ldi     3
           lskp
if_ge:     ldi     6
           lskp
if_ne:     ldi     5
           lskp
if_lt:     ldi     1
           lskp
if_gt:     ldi     4
           stxd                        ; save this
           ldi     OP_LB               ; need to load byte to stack
           sep     scall               ; output it
           dw      output
           irx                         ; recover mask
           ldx
           sep     scall               ; and output it
           dw      output
           sep     scall               ; evalute 2nd expression
           dw      expr
           lbdf    syn_err             ; jump on error
           ldi     OP_CP               ; need comparison operator
           sep     scall               ; output it
           dw      output
           sep     scall               ; get next line address
           dw      nxtlnadr
           ghi     rf                  ; get msb of next line address
           adi     080h                ; convert to J code
           sep     scall               ; and then output
           dw      output
           glo     rf                  ; get lsb of address
           sep     scall               ; and output
           dw      output
           ldn     r9                  ; get next token
           smi     23+80h              ; is it THEN
           lbnz    process_3           ; jump to process next token
           inc     r9                  ; move past THEN
           lbr     process_3           ; and keep processing

; ****************************
; *** Process LET statment ***
; ****************************
c_let:     lda     r9                  ; get token following LET
           smi     TKN_USTR            ; must be unquoted string (variable)
           lbnz    syn_err             ; otherwise syntax error
           ldn     r9                  ; get variable name
           plo     re                  ; keep a copy
           smi     'a'                 ; see if lowercase
           lbnf    let_2               ; jump if so
           ldn     r9                  ; get variable name
           smi     32                  ; convert to UC
           plo     re
let_2:     glo     re                  ; get variable
           stxd                        ; save it
           ldi     OP_LB               ; need to load byte to stack
           sep     scall               ; output it
           dw      output
           irx                         ; recover variable
           ldx
           shl                         ; multiply by 2
           sep     scall               ; and output it
           dw      output
let_lp:    lda     r9                  ; get byte from variable name
           xri     0ffh                ; see if terminator
           lbnz    let_lp              ; loop until terminator found
           lda     r9                  ; get next token
           smi     086h                ; must be =
           lbnz    syn_err             ; otherwise syntax error
           sep     scall               ; evaluate expression
           dw      expr
           lbdf    syn_err             ; jump if expression error
           ldi     OP_SV               ; function to set variable
           sep     scall               ; output it
           dw      output
           lbr     stmtend             ; good compiler

; *****************************
; *** Process END statement ***
; *****************************
c_end:     ldi     OP_WS               ; get opcode for end
           sep     scall               ; and output it
           dw      output
           lda     r9                  ; get next token
           lbnz    syn_err             ; syntax error if not terminator
good:      adi     0                   ; signal good
           sep     sret                ; and return

; ********************************
; *** Process RETURN statement ***
; ********************************
c_return:  ldi     OP_RT               ; get opcode for return
           sep     scall               ; and output it
           dw      output
           lda     r9                  ; get next token
           lbnz    syn_err             ; syntax error if not terminator
           lbr     good                ; return as good command

; *******************************
; *** Process GOSUB statement ***
; *******************************
c_gosub:   ldi     OP_JS               ; opcode for jump
           lbr     jumpcont            ; continue processing

; ******************************
; *** Process GOTO statement ***
; ******************************
c_goto:    ldi     OP_J                ; opcode for jump
jumpcont:  plo     r7                  ; save it
           lda     r9                  ; get next token
           smi     TKN_NUM             ; must be a number
           lbnz    c_cgoto             ; try computed jump
           inc     r9                  ; move to following token
           inc     r9
           ldn     r9                  ; retrieve it
           dec     r9                  ; move pointer back
           dec     r9
           lbz     c_goto_g            ; jump if line terminator
           smi     37+80h              ; check for colon
           lbz     c_goto_g            ; jump if so
           lbr     c_cgoto             ; treat as computed goto
;           lbnz    syn_err             ; syntax error if not
c_goto_g:  ldi     low pass            ; need to check pass
           plo     rb
           ldn     rb
           lbnz    c_goto_1            ; jump if second pass
           ldi     0                   ; just output 2 anything bytes
           sep     scall
           dw      output
           sep     scall
           dw      output
           inc     r9                  ; move past line number
           inc     r9
c_goto_dn: lbr     stmtend
c_goto_1:  lda     r9                  ; retrieve line number
           phi     rf
           lda     r9
           plo     rf
           glo     r7                  ; save opcode
           stxd
           sep     scall               ; retrieve line address
           dw      findlnadd
           irx                         ; recover opcode
           ldx
           plo     r7
           lbdf    line_err            ; jump if line was not found
           glo     r7                  ; get opcode offset
           str     r2                  ; prepare for add
           ghi     rf                  ; get line address
           add                         ; add in opcode offset
           sep     scall               ; and output
           dw      output
           glo     rf                  ; get low byte of address
           sep     scall               ; and output
           dw      output
           lbr     c_goto_dn           ; process ending
c_cgoto:   dec     r9                  ; move back one token
           sep     scall               ; compute line number
           dw      expr
           glo     r7                  ; get command
           smi     OP_J                ; was it J
           lbz     c_cgoto_j           ; jump if so
           ldi     OP_TS               ; opcode for table sub
c_cgotoct: sep     scall               ; output it
           dw      output
           ldi     low compjumps       ; need to signal computed jumps
           plo     rb
           ldi     1
           str     rb
           lbr     stmtend             ; check proper end
c_cgoto_j: ldi     OP_TJ               ; opcode for table jump
           lbr     c_cgotoct           ; continue


; *******************************
; *** Process PRINT statement ***
; *******************************
c_print:   lda     r9                  ; get next token
           lbz     prt_done            ; jump if terminator
           plo     re                  ; keep a copy
           smi     37+80h              ; also check for :
           lbz     prt_done
           glo     re
           smi     TKN_QSTR            ; is it a quoted string
           lbz     prt_qstr            ; jump if so
           glo     re                  ; check for semi-colon
           smi     12+80h
           lbz     prt_semi            ; jump if so
           glo     re                  ; check for comma
           smi     13+80h
           lbz     prt_comma           ; jump if so
           dec     r9                  ; move back a byte
           sep     scall               ; try evaluating as an expression
           dw      expr
           lbdf    syn_err             ; jump on error
           ldi     OP_PN               ; need to print the number
           sep     scall               ; output code
           dw      output
           lbr     c_print             ; loop back for more
prt_done:  ldi     OP_NL               ; output print new line code
           sep     scall
           dw      output
           dec     r9                  ; point back to termination
           lbr     stmtend             ; good compiler
prt_comma: ldi     OP_PT               ; output print tab code
           sep     scall
           dw      output
prt_semi:  ldn     r9                  ; see if next token is terminator
           lbnz    c_print             ; loop back for more if not
           adi     0                   ; indicate no error
           sep     sret                ; and return
prt_qstr:  ldi     OP_PC               ; opcode to print constant string
           sep     scall               ; output it
           dw      output
qstr_lp:   lda     r9                  ; get next byte
           plo     re                  ; keep for a moment
           ldn     r9                  ; get following byte
           xri     0ffh                ; see if string terminator
           lbz     qstrterm            ; jump if so
           glo     re                  ; retreive byte
           sep     scall               ; output it
           dw      output
           lbr     qstr_lp             ; loop back for more
qstrterm:  glo     re                  ; retrieve byte
           ori     080h                ; set high bit
           sep     scall               ; output it
           dw      output
           inc     r9                  ; move past string terminator
           lbr     c_print             ; and check for more printing

; **************
; *** Errors ***
; **************
line_err:  ldi     ERR_NOLN            ; indicate line does not exist
           lskp
syn_err:   ldi     ERR_SYN             ; indicate syntax error
           smi     0                   ; indicate error
           sep     sret                ; and return

; *********************************
; *** Find address of next line ***
; *********************************
nxtlnadr:  ldi     low pass            ; see what pass we are on
           plo     rb
           ldn     rb
           lbnz    nxtln_1             ; jump if pass 2
           ldi     0                   ; return zeros for pass 0
           phi     rf
           plo     rf
           sep     sret                ; and return to caller
nxtln_1:   ldi     low linenum         ; need current line number
           plo     rb                  ; put into base register
           lda     rb                  ; get current line number
           phi     rf                  ; and put into rf
           ldn     rb
           plo     rf
           sep     scall               ; get address of next line
           dw      findnext
           lbdf    syn_err             ; error if not found
           adi     0                   ; signal found
           sep     sret                ; and return

; **********************************
; *** Compile expressions        ***
; *** R9 - Pointer to expression ***
; *** Returns: DF=1 error        ***
; **********************************
expr:      ldn     r9                  ; get first character of epxressions
           smi     81h                 ; check for negative sign
           lbnz    expr_p              ; not negative, check for positive
           inc     r9                  ; move to next token
           sep     scall               ; call level 2 to get value
           dw      expr_l2
           lbdf    expr_err            ; exit out on error
           ldi     OP_NE               ; need negate opcode
           sep     scall               ; output it
           dw      output
           lbr     expr_lp             ; jump to main expression loop
expr_p:    ldn     r9                  ; get first character
           smi     80h                 ; check for positive
           lbr     expr_np             ; jump if no plus sign
           inc     r9                  ; ignore the plus sing
expr_np:   sep     scall               ; call level 2 to get value
           dw      expr_l2
           lbdf    expr_err            ; exit out on error
expr_lp:   ldn     r9                  ; get next token
           smi     26+80h              ; check for &
           lbnz    expr_1_1            ; jump if not
           ldi     OP_AN               ; operation is AND
expr_l1g:  stxd                        ; save operation code
           inc     r9                  ; move to next token
           sep     scall               ; get next value
           dw      expr_l2
           irx                         ; recover operation value
           ldx
           lbdf    expr_err            ; exit out on error
           sep     scall               ; and output it
           dw      output
           lbr     expr_lp             ; loop back for more terms
expr_1_1:  ldn     r9                  ; recover token
           smi     27+80h              ; check for |
           lbnz    expr_1_2            ; jump if not
           ldi     OP_OR               ; need OR operator
           lbr     expr_l1g            ; and process
expr_1_2:  ldn     r9                  ; recover token
           smi     28+80h              ; check for ^
           lbnz    expr_1_3            ; jump if not
           ldi     OP_XR               ; need XR operator
           lbr     expr_l1g            ; and process
expr_1_3:  adi     0                   ; signal no error
           sep     sret                ; end of expression, so return
; *************************************
; *** Level 2 of evaluator: + and - ***
; *************************************
expr_l2:   sep     scall               ; call level 3 for value
           dw      expr_l3
           lbdf    expr_err            ; exit out on error
expr_l2lp: ldn     r9                  ; get token
           smi     80h                 ; check for + token
           lbnz    expr_2_1            ; jump if not
           ldi     OP_AD               ; need add operation
expr_l2g:  stxd                        ; save operation code
           inc     r9                  ; point to next token
           sep     scall               ; and retrieve value
           dw      expr_l3
           irx                         ; recover operation code
           ldx
           lbdf    expr_err            ; exit out on error
           sep     scall               ; and output it
           dw      output
           lbr     expr_l2lp           ; loop back for more
expr_2_1:  ldn     r9                  ; recover token
           smi     81h                 ; check for - token
           lbnz    expr_2_2            ; jump if not
           ldi     OP_SU               ; need subtraction operator
           lbr     expr_l2g            ; process it
expr_2_2:  adi     0                   ; signal no error
           sep     sret                ; return to caller
; *************************************
; *** Level 3 of evaluator: * and / ***
; *************************************
expr_l3:   sep     scall               ; call level 4 for value
           dw      expr_l4
           lbdf    expr_err            ; exit out on error
expr_l3lp: ldn     r9                  ; get next token
           smi     082h                ; check for *
           lbnz    expr_3_1            ; jump if not
           ldi     OP_MP               ; need multiply operator
expr_l3g:  stxd                        ; save operation code
           inc     r9                  ; point to next token
           sep     scall               ; and retrieve value
           dw      expr_l4
           irx                         ; recover operation code
           ldx
           lbdf    expr_err            ; exit out on error
           sep     scall               ; and output it
           dw      output
           lbr     expr_l3lp           ; loop back for more
expr_3_1:  ldn     r9                  ; recover token
           smi     083h                ; check for /
           lbnz    expr_3_2            ; jump if not
           ldi     OP_DV               ; need divide operator
           lbr     expr_l3g            ; and process
expr_3_2:  adi     0                   ; signal no error
           sep     sret                ; return to caller
; **********************************************************************
; *** Level 4 of evaluator: numbers, variables, functions, sub-exprs ***
; **********************************************************************
expr_l4:   ldn     r9                  ; get token
           smi     TKN_NUM             ; is it a number
           lbnz    expr_4_1            ; jump if not
           inc     r9                  ; point to msb of number
           ldi     OP_LN               ; operator to load a number
           sep     scall               ; output it
           dw      output
           lda     r9                  ; get msb of number
           sep     scall               ; and output it
           dw      output
           lda     r9                  ; get lsb of number
           sep     scall               ; and output it
           dw      output
           adi     0                   ; signal no error
           sep     sret                ; return to caller
; ************************************
; *** Not a number, try a variable ***
; ************************************
expr_4_1:  ldn     r9                  ; get token
           smi     TKN_USTR            ; look for unquoted string (variable)
           lbnz    expr_4_2            ; jump if not
           inc     r9                  ; move to beginning of name
           ldn     r9                  ; retrieve varialbe name
           plo     re                  ; save a coyp
           smi     'a'                 ; see if lowercase
           lbnf    expr_4_1a           ; jump if already uppercase
           glo     re                  ; recover value
           smi     32                  ; convert to UC
           plo     re                  ; save it
expr_4_1a: glo     re                  ; get name
           shl                         ; multiply by 2
           stxd                        ; save for a moment
           ldi     OP_LB               ; signal load byte operator
           sep     scall               ; output it
           dw      output
           irx                         ; recover variable address
           ldx
           sep     scall               ; and output it
           dw      output
           ldi     OP_FV               ; then a fetch variable code
           sep     scall               ; output it
           dw      output
expr_4_1b: lda     r9                  ; get byte from string
           xri     0ffh                ; look for terminator
           lbnz    expr_4_1b           ; loop until terminator found
           adi     0                   ; signal no error
           sep     sret                ; then return
; ************************************
; *** Not a variable, try sub-expr ***
; ************************************
expr_4_2:  ldn     r9                  ; recover token
           smi     84h                 ; look for (
           lbnz    expr_4_3            ; jump if not
           inc     r9                  ; move past paren
           sep     scall               ; and call evaluator for sub-expr
           dw      expr
           ldn     r9                  ; get next token
           smi     085h                ; must be )
           lbnz    expr_err            ; jump if an error
           inc     r9                  ; move past ) symbol
           adi     0                   ; signal no error
           sep     sret                ; and return

; ***************************************
; *** Not a sub-expr, check functions ***
; ***************************************
expr_4_3:  ldn     r9                  ; get token
           smi     29+80h              ; check for PEEK
           lbz     f_peek              ; jump if so
           smi     1                   ; check for DPEEK
           lbz     f_dpeek             ; jump if so
           smi     1                   ; check for USR
           lbz     f_usr               ; jump if so
           smi     1                   ; check for FRE
           lbz     f_fre               ; jump if so
           smi     1                   ; check for RND
           lbz     f_rnd               ; jump if so
           smi     2                   ; check for INP
           lbz     f_inp               ; jump if so
           smi     1                   ; check for FLG
           lbz     f_flg               ; jump if so

expr_err:  smi     0                   ; signal an error
           sep     sret                ; and return

; ******************************
; *** Process FLG() function ***
; ******************************
f_flg:     inc     r9                  ; move past FLG token
           lda     r9                  ; get next token
           smi     084h                ; must be a (
           lbnz    expr_err            ; otherwise error
           ldi     OP_FG               ; setup io vector
           sep     scall               ; and output it
           dw      output
           sep     scall               ; evaluate expression for port
           dw      expr
           lbdf    expr_err            ; jump if expression error
           lda     r9                  ; get next token
           smi     085h                ; must be a )
           lbnz    expr_err            ; otherwise error
           ldi     OP_LN               ; need to put 8 onto stack
           sep     scall
           dw      output
           ldi     0
           sep     scall
           dw      output
           ldi     1
           sep     scall
           dw      output
           ldi     OP_SU               ; need to subtract the 1
           sep     scall
           dw      output
           ldi     OP_DS               ; duplicate top stack value
           sep     scall
           dw      output
           ldi     OP_US               ; perform USR call
           sep     scall
           dw      output
           adi     0                   ; signal success
           sep     sret                ; and return

; ******************************
; *** Process INP() function ***
; ******************************
f_inp:     inc     r9                  ; move past INP token
           lda     r9                  ; get next token
           smi     084h                ; must be a (
           lbnz    expr_err            ; otherwise error
           ldi     OP_IO               ; setup io vector
           sep     scall               ; and output it
           dw      output
           sep     scall               ; evaluate expression for port
           dw      expr
           lbdf    expr_err            ; jump if expression error
           lda     r9                  ; get next token
           smi     085h                ; must be a )
           lbnz    expr_err            ; otherwise error
           ldi     OP_LN               ; need to put 8 onto stack
           sep     scall
           dw      output
           ldi     0
           sep     scall
           dw      output
           ldi     8
           sep     scall
           dw      output
           ldi     OP_AD               ; add to port value
           sep     scall
           dw      output
           ldi     OP_DS               ; duplicate top stack value
           sep     scall
           dw      output
           ldi     OP_US               ; perform USR call
           sep     scall
           dw      output
           adi     0                   ; signal success
           sep     sret                ; and return

; ******************************
; *** Process FRE() function ***
; ******************************
f_rnd:     inc     r9                  ; move past RND token
           lda     r9                  ; get next token
           smi     084h                ; must be a (
           lbnz    expr_err            ; otherwise error
           ldi     high rtable1        ; point to code for RND
           phi     rf
           ldi     low rtable1
           plo     rf
rnd_1:     lda     rf                  ; load next value from table
           lbz     rnd_2               ; jump if end found
           sep     scall               ; otherwise output value
           dw      output
           lbr     rnd_1               ; loop until completed
rnd_2:     sep     scall               ; evaluate argument
           dw      expr
           lbdf    expr_err            ; jump if error
           lda     r9                  ; get next token
           smi     085h                ; must be a )
           lbnz    expr_err            ; otherwise error
           ldi     high rtable2        ; point to code for RND
           phi     rf
           ldi     low rtable2
           plo     rf
rnd_3:     lda     rf                  ; load next value from table
           xri     0ffh                ; check for end
           lbz     rnd_4               ; jump if end found
           xri     0ffh                ; restore value
           sep     scall               ; otherwise output value
           dw      output
           lbr     rnd_3               ; loop until completed
rnd_4:     adi     0                   ; signal good
           sep     sret                ; and return

; ******************************
; *** Process FRE() function ***
; ******************************
f_fre:     inc     r9                  ; move past FRE token
           lda     r9                  ; get next token
           smi     084h                ; must be a (
           lbnz    expr_err            ; otherwise error
           lda     r9                  ; get next token
           smi     085h                ; must be a )
           lbnz    expr_err            ; otherwise error
           ldi     OP_LB               ; output bytes to implement FRE
           sep     scall
           dw      output
           ldi     34
           sep     scall
           dw      output
           ldi     OP_FV
           sep     scall
           dw      output
           ldi     OP_LB
           sep     scall
           dw      output
           ldi     36
           sep     scall
           dw      output
           ldi     OP_FV
           sep     scall
           dw      output
           ldi     OP_SU
           sep     scall
           dw      output
           adi     0                   ; signal good
           sep     sret                ; and return
            
; ******************************
; *** Process USR() function ***
; ******************************
f_usr:     inc     r9                  ; move past USR token
           lda     r9                  ; get next token
           smi     084h                ; must be a (
           lbnz    expr_err            ; otherwise error
           sep     scall               ; evaluate expression (address)
           dw      expr
           lbdf    syn_err             ; jump if error
           ldn     r9                  ; get next token
           smi     85h                 ; check for )
           lbz     usr_n2              ; jump if so
           ldn     r9                  ; get next token
           smi     13+80h              ; check for a comma
           lbnz    syn_err             ; otherwise syntax error
           inc     r9                  ; move into expression
           sep     scall               ; evaluate expression (address)
           dw      expr
           lbdf    syn_err             ; jump if error
           ldn     r9                  ; get next token
           smi     85h                 ; check for )
           lbz     usr_n1              ; jump if so
           ldn     r9                  ; get next token
           smi     13+80h              ; check for a comma
           lbnz    syn_err             ; otherwise syntax error
           inc     r9                  ; move into expression
           sep     scall               ; evaluate expression (address)
           dw      expr
           lbdf    syn_err             ; jump if error
           lda     r9
           smi     085h                ; must be a )
           lbnz    expr_err
usr_go:    ldi     OP_US               ; need USR code
           sep     scall               ; output it
           dw      output
           adi     0                   ; signal good
           sep     sret                ; and return
usr_n2:    ldi     OP_DS               ; code to duplicate TOS
           sep     scall               ; output it
           dw      output
usr_n1:    ldi     OP_DS               ; code to duplicate TOS
           sep     scall               ; output it
           dw      output
           inc     r9                  ; move past )
           lbr     usr_go              ; and continue

; *******************************
; *** Process PEEK() function ***
; *******************************
f_peek:    inc     r9                  ; move past PEEK token
           lda     r9                  ; get next token
           smi     084h                ; must be a (
           lbnz    expr_err            ; otherwise error
           sep     scall               ; evaluate expression
           dw      expr
           lda     r9                  ; get next token
           smi     085h                ; must be a )
           lbnz    expr_err
           ldi     OP_PE               ; opcode for peek
           sep     scall               ; output it
           dw      output
           adi     0                   ; signal no error
           sep     sret                ; then return

; ********************************
; *** Process DPEEK() function ***
; ********************************
f_dpeek:   inc     r9                  ; move past PEEK token
           lda     r9                  ; get next token
           smi     084h                ; must be a (
           lbnz    expr_err            ; otherwise error
           ldi     OP_DE               ; need DE operator
           sep     scall               ; output it
           dw      output
           sep     scall               ; evaluate expression
           dw      expr
           lda     r9                  ; get next token
           smi     085h                ; must be a )
           lbnz    expr_err
           ldi     OP_DS               ; opcode for duplicate stack
           sep     scall               ; output it
           dw      output
           ldi     OP_US               ; opcode for USR call
           sep     scall               ; output it
           dw      output
           adi     0                   ; signal no error
           sep     sret                ; then return

; *********************************
; *** output line-address table ***
; *********************************
lineout:   ldi     low lines           ; get address of line table
           plo     rb
           lda     rb                  ; and retrieve it
           phi     r7
           ldn     rb
           plo     r7
           ldi     low lineaddr        ; get end of table
           plo     rb
           lda     rb
           phi     r8                  ; place into r8
           lda     rb
           plo     r8
lineoutlp: glo     r7                  ; check if at end of table
           str     r2
           glo     r8
           sm
           lbnz    lineoutgo           ; jump if not
           ghi     r7                  ; check high byte
           str     r2
           ghi     r8
           sm
           lbnz    lineoutgo           ; jump if not at end
           ldi     0                   ; write 2 zeroes
           sep     scall
           dw      output
           ldi     0                   ; write 2 zeroes
           sep     scall
           dw      output
           sep     sret                ; return to caller
lineoutgo: lda     r7                  ; copy byte from table to output
           sep     scall
           dw      output
           lbr     lineoutlp           ; loop until end of table

functable: db      ('+'+80h)           ; 0
           db      ('-'+80h)           ; 1
           db      ('*'+80h)           ; 2
           db      ('/'+80h)           ; 3
           db      ('('+80h)           ; 4
           db      (')'+80h)           ; 5
           db      ('='+80h)           ; 6
           db      '<',('='+80h)       ; 7
           db      '>',('='+80h)       ; 8
           db      '<',('>'+80h)       ; 9
           db      ('<'+80h)           ; 10
           db      ('>'+80h)           ; 11
           db      (';'+80h)           ; 12
           db      (','+80h)           ; 13
           db      'PRIN',('T'+80h)    ; 14
           db      'GOT',('O'+80h)     ; 15
           db      'EN',('D'+80h)      ; 26
           db      'GOSU',('B'+80h)    ; 17
           db      'RETUR',('N'+80h)   ; 18
           db      'LE',('T'+80h)      ; 19
           db      'I',('F'+80h)       ; 20
           db      'INPU',('T'+80h)    ; 21
           db      'RE',('M'+80h)      ; 22
           db      'THE',('N'+80h)     ; 23
           db      'POK',('E'+80h)     ; 24
           db      'DPOK',('E'+80h)    ; 25
           db      ('&'+80h)           ; 26
           db      ('|'+80h)           ; 27
           db      ('^'+80h)           ; 28
           db      'PEE',('K'+80h)     ; 29
           db      'DPEE',('K'+80h)    ; 30
           db      'US',('R'+80h)      ; 31
           db      'FR',('E'+80h)      ; 32
           db      'RN',('D'+80h)      ; 33
           db      'OU',('T'+80h)      ; 34
           db      'IN',('P'+80h)      ; 35
           db      'FL',('G'+80h)      ; 36
           db      (':'+80h)           ; 37
           db      'PLO',('T'+80h)     ; 38
           db      0

rtable1:   db      0ah,80h,80h,12h,0Ah,09h,29h,1Ah,0Ah
           db      1ah,85h,18h,13h,09h,80h,12h,1,0Bh,0   
rtable2:   db      0bh,4,2,3,5,3,1bh,1ah,19h
           db      0bh,09h,06h,0ah,0,0,1ch,17h,8,0ffh

err_msg:   db      'Error',10,13,0
endrom:    equ     $




; ********************
; *** Data portion ***
; ********************

fileerr:   db      'File Error',10,13,0
errmsg:    db      'Error',10,13,0
nxtline:   db      10,13,'     ',0

newmsg:    db      'New file'
crlf:      db      13,10,0
endmsg:    db      'End of buffer',10,13,0
topmsg:    db      'Top of buffer',10,13,0
prompt:    db      '>',0
colon:     db      ':',0
