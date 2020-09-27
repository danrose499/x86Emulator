     assume cs:code,ds:code
code segment
     org 100h
start:
; CODE BEGIN
     sub ax,ax ; Loop adding numbers 1 to 10 and storing in AX
     mov cx,1
  l: add ax,cx
     inc cx
     cmp cx,11
     jnz l
     sub bx,bx ; Loop adding 10 to 1 and storing in BX
     mov cx,10
 j:  add bx,cx
     loop j
     int 20h
; CODE END  
code ends
     end start 