%include "player_nasm.asm"

    cpu 8086

global radInitPlayer_
radInitPlayer_:
    mov cl, 4
    shr ax, cl  ; ax contains offset
    add ax, dx  ; dx contains segment
    mov es, ax

    call InitPlayer
    jc .error    ; InitPlayer sets carry flag if error occured
    .success:
    mov ax, 01h  ; return true
    retf
    .error:
    mov ax, 00h  ; return false
    retf

global radPlayMusic_
radPlayMusic_:
    call PlayMusic
    retf

global radEndPlayer_
radEndPlayer_:
    call EndPlayer
    retf




