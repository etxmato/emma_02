; *******************************************************************
; *** This software is copyright 2004 by Michael H Riley          ***
; *** You have permission to use, modify, copy, and distribute    ***
; *** this software so long as this copyright notice is retained. ***
; *** This software may not be used in commercial applications    ***
; *** without express written permission from the author.         ***
; *******************************************************************

errnolist: equ     1
errnobind: equ     2
errinvcmd: equ     3
erreverr:  equ     4

include    bios.inc
;include    kernel.inc

buffer:    equ     020h
oblist:    equ     100h
oblistend: equ     100h+21*16
nodelist:  equ     256+200*16

dta:       equ     7000h
dtapage:   equ     070h
stack:     equ     7fffh

           org     0d000h
           ldi     high beg
           phi     r3
           ldi     low beg
           plo     r3
           sep     r3
beg:       ldi     0
           phi     r2
           ldi     0ffh
           plo     r2
           ldi     high start
           phi     r6
           ldi     low start
           plo     r6
           lbr     f_initcall
start:     sep     scall               ; set terminal baud rate
           dw      f_setbd
           ldi     high stack          ; set stack to top of 32k
           phi     r2
           ldi     low stack
           plo     r2
           sep     scall               ; display welcome message
           dw      f_inmsg
           db      10,13,'Rc/Lisp v 1.2r',10,13
           db      'by Michael H. Riley',10,13,0

;          ldi     high welcome        ; display welcome message
;          phi     rf
;          ldi     low welcome
;          plo     rf
;          sep     scall
;          dw      f_msg
; *****************************************
; *** Copy initial oblist to low memory ***
; *****************************************
           ldi     high oblist2        ; get oblist pointer
           phi     r8
           ldi     low oblist2
           plo     r8
           ldi     high oblist         ; destination
           phi     r7
           ldi     low oblist
           plo     r7
           ldi     1                   ; copy 16*21 bytes
           phi     rc
           ldi     80
           plo     rc
cpylp:     lda     r8                  ; get source byte
           str     r7                  ; store into desination
           inc     r7                  ; point to next
           dec     rc                  ; decrement count
           glo     rc                  ; check if done 
           lbnz    cpylp
           ghi     rc
           lbnz    cpylp

           ldi     high oblistend      ; need to zero to end of memory
           phi     rf
           ldi     low oblistend
           plo     rf
           ldi     0                   ; want zero
           plo     re                  ; place in temp var
zerolp:    glo     re                  ; zero memory
           str     rf
           inc     rf                  ; point to next memory
           glo     rf                  ; check for end of memory
           bnz     zerolp              ; loop back if not at end
           ghi     rf                  ; check high byte
           smi     080h                ; stop at 32k boundary
           bnz     zerolp              ; loop back if not at end
           ldi     low nodelist        ; get address of nodelist
           smi     16                  ; minus 16 bytes
           plo     rf
           ldi     high nodelist       ; which is last oblist entry
           smbi    0
           phi     rf
           ldi     0ffh                ; indicator for end of oblist
           str     rf
mainlp:    ldi     0                   ; set nest level to zero
           plo     rc
           phi     rc
           ldi     high stack          ; set stack
           phi     r2
           ldi     low stack
           plo     r2
           sep     scall               ; get input from user
           dw      input
           sep     scall               ; trim leading spaces
           dw      ltrim
           ldn     rf                  ; get first non-space character
           smi     '('                 ; is it an s-expression?
           lbz     sexp                ; jump if so
           ldi     high syslist        ; point to system commands
           phi     r7
           ldi     low syslist
           plo     r7
mainlp2:   ghi     rf                  ; save buffer position
           phi     r8
           glo     rf
           plo     r8
mainelp:   ldn     r7                  ; get byte from syslist
           ani     080h                ; see if last character
           lbnz    mainego1            ; jump if so
           lda     r7                  ; retrieve char
           str     r2                  ; prepare for compare
           lda     rf                  ; get byte from input
           sm                          ; see if they match
           lbz     mainelp             ; jump if so
mainelp2:  lda     r7                  ; look for end of current entry
           ani     080h                ; last char has bit 7 set
           lbz     mainelp2            ; loop until found
mainego3:  inc     r7                  ; move past address
           inc     r7
           ghi     r8                  ; recover buffer position
           phi     rf
           glo     r8
           plo     rf
           ldn     r7                  ; get byte
           lbnz    mainlp2             ; jump if more commands to check
           sep     scall               ; display error
           dw      f_inmsg
           db      'Invalid command',10,13,10,13,0
           lbr     mainlp              ; back to main loop
mainego1:  lda     r7                  ; get command byte
           ani     07fh                ; strip high bit
           str     r2                  ; prepare for compare
           lda     rf                  ; get byte from input
           sm                          ; check for a match
           lbnz    mainego3            ; jump if not
           ldn     rf                  ; get next byte
           smi     33                  ; must be whitespace
           lbdf    mainego3            ; jump if not
;           ldi     high jump1          ; point to jump bytes
;           phi     r8
;           ldi     low jump1
;           plo     r8
           ldi     0
           phi     r8
           plo     r8
           ldi     0c0h
           str     r8
           inc     r8
           lda     r7                  ; get routine address
           str     r8                  ; write into jump bytes
           inc     r8
           ldn     r7
           str     r8
           lbr     0
;           db      0c0h
;jump1:     db      0,0

sexp:      ldi     0                   ; setup parens count
           plo     rc
           phi     rc
           inc     rf                  ; move past opening paren
           sep     scall               ; bind the list
           dw      bindlist
           sep     scall               ; evaluate the s-expr
           dw      eval
           lbdf    evalerror           ; jump on error
           ldi     high dta
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall
           dw      prtlist
           ldi     10
           str     rf
           inc     rf
           ldi     13
           str     rf
           inc     rf
           ldi     0
           str     rf
           inc     rf
           ldi     high dta
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall
           dw      f_msg
           lbr     mainlp
evalerror: sep     scall               ; display evaluation error
           dw      f_inmsg
           db      '??? Evaluation Error ???',10,13,10,13,0
           lbr     mainlp              ; and back to mainloop

; **********************************************************
; *** Routine to get additional input if s-expr not done ***
; **********************************************************
input:     ghi     rc                  ; need to display nest level
           phi     rd
           glo     rc
           plo     rd
           ldi     high buffer         ; where to put number
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; convert number for output
           dw      f_intout
           ldi     '>'                 ; put in rest of prompt
           str     rf
           inc     rf
           ldi     0                   ; terminator
           str     rf
           ldi     high buffer         ; point to prompt
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; and display it
           dw      f_msg
           ldi     high dta            ; point to input buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; and get user input
           dw      f_input
           sep     scall               ; display a CR/LF
           dw      crlf
           ldi     high dta            ; point to input buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     sret                ; and return to caller

          
; ************************************
; *** Bind an S-expression         ***
; *** RF - pointer to input buffer ***
; *** Returns: R7 - Root node      ***
; ************************************
bindlist:  inc     rc                  ; increment parens count
           sep     scall               ; get a free node
           dw      getfrnode
           glo     r7                  ; save root node
           plo     r8
           ghi     r7
           phi     r8
blnxtnode: ldi     1                   ; signal node is used
           str     r7
; *** Process CAR for list ***
blgo2:     sep     scall               ; trim leading spaces
           dw      ltrim
           ldn     rf                  ; get byte from input
           lbnz    blgo1               ; jump if not null
           sep     scall               ; get more input
           dw      input
           lbr     blgo2               ; and continue
blgo1:     sep     scall               ; get atom address
           dw      getoblist
           lbnf    blcarnq             ; jump if car is not quoted
           str     r2                  ; save type
           ldi     5                   ; signal car is quoted
           str     r7
           ldx                         ; recover type
blcarnq:   lbnz    blcarn0             ; jump if not an atom
strcarra:  inc     r7                  ; point to car pointer
           ghi     ra                  ; and store car address
           str     r7
           inc     r7
           glo     ra
           str     r7
           inc     r7
           lbr     blcdr               ; jump to process cdr
blcarn0:   smi     1                   ; check for a number
           lbnz    blcarn1             ; jump if not 
           ldn     r7                  ; get car flags
           ori     2                   ; indicate car is numeric
           str     r7                  ; and put back
           lbr     strcarra            ; then store value
blcarn1:   smi     1                   ; check for a list
           lbnz    blcarn2             ; jump if not
           inc     rf                  ; move past parens
           glo     r8                  ; save addresses
           stxd
           ghi     r8
           stxd
           glo     r7                  ; save addresses
           stxd
           ghi     r7
           stxd
           sep     scall               ; bind new list
           dw      bindlist
           ghi     r7                  ; move result list
           phi     ra
           glo     r7
           plo     ra
           irx                         ; recover other addresses
           ldxa
           phi     r7
           ldxa
           plo     r7
           ldxa
           phi     r8
           ldx
           plo     r8
           lbr     strcarra            ; and store list address
blcarn2:   inc     r7                  ; zero the node
           ldi     0
           str     r7
           inc     r7
           str     r7
           inc     r7
           str     r7
           inc     r7
           str     r7
           inc     r7
           ghi     r8                  ; retrieve root node
           phi     r7                  ; place into result
           glo     r8
           plo     r7
           dec     rc                  ; decrement parens count
           inc     rf                  ; move past close parens
           sep     sret                ; and return to caller
blcdr:     sep     scall               ; move past any whitespace
           dw      ltrim
           ldn     rf                  ; see if hit terminator
           lbnz    blgo3               ; jump if not
           sep     scall               ; otherwise get more input
           dw      input
           lbr     blcdr               ; and keep going
blgo3:     ghi     r7                  ; transfer address
           phi     ra
           glo     r7
           plo     ra
           sep     scall               ; get a free node
           dw      getfrnode
           ghi     r7                  ; store into previous node
           str     ra
           inc     ra
           glo     r7
           str     ra
           lbr     blnxtnode           ; process node
           

ltrim:     ldn     rf                  ; get byte from input
           lbz     ltrimdn             ; jump if is the terminator
           smi     33                  ; see if it is a white space
           lbdf    ltrimdn             ; jump if not
           inc     rf                  ; otherwise increment pointer
           lbr     ltrim               ; and look again
ltrimdn:   sep     sret                ; return to caller

; ******************************************
; *** Reduce atomic value                ***
; *** RF - pointer to string             ***
; *** Returns: - DF=1 - item is quoted   ***
; ***            DF=0 - item not quoted  ***
; ***            RA   - Address of item  ***
; ***            D=0  - Item is an atom  ***
; ***            D=1  - Item is a number ***
; ***            D=2  - Item is a list   ***
; ***            D=3  - Item is list end ***
; ******************************************
getoblist: ldn     rf                  ; get first character from symbol
           smi     027h                ; check for quote character
           lbnz    noquote             ; jump if not quoted
           inc     rf                  ; move past quote
           ldi     1                   ; indicate quoted
           lskp                        ; skip next assign
noquote:   ldi     0                   ; signal not quoted
           stxd                        ; save on stack
           ldn     rf                  ; get next byte
           smi     '('                 ; look for lists
           lbz     islist              ; jump if it is a list
           ldn     rf                  ; check for end of list
           smi     ')'
           lbz     eoflist
           ldn     rf                  ; retrieve character
           smi     '0'                 ; check for below numbers
           lbnf    notnum              ; jump if not a number
           ldn     rf                  ; retrieve character
           smi     03ah                ; check for high range of numbers
           lbdf    notnum              ; jump if not numeric
           glo     rf                  ; save buffer position
           stxd
           ghi     rf
           stxd
           ldi     0                   ; set initial number to zero
           plo     ra
           phi     ra
numloop:   ldn     rf                  ; get byte from buffer
           lbz     numdone             ; jump if done with number
           smi     ' '                 ; space is also numeric terminator
           lbz     numdone
           ldn     rf
           smi     ')'                 ; so is a list close
           lbz     numdone
           ldn     rf                  ; now check if below numbers
           smi     '0'
           lbnf    numerr              ; jump if so
           ldn     rf                  ; check if above numbers
           smi     03ah
           lbdf    numerr              ; jump if so
           glo     ra                  ; multiply current number by 10
           plo     rd
           shl
           plo     ra
           ghi     ra
           phi     rd
           shlc
           phi     ra                  ; multiplied by 2
           glo     ra
           shl
           plo     ra
           ghi     ra
           shl
           phi     ra                  ; multiplied by 4
           glo     rd
           str     r2
           glo     ra
           add
           plo     ra
           ghi     rd
           str     r2
           ghi     ra
           adc
           phi     ra                  ; multiplied by 5
           glo     ra
           shl
           plo     ra
           ghi     ra
           shlc
           phi     ra                  ; multiplied by 10
           lda     rf                  ; get next digit
           smi     030h                ; convert to binary
           str     r2                  ; add into total
           glo     ra
           add
           plo     ra
           ghi     ra
           adci    0
           phi     ra
           lbr     numloop             ; loop back for more digits
numdone:   irx                         ; remove buffer position from stack
           irx
           irx                         ; recover quote flag
           ldx
           shr                         ; move to DF
           ldi     1                   ; indicate item is a number
           sep     sret                ; and return to caller
numerr:    irx                         ; recover buffer position
           ldxa
           phi     rf
           ldx
           plo     rf                  ; then proceed as if non-numeric
notnum:    ldi     high oblist         ; point to object list
           phi     rd
           ldi     low oblist
           plo     rd
chkobloop: ldn     rd                  ; get byte from oblist entry
           lbz     noentry             ; jump if entry is blank
           smi     0ffh                ; check for end of oblist
           lbz     nomore              ; jump if no more entries
           glo     rd                  ; save position of oblist
           stxd
           ghi     rd
           stxd
           glo     rf                  ; save position of buffer
           stxd
           ghi     rf
           stxd
           inc     rd                  ; point to object name
namelp:    lda     rd                  ; get byte from oblist
           lbz     namelpdn            ; jump if end of name in oblist
           str     r2                  ; otherwise compare
           lda     rf
           sm
           lbz     namelp              ; jump if chars match
nomatch:   irx                         ; recover positions
           ldxa
           phi     rf
           ldxa
           plo     rf
           ldxa
           phi     rd
           ldx
           plo     rd
noentry:   glo     rd                  ; point to next entry
           adi     16
           plo     rd
           ghi     rd
           adci    0
           phi     rd
           lbr     chkobloop           ; and keep looking
namelpdn:  ldn     rf                  ; get byte from string
           lbz     matched             ; jump if terminator
           smi     33                  ; check for white space
           lbnf    matched
           ldn     rf                  ; or end of list
           smi     ')'
           lbz     matched
           lbr     nomatch             ; otherwise no match
matched:   irx                         ; remove RF from stack 
           irx
getentry:  irx                         ; get entry address
           ldxa
           phi     ra
           ldx
           plo     ra
           irx                         ; recover quote flag
           ldx
           shr                         ; shift into DF
           ldi     0                   ; indicate entry is an atom
           sep     sret                ; and return to caller
islist:    irx                         ; recover quote flag
           ldx
           shr                         ; shift into DF
           ldi     2                   ; indicate a list was found
           sep     sret                ; and return to caller
eoflist:   irx                         ; recover quote flag
           ldx
           shr                         ; shift into DF
           ldi     3                   ; indicate a list was found
           sep     sret                ; and return to caller
nomore:    ldi     high oblist         ; point back to beginning of oblist
           phi     rd
           ldi     low oblist
           plo     rd
findbllp:  ldn     rd                  ; get entry
           lbnz    notempty            ; jump if not empty
           glo     rd                  ; save address
           stxd
           ghi     rd
           stxd
           ldi     1                   ; indicate used entry
           str     rd
           inc     rd                  ; point to name field
newnmlp:   ldn     rf                  ; get input character
           lbz     newnmdn             ; jump if done with new name
           smi     33                  ; white space is also a terminator
           lbnf    newnmdn
           ldn     rf                  ; so is a )
           smi     ')'
           lbz     newnmdn
           lda     rf                  ; otherwise get character
           str     rd                  ; and store into buffer
           inc     rd
           lbr     newnmlp             ; keep copying name
newnmdn:   ldi     0                   ; need terminator
           str     rd
           lbr     getentry            ; now get entry and return
notempty:  glo     rd                  ; point to next entry
           adi     16
           plo     rd
           ghi     rd
           adci    0
           phi     rd
           lbr     findbllp            ; and keep looking


; *****************************************
; *** Determine identity of an item     ***
; *** R7 - item to identify             ***
; *** Returns: DF=1 - item is a list    ***
; ***          DF=0 - item is an atom   ***
; *****************************************
ident:     ghi     r7                  ; check if in high memory
           ani     080h
           lbz     ident2
           ldi     0                   ; signify atomic
           shr
           sep     sret                ; and return
ident2:    ghi     r7                  ; store item address on stack
           stxd
           glo     r7
           stxd
           irx                         ; move back to low byte
           ldi     low nodelist        ; subtract item addr from nodelist
           sd
           irx                         ; point to high byte
           ldi     high nodelist       ; continue subtract
           sdb
           lbdf    identlst            ; if DF is set, then item is a list
           ldi     0                   ; indicate an atom
           shr
           sep     sret                ; return to caller
identlst:  ldi     1                   ; indicate a list
           shr
           sep     sret                ; and return

; *********************************
; *** Print out an s-expression ***
; *** R7 - node to print        ***
; *** RF - output buffer        ***
; *** RC=1 if number            ***
; *********************************
prtlist:   glo     r7                  ; check for NIL
           lbnz    prtnotn             ; jump if not
           ghi     r7
           lbnz    prtnotn
           ldi     'N'                 ; write nil to buffer
           str     rf
           inc     rf
           ldi     'I'
           str     rf
           inc     rf
           ldi     'L'
           str     rf
           inc     rf
           sep     sret                ; return to caller
prtnotn:   glo     r7                  ; check for T
           smi     0ffh
           lbnz    prtnott
           ghi     r7
           smi     0ffh
           lbnz    prtnott
           ldi     'T'
           str     rf
           inc     rf
           sep     sret                ; return to caller
prtnott:   ghi     rc                  ; see if result was a number
           smi     01
           lbnz    prtnotn1            ; jump if not numeric
           ghi     r7                  ; transfer number
           phi     rd
           glo     r7
           plo     rd
           sep     scall               ; convert for output
           dw      f_intout
           sep     sret                ; then return
prtnotn1:  sep     scall               ; identify type of object
           dw      ident
           lbdf    plislist            ; jump if it is a list
           sep     scall               ; display the object name
           dw      prtobent
           sep     sret                ; and return to caller
plislist:  ldi     '('                 ; put open parens into buffer
           str     rf
           inc     rf
prtnode:   ghi     r7                  ; move node
           phi     r8
           glo     r7
           plo     r8
; *** Process CAR of list ***
           ldn     r8                  ; get node flags
           stxd                        ; save a copy
           ani     4                   ; see if car is quoted
           lbz     carnoqt             ; jump if not
           ldi     027h                ; need quote char
           str     rf                  ; place into output
           inc     rf
carnoqt:   ldn     r8                  ; check for numeric
           ani     2
           lbz     carnonum            ; jump if not
           inc     r8                  ; point to car number
           lda     r8
           phi     rd
           lda     r8
           plo     rd
           sep     scall               ; convert to number
           dw      f_intout
           lbr     prtcdr              ; now print cdr
carnonum:  inc     r8                  ; move past node flags
           lda     r8                  ; get car address
           phi     r7
           lda     r8
           plo     r7
           lbnz    carnz               ; see if CAR is nil
           ghi     r7
           lbnz    carnz
           lbr     prtcdr              ; jump to cdr portion
carnz:     sep     scall               ; see if car is atom or list
           dw      ident
           lbdf    carislist           ; jump if car is a list
           sep     scall               ; print out the object
           dw      prtobent
           lbr     prtcdr              ; go to print cdr
carislist: glo     r8                  ; save current node address
           stxd
           ghi     r8
           stxd
           sep     scall               ; print the list
           dw      prtlist
           irx                         ; recover current address
           ldxa
           phi     r8
           ldx
           plo     r8
; *** Process CDR of list ***
prtcdr:    irx                         ; retrieve node flags
           ldx
           ani     16                  ; see if cdr is quoted
           lbz     cdrnoqt             ; jump if not
           ldi     027h                ; need quote char
           str     rf                  ; place into output
           inc     rf
cdrnoqt:   lda     r8                  ; get cdr
           phi     r7
           ldn     r8
           plo     r7
           lbnz    prtntend            ; jump if not at end
           ghi     r7                  ; check high byte as well
           lbnz    prtntend
prtisend:  dec     rf                  ; check to remove space
           ldn     rf
           smi     32
           lbz     prtnospc            ; remove the space
           inc     rf                  ; was not space, so restore pointer
prtnospc:  ldi     ')'                 ; write end of list char
           str     rf
           inc     rf
           sep     sret                ; and return to caller
prtntend:  sep     scall               ; is cdr atom or list
           dw      ident
           lbdf    cdrislist           ; jump if cdr is a list
           ldi     ' '                 ; atomic cdr means a dotted pair
           str     rf
           inc     rf
           ldi     '.'                 ; atomic cdr means a dotted pair
           str     rf
           inc     rf
           ldi     ' '                 ; atomic cdr means a dotted pair
           str     rf
           inc     rf
           sep     scall               ; print the object
           dw      prtobent
           lbr     prtisend            ; and then terminate
cdrislist: ldi     ' '                 ; need a space before continuing
           str     rf
           inc     rf
           lbr     prtnode             ; print next node in chain

; ***************************
; *** Print oblist entry  ***
; *** R7 - entry to print ***
; *** RF - output buffer  ***
; ***************************
prtobent:  inc     r7                  ; point to name portion of entry
prtoblp:   lda     r7                  ; get byte from oblist entry
           lbz     prtobdn             ; jump if done
           str     rf                  ; otherwise store into output
           inc     rf
           lbr     prtoblp             ; loop back for more characters
prtobdn:   sep     sret                ; return to caller

; *******************************************************************

; ****************************************
; *** Evaluate an s-expr               ***
; *** R7 - S-expr to evaluate          ***
; *** Returns: R7 - evaluated S-expr   ***
; ***               D=0 - atomic       ***
; ***               D=1 - list         ***
; ***               D=2 - number       ***
; ***          RC = 1 - number         ***
; ***          DF=1 - evaluation error ***
; ***                 D - error code   ***
; ****************************************
eval:      sep     scall               ; get identity of item
           dw      ident
           lbdf    evislist            ; jump if item is a list
           ldn     r7                  ; get flags
           stxd                        ; save a copy of the flags
           glo     r7                  ; point to binding
           adi     14
           plo     r7
           ghi     r7                  ; propagate the carry
           adci    0
           phi     r7
           lda     r7                  ; get binding
           phi     ra
           ldn     r7
           plo     ra
           irx                         ; recover flags
           ldx
           ani     2                   ; see if binding is numeric
           lbz     evnonum             ; jump if not
           ghi     ra                  ; transfer number to r7
           phi     r7
           glo     ra
           plo     r7
           ldi     4                   ; signal no error, and num
           shr
           ldi     1
           phi     rc
           sep     sret                ; and return to caller
evnonum:   ghi     ra                  ; transfer binding to r7
           phi     r7
           glo     ra
           plo     r7
           ldi     0                   ; indicate valid
           phi     rc                  ; indicate non-number
           shr
           sep     sret                ; and return
evislist:  ghi     r7                  ; transfer node to ra
           phi     ra
           glo     r7
           plo     ra
           ldn     ra                  ; get node flags
           ani     2                   ; see if car is numeric
           lbnz    everror             ; not a valid command
           ldn     ra                  ; retrrieve flags
           ani     4                   ; see if car is quoted
           lbnz    everror             ; error if so
           inc     ra                  ; get car
           lda     ra
           phi     r7
           ldn     ra
           plo     r7
           dec     ra                  ; move back to beginning of node
           dec     ra
           sep     scall               ; find type of car
           dw      ident
           lbdf    everror             ; error if car is a list
           ldn     r7                  ; get oblist flags
           ani     8                   ; see if built-in function
           lbnz    evprim              ; jump if so
           glo     r7                  ; move to binding
           adi     14
           plo     r9
           ghi     r7
           adci    0
           phi     r9
           lda     r9                  ; get binding
           phi     r7
           lbz     everror             ; high should not be zero
           lda     r9
           plo     r7
           sep     scall               ; get identity
           dw      ident
           lbnf    everror             ; error if atomic
           ghi     r7                  ; make a copy
           phi     r8
           glo     r7
           plo     r8
           sep     scall               ; get car
           dw      car
           glo     r7                  ; see if lambda object
           str     r2
           ldi     low oblist
           sm
           lbnz    everror             ; jump if error
           ghi     r7                  ; check high byte
           str     r2
           ldi     high oblist
           sm
           lbnz    everror             ; jump if not lambda
           ghi     ra                  ; get arguments node
           phi     r7
           glo     ra
           plo     r7
           sep     scall               ; get cdr
           dw      cdr
           ghi     r7                  ; move back to RA
           phi     ra
           glo     r7
           plo     ra
           ghi     r8                  ; move lambda node back
           phi     r7
           glo     r8
           plo     r7
           sep     scall               ; get the cdr
           dw      cdr
           lbr     flambda             ; execute the function
;evprim:    ldi     high jump           ; point to jump address
;           phi     rf
;           ldi     low jump
;           plo     rf
evprim:    ldi     0
           phi     rf
           plo     rf
           ldi     0c0h
           str     rf
           inc     rf
           glo     r7                  ; move to binding
           adi     14
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           lda     r7                  ; get routine address
           str     rf                  ; and store into jump address
           inc     rf
           lda     r7
           str     rf
           ghi     ra                  ; transfer node back to R7
           phi     r7
           glo     ra
           plo     r7
           lbr     0
;           db      0c0h                ; long jump to routine
;jump:      db      0,0
good:      ldi     0                   ; indicate no error
           shr
           sep     sret                ; and return to caller
bad:       shl                         ; add in error flag
           ori     1
           shr
           sep     sret                ; and return to caller

everror:   ldi     errinvcmd*2+1       ; indicate evaluation error
           shr
           sep     sret                ; and return to caller

; *********************************
; *** Get car of a node         ***
; *** R7 - node                 ***
; *** Returns: R7 - car of node ***
; ***          DF=1 if numeric  ***
; *********************************
car:       ldn     r7                  ; get flags
           shr                         ; shift numeric flag into df
           shr
           inc     r7                  ; move to car
           lda     r7                  ; get car
           plo     re
           ldn     r7
           plo     r7
           glo     re
           phi     r7
           sep     sret                ; return to caller

; *********************************
; *** Get cdr of a node         ***
; *** R7 - node                 ***
; *** Returns: R7 - car of node ***
; *********************************
cdr:       ldn     r7                  ; get flags
           shr                         ; shift cdr numeric flag into df
           shr
           shr
           shr
           inc     r7                  ; move to cdr
           inc     r7
           inc     r7
           lda     r7                  ; get cdr
           plo     re
           ldn     r7
           plo     r7
           glo     re
           phi     r7
           sep     sret                ; return to caller


; **************************************
; *** Lisp functions                 ***
; *** DF=1 on evaluation error       ***
; **************************************

; **************************************
; *** Get next argument              ***
; *** R7 - node                      ***
; *** Returns: R7 - node             ***
; ***          R8 - pointer for next ***
; ***          D=0 - no eval error   ***
; ***          D=1 - eval error      ***
; ***          RC.1=1 arg is numeric ***
; ***          DF=1 - arg is a list  ***
; **************************************
getarg:    sep     scall               ; get cdr of node
           dw      cdr
           ghi     r7                  ; save point to next arg
           phi     r8
           glo     r7
           plo     r8
           ldn     r7                  ; get node flags
           phi     rc                  ; save flags
           sep     scall               ; get car of new node
           dw      car
           lbdf    getargnum           ; prevent eval of numbers
           ghi     rc                  ; recover flags
           ani     4                   ; see if quoted
           lbnz    getargno            ; jump if no eval is needed
           glo     r8                  ; save R8 through the eval
           stxd
           ghi     r8
           stxd
           sep     scall               ; evaluate it
           dw      eval
           irx                         ; recover R8
           ldxa
           phi     r8
           ldx
           plo     r8
           lbnf    getargno            ; jump if no eval error
           ldi     1                   ; signal error
           sep     sret                ; and return
getargno:  sep     scall               ; identify result
           dw      ident
           ldi     0                   ; signal no error
           phi     rc                  ; signal nonnumeric
           sep     sret                ; return to caller
getargnum: ldi     1                   ; signal a number
           phi     rc
           ldi     0                   ; signal no error
           shr                         ; and nonlist
           sep     sret                ; return to caller
         
; **************************
; *** Get CAR of an item ***
; *** R7 - Item          ***
; *** Returns: R7 - car  ***
; **************************
fcar:      sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           lbnf    ffalse              ; signal nil if nonlist
           sep     scall               ; get cdr of node
           dw      car
           ldi     0                   ; signal non-numeric
           phi     rc
           lbnf    good                ; jump if not numeric
           ldi     1
           phi     rc                  ; signal numeric
           lbr     good                ; return with good result

; **************************
; *** Get CDR of an item ***
; *** R7 - Item          ***
; *** Returns: R7 - cdr  ***
; **************************
fcdr:      sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           lbnf    ffalse              ; signal nil if nonlist
           sep     scall               ; get cdr of node
           dw      cdr
           ldi     0
           shlc
           phi     rc
           lbr     good                ; return with good result

; ******************************
; *** Reverse a list         ***
; *** R7 - Treee             ***
; *** Returns: R7 - rev list ***
; ******************************
freverse:  sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           lbnf    ffalse              ; cannot reverse an atom
           ghi     r7                  ; transfer list pointer
           phi     r8
           glo     r7
           plo     r8
           sep     scall               ; get a free node
           dw      getfrnode
frevlp:    ghi     r8                  ; copy list address
           phi     ra
           glo     r8
           plo     ra
           inc     ra                  ; check for valid node
           lda     ra
           lbnz    frevgo
           lda     ra
           lbnz    frevgo
           lda     ra
           lbnz    frevgo
           lda     ra
           lbnz    frevgo
           ldi     0                   ; signal non-numeric
           phi     rc
           lbr     good                ; and return
frevgo:    ghi     r7                  ; move dest node
           phi     ra
           glo     r7
           plo     ra
           sep     scall
           dw      getfrnode           ; get a new node
           lda     r8                  ; get flags from source list
           str     r7                  ; write into destination
           inc     r7
           lda     r8                  ; then put node address
           str     r7
           inc     r7
           lda     r8
           str     r7
           inc     r7
           ghi     ra                  ; write last node as cdr
           str     r7
           inc     r7
           glo     ra
           str     r7
           dec     r7                  ; back to beginning of node
           dec     r7
           dec     r7
           dec     r7
           lda     r8                  ; retrieve cdr
           plo     re
           ldn     r8
           plo     r8
           glo     re
           phi     r8
           lbr     frevlp              ; loop back until done

; ****************************
; *** See if atom is bound ***
; *** R7 - Item            ***
; *** Returns: R7 - T/NIL  ***
; ****************************
fboundp:   sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           lbdf    bad                 ; cannot check bindings on lists
           glo     r7                  ; move to binding
           adi     14
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           lda     r7                  ; check for nonzero
           lbnz    ftrue
           ldn     r7
           lbnz    ftrue
           lbr     ffalse              ; otherwise false

; ****************************************
; *** Check two addresses for equality ***
; *** R7 - EQ s-expr                   ***
; *** Returns: R7 - boolean            ***
; ****************************************
feq:       sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           ghi     r7                  ; save this arg
           stxd
           glo     r7
           stxd
           ghi     r8                  ; get node for next arg
           phi     r7
           glo     r8
           plo     r7
           sep     scall               ; get next arg
           dw      getarg
           lbnz    feqerr              ; jump if error occured
feqno2:    irx                         ; move to previous address
           glo     r7                  ; subtract from current address
           sm
           plo     r7
           irx
           ghi     r7
           smb
           lbnz    eqnil               ; they were not a match
           glo     r7                  ; check low byte as well
           lbnz    eqnil
           ldi     0ffh                ; indicate truth
eqgo:      phi     r7
           plo     r7
           ldi     0                   ; signal non-numeric
           phi     rc
           lbr     good                ; and exit
eqnil:     ldi     0                   ; indicate nil
           lbr     eqgo                ; store and exit
feqerr:    irx                         ; remove node from stack
           irx
           lbr     erreverr            ; then end in error

; ****************************************
; *** add 2 numbers                    ***
; *** R7 - ADD s-expr                  ***
; *** Returns: R7 - result             ***
; ****************************************
fadd:      sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           ghi     r7                  ; save this arg
           stxd
           glo     r7
           stxd
           ghi     r8                  ; get node for next arg
           phi     r7
           glo     r8
           plo     r7
           sep     scall               ; get next arg
           dw      getarg
           lbnz    erreverr            ; jump if error occured
fadno2:    irx                         ; move to previous address
           glo     r7                  ; subtract from current address
           add
           plo     r7
           irx
           ghi     r7
           adc
           phi     r7
           ldi     1                   ; indicate answer is numeric
           phi     rc
           lbr     good

; ****************************************
; *** sub 2 numbers                    ***
; *** R7 - ADD s-expr                  ***
; *** Returns: R7 - result             ***
; ****************************************
fsub:      sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           ghi     r7                  ; save this arg
           stxd
           glo     r7
           stxd
           ghi     r8                  ; get node for next arg
           phi     r7
           glo     r8
           plo     r7
           sep     scall               ; get next arg
           dw      getarg
           lbnz    erreverr            ; jump if error occured
fsbno2:    irx                         ; move to previous address
           glo     r7                  ; subtract from current address
           sd
           plo     r7
           irx
           ghi     r7
           sdb
           phi     r7
           ldi     1                   ; indicate answer is numeric
           phi     rc
           lbr     good

; ****************************************
; *** compare 2 numbers                ***
; *** R7 - ADD s-expr                  ***
; *** Returns: R7 - result             ***
; ****************************************
flessp:    sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           ghi     r7                  ; save this arg
           stxd
           glo     r7
           stxd
           ghi     r8                  ; get node for next arg
           phi     r7
           glo     r8
           plo     r7
           sep     scall               ; get next arg
           dw      getarg
           lbnz    erreverr            ; jump if error occured
           irx                         ; move to previous address
           glo     r7                  ; subtract from current address
           sd
           irx
           ghi     r7
           sdb
           lbdf    ffalse
           lbr     ftrue

; ****************************************
; *** Create a consing node            ***
; *** R7 - EQ s-expr                   ***
; *** Returns: R7 - cons node          ***
; ****************************************
fcons:     sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           glo     r7                  ; save this arg
           stxd
           ghi     r7
           stxd
           ghi     rc                  ; save numeric flag
           stxd
           ghi     r8                  ; get node for next arg
           phi     r7
           glo     r8
           plo     r7
           sep     scall               ; get next arg
           dw      getarg
           lbnz    erreverr            ; jump if error occured
feqcons2:  ghi     r7                  ; move cdr
           phi     r8
           glo     r7
           plo     r8
           sep     scall               ; get a free node
           dw      getfrnode
           irx                         ; recover numeric flag
           ldxa
           shr                         ; see if numeric
           lbnf    fconsgo1            ; jump if not
           ldi     3                   ; signal car is numeric
           str     r7
fconsgo1:  inc     r7
           ldxa
           str     r7                  ; store into node
           inc     r7
           ldx
           str     r7
           ghi     rc                  ; see if cdr is numeric
           shr
           lbnf    fconsgo2            ; jump if not
           dec     r7                  ; point to flags
           dec     r7
           ldn     r7                  ; and set cdr numeric flag
           ori     8
           str     r7
           inc     r7                  ; move pointer back
           inc     r7
fconsgo2:  inc     r7
           ghi     r8                  ; store cdr into node
           str     r7
           inc     r7
           glo     r8
           str     r7
           dec     r7                  ; move back to beginning
           dec     r7
           dec     r7
           dec     r7
           ldi     0                   ; signal non-numeric
           phi     rc
           lbr     good                ; and done

; ****************************************
; *** bind an object                   ***
; *** R7 - EQ s-expr                   ***
; *** Returns: R7 - cons node          ***
; ****************************************
fset:      sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           lbdf    erreverr            ; first arg must be an atom
           ghi     rc                  ; and not a number
           shr
           lbdf    erreverr            ; jump if numeric
           glo     r7                  ; save this address
           stxd
           ghi     r7
           stxd
           ghi     r8                  ; transfer next arg address
           phi     r7
           glo     r8
           plo     r7
           sep     scall               ; get next argument
           dw      getarg
           lbnz    feqerr              ; jump on error
           irx                         ; recover atom address
           ldxa
           phi     ra
           ldx
           plo     ra
           ghi     rc                  ; see if binding is numeric
           shr
           lbnf    fsetnotnm           ; jump if not
           ldn     ra                  ; indicate numeric binding
           ori     2
           lbr     fsetgo
fsetnotnm: ldn     ra                  ; indicate nonnumber
           ani     0fdh
fsetgo:    str     ra
           glo     ra                  ; move to binding field
           adi     14
           plo     ra
           ghi     ra
           adci    0
           phi     ra
           ghi     r7                  ; write new binding
           str     ra
           inc     ra
           glo     r7
           str     ra
           lbr     good                ; finished

; ********************************
; *** Print value of an object ***
; *** R7 - Item                ***
; *** Returns: R7 - car        ***
; ********************************
fprint:    sep     scall               ; get cdr of node
           dw      cdr
           ldn     r7                  ; get node flags
           stxd                        ; save flags
           sep     scall               ; get car of new node
           dw      car
           irx
           ldx                         ; recover flags
           ani     4                   ; see if car is quoted
           lbnz    fprtnoev            ; jump if no eval is needed
           sep     scall               ; evaluate it
           dw      eval
           lbnf    fprtnoev            ; jump if no error
           ldi     erreverr            ; indicate an error
           lbr     bad                 ; and exit
fprtnoev:  ldi     high dta            ; area for print
           phi     rf
           ldi     low dta
           plo     rf
           glo     r7                  ; save expression
           stxd
           ghi     r7
           stxd
           sep     scall               ; print the s-expr
           dw      prtlist
           ldi     10                  ; add termination
           str     rf
           inc     rf
           ldi     13
           str     rf
           inc     rf
           ldi     0
           str     rf
           ldi     high dta            ; area for print
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           irx                         ; recover expr
           ldxa
           phi     r7
           ldx
           plo     r7
           ldi     0                   ; signal non-numeric
           phi     rc
           lbr     good                ; return with good result

; **************************************
; *** Determine if object is an atom ***
; *** R7 - Item                      ***
; *** Returns: R7 - car              ***
; **************************************
fatom:     sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           lbdf    ffalse              ; indicate false
ftrue:     ldi     0ffh                ; indicate truth
ftruego:   phi     r7
           plo     r7
           ldi     0                   ; signal non-numeric
           phi     rc
           lbr     good
ffalse:    ldi     0                   ; indicate false
           lbr     ftruego

; **************************************
; *** Evaluate an s-expr             ***
; *** R7 - Item                      ***
; *** Returns: R7 - car              ***
; **************************************
feval:     sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
fevalnoev: sep     scall               ; check identitiy
           dw      eval                ; evaluate it
           lbdf    bad                 ; jump if evaluation error
           lbr     good                ; otherwise good

; **************************************
; *** see if an expr is NIL          ***
; *** R7 - Item                      ***
; *** Returns: R7 - car              ***
; **************************************
fnull:     sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
fnullnoev: glo     r7                  ; see if result is null
           lbnz    ffalse              ; jump if not
           ghi     r7                  ; check high byte
           lbnz    ffalse
           lbr     ftrue               ; otherwise it is true

; ****************************************
; *** create a new list                ***
; *** R7 - EQ s-expr                   ***
; *** Returns: R7 - root node          ***
; ****************************************
flist:     ghi     r7                  ; save starting node
           phi     r8
           glo     r7
           plo     r8
           sep     scall               ; get a free node
           dw      getfrnode
           glo     r7                  ; save starting node
           stxd
           ghi     r7
           stxd
flistlp:   ghi     r7                  ; move node
           phi     ra
           glo     r7
           plo     ra
           ghi     r8                  ; get next arg address
           phi     r7
           glo     r8
           plo     r7

           glo     ra                  ; save RA
           stxd
           ghi     ra
           stxd
           sep     scall               ; get first argument
           dw      getarg
           irx                         ; recover RA
           ldxa
           phi     ra
           ldx
           plo     ra
           ghi     r8
           phi     rb
           glo     r8
           plo     rb
           inc     rb                  ; test for terminating node
           lda     rb
           lbnz    flistgd
           lda     rb
           lbnz    flistgd
           lda     rb
           lbnz    flistgd
           lda     rb
           lbnz    flistgd
           irx                         ; recover root node
           ldxa
           phi     r7
           ldx
           plo     r7
           ldi     0                   ; signal nonnumeric
           phi     rc
           lbr     good                ; and return
flistgd:   ghi     rc                  ; see if numeric
           shr
           lbnf    flistgo             ; jump if not
           ldi     3                   ; signal numeric car
           str     ra
           lbr     flistgo             ; then continue
flistgo:   inc     ra                  ; store into car
           ghi     r7
           str     ra
           inc     ra
           glo     r7
           str     ra
           inc     ra
           sep     scall               ; get another free node
           dw      getfrnode
           ghi     r7                  ; store into cdr
           str     ra
           inc     ra
           glo     r7
           str     ra
           lbr     flistlp             ; loop back for next entry

; ********************************
; *** Evaluate a COND form     ***
; *** R7 - s-expr of COND form ***
; *** Returns: R7 - result     ***
; ********************************
fcond:     sep     scall               ; get cdr of form
           dw      cdr
fcondlp1:  glo     r7                  ; save this position
           stxd
           ghi     r7
           stxd
           sep     scall               ; get car (conditional expr)
           dw      car
           ghi     r7                  ; make sure it is not zero
           lbnz    fcondgo1            ; jump if not
           glo     r7                  ; check low byte as well
           lbnz    fcondgo1
           irx                         ; remove node from stack
           irx
           ldi     0                   ; indicate false
           phi     r7
           plo     r7
           ldi     0                   ; signal non-numeric
           phi     rc
           lbr     good                ; return
fcondgo1:  glo     r7                  ; save this node
           stxd
           ghi     r7
           stxd
           sep     scall               ; and get car (condition)
           dw      car
           sep     scall               ; evaluate it
           dw      eval
           ghi     r7                  ; see if non-NIL
           lbnz    fcondgo2            ; it was not nil
           glo     r7                  ; check low byte as well
           lbnz    fcondgo2
           irx                         ; remove condition node
           irx
           irx                         ; retrieve upper node
           ldxa
           phi     r7
           ldx
           plo     r7
           sep     scall               ; get cdr of node
           dw      cdr
           lbr     fcondlp1            ; and loop to check next condiont
fcondgo2:  irx                         ; recover node
           ldxa
           phi     r7
           ldxa
           plo     r7
           irx                         ; remove upper node from stack
           sep     scall               ; get cdr
           dw      cdr
           sep     scall               ; and then the car
           dw      car
           sep     scall               ; evaluate s-expr
           dw      eval
           lbr     good                ; it was good

; *********************************
; *** Build a lambda expression ***
; *** R7 - s-expr               ***
; *** Returns: R7- lambda expr  ***
; *********************************
fdefun:    sep     scall               ; get cdr of node
           dw      cdr
           ghi     r7                  ; save this node 
           phi     r8
           glo     r7
           plo     r8
           sep     scall               ; get the car (function name)
           dw      car
           sep     scall               ; identify it
           dw      ident
           lbnf    defungo             ; jump if atomic
           ldi     erreverr            ; indicate evaluation error
           lbr     bad                 ; and return
defungo:   ldn     r7                  ; get object flags
           ori     4                   ; inidicate lambda expr
           str     r7
           glo     r7                  ; point to bind address
           adi     14
           plo     ra
           ghi     r7
           adci    0
           phi     ra
           sep     scall               ; get a free node
           dw      getfrnode
           ghi     r7                  ; write into oblist
           str     ra
           inc     ra
           glo     r7
           str     ra
           ldi     1                   ; indicate node is used
           str     r7
           inc     r7                  ; point to car
           ldi     high oblist       ; get lambda object
           str     r7                  ; store into node
           inc     r7
           ldi     low oblist
           str     r7
           inc     r7
           inc     r8                  ; point to cdr of defun tree
           inc     r8
           inc     r8
           lda     r8                  ; and store into cdr of lambda node
           str     r7
           inc     r7
           lda     r8
           str     r7
           dec     r7                  ; point back to beginning
           dec     r7
           dec     r7
           dec     r7
           ldi     0                   ; signal non-numeric
           phi     rc
           lbr     good                ; and then return

; **********************************************
; *** Evaluate a lambda form                 ***
; *** R7 - lambda form (car args, cdr evals) ***
; *** RA - list of arguments                 ***
; *** Returns: R7 -value                     ***
; **********************************************
flambda:   ghi     r7                  ; save root of lambda tree
           phi     rd
           glo     r7
           plo     rd
           sep     scall               ; get car (arg bindings)
           dw      car
           ldi     0                   ; setup arg count
           plo     rc
lamblp1:   glo     r7                  ; save this node position
           plo     rb
           ghi     r7
           phi     rb
           ldn     r7                  ; get flags from node
           phi     rc                  ; store here
           sep     scall               ; get car (argument object)
           dw      car
           glo     r7
           lbnz    lambgo1
           ghi     r7
           lbz     lambargd
lambgo1:   sep     scall               ; check identity
           dw      ident
           lbnf    lambgo2             ; jump if atomic
           lbr     bad                 ; args cannot be lists
lambgo2:   ldn     r7                  ; get binding flags
           stxd                        ; and store onto stack
           glo     r7                  ; move to binding
           adi     14
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           inc     r7                  ; point to lsb
           ldn     r7                  ; get lsb of bound value
           stxd                        ; store on stack
           dec     r7                  ; point to high byte
           ldn     r7                  ; get msb of bound value
           stxd                        ; store onto stack
           glo     r7                  ; save addrss onto stack
           stxd
           ghi     r7
           stxd
           inc     ra                  ; get car from actual args
           lda     ra
           phi     r7
           ldn     ra
           plo     r7
           dec     ra                  ; move back to flags
           dec     ra
           ldn     ra                  ; get flags from arg
           ani     2                   ; see if numeric
           lbnz    evalisnm
           ldn     ra                  ; get flags from arg
           ani     4                   ; see if quoted
           lbnz    noeval              ; jump if no need to eval
           ghi     rc                  ; recover var flags
           ani     4                   ; see if quoted
           lbnz    noeval              ; jump if no need to eval
           glo     ra                  ; save important registers
           stxd
           ghi     ra
           stxd
           glo     rb                  ; save important registers
           stxd
           ghi     rb
           stxd
           glo     rd                  ; save important registers
           stxd
           ghi     rd
           stxd
           glo     rc
           stxd
           sep     scall               ; evaluate argument
           dw      eval
           irx                         ; recover important registers
           ldxa
           plo     rc
           ldxa
           phi     rd
           ldxa
           plo     rd
           ldxa
           phi     rb
           ldxa
           plo     rb
           ldxa
           phi     ra
           ldx
           plo     ra
           ghi     rc                  ; see if evaluated to a number
           shr
           lbdf    evalisnm            ; jump if so
           lbr     noeval
noeval:    ldi     0                   ; indicate non numeric
           phi     rc
           lbr     noeval2
evalisnm:  ldi     1                   ; indicate a number
           phi     rc
noeval2:   irx                         ; retreive address from stack
           ldxa
           phi     r8
           ldx
           plo     r8
           stxd                        ; and keep on stack as well
           ghi     r8
           stxd
           ghi     r7                  ; store value into binding
           str     r8
           inc     r8
           glo     r7
           str     r8
           ghi     rc                  ; see if binding was numeric
           shr
           lbnf    noeval3             ; jump if not
           glo     r8                  ; move back to flags
           smi     15
           plo     r8
           ghi     r8
           smbi    0
           phi     r8
           ldn     r8                  ; and set numeric flag
           ori     2
           str     r8
noeval3:   inc     rc                  ; increment arg count
           inc     ra                  ; move to next value
           inc     ra
           inc     ra
           lda     ra                  ; get cdr
           plo     re
           ldn     ra
           plo     ra
           glo     re
           phi     ra                  ; ra now pointing at next value
           inc     rb                  ; move arg list to next arg
           inc     rb
           inc     rb
           lda     rb                  ; get cdr
           phi     r7
           ldn     rb
           plo     r7                  ; R7 now pointing at next arg
           lbr     lamblp1             ; process next arg
lambargd:  ghi     rd                  ; transfer Rd to R7
           phi     r7
           glo     rd
           plo     r7
           glo     rc                  ; put arg count onto stack
           stxd
lambmnlp:  sep     scall               ; get cdr (next inst)
           dw      cdr
           glo     r7                  ; save this position
           stxd
           ghi     r7
           stxd
           sep     scall               ; get car
           dw      car
           ghi     r7                  ; get address
           lbz     lambdone            ; jump if done processing
           sep     scall               ; evaluate it
           dw      eval
           ghi     r7                  ; save result
           phi     rd
           glo     r7
           plo     rd
           irx                         ; recover last position
           ldxa
           phi     r7
           ldx
           plo     r7
           lbr     lambmnlp            ; evaluate next form
lambdone:  irx                         ; remove last position from stack
           irx
           irx                         ; recover arg count
           ldx
           plo     rc
lambrstlp: glo     rc                  ; check for end
           lbz     lambfin             ; finished
           irx                         ; get address
           ldxa
           phi     ra
           ldxa
           plo     ra
           ldxa                        ; get previous value
           str     ra                  ; store into bind address
           inc     ra
           ldxa                        ; lsb of value
           str     ra
           glo     ra                  ; move back to flags byte
           smi     15
           plo     ra
           ghi     ra
           smbi    0
           phi     ra
           ldx                         ; recover bind flags from stack
           str     ra                  ; write into object entry
           dec     rc                  ; decrement count
           lbr     lambrstlp           ; loop back for rest
lambfin:   ghi     rd                  ; transfer result
           phi     r7
           glo     rd
           plo     r7
           lbr     good                ; and return


; **************************
; *** return value       ***
; *** R7 - Item          ***
; *** Returns: R7 - val  ***
; **************************
freturn:   sep     scall               ; get argument
           dw      getarg
           lbnz    erreverr            ; jump on evaluation error
           lbr     good

; *******************************************************************
; *** System functions                                            ***
; *******************************************************************

; ************************
; *** Show object list ***
; ************************
soblist:   ldi     high oblist         ; point to the object list
           phi     r7
           ldi     low oblist
           plo     r7
soblistlp: ldn     r7                  ; get flag byte
           lbz     soblistno           ; jump if not a valid entry
           smi     0ffh                ; check for end of list
           lbz     soblistdn           ; jump if done
           ghi     r7                  ; save this address
           phi     r8
           glo     r7
           plo     r8
           inc     r7                  ; move into name
           ldi     0                   ; set character count
           plo     rc
oblistlp1: lda     r7                  ; get byte from name
           lbz     obnmdn              ; jump if end of name found
           sep     scall               ; otherwise display it
           dw      f_type
           inc     rc                  ; increment character count
           lbr     oblistlp1           ; and finish name
obnmdn:    ldi     ' '                 ; need at least 1 space
           sep     scall               ; display it
           dw      f_type
           inc     rc
           glo     rc                  ; see if on columns
           smi     8
           lbz     obnmdn2             ; jump if so
           smi     8                   ; or on double column
           lbnz    obnmdn              ; need another space
obnmdn2:   ghi     r8                  ; recover address
           phi     r7
           glo     r8
           plo     r7
soblistno: glo     r7                  ; point to next entry
           adi     16
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           lbr     soblistlp
soblistdn: sep     scall               ; display a CR/LF
           dw      crlf
           lbr     mainlp              ; then back to main loop

; ************************
; *** Show free memory ***
; ************************
sfree:     ldi     high frobmsg        ; point to free oblist message
           phi     rf
           ldi     low frobmsg
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           ldi     0                   ; set count to zero
           phi     rd
           plo     rd
           ldi     high oblist         ; point to oblist
           phi     r7
           ldi     low oblist
           plo     r7
sfreelp1:  ldn     r7                  ; get byte
           lbz     sfreefr1            ; entry is available
           smi     0ffh                ; see if end of list
           lbz     sfreedn1            ; jump if so
sfreego1:  glo     r7                  ; point to next entry
           adi     16
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           lbr     sfreelp1            ; and check it
sfreefr1:  inc     rd                  ; increment free count
           lbr     sfreego1            ; and then continue checking
sfreedn1:  ldi     high dta            ; point to buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; convert count for output
           dw      f_intout
           ldi     0                   ; terminate it
           str     rf
           ldi     high dta            ; point to beginning of buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; display the number
           dw      f_msg
           sep     scall               ; and a crlf
           dw      crlf
           ldi     high frndmsg        ; point to free oblist message
           phi     rf
           ldi     low frndmsg
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           ldi     0                   ; set count to zero
           phi     rd
           plo     rd
           ldi     high nodelist       ; point to nodelist
           phi     r7
           ldi     low nodelist
           plo     r7
sfreelp2:  ldn     r7                  ; get flags
           lbz     sfreefr2            ; jump if it is free
sfreego2:  inc     r7                  ; point to next node
           inc     r7
           inc     r7
           inc     r7
           inc     r7
           ghi     r7                  ; check for top of memory
           smi     070h
           lbnz    sfreelp2            ; loop if more to check
           ldi     high dta            ; point to buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; convert number
           dw      f_intout
           ldi     0                   ; terminate the number
           str     rf
           ldi     high dta            ; point to buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; and display it
           dw      f_msg
           sep     scall               ; display a CR/LF
           dw      crlf
           lbr     mainlp              ; back to main loop
sfreefr2:  inc     rd                  ; increment the count
           lbr     sfreego2            ; and keep looking

; **********************************
; *** Perform garbage collection ***
; **********************************
sgc:       ldi     high msgndfree      ; display freed message
           phi     rf
           ldi     low msgndfree
           plo     rf
           sep     scall
           dw      f_msg
           sep     scall               ; invoke the garbage collector
           dw      gc
           ldi     high dta            ; point to buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; convert number
           dw      f_intout
           ldi     0                   ; add terminator
           str     rf
           ldi     high dta            ; point to buffer
           phi     rf
           ldi     low dta
           plo     rf
           sep     scall               ; display number
           dw      f_msg
           sep     scall               ; and a CR/LF
           dw      crlf
           lbr     mainlp              ; and then back to main loop

; ********************************
; *** Save bound atoms to disk ***
; ********************************
;ssave:     sep     scall               ; move past whitespace
;           dw      ltrim
;           ghi     rf                  ; put address into RA
;           phi     ra
;           glo     rf
;           plo     ra
;ssavenm:   ldn     ra                  ; need to find end of name
;           smi     33
;           lbnf    sfnend              ; jump if found end
;           inc     ra                  ; point to next char
;           lbr     ssavenm             ; and loop back
;sfnend:    ldi     0                   ; need valid terminator
;           str     ra
;           ldi     high fildes         ; point to file descriptor
;           phi     rd
;           ldi     low fildes
;           plo     rd
;           ldi     3                   ; create/truncate flags
;           plo     r7
;           sep     scall               ; open the file
;           dw      o_open
;           lbnf    sopened             ; jump if file was opened
;           ldi     high dskerr         ; display disk error message
;           phi     rf
;           ldi     low dskerr
;           plo     rf
;           sep     scall               ; display it
;           dw      f_msg
;           lbr     mainlp              ; and back to main loop
;sopened:   ldi     high oblistend      ; point to oblist
;           phi     r7
;           ldi     low oblistend
;           plo     r7
;fsavelp1:  ldn     r7                  ; get byte from oblist entry
;           lbz     fsavenxt            ; jump if not valid
;           smi     0ffh                ; check for end of list
;           lbz     fsavedn             ; jump if done
;           ghi     r7                  ; save a copy
;           phi     r8
;           glo     r7
;           plo     r8
;           sep     scall               ; move to bind address
;           dw      r7p16
;           dec     r7
;           ldn     r7                  ; get low byte
;           plo     ra
;           dec     r7                  ; get high byte
;           ldn     r7
;           phi     ra
;           ghi     r8                  ; recover original pointer
;           phi     r7
;           glo     r8
;           plo     r7
;           ghi     ra                  ; see if it is bound
;           lbnz    fsavebnd            ; jump if bound
;           glo     ra
;           lbz     fsavenxt            ; not bound, so next entry
;fsavebnd:  ldi     high dta            ; setup buffer
;           phi     rf
;           ldi     low dta
;           plo     rf
;           ldi     '('                 ; write initial chars
;           str     rf
;           inc     rf 
;           ldi     'S'
;           str     rf
;           inc     rf 
;           ldi     'E'
;           str     rf
;           inc     rf 
;           ldi     'T'
;           str     rf
;           inc     rf 
;           ldi     ' '
;           str     rf
;           inc     rf 
;           ldi     027h
;           str     rf
;           inc     rf 
;           glo     r7                  ; save oblist position
;           stxd
;           ghi     r7
;           stxd
;           sep     scall               ; write atom name
;           dw      prtobent
;           ldi     ' '                 ; next chars
;           str     rf
;           inc     rf 
;           ldi     027h
;           str     rf
;           inc     rf 
;           ghi     ra                  ; get binding
;           phi     r7
;           glo     ra
;           plo     r7
;           glo     rd                  ; save descriptor
;           stxd
;           ghi     rd
;           stxd
;           sep     scall               ; print the binding
;           dw      prtlist
;           irx                         ; recover descriptor
;           ldxa
;           phi     rd
;           ldx
;           plo     rd
;           ldi     ')'                 ; next chars
;           str     rf
;           inc     rf 
;           ldi     10
;           str     rf
;           inc     rf 
;           ldi     13
;           str     rf
;           inc     rf 
;           ldi     0
;           str     rf
;           ghi     rf                  ; move end to count
;           phi     rc
;           glo     rf
;           plo     rc
;           ldi     high dta            ; point back to buffer
;           phi     rf
;           ldi     low dta
;           plo     rf
;           str     r2                  ; subtract from count
;           glo     rc
;           sm
;           plo     rc
;           ghi     rf
;           str     r2
;           ghi     rc
;           smb
;           phi     rc
;           sep     scall
;           dw      o_write
;           irx                         ; recover oblist position
;           ldxa
;           phi     r7
;           ldx
;           plo     r7
;fsavenxt:  sep     scall               ; point to next entry
;           dw      r7p16
;           lbr     fsavelp1            ; and keep going
;
;fsavedn:   sep     scall               ; close the file
;           dw      o_close
;           lbr     mainlp              ; back to main loop


; ******************************
; *** Load s-exprs from disk ***
; ******************************
;sload:     sep     scall               ; move past whitespace
;           dw      ltrim
;           ghi     rf                  ; put address into RA
;           phi     ra
;           glo     rf
;           plo     ra
;sloadnm:   ldn     ra                  ; need to find end of name
;           smi     33
;           lbnf    lfnend              ; jump if found end
;           inc     ra                  ; point to next char
;           lbr     sloadnm             ; and loop back
;lfnend:    ldi     0                   ; need valid terminator
;           str     ra
;           ldi     high fildes         ; point to file descriptor
;           phi     rd
;           ldi     low fildes
;           plo     rd
;           ldi     0                   ; no flags
;           plo     r7
;           sep     scall               ; open the file
;           dw      o_open
;           lbnf    lopened             ; jump if file was opened
;           ldi     high dskerr         ; display disk error message
;           phi     rf
;           ldi     low dskerr
;           plo     rf
;           sep     scall               ; display it
;           dw      f_msg
;           lbr     mainlp              ; and back to main loop
;lopened:   ldi     high dta            ; point to transfer area
;           phi     rf
;           ldi     low dta
;           plo     rf
;           sep     scall               ; read a line
;           dw      readln
;           glo     rc                  ; see if chars were read
;           lbnz    loadgo              ; jump if so
;           ghi     rc
;           lbnz    loadgo
;           sep     scall               ; close the file
;           dw      o_close
;           lbr     mainlp              ; and back to main loop
;loadgo:    ldi     high dta            ; point to dta
;           phi     rf
;           ldi     low dta
;           plo     rf
;           sep     scall               ; trim leading whitespace
;           dw      ltrim
;           ldn     rf                  ; get first char
;           smi     '('                 ; should be an open
;           lbnz    lopened             ; jump if not
;           inc     rf                  ; move past opening paren
;           glo     rd                  ; save descriptor
;           stxd 
;           ghi     rd
;           stxd
;           sep     scall               ; bind the list
;           dw      bindlist
;           sep     scall               ; evaluate the s-expr
;           dw      eval
;           irx                         ; recover descriptor
;           ldxa
;           phi     rd
;           ldx
;           plo     rd
;           lbr     lopened             ; and loop back for more


; ****************************************
; *** Add 16 to R7 (oblist entry size) ***
; ****************************************
r7p16:     glo     r7
           adi     16
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           sep     sret
; *******************************************************************

; **********************************
; *** Get a free node            ***
; *** Returns: R7 - node address ***
; **********************************
getfrnode: ldi     high nodelist       ; point to node memory
           phi     r7
           ldi     low nodelist
           plo     r7
getfrlp:   ldn     r7                  ; get node flags byte
           lbz     getfound            ; found a free node
           inc     r7                  ; point to next node
           inc     r7
           inc     r7
           inc     r7
           inc     r7
           ghi     r7                  ; check for end
           smi     dtapage
           lbnz    getfrlp
           sep     scall               ; perform garbage collection
           dw      gc
           lbr     getfrnode           ; and try again
getfound:  ldi     1                   ; inidicate node is used
           str     r7
           inc     r7
           ldi     0                   ; rest of node is zeored
           str     r7
           inc     r7
           str     r7
           inc     r7
           str     r7
           inc     r7
           str     r7
           dec     r7                  ; back to beginning of node
           dec     r7
           dec     r7
           dec     r7
           sep     sret                ; return to caller

; *******************************************
; *** Clean up no longer bound nodes      ***
; *** Returns: RD - number of nodes freed ***
; *******************************************
gc:        ldi     high oblist         ; point to oblist
           phi     r7
           ldi     low oblist
           plo     r7
gclp1:     ldn     r7                  ; see if valid oblist entry
           lbz     gcno1               ; jump if not
           smi     0ffh                ; see if end of oblist
           lbz     gcdone1             ; phase 1 is done
           ghi     r7                  ; save oblist pointer
           phi     r8
           glo     r7
           plo     r8
           glo     r7                  ; move to bind value
           adi     14
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           lda     r7                  ; retrieve bind value
           plo     re
           lda     r7
           plo     r7
           glo     re
           phi     r7
           sep     scall               ; mark the chain as used
           dw      markused
           ghi     r8                  ; recover oblist pointer
           phi     r7
           glo     r8
           plo     r7
gcno1:     glo     r7                  ; point to next entry
           adi     16
           plo     r7
           ghi     r7
           adci    0
           phi     r7
           lbr     gclp1               ; and check next entry
gcdone1:   ldi     high nodelist       ; point to node memory
           phi     r7
           ldi     low nodelist
           plo     r7
           ldi     0                   ; zero node counter
           phi     rd
           plo     rd
gcloop2:   ldn     r7                  ; get flags
           lbz     gcnoent             ; jump if already empty node
           ani     080h                ; see if gc bit was set
           lbnz    gcset               ; jump if so
           ldi     0                   ; free the node
           str     r7
           inc     rd                  ; increment node count
           lbr     gcnoent             ; and then on to the next one
gcset:     ldn     r7                  ; reset gc bit
           ani     07fh
           str     r7
gcnoent:   inc     r7                  ; point to next node
           inc     r7
           inc     r7
           inc     r7
           inc     r7
           ghi     r7                  ; see if still in node memory
           smi     dtapage
           lbnz    gcloop2             ; loop until done
           sep     sret                ; return to caller

markused:  glo     r7                  ; see if we have a zero node
           lbnz    munonz              ; jump if not 
           ghi     r7                  ; check high byte as well
           lbnz    munonz
           sep     sret                ; done, return to caller
munonz:    sep     scall               ; identify object
           dw      ident
           lbdf    muislist            ; jump if have a list
           sep     sret                ; otherwise return
muislist:  ldn     r7                  ; get flags for node
           ori     080h                ; indicate node is valid
           str     r7                  ; write it
           glo     r7                  ; save node address
           stxd
           ghi     r7
           stxd
           sep     scall               ; get car of node
           dw      car
           sep     scall               ; and mark chain as used
           dw      markused
           irx                         ; recover node
           ldxa
           phi     r7
           ldx
           plo     r7
           sep     scall               ; get cdr
           dw      cdr
           lbr     markused            ; and continue marking

; *******************************
; *** Read in 1 line of text  ***
; *** RF - Buffer             ***
; *** RD - File descriptor    ***
; *** Returns: RC -count      ***
; ***          DF=1 - hit eof ***
; *******************************
;readln:    ldi     0                   ; set byte count
;           phi     rc
;           plo     rc
;readln1:   sep     scall               ; read a byte
;           dw      readbyte
;           lbdf    readlneof           ; jump on eof
;           plo     re                  ; keep a copy
;           smi     32                  ; look for anything below a space
;           lbnf    readln1
;readln2:   glo     re                  ; recover byte
;           str     rf                  ; store into buffer
;           inc     rf                  ; point to next position
;           inc     rc                  ; increment character count
;           sep     scall               ; read next byte
;           dw      readbyte
;           lbdf    readlneof           ; jump if end of file
;           plo     re                  ; keep a copy of read byte
;           smi     32                  ; make sure it is positive
;           lbdf    readln2             ; loop back on valid characters
;           ldi     0                   ; signal valid read
;readlncnt: shr                         ; shift into DF
;           sep     sret                ; and return to caller
;readlneof: ldi     1                   ; signal eof
;           lbr     readlncnt
;readbyte:  glo     rf
;           stxd    
;           ghi     rf
;           stxd    
;           glo     rc
;           stxd
;           ghi     rc
;           stxd
;           ldi     high buffer
;           phi     rf
;           ldi     low buffer
;           plo     rf
;           ldi     0
;           phi     rc
;           ldi     1
;           plo     rc
;           sep     scall
;           dw      o_read
;           glo     rc
;           lbz     readbno
;           ldi     0
;readbcnt:  shr
;           ldi     high buffer
;           phi     rf
;           ldi     low buffer
;           plo     rf
;           ldn     rf
;           plo     re
;           irx
;           ldxa
;           phi     rc
;           ldxa
;           plo     rc
;           ldxa
;           phi     rf
;           ldx
;           plo     rf
;           glo     re
;           sep     sret
;readbno:   ldi     1
;           lbr     readbcnt


crlf:      ldi     high crlfmsg        ; point to CR/LF message
           phi     rf
           ldi     low crlfmsg
           plo     rf
           sep     scall               ; display it
           dw      f_msg
           sep     sret                ; and return

crlfmsg:   db      10,13,0
prompt:    db      '>',0
errmsg:    db      'File not found',10,13,0
frobmsg:   db      'Free oblist entries :',0
frndmsg:   db      'Free nodes          :',0
msgndfree: db      'Nodes freed :',0
dskerr:    db      'Disk Error',10,13,10,13,0
syslist:   db      'OBLIS',('T'+80h)
           dw      soblist
;          db      'SAV',('E'+80h)
;          dw      ssave
;          db      'LOA',('D'+80h)
;          dw      sload
           db      'FRE',('E'+80h)
           dw      sfree
           db      'G',('C'+80h)
           dw      sgc
;          db      'BY',('E'+80h)
;          dw      o_wrmboot
           db      'CLEA',('R'+80h)
           dw      start
           db      0
;fildes:    db      0,0,0,0
;           dw      dta+0c00h
;           db      0,0
;           db      0
;           db      0,0,0,0
;           dw      0,0
;           db      0,0,0,0



; oblist flags
;   1 - Used entry
;   2 - Binding is numeric
;   4 - Lambda function
;   8 - inbuilt function
           org     $
oblist2:   db      9,'LAMBDA',0,0,0,0,0,0,0
           dw      bad
           db      9,'CAR',0,0,0,0,0,0,0,0,0,0
           dw      fcar
           db      9,'CDR',0,0,0,0,0,0,0,0,0,0
           dw      fcdr
           db      9,'EQ',0,0,0,0,0,0,0,0,0,0,0
           dw      feq
           db      9,'CONS',0,0,0,0,0,0,0,0,0
           dw      fcons
           db      9,'SET',0,0,0,0,0,0,0,0,0,0
           dw      fset
           db      9,'PRINT',0,0,0,0,0,0,0,0
           dw      fprint
           db      9,'ATOM',0,0,0,0,0,0,0,0,0
           dw      fatom
           db      9,'BOUNDP',0,0,0,0,0,0,0
           dw      fboundp
           db      9,'T',0,0,0,0,0,0,0,0,0,0,0,0
           dw      ftrue
           db      9,'NIL',0,0,0,0,0,0,0,0,0,0
           dw      ffalse
           db      9,'EVAL',0,0,0,0,0,0,0,0,0
           dw      feval
           db      9,'NULL',0,0,0,0,0,0,0,0,0
           dw      fnull
           db      9,'LIST',0,0,0,0,0,0,0,0,0
           dw      flist
           db      9,'REVERSE',0,0,0,0,0,0
           dw      freverse
           db      9,'COND',0,0,0,0,0,0,0,0,0
           dw      fcond
           db      9,'ADD',0,0,0,0,0,0,0,0,0,0
           dw      fadd
           db      9,'SUB',0,0,0,0,0,0,0,0,0,0
           dw      fsub
           db      9,'LESSP',0,0,0,0,0,0,0,0
           dw      flessp
           db      9,'DEFUN',0,0,0,0,0,0,0,0
           dw      fdefun
           db      9,'RETURN',0,0,0,0,0,0,0
           dw      freturn
;oblistend: db      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;           ds      200*16

; Node Format
; 1 byte Flags
;   1 - Node used
;   2 - CAR is numeric
;   4 - CAR is quoted
;   8 - CDR is numeric
;  16 - CDR is quoted
; 2 bytes CAR address
; 2 bytes CDR address

