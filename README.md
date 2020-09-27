This is a simple Assembly emulator, intended as an educational tool to demonstrate how different x86 instructions affect the registers.
The program reads a binary assembly file line-by-line into a switch which supports many of the common used instructions.
Also included are several assembly test files, in text, binary, and lst form (for seeing how the different instructions relate to their respective opcdes).

In order to change the file you would like to emulate, edit the C++ and change line 28
such that the name of the file you would like to test is in the quotes.

Ex:
28:   FILE* f = fopen("SAMPLE.COM", "rb");


Before each itteration of the loop, the contents of the following register are displayed:
AX,BX,CX,DX,SP,DI,BP,SI, as well as the following bits of FLAGS:
ZF,SF,CF.

==========

The following test files are included, though the emulator will work with any binary assembly file that uses the currenty supported instructions (see below):
 
MYTEST.COM just goes through many of the standard instructions to show that they have been implemented properly.
These include MOV, SUB, INC, NOT, OR, XOR, XCHG, DEC, NEG, and INT.

LOOPER.COM has to loops that store the sum of numbers from 1 to 10.
The first loop goes up to 10 and the second goes down from 10.
This was based on an in-class discussion (in an assembly programming class), and the program shows that looping down to zero is more efficient with less code.
[Note: The standard implementation of a loop from 1 to 10 was to have CMP CX,10 followed by JLE l. As this emulator does not yet store overflow flag,
I replaced those two lines with CMP CX,11 followed by JNZ l]. The sums from the two loops are stored in AX and BX respectively,and their sums are, of
course, the same at the end.

Two more sample test files, SAMPLE.COM and SAMPLEL.COM are also included.

==========

Currrent Supported Assembly Instructions:
ADD,SUB,ADC,SBB,AND,OR,XOR,NOT,NEG,CMP,INC,DEC,XCHG,MOV,NOP,LOOP,CLC,TEST(partial implementation)Z
JB,JC,JNAE,JAE,JNB,JNC,JE,JZ,JNE,JE,JNZ,JBE,JNA,JA,JNBE,JS,JNS,JC,JCXZ
Instructions can take any r8, r16, imm8, or imm16 as a parameter.

==========

Daniel Rosenthal
4/2/2020
