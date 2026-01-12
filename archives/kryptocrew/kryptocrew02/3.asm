; DOS - COM Crypter
; geschrieben von SnakeByte [SnakeByte@kryptocrew.de]

; Hier will ich einfach mal zeigen wie man COM Dateien verschlüsseln
; kann, so das sie ausführbar bleiben, aber nicht jedes Kind per Hexeditor
; einfach irgendwelche Strings ändern kann.

.286
code segment
assume cs:code,ds:code      ; Definiert die einzelnen Segmente
org 100h                    ; Wir bauen eine .COM Datei
jumps

Start:
  lea dx,blank              ; Leerzeile
  call writes  
 
  lea dx, cpyrght           ; Copyright eingeben
  call writes

                            ; Datei suchen und öffnen
 lea dx,blank               ; Leerzeile
 call writes  
 
 lea dx,file                ; Eingabeaufforderung
 call writes

 lea bx, filename           ; Name einlesen
 mov dh, 50d                ; Maximal 50 Zeichen lang, damit man auch Pfadangaben hat
 call reads           
  
 mov ah,4eh                 ; Überprüfen ob Datei existiert
 xor cx,cx 
 lea dx,filename
 int 21h
 
 lea dx,blank
 call writes  
 
 jnc open_it                ; und öffnen

 lea dx, not_found
 call writes 

 jmp QUIT                   ; aufhören wenn die Datei nicht existiert
 
open_it:

 mov ax,3d02h
 lea dx,filename 
 int 21h 
 jnc GetHeader

 lea dx, cantopen           ; wenn man sie nicht öffnen kann beenden
 call writes 
 jmp errorQUIT              ; beenden
 
GetHeader:
 xchg ax,bx                 ; Dateihandle in bx speichern

 mov ax,4202h               ; Größe 
 xor cx,cx                  ; in DX:AX ermitteln
 xor dx,dx 
 int 21h

 mov word ptr cs:[lenghthw], dx
 mov word ptr cs:[lenghtlw], ax

 mov ax,4200h               ; Pointer auf Dateistart setzen
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah,3fh                 ; Header ermitteln um zwischen COM
 mov cx,1Ah                 ; und EXE Dateien zu unterscheiden
 lea dx,header
 int 21h
 
 cmp word ptr cs:[header], 'MZ'  ;check EXE ?
 je EXE
 cmp word ptr cs:[header], 'ZM'  ;check COM ?
 je EXE


COM:                        ; wir haben eine COM Datei, die wir verschlüsseln
  lea dx, cryptCOM
 call writes

 cmp byte ptr cs:[offset header + 3h],'C'
 je crypted

 cmp word ptr cs:[offset lenghtlw], 60000d
 jae toobig

 mov ax, lenghtlw
 sub ax, 3h  ;vorher 7.. 
 mov word ptr cs:[offset jump+1], ax
                          ; jump an das Ende der Datei erstellen
                          ; damit wir dort unseren Code anhängen können
 lea si, header
 lea di, oldCOM
 movsw
 movsw

 mov ax,4200h             ; Pointer an den Start
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah,3fh               ; gesamte Datei einlesen
 mov cx,lenghtlw
 mov deltah, cx

 lea dx,Buffer
 int 21h

                          ; Sprung schreiben
 add deltah, offset delta - offset comcrypt
 add deltah, 100h
 sub deltah, offset delta 

 lea si, jump
 lea di, buffer
 movsw
 movsw


 call getRAND             ; Zufallszahl ermitteln
 mov key, dh
 
 mov si, [offset buffer+4h]
 mov cx, lenghtlw
 sub cx, 4h

 push bx
                          ; und gesamte Datei verschlüseln
 COMCRYPTLOOP:
   xor byte ptr cs:[si], dh   ; dh ist der Schlüssel
   xor byte ptr cs:[si], 0CCh
   inc si
 loop COMCRYPTLOOP  

 mov si, [offset buffer+4h]
 mov cx, lenghtlw
 sub cx, 4h
 xor bx, bx
 xor dx, dx
                          ; 2 simple Checksummen erstellen um zu sehen ob doch
                          ; jmd was an der Datei manipuliert hat
createcomchecksum:
 mov al, byte ptr cs:[si] ; Checksumme
 add bl, al
 xchg bl, bh
 add dx, bx
 xor dx, cx
 inc si
loop createcomchecksum

 mov comchecksum, bx      ; CRC speichern
 mov comchecksum2, dx

 pop bx

 mov ax,4200h             ; Zeiger auf den Anfang
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah, 40h              ; Neue COM - Datei schreiben
 mov cx, lenghtlw
 lea dx, buffer
 int 21h 

                          ; 2ten Entschlüssler verschlüsseln
 
 mov si, [offset comlayer2]
 mov cx, offset endcomcrypt - offset comlayer2
 encryptcomlayer2:
  rol byte ptr [si], 2h
  xor byte ptr [si],'S'
  inc si
 loop encryptcomlayer2

                          ; und anhängen
 mov ax,4202h             ; am Ende der Datei
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah, 40h
 mov cx, (offset endcomcrypt - offset comcrypt)
 lea dx, comcrypt
 int 21h 

 lea dx, success          ; Erfolgsnachricht rausschreiben
 call writes 

 mov ah, 3eh              ; Datei schließen
 int 21h
 jmp QUIT


EXE:                      ; Wir haben eine EXE-Datei
 lea dx, cryptEXE
 call writes
 jmp errorQUIT

Crypted:                  ; Datei ist bereits verschlüsselt
 lea dx, isCrypted
 call writes
 mov ah, 3eh              ; schließen
 int 21h
 jmp errorQUIT

toobig:                   ; Datei ist zu groß
 lea dx, big
 call writes
 mov ah, 3eh
 int 21h
 
errorQUIT:                ; Es gab Fehler
 lea dx, error
 call writes

QUIT:                     ; Programmende
 mov ah,4ch
 int 21h 





;****************** unser COM encrypter ***************************************>

comcrypt:                 ; Dieser Teil wird an die COM Datei angehängt

pusha

delta:
db 0bdh                   ; vorberechnetes delta offset
deltah dw ?
                          ; zweiter Entschlüssler wird entschlüsselt

mov si, offset comlayer2
add si, bp

mov cx, offset endcomcrypt - offset comlayer2
decryptcomlayer2:
 xor byte ptr cs:[si], 'S'
 ror byte ptr cs:[si], 2h
 inc si
 int 3h                   ; Breakpoint um Debugger zu nerven
loop decryptcomlayer2

comlayer2:

cmp byte ptr cs:[103h],'C'; Marke testen..
je decryptcom

int 20h                   ; Ende

decryptcom:

 in      al,21h           ; Keyboard ausschalten ( hilft bei manchen Debuggern )
 or      al,02h
 out     21h,al              

 neg sp                   ; dies schmeißt auch manche Debugger aus dem Ring ;)
 neg sp 

 int 3h

                          ; Checksum testen
 mov si, 104h
 mov cx, [bp+offset lenghtlw]
 sub cx, 4h
 xor bx, bx 
 xor dx, dx

testcomchecksum:
 mov al, byte ptr [si]
 add bl, al               ; <--Checksumme
 xchg bl, bh

 add dx, bx
 xor dx, cx

 inc si
loop testcomchecksum

mov ax, word ptr cs:[bp+offset comchecksum]
cmp bx, ax
je comfileok1
int 20h

comfileok1:
mov ax, word ptr cs:[bp+offset comchecksum2]
cmp dx, ax
je comfileok2
int 20h


comfileok2:
breakpoint db 0cch

push ss
pop ax
mov word ptr cs:[bp+offset oldss],ax
mov word ptr cs:[bp+offset oldsp],sp

push cs
pop ss

mov sp, 100h                 ; diese Art der Enschlüsselung stoppt int1h tracing
add sp, [bp+offset lenghtlw] ; da diese den Stack zerstören

mov cx, [bp+offset lenghtlw]
mov dh, byte ptr cs:[bp+offset key]

decrypt:
   pop ax                    ; Rest entschlüsseln
   mov dl, byte ptr cs:[bp+offset breakpoint]
   xor al, dl
   xor al, dh
   push ax                   ; Speichern
   dec sp                    ; (wir wollen Bytes keine Words entschlüsseln)
loop decrypt

mov sp, word ptr cs:[bp+offset oldsp]

mov ax, word ptr cs:[bp+offset oldss]
mov ss, ax

int 3h

mov di, 100h
lea si, [bp+offset oldCOM]
movsw
movsw

int 3h

in      al,21h                  ; Keyboard wieder anstellen
and     al,not 2
out     21h,al

popa

push 100h                       ; und originalcode ausführen
ret

oldCOM       db 0h,0h,0h,0h ;unsere alte com-Datei
key          db 0h
comchecksum  dw 0h
comchecksum2 dw 0h
lenghtlw     dw 0h
oldss        dw 0h
oldsp        dw 0h

endcomcrypt:



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

getRAND:               ;get a random number in dl
  mov ah, 2CH           ;get time
  int 21h 
  xor ch, cl
  xor dh, dl
  xor dh, ch 
  
  ror dh, cl
  
  randloop:  
   in al, 40h
   xor dh, al
   ror dh, 1h
  loop randloop

  cmp dh, 0h
  je getRAND
ret


;DATA
bcksp     db 8, 32, 8, '$'                                        ;Backspace
cpyrght   db 'DOS - EXE/COM Crypter by SnakeByte',10d,13d
          db 'written for KC E-zine #2 [Kryptographie]',10d,13d
          db 'contact me at www.KryptoCrew.de',10d,13d,'$'        ;Copyright Text


file      db 'Please enter file : ','$'                           ;get Filename
filename  db 50d dup (?)
          db 0h                                                   ;store it here
not_found db 'The file does not exist...',10d,13d,'$'
cantopen  db 'Unable to open file, please check the attributes',10d,13d,'$'
blank     db ' ',10,13,'$'
cryptCOM  db 'Loading COM-File',10d,13d,'$'
cryptEXE  db 'This is a EXE-File - stop execution',10d,13d,'$'
success   db 'Encryption finished !',10d,13d,'$'
isCrypted db 'File already crypted !',10d,13d,'$'
big       db 'Filesize too huge !',10d,13d,'$'
error     db 'An Error occured ! File is not encrypted !',10d,13d,'$'
header    db 1Ch dup (?)
lenghthw  dw 0h
jump      db 0e9h,0,0,'C'    ;unser jump zum encrypter
buffer:

ENDE:
code ends
end start