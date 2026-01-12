;##LA FORMULA DE LUHN  IMPLEMENTADA EN ENSAMBLADOR PARA LINUX## 
;##############################################################
;La formula de luhn es utilizada para comprobar la validez de tarjetas
;de credito por ejemplo, para una aplicacion de andar por casa veamos esto. 
;El ultimo digito del IMEI de un movil es obtenido al aplicar la formula de 
;luhn sobre los 14 digitos restantes situados a su izkierda. El IMEI de un 
;movil puedes averiguarlo tecleando *#06#
;
;
;EXPLICACION
;
;La formula de luhn se basa en sencillas operaciones matematicas, en este caso
;vamos a verlo para comprobar o generar un IMEI valido para un movil. Dados
;14 numeros interprentandolos de tal manera que  el de mas a la derecha es
;el menos significativo  y pudiendolos encuadrar en un array de 
;n-1 numeros, doblaremos el valor de las posiciones impares y sumaremos el 
;resultado de cada operacion siendo los valores >= 10 partidos a la hora de 
;realizar la suma,esto es, si tenemos 14 su suma se realizaria de esta manera:
; 1+4=5.
;Posteriormente sumaremos las posiciones pares sin realizar ninguna 
;operacion mas sobre eios.
;A continuacion sumaremos los resultados de las sumas de los pares y los 
;impares.
;Si este numero termina en 0 el codigo de comprobacion debera ser 0 en caso
;contrario el codigo de comprobacion sera el resultado de la resta entre  
;el primer numero maior ke el obtenido y ke termine en 0 y el propio numero
;obtenido. 
;Esto que puede parecer asi al principio un poco confuso se ve mas claro 
;en este ejemplo:
;
;Tenemos este IMEI : 12345678901234X donde X es el digito de comprobacion
;
;por lo tanto esto kedara asi  
;N14 N13 N12 N11 N10 N09 N08 N07 N06 N05 N04 N03 N02 N01
; 1   2   3   4   5   6   7   8   9   0   1   2   3   4 
;    x2	     x2      x2      x2      x2      x2      x2 
;    __      __      __      __      __      __      __
;    4       8       12      16      0       4       8 
;
;Vamos a sumar sus resultados:
;(Recordad que si un numero es >=10 se deben sumar sus cifras. p.j 15=1+5)
;4+8+1+2+1+6+0+4+8 =34
;Ahora sumamos los pares directamente
;1+3+5+7+9+1+3=29
;Sumamos ambos valores
;34+29=63
;Este numero no termina en 0 por lo que el digito de comprobacion se obtendra 
;al koger el primer numero  por encima de 63 ke termine en 0 es decir 70 
;y restarlo de 63 => 70-63=7 
;Entonces 7 es el numero que validaria el IMEI ke nos hemos inventado
;Weno pues despues de este roio aki teneis la implementacion en ensamblador
;para linux de este algoritmo.  
;
;Para compilar: nasm -f elf luhngenerator.asm
;	        gcc -o  luhngenerator luhngenerator.o
;
;Un saludo para todo el Soviet clan y a la gente de #crackers :) 
;Un kohete para xezaw y darkblack . #)
;providor
;hexadem@gmx.net -= http://providor.cjb.net =-  oi! oi!

;EMPIEZA EL CODIGO=====================================

global main

;usare funciones de todos conocidas en c, asi veremos como funcionan
;a "bajo nivel"

extern scanf
extern printf
extern strlen
extern atoi
extern strncpy

section .data

;VARIABLES DE TEXTO

msg0 db "-= LUHN FORMULA CHECKSUM DIGIT GENERATOR =- by providor",0Dh,0Ah,0
msg1 db "El codigo correspondiente a esta secuencia es: 0",0Dh,0Ah,0
msg2 db "El numero debe de ser de 14 cifras",0Dh,0Ah,0
msg3 db "Introduzca su IMEI (primeros 14 digitos): ",0 
msgB db "El digito correspondiente a esta secuencia es: %s ",0Dh,0Ah,0
msg5 db "%s\n"

;esta variable nos servira para almacenar el caracter que le pasare a atoi
in_t1 db 00h,0

;variables donde guardaremos los valores de las sumas 
in_t3 db 00h
in_t4 db 00h

;variable donde almacenaremos el digito deseado
checkdigit db 00h,0Ah,0Dh,0

;variables temporales
val_temp db 00h
val_temp2 db 00h

;Donde almacenaremos nuestra cadena convertida a enteros 
Atoier  db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h

;Donde almacenaremos la cadena de caracteres introducida por teclado
imei times 14 db 00h,0

Section .text

main:
	push dword msg0  ;sin comentarios :P
	call printf	
	add esp,4
	push dword msg3
	call printf
 	add esp,4	
	push dword imei  ;pillamos la entrada de usuario mediante scanf 
	push dword msg5 
	call scanf
	add esp,8
	push dword imei 
	call strlen
	add esp,4
	cmp  eax,0Eh	;es la longitud correcta de un IMEI?
	je pinta_mil 
	 
no_pinta_nada:
	push dword msg2 ;nos la intentan dar
	call printf
	jmp  salir
pinta_mil:
	xor ecx,ecx	;preparamos el contador para el bucle
	mov edi,imei	;edi tiene la direccion base de la cadena introducida
	xor edx,edx
bucle_pun:
	cmp ecx,0Eh	;hemos recorrido los 14 caracteres?
	jz  fin_bucleatoi1
	mov ah,byte  [edi+ecx]	;warda en ah el caracter que le pasare a atoi
        mov [in_t1],ah
	push ecx	;salvamos el contador antes de entrar a atoi ke lo mata
	push dword in_t1
	Call atoi
	add esp,4	;restauramos la mitad de la pila correspondiente a in_t1
	pop ecx		;por alguna extrania razon :P atoi me jode $ecx 
	mov [Atoier+ecx],al ;wardamos el resultado en nuestra secuencia
	inc ecx		;incrementamos el contador 
	jmp bucle_pun
fin_bucleatoi1:
 	mov edi,Atoier  ;edi tiene la dire base de la secuencia numerica 	
	xor eax,eax
	mov ecx,0Dh	;preparamos el contador en sentido inverso      
	xor edx,edx
dale_cera_impares:
	cmp ecx,-1 ;hemos recorrido ia todas las posiciones impares?
	je fin_bucleimpares
	mov al,byte [edi+ecx] ;cogemos el digito correspondiente impar 
	shl al,1             ;lo multiplicamos por 2
	mov [val_temp],al    ;almacenamos el resultado en una variable temporal
	sub al,0Ah	     ;le restamos 10 para ver si hay ke partirlo	
	or  al,al	     ;el numero es 10?
	jz suma1	     ;directamente sumamos 1 ya que 1+0=1
	jg parte_numero	     ;es maior que 10 y hay ke partirlo
	add dl,[val_temp]    ;es menor que 10 y lo sumamos directamente
	sub ecx,2 	     ;decrementamos el contador al siguiente impar 
	jmp dale_cera_impares 
suma1:
	add dl,1 
	sub ecx,2
	jmp dale_cera_impares

parte_numero:
	mov al,[val_temp]
	sub al,0Ah	    ;cualquier numero maior que 10 partido quedara
	add al,1 	    ;asi, por ejemplo 15-10=5 y siempre habra que 
	add dl,al	    ;sumarle 1 debido a la particion, 1+5   
	sub ecx,2
	jmp dale_cera_impares

fin_bucleimpares:
	mov [in_t3],edx	    ;wardamos el valor de la suma de posiciones impares
 	mov ecx,0Ch	    ;preparamos el contador inverso para los pares
	xor edx,edx
bucle_pares:
	cmp ecx,-2  ;hemos recorrido todos los pares?? 	
	je fin_buclepares
	mov al,byte [edi+ecx]
	add dl,al  ;directamente sumamos el contenido de las posiciones pares
	sub ecx,2
	jmp bucle_pares 
fin_buclepares
	xor ebx,ebx
	mov al, [in_t3]    ;recuperamos el valor de la suma impar
	mov bl,dl	   ;wardamos en bl el valor de la suma par 
	add al,bl	   ;sumamos ambos
	xor ah,ah          ;limpiamos  ah 
	push eax	   ;salvamos el valor de la suma	
	xor ecx,ecx
	mov ecx,0Ah        ;vamos a dividir ese numero por 10
	xor edx,edx
	div ecx 	
	or edx,edx 	   ;mod 10 =0? 
	jz Codigo_0	   ;entonces directamente el digito de checksum es 0
	xor ecx,ecx	   ;no lo es pues preparamos el bucle
	pop edx		   ;recuperamos el valor de la suma de pares e impares
generar_digit:
	cmp ecx,0Ch	   ;el maximo numero que se puede generar en un IMEI es
	jg error_chungo	   ;126 asi que restamos del numero obtenido 10 hasta
	inc ecx		   ;que este sea menor que 0 para poder obtener 
	sub dl,0Ah	   ;el digito de checksum, ke es el korrespondiente 	
	or dl,dl	   ;a restar del numero obtenido en las sumas con  
	jl pinta_codigo    ;el primer numero por encima de el que termine en 0
	jmp generar_digit  ;Esto seria asi, si tenemos 23 restamos 10 hasta  
Codigo_0:		   ;konseguir iegar a 3-10 kon lo ke tenemos el mismo 	
 	push dword msg1    ;resultado que si restaramos 30-23 esto .       
	call printf
	add esp,4
	jmp salir	
pinta_codigo:
	not dl		  ;el resultado de la resta lo hacemos positivo
	inc dl		  ;y lo incrementamos con lo que tenemos el numero :)
	add dl,30h        ;lo convertimos a caracter para  sacarlo x pantalla  
	mov [checkdigit],dl
	push dword checkdigit
	push dword msgB
	call printf	
	add esp,8
error_chungo:		  ;algo raro ha pasado y salimos
salir: 
        mov eax,1
	xor ebx,ebx
 	int 80h
