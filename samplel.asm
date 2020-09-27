     assume cs:code,ds:code
code segment
     org 100h
start:
; CODE BEGIN
         sub ax,ax
         mov cx,10
   l:    add ax,cx
         loop l
         int 20h
; CODE END  
code ends
     end start 