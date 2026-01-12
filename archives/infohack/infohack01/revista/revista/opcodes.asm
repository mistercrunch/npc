;
;OPCODES.ASM - (c) 1998 - Maruja
;
;Programa para conocer los codigos de operacion de las
;instrucciones que permiten el acceso a los puertos de E/S
;

.model tiny
.code
org 100h

inicio:		in   al, dx
		in   ax, dx
		out  dx, al
		out  dx, ax

		nop
		nop
		nop
		nop
		nop
		nop
					
		mov  ax, 4C00h
		int  21h

end inicio
