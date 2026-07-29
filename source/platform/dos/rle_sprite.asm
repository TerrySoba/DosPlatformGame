cpu 8086

global drawRleSprite_
drawRleSprite_:

    ; data is passed in DX:AX
    ; dest is passed in CX:BX
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
    
    ; extract header type (top 2 bits)
    mov cl, al
    and cl, 0b11000000
    
    ; cmp cl, 0b00000000 ;  this is already done by the previous "and"
    jz .handle_literal
    cmp cl, 0b01000000
    je .handle_repeat
    cmp cl, 0b10000000
    je .handle_skip
    ; cmp cl, 0b11000000  ; we can skip this comparison, as there are only 4 possibilities
    jmp .handle_end
    
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
    mov cx, ax
    add di, cx
    jmp .loop
    
.handle_line_skip:
    ; skip to the next line
    ; the start of the last line is stored in dx, and the line width is stored in bx
    add dx, bx ; move to the start of the next line
    mov di, dx ; set di to the start of the next line
    jmp .loop

.handle_end:
    jmp .done

.done:
    pop di
    pop si
    pop es
    pop ds

    retf