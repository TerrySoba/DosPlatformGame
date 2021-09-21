; 2014-07-12: Ported to NASM by @fanzyflani. Also contains a player!
; Stick a filename in here if you insist (or %define it elsewhere)
; (50Hz and 18.2Hz modules are detected and the IRQ0 timer adjusted accordingly)
%ifndef RAD_NO_DEFAULT_PLAYER
%ifndef RAD_MODULE_NAME
%define RAD_MODULE_NAME "alloyrun.rad"
%endif
%endif
; Or %define RAD_NO_DEFAULT_PLAYER in your code.
;
; I make no copyright claims on any of my modifications to this code.
; (Also, the minimum CPU requirement is 80186, not 80286.)
;
; AND NOW WE RETURN TO YOUR REGULAR PROGRAMMING.

;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;          ú
;          . ÛÛÛßßÛÜ  ÜÛÛßßßß  ÜÛßßÜ  ÛÛÛ     ÛÛÛ ÛÛÛÛÛÜ  ÛÛ  ÛÛÛ ú
;          : ÛÛÛÜÜÛß  ÛÛÛÜÜ   ÞÛÛÜÜÛÝ ÛÛÛ     ÛÛÛ    ßÛÛÝ ßÛÜÜÛÛÛ :
;          ³ ÛÛÛ ßÛÛÜ ÛÛÛ     ÛÛÛ  ÛÛ ÞÛÛÜ    ÛÛÛ     ÛÛÛ     ÛÛÛ |
;  Ú Ä ÄÍÄÍ¼ ßßß   ßßß ßßßßßß ßßß  ßß  ßßßßßß ßßß     ßßß ßßßßßß  ÀÄÄ Ä -ÄÄÄ¿
;  ³ ÜÛßßÜ ÛÛßßÜ ÛÛ    ÛÛ ÛÛßßÜ   ßßßÛÜ ÛÛßßÜ ÜÛßßÜ ÜÛßßÜ ÛÛ  Û ÜÛßßß ÛÛßßÜ :
;  | ÛÛßßÛ ÛÛ  Û ÛÛ    ÛÛ ÛÛßßÜ      ÛÛ ÛÛßßÜ ÛÛßßÛ ÛÛ  Ü ÛÛßßÜ ÛÛß   ÛÛßßÜ .
;  : ßß  ß ßßßß   ßßßß ßß ßßßß       ßß ßß  ß ßß  ß  ßßß  ßß  ß  ßßßß ßß  ß ú
;                       Play-routine Code  version 1.1a
;
;                        by SHAYDE/REALITY Feb, Apr 95
;
;                                    - * -
;
; Feel free to use/hack this code about as much as you like.  In the good old
; dayz of Amiga, ALL tracker writers gave away player source-code so that the
; coder could do what he/she wanted with it.  On PC every tracker writer thinks
; their player code should be protected and they either don't release a player
; or they release it in .OBJ format which means if you need to make changes to
; the code to fit in with your demo/intro you're fucked!!!  So message to all
; tracker writers out there:
; FOR THE SAKE OF CODER SANITY, ALWAYS RELEASE PLAYER CODE FOR YOUR TRACKERS!!
; OTHERWISE WOT'S THE POINT OF WRITING A TRACKER?!?!??!?!  And release it in
; source-code form to reduce head-aches!
;
;				     - * -
;
; This source-code doesn't contain any segment directives so it is only
; INCLUDEable in other source-code.  Also it requires a minimum of a 286
; to run.  I avoided using ASSUMEs so that the code that INCLUDEs this code
; doesn't lose it's ASSUMEs, hence variables are accessed via CS:.  You can
; save a few bytes by dropping them (which you'll need to do if you want to
; use this player in protected-mode), although I use DS: to reference the
; tune segment.
;
;				     - * -
;
; Hey, 'scuse the ugliness of the listing.  I'm a coder, not an artist!
;
;				     - * -
;
; INSTRUCTIONS FOR USE:
;
;	To initialise the player, call "InitPlayer".
;	To stop play, call "EndPlayer".
;	To play music, call "PlayMusic" 50 times a second (18.2/sec for a
;					"slow-timer" tune).
;
;				     - * -
;
; BUG FIXES (Arrrrrghhhh!!! :-)
;
;   V1.1a Apr95:
;	Fuck!  Sorry dudez!  Real stupid bug.  You see, RAD itself plays the
;	note and then does the effect straight away.  But this 'ere source
;	code played the note then did the effect one beat later.  I didn't
;	notice it in testing cos it's marginal, but it fucks up the slides
;	slightly and with real complex slides (like in some of Void's latest
;	AWESOME tunez) it can stick out like elephant's balls!  I've now moved
;	the call to UpdateNotes to the end of the routine so it's executed
;	EVERY beat instead of every beat except note beats (make sense?  No?
;	GOOD! :-)
;
;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²

; Tracker commands
%define cmPortamentoUp	 	1		; Portamento up
%define cmPortamentoDwn	 	2		; Portamento down
%define cmToneSlide	 	3		; Tone Slide: xx is speed of slide
%define cmToneVolSlide	 	5		; Tone slide of 00 + Vol. Slide
%define cmVolSlide	 	10		; Volume Slide: <50 down, >50 up
%define cmSetVol	 	12		; set volume
%define cmJumpToLine	 	13		; Jump to line in next track
%define cmSetSpeed	 	15		; set speed

%define FreqStart	 	156h		; low end of frequency in each octave
%define FreqEnd		 	2aeh		; high end of frequency in each octave
%define FreqRange	 	FreqEnd-FreqStart

	bits 16
%ifndef RAD_NO_DEFAULT_PLAYER
; Test code
	org 0x0100
	cpu 186

rad_start:
	; Get segment
	mov ax, test_module
	shr ax, 4
	mov bx, ds
	add ax, bx
	mov es, ax

	; Detect speed
	xor ax, ax
	mov [timer_counter], ax
	dec ax ; This is necessary as it likes to break with a timer_increment of 0.
	test byte [es:0x11], 0x40
	jnz .is_slow
		mov ax, 0x5D37
	.is_slow:
	mov [timer_increment], ax
	
	call InitPlayer
	jnc .nofailmod
		int 0x20
	.nofailmod:

	; DISABLE INTERRUPTS
	cli

	; Prepare timer hook
	xor ax, ax
	mov es, ax
	mov ax, cs
	mov cx, word [es:0x0020]
	mov dx, word [es:0x0022]
	mov word [es:0x0020], irq_timer
	mov word [es:0x0022], ax
	mov word [old_irq_timer+0x00], cx
	mov word [old_irq_timer+0x02], dx

	; Adjust timer
	mov ax, [timer_increment]
	test ax, ax
	jz nofixtimer
		mov al, 0x36
		out 0x43, al
		mov ax, [timer_increment]
		out 0x40, al
		mov al, ah
		out 0x40, al
	nofixtimer:

	; ENABLE INTERRUPTS
	sti

	; Now wait for key press!
	mov ah, 0
	int 0x16

	; OK, kill it all off

	; DISABLE INTERRUPTS
	cli

	; Restore timer
	mov al, 0x36
	out 0x43, al
	mov al, 0xFF
	out 0x43, al
	out 0x43, al

	; Restore hook
	xor ax, ax
	mov es, ax
	mov cx, [old_irq_timer+0x00]
	mov dx, [old_irq_timer+0x02]
	mov [es:0x0020], cx
	mov [es:0x0022], dx

	; ENABLE INTERRUPTS
	sti

	; Kill player
	call EndPlayer

	; Return!
	ret

irq_timer:
	; Prelude
	my_push
	push ds
	push es
	mov ax, cs
	mov ds, ax
	mov es, ax

	; Update song
	call PlayMusic

	; Check if we do a hook call
	mov ax, [cs:timer_increment]
	add [cs:timer_counter], ax
	jnc .nohook
		; Call hook
		pushf
		call far [cs:old_irq_timer]
	
	.nohook:

	; Acknowledge interrupt and return
	mov al, 0x20
	out 0x20, al
	pop es
	pop ds
	my_popa
	iret

	align 16
test_module: incbin RAD_MODULE_NAME
%endif

	cpu 8086

%macro  my_pusha 0
        push di 
		push si 
		push bp 
		push sp 
		push bx 
		push dx 
		push cx
%endmacro

%macro  my_popa 0
		pop cx
		pop dx
		pop bx
		pop bp ; this is NOT a mistake. This would be sp, but that is not restored.
		pop bp
		pop si
		pop di
%endmacro

;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; This routine initialises the player.
; IN:
;	ES:	- points to .RAD module to play
; OUT:
;	Carry	- set on error (such as invalid module)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
InitPlayer:	
		; pusha
		my_pusha

		call	EndPlayer	; clear Adlib ready for tune

	; initialise certain Adlib registers that aren't changed
		mov	ax,0120h	; allow waveforms
		call	Adlib
		mov	ax,0800h
		call	Adlib
		mov	ah,0bdh		; no drums, etc.
		call	Adlib

	; check to see if it is a RAD file first
		cmp	word [es:0],'RA'
		jnz	.err
		cmp	word [es:2],'D '
		jnz	.err
		cmp	byte [es:16],10h		; correct version?
		jnz	.err

		mov	[cs:ModSeg],es		; keep the segment of module

	; read initial speed
		mov	al,[es:17]
		mov	ah,al
		and	al,1fh
		mov	[cs:Speed],al

	; see if there's a description to skip
		mov	si,18
		test	ah,80h			; description flag
		jz	.lc			; no description

		xor	al,al
		jmp	.le

	.ld:	inc	si
	.le:	cmp	[es:si],al		; look for null-termination
		jnz	.ld
		inc	si			; move past null

	; create table of instrument pointers
	.lc:	xor	bx,bx

	.la:	mov	bl,[es:si]		; instrument no.
		inc	si
		add	bx,bx
		jz	.lb			; no more instruments

		mov	[cs:InstPtrs-2+bx],si	; record pointer to instrument
		add	si,11
		jmp	.la

	; record offset of order list
	.lb:	xor	ax,ax
		mov	al,[es:si]		; no. of orders in order-list
		mov	[cs:OrderSize],ax
		inc	si
		mov	[cs:OrderList],si
		xor	bx,bx
		mov	bl,[es:si]		; first pattern to play
		add	bx,bx
		add	si,ax			; move to end of list

	; record table of pattern offsets
		mov	[cs:PatternList],si
		mov	ax,[es:si+bx]		; first pattern offset
		mov	[cs:PatternPos],ax	; pointer to first pattern

	; initial pointers
		xor	ax,ax
		mov	[cs:OrderPos],ax		; start at position 0.
		mov	[cs:SpeedCnt],al
		mov	[cs:Line],al		; start at line 0

		clc
		jmp	.lx			; successful initialisation

	.err:	stc
	.lx:	my_popa
		ret





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; This stops music playback (stops sound channels).
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
EndPlayer:	push	ax
		mov	ax,020h << 8
	.la:	call	Adlib
		inc	ah
		cmp	ah,00f6h
		jb	.la
		pop	ax
		ret





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; This routine does the actual playing.  It MUST be called 50 times a second
; to maintain accurate music playback.  Refer to accompanying timer source-code
; for ways of providing a 50/sec timer service.
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PlayMusic:
		my_pusha
		push	ds

		mov	ds,[cs:ModSeg]	; segment of module

		cmp	byte [cs:SpeedCnt],0
		jz	.la		; play a line of music
		dec	byte [cs:SpeedCnt]
		jmp	.lx		; no new line, so just update effects

	; switch off any effects that are in operation
	.la:	mov	si,8
		xor	al,al

	.laa:	mov	[cs:PortSlide+si],al	; reset any slides
		mov	[cs:VolSlide+si],al	; reset any slides
		mov	[cs:ToneSlide+si],al	; reset any slides
		dec	si
		jns	.laa

	; playing a new line, PatternPos should have been set-up already
		mov	si,[cs:PatternPos]
		or	si,si
		jz	.lb		; rest of this pattern is blank

		mov	al,[si]		; line indicator
		and	al,7fh		; eliminate bit 7
		cmp	al,[cs:Line]	; is this current line?
		jnz	.lb		; haven't reached it yet

		test	byte [si],80h	; last line?
		jz	.lc		; no, still more to check
		mov	word [cs:PatternPos],0	; mark rest of pattern as blank

	.lc:	inc	si		; move to first channel

	; play channels
	.lf:	mov	cl,[si]		; channel we are processing
		push	cx
		and	cl,7fh		; get rid of bit 7
		mov	ax,[1+si]	; AL=octave/note, AH=inst/command
		add	si,3

		test	ah,15		; if there's a cmd, there'll be a param.
		jz	.le		; no parameter byte
		mov	ch,[si]		; read parameter
		inc	si

	.le:	call	PlayNote	; play the note

		pop	cx
		jc	.lg		; skip rest of line, AX has new line

		test	cl,80h		; last channel to play?
		jz	.lf		; not yet

		mov	[cs:PatternPos],si; keep position in crunched track

	; update pointers
	.lb:	mov	al,[cs:Speed]	; needs to be set AFTER note playing
		dec	al
		mov	[cs:SpeedCnt],al	;    for new speeds to take effect!

		inc	byte [cs:Line]
		cmp	byte [cs:Line],64	; end of pattern?
		jb	.lx		; nope

		mov	byte [cs:Line],0	; top of next pattern
		call	NextPattern

	; now update effects (effect is acted upon straight away)
	.lx:	call	UpdateNotes

		pop	ds
		my_popa
		ret
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
	; jump to line AX
	.lg:	mov	bl,[cs:Speed]	; needs to be set AFTER note playing
		mov	[cs:SpeedCnt],bl	;    for new speeds to take effect!

		mov	[cs:Line],al

		; find start of next pattern
		call	NextPattern
		jz	.lx		; there isn't any data in next pattern

		; find line that is greater or equal to the current line
	.ll:	mov	cl,[si]		; line id.
		and	cl,7fh		; ignore bit 7
		cmp	cl,al
		jae	.lh		; found line

		test	byte [si],80h
		jz	.li		; not last line
		xor	si,si
		jmp	.lh		; ignore rest of pattern as it's last

		; skip to next line definition
	.li:	inc	si
	.lj:	mov	cl,[si]
		add	si,3
		test	byte [cs:si-1],15	; is there a valid command?
		jz	.lk
		inc	si		; skip parameter

	.lk:	add	cl,cl
		jnc	.lj		; wasn't last channel spec.
		jmp	.ll		; check next line

	.lh:	mov	[cs:PatternPos],si
		jmp	.lx





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Advances pointers to next pattern in order list.
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
NextPattern:	mov	bx,[cs:OrderPos]
		inc	bx
		cmp	bx,[cs:OrderSize]
		jb	.ld
		xor	bx,bx		; end of tune, move back to start

	.ld:	mov	[cs:OrderPos],bx
		mov	si,[cs:OrderList]
		mov	bl,[si+bx]	; no. of next pattern

		test	bl,80h
		jz	.lda
		and	bl,7fh
		jmp	.ld		; bit 7 = jump to new order

	.lda:	mov	si,[cs:PatternList]
		add	bx,bx
		mov	si,[si+bx]	; offset of next pattern
		mov	[cs:PatternPos],si
		or	si,si
		ret





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Plays a note on a channel.
; IN:
;	AL	- Octave (high nibble), Note (low nibble)
;	AH	- instrument (high nibble), command (low nibble)
;	CL	- channel to play note on (0..8)
;	CH	- parameter byte if command is non-zero
; OUT:
;	CARRY	- set if a line is to be jumped to
;	AX	- line to jump to if CARRY set
; Note: don't use SI or segment regs., otherwise registers do not need saving.
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PlayNote:	mov	di,cx
		and	di,15
		mov	dh,ah
		and	dh,15		; command

		or	al,al
		jz	.lb		; no note playing, process command

	; check to see if we are actually performing a tone slide
		cmp	dh,cmToneSlide
		jnz	.lt		; nope, play note

		; note/octave are used as parameters then (instrument ignored)
		mov	bx,ax
		and	bx,15		; note
		shr	al,1
		shr	al,1
		shr	al,1
		shr	al,1
		and	ax,7		; octave
		dec	bx		; we want 1..12
		cmp	bx,12
		jae	.lx		; not a valid note (probably KEY-OFF)

		mov dx, FreqRange
		imul dx	; scale octave

		add	bx,bx
		add	ax,[cs:NoteFreq+bx]	; add frequency of this note
		sub	ax,FreqStart	; so range starts from zero
		mov	[cs:ToneSlideFreqL+di],al	; destination frequency
		mov	[cs:ToneSlideFreqH+di],ah

		; set tone slide speed
		mov	byte [cs:ToneSlide+di],1	; switch tone slide on
		or	ch,ch
		jz	.lx		; use last speed setting
		mov	[cs:ToneSlideSpeed+di],ch
		jmp	.lx

	; KEY-OFF the previous note
	.lt:	push	ax
		mov	al,[cs:OldB0+di]	; old register value
		and	al, ~ 20h	; clear KEY-ON bit
		mov	[cs:OldB0+di],al	; so slides after KEYOFF work correctly
		mov	ah,cl
		add	ah,0b0h
		call	Adlib
		pop	ax

	; load instrument (if any)
		mov	dl,ah
		add	al,al
		rcr	dl,1
		shr	dl,1		; instrument no.
		shr	dl,1		; instrument no.
		shr	dl,1		; instrument no.
		jz	.la		; no instrument to load
		call	LoadInst

	; load note into channel
	.la:	mov	bl,al
		and	bx,15*2		; note * 2
		cmp	bx,15*2
		jz	.lb		; just a KEY-OFF so we're done

		mov	bx,[cs:NoteFreq-2+bx]	; frequency of note (BX-1)
		shr	al,1		; octave
		shr	al,1		; octave
		shr	al,1		; octave
		and	al,7*4
		or	al,20h		; KEY-ON
		or	al,bh		; Frequency high byte
		mov	ah,0b0h
		add	ah,cl
		mov	[cs:OldB0+di],al	; record the register value
		push	ax

		sub	ah,10h
		mov	al,bl		; Frequency low byte
		mov	[cs:OldA0+di],al
		call	Adlib

		pop	ax
		call	Adlib

	; process command (if any), DH has command, CH has parameter
	.lb:	xor	bx,bx
		mov	bl,dh		; command
		add	bx,bx
		jmp	[cs:.Effects+bx]

	.lx:	clc
	.lxx:	ret
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Portamento up
.PortUp:	mov	[cs:PortSlide+di],ch
		jmp	.lx
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Portamento down
.PortDown:	neg	ch
		mov	[cs:PortSlide+di],ch
		jmp	.lx
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Tone slide to note (no note supplied)
.ToneSlide:	or	ch,ch		; parameter has speed of tone slide
		jz	.lja		; keep last tone slide speed
		mov	[cs:ToneSlideSpeed+di],ch

	.lja:	mov	byte [cs:ToneSlide+di],1	; tone slide on
		jmp	.lx
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Volume slide & Volume + Tone Slide
.ToneVolSlide:
.VolSlide:	cmp	ch,50		; <50 = slide down, >50 = slide up
		jb	.lga
		sub	ch,50
		neg	ch

	.lga:	mov	[cs:VolSlide+di],ch

		cmp	dh,cmToneVolSlide	; just plain volume slide
		jnz	.lx
		mov	byte [cs:ToneSlide+di],1	; tone slide on
		jmp	.lx
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Set volume
.SetVolume:	call	SetVolume	; CH has volume, CL has channel
		jmp	.lx
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; jump to line in next pattern
.JumpToLine:	cmp	ch,64
		jae	.lx		; ignore as it is invalid
		xor	ax,ax
		mov	al,ch
		stc
		ret			; skip rest of channels
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Set speed
.SetSpeed:	mov	[cs:Speed],ch
		jmp	.lx
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.Effects	dw	.lx
		dw	.PortUp
		dw	.PortDown
		dw	.ToneSlide
		dw	.lx
		dw	.ToneVolSlide
		dw	.lx
		dw	.lx
		dw	.lx
		dw	.lx
		dw	.VolSlide
		dw	.lx
		dw	.SetVolume
		dw	.JumpToLine
		dw	.lx
		dw	.SetSpeed

NoteFreq	dw	16bh,181h,198h,1b0h,1cah,1e5h	; 156h = C
		dw	202h,220h,241h,263h,287h,2aeh





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Check each channel for ongoing effects to update.
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
UpdateNotes:	xor	bh,bh		; channel index
		xor	si,si

	; process portamentos
	.la:	mov	bl,[cs:PortSlide+si]
		or	bl,bl
		jz	.lb		; no slide for this channel
		call	GetFreq
		mov	ch,bl
		mov cl, 8
		sar	cx, cl		; sign extend 8bit->16bit
		add	ax,cx
		call	SetFreq

	; process volume slides
	.lb:	mov	ch,[cs:VolSlide+si]
		mov	cl,[cs:Old43+si]	; contains current volume
		and	cl,3fh
		xor	cl,3fh
		or	ch,ch
		jz	.lc
		jns	.lba

		; slide volume up
		sub	cl,ch
		cmp	cl,64
		jb	.lbb
		mov	cl,63
		jmp	.lbb

		; slide volume down
	.lba:	sub	cl,ch
		jns	.lbb
		xor	cl,cl

	.lbb:	mov	ch,cl
		mov	cl,bh		; channel to set
		call	SetVolume

	; process tone slides
	.lc:	cmp	byte [cs:ToneSlide+si],0
		jz	.lx		; no tone slide
		mov	bl,[cs:ToneSlideSpeed+si]	; shouldn't get wiped uc

		; get current absolute frequency
		call	GetFreq

		; sign extend speed/direction
		mov	dh,bl
		mov cl, 8
		sar	dx,cl

		; get destination frequency
		mov	cl,[cs:ToneSlideFreqL+si]
		mov	ch,[cs:ToneSlideFreqH+si]
		cmp	ax,cx
		jz	.le		; already at destination?!
		ja	.ld		; tone slide down (source > dest)

		; doing a tone slide up
		add	ax,dx
		cmp	ax,cx
		jb	.lg		; still under destination
		jmp	.le		; reached destination

		; doing a tone slide down
	.ld:	sub	ax,dx
		cmp	ax,cx
		ja	.lg		; still over destination

		; reached destination so stop tone slide
	.le:	mov	ax,cx		; clip it onto destination
		mov	byte [cs:ToneSlide+si],0	; disables tone slide

		; write new frequency back to channel
	.lg:	call	SetFreq

	.lx:	inc	bh
		inc	si
		cmp	si,9
		jb	.la
		ret





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Returns the current absolute frequency of channel
; IN:
;	SI	- channel
; OUT:
;	AX	- frequency
; USES:
;	CX, DX
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
GetFreq:	mov	cl,[cs:OldA0+si]
		mov	ch,[cs:OldB0+si]
		and	ch,3		; mask to get high frequency
		sub	cx,FreqStart
		mov	al,[cs:OldB0+si]
		shr	al,1
		shr	al,1
		and	ax,7		; mask to get octave
		mov	dx,FreqRange
		mul	dx
		add	ax,cx
		ret





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Sets the channel's frequency
; IN:
;	AX	- absolute frequency
;	SI	- channel
; USES:
;	CX, DX
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
SetFreq:	mov	cx,FreqRange
		xor	dx,dx
		div	cx		; extracts octave in AX and freq. in DX
		add	dx,FreqStart

		mov	ah,[cs:OldB0+si]
		and	ah,11100000b	; keep old toggles
		shl	al,1		; move octave to correct bit position
		shl	al,1		; move octave to correct bit position
		or	al,ah		; insert octave
		or	al,dh		; insert high frequency
		mov	ah,bh
		add	ah,0b0h
		mov	[cs:OldB0+si],al
		call	Adlib

		sub	ah,10h
		mov	al,dl		; low frequency
		mov	[cs:OldA0+si],al
		jmp	Adlib





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Load in instrument data into a given channel.
; IN:
;	CL	- channel to load instrument into (0..8)
;	DL	- instrument no. (1..31)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
LoadInst:	push	ax
		push	bx
		push	si

		mov	si,cx
		and	si,0ffh
		mov	ah,[cs:ChannelOffs+si]	; Adlib register offsets

		xor	bx,bx
		mov	bl,dl
		dec	bx
		add	bx,bx
		mov	bx,[cs:InstPtrs+bx]	; get instrument offset
		or	bx,bx
		jz	.lx		; no instrument data ?!

		mov	al,[2+bx]
		mov	[cs:Old43+si],al	; old 43.. value

		mov	dl,4

	.la:	mov	al,[1+bx]
		call	Adlib		; load carrier
		add	ah,3
		mov	al,[bx]
		call	Adlib		; load modulator
		add	bx,2

		add	ah,20h-3
		dec	dl
		jnz	.la

		add	ah,40h		; do E0 range now
		mov	al,[2+bx]
		call	Adlib
		add	ah,3
		mov	al,[1+bx]
		call	Adlib

		mov	ah,0c0h
		add	ah,cl
		mov	al,[bx]
		call	Adlib

	.lx:	pop	si
		pop	bx
		pop	ax
		ret

ChannelOffs	db	20h,21h,22h,28h,29h,2ah,30h,31h,32h





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Outputs a value to an ADLIB register.
; IN:
;	CL	- channel to set volume on
;	CH	- new volume
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
SetVolume:	push	ax
		push	bx

		xor	bx,bx
		mov	bl,cl

	; ensure volume is within range
		cmp	ch,64
		jb	.la
		mov	ch,63

	; get old 43.. value
	.la:	mov	al,[cs:Old43+bx]
		and	al,0c0h		; mask out volume bits
		xor	ch,3fh
		or	al,ch		; insert volume
		mov	[cs:Old43+bx],al	; keep new 43.. value

	; write new volume into Adlib
		mov	ah,[cs:ChannelOffs+bx]
		add	ah,23h
		call	Adlib

		pop	bx
		pop	ax
		ret





;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
; Outputs a value to an ADLIB register.
; IN:
;	AH	- register no.
;	AL	- value
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Adlib:		push	ax
		push	dx

		mov	dx,[cs:AdlibPort]
		xchg	ah,al
		out	dx,al
		in	al,dx
		in	al,dx
		in	al,dx
		in	al,dx
		in	al,dx
		in	al,dx

		inc	dx
		mov	al,ah
		out	dx,al
		dec	dx
		mov	ah,22
	.la:	in	al,dx
		dec	ah
		jnz	.la

		pop	dx
		pop	ax
		ret






; Variables
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
AdlibPort	dw	388h		; default Adlib base port

InstPtrs	times	31 dw 0	; offsets of instrument data
Old43		times	9 db 0	; record of 43..   register values
OldA0		times	9 db 0	; record of A0..A8 register values
OldB0		times	9 db 0	; record of B0..B8 register values

ToneSlideSpeed	times	9 db 1	; speed of tone slide

	section .bss
ToneSlideFreqL	resb	9	; destination frequency of tone slide
ToneSlideFreqH	resb	9

ToneSlide	resb	9	; tone slide flag
PortSlide	resb	9	; portamento slide
VolSlide	resb	9	; volume slide

ModSeg		resw	1	; segment of module (starts at offset 0)
Speed		resb	1	; speed (n/50Hz) of tune
SpeedCnt	resb	1	; counter used for deriving speed

OrderSize	resw	1	; no. of entries in Order List
OrderList	resw	1	; offset in module of Order List
OrderPos	resw	1	; current playing position in Order List

PatternList	resw	1	; offset of pattern offset table in module
PatternPos	resw	1	; offset to current line in current pattern
Line		resb	1	; current line being played (usually +1)

	; end of PLAYER.ASM base - define your stuff here!
%ifndef RAD_NO_DEFAULT_PLAYER
old_irq_timer: resw 2
timer_increment: resw 1
timer_counter: resw 1
%endif
	section .data

