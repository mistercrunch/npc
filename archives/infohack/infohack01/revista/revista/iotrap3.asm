;
;IOTRAP3.ASM - (c) 1998 - Maruja
;
;Ejemplo de rastreo de operaciones en puertos de E/S:
;Nueva implementacion sin bugs
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
IPanterior	dw	0
CSanterior	dw	0

Trap:
	SP_INICIAL	EQU	$

		push dx			;Salvar registros utilizados
		push ds
		push ax
		push si
		push bp

	SP_FINAL	EQU	$

		cmp  cs:CSanterior, 0	;CSanterior tiene un valor incorrecto ?
		jz   Trap_salir		;Si: salir
		
		mov  si, cs:CSanterior
		mov  ds, si
		mov  si, cs:IPanterior	;DS:SI = CS:IP instruccion anterior
		mov  al, byte ptr [si]	;Obtener opcode 

		cmp  al, 0ECh		;El opcode es 0ECh ? 
		jb   Trap_salir		;Si es menor salir
		cmp  al, 0EFh		;El opcode es 0EFh ?
		ja   Trap_salir		;Si es mayor salir

		push cs			;Como el opcode es uno entre
		pop  ds			;0ECh y 0EFh imprimir mensaje TRAP!
		lea  dx, trap_msg
		mov  ah, 9
		int  21h

Trap_salir:	mov  bp, sp		;Guardar CS:IP de proxima instruccion
		mov  si, [bp + (SP_FINAL-SP_INICIAL)*2 + 2 ]
		mov  cs:CSanterior, si
		mov  si, [bp + (SP_FINAL-SP_INICIAL)*2 ]
		mov  cs:IPanterior, si

		pop  bp			;Recuperar registros y salir
		pop  si
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
		mov  dx, 60h
		nop
		in   al, 60h
		in   al, dx
		in   ax, dx
		in   ax, 60h
		mov  al, 0EEh
		xor  dx, dx

		int  91h			;Desactivar Trap

		Call RestaurarVectores

		mov  ax, 4C00h			;Terminar programa
		int  21h

end inicio
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
