;
;LPT1CAP.ASM - (c) 1998 - Maruja
;
;Programa residente que captura todos los accesos al puerto LPT1
;Se activa con 'INT 90h' y se desactiva con 'INT 91h'
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

IPanterior	dw	0		;CS:IP Instruccion anterior
CSanterior	dw	0

lpt11		dw	?		;Direccion base (1er registro) de LPT1
lpt13		dw	?		;Direccion 3er registro de LPT1

TAMBUF		EQU	4096*8		;Longitud buffer (grande)
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

		push dx
		push ds
		push cs
		pop  ds
		mov  ax, 2501h		;Setear nuestro vector de int TRAP
		lea  dx, Trap
		int  21h
		pop  ds
		pop  dx

		pop  bp
		pop  ax


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; INSTRUCCION ORIGINAL EN EL ARCHIVO DE PROTECCION
	; QUE FUE CAMBIADA POR EL 'INT 90h' (CD 90)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		ADD  DI, AX
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
UnTrap:		push ax
		push bp

		mov  bp, sp
		mov  ax, [bp+8]		;Obtener flags de la pila
		and  ah, 0FEh		;Desactivar bit T
		mov  [bp+8], ax		;Colocar nuevos flags en la pila

		cmp  cs:index, 0	;El buffer est  vac¡o ?
		jz   UnTrap_salir	;Si: salir

		push bx
		push cx
		push dx
		push ds
		push cs
		pop  ds
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
		mov  si, cs:IPanterior	;DS:SI = CS:IP instrucci¢n anterior
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
GrabarBuffer:	mov  ax, 3D02h		;Abrir archivo para L/E
		lea  dx, filename
		int  21h
		jnc  GB_append
		mov  ah, 3Ch		;Si no existe crearlo
		xor  cx, cx
		int  21h
		jc   GB_salir		;Si hubo error salir

GB_append:	mov  bx, ax		;Poner archivo en modo append
		mov  ax, 4202h
		xor  dx, dx
		xor  cx, cx
		int  21h

		mov  ah, 40h		;Grabar buffer
		mov  cx, index
		lea  dx, buffer
		int  21h

		mov  ah, 3Eh		;Cerrar archivo
		int  21h
		mov  index, 0		;Resetear indice

GB_salir:	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FINRESID	EQU	$		;Aca termina el residente

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SetearVectores:	mov  ax, 2590h		;Hacer que una INT 90h ejecute el
		lea  dx, SetTrap	;codigo 'SetTrap'
		int  21h

		mov  ax, 2591h		;Hacer que una INT 91h ejecute el
		lea  dx, UnTrap		;codigo 'UnTrap'
		int  21h

		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GetLPT1:	push es				;Obtener registros de LPT1
		mov  di, 40h
		mov  es, di
		mov  di, 8			;ES:DI = 0040:0008h
		mov  ax, word ptr es:[di]
		mov  lpt11, ax			;lpt11 = Registro 1 LPT1
		add  ax, 2
		mov  lpt13, ax			;lpt13 = Registro 3 LPT1
		pop  es
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
mok		db	13, 10, 'OK', 13, 10, '$'
myaestoy	db	13, 10, 'Ya estaba residente', 13, 10, '$'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Arranque:	push ds				;El offset del vector de
		xor  si, si			;interrupcion 90h es igual
		mov  ds, si			;al offset de la SetTrap ?
		mov  si, 240h
		mov  ax, word ptr [si]
		pop  ds
		cmp  ax, offset SetTrap
		jne  A_instalar			;No: instalar residente

		lea  dx, myaestoy		;Como el residente ya estaba
		mov  ah, 9			;instalado mostrar mensaje
		int  21h			;y salir
		mov  ax, 4C00h
		int  21h

A_instalar:	call GetLPT1			;Obtener direcciones de LPT1
		call SetearVectores		;Setear nuevos vectores de ints
		lea  dx, mok			;Mostrar mensaje 'OK'
		mov  ah, 9
		int  21h
		mov  ax, 3100h			;Terminar y quedar residente
		lea  dx, FINRESID
		shr  dx, 4
		inc  dx
		int  21h

end inicio
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
