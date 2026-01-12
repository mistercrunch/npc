;
;LPT1CAP1.ASM - (c) 1998 - Maruja
;
;Ejemplo de rastreo de operaciones en el puerto LPT1
;

.model tiny
.code
org 100h

inicio:		jmp  Arranque

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IN_BYTE		EQU	0ECh		;IN  AL, DX
IN_WORD		EQU	0EDh		;IN  AX, DX
OUT_BYTE	EQU	0EEh		;OUT DX, AL
OUT_WORD	EQU	0EFh		;OUT DX, AX

ES_OUT		EQU	32768		;Bit 15 flags
ES_WORD		EQU	16384		;Bit 14 flags

vieja90		dd	?		;Direccion original INT 90h
vieja91		dd	?		;Direccion original INT 91h
vieja01		dd	?		;Direccion original TRAP

IPanterior	dw	0		;CS:IP Instruccion anterior
CSanterior	dw	0

lpt11		dw	?		;Direccion base (1er registro) de LPT1
lpt13		dw	?		;Direccion 3er registro de LPT1

TAMBUF		EQU	256*8		;Longitud buffer
buffer		db	TAMBUF	dup (0)	;Buffer temporal para datos capturados
index		dw	0		;Posicion en buffer

filename	db	'C:\LPT.DAT', 0	;Archivo con datos capturados

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SetTrap:	push ax
		push bp

		mov  bp, sp
		mov  ax, [bp+8]		;Obtener flags de la pila
		or   ah, 1		;Activar bit T
		mov  [bp+8], ax		;Colocar nuevos flags en la pila

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

		cmp  cs:index, 0	;El buffer esta vacio ?
		jz   UnTrap_salir	;Si: salir

		push bx
		push cx
		push dx
		push ds
		call GrabarBuffer	;Forzar la grabacion del buffer
		pop  ds
		pop  dx
		pop  cx
		pop  bx

UnTrap_salir:	pop  bp
		pop  ax
		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Trap:
	SP_INICIAL	EQU	$

		push ax			;Salvar registros utilizados
		push bx
		push cx
		push dx
		push si
		push ds
		push bp

	SP_FINAL	EQU	$

		cmp  cs:CSanterior, 0	;CSanterior tiene un valor incorrecto ?
		jz   Trap_salir		;Si: salir
		
		mov  si, cs:CSanterior
		mov  ds, si
		mov  si, cs:IPanterior	;DS:SI = CS:IP instruccion anterior
		mov  cl, byte ptr [si]	;Obtener opcode 

		cmp  cl, IN_BYTE	;El opcode es alguno entre
		jb   Trap_salir		;IN_BYTE u OUT_WORD ?
		cmp  cl, OUT_WORD
		ja   Trap_salir		;No: salir

		push cs
		pop  ds
		cmp  dx, lpt11		;Acceso a alguno de los puertos LPT1 ?
		jb   Trap_salir
		cmp  dx, lpt13
		ja   Trap_salir		;No: salir

		call CapturarAcceso

Trap_salir:	mov  bp, sp		;Guardar CS:IP de proxima instruccion
		mov  si, [bp + (SP_FINAL-SP_INICIAL)*2 + 2 ]
		mov  cs:CSanterior, si
		mov  si, [bp + (SP_FINAL-SP_INICIAL)*2 ]
		mov  cs:IPanterior, si

		pop  bp			;Recuperar registros y salir
		pop  ds
		pop  si
		pop  dx
		pop  cx
		pop  bx
		pop  ax
		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CapturarAcceso:	;AX = Dato; DX = Puerto; CL = Opcode

		cmp  cl, OUT_WORD	;Es un OUT ?
		je   CA_setout
		cmp  cl, OUT_BYTE
		jne  CA_verdata
CA_setout:	or   dx, ES_OUT		;Si: setear bit 15

CA_verdata:	cmp  cl, IN_WORD	;El dato es word ?
		je   CA_setword
		cmp  cl, OUT_WORD
		jne  CA_push
CA_setword:	or   dx, ES_WORD	;Si: setear bit 14

CA_push:	lea  si, buffer		;Guardar estructura en buffer
		add  si, index
		mov  cx, CSanterior
		mov  [si], cx		;Guardar CS
		mov  cx, IPanterior
		mov  [si+2], cx		;Guardar IP
		mov  [si+4], dx		;Guardar Flags
		mov  [si+6], ax		;Guardar Dato
		add  index, 8		;Actualizar indice

		cmp  index, TAMBUF	;El buffer esta lleno ?
		je   GrabarBuffer	;Si: grabar buffer en disco

		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GrabarBuffer:	mov  ax, 3D02h			;Abrir archivo para L/E
		lea  dx, filename
		int  21h
		jnc  GB_append
		mov  ah, 3Ch			;Si no existe crearlo
		xor  cx, cx
		int  21h
		jc   GB_salir			;Si hubo error salir

GB_append:	mov  bx, ax			;Poner archivo en modo append
		mov  ax, 4202h
		xor  dx, dx
		xor  cx, cx
		int  21h

		mov  ah, 40h			;Grabar buffer
		mov  cx, index
		lea  dx, buffer
		int  21h

		mov  ah, 3Eh			;Cerrar archivo
		int  21h
		mov  index, 0			;Resetear indice

GB_salir:	ret

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
GetLPT1:	mov  di, 40h
		mov  es, di
		mov  di, 8			;ES:DI = 0040:0008h
		mov  ax, word ptr es:[di]
		mov  lpt11, ax
		add  ax, 2
		mov  lpt13, ax
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Arranque:	call GetLPT1			;Obtener direcciones de LPT1

		call SetearVectores

		int  90h			;Activar Trap

		xor  ax, ax
		mov  dx, lpt11			;DX = LPT1 registro 1
		out  dx, al
		inc  dx				;DX = LPT1 registro 2	
		in   ax, dx
		out  dx, ax
		inc  dx				;DX = LPT1 registro 3
		in   al, dx

		inc  dx				;DX = LPT1Base+4: NO es LPT1
		in   ax, dx

		int  91h			;Desactivar Trap

		Call RestaurarVectores

		mov  ax, 4C00h			;Terminar programa
		int  21h

end inicio
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
