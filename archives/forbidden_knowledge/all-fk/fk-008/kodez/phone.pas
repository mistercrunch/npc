{ phone.pas - Make your phone ring - Sigma <sigma@mail.com>
-----------------------------------------------------------

This is really just an implementation of the lame old star-one-hash
trick. I coded it due to boredom. It works on a Telkom SA phone line.
I suppose you could adapt it to make an alarm clock or someting. }

program makedaphonering;
uses crt;
const baseport =  $3E8;
{CHANGE baseport above to the com port of your modem:
 COM1 - $3F8   COM2 - $2F8
 COM3 - $3E8   COM4 - $2E8}

procedure sendstring(str : string);
var x : integer;
begin
     for x := 1 to length(str) do
         port[baseport] := ord(str[x]);
end;

begin
     sendstring('ATDT*1#'#13);  {Dial "*1#"}
     delay(2500); {Increase this delay if the line doesn't ring}
     sendstring('AT+++H0'#13);  {Escape back into command mode and hang up}
     delay(2000); {Change this delay to make the line ring longer/shorter}
     sendstring('ATH1'#13);     {Go off-hook}
     delay(1000); {Increase this delay if the line doesn't stop ringin'}
     sendstring('ATH0'#13);     {Hang up again}
end.
