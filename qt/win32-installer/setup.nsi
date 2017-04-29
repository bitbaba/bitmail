;NSIS Modern User Interface
;Header Bitmap Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "BitMail-Qt Win32 Installer"
  OutFile "bitmail-qt-win32-installer.exe"
  Icon "${BitMailQtRoot}\res\image\bitmail.256.ico"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\BitMail"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\BitMail" "InstallLocation"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user
  
;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "win.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  ; For Installation
  !insertmacro MUI_PAGE_LICENSE "${BitMailQtRoot}\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  ; For Finish page
  !define MUI_FINISHPAGE_AUTOCLOSE
  !define MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_CHECKED
  !define MUI_FINISHPAGE_RUN_TEXT "Startup BitMail"
  !define MUI_FINISHPAGE_RUN_FUNCTION "StartupBitMail"
  !insertmacro MUI_PAGE_FINISH
  
  ; For Un-installation
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"
  ;!insertmacro MUI_LANGUAGE "SimpChinese"

;--------------------------------
;Installer Sections

Section "BitMail" SecBitMail

  SetOutPath "$INSTDIR"
	File "${BitMailQtRoot}\License.txt"
	File "${BitMailQtRoot}\res\image\bitmail.256.ico"
	File "${BitMailQtReleaseRoot}\bitmail.exe"
	File "${BitMailQtReleaseRoot}\audiorecorder.exe"
	File "${BitMailQtReleaseRoot}\camera.exe"
	File "${BitMailQtReleaseRoot}\screenshot.exe"
	
  SetOutPath "$INSTDIR"
	File "${QtLicenseRoot}\LICENSE"
	File "${QtLicenseRoot}\LICENSE.FDL"
	File "${QtDistRoot}\Qt5Core.dll"
	File "${QtDistRoot}\Qt5Gui.dll"
	File "${QtDistRoot}\Qt5Widgets.dll"
	File "${QtDistRoot}\Qt5Network.dll"
	File "${QtDistRoot}\Qt5Multimedia.dll"
	File "${QtDistRoot}\Qt5MultimediaWidgets.dll"
	File "${QtDistRoot}\Qt5OpenGL.dll"	
	File "${QtDistRoot}\icudt53.dll"
	File "${QtDistRoot}\icuin53.dll"
	File "${QtDistRoot}\icuuc53.dll"
	
  SetOutPath "$INSTDIR"
	File "${QtToolsRoot}\libgcc_s_dw2-1.dll"
	File "${QtToolsRoot}\libstdc++-6.dll"
	File "${QtToolsRoot}\libwinpthread-1.dll"
	
  SetOutPath "$INSTDIR\locale"
	File "${BitMailQtRoot}\locale\*.qm"	
	
  SetOutPath "$INSTDIR\res"
	File /r "${BitMailQtRoot}\res\*.*"
	
  SetOutPath "$INSTDIR\imageformats"
	File "${QtPluginsRoot}\imageformats\qdds.dll"
	File "${QtPluginsRoot}\imageformats\qgif.dll"
	File "${QtPluginsRoot}\imageformats\qicns.dll"
	File "${QtPluginsRoot}\imageformats\qico.dll"
	File "${QtPluginsRoot}\imageformats\qjpeg.dll"
	File "${QtPluginsRoot}\imageformats\qsvg.dll"
	File "${QtPluginsRoot}\imageformats\qtga.dll"
	File "${QtPluginsRoot}\imageformats\qtiff.dll"
	File "${QtPluginsRoot}\imageformats\qwbmp.dll"
	File "${QtPluginsRoot}\imageformats\qwebp.dll"
  
  SetOutPath "$INSTDIR\platforms"
	File "${QtPluginsRoot}\platforms\qminimal.dll"   
	File "${QtPluginsRoot}\platforms\qoffscreen.dll"   
	File "${QtPluginsRoot}\platforms\qwindows.dll"   

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ;Create a shortcut
  SetOutPath "$INSTDIR"
  CreateDirectory "$SMPROGRAMS\BitMail"
  CreateShortCut  "$SMPROGRAMS\BitMail\BitMail.lnk" \
				  "$INSTDIR\bitmail.exe" \
				  '' \
				  "$INSTDIR\bitmail.256.ico" 0 \
				  SW_SHOWNORMAL \
				  CONTROL|SHIFT|B \
				  "Startup BitMail"
				  
  CreateShortCut  "$SMPROGRAMS\BitMail\Uninstall.lnk" \
				  "$INSTDIR\Uninstall.exe" \
				  '' \
				  "$INSTDIR\Uninstall.exe" 0 
				  
  ;Store installation folder
  WriteRegStr HKCU "Software\BitMail" "Location" "$INSTDIR"
  
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "DisplayName" \
				 "BitMail"

  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "Publisher" \
				 "BitMailPublisher"
				 
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "DisplayVersion" \
				 "1.0.0.0"	

  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "VersionMajor" \
				 "1"	
				 
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "VersionMinor" \
				 "1"					 
				 				 
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "EstimatedSize" \
				 "52000"					 
				 
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "UninstallString" \
				 "$\"$INSTDIR\Uninstall.exe$\""				  
	
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "QuietUninstallString" \
				 "$\"$INSTDIR\uninstall.exe$\" /S"
SectionEnd

;--------------------------------
;Descriptions
  ;Language strings
  LangString DESC_BitMail ${LANG_ENGLISH} "main module for BitMail"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecBitMail} $(DESC_BitMail)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section
Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  
  RMDir /r "$SMPROGRAMS\BitMail"
  
  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\BitMail"

  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail"
  
SectionEnd

Function StartupBitMail
  ExecShell "open" "$SMPROGRAMS\BitMail\BitMail.lnk"
FunctionEnd


