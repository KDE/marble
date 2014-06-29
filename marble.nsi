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

!ifndef setup
!define setup "marble-setup.exe"
!endif
 
; change this to wherever the files to be packaged reside
!ifndef srcdir
!define srcdir "C:\Program Files\marble"
!endif
 
!define company "KDE"
 
!ifndef prodname
!define prodname "Marble"
!endif

!define exec "marble-qt.exe"
 
; optional stuff
 
; text file to open in notepad after installation
; !define notefile "README.txt"
 
; license text file
!define licensefile lgpl2.txt
 
; icons must be Microsoft .ICO files
; !define icon "icon.ico"
 
; installer background screen
; !define screenimage background.bmp
 
; file containing list of file-installation commands
;!define files "files.nsi"
 
; file containing list of file-uninstall commands
;!define unfiles "unfiles.nsi"
 
; registry stuff
 
!define regkey "Software\${company}\${prodname}"
!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"
 
!define startmenu "$SMPROGRAMS\${prodname}"
!define uninstaller "uninstall.exe"
 
;--------------------------------
 
XPStyle on
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
 
InstallDir "$PROGRAMFILES\${prodname}"
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
ShowInstDetails show
 
 
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
 
 
; package all files, recursively, preserving attributes
; assume files are in the correct places

File /a /r \
  /x "*.nsi" \
  /x "${setup}" \
  /x "marble-touch.exe" \
  /x "marble-mobile.exe" \
  /x "RoutinoPlugin.dll" \
  "${srcdir}\*.*"

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

