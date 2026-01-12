;
;TRAP.ASM - (c) 1998 - Maruja
;
;Ejemplo de funcionamiento de la interrupcion TRAP
;

.model tiny
.code
org 100h

inicio:		jmp  Arranque

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
settrap_msg	db	'TRAP ON', 13, 10, '$'
untrap_msg	db	'TRAP OFF', 13, 10, '$'
trap_msg	db	'TRAP!', 13, 10, '$'

vieja90		dd	?
vieja91		dd	?
vieja01		dd	?

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SetTrap:	push ax
		push bp
		mov  bp, sp
		mov  ax, [bp+8]		;Obtener flags de la pila
		or   ah, 1		;Activar bit T
		mov  [bp+8], ax		;Colocar nuevos flags en la pila

		push dx			;Imprimir mensaje 'TRAP ON'
		push ds
		push cs
		pop  ds
		lea  dx, settrap_msg
		mov  ah, 9
		int  21h
		pop  ds
		pop  dx

		pop  bp
		pop  ax
		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
UnTrap:		push ax
		push bp
		mov  bp, sp
		mov  ax, [bp+8]		;Obtener flags de la pila
		and  ah, 0FEh		;Desactivar bit T
		mov  [bp+8], ax		;Colocar nuevos flags en la pila

		push dx			;Imprimir mensaje 'TRAP OFF'
		push ds
		push cs
		pop  ds
		lea  dx, untrap_msg
		mov  ah, 9
		int  21h
		pop  ds
		pop  dx

		pop  bp
		pop  ax
		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Trap:		push dx			;Imprimir mensaje 'TRAP!'
		push ds
		push ax
		push cs
		pop  ds
		lea  dx, trap_msg
		mov  ah, 9
		int  21h
		pop  ax
		pop  ds
		pop  dx
		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SetearVectores:	mov  ax, 3501h		;Obtener vector de interrupcion TRAP
		int  21h		;(tipo 1) y guardarlo
		mov  word ptr vieja01, bx
		mov  bx, es
		mov  word ptr vieja01+2, bx
		
		mov  ax, 3590h		;Obtener vector de interrupcion 90h
		int  21h		;y guardarlo
		mov  word ptr vieja90, bx
		mov  bx, es
		mov  word ptr vieja90+2, bx
		
		mov  ax, 3591h		;Obtener vector de interrupcion 91h
		int  21h		;y guardarlo
		mov  word ptr vieja91, bx
		mov  bx, es
		mov  word ptr vieja91+2, bx
		
		mov  ax, 2590h		;Hacer que una INT 90h ejecute el
		lea  dx, SetTrap	;codigo 'SetTrap'
		int  21h

		mov  ax, 2591h		;Hacer que una INT 91h ejecute el
		lea  dx, UnTrap		;codigo 'UnTrap'
		int  21h

		mov  ax, 2501h		;Hacer que la interrupcion TRAP
		lea  dx, Trap		;provoque la ejecucion del codigo
		int  21h		;'Trap'
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RestaurarVectores:
		mov  ax, 2501h			;Restaurar vector anterior
		lds  dx, dword ptr vieja01	;interrupcion TRAP
		int  21h

		mov  ax, 2590h			;Restaurar vector anterior
		lds  dx, dword ptr cs:vieja90	;interrupcion 90h
		int  21h

		mov  ax, 2591h			;Restaurar vector anterior
		lds  dx, dword ptr cs:vieja91	;interrupcion 91h
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Arranque:	call SetearVectores

		int  90h			;Activar Trap
		mov  ax, 1
		mov  dx, 2
		nop
		int  91h			;Desactivar Trap

		Call RestaurarVectores

		mov  ax, 4C00h			;Terminar programa
		int  21h

end inicio
