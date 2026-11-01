cpu 8086

; Chunk header byte: 2 bits type, 6 bits count
;   00xxxxxx = literal: copy the following count bytes
;   01xxxxxx = repeat:  write the following byte count times
;   10xxxxxx = skip:    skip count transparent pixels, count 0 skips to the start of the next line
;   11xxxxxx = end of sprite data
;
; Sprites are small, so most chunks only cover 1 to 4 pixels. Because of that the time is mostly spent
; decoding the chunk headers, not copying pixels. The code is optimized for that:
; - "shl al, 1" moves bit 7 of the header to CF and bit 6 to SF, and sets ZF only for the
;   "skip to next line" header (0x80). So one instruction decodes the whole chunk type.
; - CH is always 0 (rep leaves CX at 0 and nothing else writes CH), so the count only needs
;   to be written to CL.
; - Every handler decodes the next header itself instead of jumping back to a single loop,
;   which saves a jump for most chunks.

; Reads the next header, leaves the count in CL and branches to the skip/end or repeat handler.
; Literal chunks continue after the macro, every expansion is followed by a jump to the literal handler.
%macro DECODE_NEXT_CHUNK 0
    lodsb
    mov cl, al
    and cl, 0x3f            ; count, CH is already 0
    shl al, 1               ; CF = bit 7, SF = bit 6, ZF = header was 0x80
    jc .skip_or_end
    js .repeat
%endmacro

global drawRleSprite_
drawRleSprite_:
    ; data is passed in DX:AX
    ; dest is passed in CX:BX
    ; line width is passed on the stack (2 bytes)
    push ds
    push es
    push si
    push di

    mov ds, dx
    mov si, ax
    mov es, cx
    mov di, bx

    ; dx holds the start of the current line
    mov dx, di

    ; put line width in bx, line width was passed via the stack
    push bp
    mov bp, sp
    mov bx, [bp + 14]
    pop bp

    xor cx, cx  ; CH has to be 0 for the whole routine
    cld         ; rep movsb and rep stosb have to move forward in memory
    jmp .next_chunk

.literal:
    rep movsb
    ; fall through to decode the next chunk

.next_chunk:
    DECODE_NEXT_CHUNK
    jmp .literal

.repeat:
    lodsb
    rep stosb
    DECODE_NEXT_CHUNK
    jmp .literal

.skip_or_end:
    jz .next_line           ; header 0x80: skip to the start of the next line
    js .done                ; header 11xxxxxx: end of sprite data
    add di, cx              ; skip transparent pixels
    DECODE_NEXT_CHUNK
    jmp .literal

.next_line:
    add dx, bx              ; start of the next line
    mov di, dx
    DECODE_NEXT_CHUNK
    jmp .literal

.done:
    pop di
    pop si
    pop es
    pop ds

    retf 2
