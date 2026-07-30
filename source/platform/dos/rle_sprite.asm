cpu 8086

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

    xor ax,ax ; clear ax for later usage

    ; store the start of the current line for later usage
    mov dx, di ; store the start of the current line in dx for later usage

    ; put line width in bx for later usage, line width was passed via the stack
    push bp
    mov bp, sp
    mov bx, [bp + 14] ; store line width in bx for later usage
    pop bp


.loop:
    ;mov al, [si]
    ;inc si
    lodsb

    ; Branch on header range directly:
    ; 00xxxxxx = literal, 01xxxxxx = repeat, 10xxxxxx = skip, 11xxxxxx = end
    cmp al, 0b01000000
    jb .handle_literal
    cmp al, 0b10000000
    jb .handle_repeat
    cmp al, 0b11000000
    jb .handle_skip
    jmp .done
    
.handle_literal:
    ; handle literal data
    ; low 6 bits = count
    and al, 0b00111111
    mov cx, ax
    rep movsb
    jmp .loop
    
.handle_repeat:
    ; handle repeat data
    ; low 6 bits = count
    and al, 0b00111111
    mov cx, ax
    ; mov al, [si]
    ; inc si
    lodsb
    rep stosb
    jmp .loop
    
.handle_skip:
    ; handle skip (transparent pixels)
    ; low 6 bits = count
    and al, 0b00111111
    jz .handle_line_skip ; if count is zero, we skip to the next line
    add di, ax
    jmp .loop
    
.handle_line_skip:
    ; skip to the next line
    ; the start of the last line is stored in dx, and the line width is stored in bx
    add dx, bx ; move to the start of the next line
    mov di, dx ; set di to the start of the next line
    jmp .loop

.done:
    pop di
    pop si
    pop es
    pop ds

    retf 2