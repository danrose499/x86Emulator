## x86 Assembly Emulator

This is a simple Assembly emulator, intended as an educational tool to demonstrate how different x86 instructions affect the registers. The program reads a binary assembly file bit-by-bit into a switch which supports many of the common used instructions. Also included are several assembly test files, in text, binary, and lst formats (for seeing how the different instructions relate to their respective opcdes).
#### Getting Started:

In order to change the file you would like to emulate, edit the file __emulator.cpp__ and change line 28 such that the name of the binary file you would like to test is in the quotes. The file currently set to open is __SAMPLE.COM__, as seen below.

```C++
FILE* f = fopen("SAMPLE.COM", "rb");
```
After reading the file, a while loop passes each byte of the file into a switch which performs the appropraite instruction given. 
Before each itteration of the loop, the contents of the following register are displayed:
AX,BX,CX,DX,SP,DI,BP,SI
Additionally, the following bits of FLAGS are displayed:
ZF,SF,CF.
 
For example, the default file, __SAMPLE.COM__ contains the following instructions:
```Assembly
  mov al, 1
  add al,al
  add al,al
  add al,al
  add al,al
  add al,al
  add al,al
  add al,al
  add al,al
  int 20h
 ```
As seen above, the program loads the AL register with a value of 1 and adds the contents of the register with itself 8 times. Aftter the 8th addition, the value should be 256, but, as AL is an 8-bit register, it overflows and the flags are set accordingly. When emulating the file in __emulator.cpp__, the following output is produced:
 
 ```
Displaying state of registers after each instruction:

Update #1 (IP: 256)
AX: 0000000000000000 = 0
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0

Update #2 (IP: 258)
AX: 0000000000000001 = 1
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0

Update #3 (IP: 260)
AX: 0000000000000010 = 2
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0    
DI: 0000000000000000 = 0    
BP: 0000000000000000 = 0    
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0     

Update #4 (IP: 262)
AX: 0000000000000100 = 4    
BX: 0000000000000000 = 0    
CX: 0000000000000000 = 0    
DX: 0000000000000000 = 0    
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0

Update #5 (IP: 264)
AX: 0000000000001000 = 8
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0

Update #6 (IP: 266)
AX: 0000000000010000 = 16
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0

Update #7 (IP: 268)
AX: 0000000000100000 = 32
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0

Update #8 (IP: 270)
AX: 0000000001000000 = 64
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 0

Update #9 (IP: 272)
AX: 0000000010000000 = 128
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 0    ZF: 0    SF: 1

Update #10 (IP: 274)
AX: 0000000000000000 = 0
BX: 0000000000000000 = 0
CX: 0000000000000000 = 0
DX: 0000000000000000 = 0
SI: 0000000000000000 = 0
DI: 0000000000000000 = 0
BP: 0000000000000000 = 0
SP: 1111111111111110 = 65534
CF: 1    ZF: 1    SF: 0
``` 
 
 #### Other Test Files:
 The following test files are included, though the emulator will work with any binary assembly file that uses the currenty supported instructions:
__ADD, SUB, ADC, SBB, AND, OR, XOR, NOT, NEG, CMP, INC, DEC, XCHG, MOV, NOP, LOOP, CLC, TEST__ (partial implementation)__, JB, JC, JNAE, JAE, JNB, JNC, JE, JZ, JNE, JE, JNZ, JBE, JNA, JA, JNBE, JS, JNS, JC, JCXZ__
Instructions can take any __r8, r16, imm8__, or __imm16__ as a parameter.
 
__MYTEST.COM__ goes through many of the standard instructions to show that they have been implemented properly.
These include MOV, SUB, INC, NOT, OR, XOR, XCHG, DEC, NEG, and INT.
```Assembly
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
```

__SAMPLEL.COM__ simply loops from 10 to 1
```Assembly
         sub ax,ax
         mov cx,10
   l:    add ax,cx
         loop l
         int 20h
```

__LOOPER.COM__ has two loops that store the sum of numbers from 1 to 10.
The first loop goes up from 1 to 10 and the second goes down from 10 to 1.
This was based on an in-class discussion in an assembly programming class and the program shows that looping down to zero is more efficient with less code.
The sums from the two loops are stored in AX and BX respectively, and their sums are, of course, the same at the end.
(Note: The standard implementation of a loop from 1 to 10 is to write:
```Assembly
CMP CX,10 
JLE l
```
However, as this emulator does not yet store overflow flag, I replaced those two lines with:
```Assembly
CMP CX,11
JNZ l
```
The full file:
```Assembly
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
```

Daniel Rosenthal
4/2/2020
