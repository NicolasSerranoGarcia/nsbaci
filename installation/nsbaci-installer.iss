; -- nsbaci-installer.iss --

#define AppName "nsbaci"
#define RootDir "..\"
#define BuildDir "..\build"
#define InstallerIconFile "..\assets\nsbaci-installer.ico"
#define WizardSmallIconFile "..\assets\nsbaci-installer-128x128-no-transparent.bmp"
; expected to have run "windeployqt nsbaci.exe --release --dir ../build_inno"
#define BuildInno "..\build_inno"
#define ReadmeFile "..\README.md"
#define LicenseFile "..\LICENSE"
#define AppURL "https://nsbaci-docs.nicolasserranogarcia.com"
#define WorkspacesDir "{userappdata}\nsbaci"
#define FileDesc = FileOpen("..\VERSION")
#define VersionNumber = FileRead(FileDesc)
#expr FileClose(FileDesc)
#define AppExeName "nsbaci.exe"
#define AppCopyright "© 2025 Nicolas Serrano Garcia"
#define AppDescription "nsbaci workspace manager"
#define AppCompany "nsbaci Project"
#define AppPath "..\build\nsbaci.exe"

[Setup]
AppId={{A3D21D63-7C43-4F5D-8AF5-4A31BBF708CF}}
AppName={#AppName}
AppVersion={#VersionNumber}
AppVerName={#AppName} {#VersionNumber}
VersionInfoVersion={#VersionNumber}
VersionInfoCopyright={#AppCopyright}
VersionInfoCompany={#AppCompany}
VersionInfoDescription={#AppDescription}
AppPublisherURL={#AppURL}
WizardStyle=modern
WizardSmallImageFile={#WizardSmallIconFile}
; WizardImageAlphaFormat=premultiplied
WizardSmallImageBackColor=clWhite
;Program Files(x86)\nsbaci or Program Files\nsbaci, depending on the user arch
DefaultDirName={autopf}\{#AppName}
;In windows 10, the search bar included all files from A-Z. This is the name that appears as a group there
DefaultGroupName={#AppName}
;This is the name of the .exe installer
OutputBaseFilename={#AppName}-installer-{#VersionNumber}
;Where the Innosetup generated files will go
OutputDir={#BuildInno}\Output   
LicenseFile={#LicenseFile}
Compression=lzma
SolidCompression=yes
SetupIconFile={#InstallerIconFile}
ShowLanguageDialog=no
UninstallDisplayName={#AppName}
AppModifyPath="{uninstallexe}"

[FILES]
; BuildInno contains the exe, dll and plugins needed
Source: "{#BuildInno}\*"; DestDir: "{app}"; Flags: recursesubdirs; Excludes: "*.pdb;*.log;CMakeFiles\*"
Source: "{#ReadmeFile}"; DestDir: "{app}"; Flags: isreadme
Source: "{#AppPath}"; DestDir: "{app}";

[TASKS]
Name: startmenu; Description: "Add to Start Menu"; GroupDescription: "Integration"
Name: desktopicon; Description: "Create Desktop Shortcut"; GroupDescription: "Integration"

[Dirs]
Name: {#WorkspacesDir}

[ICONS]
Name: "{group}\nsbaci"; FileName: "{app}\{#AppExeName}"; Tasks: startmenu
Name: "{group}\{cm:ProgramOnTheWeb,nsbaci}"; FileName: "{#AppURL}"; Tasks: startmenu
Name: "{group}\{cm:UninstallProgram,nsbaci}"; FileName: "{uninstallexe}"; Tasks:startmenu
Name: "{commondesktop}\nsbaci"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[RUN]
Filename: "{app}\{#AppExeName}"; WorkingDir: "{#WorkspacesDir}"; Description: "{cm:LaunchProgram,{#StringChange(AppName, '&', '&&')}}"; Flags: postinstall
Filename: "{#AppURL}"; Description: "Visit nsbaci website"; Flags: postinstall shellexec skipifsilent unchecked

[UninstallDelete]
Type: dirifempty; Name: "{#WorkspacesDir}"

; [Registry]
; Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\nsbaci.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\nsbaci.exe"
; Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\nsbaci.exe"; ValueType: string; ValueName: "Path"; ValueData: "{app}"

; Root: HKCR; Subkey: "*\shell\Open with nsbaci"; \
;     ValueType: string; ValueName: ""; ValueData: "Open with nsbaci"
; Root: HKCR; Subkey: "*\shell\Open with nsbaci\command"; \
;     ValueType: string; ValueName: ""; ValueData: """{app}\nsbaci.exe"" ""%1"""