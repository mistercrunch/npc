{ phone2.pas - Wyzewun's simpler version of Sigma's phone.pas }

uses crt;
var comport : text; { Yes, open it as a textfile, Unix style }

begin
     Assign(comport, 'COM1'); { Change to COM Port desired }
     Writeln(comport, 'ATDT*1#');
     delay(2500); {Increase this delay if the line doesn't ring}
     Writeln(comport, 'AT+++H0');
     delay(3000); {Change this delay to make the line ring longer/shorter}
     Writeln(comport, 'ATH1');
     delay(1000); {Increase this delay if the line doesn't stop ringin'}
     Writeln(comport, 'ATH0');
end.
