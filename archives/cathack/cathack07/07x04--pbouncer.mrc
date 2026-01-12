/*  Extracted from CatHack #007 ( www.sindominio.net/cathack )   */

;//////////////////////////////////////////////////////////////////////
; Bouncer.mrc escrit per Pop.tPH pel popIRK v0.8 i per la CatHack#007
;
;
; Per cargar-lo:
;   /load -rs bouncer.mrc
;

; %bouncer   = bouncer general
; %bouncerr  = conexió remota del tiu q entra
; %bouncerrr = conexió a la maquina remota
; &bouncerd  = buffer

on *:START:{
     window -a @bouncer 100 100 400 200
     aline 4 @bouncer Loaded popIRK bouncer...
     }

on *:SOCKLISTEN:bouncer:{
     if ( $sockerr > 0 ) {
      aline 10 @bouncer > Un segon usuari està intentant aprofitar el bouncer.
      halt
      }
     sockaccept bouncerr
     aline 10 @bouncer > Acceptada la conexió bouncer de $sock(bouncerr).ip
     sockwrite -n bouncerr BOUNCER del popIRK %versio a: %bouncer_out_ip %bouncer_out_port $chr(10) $chr(13)

     sockopen bouncerrr %bouncer_out_ip %bouncer_out_port
     }

   on *:SOCKOPEN:bouncerrr:{
     if ( $sockerr > 0 ) {
      sockwrite -n bouncerr --ERROR-- No es pot establir la conexió remota.
      sockclose bouncerr }
     }

   on *:SOCKCLOSE:bouncerr:{
     aline 10 @bouncer > Tancada la conexió bouncer de $sock(bouncerr).ip
     sockclose bouncerr
     sockclose bouncerrr
     }

   on *:SOCKCLOSE:bouncerrr:{
     aline 10 @bouncer > La maquina remota ha tancat la conexi¢ ( %bouncer_out_ip %bouncer_out_port )
     sockclose bouncerrr
     sockwrite -n bouncerr --ERROR-- La màquina remota ha tancat la conexió.
     sockclose bouncerr
     }

   on *:SOCKREAD:bouncerr:{
     sockread &bouncerd
     sockwrite bouncerrr &bouncerd
     }

   on *:SOCKREAD:bouncerrr:{
     sockread &bouncerd
     sockwrite bouncerr &bouncerd
     }


alias bouncer_on {
    aline 10 @bouncer > Bouncer activat ( ListenPort: %bouncer_in_port , RemoteIp: %bouncer_out_ip , RemotePort: %bouncer_out_port )
    socklisten bouncer %bouncer_in_port
    }

alias bouncer_conf {
    aline 10 @bouncer Bouncer v1.00 per Pop.tPH
    set %bouncer_in_port $?="Port d'entrada?"
    set %bouncer_out_ip   $?="IP remota?"
    set %bouncer_out_port $?="port remot?"
    }

menu menubar {
    popIRK bouncer
    ..Activar:/bouncer_on!
    ..Desactivar:/echo Bouncer desactivat | /sockclose bouncer
    ..-
    ..Configurar:/bouncer_conf
    ..-
    ..ListenPort ( %bouncer_in_port ):/set %bouncer_in_port $?="Listen Port?"
    ..RemoteIP ( %bouncer_out_ip ):/set %bouncer_out_ip $?="Remote IP?"
    ..RemotePort ( %bouncer_out_port ):/set %bouncer_out_port $?="Remote port?"
    }
