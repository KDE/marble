; basic script template for NSIS installers
;
; Written by Philip Chu
; Copyright (c) 2004-2005 Technicat, LLC
;
; This software is provided 'as-is', without any express or implied warranty.
; In no event will the authors be held liable for any damages arising from the use of this software.
 
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it ; and redistribute
; it freely, subject to the following restrictions:
 
;    1. The origin of this software must not be misrepresented; you must not claim that
;       you wrote the original software. If you use this software in a product, an
;       acknowledgment in the product documentation would be appreciated but is not required.
 
;    2. Altered source versions must be plainly marked as such, and must not be
;       misrepresented as being the original software.
 
;    3. This notice may not be removed or altered from any source distribution.

!include "FileAssociation.nsh"

!define architecture "x86" ; 32 or 64 bit build? x86 or x64

!define version "1.9.1" ; current Marble version
!define VCREDIST_DIR "C:\marble" ; where do visual studio redistributable packages lie?

!define setup "marble-setup_${version}_${architecture}.exe"
!define VCREDIST_FILE "vcredist_${architecture}.exe"

!define company "KDE"
!define prodname "Marble"
!define exec "marble-qt.exe"

; x64
;  !define qtdir "C:\Qt\5.3\5.3\msvc2013_64_opengl"
;  !define srcdir "C:\marble\export64" ; where did you install Marble (CMAKE_INSTALL_PREFIX)?
;  InstallDir "$PROGRAMFILES64\${prodname}"
; x86
  !define qtdir "C:\Qt\5.3\5.3\msvc2013_opengl"
  !define srcdir "C:\marble\export32" ; where did you install Marble (CMAKE_INSTALL_PREFIX)?
  InstallDir "$PROGRAMFILES\${prodname}"
  
; optional stuff
  
; license text file
!define licensefile lgpl2.txt
 
; icons must be Microsoft .ICO files
; !define icon "icon.ico"
 
; installer background screen
; !define screenimage background.bmp
  
; registry stuff
 
!define regkey "Software\${company}\${prodname}"
!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"
 
!define startmenu "$SMPROGRAMS\${prodname}"
!define uninstaller "uninstall.exe"
 
;--------------------------------
   
;XPStyle on
ShowInstDetails hide
ShowUninstDetails hide
 
Name "${prodname}"
Caption "${prodname}"
 
!ifdef icon
Icon "${icon}"
!endif
 
OutFile "${setup}"
 
SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
 
InstallDirRegKey HKLM "${regkey}" ""
 
!ifdef licensefile
LicenseText "License"
LicenseData "${srcdir}\data\licenses\${licensefile}"
!endif
 
; pages
; we keep it simple - leave out selectable installation types
 
!ifdef licensefile
Page license
!endif
 
; Page components
Page directory
Page instfiles
 
UninstPage uninstConfirm
UninstPage instfiles
 
;--------------------------------
 
AutoCloseWindow false
;ShowInstDetails show
 
!ifdef screenimage
 
; set up background image
; uses BgImage plugin
 
Function .onGUIInit
	; extract background BMP into temp plugin directory
	InitPluginsDir
	File /oname=$PLUGINSDIR\1.bmp "${screenimage}"
 
	BgImage::SetBg /NOUNLOAD /FILLSCREEN $PLUGINSDIR\1.bmp
	BgImage::Redraw /NOUNLOAD
FunctionEnd
 
Function .onGUIEnd
	; Destroy must not have /NOUNLOAD so NSIS will be able to unload and delete BgImage before it exits
	BgImage::Destroy
FunctionEnd
 
!endif
 
; beginning (invisible) section
Section
 
  WriteRegStr HKLM "${regkey}" "Install_Dir" "$INSTDIR"
  ; write uninstall strings
  WriteRegStr HKLM "${uninstkey}" "DisplayName" "${prodname} (remove only)"
  WriteRegStr HKLM "${uninstkey}" "UninstallString" '"$INSTDIR\${uninstaller}"'
 
!ifdef filetype
  WriteRegStr HKCR "${filetype}" "" "${prodname}"
!endif
 
  WriteRegStr HKCR "${prodname}\Shell\open\command\" "" '"$INSTDIR\${exec} "%1"'
  ${registerExtension} "$INSTDIR\${exec}" ".kml" "Keyhole Markup Language (KML)"
  ${registerExtension} "$INSTDIR\${exec}" ".gpx" "GPS Exchange Format (GPX)"
 
!ifdef icon
  WriteRegStr HKCR "${prodname}\DefaultIcon" "" "$INSTDIR\${icon}"
!endif
 
  SetOutPath $INSTDIR

ReadRegDword $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{A749D8E6-B613-3BE3-8F5F-045C84EBA29B}" "Version"
IntCmp $0 0 0 +3
  File "${VCREDIST_DIR}\${VCREDIST_FILE}" 	
  ExecWait '"$INSTDIR\${VCREDIST_FILE}" /q /norestart'	
 
; package all files, recursively, preserving attributes
; assume files are in the correct places

File /a /r \
  /x "*.nsi" \
  /x "${setup}" \
  /x "marble-touch.exe" \
  /x "marble-mobile.exe" \
  /x "RoutinoPlugin.dll" \
  "${srcdir}\*.*"    
  
File /a /r \
  "${qtdir}\bin\ICUDT52.DLL" \
  "${qtdir}\bin\ICUIN52.DLL" \
  "${qtdir}\bin\ICUUC52.DLL" \
  "${qtdir}\bin\QT5CORE.DLL" \
  "${qtdir}\bin\QT5GUI.DLL" \
  "${qtdir}\bin\QT5MULTIMEDIA.DLL" \
  "${qtdir}\bin\QT5MULTIMEDIAWIDGETS.DLL" \
  "${qtdir}\bin\QT5NETWORK.DLL" \
  "${qtdir}\bin\QT5OPENGL.DLL" \
  "${qtdir}\bin\QT5POSITIONING.DLL" \
  "${qtdir}\bin\QT5PRINTSUPPORT.DLL" \
  "${qtdir}\bin\QT5QML.DLL" \
  "${qtdir}\bin\QT5QUICK.DLL" \
  "${qtdir}\bin\QT5SCRIPT.DLL" \
  "${qtdir}\bin\QT5SENSORS.DLL" \
  "${qtdir}\bin\QT5SQL.DLL" \
  "${qtdir}\bin\QT5SVG.DLL" \
  "${qtdir}\bin\QT5WEBKIT.DLL" \
  "${qtdir}\bin\QT5WEBKITWIDGETS.DLL" \
  "${qtdir}\bin\QT5WIDGETS.DLL" \
  "${qtdir}\bin\QT5XML.DLL"


SetOutPath $INSTDIR\platforms
File /a "${qtdir}\plugins\platforms\qminimal.dll" \
        "${qtdir}\plugins\platforms\qwindows.dll"
SetOutPath $INSTDIR\imageformats
File /a "${qtdir}\plugins\imageformats\qjpeg.dll" \
        "${qtdir}\plugins\imageformats\qsvg.dll" \
        "${qtdir}\plugins\imageformats\qtiff.dll" \
        "${qtdir}\plugins\imageformats\qgif.dll"
SetOutPath $INSTDIR\bearer
File /a "${qtdir}\plugins\bearer\qgenericbearer.dll" \
        "${qtdir}\plugins\bearer\qnativewifibearer.dll"
SetOutPath $INSTDIR\printsupport
File /a "${qtdir}\plugins\printsupport\windowsprintersupport.dll"
SetOutPath $INSTDIR\sqldrivers
File /a "${qtdir}\plugins\sqldrivers\qsqlite.dll"
SetOutPath $INSTDIR
  
!ifdef licensefile
File /a "${srcdir}\data\licenses\${licensefile}"
!endif
 
!ifdef notefile
File /a "${srcdir}\${notefile}"
!endif
 
!ifdef icon
File /a "${srcdir}\${icon}"
!endif
 
; any application-specific files
!ifdef files
include "${files}"
!endif
 
  WriteUninstaller "${uninstaller}"
  
SectionEnd
 
; create shortcuts
Section
  
  CreateDirectory "${startmenu}"
  SetOutPath $INSTDIR ; for working directory
  CreateShortCut "${startmenu}\${prodname}.lnk" "$INSTDIR\${exec}"
  CreateShortCut "${startmenu}\Uninstall.lnk" $INSTDIR\uninstall.exe"

!ifdef licensefile
  CreateShortCut "${startmenu}\LICENSE.lnk "$INSTDIR\${licensefile}"
!endif
 
!ifdef notefile
  CreateShortCut "${startmenu}\Release Notes.lnk "$INSTDIR\${notefile}"
!endif
 
!ifdef helpfile
  CreateShortCut "${startmenu}\Documentation.lnk "$INSTDIR\${helpfile}"
!endif
 
!ifdef website
WriteINIStr "${startmenu}\web site.url" "InternetShortcut" "URL" ${website}
 ; CreateShortCut "${startmenu}\Web Site.lnk "${website}" "URL"
!endif
 
!ifdef notefile
ExecShell "open" "$INSTDIR\${notefile}"
!endif
 
SectionEnd
 
; Uninstaller
; All section names prefixed by "Un" will be in the uninstaller
 
UninstallText "This will uninstall ${prodname}."
 
!ifdef icon
UninstallIcon "${icon}"
!endif

Section "Uninstall"
 
DeleteRegKey HKLM "${uninstkey}"
DeleteRegKey HKLM "${regkey}"
${unregisterExtension} ".kml" "Keyhole Markup Language (KML)"
${unregisterExtension} ".gpx" "GPS Exchange Format (GPX)"

RMDir /r "${startmenu}"
RMDir /r "$INSTDIR"

; generated on runtime
DeleteRegKey HKCU "Software\${company}\Marble Desktop Globe"
RMDir /r "$%USERPROFILE%\.marble"

SectionEnd
