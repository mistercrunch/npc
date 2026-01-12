
{run this program at your own risk. :)}

{$M $500,000,000 }

program tsrattempt;

uses crt,dos;

var clockintvec : procedure;

{$F+}
procedure checkclock; interrupt;
var l,c : byte;
    w : word;
begin
  for l := 1 to 80 do
    for c := 1 to 25 do
      begin
        w := ((c-1) shl 7) + ((c-1) shl 5)+((l-1) shl 1);
        case chr(mem[$b800:w]) of
          'o','O' : mem[$b800:w] := 48;
          'I' : mem[$b800:w] := 105;
          'e','E' : mem[$b800:w] := 51;
          'l','L' : mem[$b800:w] := 124;
          't','T' : if(random(2)+1=1) then mem[$b800:w] := 43 else mem[$b800:w] := 55;
          's','S' : mem[$b800:w] := 36;
          'a','A' : mem[$b800:w] := 52;
          'U' : mem[$b800:w] := 117;
        end;
      end;
  clockintvec;
end;

{$F-}
begin
  randomize;
  writeln('ELEET.EXE was made by TayX');
  writeln('(c) 1995 SSE');
  writeln;
  writeln('Press a key for the fun to begin!');
  readkey;
  getintvec($1C,@clockintvec);
  setintvec($1C,addr(checkclock));
  keep(0);
end.
