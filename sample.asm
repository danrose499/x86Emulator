     assume cs:code,ds:code
code segment
     org 100h
start:
; CODE BEGIN
     mov al,1
	 add al,al
	 add al,al
	 add al,al
	 add al,al
	 add al,al
	 add al,al
	 add al,al
	 add al,al
     int 20h
; CODE END  
code ends
     end start 