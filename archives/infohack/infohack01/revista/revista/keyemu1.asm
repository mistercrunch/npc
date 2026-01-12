;
;KEYEMU1.ASM - (c) 1998 - Maruja
;
;Programa residente de prueba que emula la llave de harware
;marca 'SOStuare Look' que protege a un programa berreta de facturacion
;
;Confunde al programa de proteccion pero no al programa protegido
;

.model tiny
.code
org 100h

inicio:		jmp  Arranque

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IN_BYTE		EQU	0ECh		;IN  AL, DX

IPanterior	dw	0		;CS:IP Instruccion anterior
CSanterior	dw	0

lpt11		dw	?		;Direccion base (1er registro) de LPT1
lpt13		dw	?		;Direccion 3er registro de LPT1

KEYBUF		EQU	444
keyindex	dw	0
keydata		db	000, 085, 170, 000, 204, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 000, 000, 204, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 134, 134, 150, 134, 134, 150
		db	134, 150, 134, 134, 134, 150, 150, 134, 134, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 134
		db	134, 134, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 134, 134, 134, 134, 134, 134
		db	134, 134, 150, 150, 150, 134, 150, 134, 134, 150
		db	150, 150, 150, 134, 134, 150, 134, 134, 150, 134
		db	150, 134, 134, 134, 150, 150, 133, 204, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	149, 204, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150, 150, 150, 150, 150, 150, 150
		db	150, 150, 150, 150

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

		mov  cs:keyindex, 0	;Resetear posicion en buffer de llave

		pop  bp
		pop  ax
		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Trap:
	SP_INICIAL	EQU	$

		push cx			;Salvar registros utilizados
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

		cmp  cl, IN_BYTE	;El opcode es IN_BYTE ?
		jne  Trap_salir		;No: salir

		push cs
		pop  ds
		cmp  dx, lpt11		;Acceso a alguno de los puertos LPT1 ?
		jb   Trap_salir
		cmp  dx, lpt13
		ja   Trap_salir		;No: salir

		call EmularKey		;Si: emular llave

Trap_salir:	mov  bp, sp		;Guardar CS:IP de proxima instruccion
		mov  si, [bp + (SP_FINAL-SP_INICIAL)*2 + 2 ]
		mov  cs:CSanterior, si
		mov  si, [bp + (SP_FINAL-SP_INICIAL)*2 ]
		mov  cs:IPanterior, si

		pop  bp			;Recuperar registros y salir
		pop  ds
		pop  si
		pop  cx
		iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EmularKey:	;En AL pone el mismo dato que pondria la llave

		lea  si, keydata	;Posicionarse en el buffer
		add  si, keyindex
		mov  al, byte ptr [si]	;Obtener dato de la llave ;)

		inc  keyindex
		cmp  keyindex, KEYBUF	;Ya llego al ultimo dato ?
		jne  EK_salir

		mov  keyindex, 0	;Si: resetear posicion en el buffer

EK_salir:	ret

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
