	;; Simple disassembler for Elf2K by Al Williams
	;; Should work OK for platforms with BIOS present
	;; otherwise, you'll need to implement call/ret
	;; and the f_* functions used below
include bios.inc
stack: equ stacktop
monitor: equ 8000h


	;; set to DEBUG for breakpoints, XDEBUG for none
#define XDEBUG 1
	;; Set to SIMULATE if you want UART timing, XSIMULATE if not
#define XSIMULATE 1  (calls UART timing setup)

#define ADDRESS ra	
#define SP r2
#define OPCODE r9	
#define PC r3	
#define STRING rf
	
	;; breakpoint if DEBUG is set
breakpoint: macro
#ifdef DEBUG
	hbreakpoint
#endif
	endm
	
	;; Fixed breakpoint
hbreakpoint:macro
        mark
	sep r1
	endm
	
	;; Load 16-bit imm to register
ldir: macro REG,VALUE 
        ldi VALUE.0
	plo REG
	ldi VALUE.1
	phi REG
	endm
	
	
	;; SCRT call
call: macro ADD
	sep scall
	dw ADD
	endm
	
	;; SCRT Return
return: macro
	sep sret
	endm


	;; Set register to 0
zeroreg: macro REG
	ldi 0
	plo REG
	phi REG
	endm

	;; 16 bit shift left
sl16: macro REG
	glo REG
	shl
	plo REG
	ghi REG
	shlc
	phi REG
	endm

	;; 16 bit shift right
sr16: macro REG
	ghi REG
	shr
	phi REG
	glo REG
	shrc
	plo REG
	endm	


	org 0
	; standard initialization
top:    ldir SP,stack
	ldir r6,start   ; set main return address to init SCRT
	lbr f_initcall  ; init SCRT
start:
	sex SP 			; Stack pointer
	
#ifdef SIMULATE			 
	call f_setbd    	;  init UART (debug only)
#endif	

	;; Sign on message
	ldir STRING,msg0
	call f_msg
	zeroreg ADDRESS
	call readhex
	call crlf
readloop:
	;; check for break
	call f_brktest
	bnf continue
	hbreakpoint		; end program here
continue:		
	ghi ADDRESS
	call outbyted
	glo ADDRESS
	call outbyted
	call space
	lda ADDRESS
	plo OPCODE
	call outbyted
	glo OPCODE
	ani 0f0h
	shr
	shr
	shr            		;  high nibble/8 (so 30 -> 06)
	str SP
	shr
	add     		;  3*
	adi jtbl.0
	plo PC			; computed jump
jtbl:
	lbr i0
	lbr i1
	lbr i2
	lbr i3
	lbr i4
	lbr i5
	lbr i6
	lbr i7
	lbr i8
	lbr i9
	lbr ia
	lbr ib
	lbr ic
	lbr id
	lbr ie
	lbr if
	
	;; i0 for instruction 0x, i1 is 1x, etc.
i0:
	glo OPCODE
	ani 0fh
	bnz ildn
	ldir STRING,tidl	; 00=IDL
noarg:		
	call f_msg
	br readloop
	
ildn: 				; 0N=LDN
	ldir STRING,tldn
	;; here RN is printed (reused by many others)
reginst:		
	call f_msg
	glo OPCODE
	ani 0fh
	plo STRING
	call printr
	;; end of line
doneline:		
	call crlf
	br readloop
	
i1:
	ldir STRING,tinc
	br reginst
i2:
	ldir STRING,tdec
	br reginst
i3:
	call space
	;; output next byte (but don't advance)
	ldn ADDRESS  		
	call outbyted
	call space
	call space
	call space
	call space
	ldir STRING, btable
	;; figure out which branch it is
	glo OPCODE
	ani 0fh
i3loop:		
	bz print3		; scan btable until we find it
	adi -1
	inc STRING
	inc STRING
	inc STRING
	br i3loop
print3:	lda STRING		; print it
	call f_type
	lda STRING
	call f_type
	lda STRING
	call f_type
	call space
	ghi ADDRESS
	call outbyted
	lda ADDRESS		; now get next byte and advance
	call outbyted		; print that
	br doneline		; done
i4:
	ldir STRING,tlda
	lbr reginst
i5:
	ldir STRING,tstr
	lbr reginst
	
i6dead:
		;; dead op code
	ldir STRING,twhat
	lbr noarg

i6:				; I/O is odd 60=IRX 61-67, 69-6F and 68 is dead
	glo OPCODE
	xri 68h
	lbz i6dead
i6norm:		
	ani 0fh
	bnz i6io		; 60 is IRX
	ldir STRING,tirx
	lbr noarg
i6io:	sdi 7
	ldir STRING,tout	; assume OUT
	bge io
	ldir STRING,tinp	; Nope, was an INP
io:	
	call f_msg
	glo OPCODE		; print one nibble
	call prhex
	lbr doneline


i7:
	;; 7C, 7D, 7F require byte arguments
	ldi 0
	phi OPCODE  		;  OPCODE is 0 if one byte opcode; or C, D, or F 
	glo OPCODE
	str SP    		;  put low nibbble on TOS
	ldi 7Ch
	sd			; D=(TOS)-7CH (or 7d or 7f)
	bz  i7b
	ldi 7Dh
	sd
	bz i7b
	ldi 7Fh
	sd
	bz i7b
	;; "normal" (no extra byte)
	call space
	call space
	call space
	call space
	br i7norm
i7b:	call space  		; extra byte
	ldi 01
	phi OPCODE		; put non-zero byte in flag
	ldn ADDRESS
	call outbyted
	call space
	call space
	br i7byte
i7norm:	
	call space
i7byte:		
	call space
	call space
	ldir STRING,i7tbl
	glo OPCODE			; get op code again
	ani 0fh  		
i7loop:	
	bz print7		;  locate entry in table
	adi -1
	inc STRING
	inc STRING
	inc STRING
	inc STRING
	br i7loop
print7:	lda STRING			; print table entry
	call f_type
	lda STRING
	call f_type
	lda STRING
	call f_type
	lda STRING
	call f_type
	ghi OPCODE			; get the "one byte flag"
	lbnz doneline
	call space
	lda ADDRESS
	call outbyted
	lbr doneline

i8:
	ldir STRING,tglo
	lbr reginst
i9:
	ldir STRING,tghi
	lbr reginst
ia:
	ldir STRING,tplo
	lbr reginst
ib:
	ldir STRING,tphi
	lbr reginst
	
id:
	ldir STRING,tsep
	lbr reginst
ie:
	ldir STRING,tsex
	lbr reginst

if:

	;; F8, F9 FA FB FC FD and FF take arguments
	ldi 0
	phi OPCODE  		;  OPCODE is 0 if one byte opcode; or C, D, or F 
	glo OPCODE
	xri 0FEh
	bz fsingle
	glo OPCODE
	ani 8
	bz fsingle
	;; two bytes
	call space  		; extra byte
	phi OPCODE			; non-zero flag
	ldn ADDRESS
	call outbyted
	br ifbyte
fsingle:	
	;; "normal" (no extra byte)
	call space
	call space
	call space
ifbyte:		
	call space
	call space
	call space
	call space
	ldir STRING,iftbl
	glo OPCODE			; get op code again
	ani 0fh  		
ifloop:	
	bz printf		;  locate entry in table
	adi -1
	inc STRING
	inc STRING
	inc STRING
	br ifloop
printf:	lda STRING			; print table entry
	call f_type
	lda STRING
	call f_type
	lda STRING
	call f_type
	ghi OPCODE			; get the "one byte flag"
	lbz doneline
	call space
	lda ADDRESS
	call outbyted
	lbr doneline


ic:
	;; annoyingly enough 012389AB take an extra 2 byte
	;; 567CDEF do not take any bytes
	;; 4 is apparently not used
	ldi 0
	phi OPCODE    		;  extra flag
	glo OPCODE
	str SP   		; put at top of stack
	;;  look for special cases
	ldi 0c4h
	sd
	bz csingle
	ldi 0c5h
	sd
	bz csingle
	ldi 0c6h
	bz csingle
	ldi 0c7h
	bz csingle
	ldi 0cch
	bz csingle
	ldi 0cdh
	bz csingle
	ldi 0ceh
	bz csingle
	ldi 0cfh
	bz csingle
	;; multibyte
	ldi 1
	phi OPCODE  		; set flag

	call space
	lda ADDRESS  		; get next byte
	call outbyted
	ldn ADDRESS
	call outbyted
	dec ADDRESS
	call space
	call space
	br icgo
csingle:
	call space
	call space
	call space
	call space
	call space
	call space
	call space	
icgo:		
	ldir STRING, cbtable  	;  find right entry in cbtable
	glo OPCODE
	ani 0fh
icloop:		
	bz printc
	adi -1
	inc STRING
	inc STRING
	inc STRING
	inc STRING
	br icloop
printc:	lda STRING		; print it
	call f_type
	lda STRING
	call f_type
	lda STRING
	call f_type
	lda STRING
	call f_type
	ghi OPCODE		; check flag
	lbz doneline
	call space
	lda ADDRESS		; print next bytes
	call outbyted
	lda ADDRESS
	call outbyted		
	lbr doneline
	

	;; print a space
space: ldi ' '
       br emit
	;; print a crlf
crlf:  ldi 13
       call f_type
       ldi 10
emit:  call f_type
       return
	   
	;; Print register Rn (n in RF)
printr: 
	ldi 'R'
	call f_type
	glo STRING
	br prhex   		;  just over the boundry :(
	
	;;  output byte in D
outbyted: plo STRING	
outbyte: ; print a hex byte in STRING
	glo STRING
	ani 0f0h
	shr
	shr
	shr
	shr
	call prhex
	glo STRING
	ani 0fh
	
	;; Print  a hex digit in D
prhex:		
	ani 0fh
	adi 30h
	str SP
	sdi 39h
	ldn SP
	bge prhex09
	adi 7   		;  adjust A-F
prhex09:
	call f_type
	return
	
	;; This message appears here so the next block of code will hit the next page
msg0: db 13,10,'1802 Disassembler V1 by Al Williams alw@al-williams.com',13,10,'Start Address?',0
	   
	;; if readhex sees a backspace it comes here  
backspace:
	sr16 ADDRESS
	sr16 ADDRESS
	sr16 ADDRESS
	sr16 ADDRESS
	
readhex: 			; read hex number to ra (uses OPCODE)
	call f_read
	plo OPCODE
	xri 08  		;  backspace
	bz backspace
	glo OPCODE
	xri 0dh			; CR?
	bz done
skipdone:	
; shift ADDRESS 4 places left
	sl16 ADDRESS
	sl16 ADDRESS
	sl16 ADDRESS
	sl16 ADDRESS
	breakpoint
; get character back
	glo OPCODE
	sdi 03ah  ; >'9'?
	glo OPCODE
	bl alphahex
; digit
	breakpoint
        adi -30h
	br  addhex
alphahex:	
	breakpoint
	ani 0dfh  ; make lc upper case
	adi -37h  ; make A->10
addhex:
    breakpoint
	str SP   ; put on top of stack temporarily
	glo ADDRESS
	add
	plo ADDRESS
	ldi 0
	str SP
	ghi ADDRESS
	adc
	phi ADDRESS
	br readhex
done:
    return 


tidl: db '       IDL',13,10,0
tldn: db '       LDN ',0
tinc: db '       INC ',0
tdec: db '       DEC ',0
tlda: db '       LDA ',0
tstr: db '       STR ',0
tglo: db '       GLO ',0
tghi: db '       GHI ',0
tplo: db '       PLO ',0
tphi: db '       PHI ',0
tsep: db '       SEP ',0
tsex: db '       SEX ',0			
tirx: db '       IRX',13,10,0	
tout: db '       OUT ',0
tinp: db '       INP ',0		
twhat: db '       ???',13,10,0
btable:	db 'BR BQ BZ BDFB1 B2 B3 B4 BM BNQBNZBNFBN1BN2BN3BN4'
i7tbl:	db 'RET DIS LDXASTXDADC SDB SHRCSMB SAV MARKREQ SEQ ADCISDBISHLCSMBI'
cbtable:	db 'LBR LBQ LBZ LBDFNOP LSNQLSNZLSNFNLBRLBNQLBNZLBNFLSIELSQ LSZ LSDF'
iftbl:	db 'LDXOR ANDXORADDSD SHRSM LDIORIANIXRIADISDISHLSMI'	



	
stacktop:  equ	$+300h
	end top
	