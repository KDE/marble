#define MyAppVersion "2.2.0"

#define MySrcDir SourcePath + "\..\..\"
#define MyAppName "Marble"
#define MyAppPublisher "KDE"
#define MyAppURL "https://marble.kde.org/"
#define MyAppExeName "marble-qt.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{E4E20D3E-4180-4747-A111-9A835A6FB1F0}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile={#MySrcDir}\LICENSE.txt
OutputBaseFilename={#MyAppName}-setup_{#MyAppVersion}_{#MyArch}
SetupIconFile={#MySrcDir}\data\ico\marble.ico
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#MySrcDir}\marble.kde.org.url"; DestDir: "{app}"
Source: "{#MyBuildDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\{#MyAppName}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{commonprograms}\{#MyAppName}\Marble Website"; Filename: "{app}\marble.kde.org.url"

[Tasks]
Name: kmlAssociation; Description: "Associate ""kml"" extension (KML files from e.g. Google Earth)"; GroupDescription: File extensions:
Name: kmzAssociation; Description: "Associate ""kmz"" extension (Compressed KML files from e.g. Google Earth)"; GroupDescription: File extensions:
Name: gpxAssociation; Description: "Associate ""gpx"" extension (GPS Exchange Format)"; GroupDescription: File extensions:
Name: jsonAssociation; Description: "Associate ""json"" extension (GeoJSON files)"; Flags: unchecked; GroupDescription: File extensions:
Name: geojsonAssociation; Description: "Associate ""geojson"" extension (GeoJSON files)"; GroupDescription: File extensions:

[Registry]

Root: HKCR; Subkey: ".kml";                             ValueData: "{#MyAppName}";             Flags: uninsdeletevalue; ValueType: string;  ValueName: ""; Tasks: kmlAssociation 
Root: HKCR; Subkey: ".kmz";                             ValueData: "{#MyAppName}";             Flags: uninsdeletevalue; ValueType: string;  ValueName: ""; Tasks: kmzAssociation  
Root: HKCR; Subkey: ".gpx";                             ValueData: "{#MyAppName}";             Flags: uninsdeletevalue; ValueType: string;  ValueName: ""; Tasks: gpxAssociation 
Root: HKCR; Subkey: ".json";                            ValueData: "{#MyAppName}";             Flags: uninsdeletevalue; ValueType: string;  ValueName: ""; Tasks: jsonAssociation 
Root: HKCR; Subkey: ".geojson";                         ValueData: "{#MyAppName}";             Flags: uninsdeletevalue; ValueType: string;  ValueName: ""; Tasks: geojsonAssociation 

Root: HKCR; Subkey: "{#MyAppName}";                     ValueData: "Program {#MyAppName}";     Flags: uninsdeletekey;   ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}\DefaultIcon";         ValueData: "{app}\{#MyAppExeName},0";                           ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}\shell\open\command";  ValueData: """{app}\{#MyAppExeName}"" ""%1""";                  ValueType: string;  ValueName: ""

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
