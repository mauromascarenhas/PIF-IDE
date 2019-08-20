[Setup]
AppId={{03219D60-AD8F-4F8F-8AAC-A4BAA3DBC54C}
AppName=PIF IDE
AppVersion=0.8.02
AppPublisher=PIF Project
AppContact=https://github.com/mauromascarenhas/PIF-IDE/issues
AppPublisherURL=https://github.com/mauromascarenhas/PIF-IDE/
AppSupportURL=https://github.com/mauromascarenhas/PIF-IDE/issues
AppUpdatesURL=https://github.com/mauromascarenhas/PIF-IDE/
AppCopyright=Copyright (C) 2019 PIF Project.
DefaultDirName={pf}\Projeto PIF\PIF IDE\
DisableDirPage=yes
DefaultGroupName=PIF IDE
AllowNoIcons=yes
OutputDir=C:\Users\Mauro\Desktop\PIF_IDE
OutputBaseFilename=pif_ide-0.8.02_x64
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
ArchitecturesInstallIn64BitMode = x64 ia64

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
Source: "C:\Users\Mauro\Documents\Completo\Extra\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; AfterInstall: InstMSVSC
Source: "C:\Users\Mauro\Documents\Completo\pif_ide_x64\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program
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
    if not Exec(ExpandConstant('{tmp}\vc_redist.x64.exe'), '', '', SW_SHOWNORMAL,
      ewWaitUntilTerminated, ResultCode)
    then
      ExitProcess(1);
  if not FileExists(ExpandConstant('{sys}\MSVCP140.DLL'))
  then
  ExitProcess(1);
end;

[Icons]
Name: "{group}\PIF IDE"; Filename: "{app}\pif_ide.exe"
Name: "{group}\{cm:IDEProgramSources}"; Filename: "{app}\sources"
Name: "{group}\{cm:Licence}"; Filename: "{app}\licence\LGPL_V3.txt"
Name: "{group}\{cm:NSLicence}"; Filename: "{app}\licence\Licença de Código Aberto Nintersoft rev1.pdf"
Name: "{group}\{cm:ProgramOnTheWeb,PIF IDE}"; Filename: "https://github.com/mauromascarenhas/PIF-IDE/"
Name: "{group}\{cm:UninstallProgram,PIF IDE}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\PIF IDE"; Filename: "{app}\pif_ide.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\PIF IDE"; Filename: "{app}\pif_ide.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\pif_ide.exe"; Description: "{cm:LaunchProgram,PIF IDE}"; Flags: nowait postinstall skipifsilent

[CustomMessages]
CustomInstall=Custom installation
FullInstall=Full installation
Licence=LGPL_v3 Licence
NSLicence=Nintersoft Open Source Code Licence
ProgramFiles=Program Files
IDEProgramSources=PIF IDE Sources
SourceFiles=Source Files

brazilianportuguese.CustomInstall=Instalação personalizada
brazilianportuguese.FullInstall=Instalação completa
brazilianportuguese.Licence=Licença LGPL_v3
brazilianportuguese.NSLicence=Licença de Código aberto Nintersoft
brazilianportuguese.ProgramFiles=Arquivos de programa
brazilianportuguese.IDEProgramSources=Código fonte PIF IDE
brazilianportuguese.SourceFiles=Código fonte