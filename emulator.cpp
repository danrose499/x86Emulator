#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include<cstdio>
#include<cstdlib>
#include<bitset>
#include<iomanip>
#define AX regs[0]
#define CX regs[1]
#define DX regs[2]
#define BX regs[3]
#define SP regs[4]
#define BP regs[5]
#define SI regs[6]
#define DI regs[7]
using namespace std;
void updateFlags(unsigned short r16, unsigned short *flags);
void updateFlags(unsigned char r8, unsigned short *flags);
void updateCarry(unsigned short r16, unsigned short imm16, unsigned short *FLAGS, bool plus);
void updateCarry(unsigned char r8, unsigned char imm8, unsigned short *FLAGS, bool plus);
int main() {
	int start = 0x100;
	unsigned char mb[1000000];
	unsigned short regs[8] = {0,0,0,0,0xFFFE,0,0,0}; // [AX,CX,DX,BX,SP,BP,SI,DI];
	unsigned short segRegs[4] = { 0x100,0x100,0x100,0x100 }; //[CS,DS,SS,ES];
	unsigned short FLAGS, IP = 0x100;
	FILE* f = fopen("LOOPER.COM", "rb");
	if (!f) {
		cout << "Error! No file found!\n";
		return 0;
	}
	fseek(f, 0, SEEK_END);
	long l = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread((mb + start), 1, l, f); 
	fclose(f);
	cout << "\nDisplaying state of registers after each instruction:\n\n";
	int instruction = 1;
	while (1) {
		unsigned char curbyte = mb[IP];
		cout << "Update #"<<instruction<<" (IP: "<<IP<<")"<<endl;
		cout << "AX: " << bitset<16>(AX) << " = " << AX << "\nBX: " << bitset<16>(BX) << " = " << BX << "\nCX: " << bitset<16>(CX) << " = " << CX << "\nDX: " << bitset<16>(DX) << " = " << DX <<
		"\nSI: " << bitset<16>(SI) << " = " << SI << "\nDI: " << bitset<16>(DI) << " = " << DI << "\nBP: " << bitset<16>(BP) << " = " << BP << "\nSP: " << bitset<16>(SP) << " = " << SP << endl; //Outputs Registers at each iteration of the while loop
		cout << "CF: " << bitset<1>(FLAGS & 1) << "    ZF: ";
		if((bitset<7>(FLAGS & 64))==64){cout << "1";}
		else{cout << "0";}
		cout << "    SF: ";
		if((bitset<8>(FLAGS & 128))==128){cout << "1\n\n";}
		else{cout << "0\n\n";}
		instruction++;
		switch (curbyte) {
			case 0x02: { // add r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short adder = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2, sum;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8 & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((adder & 0xFF00)>>8);
					}
					else { //H,L
						term2=(adder & 0xFF);
					}
					sum=term1+term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(sum<<8));
				}
				else { // L,..
					term1=(r8 & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((adder & 0xFF00)>>8);
					}
					else { //L,L
						term2=(adder & 0xFF);
					}
					sum=term1+term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+sum);
				}
				updateCarry(term1, term2, &FLAGS, true);
				updateFlags(sum, &FLAGS);
				break;
			}
			case 0x03: { // add r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)];
				unsigned short term2 = regs[(mb[IP]&7)];
				unsigned short sum = term1 + term2;
				regs[((mb[IP]&0x38)>>3)]=sum;
				updateCarry(term1, term2, &FLAGS, true);
				updateFlags(sum, &FLAGS);
				break;
			}
			case 0x04: { // add AL, imm8
				unsigned short AL = (AX & 0xFF), imm8 = mb[++IP];
				updateCarry(AL, imm8, &FLAGS, true);
				AL += imm8;
				AL = (AL & 0xFF); 
				AX = ((AX & 0xFF00)+AL);
				updateFlags(AL, &FLAGS);
				break;
			}
			case 0x05: { // add AX, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));
				updateCarry(AX, imm16, &FLAGS, true);
				AX += imm16;
				updateFlags(AX, &FLAGS);
				break;
			}
			case 0x0A: { // or r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short ander = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8 & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //H,L
						term2=(ander & 0xFF);
					}
					term1=(term1|term2);
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(term1<<8));
				}
				else { // L,..
					term1=(r8 & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //L,L
						term2=(ander & 0xFF);
					}
					term1=(term1|term2);
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+term1);
				}
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(term1, &FLAGS);
				break;
			}
			case 0x0B: { // or r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)];
				unsigned short term2 = regs[(mb[IP]&7)];
				term1 = (term1 | term2);
				regs[((mb[IP]&0x38)>>3)]=term1;
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(term1, &FLAGS);
				break;
			}
			case 0x0C: { // or al, imm8
				unsigned char AL = (AX&0xFF);
				AL=(AL|(mb[++IP]));
				AX=(AX&0xFF00)+AL;
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(AL, &FLAGS);
				break;
			}
			case 0x0D: { // or ax, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));
				AX=(AX|imm16);
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(AX, &FLAGS);
				break;				
			}
			case 0x12: { // adc r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short adder = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2, sum;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=(((r8 & 0xFF00)>>8)+(FLAGS&1));
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((adder & 0xFF00)>>8);
					}
					else { //H,L
						term2=(adder & 0xFF);
					}
					sum=term1+term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(sum<<8));
				}
				else { // L,..
					term1=((r8 & 0xFF)+(FLAGS&1));
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((adder & 0xFF00)>>8);
					}
					else { //L,L
						term2=(adder & 0xFF);
					}
					sum=term1+term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+sum);
				}
				updateCarry(term1, term2, &FLAGS, true);
				updateFlags(sum, &FLAGS);
				break;
			}
			case 0x13: { // adc r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)];
				unsigned short term2 = (regs[(mb[IP]&7)]+(FLAGS&1));
				unsigned short sum = term1 + term2;
				regs[((mb[IP]&0x38)>>3)]=sum;
				updateCarry(term1, term2, &FLAGS, true);
				updateFlags(sum, &FLAGS);
				break;
			}
			case 0x14: { // adc al, imm8
				unsigned char al = (AX&0xFF), imm8=(mb[++IP])+(FLAGS&1);
				AX=((AX&0xFF00)+(al+imm8));
				updateCarry(al, imm8, &FLAGS, true);
				updateFlags(AX,&FLAGS);
				break;
			}
			case 0x15: { // adc ax, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));				
				updateCarry(AX, imm16, &FLAGS, true);
				AX+=(imm16+(FLAGS&1));
				updateFlags(AX, &FLAGS);
				break;
			}
			case 0x1A: { // sbb r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short adder = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2, diff;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8 & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=(((adder & 0xFF00)>>8)+(FLAGS&1));
					}
					else { //H,L
						term2=((adder & 0xFF)+(FLAGS&1));
					}
					diff=term1-term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(diff<<8));
				}
				else { // L,..
					term1=(r8 & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=(((adder & 0xFF00)>>8)+(FLAGS&1));
					}
					else { //L,L
						term2=((adder & 0xFF)+(FLAGS&1));
					}
					diff=term1-term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+diff);
				}
				updateCarry(term1, term2, &FLAGS, false);
				updateFlags(diff, &FLAGS);
				break;			
			}
			case 0x1B: { // sbb r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)];
				unsigned short term2 = (regs[(mb[IP]&7)]+(FLAGS&1));
				unsigned short diff = term1 - term2;
				regs[((mb[IP]&0x38)>>3)]=diff;
				updateCarry(term1, term2, &FLAGS, false);
				updateFlags(diff, &FLAGS);
				break;			
			}
			case 0x1C: { // sbb al, imm8
				unsigned char al = (AX&0xFF), imm8=(mb[++IP])+(FLAGS&1);
				AX=((AX&0xFF00)+(al-imm8));
				updateCarry(al, imm8, &FLAGS, false);
				updateFlags(AX,&FLAGS);
				break;
			}
			case 0x1D: { // sbb ax, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));				
				updateCarry(AX, imm16, &FLAGS, false);
				AX-=(imm16+(FLAGS&1));
				updateFlags(AX, &FLAGS);
				break;			
			}
			case 0x22: { // and r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short ander = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8 & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //H,L
						term2=(ander & 0xFF);
					}
					term1=(term1&term2);
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(term1<<8));
				}
				else { // L,..
					term1=(r8 & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //L,L
						term2=(ander & 0xFF);
					}
					term1=(term1&term2);
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+term1);
				}
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(term1, &FLAGS);
				break;
			}
			case 0x23: { // and r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)];
				unsigned short term2 = regs[(mb[IP]&7)];
				term1 = (term1 & term2);
				regs[((mb[IP]&0x38)>>3)]=term1;
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(term1, &FLAGS);
				break;
			}
			case 0x24: { // and al, imm8
				unsigned char AL = (AX&0xFF);
				AL=(AL&(mb[++IP]));
				AX=(AX&0xFF00)+AL;
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(AL, &FLAGS);
				break;
			}
			case 0x25: { // and ax, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));
				AX=(AX&imm16);
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(AX, &FLAGS);
				break;				
			}
			case 0x2A: { // sub r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short adder = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2, diff;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8 & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((adder & 0xFF00)>>8);
					}
					else { //H,L
						term2=(adder & 0xFF);
					}
					diff=term1-term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(diff<<8));
				}
				else { // L,..
					term1=(r8 & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((adder & 0xFF00)>>8);
					}
					else { //L,L
						term2=(adder & 0xFF);
					}
					diff=term1-term2;
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+diff);
				}
				updateCarry(term1, term2, &FLAGS, false);
				updateFlags(diff, &FLAGS);
				break;
			}
			case 0x2B: { // sub r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)];
				unsigned short term2 = regs[(mb[IP]&7)];
				unsigned short diff = term1 - term2;
				regs[((mb[IP]&0x38)>>3)]=diff;
				updateCarry(term1, term2, &FLAGS, false);
				updateFlags(diff, &FLAGS);
				break;
			}
			case 0x2C: { // sub AL, imm8
				unsigned short AL = (AX & 0xFF), imm8 = mb[++IP];
				updateCarry(AL, imm8, &FLAGS, false);
				AL -= imm8;
				AL = (AL & 0xFF); 
				AX = ((AX & 0xFF00)+AL);
				updateFlags(AL, &FLAGS);
				break;
			}
			case 0x2D: { // sub AX, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));
				updateCarry(AX, imm16, &FLAGS, false);
				AX -= imm16;
				updateFlags(AX, &FLAGS);
				break;
			}
			case 0x32: { // xor r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short ander = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8 & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //H,L
						term2=(ander & 0xFF);
					}
					term1=(term1^term2);
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(term1<<8));
				}
				else { // L,..
					term1=(r8 & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //L,L
						term2=(ander & 0xFF);
					}
					term1=(term1^term2);
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+term1);
				}
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(term1, &FLAGS);
				break;
			}
			case 0x33: { // xor r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)];
				unsigned short term2 = regs[(mb[IP]&7)];
				term1 = (term1 ^ term2);
				regs[((mb[IP]&0x38)>>3)]=term1;
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(term1, &FLAGS);
				break;
			}
			case 0x34: { // xor al, imm8
				unsigned char AL = (AX&0xFF);
				AL=(AL^(mb[++IP]));
				AX=(AX&0xFF00)+AL;
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(AL, &FLAGS);
				break;
			}
			case 0x35: { // xor ax, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));
				AX=(AX^imm16);
				FLAGS = (FLAGS & 0xFFFE);
				updateFlags(AX, &FLAGS);
				break;				
			}
			case 0x3A: { // cmp r8, r/m8
				IP++;
				unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short ander = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8 & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //H,L
						term2=(ander & 0xFF);
					}
				}
				else { // L,..
					term1=(r8 & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((ander & 0xFF00)>>8);
					}
					else { //L,L
						term2=(ander & 0xFF);
					}
				}
				if((term2&0x80)==0x80) {
					term2=((term2^0xFF)+1);
					updateCarry(term1, term2, &FLAGS, true);
					term1+=term2;
				}
				else{
					updateCarry(term1, term2, &FLAGS, false);
					term1-=term2;
				}
				updateFlags(term1, &FLAGS);
				break;			
			}
			case 0x3B: { // cmp r16, r/m16
				IP++;
				unsigned short term1 = regs[((mb[IP]&0x38)>>3)], term2 = regs[(mb[IP]&7)];
				if((term2&0x8000)==0x8000) {
					term2=((term2^0xFFFF)+1);
					updateCarry(term1, term2, &FLAGS, true);
					term1+=term2;
				}
				else{
					term1-=term2;
					updateCarry(term1, term2, &FLAGS, false);
				}
				updateFlags(term1, &FLAGS);
				break;
			}
			case 0x3C: { // cmp al, imm8
				unsigned char temp, al = (AX&0xFF), imm8 = mb[++IP];
				if((imm8&0x80)==0x80) {
					imm8=((imm8^0xFF)+1);
					temp=al+imm8;
					updateCarry(al, imm8, &FLAGS, true);
				}
				else{
					temp=al-imm8;
					updateCarry(al, imm8, &FLAGS, false);
				}
				updateFlags(temp, &FLAGS);
				break;
			}
			case 0x3D: { // cmp AX, imm16
				unsigned short imm16, A, B, temp;
				A = mb[IP+1];
				B = mb[IP+2];
				imm16 = ((B << 8) | (A));
				if((imm16&0x8000)==0x8000) {
					imm16=((imm16^0xFFFF)+1);
					temp=AX+imm16;
					updateCarry(AX, imm16, &FLAGS, true);
				}
				else{
					temp=AX-imm16;
					updateCarry(AX, imm16, &FLAGS, false);
				}
				updateFlags(temp, &FLAGS);
				break;
			}
			case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47: { // inc registers
				regs[curbyte & 7]++;
				updateFlags((regs[curbyte & 7]), &FLAGS);
				break;
			}
			case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F: { // dec registers
				regs[curbyte & 7]--;
				updateFlags((regs[curbyte & 7]), &FLAGS);
				break;
			}
			case 0x72: { // jb rel8, jc rel8, jnae rel8
				if((FLAGS&0x1)==1) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x73: { // jae rel8, jnb rel8, jnc rel8
				if((FLAGS&0x1)==0) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x74: { // je rel8, jz rel8
				if((FLAGS&0x40)==0x40) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x75: { //jne rel8, jnz rel8
				if((FLAGS&0x40)==0) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x76: { // jbe rel8, jna rel8
				if(((FLAGS&1)==1)||((FLAGS&0x40)==0x40)) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x77: { // ja rel8, jnbe rel8
				if((FLAGS&0x41)==0) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x78: { //js rel8
				if((FLAGS&0x80)==1) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x79: { // jns rel8
				if((FLAGS&0x80)==0) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0x80: { // (and/or/add/sub/adc/sbb/cmp) r/m8, imm8
				IP++;
				unsigned char r8;
				if((mb[IP]&0x38)==0x0) { // add r/m8, imm8
					unsigned char term1 = mb[IP+1], term2;
					if(((mb[IP])&0x04)==0x04) { //h
						term2=(((regs[((mb[IP])&3)])&0xFF00)>>8);
						r8=term1+term2;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF)+(r8<<8));
					}
					else { //l
						term2=(regs[((mb[IP])&3)]);
						r8=term1+term2;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF00)+r8);
					}
					updateCarry(term1, term2, &FLAGS, true);
					updateFlags(r8, &FLAGS);
				}
				else if((mb[IP]&0x38)==0x10) { // adc r/m8, imm8
					unsigned char term1 = (mb[IP+1]+(FLAGS&1)), term2;
					if(((mb[IP])&0x04)==0x04) { //h
						term2=(((regs[((mb[IP])&3)])&0xFF00)>>8);
						r8=term1+term2;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF)+(r8<<8));
					}
					else { //l
						term2=(regs[((mb[IP])&3)]);
						r8=term1+term2;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF00)+r8);
					}
					updateCarry(term1, term2, &FLAGS, true);
					updateFlags(r8, &FLAGS);
				}
				else if((mb[IP]&0x38)==0x28) { // sub r/m8, imm8
					unsigned char term1 = mb[IP+1], term2;
					if(((mb[IP])&0x04)==0x04) { //h
						term2=(((regs[((mb[IP])&3)])&0xFF00)>>8);
						r8=term2-term1;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF)+(r8<<8));
					}
					else { //l
						term2=(regs[((mb[IP])&3)]);
						r8=term2-term1;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF00)+r8);
					}
					updateCarry(term2, term1, &FLAGS, false);
					updateFlags(r8, &FLAGS);
				} 
				else if((mb[IP]&0x38)==0x18) { // sbb r/m8, imm8
					unsigned char term1 =(mb[IP+1]+(FLAGS&1)), term2;
					if(((mb[IP])&0x04)==0x04) { //h
						term2=(((regs[((mb[IP])&3)])&0xFF00)>>8);
						r8=term2-term1;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF)+(r8<<8));
					}
					else { //l
						term2=(regs[((mb[IP])&3)]);
						r8=term2-term1;
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF00)+r8);
					}
					updateCarry(term2, term1, &FLAGS, false);
					updateFlags(r8, &FLAGS);
				}
				else if((mb[IP]&0x38)==0x20) { // and r/m8, imm8
					if(((mb[IP])&0x04)==0x04) { //h
						r8=(((regs[((mb[IP])&3)])&0xFF00)>>8);
						r8=(r8&(mb[IP+1]));
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF)+(r8<<8));
					}
					else { //l
						r8=(regs[((mb[IP])&3)]);
						r8=(r8&(mb[IP+1]));
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF00)+r8);
					}
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags(r8, &FLAGS);
				}
				else if((mb[IP]&0x38)==0x8) { // or r/m8, imm8
					if(((mb[IP])&0x04)==0x04) { //h
						r8=((((regs[(mb[IP])&3]))&0xFF00)>>8);
						r8=(r8|(mb[IP+1]));
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF)+(r8<<8));
					}
					else { //l
						r8=((regs[(mb[IP])&3]));
						r8=(r8|(mb[IP+1]));
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF00)+r8);
					}
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags(r8, &FLAGS);
				}
				else if((mb[IP]&0x38)==0x30) { // xor r/m8, imm8
					if(((mb[IP])&0x04)==0x04) { //h
						r8=((((regs[(mb[IP])&3]))&0xFF00)>>8);
						r8=(r8^(mb[IP+1]));
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF)+(r8<<8));
					}
					else { //l
						r8=((regs[(mb[IP])]&3));
						r8=(r8^(mb[IP+1]));
						(regs[((mb[IP])&3)])=(((regs[(mb[IP])&3])&0xFF00)+r8);
					}
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags(r8, &FLAGS);
				}
				else if((mb[IP]&0x38)==0x38) { // cmp r/m8, imm8
					unsigned char imm8 = mb[IP+1], temp;
					if(((mb[IP])&0x04)==0x04) { //h
						r8=((((regs[(mb[IP])&3]))&0xFF00)>>8);
						if((imm8&0x80)==0x80) {
							imm8=((imm8^0xFF)+1);
							updateCarry(r8, imm8, &FLAGS, true);
							temp=r8+imm8;
						}
						else{
							updateCarry(r8, imm8, &FLAGS, false);
							temp=r8-imm8;
						}
					}
					else { //l
						r8=((regs[(mb[IP])]&3));
						if((imm8&0x80)==0x80) {
							imm8=((imm8^0xFF)+1);
							updateCarry(r8, imm8, &FLAGS, true);
							temp=r8+imm8;
						}
						else{
							updateCarry(r8, imm8, &FLAGS, false);
							temp=r8-imm8;
						}
					}
					updateFlags(temp, &FLAGS);
				}
				else {cout<<"\nError! Unknown form of 0x80!\n";}
				IP++;
				break;
			}
			case 0x81: { // (and/or/add/sub/adc/sbb/cmp r/m16), imm16
				unsigned short imm16, A, B;
				A = mb[IP+1];
				B = mb[IP+2];
				imm16 = ((B << 8) | (A));
				if((mb[IP]&0x38)==0x0) { // add r/m16, imm16
					unsigned short term = regs[(mb[IP])&7];
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]+(imm16));
					updateCarry(term, imm16, &FLAGS, true);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x10) { // adc r/m16, imm16
					imm16+=(FLAGS&1);
					unsigned short term = regs[(mb[IP])&7];
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]+(imm16));
					updateCarry(term, imm16, &FLAGS, true);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x0) { // sub r/m16, imm16
					unsigned short term = regs[(mb[IP])&7];
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]-(imm16));
					updateCarry(term, imm16, &FLAGS, false);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x0) { // sbb r/m16, imm16
					imm16+=(FLAGS&1);
					unsigned short term = regs[(mb[IP])&7];
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]-(imm16));
					updateCarry(term, imm16, &FLAGS, false);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x20) { // and r/m16, imm16
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]&(imm16));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x8) { // or r/m16, imm16
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]|(imm16));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x30) { // xor r/m16, imm16
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]^(imm16));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x38) { // cmp r/m16, imm16
					unsigned short temp;
					if((imm16&0x8000)==0x8000) {
						imm16=((imm16^0xFFFF)+1);
						updateCarry(temp, imm16, &FLAGS, true);
						temp=regs[(mb[IP])&7]+imm16;
					}
					else{
						updateCarry(temp, imm16, &FLAGS, false);
						temp=regs[(mb[IP])&7]-imm16;
					}
					updateFlags(temp, &FLAGS);
				}
				else {cout<<"\nError! Unknown form of 0x81!\n";}
				IP+=2;
				break;
			}
			case 0x83: { // (and/or/add/sub/adc/sbb/cmp) r/m16, imm8
				IP++;
				if((mb[IP]&0x38)==0x0) { // add r/m16, imm8
					unsigned short imm8 = mb[IP+1];
					//updateCarry((regs[(mb[IP])&7]), ((mb[IP+1])>>8), &FLAGS, true);
					updateCarry((regs[(mb[IP])&7]), imm8, &FLAGS, true);
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]+(mb[IP+1]));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x0) { // adc r/m16, imm8
					//updateCarry((regs[(mb[IP])&7]), ((mb[IP+1]+(FLAGS&1))>>8), &FLAGS, true);
					unsigned short imm8 = (mb[IP+1]+(FLAGS&1));
					updateCarry((regs[(mb[IP])&7]), imm8, &FLAGS, true);
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]+(mb[IP+1]+(FLAGS&1)));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x28) { // sub r/m16, imm8
					//updateCarry((regs[(mb[IP])&7]), ((mb[IP+1])>>8), &FLAGS, false);
					unsigned short imm8 = mb[IP+1];
					updateCarry((regs[(mb[IP])&7]), imm8, &FLAGS, false);
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]-(mb[IP+1]));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x18) { // sbb r/m16, imm8
					//updateCarry((regs[(mb[IP])&7]), (((mb[IP+1])>>8)+(FLAGS&1)), &FLAGS, false);
					unsigned short imm8 = (mb[IP+1]+(FLAGS&1));
					updateCarry((regs[(mb[IP])&7]), imm8, &FLAGS, false);
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]-((mb[IP+1])+(FLAGS&1)));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x20) { // and r/m16, imm8
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]&(mb[IP+1]));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x8) { // or r/m16, imm8
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]|(mb[IP+1]));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x30) { // xor r/m16, imm8
					regs[(mb[IP])&7]=(regs[(mb[IP])&7]^(mb[IP+1]));
					FLAGS = (FLAGS & 0xFFFE);
					updateFlags((regs[(mb[IP])&7]), &FLAGS);
				}
				else if((mb[IP]&0x38)==0x38) { // cmp r/m16, imm8
					unsigned short temp, imm8 = mb[IP+1];
					if((imm8&0x80)==0x80) {
						imm8=((imm8^0xFF)+1);
						updateCarry(temp, imm8, &FLAGS, true);
						temp=regs[(mb[IP])&7]+imm8;
					}
					else{
						updateCarry(temp, imm8, &FLAGS, false);
						temp=regs[(mb[IP])&7]-imm8;
					}
					updateFlags(temp, &FLAGS);					
				}
				else {cout<<"\nError! Unknown form of 0x83!\n";}
				IP++;
				break;
			}
			case 0x86: { // xchg r/m8, r8
				IP++;
				unsigned short r8a = regs[(mb[IP] & 0x18)>>3];
				unsigned short r8b = regs[(mb[IP] & 0x3)];
				unsigned char term1, term2;
				if((mb[IP] & 0x20)==0x20) { // H,..
					term1=((r8a & 0xFF00)>>8);
					if((mb[IP] & 0x4)==0x4) { // H,H
						term2=((r8b & 0xFF00)>>8);
						regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3]&0xFF)+(term2<<8));
						regs[(mb[IP] & 0x3)]=((regs[(mb[IP] & 0x3)]&0xFF)+(term1<<8));
					}
					else { //H,L
						term2=(r8b & 0xFF);
						regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3]&0xFF)+(term2<<8));
						regs[(mb[IP] & 0x3)]=((regs[(mb[IP] & 0x3)]&0xFF00)+term1);
					}
				}
				else { // L,..
					term1=(r8a & 0xFF);
					if((mb[IP] & 0x4)==0x4) { // L,H
						term2=((r8b & 0xFF00)>>8);
					}
					else { //L,L
						term2=(r8b & 0xFF);
						regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3]&0xFF00)+term2);
						regs[(mb[IP] & 0x3)]=((regs[(mb[IP] & 0x3)]&0xFF00)+term1);
					}
				}
				break;
			}
			case 0x87: { // xchg r/m16, r16
				unsigned short temp = regs[(mb[++IP])&7];
				regs[(mb[IP])&7] = regs[((mb[IP]&0x38)>>3)];
				regs[((mb[IP]&0x38)>>3)] = temp;
				break;
			}
			case 0x8A: { // mov r8, r/m8
				IP++;
				//unsigned short r8 = regs[(mb[IP] & 0x18)>>3];
				unsigned short adder = regs[(mb[IP] & 0x3)];
				unsigned char source;
				if((mb[IP] & 0x38)==0x20) { // H,..
					if((mb[IP] & 0x4)==0x4) { // H,H
						source=((adder & 0xFF00)>>8);
					}
					else { //H,L
						source=(adder & 0xFF);
					}
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF)+(source<<8));
				}
				else { // L,..
					if((mb[IP] & 0x4)==0x4) { // L,H
						source=((adder & 0xFF00)>>8);
					}
					else { //L,L
						source=(adder & 0xFF);
					}
					regs[(mb[IP] & 0x18)>>3]=((regs[(mb[IP] & 0x18)>>3] & 0xFF00)+source);
				}
				break;
			}
			case 0x8B: { // mov r16, r/m16
				IP++;
				unsigned short source = regs[(mb[IP]&7)];
				regs[((mb[IP]&0x38)>>3)]=source;
				break;
			}
			case 0x90: { //nop
				break;
			}
			case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97: { //xchg ax, r16
				unsigned short temp = AX;
				AX = regs[(mb[IP]&7)];
				regs[(mb[IP]&7)] = temp;
				break;
			}
			case 0xA8: { // test al, imm8
				unsigned char al = (AX&0xFF), imm8 = mb[++IP], temp;
				temp = al&imm8;
				updateFlags(temp, &FLAGS);
				AX=((AX&0xFF00)+temp);
				break;
			}
			case 0xA9: { // test ax, imm16
				unsigned short temp = (mb[++IP]&AX);
				updateFlags(temp, &FLAGS);
				break;
			}
			case 0xB0: case 0xB1: case 0xB2: case 0xB3: { //mov r8, imm8 (L)
				unsigned char imm8 = (mb[++IP]);
				regs[curbyte & 3] = (((regs[curbyte & 3]) & 0xFF00) + imm8);
				break;
			}
			case 0xB4: case 0xB5: case 0xB6: case 0xB7: { //mov r8, imm8 (H)
				unsigned char imm8 = (mb[++IP]);
				regs[curbyte & 3] = (((regs[curbyte & 3]) & 0xFF) + (imm8<<8));
				break;
			}
			case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF: { //mov r16, imm16
				unsigned short imm16, A, B;
				A = mb[++IP];
				B = mb[++IP];
				imm16 = ((B << 8) | (A));
				regs[curbyte & 7]= imm16;
				break;
			}
			case 0xCD: { //int
				if (mb[IP + 1] != 0x20) { cout << "\nError! Unimplemented form of 0xCD!\n"; }
				else exit(0);
				break;
			}
			case 0xE2: { //loop
				IP++;
				if(CX!=0) {
					unsigned short delta = mb[IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
					CX--;
				} 
				break;
			}
			case 0xE3: { //jcxz rel8
				if(CX==0) {
					unsigned short delta = mb[++IP];
					if((delta&0x80)==0x80) {IP-=((delta^0xFF)+1);} // jump backwards (negative)
					else {IP+=delta;} //jump forward (positive)
				} 
				break;
			}
			case 0xF6: { //(neg r/m8),(not r/,8)
				IP++;
				unsigned char r8;
				if((mb[IP]&0xD0)==0xD0){ // neg r/m8
					if((mb[IP]&4)==4) { // h
						r8=((regs[(mb[IP])&3]&0xFF00)>>8);
						r8=0-r8;
						unsigned short r16=(r8<<8);
						regs[(mb[IP])&3]=((regs[(mb[IP])&3]&0xFF)+r16);
					}
					else { // l
						r8=(regs[(mb[IP])&3]&0xFF);
						r8=0-r8;
						regs[(mb[IP])&3]=((regs[(mb[IP])&3]&0xFF00)+r8);
					}
					if(r8==0) {FLAGS = (FLAGS & 0xFFFE);}
					else {FLAGS=(FLAGS | 1);}
					updateFlags(r8, &FLAGS);
				}
				else if ((mb[IP]&0xD8)==0xD8) { // not r/,8
					if((mb[IP]&4)==4) { // h
						r8=((regs[(mb[IP])&3]&0xFF00)>>8);
						r8=~r8;
						unsigned short r16=(r8<<8);
						regs[(mb[IP])&3]=((regs[(mb[IP])&3]&0xFF)+r16);
					}
					else { // l
						r8=(regs[(mb[IP])&3]&0xFF);
						r8=~r8;
						regs[(mb[IP])&3]=((regs[(mb[IP])&3]&0xFF00)+r8);
					}
				}
				else {cout << "\nError! Unimplemented form of 0xF6!\n";}
				break;
			}
			case 0xF7: { //(neg r/m16),(not r/m16)
				IP++;
				if((mb[IP]&0xF8)==0xD8){ // neg r/m16
					if(regs[(mb[IP]&7)]==0) {FLAGS = (FLAGS & 0xFFFE);}
					else {FLAGS=(FLAGS | 1);}
					regs[(mb[IP]&7)]=(0-(regs[(mb[IP])&7]));
					updateFlags(regs[(mb[IP]&7)], &FLAGS);
				}
				else if((mb[IP]&0xF8)==0xD0){ // not r/m16
					regs[(mb[IP]&7)]=(~(regs[(mb[IP])&7]));
				}
				else{cout << "Unknown form of 0xF7!\n";}
				break;
			}
			case 0xF8: { //clc
				FLAGS = (FLAGS & (~1)); //check logic
				break;
			}
			case 0xFE: { // inc/dec r8
				IP++;
				unsigned char r8;
				if((mb[IP]&0xF8)==0xC0) { // inc r8
					if((mb[IP]&0x4)==0x4){ //h
						r8=(((regs[(mb[IP]&3)]&0xFF00)>>8)+1);
						regs[(mb[IP]&3)]=(((regs[(mb[IP]&3)])&0xFF)+(r8<<8));
					}
					else { //l
						r8=((regs[(mb[IP]&3)]&0xFF)+1);
						regs[(mb[IP]&3)]=((regs[(mb[IP]&3)]&0xFF00)+r8);
					}
				}
				else if((mb[IP]&0xF8)==0xC8) { // dec r8
					if((mb[IP]&0x4)==0x4){ //h
						r8=(((regs[(mb[IP]&3)]&0xFF00)>>8)-1);
						regs[(mb[IP]&3)]=(((regs[(mb[IP]&3)])&0xFF)+(r8<<8));
					}
					else { //l
						r8=((regs[(mb[IP]&3)]&0xFF)-1);
						regs[(mb[IP]&3)]=((regs[(mb[IP]&3)]&0xFF00)+r8);
					}
				}
				else {cout << "Error! Unkown form of 0xFE!\n";}
				updateFlags(r8, &FLAGS);
				break;
			}
		}
		IP++;
	}
}

void updateFlags(unsigned short r16, unsigned short *flags) {
	if((r16 & 0x8000)==0x8000) {*flags = (*flags | 0x80);} //Sets sign flag
	else {*flags = (*flags & 0xFF7F);} //Clears sign flag
	if(r16==0) {*flags = (*flags | 0x40);} //Sets zero flag
	else {*flags = (*flags & 0xFFBF);} //Clears zero flag
}
void updateFlags(unsigned char r8, unsigned short *flags) {
	if((r8 & 0x80)==0x80) {*flags = (*flags | 0x80);} //Sets sign flag
	else {*flags = (*flags & 0xFF7F);} //Clears sign flag
	if(r8==0) {*flags = (*flags | 0x40);} //Sets zero flag
	else {*flags = (*flags & 0xFFBF);} //Clears zero flag
}
void updateCarry(unsigned short r16, unsigned short imm16, unsigned short *flags, bool plus) {
	if(plus==true){ //Addition
		if(0xFFFF > (r16+imm16)) {*flags=(*flags & 0xFFFE);} //Clears carry flag
		else {*flags = (*flags | 1);} //Sets carry flag
	}
	else { //Subtraction
		if(r16 >= imm16){*flags=(*flags & 0xFFFE);} //Clears carry flag
		else {*flags = (*flags | 1);} //Sets carry flag
	}
}
void updateCarry(unsigned char r8, unsigned char imm8, unsigned short *flags, bool plus) {
	if(plus==true){ //Addition
		if(0xFF > (r8+imm8)) {*flags=(*flags & 0xFFFE);} //Clears carry flag
		else {*flags = (*flags | 1);} //Sets carry flag
	}
	else { //Subtraction
		if(r8 >= imm8){*flags=(*flags & 0xFFFE);} //Clears carry flag
		else {*flags = (*flags | 1);} //Sets carry flag
	}
}