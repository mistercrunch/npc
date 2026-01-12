{ Questo programma e' stato fatto da N0bodY88 in preda a convulsioni provocate
  dall'ansia per l'esame quindi non venite a dirmi che e' fatto da cani xche'
  lo so gia' :PPP
  Ho cercato di mettere tutte le cose che sapevo e le funzioni che ho imparato
  in questi 2-3 gg in cui ci ho dedicato un paio d'ore al gg tralasciando le
  operazioni su file che forse erano le piu' interessanti ma che spero di
  inserire nel prossimo "insieme di programm-ini" che vertera' all'accesso,
  alla creazione e alla cancellazione dei file (virus in vista :-). Ho anche
  lasciato perdere i vari interfacciamenti con l'assembler causa la mia piu'
  totale ignoranza di quest'ultimo (lacuna che spero di colmare presto
  prendendo qualche libro di asm e facendo diventare la lista degli interrupt
  la mia bibbia).
  Sperando che possa servire a qualcuno e che soprattutto riusciate a capirlo
  nonostante la mia complessita' anche nelle cose piu' semplici, ho introdotto
  anche un po' di algoritmi (non difficili) come il capovolgere una frase, lo
  spostare l'alfabeto, il distinguere un numero pari e uno dispari, il
  criptare e decriptare una frase fatta da parole numeri e altro in modo non
  difficile da crackare...hihihi (saluti a te o Maestro...ehm...Master).

  Assolutamente e sicuramente NO Coppi-right                                 }

Program angela;
uses Crt, Dos;
const
     data1:array [0..25] of char = ('A','B','C','D','E','F','G','H','I'
        ,'J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z');
     vocali:array [0..4] of char = ('a','e','i','o','u');
     consonanti:array [0..20] of char = ('b','c','d','f','g','h','j','k','l'
        ,'m','n','p','q','r','s','t','v','w','x','y','z');
     giorni:array [0..6] of String[9] = ('Domenica','Lunedi''','Martedi'''
        ,'Mercoledi''','Giovedi''','Venerdi''','Sabato');
     numeri:array [0..9] of char = ('0','1','2','3','4','5','6','7','8','9');
     lettere:array [0..25] of char = ('a','b','c','d','e','f','g','h','i'
        ,'j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z');
var
   data2,lettere2:array [0..25] of char;
   i,scelta,j,x,maiusc,minusc,voc,cons,spazi,numfrase,c,d:integer;
   pass,capov,olta,canc,frase,quellochevuoi,prog,par,comando,cri,nas,nas2:string;
   dirinfo:searchrec;
   anno,mese,giorno,ggsettimana,ore,minuti,secondi,centesimi,d1,d2,d3,d4:word;
   risposta,valore,risp:char;
   numero,n,sommap,sommad,contap,contad,controllo:byte;
   ascii,asci:array [0..255] of char;
   ascii2,asci2:array [0..255] of integer;
function leadingzero(w:word):String;
var
   s:string;
begin
     str(w:0,s);
     if (length(s)=1) then
        s:='0'+s;
     leadingzero := s;
end;
begin
clrscr;
  for i:=1 to 80 do
      begin
           TextColor(White);
           TextBackground(Black);
           if i=1 then
              begin
                   GotoXY(I,25);
                   Write('È');
                   HighVideo;
                   GotoXY(22,13);
                   TextColor(Blue+Blink);
                   TextBackground(White);
                   WriteLn('     MY 0NLY CRIME IS CURI0SITY   ');
                   HighVideo;
              end;
           if i=80 then
              begin
                   GotoXY(I,25);
                   Write('¼');
                   HighVideo;
              end;
           if (i<>1) and (i<>80) then
              begin
                   GotoXY(I,25);
                   Write('Í');
                   HighVideo;
                   GotoXY(I,2);
                   Write('Í');
                   HighVideo;
              end;
      end;
  for I:=1 to 23 do
      begin
           TextColor(White);
           TextBackground(Black);
           if i=1 then
              begin
                   GotoXY(1,1);
                   Write ('É');
                   HighVideo;
                   GotoXY(80,1);
                   Write ('»');
                   HighVideo;
              end;
           if (i<>1) then
              begin
                   GotoXY(1,I);
                   Write('º');
                   HighVideo;
                   GotoXY(80,I);
                   Write ('º');
                   HighVideo;
              end;
      end;
  for i:=1 to 40 do
      begin
           TextColor(Black);
           TextBackground(LightGreen);
           GotoXY(19+I,14);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 40 do
      begin
           TextColor(Black);
           TextBackground(LightRed);
           GotoXY(19+I,10);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 5 do
      begin
           TextColor(Black);
           TextBackground(Blue);
           GotoXY(19,9+I);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 5 do
      begin
           TextColor(Black);
           TextBackground(LightBlue);
           GotoXY(60,9+I);
           Write (' ');
           HighVideo;
      end;
 TextColor(cyan);
 TextBackground(black);
 GotoXY(69,23);
 Write ('by N0bodY88');
 HighVideo;
  readln;
  clrscr;
  GotoXY(10,3);
  TextColor(White);
  TextBackground(Black);
  Write ('Inserisci la tua password: ');
  read (pass);
  if (pass='angela') then
     begin
     repeat
     repeat
          clrscr;
          TextColor(White);
          TextBackground(Black);
          GotoXY(10,2);  writeln ('ELENCO POSSIBILI OPERAZIONI TRA CUI SCEGLIERE:           ');
          GotoXY(10,4);  writeln (' 1) CAPOVOLGIMENTO DI UNA FRASE                           ');
          GotoXY(10,5);  writeln (' 2) FARSI UN PROPRIO ALFABETO                             ');
          GotoXY(10,6);  writeln (' 3) ELENCA I FILE EXE E PAS CONTENUTI IN QUESTA DIRECTORY ');
          GotoXY(10,7);  writeln (' 4) ESTRAI UNA LETTERA A CASO                             ');
          GotoXY(10,8);  writeln (' 5) TOGLI UN PO'' DI LETTERE ALLA TUA FRASE               ');
          GotoXY(10,9);  writeln (' 6) CONTA VOCALI, CONSONANTI, MAIUSCOLE E MINUSCOLE       ');
          GotoXY(10,10); writeLN (' 7) RIEMPI LO SCHERMO DI CIO'' CHE VUOI                   ');
          GotoXY(10,11); writeln (' 8) ESEGUI UN PROGRAMMA CON ALCUNI PARAMETRI              ');
          GotoXY(10,12); writeln (' 9) VISUALIZZA LA LISTA DEGLI ERRORI DOS E LA SPIEGAZIONE ');
          GotoXY(10,13); writeln ('10) ESEGUI UN COMANDO DOS                                 ');
          GotoXY(10,14); writeln ('11) GUARDA LE LINEE DEL TUO DOS CARICATE IN MEMORIA       ');
          GotoXY(10,15); writeln ('12) LEGGI O IMPOSTA IL GIORNO, IL MESE E L''ANNO          ');
          GotoXY(10,16); writeln ('13) LEGGI O IMPOSTA L''ORA                                ');
          GotoXY(10,17); writeln ('14) CONTROLLA QUANTA MEMORIA LIBERA HAI                   ');
          GotoXY(10,18); writeln ('15) CONTA NUMERI PARI E DISPARI                           ');
          GotoXY(10,19); writeln ('16) GUARDA TUTTI I CODICI ASCII E RELATIVI VALORI ORDINALI');
          GotoXY(10,20); writeln ('17) CRIPTARE QUALCOSA IN MODO INSICURO                    ');
          GotoXY(10,21); writeln ('18) DECRIPTA QUELLO CHE HAI CRIPTATO AL PUNTO 17          ');
          GotoXY(10,22); writeln ('19) NOTE DELL''AUTORE                                     ');
          GotoXY(10,23); writeln ('20) USCIRE DA QUESTO PROGRAMMA ');
          GotoXY(10,25); writeln ('Cosa desideri fare ?');
          GotoXY(31,24); readln (scelta);
     until (scelta>0) and (scelta<21);
          IF SCELTA=1 THEN
             BEGIN
{ Per capovolgere una frase uso un semplice algoritmo che consiste nel leggere
  la lunghezza della frase con la funzione Length, dopodiche' si fa un ciclo
  che va da 1 alla lunghezza totale della frase e assegniamo a una nuova
  variabile azzerata prima del ciclo (x evitare problemi se si riusa questa
  parte del programma) una alla volta tutte le lettere, gli spazi e i numeri
  assegnando prima l'ultima lettera, poi la penultima, poi la terzultima e
  cosi' via. Per ottenere questo basta che alla lunghezza totale togliamo la I
  che da 1 andra' in crescendo. Alla fine non ci rimarra altro da fare che
  stampare la nostra nova variabile contenente tutti i valori della frase di
  'capov', solo che...capovolti :)                                           }
                  clrscr;
                  capov:=''; olta:='';
                  GotoXY(10,10); writeln ('Scrivi la frase da capovolgere. ');
                  GotoXY(10,12); readln (capov);
                  J:=length(capov);
                  olta:='';
                  FOR I:=1 TO J DO
                      BEGIN
                           olta:=olta+capov[j-I];
                      END;
                  GotoXY(10,15);
                  writeln ('La frase capovolta e'': ',capov[J]+olta);
                  GotoXY(10,17);
                  readln;
             END;
          IF SCELTA=2 THEN
             begin
                  clrscr;
                  GotoXY(10,10); writeln ('Dammi un numero da 1 a 26 ');
                  GotoXY(10,12); readln (X);
                  for I:=1 to (26-X+2) do
                      begin
                           data2[I-1]:=data1[I+X-2];
                      end;
                  for I:=1 to (X-1) do
                      begin
                           data2[I+26-X]:=data1[I-1];
                      end;
                  GotoXY(10,16); writeln ('Vediamo come sarebbe l''alfabeto se     ');
                  GotoXY(10,17); writeln ('iniziasse x la lettera che hai scelto: ');
                  GotoXY(10,20);
                  for I:=1 to 26 do
                      begin
                           write (data2[I-1]);
                      end;
                  readln;
             end;
          IF SCELTA=3 THEN
             begin
                   clrscr;
                   GotoXY(1,5); writeln ('ELENCO FILE .EXE: ');
                   FindFirst('*.EXE', Archive, DirInfo);
                   GotoXY(1,7);
                   while DosError = 0 do
                   begin
                        Writeln(DirInfo.Name);
                        FindNext(DirInfo);
                   end;
                   readln;
                   clrscr;
                   GotoXY(1,5); writeln ('ELENCO FILE .PAS: ');
                   FindFirst('*.PAS', Archive, DirInfo);
                   GotoXY(1,7);
                   while DosError = 0 do
                         begin
                              Writeln(DirInfo.Name);
                              FindNext(DirInfo);
                         end;
                   readln;
             end;
          IF SCELTA=4 THEN
             begin
                  clrscr;
                  GotoXY(10,8); write ('Per fermare e vedere la lettera random schiacciare la barra');
                  Randomize;
                  repeat
                        begin
                             GotoXY(10,10);
                             I:=random(26);
                             write ('La lettera estratta a caso risulta essere la ',data1[I]);
                        end;
                  until KeyPressed;
                  readln;
             end;
          IF SCELTA=5 THEN
             begin
                  clrscr;
                  GotoXY(10,6); writeln ('Scrivi una frase di almeno tre parole: ');
                  GotoXY(10,8); readln (canc);
                  repeat
                  GotoXY(10,10); writeln ('Dammi un numero minore della lunghezza della frase: ');
                  GotoXY(10,12); readln (I);
                  until I<Length(canc);
                  repeat
                  GotoXY(10,14); writeln ('Dimmi quante lettere cancellare: ');
                  GotoXY(10,16); readln (J);
                  until (j<(length(canc)-I));
                  Delete(canc,I,J);
                  GotoXY(10,18); Writeln('La frase con le lettere tolte risulta essere: ');
                  GotoXY(10,20); Writeln(canc);
                  readln;
             end;
          IF SCELTA=6 THEN
             begin
                  clrscr;
                  voc:=0; cons:=0; maiusc:=0; minusc:=0; spazi:=0; numfrase:=0;
                  GotoXY(10,6); writeln ('Scrivi una frase: ');
                  GotoXY(10,8); readln (frase);
                  for i:=1 to (length(frase)+1) do
                      begin
                           for j:=1 to 5 do
                               begin
                                    if ((frase[i-1])=vocali[J-1]) then
                                       begin
                                            voc:=voc+1;
                                            minusc:=minusc+1;
                                       end;
                                    if ((frase[i-1])=(upcase(vocali[J-1]))) then
                                       begin
                                            voc:=voc+1;
                                            maiusc:=maiusc+1;
                                       end;
                               end;
                               if ((frase[i-1])=' ') then
                                  begin
                                       spazi:=spazi+1;
                                  end;
                           for x:=1 to 10 do
                               begin
                                    if ((frase[i-1])=numeri[x-1]) then
                                       begin
                                            numfrase:=numfrase+1;
                                       end;
                               end;
                      end;
                  for i:=1 to (length(frase)+1) do
                      begin
                            for J:=1 to 21 do
                               begin
                                    if (frase[i-1]=consonanti[J-1]) then
                                       begin
                                            cons:=cons+1;
                                            minusc:=minusc+1;
                                       end;
                                    if ((frase[i-1])=(upcase(consonanti[J-1]))) then
                                       begin
                                            cons:=cons+1;
                                            maiusc:=maiusc+1;
                                       end;
                               end;
                      end;
                      GotoXY(10,11); writeln ('Le vocali nella tua frase sono     ',voc);
                      GotoXY(10,12); writeln ('Le consonanti nella tua frase sono ',cons);
                      GotoXY(10,13); writeln ('Le maiuscole nella tua frase sono  ',maiusc);
                      GotoXY(10,14); writeln ('Le minuscole nella tua frase sono  ',minusc);
                      GotoXY(10,15); writeln ('Gli spazi nella tua frase sono     ',spazi);
                      GotoXY(10,16); writeln ('I numeri nella tua frase sono      ',numfrase);
                      GotoXY(10,18); readln;
             end;
           IF SCELTA=7 THEN
              begin
              clrscr;
              GotoXY(10,10); writeln ('Scrivi quello che vuoi: ');
              GotoXY(10,12); writeln ('[ Se vuoi un consiglio prova a scrivere solo ALT+206 o ALT+197 ]');
              GotoXY(10,14); readln (quellochevuoi);
                     repeat
                           write (quellochevuoi);
                     until keypressed;
              end;
           IF SCELTA=8 THEN
              begin
              clrscr;
              GotoXY(10,10); writeln('Programma da eseguire: ');
              GotoXY(10,12); readln (prog);
              GotoXY(10,14); write('Parametri da passare al programma ',prog,' : ');
              GotoXY(10,16); readln(par);
              GotoXY(10,18); writeln('Adesso vediamo se funziona...');
              SwapVectors;
              Exec(prog,par);
              SwapVectors;
              clrscr;
              GotoXY(10,10); writeln ('...speriamo sia andato tutto a buon fine.');
              if (doserror<>0) then
                 begin
                      GotoXY(10,12);
                      writeln('Numero di errore di dos: ',DosError);
                 end
              else
                 begin
                      GotoXY(10,14); writeln('Esecuzione andata a buon fine...forse.');
                 end;
              readln;
              end;
           IF SCELTA=9 THEN
              begin
              clrscr;
                    GotoXY(10,3);  writeln (' LISTA ERRORI DOS E LORO SPIEGAZIONI ');
                    GotoXY(10,6);  writeln (' 2 --> File not found      ');
                    GotoXY(10,7);  writeln (' 3 --> Path not found      ');
                    GotoXY(10,8);  writeln (' 5 --> Access denied       ');
                    GotoXY(10,9);  writeln (' 6 --> Invalid handle      ');
                    GotoXY(10,10); writeln (' 8 --> Not enough memory   ');
                    GotoXY(10,11); writeln ('10 --> Invalid environment ');
                    GotoXY(10,12); writeln ('11 --> Invalid format      ');
                    GotoXY(10,13); writeln ('18 --> No more files       ');
                    GotoXY(10,15); writeln ('Ho voluto mettere questa lista perche'' il        ');
                    GotoXY(10,16); writeln ('programma scelto con l''8 puo'' riportare degli   ');
                    GotoXY(10,17); writeln ('errori dovuti a poca memoria o altro e cosi''     ');
                    GotoXY(10,18); writeln ('da questa lista potete capire perche'' non funzia.');
                    GotoXY(10,20); readln;
              end;
           IF SCELTA=10 THEN
{ QUESTA FUNZIONE VIENE ESEGUITA SENZA PROBLEMI SOLO SE IL FILE ANGELA E'
  CONTENUTO NELLA DIRECTORY C:\WINDOWS\COMMAND [se in modalita' windows] O
  NELLA DIRECTORY C:\DOS [se in modalita' dos] ... O ALMENO CREDO }
              begin
              clrscr;
              GotoXY(10,10); writeln ('Dammi il comando dos che vuoi eseguire: ');
              GotoXY(10,12); readln (comando);
              if (comando<>'') then
                 begin
                      comando:='/C ' + comando;
                      swapvectors;
                      exec(getenv('COMSPEC'), comando);
                      swapvectors;
                      if (DosError<>0) then
                         begin
                              GotoXY(10,14); writeln ('Non si riesce a far eseguire command.com');
                              GotoXY(10,16); readln;
                         end
                 end;
              readln;
              end;
           IF SCELTA=11 THEN
              begin
              clrscr;
              for i:=1 to envcount do
                  writeln(envstr(i));
              readln;
              end;
           IF SCELTA=12 THEN
              begin
                   clrscr;
                   getdate(anno,mese,giorno,ggsettimana);
                   GotoXY(10,6);
                   writeln('Oggi e'' esattamente ',giorni[ggsettimana],' ',giorno:0, '/', mese:0, '/', anno:0);
                   GotoXY(10,8); readln;
                   anno:=0; mese:=0; giorno:=0;
                   GotoXY(10,10); writeln ('Vuoi cambiare la data ? (S/N o s/n)');
                   GotoXY(10,12); readln (risposta);
                   if (risposta='s') or (risposta='S') then
                      begin
                      clrscr;
                      GotoXY(10,3);
                      writeln ('ATTENZIOME se immettete dati errati la data non variera''.');
                      GotoXY(10,5);
                      writeln ('Dammi il nuovo giorno in numeri : ');
                      GotoXY(10,7);readln (giorno);
                      GotoXY(10,9);
                      writeln ('Dammi il nuovo mese in numeri: ');
                      GotoXY(10,11);readln (mese);
                      GotoXY(10,13);
                      writeln ('Dammi il nuovo anno in numeri: ');
                      GotoXY(10,15);readln (anno);
                      setdate(anno,giorno,mese);
                      GotoXY(10,17);
                      writeln ('Ora la data e'' impostata sul seguente giorno:');
                      GotoXY(10,19);
                      getdate(anno,mese,giorno,ggsettimana);
                      writeln(giorni[ggsettimana],' ',giorno:0, '/', mese:0, '/', anno:0);
                      GotoXY(10,21); Readln;
                      end;
              end;
           IF SCELTA=13 THEN
              begin
                   clrscr;
                   risposta:='n';
                   gettime(ore,minuti,secondi,centesimi);
                   GotoXY(10,6);
                   writeln('Adesso sono esattamente le ',leadingzero(ore),
                           ':',leadingzero(minuti),':',leadingzero(secondi),
                           '.',leadingzero(centesimi));
                   GotoXY(10,8); readln;
                   GotoXY(10,10); writeln ('Vuoi cambiare l''ora ? (S/N o s/n)');
                   GotoXY(10,12); readln (risposta);
                   if (risposta='s') or (risposta='S') then
                      begin
                           clrscr;
                           GotoXY(10,6);  writeln ('Dammi il primo dato: ');
                           GotoXY(35,6);  readln (d1);
                           GotoXY(10,8);  writeln ('Dammi il secondo dato: ');
                           GotoXY(35,8);  readln (d2);
                           GotoXY(10,10); writeln ('Dammi il terzo dato: ');
                           GotoXY(35,10); readln (d3);
                           GotoXY(10,12); writeln ('Dammi il quarto dato: ');
                           GotoXY(35,12); readln (d4);
                           settime(d1,d2,d3,d4);
                           GotoXY(10,14);
                           writeln('Adesso l''ora e'' impostata sulle '
                           ,leadingzero(ore),':',leadingzero(minuti),
                           ':',leadingzero(secondi),'.',leadingzero(centesimi));
                           GotoXY(10,16); readln;
                      end;
              end;
           IF SCELTA=14 THEN
              begin
                   clrscr;
                   GotoXY(10,10); writeln ('Hai ben ',memavail, ' byte liberi.');
                   GotoXY(10,12); writeln ('Il blocco libero piu'' grande e'' di ',maxavail,' byte.');
                   GotoXY(10,14); readln;
              end;
           IF SCELTA=15 THEN
           begin
           clrscr;
           x:=5;
                  repeat
                  clrscr;
                  repeat
                        GotoXY(10,3); writeln('Dammi il numero degli addendi: ');
                        GotoXY(10,4); readln (n);
                  until (n>0);
                        I:=0; sommap:=0; sommad:=0; contap:=0; contad:=0;
                        repeat
                              GotoXY(10,x+1); writeln('Dammi un numero da sommare: ');
                              GotoXY(10,x+2); readln(numero);
                              controllo:=numero;
                              x:=x+2;
                                     if odd(numero) then
                                        begin
                                             GotoXY(16,x);
                                             writeln ('...questo numero e'' --> Dispari');
                                        end
                                     else
                                         begin
                                             GotoXY(16,x);
                                             writeln ('...questo numero e'' --> Pari');
                                        end;
                              repeat
                                    controllo:=controllo-2
                              until (controllo=0) or (controllo=1);
                              if (controllo=0) then
                                 begin
                                      sommap:=sommap+numero;
                                      contap:=contap+1;
                                 end
                              else
                                  begin
                                       sommad:=sommad+numero;
                                       contad:=contad+1;
                                  end;
                        i:=i+1;
                        GotoXY(10,x+1); until i=n;
                        readln;
                        clrscr;
                        GotoXY(10,4); writeln('La somma dei numeri pari e'' uguale a      ',sommap);
                        GotoXY(10,5); writeln('I numeri pari sono uguali a               ',contap);
                        GotoXY(10,6); writeln('La somma dei numeri dispari e'' uguale a   ',sommad);
                        GotoXY(10,7); writeln('I numeri dispari sono uguali a            ',contad);
                        GotoXY(10,9); writeln('Vuoi calcolare la somma di altri numeri? (S/N o s/n)');
                        GotoXY(10,11); readln(risp);
                  until (risp='n') or (risp='N');
              end;
           IF SCELTA=16 THEN
              begin
              clrscr;
              x:=5;
                     GotoXY(6,3); writeln ('Questa e'' la lista dei codici ascii con relativi valori ordinali: ');
                     for i:=1 to 255 do
                         begin
                              ascii[i]:=(chr(i));
                              ascii2[i]:=(ord(i));
                         end;
                     clrscr;
                     for i:=1 to 25 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=25 to 50 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=50 to 75 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=75 to 100 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=100 to 125 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=125 to 150 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=150 to 175 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=175 to 200 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=200 to 225 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=225 to 250 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
                     clrscr;
                     for i:=250 to 255 do
                         begin
                              writeln ('Il carattere ascii ',ascii[i],' si puo'' ottenere usando ALT+',ascii2[i]);
                         end;
                     readln;
              end;
          IF SCELTA=17 THEN
             begin
                  clrscr;
                  GotoXY(10,4); writeln ('Dammi un numero da 1 a 26 ');
                  GotoXY(10,5); readln (X);
                  for I:=1 to (26-X+2) do
                      begin
                           lettere2[I-1]:=lettere[I+X-2];
                      end;
                  for I:=1 to (X-1) do
                      begin
                           lettere2[I+26-X]:=lettere[I-1];
                      end;
                  GotoXY(10,7); writeln ('Ora vediamo il confronto tra l''alfabeto normale e');
                  GotoXY(10,8); writeln ('quello che inizia x la lettera che hai scelto: ');
                  GotoXY(10,9);
                  for I:=1 to 26 do
                      begin
                           write (lettere[I-1]);
                      end;
                  GotoXY(10,10);
                  for I:=1 to 26 do
                      begin
                           write (lettere2[I-1]);
                      end;
                  readln;
                  GotoXY(10,12); writeln ('Ora proviamo a criptare qualcosa. Scrivi una frase');
                  GotoXY(10,13); readln (cri);
                  nas:=''; c:=0;
                  for i:=1 to Length(cri)+1 do
                      begin
                           for j:=1 to 26 do
                               begin
                                    if cri[i-1]=upcase(lettere[j-1]) then
                                       begin
                                            nas:=nas+(upcase(lettere2[j-1]));
                                       end
                                    else
                                        begin
                                             c:=c+1;
                                        end;
                                    if cri[i-1]=lettere[j-1] then
                                       begin
                                            nas:=nas+lettere2[j-1];
                                       end
                                    else
                                        begin
                                             c:=c+1;
                                        end;
                                    if cri[i-1]=' ' then
                                       begin
                                            nas:=nas+' ';
                                            j:=26;
                                       end
                                    else
                                        begin
                                             c:=c+1;
                                        end;
                               end;
                           if c=78 then
                              begin
                                   nas:=nas+cri[i-1];
                                   c:=0;
                              end;
                           c:=0;
                      end;
                  GotoXY(10,16); writeln ('Questa e'' la tua frase/parola criptata col nuovo alfabeto:');
                  GotoXY(10,18); writeln (nas);
                  nas2:=nas;
                  GotoXY(10,20); readln;
             end;
           IF SCELTA=18 THEN
              begin
              clrscr;
              nas:=nas2;
                     if nas='' then
                        begin
                             GotoXY(8,6);
                             writeln ('Devi eseguire il punto 17 prima di questo punto.');
                             GotoXY(8,7); readln;
                        end
                     else
                         begin
                              GotoXY(8,3);
                              writeln ('Ora vediamo se riusciamo a decriptare la seguente frase:');
                              GotoXY(8,5); writeln (nas);
                              GotoXY(8,6); readln;
                              repeat
                              GotoXY(8,8);
                              writeln ('Inserisci un numero con il quale provare a decriptare la frase: ');
                              GotoXY(8,9); readln (x);
                              until (x>0) and (x<27);
                              for I:=1 to (26-X+2) do
                                  begin
                                       lettere2[I-1]:=lettere[I+X-2];
                                  end;
                              for I:=1 to (X-1) do
                                  begin
                                       lettere2[I+26-X]:=lettere[I-1];
                                  end;
                              GotoXY(8,11); writeln ('Ora vediamo il solito confronto tra l''alfabeto normale e');
                              GotoXY(8,12); writeln ('quello che inizia x la lettera che hai scelto: ');
                              GotoXY(8,13);
                              for I:=1 to 26 do
                                  begin
                                       write (lettere[I-1]);
                                  end;
                              GotoXY(8,14);
                              for I:=1 to 26 do
                                  begin
                                       write (lettere2[I-1]);
                                  end;
                              GotoXY(8,15); readln;
                              GotoXY(8,16); writeln ('Ora proviamo a decriptare la frase immessa al punto 17: ');
                              cri:=''; d:=0;
                              for i:=1 to Length(nas)+1 do
                                  begin
                                       for j:=1 to 26 do
                                           begin
                                                if nas[i-1]=upcase(lettere2[j-1]) then
                                                   begin
                                                        cri:=cri+(upcase(lettere[j-1]));
                                                   end
                                                else
                                                    begin
                                                         d:=d+1;
                                                    end;
                                                if nas[i-1]=lettere2[j-1] then
                                                   begin
                                                        cri:=cri+lettere[j-1];
                                                   end
                                                else
                                                    begin
                                                         d:=d+1;
                                                    end;
                                                if nas[i-1]=' ' then
                                                   begin
                                                        cri:=cri+' ';
                                                        j:=26;
                                                   end
                                                else
                                                    begin
                                                         d:=d+1;
                                                    end;
                                           end;
                                       if d=78 then
                                          begin
                                               cri:=cri+nas[i-1];
                                               d:=0;
                                          end
                                       else if (d<>78) then
                                           begin
                                                d:=0;
                                           end;
                                  end;
                              GotoXY(8,18); writeln (cri);
                              GotoXY(10,19); readln;
                         end;
              end;
           IF SCELTA=19 THEN
              begin
              clrscr;
                     GotoXY(8,2); writeln  (' NOTE DI N0bodY88');
                     GotoXY(8,3); writeln  (' Questo programma e'' frutto di alcune ore di studio');
                     GotoXY(8,4); writeln  (' del PASCAL fatte solamente sulla base degli Help che');
                     GotoXY(8,5); writeln  (' ci sono in Turbo Pascal, quindi se volete dirmi che');
                     GotoXY(8,6); writeln  (' voi ci avreste messo meta'' del tempo, l''avreste fatto');
                     GotoXY(8,7); writeln  (' meglio e senza errori...beh...non ditemelo che tanto lo');
                     GotoXY(8,8); writeln (' so gia'' :P ');
                     GotoXY(8,9); writeln (' Per quanto riguarda i molti GotoXY e la poca, scarsa e complessa');
                     GotoXY(8,10); writeln (' grafica considerate che la libreria Graph non mi funzia quindi...');
                     GotoXY(8,11); writeln (' Ci tengo a precisare che ho corretto molti degli errori presenti');
                     GotoXY(8,12); writeln (' sia di codice che di logica ma non assicuro che non ce ne siano');
                     GotoXY(8,13); writeln (' molti altri eheheh');
                     GotoXY(8,14); writeln (' Dedico questo programma a: tutti gli Spippolatori con in testa');
                     GotoXY(8,15); writeln (' RigoR MorteM e Jamil, a tutti quelli del NewBies Staff che magari');
                     GotoXY(8,16); writeln (' adesso sono al numero 10 della e-zine, ai DiB, al S0ftProject con in');
                     GotoXY(8,17); writeln (' testa Spirit, Cavallo, Fusys e LordFelix, alle BadLands, e in particolar');
                     GotoXY(8,18); writeln (' modo a mia madre per tutto quello che ha fatto per me.');
                     GotoXY(8,19); writeln (' Un augurio speciale a Chrome per il suo "Supermercato dell''hacker"');
                     GotoXY(8,20); writeln (' (si chiama cosi'' mi hanno detto... e'' vero? hihihi)');
                     GotoXY(8,21); writeln (' Infine un ultimo saluto al mitico ULNTWH99 e ad Alfredo, amici');
                     GotoXY(8,22); writeln (' che confermano il detto "chi trova un amico trova un tesoro".');
                     GotoXY(8,23); writeln (' Un ringraziamento molto particolare a RigoR MorteM al');
                     GotoXY(8,24); writeln (' quale dico che il pacchetto lo tengo ancora x ricordo...');
                     GotoXY(8,26); readln;
              end;
     until (scelta=20);
     clrscr;
  for i:=1 to 80 do
      begin
           TextColor(White);
           TextBackground(Black);
           if i=1 then
              begin
                   GotoXY(I,25);
                   Write('È');
                   HighVideo;
                   GotoXY(22,13);
                   TextColor(White);
                   TextBackground(Black);
                   WriteLn('        CONOSCENZA E'' POTERE        ');
                   HighVideo;
              end;
           if i=80 then
              begin
                   GotoXY(I,25);
                   Write('¼');
                   HighVideo;
              end;
           if (i<>1) and (i<>80) then
              begin
                   GotoXY(I,25);
                   Write('Í');
                   HighVideo;
                   GotoXY(I,2);
                   Write('Í');
                   HighVideo;
              end;
      end;
  for I:=1 to 23 do
      begin
           TextColor(White);
           TextBackground(Black);
           if i=1 then
              begin
                   GotoXY(1,1);
                   Write ('É');
                   HighVideo;
                   GotoXY(80,1);
                   Write ('»');
                   HighVideo;
              end;
           if (i<>1) then
              begin
                   GotoXY(1,I);
                   Write('º');
                   HighVideo;
                   GotoXY(80,I);
                   Write ('º');
                   HighVideo;
              end;
      end;
  for i:=1 to 40 do
      begin
           TextColor(Black);
           TextBackground(LightRed);
           GotoXY(19+I,14);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 40 do
      begin
           TextColor(Black);
           TextBackground(LightGreen);
           GotoXY(19+I,10);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 5 do
      begin
           TextColor(Black);
           TextBackground(Blue);
           GotoXY(19,9+I);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 5 do
      begin
           TextColor(Black);
           TextBackground(LightBlue);
           GotoXY(60,9+I);
           Write (' ');
           HighVideo;
      end;
 TextColor(Cyan);
 TextBackground(Black);
 GotoXY(69,23);
 Write ('by N0bodY88');
 HighVideo;
     end;
  if (pass<>'angela') then
      begin
           GotoXY(10,5);
           write ('Hai fallito... che lamer :PPP hihihi');
           readln;
           clrscr;
  for i:=1 to 80 do
      begin
           TextColor(White);
           TextBackground(Black);
           if i=1 then
              begin
                   GotoXY(I,25);
                   Write('È');
                   HighVideo;
                   GotoXY(22,13);
                   TextColor(White);
                   TextBackground(Black);
                   WriteLn('        CONOSCENZA E'' POTERE        ');
                   HighVideo;
              end;
           if i=80 then
              begin
                   GotoXY(I,25);
                   Write('¼');
                   HighVideo;
              end;
           if (i<>1) and (i<>80) then
              begin
                   GotoXY(I,25);
                   Write('Í');
                   HighVideo;
                   GotoXY(I,2);
                   Write('Í');
                   HighVideo;
              end;
      end;
  for I:=1 to 23 do
      begin
           TextColor(White);
           TextBackground(Black);
           if i=1 then
              begin
                   GotoXY(1,1);
                   Write ('É');
                   HighVideo;
                   GotoXY(80,1);
                   Write ('»');
                   HighVideo;
              end;
           if (i<>1) then
              begin
                   GotoXY(1,I);
                   Write('º');
                   HighVideo;
                   GotoXY(80,I);
                   Write ('º');
                   HighVideo;
              end;
      end;
  for i:=1 to 40 do
      begin
           TextColor(Black);
           TextBackground(LightRed);
           GotoXY(19+I,14);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 40 do
      begin
           TextColor(Black);
           TextBackground(LightGreen);
           GotoXY(19+I,10);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 5 do
      begin
           TextColor(Black);
           TextBackground(Blue);
           GotoXY(19,9+I);
           Write (' ');
           HighVideo;
      end;
  for i:=1 to 5 do
      begin
           TextColor(Black);
           TextBackground(LightBlue);
           GotoXY(60,9+I);
           Write (' ');
           HighVideo;
      end;
 TextColor(Cyan);
 TextBackground(Black);
 GotoXY(69,23);
 write ('by N0bodY88');
 HighVideo;
 end;
readln;
end.
