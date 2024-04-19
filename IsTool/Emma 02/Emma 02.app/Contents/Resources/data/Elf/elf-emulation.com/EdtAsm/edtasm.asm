; *******************************************************************
; *** This software is copyright 2006 by Michael H Riley          ***
; *** You have permission to use, modify, copy, and distribute    ***
; *** this software so long as this copyright notice is retained. ***
; *** This software may not be used in commercial applications    ***
; *** without express written permission from the author.         ***
; *******************************************************************

include    bios.inc

; RA - text buffer pointer
; R8 - Reg1 (line number)
; R9 - Reg2 (count)

#ifdef ELF2K
           org     0d000h
           lbr     start
           lbr     restart
#else
           org     0c000h
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
#endif

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

docrlf:    ldi     high crlf           ; display a crlf
           phi     rf
           ldi     low crlf
           plo     rf
           sep     scall
           dw      f_msg
           sep     sret

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

#ifndef ELF2K
start:     sep     scall               ; set terminal baud rate
           dw      f_setbd
           ldi     high baud           ; point to baud storage
           phi     rf
           ldi     low baud
           plo     rf
           ghi     re                  ; save baud value
           str     rf
#else
start:     ldi     high stack          ; set initial stack frame
           phi     r2
           ldi     low stack
           plo     r2
#endif
           ldi     high hello          ; display startup message
           phi     rf
           ldi     low hello
           plo     rf
           sep     scall
           dw      f_msg
           ldi     high textbuf        ; point to text buffer
           phi     rf
           ldi     low textbuf
           plo     rf
           ldi     0                   ; need terminator
           str     rf
           lbr     newfile             ; jump if file does not exist

#ifndef ELF2K
restart:   ldi     high baud           ; point to baud storage
           phi     rf
           ldi     low baud
           plo     rf
           ldn     rf                  ; get baud value
           phi     re
           lbr     mainlp              ; back to editor
#else
restart:   ldi     high stack          ; set initial stack frame
           phi     r2
           ldi     low stack
           plo     r2
           lbr     mainlp              ; back to editor
#endif
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
           smi     'A'                 ; check for assemble command
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
           ldn     rf                  ; quit command
           smi     'Q'                 ; check for quit command
#ifdef ELF2K
           lbz     08003h              ; monitor re-entry point
#else
           lbz     0f900h
#endif


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

doasm:     sep     scall               ; invoke the assembler
           dw      assemble
           lbr     mainlp              ; then back to main loop

run:       ldi     high startaddr      ; get start address of program
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
           ldi     0                   ; set initial line number
           plo     r8
           phi     r8
           sep     scall
           dw      setcurln
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


newmsg:    db      'New file'
crlf:      db      13,10,0
endmsg:    db      'End of buffer',10,13,0
topmsg:    db      'Top of buffer',10,13,0
prompt:    db      '>',0
colon:     db      ':',0


; ******************************
; *** Now the assembler code ***
; ******************************

assemble:  sep     scall               ; find end of text buffer
           dw      findend
           inc     ra                  ; 1 byte beyond end
           ldi     high symtab         ; point to symbol table pointer
           phi     rf
           ldi     low symtab
           plo     rf
           ghi     ra                  ; store symbol table address
           str     rf
           inc     rf
           glo     ra
           str     rf
           ldi     0                   ; set symbol table end
           str     ra
           ldi     high lastsym        ; set last symbol address
           phi     rf
           ldi     low lastsym
           plo     rf
           ghi     ra
           str     rf
           inc     rf
           glo     ra
           str     rf
           ldi     high pass           ; set pass to 0
           phi     rf
           ldi     low pass
           plo     rf
           ldi     0
           str     rf
           phi     ra                  ; set starting address
           plo     ra
; *** Have to setup to read lines from editor ***
; *** RD will point to editor lines           ***
           ldi     high textbuf
           phi     rd
           ldi     low textbuf
           plo     rd
           sep     scall               ; process the file
           dw      asmloop

           ldi     high pass           ; need to set pass to 1
           phi     rf
           ldi     low pass
           plo     rf
           ldi     1
           str     rf
           ldi     0                   ; set starting address
           phi     ra
           plo     ra

; *** Have to setup to read lines from editor ***
; *** RD will point to editor lines           ***
           ldi     high textbuf
           phi     rd
           ldi     low textbuf
           plo     rd
           sep     scall               ; perform second pass
           dw      asmloop

; *** Need to return control to editor ***
           sep     sret                ; and return to caller

; ****************************
; *** Move past whitespace ***
; ****************************
trim:      lda     rf                  ; get byte from buffer
           lbz     ltrim0              ; jump if it was zero
           smi     33                  ; get rid of anything space or lower
           lbnf    trim
ltrim0:    dec     rf                  ; point back to character
           sep     sret                ; and return

; Codes:
;     1 - 1 byte code
;     2 - 2 byte code
;     3 - 3 byte code
;     4 - special handling, 
;     5 - nybble mode

; **************************
; *** Process END opcode ***
; **************************
opend:     sep     scall               ; get argument
           dw      getarg
           ldi     high startaddr      ; get address for start
           phi     r8
           ldi     low startaddr
           plo     r8
           ghi     rd                  ; write start address
           str     r8
           inc     r8
           glo     rd
           str     r8
           lbr     opgood              ; done

; **************************
; *** Process ORG opcode ***
; **************************
oporg:     sep     scall               ; get argument
           dw      getarg
           ghi     rd                  ; set as addres
           phi     ra
           glo     rd
           plo     ra
           lbr     opgood
 
; *************************
; *** Process DB opcode ***
; *************************
opdb:      sep     scall               ; move past whitespace
           dw      ltrim
           ldn     rf                  ; get next byte
           smi     27h                 ; check for open quote
           lbz     opdbqt              ; jump if so
           sep     scall               ; get argument
           dw      getarg
           glo     rd                  ; and output low byte
           sep     scall
           dw      output
opdbclqt:  sep     scall               ; move past whitespace
           dw      ltrim
           ldn     rf                  ; check for a comma
           smi     ','
           lbnz    opdbdn              ; jump if not
           inc     rf                  ; move past comma
           lbr     opdb                ; process next byte
opdbdn:    lbr     opgood
opdbqt:    inc     rf                  ; move to next character
opdbqtlp:  lda     rf                  ; get byte
           lbz     opdbdn              ; jump if terminator found
           plo     re                  ; save a copy
           smi     27h                 ; see if closing quote
           lbz     opdbclqt            ; jump if so
           glo     re                  ; get byte
           sep     scall               ; and output it
           dw      output
           lbr     opdbqtlp            ; keep walking through
 
; *************************
; *** Process DW opcode ***
; *************************
opdw:      sep     scall               ; move past whitespace
           dw      ltrim
           ldn     rf                  ; get next byte
           smi     27h                 ; check for open quote
           lbz     opdwqt              ; jump if so
           sep     scall               ; get argument
           dw      getarg
           ghi     rd                  ; and output high byte
           sep     scall
           dw      output
           glo     rd                  ; and output low byte
           sep     scall
           dw      output
opdwclqt:  sep     scall               ; move past whitespace
           dw      ltrim
           ldn     rf                  ; check for a comma
           smi     ','
           lbnz    opdwdn              ; jump if not
           inc     rf                  ; move past comma
           lbr     opdw                ; process next byte
opdwdn:    lbr     opgood
opdwqt:    inc     rf                  ; move to next character
opdwqtlp:  lda     rf                  ; get byte
           lbz     opdwdn              ; jump if terminator found
           plo     re                  ; save a copy
           smi     27h                 ; see if closing quote
           lbz     opdwclqt            ; jump if so
           glo     re                  ; get byte
           sep     scall               ; and output it
           dw      output
           lbr     opdwqtlp            ; keep walking through

; **************************
; *** Process LDN opcode ***
; **************************
opldn:     sep     scall               ; get argument
           dw      getarg
           glo     rd                  ; get low value
           ani     0fh                 ; want only low byte
           lbz     opldner             ; zero is not allowed
           sep     scall               ; output the byte
           dw      output
opgood:    ldi     0                   ; signal success
           shr
           sep     sret                ; return to caller
opldner:   ldi     1                   ; signal an error
           shr
           sep     sret                ; and return to caller

; **************************
; *** Process OUT opcode ***
; **************************
opout:     sep     scall               ; get argument
           dw      getarg
           glo     rd
           lbz     opldner             ; cannot be zero
           smi     8                   ; or greater than 7
           lbdf    opldner
           glo     rd
           ori     060h                ; convert to out instruction
           sep     scall               ; output the byte
           dw      output
           lbr     opgood              ; then signal good

; **************************
; *** Process INP opcode ***
; **************************
opinp:     sep     scall               ; get argument
           dw      getarg
           glo     rd
           lbz     opldner             ; cannot be zero
           smi     8                   ; or greater than 7
           lbdf    opldner
           glo     rd
           adi     8                   ; move to input range
           ori     060h                ; convert to out instruction
           sep     scall               ; output the byte
           dw      output
           lbr     opgood              ; then signal good

ltrim:     lda     rf                  ; get byte from buffer
           lbz     ltrimdn             ; jump if end of string found
           smi     33                  ; check for space or less
           lbnf    ltrim               ; loop back if space or less
ltrimdn:   dec     rf                  ; move back to prev char
           sep     sret                ; and return

; *****************************
; *** Output assembled byte ***
; *****************************
output:    str     ra                  ; store value
           ldi     high pass           ; find out which pass
           phi     r8
           ldi     low pass
           plo     r8
           ldn     r8                  ; get current pass
           lbz     outputps0           ; jump if pass 0
           glo     rd                  ; save consumed registers
           stxd
           ghi     rd
           stxd
           glo     rf
           stxd
           ghi     rf
           stxd
           ldi     high bytecnt        ; need to check byte counter
           phi     rf
           ldi     low bytecnt
           plo     rf
           ldn     rf
           smi     4                   ; have 4 bytes already been output
           lbnz    outputgo            ; jump if not
           ldi     0                   ; reset count
           str     rf
           ldi     high nxtline        ; move pointer to next line
           phi     rf
           ldi     low nxtline
           plo     rf
           sep     scall
           dw      f_msg
outputgo:  ldn     ra                  ; recover byte
           plo     rd                  ; prepare to convert
           ldi     high outbuffer      ; point to output buffer
           phi     rf
           ldi     low outbuffer
           plo     rf 
           sep     scall               ; convert the value
           dw      f_hexout2
           ldi     32                  ; need a space
           str     rf
           inc     rf
           ldi     0                   ; terminate string
           str     rf
           ldi     high outbuffer      ; point to output buffer
           phi     rf
           ldi     low outbuffer
           plo     rf 
           sep     scall               ; display the byte
           dw      f_msg
           ldi     high bytecnt        ; need to increment byte counter
           phi     rf
           ldi     low bytecnt
           plo     rf
           ldn     rf
           adi     1
           str     rf
           irx                         ; recover consumed registers
           ldxa
           phi     rf
           ldxa
           plo     rf
           ldxa
           phi     rd
           ldx
           plo     rd
outputps0: inc     ra                  ; point to next memory location
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


; **********************************
; *** check D if hex             ***
; *** Returns DF=0 - hex         ***
; ***         DF=1 - non-hex     ***
; **********************************
ishex:     stxd                        ; keep a copy of the number
           sep     scall               ; see if it is numeric
           dw      isnum
           lbnf    ishexyes            ; jump if it is numeric
           irx                         ; retrieve value
           ldx
           stxd                        ; and keep on stack 
           smi     'A'                 ; check for below uppercase a
           lbnf    ishexno             ; value is not hex
           smi     6                   ; check for less then 'G'
           lbnf    ishexyes            ; jump if so
           irx                         ; retrieve value
           ldx
           smi     'a'                 ; check for lowercase a
           lbnf    ishexno2            ; jump if not
           smi     6                   ; check for less than 'g'
           lbnf    ishexyes2           ; jump if so
           lbr     ishexno2            ; value is not hex
ishexno:   irx                         ; remove value from stack
ishexno2:  ldi     1                   ; signal not hex
           shr
           sep     sret                ; and return to caller
ishexyes:  irx                         ; remove value from stack 
ishexyes2: ldi     0                   ; indicate a yes
           shr
           sep     sret                ; and return to caller

; *******************************
; *** Convert hex D to binary ***
; *** Returns: D - value      ***
; *******************************
hexbin:    stxd                        ; save value
           sep     scall               ; see if numeric
           dw      isnum
           lbdf    hexbin2             ; jump if alpha
           irx                         ; recover value
           ldx
           smi     '0'                 ; convert to binary
           sep     sret                ; and return to caller
hexbin2:   irx                         ; recover value
           ldx
           ani     0dfh                ; force to uppercase
           smi     55                  ; convert to binary
           sep     sret                ; and return to caller

; ***********************************************
; *** identify symbol as decimal, hex, or non ***
; *** RF - pointer to symbol                  ***
; *** Returns: D=0 - decimal number           ***
; ***          D=1 - hex number               ***
; ***          D=3 - non numeric              *** 
; ***          RD  - number                   ***
; ***          RF  - first char after symbol  ***
; ***********************************************
identify:  glo     rf                  ; save position in case of label
           stxd
           ghi     rf
           stxd
idlp1:     ldn     rf                  ; get byte from buffer
           smi     33                  ; check for whitspace
           ldn     rf                  ; also check for comma
           smi     ','
           lbz     isdec               ; still a valid number
           lbnf    isdec               ; number is a decimal
           ldn     rf                  ; recover byet
           sep     scall               ; see if it is numeric
           dw      isnum
           lbdf    idlp2               ; jump if not, hex routine
           inc     rf                  ; point to next position
           lbr     idlp1               ; loop until done
idlp2:     ldn     rf                  ; get byte from buffer
           smi     33                  ; check for whitspace
           lbnf    iderr               ; failed to find 'h' character
           ldn     rf                  ; check for H
           smi     'H'
           lbz     idhex               ; jump if found
           ldn     rf                  ; check for h
           smi     'h'
           lbz     idhex
           ldn     rf                  ; recover byet
           sep     scall               ; see if it is numeric
           dw      ishex
           lbdf    iderr               ; jump if not, hex routine
           inc     rf                  ; point to next position
           lbr     idlp2               ; loop until done
idhex:     inc     rf                  ; need to check byte following h
           ldn     rf
           smi     ','
           lbz     hexisgood           ; jump if it is
           ldn     rf                  ; comma is valid character
           smi     33                  ; must be space or less
           lbdf    iderr               ; otherwise error
hexisgood: irx                         ; recover beginning of number
           ldxa
           phi     rf
           ldx
           plo     rf
           sep     scall               ; convert ascii hex to binary
           dw      f_hexin
           inc     rf                  ; move past h
           ldi     1                   ; signal number was hex
           sep     sret                ; and return to caller
iderr:     irx                         ; number is an error, recover RF
           ldxa
           phi     rf
           ldx
           plo     rf
           ldi     3                   ; signal non-numeric
           sep     sret                ; return to caller
isdec:     irx                         ; recover beginning of number
           ldxa
           phi     rf
           ldx
           plo     rf
           sep     scall               ; convert ascii decimal to binary
           dw      f_atoi
           ldi     0                   ; signal number was decimal
           sep     sret                ; and return to caller

; ***************************************
; *** Get argument from assembly line ***
; *** RF - line after opcode          ***
; *** RB - pointer to symbol table    ***
; *** Returns: RD - value of arg      ***
; ***          DF=0 - no error        ***
; ***          DF=1 - error           ***
; ***             D=1 - no arg        ***
; ***             D=2 - inv number    ***
; ***             D=3 - inv label     ***
; ***************************************
getarg:    sep     scall               ; move past whitespace
           dw      ltrim
           ldn     rf                  ; get first argument byte
           lbz     getarg1             ; jump if at terminator
           sep     scall               ; see if it is numeric
           dw      isnum
           lbnf    argnum              ; jump if argument is numeric
           ldn     rf                  ; check for possible register
           smi     'R'
           lbz     argreg              ; might be
           ldn     rf                  ; check lowercase r as well
           smi     'r'
           lbz     argreg
notreg:    sep     scall               ; search for symbol
           dw      getsymbol
           lbdf    getnolab            ; jump if label was not valid
           ldi     0                   ; signal success
           shr
           sep     sret                ; return to caller
getnolab:  ldi     0                   ; invalid labels are zero
           phi     rd
           plo     rd
           ldi     7                   ; signal invalid label
           shr
           sep     sret                ; and return to caller

argreg:    inc     rf                  ; point to next byte
           ldn     rf                  ; retrieve it
           sep     scall               ; see if is hex
           dw      ishex
           lbdf    argno               ; jump if not hex
           inc     rf                  ; get following byte
           ldn     rf
           dec     rf                  ; keep pointer pointing at 2nd byte
           smi     33                  ; must be a space or less
           lbdf    argno               ; nope, it was not a register
           ldn     rf                  ; retrieve register number
           sep     scall              
           dw      hexbin              ; convert to binary
           plo     rd                  ; place into return register
           ldi     0                   ; high byte is zero
           phi     rd
           shr                         ; signal success
           sep     sret                ; return to caller
argno:     dec     rf                  ; point back to first byte
           lbr     notreg              ; and process as a label

argnum:    sep     scall               ; identify number
           dw      identify
           smi     3                   ; was it non-numeric
           lbz     argnumer            ; jump if non-numeric
           ldi     0                   ; signal valid
           shr
           sep     sret                ; and return to caller
argnumer:  ldi     5                   ; signal invalid number
           shr
           sep     sret                ; and return to caller
          

getarg1:   ldi     0                   ; not found, set value as zero
           phi     rd
           plo     rd
           ldi     3                   ; set no arg error
           shr
           sep     sret                ; return to caller

; ************************************
; *** Assemble next line           ***
; *** RF - line to assemble        ***
; *** RA - Current address         ***
; *** RB - pointer to symbol table ***
; *** Returns: DF=0 - success      ***
; ***          DF=1 - failed       ***
; ************************************
asm:       ldi     high curaddr        ; set current address
           phi     r8
           ldi     low curaddr
           plo     r8
           ghi     ra
           str     r8
           inc     r8
           glo     ra
           str     r8
           ldi     high pass           ; get current pass
           phi     r8
           ldi     low pass
           plo     r8
           ldn     r8
           lbz     asmpass0            ; can only get on second pass
           glo     rf                  ; save line position
           stxd
           ghi     rf
           stxd
           ldi     high bytecnt        ; zero byte count for line
           phi     rf
           ldi     low bytecnt
           plo     rf
           ldi     0
           str     rf
           ghi     ra                  ; transfer address to RD
           phi     rd
           glo     ra
           plo     rd
           ldi     high outbuffer      ; get address of buffer
           phi     rf
           ldi     low outbuffer
           plo     rf
           sep     scall               ; convert address
           dw      f_hexout4
           ldi     32                  ; need a space
           str     rf
           inc     rf
           ldi     0                   ; place terminator
           str     rf
           ldi     high outbuffer      ; get address of buffer
           phi     rf
           ldi     low outbuffer
           plo     rf
           sep     scall               ; display the address
           dw      f_msg
           irx                         ; recover line buffer
           ldxa
           phi     rf
           ldx
           plo     rf
asmpass0:  ldn     rf                  ; get first byte from line
           smi     33                  ; check for space or less
           lbnf    asm2                ; jump if no labels
           ghi     rf                  ; make a copy
           phi     r9
           glo     rf
           plo     r9
asmlbl1:   ldn     rf                  ; get byte from label
           smi     ':'                 ; check for end of label
           lbz     asmlble             ; found end
           ldn     rf                  ; get another byte
           smi     33                  ; check for space or less
           lbnf    asmlble             ; found end
           inc     rf                  ; point to next byte
           lbr     asmlbl1             ; and keep looking
asmlble:   ldi     0                   ; terminate the label
           str     rf
           glo     rf                  ; save positoin
           stxd
           ghi     rf
           stxd
           ghi     r9                  ; get start of label
           phi     rf
           glo     r9
           plo     rf
           sep     scall               ; add the symbol
           dw      addsym
           irx                         ; recover line pointer
           ldxa
           phi     rf
           ldx
           plo     rf
           ldi     ':'
           str     rf
           inc     rf
asm2:      ldi     high insttab        ; point to opcode table
           phi     r9
           ldi     low insttab
           plo     r9
           sep     scall               ; move past leading spaces
           dw      ltrim
asmlp1:    sep     scall               ; check for opcode match
           dw      chkentry
           lbnf    asmfnd              ; jump if entry was found
           inc     r9                  ; point to next opcode entry
           inc     r9
           inc     r9
           ldn     r9                  ; get byte from table
           lbnz    asmlp1              ; loop back if not end of table
           ldi     1                   ; signal an error
asmret:    shr
           sep     sret                ; and return to caller
asmgood:   ldi     0                   ; signal success
           lbr     asmret
asmfnd:    lda     r9                  ; get instruction type
           smi     1                   ; see if 1 byte code
           lbnz    asmnot1             ; jump if not
           lda     r9                  ; get opcode
           sep     scall               ; output the byte
           dw      output
           lbr     asmgood             ; then return
asmnot1:   smi     1                   ; check for 2 byte instruction
           lbnz    asmnot2             ; jump if not
           lda     r9                  ; get opcode
           sep     scall               ; and output it
           dw      output
           sep     scall               ; get argument
           dw      getarg
           lbdf    asmerror            ; jump if error in argument
           glo     rd                  ; get low value
           sep     scall               ; and output it
           dw      output
           lbr     asmgood             ; then return
asmnot2:   smi     1                   ; check for 3 byte instruction
           lbnz    asmnot3             ; jump if not
           lda     r9                  ; get opcode
           sep     scall               ; and output it
           dw      output
           sep     scall               ; get argument
           dw      getarg
           lbdf    asmerror            ; jump if error in argument
           ghi     rd                  ; get high value
           sep     scall               ; and output it
           dw      output
           glo     rd                  ; get low value
           sep     scall               ; and output it
           dw      output
           lbr     asmgood             ; then return
asmnot3:   smi     1                   ; check for special handling
           lbnz    asmnot4             ; jump if not
           ldi     high jump           ; get jump vector
           phi     r7
           ldi     low jump
           plo     r7
           ldi     0c0h
           str     r7
           inc     r7                  ; point to jump destination
           lda     r9                  ; get handler
           str     r7                  ; and write into jump address
           inc     r7
           ldn     r9
           str     r7
           lbr     jump                ; jump to jump address
asmnot4:   sep     scall               ; get argument
           dw      getarg
           lbdf    asmerror            ; jump if error in argument
           glo     rd                  ; get low value
           ani     0fh                 ; keep only low nybble
           str     r2                  ; store into memory
           lda     r9                  ; get opcode
           or                          ; and or with register nybble
           sep     scall               ; and output it
           dw      output
           lbr     asmgood             ; then return
asmerror:  ldi     high errmsg         ; point to error message
           phi     rf
           ldi     low errmsg
           plo     rf
           sep     scall
           dw      f_msg
           ldi     0
           shr
           sep     sret
           


; ***********************************************
; *** Check to see if a table entry matches   ***
; *** RF - buffer                             ***
; *** R9 - table entry                        ***
; *** Returns: R9 - first byte following name ***
; ***          DF=0 - entry found             ***
; ***          DF=1 - entry not foune         ***
; ***          RF=orig on failure             ***
; ***          otherwise first byte after     ***
; ***********************************************
chkentry:  glo      rf                 ; save buffer position
           stxd
           ghi      rf
           stxd
chkloop:   lda      r9                 ; get byte from table entry
           plo      re                 ; keep a copy
           ani      128                ; see if high bit is set
           lbnz     chklst             ; jump if last character
           lda      rf                 ; get byte from buffer
           str      r2                 ; put in memory for compare
           glo      re                 ; recover table byte
           sm                          ; compare values
           lbz      chkloop            ; loop back if a match
chklp2:    lda      r9                 ; need to find end of entry
           ani      128
           lbz      chklp2             ; loop until end found
chkno:     ldi      1                  ; signal entry not found
chkend:    shr
           irx                         ; recover buffer position
           ldxa
           phi      rf
           ldx
           plo      rf
           sep      sret               ; and return to caller
chklst:    lda      rf                 ; get byte from buffer
           str      r2                 ; put in memory for compare
           glo      re                 ; recover table byte
           ani      07fh               ; strip high bit
           sm                          ; compare with buffer byte
           lbnz     chkno              ; jump if no match
           ldn      rf                 ; load next byte from buffer
           smi      33                 ; must be space or less
           lbdf     chkno              ; jump if not space or less
           ldi      0                  ; signal success
           shr
           irx                         ; remove RF from stack
           irx
           sep      sret               ; and return to caller

; ***********************************
; *** Process over the input file ***
; *** RD - file descriptor        ***
; ***********************************
asmloop:   ldi     high buffer         ; point to buffer
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; read next line
           dw      readln
           lbdf    asmloopdn           ; jump if end of file reached
           glo     rc                  ; get character count
           lbz     asmloop             ; jump if at end of file
asmnodsp:  ldi     high buffer         ; point back to input line
           phi     rf
           ldi     low buffer
           plo     rf
           glo     rd                  ; save descriptor
           stxd
           ghi     rd
           stxd
           sep     scall               ; assemble the line
           dw      asm
           ldi     high pass           ; get pass
           phi     rf
           ldi     low pass
           plo     rf
           ldn     rf
           lbz     asmlpps0            ; jump if on first pass
           shrc                        ; save DF
           stxd
           ldi     high bytecnt        ; need output byte count
           phi     rf
           ldi     low bytecnt
           plo     rf
           ldn     rf
outbloop:  stxd                        ; save copy of count
           smi     4                   ; have 4 bytes been output
           lbz     outlpdn             ; jump if so
           ldi     high outbuffer      ; point back to input line
           phi     rf
           ldi     low outbuffer
           plo     rf
           ldi     32                  ; need 3 spaces
           str     rf
           inc     rf
           str     rf
           inc     rf
           str     rf
           inc     rf
           ldi     0                   ; and a terminator
           str     rf
           inc     rf
           dec     rf                  ; back to beginning of buffer
           dec     rf
           dec     rf
           dec     rf
           sep     scall               ; display the blanks
           dw      f_msg
           irx                         ; recover count
           ldx
           adi     1                   ; add 1
           lbr     outbloop            ; loop back til done
outlpdn:   irx                         ; remove D from stack
           ldi     high buffer         ; point back to input line
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           ldi     high crlf           ; display a cr/lf
           phi     rf
           ldi     low crlf
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           irx                         ; recover DF
           shl
asmlpps0:  lbnf    asmnoerr            ; jump if no error happened
           ldi     high errmsg         ; otherwise display error message
           phi     rf
           ldi     low errmsg
           plo     rf
           sep     scall
           dw      f_msg
asmnoerr:  irx                         ; recover descripter
           ldxa
           phi     rd
           ldx
           plo     rd
           lbr     asmloop             ; loop back for next line
asmloopdn: sep     sret                ; return to caller

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

; **************************************
; *** Scan symbol table for a symbol ***
; *** RF - symbol to search for      ***
; *** Returns: RD - value            ***
; ***          DF=0 - entry found    ***
; ***          DF=1 - not found      ***
; **************************************
getsymbol: ldn     rf                  ; get first byte
           smi     '$'                 ; check for dollar sign
           lbnz    notdollar           ; jump if not
           ldi     high curaddr        ; point to current address
           phi     rd
           ldi     low curaddr
           plo     rd
           lda     rd                  ; get high byte
           plo     re                  ; set aside
           ldn     rd                  ; get low byte
           plo     rd                  ; set rd to current address
           glo     re
           phi     rd
           ldi     0                   ; signal success
           shr
           sep     sret                ; return to caller
notdollar: ldi     high pass           ; get current pass
           phi     r8
           ldi     low pass
           plo     r8
           ldn     r8
           lbnz    getsym1             ; can only get on second pass
           ldi     0                   ; first pass symbol is zero
           phi     rd
           plo     rd
           shr                         ; and symbol is assumed to exist
           sep     sret                ; otherwise return to caller
getsym1:   ldi     high symtab         ; point to symbol table
           phi     r8
           ldi     low symtab
           plo     r8
           lda     r8                  ; high byte of symbol table
           plo     re                  ; save for a momenht
           ldn     r8                  ; get low byte
           plo     r8                  ; set R8 to symbol table
           glo     re
           phi     r8
getsymlp:  ldn     r8                  ; get byte from symbol table
           lbnz    getsymgo            ; go if still valid symbols
           ldi     1                   ; signal not found
           shr
           sep     sret                ; and return to caller
getsymgo:  glo     rf                  ; save user search
           stxd
           ghi     rf
           stxd
getsymlp2: lda     r8                  ; get byte from symbol table
           str     r2                  ; keep a copy
           ani     128                 ; is high bit set
           lbnz    getsymlst           ; jump if so
           lda     rf                  ; get byte from search
           sm                          ; compare
           lbz     getsymlp2           ; jump if a match
getsymnxt: lda     r8                  ; get byte from sybol table
           ani     128                 ; check for final byte
           lbz     getsymnxt           ; loop back if not
getsymnls: inc     r8                  ; point to next symbol
           inc     r8
           irx                         ; recover user search
           ldxa
           phi     rf
           ldx
           plo     rf
           lbr     getsymlp            ; and check next symbol
getsymlst: ldn     r2                  ; recover byte
           ani     07fh                ; strip high bit 
           str     r2                  ; put back in memory
           lda     rf                  ; get byte from user search
           sm                          ; are they the same
           lbnz    getsymnls           ; jump if not
           lda     rf                  ; get next byte
           plo     re                  ; keep a copy
           smi     '.'                 ; see if it is a dot
           lbz     getsymdot           ; jump if so
           glo     re                  ; recover byte
           smi     33                  ; must be space or less
           lbdf    getsymnls           ; jump if not
           irx                         ; recover user search
           ldxa
           phi     rf
           ldx
           plo     rf
           ldi     0                   ; indicate use symbol as is
           plo     re
getsymcnt: lda     r8                  ; get symbol value
           phi     rd
           lda     r8
           plo     rd
           glo     re                  ; see if need high in low
           smi     '1'
           lbz     getsymsw            ; yep
           ldi     0                   ; signal success
           shr
           sep     sret                ; and return to caller
getsymsw:  ghi     rd                  ; move high value to low
           plo     rd
           ldi     0                   ; zero high byte
           shr
           phi     rd
           sep     sret                ; return to caller
getsymdot: ldn     rf                  ; get byte following dot
           plo     re                  ; store into re
           irx                         ; recover user search
           ldxa
           phi     rf
           ldx
           plo     rf
           lbr     getsymcnt           ; then continue

; *******************
; *** Add symbol  ***
; *** RF - symbol ***
; *** RA - value  ***
; *******************
addsym:    ldi     high pass           ; get current pass
           phi     r8
           ldi     low pass
           plo     r8
           ldn     r8
           lbz     addsym1             ; can only add on first pass
           sep     sret                ; otherwise return to caller
addsym1:   ldi     high lastsym        ; get address of last symbol
           phi     r8
           ldi     low lastsym
           plo     r8
           lda     r8                  ; get high byte for next symbol
           plo     re                  ; keep it
           ldn     r8                  ; get low byte
           plo     r8                  ; set r8 to next symbol address
           glo     re
           phi     r8
addsymlp:  lda     rf                  ; get byte from symbol
           plo     re                  ; keep a copy
           smi     33                  ; check for space or less
           lbnf    addsym2             ; jump if end of symbol reached
           glo     re                  ; recover byte
           str     r8                  ; store into symbol table
           inc     r8                  ; point to next position
           lbr     addsymlp            ; loop until done copying name
addsym2:   dec     r8                  ; point back to previous char
           ldn     r8                  ; retrieve it
           ori     128                 ; set high bit
           str     r8                  ; and put back
           inc     r8                  ; point to value field
           ghi     ra                  ; store value
           str     r8
           inc     r8
           glo     ra
           str     r8
           inc     r8
           ldi     0                   ; end of table marker
           str     r8
           ldi     high lastsym        ; need to store new end value
           phi     rf
           ldi     low lastsym
           plo     rf
           ghi     r8
           str     rf
           inc     rf
           glo     r8
           str     rf
           sep     sret                ; return to caller
           

hello:     db      'Edt/Asm (02/21/06)',10,13
           db      '(c) Copyright 2006 by Michael H. Riley',10,13,0
fileerr:   db      'File Error',10,13,0
errmsg:    db      'Error',10,13,0
nxtline:   db      10,13,'     ',0

insttab:   db      'AD',('D'+80h),1,0f4h,0
           db      'AD',('C'+80h),1,074h,0
           db      'ADC',('I'+80h),2,07ch,0
           db      'AD',('I'+80h),2,0fch,0
           db      'AN',('D'+80h),1,0f2h,0
           db      'AN',('I'+80h),2,0fah,0
           db      'B',('1'+80h),2,034h,0
           db      'B',('2'+80h),2,035h,0
           db      'B',('3'+80h),2,036h,0
           db      'B',('4'+80h),2,037h,0
           db      'BD',('F'+80h),2,033h,0
           db      'BN',('1'+80h),2,03ch,0
           db      'BN',('2'+80h),2,03dh,0
           db      'BN',('3'+80h),2,03eh,0
           db      'BN',('4'+80h),2,03fh,0
           db      'BN',('F'+80h),2,03bh,0
           db      'BN',('Q'+80h),2,039h,0
           db      'B',('Q'+80h),2,031h,0
           db      'B',('R'+80h),2,030h,0
           db      'B',('Z'+80h),2,032h,0
           db      'BN',('Z'+80h),2,03ah,0
           db      'DE',('C'+80h),5,020h,0
           db      'DI',('S'+80h),1,071h,0
           db      'GH',('I'+80h),5,090h,0
           db      'GL',('O'+80h),5,080h,0
           db      'ID',('L'+80h),1,000h,0
           db      'IN',('C'+80h),5,010h,0
           db      'IN',('P'+80h),4
           dw           opinp
           db      'IR',('X'+80h),1,060h,0
           db      'LBD',('F'+80h),3,0c3h,0
           db      'LBN',('F'+80h),3,0cbh,0
           db      'LBN',('Q'+80h),3,0c9h,0
           db      'LBN',('Z'+80h),3,0cah,0
           db      'LB',('Q'+80h),3,0c1h,0
           db      'LB',('R'+80h),3,0c0h,0
           db      'LD',('A'+80h),5,040h,0
           db      'LD',('I'+80h),2,0f8h,0
           db      'LD',('N'+80h),4
           dw           opldn
           db      'LD',('X'+80h),1,0f0h,0
           db      'LDX',('A'+80h),1,072h,0
           db      'LSD',('F'+80h),1,0cfh,0
           db      'LSI',('E'+80h),1,0cch,0
           db      'LSN',('F'+80h),1,0c7h,0
           db      'LSN',('Q'+80h),1,0c5h,0
           db      'LSN',('Z'+80h),1,0c6h,0
           db      'LS',('Q'+80h),1,0cdh,0
           db      'LS',('Z'+80h),1,0ceh,0
           db      'MAR',('K'+80h),1,079h,0
           db      'NB',('R'+80h),1,038h,0
           db      'NLB',('R'+80h),1,0c2h,0
           db      'NO',('P'+80h),1,0c4h,0
           db      'O',('R'+80h),1,0f1h,0
           db      'OR',('I'+80h),2,0f9h,0
           db      'OU',('T'+80h),4
           dw           opout
           db      'PH',('I'+80h),5,0b0h,0
           db      'PL',('O'+80h),5,0a0h,0
           db      'RE',('T'+80h),1,070h,0
           db      'RE',('Q'+80h),1,07ah,0
           db      'S',('D'+80h),1,0f5h,0
           db      'SA',('V'+80h),1,078h,0
           db      'SD',('B'+80h),1,075h,0
           db      'SDB',('I'+80h),2,07dh,0
           db      'SD',('I'+80h),2,0fdh,0
           db      'SE',('P'+80h),5,0d0h,0
           db      'SE',('Q'+80h),1,07bh,0
           db      'SE',('X'+80h),5,0e0h,0
           db      'SH',('L'+80h),1,0feh,0
           db      'SH',('R'+80h),1,0f6h,0
           db      'SHL',('C'+80h),1,07eh,0
           db      'SHR',('C'+80h),1,076h,0
           db      'S',('M'+80h),1,0f7h,0
           db      'SM',('B'+80h),1,077h,0
           db      'SMB',('I'+80h),2,07fh,0
           db      'SM',('I'+80h),2,0ffh,0
           db      'ST',('R'+80h),5,050h,0
           db      'STX',('D'+80h),1,073h,0
           db      'XO',('R'+80h),1,0f3h,0
           db      'XR',('I'+80h),2,0fbh,0
           db      'EN',('D'+80h),4
           dw           opend
           db      'OR',('G'+80h),4
           dw           oporg
           db      'D',('B'+80h),4
           dw           opdb
           db      'D',('W'+80h),4
           dw           opdw
           db      0





           org     2000h
curline:   dw      0
reg1:      dw      0
reg2:      dw      0
char:      db      0
lastsym:   dw      0
pass:      db      0
bytecnt:   db      0
curaddr:   dw      0
symtab:    dw      0
startaddr: dw      0
#ifndef ELF2K
baud:      db      0
#endif
jump:      db      0,0,0               ; jump to special routine
numbuf:    db      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
buffer:    db      0
           org     $+128
outbuffer: db      0
           org     $+336
stack:     db      0


; text buffer format
; byte size of line (0 if end of buffer)
textbuf:   db      0

