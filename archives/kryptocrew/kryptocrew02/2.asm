; Simpler Caesar Chiffre Crypter für
; Dateien bis zu 63kb und ein Passwort von maximal
; 12 Zeichen.. 
; geschrieben von SnakeByte für
; das 2. KryptoCrew E-Zine [Kryptographie]
;
; Ok Assembler ist vielleicht nicht jedermanns Sache, 
; aber das ist halt die Sprache, die mir am geläufigsten
; ist.. ;) .. 
;
; Gut was macht dieses Programm ?
;
; Es öffnet eine Datei und wenn sie als Endzeichen 'CC' 
; enthält, wird sie entschlüsselt ansonsten verschlüsselt.
; Die Ent- bzw Verschlüsselung funktioniert nach einem simplen
; Prinzip (Caesar Chiffre). Bei der Verschlüsselung wird einfach zu
; jedem Zeichen der Datei ein Zeichen des Passwortes aufaddiert
; aus der Zeichenfolge 23h, 43h, 12h mit dem Passwort 'Hai'
; ensteht also 23h + 'H', 43h + 'a', 12h + 'i'.. wenn die Datei
; größer als das Passwort ist (wohl meistens) wird nachdem das Passwort
; zuende ist, wieder beim Anfang des Passwortes angefangen. 
; Die Entschlüsselung funktioniert genau nach dem Entgegengesetzten
; Prinzip, nur hier wird nicht addiert sondern subtrahiert, was
; wieder den Originalwert ergibt. 
;
; Wo liegen die Schwächen dieser Verschlüsselungsmethode ?
; 
; Wenn man größere Teile der Originaldatei kennt (Bei Textdateien
; z.B. die 10d, 13d Markierung oder in EXE-Dateien den Header) kann
; man durch einfaches subtrahieren der bekannten Werte das
; Passwort ermitteln, wodurch man natürlich auch den Rest der
; Datei entschlüsseln kann. Am Besten umgeht man dieses Problem
; durch Kompression der Datei vor dem Verschlüsseln, was aber nicht
; in diesem (simplen) Programm angewandt wird. 
;
; 
; Ich hoffe ihr versteht den Code, ich denke er ist ausreichend
; kommentiert.. ;)
;          Viel Spaß beim lesen..  SnakeByte


.286                        ;Das Programm läuft auf 286+
code segment                ;Dies ist das Code-Segment
assume cs:code,ds:code      ;Definiert die einzelnen Segmente
org 100h                    ;Wir erstellen eine .COM Datei
jumps                       ;Die Sprünge werden berechnet..

Start:
 lea dx, cpyrght            ;schreib das Copyright auf
 call writes                ;den Bildschirm

 lea dx,file                ;fordere zur Eingabe der Datei auf
 call writes

 lea bx, filename           ;lese die Eingabe des Dateinamens
 mov dh, 50d                ;der maximal 50 Zeichen sein darf..
 call reads           
  
 mov ah,4eh                 ;suche die Datei
 xor cx,cx 
 lea dx,filename
 int 21h
 
 lea dx,blank               ;gebe eine Leerzeile auf dem Bildschirm aus
 call writes  
 
 jnc open_it                ;wenn die Datei gefunden wurde öffne sie

 lea dx, not_found          ;ansonsten gebe eine Errormeldung aus
 call writes 

 jmp QUIT                   ;Beende das Programm

open_it:                    ;hier wird die Datei geöffnet

 lea dx,blank               ;Leerzeile ausgeben
 call writes  
 
 lea dx,enterkey            ;fordere zur Eingabe des Passworts auf
 call writes

 lea bx, keystring          ;lese das Passwort (max 12 Zeichen)
 mov dh, 12d                ;ein
 call reads           

 lea dx,blank               ;Leerzeile
 call writes  

 mov ax,3d02h               ;öffne die Datei zum Lesen und Schreiben
 lea dx,filename
 int 21h

 jnc LoadFile               ;wenn es keinen Error gab lade die Datei

 lea dx, cantopen           ;ansonsten gib den Fehler auf dem
 call writes                ;Bildschirm aus

 jmp errorQUIT              ;Beende das Programm mit Fehlermeldung
 
LoadFile:
 xchg ax,bx                 ;Speicher das Datei-Handle

 mov ax,4202h               ;gehe zum Ende der Datei, was uns 
 xor cx,cx                  ;in DX:AX die Dateilänge 
 xor dx,dx                  ;ausgibt...
 int 21h

 mov word ptr cs:[lenghtlw], ax
                            ;speichern der Dateilänge

 cmp ax, 63000d             ;prüfe ob die Datei größer als 63kb ist
 jae toobig                 ;wenn ja beende..

 cmp ax, 0d                 ;prüfe ob die Datei 0 Bytes groß ist
 je toosmall                ;wenn ja beende..

 mov ax,4200h               ;gehe zum Anfang der Datei
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah,3fh                 ;lade nun die gesamte Datei in
 mov cx,lenghtlw            ;den Buffer um sie dort zu bearbeiten
 lea dx,buffer
 int 21h
 
 mov si, [offset buffer]    ;setze SI zum Ende der eingelesenen Datei
 add si, lenghtlw           ;um dort die Markierung zu testen
 sub si,2h

 cmp word ptr [si], 'CC'    ;ist dort unsere Markierung ?
 je DECRYPT                 ;wenn ja wird die Datei entschlüsselt

ENCRYPT:                    ;hier beginnt die Verschlüsselung

 lea di, keystring          ;Setze DI auf das Passwort
 mov si, [offset buffer]    ;SI ist die Datei
 mov cx, lenghtlw           ;CX ist die Anzahl der zu verschlüsselnden Bytes

 COMCRYPTLOOP:              ;für jedes Byte wird diese Schleife ausgeführt
   mov dh, byte ptr cs:[di] ;Lade ein Zeichen des Passwortes
   add byte ptr cs:[si], dh ;addiere es zu dem Byte der Datei
   inc di                   ;erhöhe DI (Passwortzeiger)
   cmp byte ptr cs:[di],0h  ;wenn DI auf 0h zeigt ist das Passwort zuende
   jne goon                 ;ansonsten fahren wir fort..
   lea di, keystring        ;wenn DI=0 dann setzen wir DI auf den Anfang des Passwortes
   goon:
   inc si                   ;erhöhe SI (Dateizeiger)
 loop COMCRYPTLOOP  

 mov word ptr [si], 'CC'    ;Setze nun unsere Markierung ans Ende der Datei

 mov ax,4200h               ;Gehe an den Anfang der Datei
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah, 40h                ;überschreibe die alte Datei mit der
 mov cx, lenghtlw           ;Verschlüsselten Version
 add cx, 2h
 lea dx, buffer
 int 21h 

 lea dx, successe           ;gebe eine Erfolgsmeldung aus
 call writes 

 mov ah, 3eh                ;schließen der Datei
 int 21h

 jmp QUIT                   ;beenden des Programmes



DECRYPT:                    ;hier wird die Datei entschlüsselt
 mov si, [offset buffer]    ;SI zeigt auf den Anfang der Datei
 mov cx, lenghtlw           ;cx enthält die Länge der Datei
 lea di, keystring          ;DI zeigt auf das Passwort
 
 COMCRYPTLOOP2:             ;wird für jedes Byte ausgeführt
   mov dh, byte ptr cs:[di] ;lade ein Zeichen des Passwortes in dh
   sub byte ptr cs:[si], dh ;ziehe dieses vom Wert in der Datei ab..
   inc di                   ;erhöhe DI (Passwortzeiger)
   cmp byte ptr cs:[di],0h  ;Passwortende ?
   jne goon2            
   lea di, keystring        ;wenn ja, dann fange wieder vorne an...
   goon2:
   inc si                   ;erhöhe SI (Dateizeiger)
 loop COMCRYPTLOOP2

 mov ah, 3eh                ;schließe die Datei
 int 21h

 mov ah, 41h                ;lösche die Datei von der Festplatte
 lea dx, filename           ;wir haben sie ja noch im Speicher
 int 21h  
 
 mov ah, 3Ch                ;erstelle eine neue Datei mit gleichem Namen
 lea dx, filename
 xor cx, cx
 int 21h
 
 xchg ax, bx                ;speicher das Datei-Handle

 mov ax,4200h               ;gehe zum Start der Datei
 xor cx,cx
 xor dx,dx
 int 21h

 mov ah, 40h                ;schreibe die Datei aus
 mov cx, lenghtlw           ;dem Speicher wieder auf
 sub cx, 2h                 ;die Festplatte, diesmal
 lea dx, buffer             ;ohne die Markierung
 int 21h 

 lea dx, successd           ;gib eine Erfolgsnachricht aus
 call writes 

 mov ah, 3eh                ;schließ die Datei
 int 21h

 jmp QUIT                   ;beende das Programm

toobig:
 lea dx, big                ;gib Fehlermeldung aus (zu groß)
 call writes
 
 mov ah, 3eh                ;schließen der Datei
 int 21h
 jmp errorQuit              ;beende das Programm mit Fehlermeldung

toosmall:
 lea dx, tsmall             ;gib Fehlermeldung aus (zu klein)
 call writes
 
 mov ah, 3eh                ;schließen der Datei
 int 21h
 
errorQUIT: 
 lea dx, error              ;gib eine Fehlermeldung aus..
 call writes

QUIT:
 mov ah,4ch                 ;Beende das Programm !
 int 21h 


;hier sind unsere Prozeduren definiert..

writes:                     ;schreibt den String in DX auf den 
 mov ah, 09h                ;Bildschirm
 int 21h
ret

reads:                      ;einlesen einer Eingabe
 xor cx, cx                 ;setze eine 0 nach bx (dem Platz
 mov byte ptr cs:[bx], cl   ;wo die Eingabe gespeichert wird)
readch:    
 call readc                 ;lese einen Buchstaben ohne ihn auszugeben
 cmp al, 0Dh                ;Enter ?
 jz readend                 ;beende das Einlesen
 cmp al, 08h                ;zurück ? (Backspace)
 jz readback                ;lösche den letzten Buchstaben
 cmp cl, dh                 ;Max. Anzahl der Eingabe erreicht ?
 jz readch                  ;fange wieder vorne an (warten auf Enter)
 test al, al                ;Alt, Strg etc ?
 jz readfunc                ;lese den nächsten Buchstaben
 mov dl, al                 ;gebe den Buchstaben
 call writec                ;aus
 mov [bx], al               ;und speicher ihn
 inc bx                     ;erhöhe Speicherplatz für Buchstaben
 inc cl                     ;Erhöhe Anzahl der eingelesenen Zeichen
 jmp readch                 ;lese den nächsten Buchstaben ein

readend:                    ;Hier ist das einlesen beendet
 mov byte ptr cs:[bx+1], 00h
ret

readfunc:
 call readc                 ;lese den nächsten Buchstaben
 jmp readch                 ;und fange wieder vorne an..

readback:                   ;entfernen der letzten Eingabe
 test cl, cl                ;gibt es eine vorherige Eingabe ?
 jz readch                  ;nein.. lese die nächste taste ein
 dec cl                     ;vermindere den Zähler
 dec bx                     ;vermindere den Zeiger
 mov byte ptr cs:[bx], 00h  ;entferne den letzten Buchstaben

 mov ch, dh                 ;dh speichern
 lea dx, bcksp              ;setze dx, auf die letzte gültige Eingabe
 call writes                ;restore den Bildschirm
 mov dh, ch                 ;dh wiederherstellen
 xor ch, ch                 ;ch löschen
 jmp readch

readc:                      ;lesen einer Taste ohne sie auszugeben
 mov ah, 07h
 int 21h
ret


writec:                     ;ausgeben des Zeichens in al
 mov ah, 02h
 int 21h
ret


;hier sind unsere Daten definiert..

bcksp     db 8, 32, 8, '$'
cpyrght   db 10d,13d,'Caesar Chiffre Crypter by SnakeByte',10d,13d
          db 'written for KC E-zine #2 [Kryptographie]',10d,13d
          db 'contact me at www.KryptoCrew.de',10d,13d,10d,13d,'$'

file      db 'Please enter file : ','$'
filename  db 50d dup (?)
          db 0h
not_found db 'The file does not exist...',10d,13d,'$'
cantopen  db 'Unable to open file, please check the attributes',10d,13d,'$'
blank     db ' ',10,13,'$'
successd  db 'Decryption finished !',10d,13d,'$'
successe  db 'Encryption finished !',10d,13d,'$'
big       db 'Filesize too huge ! 63kb Maximum Filesize',10d,13d,'$'
tsmall    db 'File is only 0 kb ...',10d,13d,'$'
error     db 'An Error occured ! File is not encrypted !',10d,13d,'$'
enterkey  db 'Please enter Password : ','$'
keystring db 12d dup (0h)
          db 0h
lenghtlw  dw 0h

buffer:

ENDE:
code ends
end start