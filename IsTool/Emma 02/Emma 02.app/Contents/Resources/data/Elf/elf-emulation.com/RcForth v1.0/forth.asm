include    bios.inc

;  R2   - program stack
;  R3   - Main PC
;  R4   - standard call
;  R5   - standard ret
;  R6   - used by Scall/Sret linkage
;  R9   - Data segment

biosv:     equ     rb
scall:     equ     r4
sret:      equ     r5
T_NUM:     equ     255
T_ASCII:   equ     254
FWHILE:    equ     81h
FREPEAT:   equ     FWHILE+1
FIF:       equ     FREPEAT+1
FELSE:     equ     FIF+1
FTHEN:     equ     FELSE+1
FVARIABLE: equ     FTHEN+1
FCOLON:    equ     FVARIABLE+1
FSEMI:     equ     FCOLON+1
FDUP:      equ     FSEMI+1
FDROP:     equ     FDUP+1
FSWAP:     equ     FDROP+1
FPLUS:     equ     FSWAP+1
FMINUS:    equ     FPLUS+1
FMUL:      equ     FMINUS+1
FDIV:      equ     FMUL+1
FDOT:      equ     FDIV+1
FUDOT:     equ     FDOT+1
FI:        equ     FUDOT+1
FAND:      equ     FI+1
FOR:       equ     FAND+1
FXOR:      equ     FOR+1
FCR:       equ     FXOR+1
FMEM:      equ     FCR+1
FDO:       equ     FMEM+1
FLOOP:     equ     FDO+1
FPLOOP:    equ     FLOOP+1
FEQUAL:    equ     FPLOOP+1
FUNEQUAL:  equ     FEQUAL+1
FBEGIN:    equ     FUNEQUAL+1
FUNTIL:    equ     FBEGIN+1
FRGT:      equ     FUNTIL+1
FGTR:      equ     FRGT+1
FWORDS:    equ     FGTR+1
FEMIT:     equ     FWORDS+1
FDEPTH:    equ     FEMIT+1
FROT:      equ     FDEPTH+1
FMROT:     equ     FROT+1
FOVER:     equ     FMROT+1
FAT:       equ     FOVER+1
FEXCL:     equ     FAT+1
FCAT:      equ     FEXCL+1
FCEXCL:    equ     FCAT+1
FDOTQT:    equ     FCEXCL+1
FKEY:      equ     FDOTQT+1
FALLOT:    equ     FKEY+1
FERROR:    equ     FALLOT+1
FSEE:      equ     FERROR+1
FFORGET:   equ     FSEE+1

           org     0
           ldi     high start          ; get address of main start
           phi     r3                  ; and place into standard PC
           ldi     low start
           plo     r3
           sep     r3                  ; transfer control to main PC
himem:     dw      0                   ; highest available memory
rstack:    dw      0                   ; bottom of return stack
tos:       dw      0                   ; topmost memory value for forth stack
freemem:   dw      0                   ; next free memory
fstack:    dw      0                   ; forth stack pointer
start:     sex     r2                  ; set X to stack
           ldi     high call           ; get address of standard call
           phi     r4                  ; place into r4
           ldi     low call
           plo     r4
           ldi     high ret            ; get address of standard return
           phi     r5
           ldi     low ret
           plo     r5
           ldi     high storage        ; get temporary stack
           phi     r2
           ldi     low storage
           plo     r2

           ldi     50h
           phi     r2
           ldi     0
           plo     r2
           sep     scall               ; call bios to set terminal speed
           dw      f_setbd             ; function to set terminal
           ghi     r3                  ; get page of data segment
           phi     r9                  ; place into r9

           ldi     high hello          ; address of signon message
           phi     rf                  ; place into r6
           ldi     low hello
           plo     rf
           sep     scall               ; call bios to display message
           dw      f_msg               ; function to display a message

; ************************************************
; **** Determine how much memory is installed ****
; ************************************************
           ldi     low freemem         ; free memory pointer
           plo     r9                  ; place into date pointer
           ldi     high progend        ; get end of program address
           str     r9                  ; write to free memory pointer
           inc     r9                  ; point to next position
           phi     rb                  ; place into r6
           phi     r2                  ; also use as initial stack
           ldi     low progend
           str     r9                  ; write to free memory pointer
           plo     rb
           plo     r2
           inc     rb
           inc     rb
memlp:     ldi     0                   ; get a zero
           str     rb                  ; write to memory
           ldn     rb                  ; recover retrieved byte
           bnz     memdone             ; jump if not same
           ldi     255                 ; another value
           str     rb                  ; write to memory
           ldn     rb                  ; retrieve it
           smi     255                 ; compare against written
           bnz     memdone             ; jump if not same
           inc     rb                  ; incrment pointer
           br      memlp               ; and keep looking
memdone:   dec     rb                  ; move back into writable memory
           ldi     low himem           ; memory pointer
           plo     r9                  ; place into r9
           ghi     rb                  ; get high of last memory
           str     r9                  ; write to data
           phi     r2                  ; and to machine stack
           inc     r9                  ; point to low byte
           glo     rb                  ; get low of himem
           str     r9                  ; and store
           plo     r2
           ldi     low rstack          ; get return stack address
           plo     r9                  ; select in data segment
           ghi     rb                  ; get hi memory
           smi     1                   ; 1 page lower for forth stack
           str     r9                  ; write to pointer
           inc     r9                  ; point to low byte
           glo     rb                  ; get low byte
           str     r9                  ; and store
           ldi     low tos             ; get stack address
           plo     r9                  ; select in data segment
           ghi     rb                  ; get hi memory
           smi     2                   ; 2 page lower for forth stack
           str     r9                  ; write to pointer
           inc     r9                  ; point to low byte
           glo     rb                  ; get low byte
           str     r9                  ; and store
           ldi     low fstack          ; get stack address
           plo     r9                  ; select in data segment
           ghi     rb                  ; get hi memory
           smi     2                   ; 2 page lower for forth stack
           str     r9                  ; write to pointer
           inc     r9                  ; point to low byte
           glo     rb                  ; get low byte
           str     r9                  ; and store

; *************************
; *** Main program loop ***
; *************************
mainlp:    ldi     high prompt         ; address of prompt
           phi     rf                  ; place into r6
           ldi     low prompt
           plo     rf
           sep     scall               ; display prompt
           dw      f_msg               ; function to display a message
           ldi     high buffer         ; point to input buffer
           phi     rf
           ldi     low buffer
           plo     rf
           sep     scall               ; read a line
           dw      f_input             ; function to read a line
           ldi     high crlf           ; address of CR/LF
           phi     rf                  ; place into r6
           ldi     low crlf  
           plo     rf
           sep     scall               ; call bios
           dw      f_msg               ; function to display a message
           sep     scall               ; call tokenizer
           dw      tknizer

       ldi     low freemem         ; get free memory pointer
       plo     r9                  ; place into data segment
       lda     r9                  ; get free memory pointer
       phi     rb                  ; place into rF
       ldn     r9
       plo     rb
   inc rb
   inc rb
       sep     scall
       dw      exec


           br      mainlp              ; return to beginning of main loop

; **************************************
; *** Display a character, char in D ***
; **************************************
disp:      sep     scall               ; call bios
           dw      f_type              ; function to type a charactr
           sep     sret                ; return to caller

; ********************************
; *** Read a key, returns in D ***
; ********************************
getkey:    sep     scall               ; call bios
           dw      f_read              ; function to read a key
           sep     sret                ; return to caller

; ***************************************************
; *** Function to retrieve value from forth stack ***
; *** Returns R[B] = value                        ***
; ***         DF=0 no error, DF=1 error           ***
; ***************************************************
pop:       sex     r2                  ; be sure x points to stack
           ldi     low fstack          ; get stack address
           plo     r9                  ; select in data segment
           lda     r9
           phi     ra
           ldn     r9
           plo     ra
           ldi     low tos             ; pointer to maximum stack value
           plo     r9                  ; put into data frame
           lda     r9                  ; get high value
           str     r2                  ; place into memory
           ghi     ra                  ; get high byte of forth stack
           sm                          ; check if same
           bnz     stackok             ; jump if ok
           ldn     r9                  ; get low byte of tos
           str     r2
           glo     ra                  ; check low byte of stack pointer
           sm
           bnz     stackok             ; jump if ok
           ldi     1                   ; signal error
popret:    shr                         ; shift status into DF
           sep     sret                ; return to caller
stackok:   inc     ra                  ; point to high byte
           lda     ra                  ; get it
           phi     rb                  ; put into r6
           ldn     ra                  ; get low byte
           plo     rb
           ldi     low fstack          ; get stack address
           plo     r9                  ; select in data segment
           ghi     ra                  ; get hi memory
           str     r9                  ; write to pointer
           inc     r9                  ; point to low byte
           glo     ra                  ; get low byte
           str     r9                  ; and store
           ldi     0                   ; signal no error
           br      popret              ; and return to caller

; ********************************************************
; *** Function to push value onto stack, value in R[B] ***
; ********************************************************
push:      ldi     low fstack          ; get stack address
           plo     r9                  ; select in data segment
           lda     r9
           phi     ra
           ldn     r9
           plo     ra
           glo     rb                  ; get low byte of value
           str     ra                  ; store on forth stack
           dec     ra                  ; point to next byte
           ghi     rb                  ; get high value
           str     ra                  ; store on forth stack
           dec     ra                  ; point to next byte
           ldi     low fstack          ; get stack address
           plo     r9                  ; select in data segment
           ghi     ra                  ; get hi memory
           str     r9                  ; write to pointer
           inc     r9                  ; point to low byte
           glo     ra                  ; get low byte
           str     r9                  ; and store
           sep     sret                ; return to caller

; ****************************************************
; *** Function to retrieve value from return stack ***
; *** Returns R[B] = value                         ***
; ***         D=0 no error, D=1 error              ***
; ****************************************************
rpop:      sex     r2                  ; be sure x points to stack
           ldi     low rstack          ; get stack address
           plo     r9                  ; select in data segment
           lda     r9
           phi     ra
           ldn     r9
           plo     ra
           inc     ra                  ; point to high byte
           lda     ra                  ; get it
           phi     rb                  ; put into r6
           ldn     ra                  ; get low byte
           plo     rb
           ldi     low rstack          ; get stack address
           plo     r9                  ; select in data segment
           ghi     ra                  ; get hi memory
           str     r9                  ; write to pointer
           inc     r9                  ; point to low byte
           glo     ra                  ; get low byte
           str     r9                  ; and store
           ldi     0                   ; signal no error
           sep     sret                ; and return

; ***************************************************************
; *** Function to push value onto return stack, value in R[B] ***
; ***************************************************************
rpush:     ldi     low rstack          ; get stack address
           plo     r9                  ; select in data segment
           lda     r9
           phi     ra
           ldn     r9
           plo     ra
           glo     rb                  ; get low byte of value
           str     ra                  ; store on forth stack
           dec     ra                  ; point to next byte
           ghi     rb                  ; get high value
           str     ra                  ; store on forth stack
           dec     ra                  ; point to next byte
           ldi     low rstack          ; get stack address
           plo     r9                  ; select in data segment
           ghi     ra                  ; get hi memory
           str     r9                  ; write to pointer
           inc     r9                  ; point to low byte
           glo     ra                  ; get low byte
           str     r9                  ; and store
           sep     sret                ; return to caller

           org     200h 
; ********************************************
; *** Function to find stored name address ***
; ***  Needs: name to search in R[8]       ***
; ***  returns: R[B] first byte in data    ***
; ***           R[7] Address of descriptor ***
; ***           R[8] first addr after name ***
; ***           DF = 1 if not found        ***
; ********************************************
findname:  ldi     high storage        ; get address of stored data
           phi     rb                  ; put into r6
           ldi     low storage
           plo     rb
           sex     r2                  ; make sure X points to stack
findlp:    ghi     rb                  ; copy address
           phi     r7
           glo     rb
           plo     r7
           lda     rb                  ; get link address
           bnz     findgo              ; jump if nonzero
           ldn     rb                  ; get low byte
           bnz     findgo              ; jump if non zero
           ldi     1                   ; not found
findret:   shr                         ; set DF
           sep     sret                ; and return to caller
findgo:    inc     rb                  ; pointing now at type
           inc     rb                  ; pointing at ascii indicator
           inc     rb                  ; first byte of name
           glo     r8                  ; save requested name
           stxd
           ghi     r8
           stxd
findchk:   ldn     r8                  ; get byte from requested name
           str     r2                  ; place into memory
           ldn     rb                  ; get byte from descriptor
           sm                          ; compare equality
           bnz     findnext            ; jump if not found
           ldn     r8                  ; get byte
           bz      findfound           ; entry is found
           inc     r8                  ; increment positions
           inc     rb
           br      findchk             ; and keep looking
findfound: inc     rb                  ; r6 now points to data
           irx                         ; remove r8 from stack
           irx
           inc     r8                  ; move past terminator in name
           ldi     0                   ; signal success
           br      findret             ; and return to caller
findnext:  irx                         ; recover start of requested name
           ldxa
           phi     r8
           ldx
           plo     r8
           lda     r7                  ; get next link address
           phi     rb
           ldn     r7
           plo     rb
           br      findlp              ; and check next entry

mul16:     ldi     0                   ; zero out total
           phi     r8
           plo     r8
           phi     rc
           plo     rc
           sex     r2                  ; make sure X points to stack
mulloop:   glo     r7                  ; get low of multiplier
           bnz     mulcont             ; continue multiplying if nonzero
           ghi     r7                  ; check hi byte as well
           bnz     mulcont
           ghi     r8                  ; transfer answer
           phi     rb
           glo     r8
           plo     rb
           sep     sret                ; return to caller
mulcont:   ghi     r7                  ; shift multiplier
           shr
           phi     r7
           glo     r7
           shrc
           plo     r7
           bnf     mulcont2            ; loop if no addition needed
           glo     rb                  ; add 6 to 8
           str     r2
           glo     r8
           add
           plo     r8
           ghi     rb
           str     r2
           ghi     r8
           adc
           phi     r8
           glo     rc                  ; carry into high word
           adci    0
           plo     rc
           ghi     rc
           adci    0
           phi     rc
mulcont2:  glo     rb                  ; shift first number
           shl
           plo     rb
           ghi     rb
           shlc
           phi     rb
           br      mulloop             ; loop until done

; *** RC = RB/R7
; *** RB = remainder
; *** uses R8 and R9
div16:     ldi     0                   ; clear answer 
           phi     rc
           plo     rc
           phi     r8                  ; set additive
           plo     r8
           inc     r8
           glo     r7                  ; check for divide by 0
           bnz     d16lp1
           ghi     r7
           bnz     d16lp1
           ldi     0ffh                ; return 0ffffh as div/0 error
           phi     rc
           plo     rc
           sep     sret                ; return to caller
d16lp1:    ghi     r7                  ; get high byte from r7
           ani     128                 ; check high bit 
           bnz     divst               ; jump if set
           glo     r7                  ; lo byte of divisor
           shl                         ; multiply by 2
           plo     r7                  ; and put back
           ghi     r7                  ; get high byte of divisor
           shlc                        ; continue multiply by 2
           phi     r7                  ; and put back
           glo     r8                  ; multiply additive by 2
           shl     
           plo     r8
           ghi     r8
           shlc
           phi     r8
           br      d16lp1              ; loop until high bit set in divisor
divst:     glo     r7                  ; get low of divisor
           bnz     divgo               ; jump if still nonzero
           ghi     r7                  ; check hi byte too
           bnz     divgo
           sep     sret                ; jump if done
divgo:     ghi     rb                  ; copy dividend
           phi     r9
           glo     rb
           plo     r9
           glo     r7                  ; get lo of divisor
           stxd                        ; place into memory
           irx                         ; point to memory
           glo     rb                  ; get low byte of dividend
           sm                          ; subtract
           plo     rb                  ; put back into r6
           ghi     r7                  ; get hi of divisor
           stxd                        ; place into memory
           irx                         ; point to byte
           ghi     rb                  ; get hi of dividend
           smb                         ; subtract
           phi     rb                  ; and put back
           bdf     divyes              ; branch if no borrow happened
           ghi     r9                  ; recover copy
           phi     rb                  ; put back into dividend
           glo     r9
           plo     rb
           br      divno               ; jump to next iteration
divyes:    glo     r8                  ; get lo of additive
           stxd                        ; place in memory
           irx                         ; point to byte
           glo     rc                  ; get lo of answer
           add                         ; and add
           plo     rc                  ; put back
           ghi     r8                  ; get hi of additive
           stxd                        ; place into memory
           irx                         ; point to byte
           ghi     rc                  ; get hi byte of answer
           adc                         ; and continue addition
           phi     rc                  ; put back
divno:     ghi     r7                  ; get hi of divisor
           shr                         ; divide by 2
           phi     r7                  ; put back
           glo     r7                  ; get lo of divisor
           shrc                        ; continue divide by 2
           plo     r7
           ghi     r8                  ; get hi of divisor
           shr                         ; divide by 2
           phi     r8                  ; put back
           glo     r8                  ; get lo of divisor
           shrc                        ; continue divide by 2
           plo     r8
           br      divst               ; next iteration

           org     300h
; ***************************
; *** Setup for tokenizer ***
; ***************************
tknizer:   ldi     high buffer         ; point to input buffer
           phi     rb
           ldi     low buffer
           plo     rb
           ldi     low freemem         ; get free memory pointer
           plo     r9                  ; place into data segment
           lda     r9                  ; get free memory pointer
           phi     rf                  ; place into rF
           ldn     r9
           plo     rf
   inc rf
   inc rf
           sex     r2                  ; make sure x is pointing to stack

; ******************************
; *** Now the tokenizer loop ***
; ******************************
tokenlp:   ldn     rb                  ; get byte from buffer
           bz      tokendn             ; jump if found terminator
           smi     (' '+1)             ; check for whitespace
           bdf     nonwhite            ; jump if not whitespace
           inc     rb                  ; move past white space
           br      tokenlp             ; and keep looking

; ********************************************
; *** Prepare to check against token table ***
; ********************************************
nonwhite:  ldi     high cmdTable       ; point to comand table
           phi     r7                  ; r7 will be command table pointer
           ldi     low cmdTable
           plo     r7
           ldi     1                   ; first command number
           plo     r8                  ; r8 will keep track of command number
; **************************
; *** Command check loop ***
; **************************
cmdloop:   ghi     rb                  ; save buffer address
           phi     rc
           glo     rb
           plo     rc
; ************************
; *** Check next token ***
; ************************
tokloop:   ldn     r7                  ; get byte from token table
           ani     128                 ; check if last byte of token
           bnz     cmdend              ; jump if last byte
           ldn     r7                  ; reget token byte
           str     r2                  ; store to stack
           ldn     rb                  ; get byte from buffer
           sm                          ; do bytes match?
           bnz     toknomtch           ; jump if no match
           inc     r7                  ; incrment token pointer
           inc     rb                  ; increment buffer pointer
           br      tokloop             ; and keep looking
; *********************************************************
; *** Token failed match, move to next and reset buffer ***
; *********************************************************
toknomtch: ghi     rc                  ; recover saved address
           phi     rb
           glo     rc
           plo     rb
nomtch1:   ldn     r7                  ; get byte from token
           ani     128                 ; looking for last byte of token
           bnz     nomtch2             ; jump if found
           inc     r7                  ; point to next byte
           br      nomtch1             ; and keep looking
nomtch2:   inc     r7                  ; point to next token
           inc     r8                  ; increment command number
           ldn     r7                  ; get next token byte
           bnz     cmdloop             ; jump if more tokens to check
           br      notoken             ; jump if no token found
; ***********************************************************
; *** Made it to last byte of token, check remaining byte ***
; ***********************************************************
cmdend:    ldn     r7                  ; get byte fro token
           ani     07fh                ; strip off end code
           str     r2                  ; save to stack
           ldn     rb                  ; get byte from buffer
           sm                          ; do they match
           bnz     toknomtch           ; jump if not
           inc     rb                  ; point to next byte
           ldn     rb                  ; get it
           smi     (' '+1)             ; it must be whitespace
           bdf     toknomtch           ; otherwise no match
; *************************************************************
; *** Match found, store command number into command buffer ***
; *************************************************************
           glo     r8                  ; get command number
           ori     128                 ; set high bit
           str     rf                  ; write to command buffer
           inc     rf                  ; point to next position
           smi     FDOTQT              ; check for ." function
           bnz     tokenlp             ; jump if not
           inc     rb                  ; move past first space
           ldi     T_ASCII             ; need an ascii token
tdotqtlp:  str     rf                  ; write to command buffer
           inc     rf
           ldn     rb                  ; get next byte
           smi     34                  ; check for end quote
           bz      tdotqtdn            ; jump if found
           lda     rb                  ; transfer character to code
           br      tdotqtlp            ; and keep looking
tdotqtdn:  ldn     rb                  ; retrieve quote
           str     rf                  ; put quote into output
           inc     rf
           ldi     0                   ; need string terminator
           str     rf
           inc     rf
           inc     rb                  ; move past quote
           br      tokenlp             ; then continue tokenizing
         



notoken:   ldn     rb                  ; get byte
           smi     '0'                 ; check for below numbers
           bnf     nonnumber           ; jump if not a number
           ldn     rb
           smi     ('9'+1)
           bdf     nonnumber
           ghi     rb                  ; save pointer in case of bad number
           phi     rc
           glo     rb
           plo     rc
; **********************
; *** Found a number ***
; **********************
isnumber:  ldi     0                   ; number starts out as zero
           phi     r7                  ; use r7 to compile number
           plo     r7
           sex     r2                  ; make sure x is pointing to stack
numberlp:  ghi     r7                  ; copy number to temp
           phi     r8
           glo     r7
           plo     r8
           glo     r7                  ; mulitply by 2
           shl
           plo     r7
           ghi     r7
           shlc
           phi     r7
           glo     r7                  ; mulitply by 4
           shl
           plo     r7
           ghi     r7
           shlc
           phi     r7
           glo     r8                  ; multiply by 5
           str     r2
           glo     r7
           add
           plo     r7
           ghi     r8
           str     r2
           ghi     r7
           adc
           phi     r7
           glo     r7                  ; mulitply by 10
           shl
           plo     r7
           ghi     r7
           shlc
           phi     r7
           lda     rb                  ; get byte from buffer
           smi     '0'                 ; convert to numeric
           str     r2                  ; store it
           glo     r7                  ; add to number
           add
           plo     r7
           ghi     r7                  ; propate through high byte
           adci    0
           phi     r7
           ldn     rb                  ; get byte
           smi     (' '+1)             ; check for space
           bnf     numberdn            ; number also done
           ldn     rb
           smi     '0'                 ; check for below numbers
           bnf     numbererr           ; jump if not a number
           ldn     rb
           smi     ('9'+1)
           bdf     numbererr
           br      numberlp            ; get rest of number
numbererr: ghi     rc                  ; recover address
           phi     rb
           glo     rc
           plo     rb
           br      nonnumber
numberdn:  ldi     T_NUM               ; code to signify a number
           str     rf                  ; write to code buffer
           inc     rf                  ; point to next position
           ghi     r7                  ; get high byte of number
           str     rf                  ; write to code buffer
           inc     rf                  ; point to next position
           glo     r7                  ; get lo byte of numbr
           str     rf                  ; write to code buffer
           inc     rf                  ; point to next position
           br      tokenlp             ; continue reading tokens

; *************************************************************
; *** Neither token or number found, insert as ascii string ***
; *************************************************************
nonnumber: ldi     T_ASCII             ; indicate ascii to follow
           dec     rb                  ; account for first increment
notokenlp: str     rf                  ; write to buffer
           inc     rf                  ; advance to next position
           inc     rb                  ; point to next position
           ldn     rb                  ; get next byte
           smi     (' '+1)             ; check for whitespace
           bnf     notokwht            ; found whitespace
           ldn     rb                  ; get byte
           br      notokenlp           ; get characters til whitespace
notokwht:  ldi     0                   ; need ascii terminator
           str     rf                  ; store into buffer
           inc     rf                  ; point to next position
           br      tokenlp             ; and keep looking
tokendn:   ldi     0                   ; need to terminate command string
           str     rf                  ; write to buffer
           sep    sret                 ; return to caller
 
           org     400h
uintout:   br      positive
intout:    sex     r2                  ; point X to stack
           ghi     rb                  ; get high of number
           ani     128                 ; mask all bit sign bit
           bz      positive            ; jump if number is positive
           ldi     '-'                 ; need a minus sign
           sep     scall
           dw      disp
           glo     rb                  ; get low byte
           str     r2                  ; store it
           ldi     0                   ; need to subtract from 0
           sm
           plo     rb                  ; put back
           ghi     rb                  ; get high byte
           str     r2                  ; place into memory
           ldi     0                   ; still subtracting from zero
           smb
           phi     rb                  ; and put back
positive:  ldi     27h                 ; hi byte of 10000
           phi     r7                  ; place into subtraction
           ldi     10h                 ; lo byte of 10000
           plo     r7
           ldi     0                   ; leading zero flag
           stxd                        ; store onto stack
nxtiter:   ldi     0                   ; star count at zero
           plo     r8                  ; place into low of r8
divlp:     glo     r7                  ; get low of number to subtrace
           str     r2                  ; place into memory
           glo     rb                  ; get low of number
           sm                          ; subtract
           phi     r8                  ; place into temp space
           ghi     r7                  ; get high of subtraction
           str     r2                  ; place into memory
           ghi     rb                  ; get high of number
           smb                         ; perform subtract
           bnf     nomore              ; jump if subtraction was too large
           phi     rb                  ; store result
           ghi     r8
           plo     rb
           inc     r8                  ; increment count
           br      divlp               ; and loop back
nomore:    irx                         ; point back to leading zero flag
           glo     r8
           bnz     nonzero             ; jump if not zero
           ldn     r2                  ; get flag
           bnz     allow0              ; jump if no longer zero
           dec     r2                  ; keep leading zero flag
           br      findnxt             ; skip output
allow0:    ldi     0                   ; recover the zero
nonzero:   adi     30h                 ; convert to ascii
           sep     scall
           dw      disp
           ldi     1                   ; need to set leading flag
           stxd                        ; store it
findnxt:   ghi     r7                  ; get high from subtraction byte
           smi     27h                 ; check for 10000
           bnz     not10000            ; jump if not
           ldi     3                   ; high byte of 1000
           phi     r7                  ; place into r7
           ldi     0e8h                ; low byte of 1000
           plo     r7                  ; plate into r7
           br      nxtiter             ; perform next iteration
not10000:  ghi     r7                  ; get high byte of subtraction
           smi     3                   ; check for 1000
           bnz     not1000             ; jump if not
           ldi     0                   ; high byte of 100
           phi     r7                  ; place into r7
           ldi     100                 ; low byte of 100
           plo     r7                  ; plate into r7
           br      nxtiter             ; perform next iteration
not1000:   glo     r7                  ; get byte from subtraction
           smi     100                 ; check for 100
           bnz     not100              ; jump if not 100
           ldi     0                   ; high byte of 10
           phi     r7                  ; place into r7
           ldi     10                  ; low byte of 10
           plo     r7                  ; plate into r7
           br      nxtiter             ; perform next iteration
not100:    glo     r7                  ; get byte from subtraction
           smi     10                  ; check for 10
           bnz     intdone             ; jump if done
  irx 
  ldi 1
  stxd
           ldi     0                   ; high byte of 1
           phi     r7                  ; place into r7
           ldi     1                   ; low byte of 1
           plo     r7                  ; plate into r7
           br      nxtiter             ; perform next iteration
intdone:   irx                         ; put x back where it belongs
           sep     sret                ; return to caller

           org     500h
; ****************************************************
; *** Execute forth byte codes, RB points to codes ***
; ****************************************************
exec:      ldn     rb                  ; get byte from codestream
           bz      execdn              ; jump if at end of stream
           smi     T_NUM               ; check for numbers
           bz      execnum             ; code is numeric
           ldn     rb                  ; recover byte
           smi     T_ASCII             ; check for ascii data
           bz      execascii           ; jump if ascii
           ldi     high jump
           phi     r8
           ldi     low jump
           plo     r8
           inc     r8
           ldn     rb                  ; recover byte
           ani     07fh                ; strip high bit
           smi     1                   ; reset to origin
           shl                         ; addresses are two bytes
           sex     r2                  ; point X to stack
           str     r2                  ; write offset for addtion
           ldi     low cmdvecs
           add                         ; add offset
           plo     r7
           ldi     high cmdvecs        ; high address of command vectors
           adci    0                   ; propagate carry
           phi     r7                  ; r[7] now points to command vector
           lda     r7                  ; get high byte of vector
           str     r8
           inc     r8
           lda     r7                  ; get low byte of vector
           str     r8
           inc     rb                  ; point r6 to next command
           glo     rb                  ; save RF
           stxd
           ghi     rb
           stxd
jump:      lbr     0                   ; jump vector
execret:   sex     r2                  ; be sure X poits to stack
           plo     r7                  ; save return code
           irx                         ; recover RF
           lda     r2
           phi     rb
           ldn     r2
           plo     rb
           glo     r7                  ; get result code
           bz      exec                ; jump if no error
           ldi     high msempty        ; get error message
           phi     rf
           ldi     low msempty
           plo     rf
execrmsg:  sep     scall
           dw      f_msg
           sep     sret                ; return to caller

execnum:   inc     rb                  ; point to number
           ghi     rb
           phi     r7
           glo     rb
           plo     r7
           lda     r7
           phi     rb
           lda     r7
           plo     rb
           sep     scall
           dw      push
           ghi     r7
           phi     rb
           glo     r7
           plo     rb
           br      exec                ; execute next code
execascii: inc     rb                  ; move past ascii code
           ghi     rb                  ; transfer name to R8
           phi     r8
           glo     rb
           plo     r8
           sep     scall               ; find entry
           dw      findname
           bnf     ascnoerr            ; jump if name was found
ascerr:    ldi     high msgerr         ; get error message
           phi     rf
           ldi     low msgerr
           plo     rf
           br      execrmsg
ascnoerr:  inc     r7                  ; point to type
           inc     r7
           ldn     r7                  ; get type
           smi     86h                 ; check for variable
           bz      execvar             ; jump if so
           ldn     r7                  ; get type
           smi     87h                 ; check for function
           bnz     ascerr              ; jump if not
           sex     r2                  ; be sure X is pointing to stack
           glo     r8                  ; save position
           stxd                        ; and store on stack
           ghi     r8
           stxd
           sep     scall               ; call exec to execute stored program
           dw      exec
           irx                         ; recover pointer
           ldxa
           phi     rb
           ldx
           plo     rb
           br      exec                ; and continue execution
execvar:   sep     scall               ; push var address to stack
           dw      push
           ghi     r8                  ; transfer address back to rb
           phi     rb
           glo     r8
           plo     rb
           br      exec                ; execute next code
           
execdn:    sep     sret                ; return to caller

error:     ldi     1                   ; indicate error
           lbr     execret             ; return to caller
good:      ldi     0                   ; indicate success
           lbr     execret             ; return to caller

           org     600h
cdup:      sep     scall               ; pop value from forth stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           sep     scall               ; push back twice
           dw      push
           sep     scall
           dw      push
           lbr     good                ; return

cdrop:     sep     scall               ; pop value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           lbr     good                ; return
           
cplus:     sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           sex     r2                  ; be sure X points to stack
           glo     r7                  ; perform addition
           str     r2
           glo     rb
           add
           plo     rb
           ghi     r7
           str     r2
           ghi     rb
           adc
           phi     rb
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good
           
 
cminus:    sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           sex     r2                  ; be sure X points to stack
           glo     r7                  ; perform addition
           str     r2
           glo     rb
           sm
           plo     rb
           ghi     r7
           str     r2
           ghi     rb
           smb
           phi     rb
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good

cdot:      sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           sep     scall               ; call integer out routine
           dw      intout
           ldi     ' '                 ; need a space
           sep     scall               ; need to call character out
           dw      disp
           lbr     good                ; return

cudot:     sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           sep     scall               ; call integer out routine
           dw      uintout
           ldi     ' '                 ; need a space
           sep     scall               ; need to call character out
           dw      disp
           lbr     good                ; return

cand:      sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           sex     r2                  ; be sure X points to stack
           glo     r7                  ; perform and
           str     r2
           glo     rb
           and
           plo     rb
           ghi     r7
           str     r2
           ghi     rb
           and
           phi     rb
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good

cor:       sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           sex     r2                  ; be sure X points to stack
           glo     r7                  ; perform and
           str     r2
           glo     rb
           or
           plo     rb
           ghi     r7
           str     r2
           ghi     rb
           or
           phi     rb
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good

cxor:      sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           sex     r2                  ; be sure X points to stack
           glo     r7                  ; perform and
           str     r2
           glo     rb
           xor
           plo     rb
           ghi     r7
           str     r2
           ghi     rb
           xor
           phi     rb
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good

ccr:       ldi     10                  ; line feed
           sep     scall               ; call character display
           dw      disp
           ldi     13                  ; now cr
           sep     scall               ; call character display
           dw      disp
           lbr     good                ; return

cswap:     sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r8
           glo     rb
           plo     r8
           ghi     r7                  ; move number 
           phi     rb
           glo     r7
           plo     rb
           sep     scall               ; put answer back on stack
           dw      push
           ghi     r8                  ; move number 
           phi     rb
           glo     r8
           plo     rb
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good                ; return

ci:        sep     scall               ; get value from return stack
           dw      rpop
           sep     scall               ; put back on return stack
           dw      rpush 
           sep     scall               ; and forth stack
           dw      push
           lbr     good                ; return to caller

cmem:      sex     r2                  ; be sure x is pointing to stack
           ldi     low freemem         ; point to free memory pointer
           plo     r9                  ; place into data frame
           lda     r9                  ; get high byte of free memory pointer
           stxd                        ; store on stack
           lda     r9                  ; get low byte
           str     r2                  ; store on stack
           ldi     low fstack          ; get pointer to stack
           plo     r9                  ; set into data frame
           inc     r9                  ; point to lo byte
           ldn     r9                  ; get it
           sm                          ; perform subtract
           plo     rb                  ; put into result
           dec     r9                  ; high byte of stack pointer
           irx                         ; point to high byte os free mem
           ldn     r9                  ; get high byte of stack
           smb                         ; continue subtraction
           phi     rb                  ; store answer
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good                ; return
 

cdo:       sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r8
           glo     rb
           plo     r8
           ghi     r2                  ; get copy of machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; pointing at R[6] value high
           lda     ra                  ; get high of R[6]
           phi     rb                  ; put into r6
           lda     ra
           plo     rb
           sep     scall               ; store inst point on return stack
           dw      rpush
           ghi     r8                  ; transfer termination to rb
           phi     rb
           glo     r8
           plo     rb
           sep     scall               ; store termination on return stack
           dw      rpush
           ghi     r7                  ; transfer count to rb
           phi     rb
           glo     r7
           plo     rb
           sep     scall               ; store count on return stack
           dw      rpush
           lbr     good

cloop:     sep     scall               ; get top or return stack
           dw      rpop
           inc     rb                  ; add 1 to it
loopcnt:   ghi     rb                  ; move it
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get termination
           dw      rpop
           sex     r2                  ; make sure x is pointing to stack
           glo     rb                  ; get lo of termination
           str     r2                  ; place into memory 
           glo     r7                  ; get count
           sm                          ; perform subtract
           ghi     rb                  ; get hi of termination
           str     r2                  ; place into memory
           ghi     r7                  ; get high of count
           smb                         ; continue subtract
           bdf     cloopdn             ; jump if loop complete
           ghi     rb                  ; move termination
           phi     r8
           glo     rb
           plo     r8
           sep     scall               ; get loop address
           dw      rpop
           sep     scall               ; keep on stack as well
           dw      rpush
           ghi     r2                  ; get copy of machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; pointing at R[6] value high
           ghi     rb
           str     ra                  ; and write it
           inc     ra                 
           glo     rb                  ; get R[6] lo value
           str     ra                  ; and write it
           ghi     r8                  ; transfer termination
           phi     rb
           glo     r8
           plo     rb
           sep     scall               ; push onto return stack
           dw      rpush
           ghi     r7                  ; transfer count
           phi     rb
           glo     r7
           plo     rb
           sep     scall               ; push onto return stack
           dw      rpush
           lbr     good                ; and return
cloopdn:   sep     scall               ; pop off start of loop address
           dw      rpop
           lbr     good                ; and return
cploop:    sep     scall               ; get top or return stack
           dw      rpop
           sex     r2                  ; make sure X points to stack
           ghi     rb                  ; put count into memory
           stxd
           glo     rb
           stxd
           sep     scall               ; get word from data stack
           dw      pop
           lbdf    error
           irx
           glo     rb                  ; add to count
           add
           plo     rb
           ghi     rb
           irx
           adc
           phi     rb
           br      loopcnt             ; then standard loop code

cbegin:    ghi     r2                  ; get copy of machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; pointing at R[6] value high
           lda     ra                  ; get high of R[6]
           phi     rb                  ; put into r6
           lda     ra
           plo     rb
           sep     scall               ; store inst point on return stack
           dw      rpush
           lbr     good                ; and return

cuntil:    sep     scall               ; get top of stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           glo     rb                  ; check for nonzero
           bnz     untilyes            ; jump if non zero
           ghi     rb                  ; check high byte
           bnz     untilyes            ; for non zero
           sep     scall               ; pop off begin address
           dw      rpop
           lbr     good                ; we are done, just return
untilyes:  sep     scall               ; get return address
           dw      rpop
           sep     scall               ; also keep on stack
           dw      rpush
           ghi     r2                  ; get copy of machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; pointing at R[6] value high
           ghi     rb
           str     ra                  ; and write it
           inc     ra                 
           glo     rb                  ; get R[6] lo value
           str     ra                  ; and write it
           lbr     good                ; now return

crgt:      sep     scall               ; get value from return stack
           dw      rpop
           sep     scall               ; push to data stack
           dw      push
           lbr     good                ; return to caller

cgtr:      sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           sep     scall               ; push to return stack
           dw      rpush
           lbr     good                ; return to caller

cunequal:   sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           sex     r2                  ; be sure X points to stack
           glo     r7                  ; perform and
           str     r2
           glo     rb
           xor
           lbnz    unequal             ; jump if not equal
           ghi     r7
           str     r2
           ghi     rb
           xor
           lbnz    unequal             ; jump if not equal
           phi     rb                  ; set return result
           plo     rb
eq2:       sep     scall               ; put answer back on stack
           dw      push
           lbr     good
unequal:   ldi     0                   ; set return result
           phi     rb
           plo     rb
           inc     rb                  ; it is now 1
           br      eq2

cwords:    ldi     high cmdtable       ; point to command table
           phi     r7                  ; put into a pointer register
           ldi     low cmdtable
           plo     r7
cwordslp:  lda     r7                  ; get byte
           lbz     cwordsdn            ; jump if done
           plo     rb                  ; save it
           ani     128                 ; check for final of token
           lbnz    cwordsf             ; jump if so
           glo     rb                  ; get byte
           sep     scall               ; display it
           dw      disp 
           lbr      cwordslp           ; and loop back
cwordsf:   glo     rb                  ; get byte
           ani     07fh                ; strip high bit
           sep     scall               ; display it
           dw      disp
           ldi     ' '                 ; display a space
           sep     scall               ; display it
           dw      disp
           lbr     cwordslp            ; and loop back
cwordsdn:  ldi     10
           sep     scall               ; display it
           dw      disp
           ldi     13                  ; display a space
           sep     scall               ; display it
           dw      disp
           ldi     high storage        ; get beginning of program memory
           phi     r7
           ldi     low storage
           plo     r7
cwordslp2: lda     r7                  ; get pointer to next entry
           phi     r8                  ; put into r8
           lda     r7                  ; now pointing at type indicator
           plo     r8                  ; save low of link
           bnz     cwordsnot           ; jump if not link terminator
           ghi     r8                  ; check high byte too
           bnz     cwordsnot
cwordsdn1: ldi     10                  ; display a space
           sep     scall               ; display it
           dw      disp
           ldi     13                  ; display a space
           sep     scall               ; display it
           dw      disp
           lbr     good                ; return to caller
cwordsnot: inc     r7                  ; now pointing at ascii indicator
           inc     r7                  ; first character of name
wordsnotl: lda     r7                  ; get byte from string
           bz      wordsnxt            ; jump if end of string
           sep     scall               ; display it
           dw      disp
           br      wordsnotl           ; keep going
wordsnxt:  ldi     ' '                 ; want a space
           sep     scall               ; display it
           dw      disp
           ghi     r8                  ; transfer next word address to r7
           phi     r7
           glo     r8
           plo     r7
           br      cwordslp2           ; and check next word

cemit:     sep     scall               ; get top of stack
           dw      pop
           lbdf    error               ; jump if error
           glo     rb                  ; get low of return value
           sep     scall               ; and display ti
           dw      disp
           lbr     good                ; return to caller

cwhile:    sep     scall               ; get top of stack
           dw      pop
           lbdf    error               ; jump if error
           glo     rb                  ; need to check for zero
           bnz     whileno             ; jump if not zero
           ghi     rb                  ; check high byte
           bnz     whileno
           ghi     r2                  ; copy machine stack to RA
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; point to R[6]
           lda     ra                  ; get command stream
           phi     rb                  ; put into r6
           ldn     ra
           plo     rb
           ldi     0                   ; set while count to zero
           plo     r7
findrep:   ldn     rb                  ; get byte from stream
           smi     81h                 ; was a while found
           bnz     notwhile            ; jump if not
           inc     r7                  ; increment while count
notrep:    inc     rb                  ; point to next byte
           br      findrep             ; and keep looking
notwhile:  ldn     rb                  ; retrieve byte
           smi     82h                 ; is it a repeat
           bnz     notrep              ; jump if not
           glo     r7                  ; get while count
           bz      fndrep              ; jump if not zero
           dec     r7                  ; decrement count
           br      notrep              ; and keep looking
fndrep:    inc     rb                  ; move past the while
           glo     rb                  ; now put back into R[6]
           str     ra
           dec     ra
           ghi     rb
           str     ra
           lbr     good                ; then return to caller
whileno:   ghi     r2                  ; copy machine stack to RA
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; now pointing to high byte of R[6]
           lda     ra                  ; get it
           phi     rb                  ; and put into r6
           ldn     ra                  ; get low byte
           plo     rb
           dec     rb                  ; point back to while command
           sep     scall               ; put onto return stack
           dw      rpush
           lbr     good                ; then return

crepeat:   sep     scall               ; get address on return stack
           dw      rpop
           ghi     r2                  ; transfer machine stack to RA
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; now pointing at high byte of R[6]
           ghi     rb                  ; get while address
           str     ra                  ; and place into R[6]
           inc     ra
           glo     rb
           str     ra
           lbr     good                ; then return
           
cif:       sep     scall               ; get top of stack 
           dw      pop
           lbdf    error               ; jump if error
           glo     rb                  ; check for zero
           bnz     ifno                ; jump if not zero
           ghi     rb                  ; check hi byte too
           bnz     ifno                ; jump if not zero
           ghi     r2                  ; transfer machine stack to RA
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; now pointing at R[6]
           lda     ra                  ; get R[6]
           phi     rb
           ldn     ra
           plo     rb
           ldi     0                   ; set IF count
           plo     r7                  ; put into counter
iflp1:     ldn     rb                  ; get next byte
           smi     83h                 ; check for IF
           bnz     ifnotif             ; jump if not
           inc     r7                  ; increment if count
ifcnt:     inc     rb                  ; point to next byte
           br      iflp1               ; keep looking
ifnotif:   ldn     rb                  ; retrieve byte
           smi     84h                 ; check for ELSE
           bnz     ifnotelse           ; jump if not
           glo     r7                  ; get IF count
           bnz     ifcnt               ; jump if it is not zero
           inc     rb                  ; move past the else
ifsave:    glo     rb                  ; store back into instruction pointer
           str     ra
           dec     ra
           ghi     rb
           str     ra
           lbr     good                ; and return
ifnotelse: ldn     rb                  ; retrieve byte
           smi     85h                 ; check for THEN
           bnz     ifcnt               ; jump if not
           glo     r7                  ; get if count
           dec     r7                  ; decrement if count
           bnz     ifcnt               ; jump if not zero
           br      ifsave              ; otherwise found
ifno:      lbr     good                ; no action needed, just return

celse:     ghi     r2                  ; transfer machine stack to ra
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; now pointing at R[6]
           lda     ra                  ; get current R[6]
           phi     rb                  ; and place into r6
           ldn     ra
           plo     rb
           ldi     0                   ; count of IFs
           plo     r7                  ; put into R7
elselp1:   ldn     rb                  ; get next byte from stream
           smi     83h                 ; check for IF
           bnz     elsenif             ; jump if not if
           inc     r7                  ; increment IF count
elsecnt:   inc     rb                  ; point to next byte
           br      elselp1             ; keep looking
elsenif:   ldn     rb                  ; retrieve byte
           smi     85h                 ; is it THEN
           bnz     elsecnt             ; jump if not
           glo     r7                  ; get IF count
           dec     r7                  ; minus 1 IF
           bnz     elsecnt             ; jump if not 0
           glo     rb                  ; put into instruction pointer
           str     ra
           dec     ra
           ghi     rb
           str     ra
           lbr     good                ; now pointing at a then

cthen:     lbr     good                ; then is nothing but a place holder

cequal:    sep     scall               ; get value from stack
           dw      pop
           lbdf    error               ; jump if stack was empty
           ghi     rb                  ; move number 
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get next number
           dw      pop
           lbdf    error               ; jump if stack was empty
           sex     r2                  ; be sure X points to stack
           glo     r7                  ; perform and
           str     r2
           glo     rb
           xor
           lbnz    unequal2            ; jump if not equal
           ghi     r7
           str     r2
           ghi     rb
           xor
           lbnz    unequal2            ; jump if not equal
           phi     rb                  ; set return result
           plo     rb
           inc     rb
eq3:       sep     scall               ; put answer back on stack
           dw      push
           lbr     good
unequal2:  ldi     0                   ; set return result
           phi     rb
           plo     rb
           br      eq3

cdepth:    sex     r2                  ; be sure x is pointing to stack
           ldi     low fstack          ; point to free memory pointer
           plo     r9                  ; place into data frame
           lda     r9                  ; get high byte of free memory pointer
           stxd                        ; store on stack
           lda     r9                  ; get low byte
           str     r2                  ; store on stack
           ldi     low tos             ; get pointer to stack
           plo     r9                  ; set into data frame
           inc     r9                  ; point to lo byte
           ldn     r9                  ; get it
           sm                          ; perform subtract
           plo     rb                  ; put into result
           dec     r9                  ; high byte of stack pointer
           irx                         ; point to high byte os free mem
           ldn     r9                  ; get high byte of stack
           smb                         ; continue subtraction
           shr                         ; divide by 2
           phi     rb                  ; store answer
           glo     rb                  ; propagate the shift
           shrc
           plo     rb
           sep     scall               ; put answer back on stack
           dw      push
           lbr     good                ; return
 
crot:      sep     scall               ; get C
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R7
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get B
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R7
           phi     r8
           glo     rb
           plo     r8
           sep     scall               ; get A
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R7
           phi     rc
           glo     rb
           plo     rc
           ghi     r8                  ; get B
           phi     rb
           glo     r8
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           ghi     r7                  ; get C
           phi     rb
           glo     r7
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           ghi     rc                  ; get A
           phi     rb
           glo     rc
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           lbr     good                ; return to caller
 
cmrot:     sep     scall               ; get C
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R7
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get B
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R7
           phi     r8
           glo     rb
           plo     r8
           sep     scall               ; get A
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R7
           phi     rc
           glo     rb
           plo     rc
           ghi     r7                  ; get C
           phi     rb
           glo     r7
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           ghi     rc                  ; get A
           phi     rb
           glo     rc
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           ghi     r8                  ; get B
           phi     rb
           glo     r8
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           lbr     good                ; return to caller
 
cover:     sep     scall               ; get B
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R7
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get A
           dw      pop
           lbdf    error               ; jump if error
           ghi     rb                  ; transfer to R*
           phi     r8
           glo     rb
           plo     r8
           sep     scall               ; put onto stack
           dw      push
           ghi     r7                  ; get B
           phi     rb
           glo     r7
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           ghi     r8                  ; get A
           phi     rb
           glo     r8
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           lbr     good                ; return to caller
           
cat:       sep     scall               ; get address from stack
           dw      pop
           lbdf    error               ; jump on error
           ghi     rb                  ; transfer address
           phi     r7
           glo     rb
           plo     r7
           lda     r7                  ; get word at address
           phi     rb
           ldn     r7
           plo     rb
           sep     scall               ; put onto stack
           dw      push
           lbr     good                ; return to caller
           
cexcl:     sep     scall               ; get address from stack
           dw      pop
           lbdf    error               ; jump on error
           ghi     rb                  ; transfer address
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; date data word from stack
           dw      pop
           lbdf    error               ; jump on error
           ghi     rb                  ; write word to memory
           str     r7
           inc     r7
           glo     rb
           str     r7
           lbr     good                ; and return
           
ccat:      sep     scall               ; get address from stack
           dw      pop
           lbdf    error               ; jump on error
           ghi     rb                  ; transfer address
           phi     r7
           glo     rb
           plo     r7
           lda     r7                  ; get word at address
           plo     rb
           ldi     0                   ; high byte is zero
           phi     rb
           sep     scall               ; put onto stack
           dw      push
           lbr     good                ; return to caller
           
ccexcl:    sep     scall               ; get address from stack
           dw      pop
           lbdf    error               ; jump on error
           ghi     rb                  ; transfer address
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; date data word from stack
           dw      pop
           lbdf    error               ; jump on error
           glo     rb                  ; write byte to memory
           str     r7
           lbr     good                ; and return

cvariable: ghi     r2                  ; transfer machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; point to R[6]
           lda     ra                  ; and retrieve it
           phi     rb
           ldn     ra
           plo     rb
           ldn     rb                  ; get next byte
           smi     T_ASCII             ; it must be an ascii mark
           lbnz    error               ; jump if not
           inc     rb                  ; move into string
varlp1:    lda     rb                  ; get byte
           bnz     varlp1              ; jump if terminator not found
           inc     rb                  ; allow space for var value
           inc     rb                  ; new value of freemem
           ldi     low freemem         ; get current free memory pointer
           plo     r9                  ; put into data segment
           lda     r9                  ; get current pointer
           phi     r7                  ; place here
           ldn     r9                  ; get low byte
           plo     r7
           ghi     rb                  ; get memory pointer
           str     r7                  ; and store into link list
           inc     r7
           glo     rb
           str     r7
           glo     rb                  ; store new freemem value
           str     r9
           dec     r9
           ghi     rb
           str     r9
           ldi     0                   ; need zero at end of list
           str     rb                  ; store it
           inc     rb
           str     rb
           glo     rb                  ; write back to instruction pointer
           str     ra
           dec     ra
           ghi     rb
           str     ra
           lbr     good                ; return

ccolon:    ghi     r2                  ; transfer machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; point to R[6]
           lda     ra                  ; and retrieve it
           phi     rb
           ldn     ra
           plo     rb
           ldn     rb                  ; get next byte
           smi     T_ASCII             ; it must be an ascii mark
           lbnz    error               ; jump if not
           inc     rb                  ; move into string
colonlp1:  lda     rb                  ; get byte
           smi     88h                 ; look for the ;
           bnz     colonlp1            ; jump if terminator not found
           ldi     0                   ; want a command terminator
           str     rb                  ; write it
           inc     rb                  ; new value for freemem
           ldi     low freemem         ; get current free memory pointer
           plo     r9                  ; put into data segment
           lda     r9                  ; get current pointer
           phi     r7                  ; place here
           ldn     r9                  ; get low byte
           plo     r7
           ghi     rb                  ; get memory pointer
           str     r7                  ; and store into link list
           inc     r7
           glo     rb
           str     r7
           glo     rb                  ; store new freemem value
           str     r9
           dec     r9
           ghi     rb
           str     r9
           ldi     0                   ; need zero at end of list
           str     rb                  ; store it
           inc     rb
           str     rb
           glo     rb                  ; write back to instruction pointer
           str     ra
           dec     ra
           ghi     rb
           str     ra
           lbr     good                ; return

csemi:     lbr     good

csee:      ghi     r2                  ; transfer machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; point to R[6]
           lda     ra                  ; and retrieve it
           phi     r8
           ldn     ra
           plo     r8
           ldn     r8                  ; get next byte
           smi     T_ASCII             ; it must be an ascii mark
           lbnz    error               ; jump if not
           inc     r8                  ; move into string
           sep     scall               ; find the name
           dw      findname
           lbdf    error               ; jump if not found
           glo     r8                  ; put new address into inst pointer
           str     ra 
           dec     ra
           ghi     r8
           str     ra
           inc     r7                  ; move past lind address
           inc     r7
           ldn     r7                  ; get type byte
           smi     86h                 ; check for variable
           bnz     cseefunc            ; jump if not
           lda     rb                  ; get value
           phi     r7
           lda     rb
           plo     rb
           ghi     r7
           phi     rb
           sep     scall               ; display the value
           dw      intout
seeexit:   ldi     10                  ; display cr/lf
           sep     scall
           dw      disp
           ldi     13                  ; display cr/lf
           sep     scall
           dw      disp
           lbr     good                ; otherwise good
cseefunc:  ldi     ':'                 ; start with a colon
           sep     scall               ; display character
           dw      disp
           inc     r7                  ; move address to name
seefunclp: ldi     ' '                 ; need a spae
           sep     scall               ; display character
           dw      disp
           ldn     r7                  ; get next token
           bz      seeexit             ; jump if done
           smi     T_ASCII             ; check for ascii
           bnz     seenota             ; jump if not ascii
           inc     r7                  ; move into string
seestrlp:  ldn     r7                  ; get next byte
           bz      seenext             ; jump if done with token
           sep     scall               ; display character
           dw      disp
           inc     r7                  ; point to next character
           br      seestrlp            ; and continue til done
seenext:   inc     r7                  ; point to next token
           br      seefunclp
seenota:   ldn     r7                  ; reget token
           smi     T_NUM               ; is it a number
           bnz     seenotn             ; jump if not a number
           inc     r7                  ; move past token
           lda     r7                  ; get number into rb
           phi     rb
           ldn     r7
           plo     rb
           glo     r7                  ; save r7
           stxd
           ghi     r7
           stxd
           sep     scall               ; display the number
           dw      intout
           irx                         ; retrieve r7
           ldxa
           phi     r7
           ldx
           plo     r7
           br      seenext             ; on to next token
seenotn:   ldi     high cmdtable       ; point to command table
           phi     rb
           ldi     low cmdtable
           plo     rb
           ldn     r7                  ; get token
           ani     07fh                ; strip high bit
           plo     r8                  ; token counter
seenotnlp: dec     r8                  ; decrement count
           glo     r8                  ; get count
           bz      seetoken            ; found the token
seelp3:    lda     rb                  ; get byte from token
           ani     128                 ; was it last one?
           bnz     seenotnlp           ; jump if it was
           br      seelp3              ; keep looking
seetoken:  ldn     rb                  ; get byte from token
           ani     128                 ; is it last
           bnz     seetklast           ; jump if so
           ldn     rb                  ; retrieve byte
           sep     scall               ; display it
           dw      disp
           inc     rb                  ; point to next character
           br      seetoken            ; and loop til done
seetklast: ldn     rb                  ; retrieve byte
           ani     07fh                ; strip high bit
           sep     scall               ; display it
           dw      disp
           lbr     seenext             ; jump for next token

cdotqt:    ghi     r2                  ; transfer machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; point to R[6]
           lda     ra                  ; and retrieve it
           phi     r8
           ldn     ra
           plo     r8
           ldn     r8                  ; get next byte
           smi     T_ASCII             ; it must be an ascii mark
           lbnz    error               ; jump if not
           inc     r8                  ; move past ascii mark
cdotqtlp:  lda     r8                  ; get next byte
           bz      cdotqtdn            ; jump if terinator
           smi     34                  ; check for quote
           bz      cdotqtlp            ; do not display quotes
           dec     r8
           lda     r8
           sep     scall               ; display byte
           dw      disp
           br      cdotqtlp            ; loop back
cdotqtdn:  glo     r8                  ; put pointer back
           str     ra
           dec     ra
           ghi     r8
           str     ra
           lbr     good                ; and return

ckey:      sep     scall               ; go and get a key
           dw      getkey
           plo     rb                  ; put into r6
           ldi     0                   ; zero the high byte
           phi     rb
           sep     scall               ; place key on the stack
           dw      push
           lbr     good                ; then return to caller

callot:    ldi     high storage        ; get address of storage
           phi     r7
           ldi     low storage
           plo     r7
callotlp1: lda     r7                  ; get next link
           phi     r8
           ldn     r7
           plo     r8
           lda     r8                  ; get value at that link
           phi     rb
           ldn     r8
           dec     r8                  ; keep r8 pointing at link
           lbnz    callotno            ; jump if next link is not zero
           ghi     rb                  ; check high byte
           lbnz    callotno            ; jump if not zero
           lbr     callotyes
callotno:  ghi     r8                  ; transfer link to r7
           phi     r7
           glo     r8
           plo     r7
           lbr     callotlp1           ; and keep looking
callotyes: inc     r7                  ; point to type byte
           ldn     r7                  ; get it
           smi     FVARIABLE           ; it must be a variable
           lbnz    error               ; jump if not
           sep     scall               ; get word from stack
           dw      pop
           lbdf    error               ; jump if error
           glo     rb                  ; multiply by 2
           shl
           plo     rb
           ghi     rb
           shlc
           phi     rb
           sex     r2                  ; be sure X points to stack
           glo     rb                  ; add r6 to r8
           str     r2
           glo     r8
           add
           plo     r8
           ghi     rb
           str     r2
           ghi     r8
           adc
           phi     r8
           dec     r7                  ; point back to link
           glo     r8                  ; and write new pointer
           str     r7
           dec     r7
           ghi     r8
           str     r7
           ldi     low freemem         ; need to adjust free memory pointer
           plo     r9                  ; put into data frame
           ghi     r8                  ; and save new memory position
           str     r9
           inc     r9
           glo     r8
           str     r9
           ldi     0                   ; zero new position
           str     r8
           inc     r8
           str     r8
           lbr good

cmul:      sep     scall               ; get first value from stack
           dw      pop
           lbdf    error               ; jump on error
           ghi     rb                  ; transfer to r7
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get second number
           dw      pop
           lbdf    error               ; jump on error
           sep     scall               ; call multiply routine
           dw      mul16
           sep     scall               ; push onto stack
           dw      push
           lbr     good                ; return

cdiv:      sep     scall               ; get first value from stack
           dw      pop
           lbdf    error               ; jump on error
           ghi     rb                  ; transfer to r7
           phi     r7
           glo     rb
           plo     r7
           sep     scall               ; get second number
           dw      pop
           lbdf    error               ; jump on error
           sex     r2
           ghi     r9
           stxd
           sep     scall               ; call multiply routine
           dw      div16
           irx
           ldx
           phi     r9
           ghi     rc                  ; transfer answer
           phi     rb
           glo     rc
           plo     rb
           sep     scall               ; push onto stack
           dw      push
           lbr     good                ; return

cforget:   ghi     r2                  ; transfer machine stack
           phi     ra
           glo     r2
           plo     ra
           inc     ra                  ; point to R[6]
           lda     ra                  ; and retrieve it
           phi     r8
           ldn     ra
           plo     r8
           ldn     r8                  ; get next byte
           smi     T_ASCII             ; it must be an ascii mark
           lbnz    error               ; jump if not
           inc     r8                  ; move into string
           sep     scall               ; find the name
           dw      findname
           lbdf    error               ; jump if not found
           glo     r8
           str     ra
           dec     ra
           ghi     r8
           str     ra
           lda     r7                  ; get next entry
           phi     rb
           ldn     r7
           plo     rb
           dec     r7
           sex     r2                  ; be sure X is pointing to stack
           glo     r7                  ; find difference in pointers
           str     r2
           glo     rb
           sm
           plo     rc
           ghi     r7
           str     r2
           ghi     rb
           smb
           phi     rc                  ; RC now has offset, RB is next descr.
forgetlp1: lda     rb                  ; get pointer
           phi     ra                  ; put into ra
           str     r2
           ldn     rb
           plo     ra
           or                          ; see if it was zero
           bz      forgetd1            ; jump if it was
           glo     rc                  ; subtract RC from RA
           str     r2
           glo     ra
           sm
           str     rb                  ; store back into pointer
           dec     rb
           ghi     rc
           str     r2
           ghi     ra
           smb
           str     rb
           ghi     ra                  ; transfer value
           phi     rb
           glo     ra
           plo     rb
           br      forgetlp1           ; loop until done

forgetd1:  lda     r7                  ; get next entry
           phi     rb
           ldn     r7
           plo     rb
           dec     r7

           ldi     low freemem         ; get end of memory pointer
           plo     r9                  ; and place into data frame
           lda     r9                  ; get free memory position
           phi     r8
           ldn     r9
           plo     r8
           inc     r8                  ; account for zero bytes at end
           inc     r8
           glo     rb                  ; subtract R6 from R8
           str     r2
           glo     r8
           sm
           plo     r8
           ghi     rb
           str     r2
           ghi     r8
           smb
           phi     r8                  ; r8 now has number of bytes to move
forgetlp:  lda     rb                  ; get byte from higher memory
           str     r7                  ; write to lower memory
           inc     r7                  ; point to next position
           dec     r8                  ; decrement the count
           glo     r8                  ; check for zero
           bnz     forgetlp
           ghi     r8
           bnz     forgetlp
           dec     r7                  ; move back to freemem position
           dec     r7 
           glo     r7                  ; store back into freemem pointer
           str     r9
           dec     r9
           ghi     r7
           str     r9
           lbr     good                ; and return

cerror:    sep     scall               ; get number fro stack
           dw      pop
           lbdf    error               ; jump on error
           glo     rb                  ; get returned value
           lbr     execret             ; return to caller

hello:     db      'Rc/Forth 0.1'
crlf:      db       10,13,0
prompt:    db      'ok ',0
msempty:   db      'stack empty',10,13,0
msgerr:    db      'err',10,13,0
cmdtable:  db      'WHIL',('E'+80h)
           db      'REPEA',('T'+80h)
           db      'I',('F'+80h)
           db      'ELS',('E'+80h)
           db      'THE',('N'+80h)
           db      'VARIABL',('E'+80h)
           db      (':'+80h)
           db      (';'+80h)
           db      'DU',('P'+80h)
           db      'DRO',('P'+80h)
           db      'SWA',('P'+80h)
           db      ('+'+80h)
           db      ('-'+80h)
           db      ('*'+80h)
           db      ('/'+80h)
           db      ('.'+80h)
           db      'U',('.'+80h)
           db      ('I'+80h)
           db      'AN',('D'+80h)
           db      'O',('R'+80h)
           db      'XO',('R'+80h)
           db      'C',('R'+80h)
           db      'ME',('M'+80h)
           db      'D',('O'+80h)
           db      'LOO',('P'+80h)
           db      '+LOO',('P'+80h)
           db      ('='+80h)
           db      '<',('>'+80h)
           db      'BEGI',('N'+80h)
           db      'UNTI',('L'+80h)
           db      'R',('>'+80h)
           db      '>',('R'+80h)
           db      'WORD',('S'+80h)
           db      'EMI',('T'+80h)
           db      'DEPT',('H'+80h)
           db      'RO',('T'+80h)
           db      '-RO',('T'+80h)
           db      'OVE',('R'+80h)
           db      ('@'+80h)
           db      ('!'+80h)
           db      'C',('@'+80h)
           db      'C',('!'+80h)
           db      '.',(34+80h)
           db      'KE',('Y'+80h)
           db      'ALLO',('T'+80h)
           db      'ERRO',('R'+80h)
           db      'SE',('E'+80h)
           db      'FORGE',('T'+80h)
           db      0                   ; no more tokens
cmdvecs:   dw      cwhile              ; 81h
           dw      crepeat             ; 82h
           dw      cif                 ; 83h
           dw      celse               ; 84h
           dw      cthen               ; 85h
           dw      cvariable           ; 86h
           dw      ccolon              ; 87h
           dw      csemi               ; 88h
           dw      cdup                ; 89h
           dw      cdrop               ; 8ah
           dw      cswap               ; 8bh
           dw      cplus               ; 8ch
           dw      cminus              ; 8dh
           dw      cmul
           dw      cdiv
           dw      cdot                ; 8eh
           dw      cudot               ; 8fh
           dw      ci                  ; 90h
           dw      cand                ; 91h
           dw      cor                 ; 92h
           dw      cxor                ; 93h
           dw      ccr                 ; 94h
           dw      cmem                ; 95h
           dw      cdo                 ; 96h
           dw      cloop               ; 97h
           dw      cploop              ; 98h
           dw      cequal              ; 99h
           dw      cunequal            ; 9ah
           dw      cbegin              ; 9bh
           dw      cuntil              ; 9ch
           dw      crgt                ; 9dh
           dw      cgtr                ; 9eh
           dw      cwords              ; 9fh
           dw      cemit               ; a0h
           dw      cdepth              ; a1h
           dw      crot                ; a2h
           dw      cmrot               ; a3h
           dw      cover               ; a4h
           dw      cat                 ; a5h
           dw      cexcl               ; a6h
           dw      ccat                ; a7h
           dw      ccexcl              ; a8h
           dw      cdotqt              ; a9h
           dw      ckey
           dw      callot
           dw      cerror
           dw      csee                ; aah
           dw      cforget             ; abh
buffer:    ds      256
storage:   dw      pinst1
           db      FCOLON,T_ASCII,'1+',0,T_NUM,0,1,FPLUS,FSEMI,0
pinst1:    dw      pinst2
           db      FCOLON,T_ASCII,'1-',0,T_NUM,0,1,FMINUS,FSEMI,0
pinst2:    dw      pinst3
           db      FCOLON,T_ASCII,'CLEAR',0,FDEPTH,FWHILE,FDROP,FDEPTH,FREPEAT
           db      FSEMI,0
pinst3:    dw      pinst4
           db      FCOLON,T_ASCII,'FREE',0,FMEM,FUDOT,FCR,FSEMI,0
pinst4:    dw      pinst5
           db      FCOLON,T_ASCII,'SPACES',0,T_NUM,0,0,FDO,T_NUM,0,32,FEMIT
           db      FLOOP,FSEMI,0
pinst5:    dw      pinst6
           db      FCOLON,T_ASCII,'+!',0,FDUP,FROT,FSWAP,FAT,FPLUS,FSWAP,FEXCL
           db      FSEMI,0
pinst6:    dw      pinst7
           db      FCOLON,T_ASCII,'-!',0,FDUP,FROT,FSWAP,FAT,FSWAP,FMINUS,FSWAP
           db      FEXCL,FSEMI,0
pinst7:    dw      pinst8
           db      FCOLON,T_ASCII,'NOT',0,FIF,T_NUM,0,0,FELSE,T_NUM,0,1,FTHEN
           db      FSEMI,0
pinst8:    dw      pinst9
           db      FCOLON,T_ASCII,'0=',0,T_NUM,0,0,FEQUAL,FSEMI,0
pinst9:    dw      pinst10
           db      FCOLON,T_ASCII,'2+',0,T_NUM,0,2,FPLUS,FSEMI,0
pinst10:   dw      pinst11
           db      FCOLON,T_ASCII,'2-',0,T_NUM,0,2,FMINUS,FSEMI,0
pinst11:   dw      pinst12
           db      FCOLON,T_ASCII,'@+',0,FDUP,FAT,FSWAP,T_NUM,0,2,FPLUS,FSWAP,FSEMI,0
pinst12:   dw      pinst13
           db      FCOLON,T_ASCII,'>0',0,FDUP,FIF,T_NUM,128,0,FAND,FIF,T_NUM,0,0
           db      FELSE,T_NUM,0,1,FTHEN,FELSE,FDROP,T_NUM,0,0,FTHEN,FSEMI,0
pinst13:   dw      pinst14
           db      FCOLON,T_ASCII,'<0',0,FDUP,FIF,T_NUM,128,0,FAND,FIF,T_NUM,0,1
           db      FELSE,T_NUM,0,0,FTHEN,FELSE,FDROP,T_NUM,0,0,FTHEN,FSEMI,0
pinst14:   dw      pinst15
           db      FCOLON,T_ASCII,'>',0,FMINUS,FDUP,FIF,T_NUM,128,0,FAND,FIF,T_NUM,0,0
           db      FELSE,T_NUM,0,1,FTHEN,FELSE,FDROP,T_NUM,0,0,FTHEN,FSEMI,0
pinst15:   dw      pinst16
           db      FCOLON,T_ASCII,'<',0,FMINUS,FDUP,FIF,T_NUM,128,0,FAND,FIF,T_NUM,0,1
           db      FELSE,T_NUM,0,0,FTHEN,FELSE,FDROP,T_NUM,0,0,FTHEN,FSEMI,0
pinst16:   dw      pinst17
           db      FCOLON,T_ASCII,'>=',0,FMINUS,FDUP,FIF,T_NUM,128,0,FAND,FIF,T_NUM,0,0
           db      FELSE,T_NUM,0,1,FTHEN,FELSE,FDROP,T_NUM,0,1,FTHEN,FSEMI,0
pinst17:   dw      pinst18
           db      FCOLON,T_ASCII,'<=',0,FMINUS,FDUP,FIF,T_NUM,128,0,FAND,FIF,T_NUM,0,1
           db      FELSE,T_NUM,0,0,FTHEN,FELSE,FDROP,T_NUM,0,1,FTHEN,FSEMI,0
pinst18:   dw      pinst19
           db      FCOLON,T_ASCII,'.S',0,T_NUM,0,15,FAT,T_NUM,0,1,FPLUS,FDEPTH,
           db      T_NUM,0,1,FMINUS
           db      T_NUM,0,0,FDO,FDUP,FAT,FDOT,T_NUM,0,2,FPLUS,FLOOP,FDROP,FSEMI,0
pinst19:   dw      pinst20
           db      FCOLON,T_ASCII,'?',0,FAT,FDOT,FSEMI,0
pinst20:   dw      pinst21
           db      FCOLON,T_ASCII,'NEG',0,T_NUM,0,0,FSWAP,FMINUS,FSEMI,0
pinst21:   dw      pinst22
           db      FCOLON,T_ASCII,'MAX',0,FDUP,FROT,FDUP,FROT,T_ASCII,'>',0,FIF
           db      FSWAP,FDROP,FELSE,FDROP,FTHEN,FSEMI,0
pinst22:   dw      pinst23
           db      FCOLON,T_ASCII,'MIN',0,FDUP,FROT,FDUP,FROT,T_ASCII,'>',0,FIF
           db      FDROP,FELSE,FSWAP,FDROP,FTHEN,FSEMI,0
pinst23:   dw      pinst24
           db      FCOLON,T_ASCII,'?DUP',0,FDUP,FIF,FDUP,FTHEN,FSEMI,0
pinst24:   dw      pinst25
           db      FCOLON,T_ASCII,'ABS',0,FDUP,T_ASCII,'<0',0,FIF,T_NUM,0,0
           db      FSWAP,FMINUS,FTHEN,FSEMI,0
pinst25:   dw      pinst26
           db      FCOLON,T_ASCII,'BL',0,T_NUM,0,32,FSEMI,0
pinst26:   dw      pinst27
           db      FCOLON,T_ASCII,'SPACE',0,T_NUM,0,32,FEMIT,FSEMI,0
pinst27:   dw      pinst28
           db      FCOLON,T_ASCII,'NIP',0,FSWAP,FDROP,FSEMI,0
pinst28:   dw      pinst29
           db      FCOLON,T_ASCII,'TUCK',0,FSWAP,FOVER,FSEMI,0
pinst29:   dw      pinst30
           db      FCOLON,T_ASCII,'TRUE',0,T_NUM,0,1,FSEMI,0
pinst30:   dw      pinst31
           db      FCOLON,T_ASCII,'FALSE',0,T_NUM,0,0,FSEMI,0
pinst31:   dw      pinst32
           db      FCOLON,T_ASCII,'CLS',0,T_NUM,0,27,FEMIT,T_NUM,0,91,FEMIT
           db      T_NUM,0,50,FEMIT,T_NUM,0,74,FEMIT,FSEMI,0
pinst32:   dw      pinst33
           db      FCOLON,T_ASCII,'MOD',0,FDUP,FROT,FDUP,FROT,FDIV,FROT,FMUL
           db      FMINUS,FSEMI,0
pinst33:   dw      pinst34
           db      FCOLON,T_ASCII,'LSHIFT',0,FDUP,FWHILE,FSWAP,T_NUM,0,2,FMUL
           db      FSWAP,T_NUM,0,1,FMINUS,FDUP,FREPEAT,FDROP,FSEMI,0
pinst34:   dw      pinst35
           db      FCOLON,T_ASCII,'RSHIFT',0,FDUP,FWHILE,FSWAP,T_NUM,0,2,FDIV
           db      FSWAP,T_NUM,0,1,FMINUS,FDUP,FREPEAT,FDROP,FSEMI,0
pinst35:   dw      pinst36
           db      FCOLON,T_ASCII,'FILL',0,FSWAP,FMROT,FDUP,FIF,T_NUM,0,0,FDO
           db      FOVER,FOVER,FCEXCL,T_NUM,0,1,FPLUS,FLOOP,FELSE,FDROP,FTHEN
           db      FDROP,FDROP,FSEMI,0
pinst36:   dw      pinst37
           db      FCOLON,T_ASCII,'INVERT',0,T_NUM,255,255,FXOR,FSEMI,0
pinst37:   dw      pinst38
           db      FCOLON,T_ASCII,'TYPE',0,FDUP,FIF,T_NUM,0,0,FDO,FDUP,FCAT
           db      FEMIT,T_NUM,0,1,FPLUS,FLOOP,FELSE,FDROP,FTHEN,FDROP,FSEMI,0
pinst38:   dw      pinst39
           db      FCOLON,T_ASCII,'SGN',0,FDUP,FIF,T_NUM,128,0,FAND,FIF
           db      T_NUM,255,255,FELSE,T_NUM,0,1,FTHEN,FTHEN,FSEMI,0
pinst39:   dw      pinst40
           db      FCOLON,T_ASCII,'2DUP',0,FOVER,FOVER,FSEMI,0
pinst40:   dw      pinst41
           db      FCOLON,T_ASCII,'PICK',0,FDUP,FDEPTH,T_NUM,0,1,FMINUS,
           db      T_ASCII,'<',0,FIF
           db      T_NUM,0,2,FMUL,T_NUM,0,15,FAT,T_NUM,0,1,FPLUS,FPLUS,FAT
           db      FELSE,FDROP,T_NUM,0,1,FERROR,FTHEN,FSEMI,0
pinst41:   dw      pinst42
           db      FCOLON,T_ASCII,'/MOD',0,FOVER,FOVER,T_ASCII,'MOD',0
           db      FMROT,FDIV,FSEMI,0
pinst42:   dw      pinst43
           db      FCOLON,T_ASCII,'*!',0,FDUP,FROT,FSWAP,FAT,FMUL,FSWAP,FEXCL
           db      FSEMI,0
pinst43:
progend:   dw      0
