;
;FLUSH.ASM - (c) 1998 - Maruja
;
;Baja el contenido del buffer de la rutina TRAP residente a disco
;

.model tiny
.code

org 100h

inicio:	int 91h
	mov ax, 4C00h
	int 21h

end inicio
