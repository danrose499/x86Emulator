     assume cs:code,ds:code
code segment
     org 100h
start:
; CODE BEGIN
     mov al,1
     mov ah,1
     mov cx, 257
     sub ax, cx
     inc ah
     mov bx, ax
     not ax
     or bx, ax
     xor ax, bx
     xchg ax,dx
     dec dx
     neg dx
     int 20h
; CODE END  
code ends
     end start 