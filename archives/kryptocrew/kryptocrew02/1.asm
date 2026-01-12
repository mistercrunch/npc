; DOS - COM Crypter
; written by SnakeByte [Kefrens/KryptoCrew]
; ...very simple ;)
;coded for:  sentinel@therim.net


.286
code segment
assume cs:code,ds:code      ;Definiert die einzelnen Segmente
org 100h                    ;Wir bauen eine .COM Datei
jumps

Start:
  lea dx,blank              ;leere Zeile
  call writes  
 
  lea dx, cpyrght           ;Copyright ausgeben ;)
  call writes

;Datei suchen und öffnen

 lea dx,blank               ;leere Zeile
 call writes  
 
 lea dx,file
 call writes

 lea bx, filename           ;Dateiname einlesen
 mov dh, 50d                ;maximal 50 Zeichen ( für Pfadangaben )
 call reads           
  
 mov ah,4eh                 ;Datei suchen
 xor cx,cx 
 lea dx,filename
 int 21h
 
 lea dx,blank
 call writes  
 
 jnc open_it

 lea dx, not_found          ;nicht gefunden Nachricht ausgeben
 call writes 

 jmp QUIT                   ;und beenden

open_it:

 lea dx,blank
 call writes  
 
 lea dx,enterkey            ;Schlüssel einlesen
 call writes

 lea bx, keystring
 mov dh, 12d
 call reads           

 lea dx,blank
 call writes  

 lea si, keystring          ;verschiedene Werte berechnen mit denen wir
 mov cx, 12d                :das XOR durchführen
 mov bx, 'S'
 keycalc:
  lodsb
  xor bx, ax
 loop keycalc
 mov byte ptr cs:[key], bl

 lea si, keystring
 mov cx, 12d
 mov bx, 'S'
 keycalc2:
  lodsb
  add bx, ax
 loop keycalc2
 mov byte ptr cs:[key2], bl

 lea si, keystring
 mov cx, 12d
 mov bx, 'S'
 keycalc3:
  lodsb
  add bx, ax
  rol bx, cl
  xor bx, ax
 loop keycalc3
 mov byte ptr cs:[key3], bl


 mov ax,3d02h             ;Datei öffnen
 lea dx,filename 
 int 21h 

 jnc LoadFile

 lea dx, cantopen         ;Fehlermeldung
 call writes 

 jmp errorQUIT
 
LoadFile:
 xchg ax,bx

 mov ax,4202h             ;Dateilänge
 xor cx,cx                ;in DX:AX ermitteln
 xor dx,dx 
 int 21h

 mov word ptr cs:[lenghtlw], ax

 cmp word ptr cs:[offset lenghtlw], 63000d
 jae toobig

 cmp word ptr cs:[offset lenghtlw], 0
 je toosmall


 mov ax,4200h             ;Zeiger an den Start setzen
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah,3fh               ;Datei einlesen
 mov cx,lenghtlw
 lea dx,buffer
 int 21h
 
 mov si, [offset buffer]
 add si, lenghtlw
 sub si,2h

 cmp word ptr [si], 'CC' ;testen ob sie schon verschlüsselt ist
 je DECRYPT

ENCRYPT:

 mov si, [offset buffer]
 mov cx, lenghtlw

 COMCRYPTLOOP:           ;im Speicher verschlüsseln
   mov dh, key
   xor byte ptr cs:[si],dh
   mov dh, key2
   xor byte ptr cs:[si],dh
   mov dh, key3
   xor byte ptr cs:[si],dh
   inc si
 loop COMCRYPTLOOP  

 mov word ptr [si], 'CC' ; Marker setzen

 mov ax,4200h
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah, 40h             ;Auf die Platte schreiben
 mov cx, lenghtlw 
 add cx, 2h
 lea dx, buffer
 int 21h 

 lea dx, successe
 call writes 

 mov ah, 3eh              ;Datei schließen
 int 21h

 jmp QUIT



DECRYPT:                  ;entschlüsseln
 mov dh, KEY
 mov si, [offset buffer]
 mov cx, lenghtlw
 
 COMCRYPTLOOP2:
   mov dh, key3
   xor byte ptr cs:[si], dh
   mov dh, key2
   xor byte ptr cs:[si], dh
   mov dh, key
   xor byte ptr cs:[si], dh
   inc si
 loop COMCRYPTLOOP2

 mov ah, 3eh
 int 21h

 mov ah, 41h              ;Datei löschen, damit nichts von der alten über bleibt
 lea dx, filename
 int 21h  
 
 mov ah, 3Ch              ;und wieder erstellen & beschreiben
 lea dx, filename
 xor cx, cx
 int 21h
 
 xchg ax, bx

 mov ax,4200h
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah, 40h
 mov cx, lenghtlw
 sub cx, 2h
 lea dx, buffer
 int 21h 

 lea dx, successd
 call writes 

 mov ah, 3eh
 int 21h

jmp QUIT




toobig:                   ;Error Message Handling
 lea dx, big
 call writes
 
 mov ah, 3eh
 int 21h
 jmp errorQUIT

toosmall:
 lea dx, small
 call writes
 
 mov ah, 3eh              ;close file
 int 21h

 
errorQUIT: 
 lea dx, error
 call writes

QUIT:
 mov ah,4ch
 int 21h 


; ***************** Procedures... *******************************************>

writes:
 mov ah, 09h           ; print string at DS:DX
 int 21h
ret                    ; return to caller

reads:                 ;get Filename Procedure
 xor cx, cx            ; reset counter
 mov byte ptr cs:[bx], cl
readch:    
 call readc            ; read char without output
 cmp al, 0Dh           ; is return?
 jz readend            ; end-read
 cmp al, 08h           ; is backspace?
 jz readback           ; do backspace
 cmp cl, dh            ; max char?
 jz readch             ; read next
 test al, al           ; is function key?
 jz readfunc           ; jump to read function key
 mov dl, al            ; dl = char to write
 call writec           ; write char
 mov [bx], al          ; save char in buffer
 inc bx                ; buffer[pos]++
 inc cl                ; counter++
 jmp readch            ; read next

readend:
 mov byte ptr cs:[bx+1], 00h
                       ; set stringend
ret                    ; return to caller

readfunc:
 call readc            ; read char without output
 jmp readch            ; jump back

readback:
 test cl, cl           ; test, if counter = 0
 jz readch             ; -1 char ... cannot delete

 dec cl                ; counter--
 dec bx                ; bx--
 mov byte ptr cs:[bx], 00h 
                       ; remove last
 mov ch, dh            ; save dh
 lea dx, bcksp         ; set clear last char
 call writes           ; write string
 mov dh, ch            ; restore dh
 xor ch, ch            ; clear ch
 jmp readch            ; jump back

readc:
 mov ah, 07h           ; read char w/o output (al)
 int 21h
ret


writec:
 mov ah, 02h        ; write char (dl)
 int 21h
ret


;DATA
bcksp     db 8, 32, 8, '$'                                        ;Backspace
cpyrght   db 'Triple XOR Chiffre Crypter by SnakeByte',10d,13d
          db 'written for KC E-zine #2 [Kryptographie]',10d,13d
          db 'contact me at www.KryptoCrew.de',10d,13d,'$'        ;Copyright Text


file      db 'Please enter file : ','$'                           ;get Filename
filename  db 50d dup (?)
          db 0h                                                   ;store it here
not_found db 'The file does not exist...',10d,13d,'$'
cantopen  db 'Unable to open file, please check the attributes',10d,13d,'$'
blank     db ' ',10,13,'$'
successd  db 'Decryption finished !',10d,13d,'$'
successe  db 'Encryption finished !',10d,13d,'$'
big       db 'Filesize too huge ! 63kb Maximum Filesize',10d,13d,'$'
small     db 'File is only 0 bytes big.. ',10d,13d,'$'
error     db 'An Error occured ! File is not encrypted !',10d,13d,'$'
enterkey  db 'Please enter Password : ','$'
key       db 0h
key2      db 0h
key3      db 0h
keystring db 12d dup ('C')
lenghtlw  dw 0h
buffer:

ENDE:
code ends
end start