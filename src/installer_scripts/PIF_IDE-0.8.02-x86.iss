[Setup]
AppId={{B5A4E130-3DE8-4D90-86AA-9BBE5BD37C59}
AppName=PIF IDE
AppVersion=0.8.02
AppPublisher=PIF Project
AppContact=https://github.com/mauromascarenhas/PIF-IDE/issues
AppPublisherURL=https://github.com/mauromascarenhas/PIF-IDE/
AppSupportURL=https://github.com/mauromascarenhas/PIF-IDE/issues
AppUpdatesURL=https://github.com/mauromascarenhas/PIF-IDE/
AppCopyright=Copyright (C) 2019 PIF Project.
DefaultDirName={pf}\PIF Project\PIF IDE
DisableDirPage=yes
DefaultGroupName=PIF
AllowNoIcons=yes
OutputDir=C:\Users\Mauro\Desktop\PIF_IDE
OutputBaseFilename=pif_ide-0.8.02_x86
SetupIconFile=C:\Users\Mauro\Documents\GitHub\PIF_IDE\src\pif_ide\images\logo\pif_logo_x64.ico
LicenseFile=C:\Users\Mauro\Documents\Nintersoft\Licença de Código Aberto Nintersoft rev1-inst.rtf
UsePreviousAppDir=yes
UsePreviousGroup=yes
UsePreviousLanguage=yes
UsePreviousSetupType=yes
UsePreviousTasks=yes
UsePreviousUserInfo=yes
BackColor=$090067
BackSolid=yes
ChangesAssociations=yes
UninstallDisplayName=PIF IDE
UninstallDisplayIcon={app}\pif_ide.exe
Compression=lzma2/max
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

[Types]
Name: "full"; Description: "{cm:FullInstall}"
Name: "custom"; Description: "{cm:CustomInstall}"; Flags: iscustom

[Components]
Name: "program"; Description: "{cm:ProgramFiles}"; Types: full custom; Flags: fixed
Name: "sources"; Description: "{cm:SourceFiles}"; Types: full;

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; OnlyBelowVersion: 0,6.1

[Files]
Source: "C:\Users\Mauro\Documents\Completo\Extra\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; AfterInstall: InstMSVSC
Source: "C:\Users\Mauro\Documents\Completo\pif_ide\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program
Source: "C:\Users\Mauro\Documents\GitHub\PIF_IDE\*"; DestDir: "{app}\sources"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: sources

[Code]
procedure ExitProcess(exitCode:integer);
  external 'ExitProcess@kernel32.dll stdcall';

procedure InstMSVSC;
var
  ResultCode: Integer;       
begin
  if not FileExists(ExpandConstant('{sys}\MSVCP140.DLL'))
  then
    if not Exec(ExpandConstant('{tmp}\vc_redist.x86.exe'), '', '', SW_SHOWNORMAL,
      ewWaitUntilTerminated, ResultCode)
    then
      ExitProcess(1);
  if not FileExists(ExpandConstant('{sys}\MSVCP140.DLL'))
  then
  ExitProcess(1);
end;

[Icons]
Name: "{group}\PIF IDE"; Filename: "{app}\pif_ide.exe"
Name: "{group}\Licença de código aberto"; Filename: "{app}\Licença de Código Aberto Nintersoft rev1.pdf"
Name: "{group}\{cm:ProgramOnTheWeb,PIF IDE}"; Filename: "https://github.com/mauromascarenhas/PIF-IDE/"
Name: "{group}\{cm:UninstallProgram,PIF IDE}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\PIF IDE"; Filename: "{app}\pif_ide.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\PIF IDE"; Filename: "{app}\pif_ide.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\pif_ide.exe"; Description: "{cm:LaunchProgram,PIF IDE}"; Flags: nowait postinstall skipifsilent

[CustomMessages]
CustomInstall=Custom installation
FullInstall=Full installation
ProgramFiles=Program Files
SourceFiles=Source Files

brazilianportuguese.CustomInstall=Instalação personalizada
brazilianportuguese.FullInstall=Instalação completa
brazilianportuguese.ProgramFiles=Arquivos de programa
brazilianportuguese.SourceFiles=Código fonte