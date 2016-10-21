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
  Icon "bitmail.256.ico"

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
  !insertmacro MUI_PAGE_LICENSE "License.txt"
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

;--------------------------------
;Installer Sections

Section "BitMail" SecBitMail

  SetOutPath "$INSTDIR"
	File "License.txt"
	File "bitmail.256.ico"
	File "release\bitmail.exe"
	
  SetOutPath "$INSTDIR"
	File "qtdist\LICENSE"
	File "qtdist\LICENSE.FDL"
	File "qtdist\Qt5Core.dll"
	File "qtdist\Qt5Gui.dll"
	File "qtdist\Qt5Widgets.dll"
	File "qtdist\Qt5Network.dll"
	
  SetOutPath "$INSTDIR"	
	File "qtdist\icudt54.dll"
	File "qtdist\icuin54.dll"
	File "qtdist\icuuc54.dll"
	
  SetOutPath "$INSTDIR"
	File "qtdist\libgcc_s_dw2-1.dll"
	File "qtdist\libstdc++-6.dll"
	File "qtdist\libwinpthread-1.dll"  

  SetOutPath "$INSTDIR\locale"
  File "locale\*.qm"
  
  SetOutPath "$INSTDIR\platforms"
  File "platforms\*.dll"
  
  SetOutPath "$INSTDIR\imageformats"
  File "imageformats\*.dll"
    
  ;Store installation folder
  WriteRegStr HKCU "Software\BitMail" "Location" "$INSTDIR"
  
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

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "DisplayName" \
				 "BitMail"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "Publisher" \
				 "BitMailPublisher"
				 
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "DisplayVersion" \
				 "1.0.0.0"	

  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "VersionMajor" \
				 "1"	
				 
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "VersionMinor" \
				 "1"					 
				 				 
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "EstimatedSize" \
				 "52000"					 
				 
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
                 "UninstallString" \
				 "$\"$INSTDIR\Uninstall.exe$\""				  
	
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail" \
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

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BitMail"
  
SectionEnd

Function StartupBitMail
  ExecShell "open" "$SMPROGRAMS\BitMail\BitMail.lnk"
FunctionEnd


