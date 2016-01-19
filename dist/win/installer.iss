[Setup]
AppId=3E41E91E-2E8A-4634-BCC8-C987643A1FC1
AppName=RecMind
AppVersion=1.2.0
AppCopyright=Copyright(c)2013-2014 JETCODES. All Rights Reserved.
AppContact=support@jetcodes.io
AppSupportURL=http://jetcodes.io/support
AppPublisherURL=http://jetcodes.io
AppPublisher=RecMind
DefaultDirName={pf}\RecMind
DefaultGroupName=RecMind 1.2.0
UninstallDisplayIcon={app}\recmind.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.\
SetupIconFile=logo.ico

[Dirs]
Name:"{app}\templates"
Name:"{tmp}\tmp"; Flags: deleteafterinstall

[Files]
Source: "recmind.exe"; DestDir:"{app}"
Source: "vcredist_x86.exe"; DestDir:"{tmp}\tmp"; Flags: onlyifdoesntexist deleteafterinstall
Source: "logo.ico"; DestDir:"{app}"
Source: "file.ico"; DestDir:"{app}"
Source: "*.dll"; DestDir:"{app}"

[Tasks]
Name: common; Description: "&All users"; GroupDescription: "Install For:"; Flags: exclusive unchecked
Name: local;  Description: "Just &me"; GroupDescription: "Install For:"; Flags: exclusive

[Registry]
Root: HKCR; Subkey: "RecMind File"; ValueType: String; ValueData: "RecMind File";Flags: uninsdeletekey
Root: HKCR; Subkey: "RecMind File\DefaultIcon"; ValueType: String; ValueData: "{app}\recmind.exe,1";Flags: uninsdeletekey
Root: HKCR; Subkey: "RecMind File\shell\open\command"; ValueType: String; ValueData: "{app}\recmind.exe %1";Flags: uninsdeletekey
Root: HKCR; Subkey: "RecMind File\shell\Open with RecMind(&O)\command"; ValueType: String; ValueData: "{app}\recmind.exe %1";Flags: uninsdeletekey
Root: HKCR; Subkey: ".rdx"; ValueType: String; ValueData: "RecMind File";Flags: uninsdeletekey

[Icons]
Name: "{group}\RecMind"; Filename: "{app}\recmind.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
Name: "{commondesktop}\RecMind"; Filename: "{app}\recmind.exe"

[Run]
Filename: "{tmp}\tmp\vcredist_x86.exe"; Parameters: /q; WorkingDir: "{tmp}\tmp"; Flags: skipifdoesntexist; StatusMsg: "Installing Microsoft Visual C++ Runtime ..."; Check: NeedInstallVC9SP1
Filename: "{app}\recmind.exe"; Description: "Run RecMind right now!"; Flags: nowait postinstall skipifsilent

[InstallDelete]
Type:files; Name:"{app}\vcredist_x86.exe"

[Code]
var
 vc9SP1Missing: Boolean;

function GetPathInstalled( AppID: String ): String;
var
   sPrevPath: String;
begin
  sPrevPath := '';
  if not RegQueryStringValue(HKLM,'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1','UninstallString', sPrevpath)
  then
    RegQueryStringValue(HKCU,'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1', 'UninstallString', sPrevpath);
  Result := sPrevPath;
end;

function NeedInstallVC9SP1(): Boolean;
begin
 Result := vc9SP1Missing;
end;

function InitializeSetup(): Boolean;
var
 sPrevPath: String;
 sPrevID: String;
 version: Cardinal;
 appHandle: HWND;
 resultCode: Integer;
 appIsRunning: Boolean;
begin
 sPrevID := '3E41E91E-2E8A-4634-BCC8-C987643A1FC1';
 appHandle := FindWindowByClassName('QWidget');
 if appHandle <> 0
 then
  appHandle := FindWindowByWindowName('RecMind');

 if appHandle<>0
 then
   begin
      if MsgBox('RecMind is running now, you have to close it before installation. Click "yes" to continue, or click "no" to cancel.', mbConfirmation, MB_YESNO) = idYES
      then
       begin
        Result := true;
        appIsRunning := false;
       end
      else
       begin
        Result := false;
        appIsRunning := true;
       end
   end
 else
  begin
    Result := true;
    appIsRunning := false
  end;

 sPrevPath := GetPathInstalled( sprevID );
 if (Length(sPrevPath) > 0 ) 
 then
   begin
     if MsgBox( 'A previous version of RecMind is already installed. It is recommended that you uninstall the existing version before running this setup. Click OK to continue, or Cancel to abort.', mbInformation, MB_OKCANCEL) = idOK
     then
      begin
       exec(sPrevPath, '', '', SW_SHOW, ewWaitUntilTerminated, resultCode);
       Result := true;
      end
     else
       Result := false;

     if ( Length (sPrevPath) = 0 ) 
     then
      Result := true;
   end;

 if not RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9A25302D-30C0-39D9-BD6F-21E6EC160475}', 'Version', version)
 then
   begin
     if not RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{FF66E9F6-83E7-3A3E-AF14-8DE9A809A6A4}', 'Version', version)
     then
       begin
         if not RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{86CE1746-9EFF-3C9C-8755-81EA8903AC34}', 'Version', version)
         then
           begin
             vc9SP1Missing := true;
           end
         else
           vc9SP1Missing := false; 
       end
     else
      vc9SP1Missing := false; 
   end
 else
   vc9SP1Missing := false;
end;

function InitializeUninstall(): Boolean;
var
  appHandle: HWND;
begin
  appHandle := FindWindowByClassName('QWidget');
  if appHandle <> 0
  then
   appHandle := FindWindowByWindowName('RecMind');

  if appHandle<>0
  then
    begin
      MsgBox('RecMind is running, you have to close it before you uninstall', mbError, MB_OK);
      Result := false;
    end
   else
    Result := true;
end;

