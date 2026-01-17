; Script generado para Inno Setup
; AJUSTA LAS RUTAS ABSOLUTAS SI ES NECESARIO

#define MyAppName "MadNotesCpp"
#define MyAppVersion "1.0"
#define MyAppPublisher "Martin Dumont"
#define MyAppURL "https://github.com/martdumo/madnotescpp"
#define MyAppExeName "MadNotesCpp.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
AppId={{A1B2C3D4-E5F6-7890-1234-56789ABCDEF0}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
; Icono del instalador
SetupIconFile=resources\app.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
OutputDir=Releases
OutputBaseFilename=MadNotes_Setup_v1.0

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; IMPORTANTE: Apunta a tu carpeta Portable generada
Source: "MadNotes_Portable\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Registry]
; Asociar archivos .html (Opcional, cuidado con robarle a Chrome)
; Root: HKCR; Subkey: ".html"; ValueType: string; ValueName: ""; ValueData: "MadNotesFile"; Flags: uninsdeletevalue
; Root: HKCR; Subkey: "MadNotesFile"; ValueType: string; ValueName: ""; ValueData: "MadNotes Document"; Flags: uninsdeletekey
; Root: HKCR; Subkey: "MadNotesFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"
; Root: HKCR; Subkey: "MadNotesFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""