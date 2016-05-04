Name "BitMail-qt-win32"

SetCompressor /SOLID lzma

# General Symbol Definitions
!define VERSION 2015.9
!define COMPANY "Bitbaba Inc."
!define URL http://bitmail.bitbaba.com/
!define SRCTOP "D:\workspace\github\bitmail\share"

# MUI Symbol Definitions
!define MUI_ICON "${SRCTOP}\res\images\bitmail.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Bitmail-qt"
!define MUI_FINISHPAGE_RUN "$INSTDIR\bitmail-qt.exe"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile ${SRCTOP}/bitmail-${VERSION}-installer.exe

InstallDir $PROGRAMFILES\Bitmail

CRCCheck on
XPStyle on
BrandingText " "
ShowInstDetails show
ShowUninstDetails show

# Installer sections
Section -Main SEC0000
    SetOutPath $INSTDIR
    SetOverwrite on
    File /oname=bitmail-qt.exe ${SRCTOP}\release-bin\bitmail.exe
    File ${SRCTOP}\release-bin\bitmailclient.exe
    File ${SRCTOP}\release-bin\qrencode.exe
    File ${SRCTOP}\release-bin\icudt53.dll
    File ${SRCTOP}\release-bin\icuin53.dll
    File ${SRCTOP}\release-bin\icuuc53.dll
    File ${SRCTOP}\release-bin\libgcc_s_dw2-1.dll
    File ${SRCTOP}\release-bin\libstdc++-6.dll
    File ${SRCTOP}\release-bin\libwinpthread-1.dll
    File ${SRCTOP}\release-bin\Qt5Core.dll
    File ${SRCTOP}\release-bin\Qt5Gui.dll
    File ${SRCTOP}\release-bin\Qt5Widgets.dll
    
    SetOutPath $INSTDIR\res\head
    File ${SRCTOP}\res\head\alice.png
    File ${SRCTOP}\res\head\bob.png
    SetOutPath $INSTDIR\res\images
    File ${SRCTOP}\res\images\bitmail.png
    File ${SRCTOP}\res\images\bitmail.ico
SectionEnd

Section -post SEC0001
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        CreateDirectory $SMPROGRAMS\$StartMenuGroup
        CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^Name).lnk" $INSTDIR\bitmail-qt.exe
        CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

# Uninstaller sections
Section /o -un.Main UNSEC0000
    Delete /REBOOTOK $INSTDIR\bitmail-qt.exe
    Delete /REBOOTOK $INSTDIR\bitmailclient.exe
    Delete /REBOOTOK $INSTDIR\qrencode.exe
    Delete /REBOOTOK $INSTDIR\icudt53.dll
    Delete /REBOOTOK $INSTDIR\icuin53.dll
    Delete /REBOOTOK $INSTDIR\icuuc53.dll
    Delete /REBOOTOK $INSTDIR\libgcc_s_dw2-1.dll
    Delete /REBOOTOK $INSTDIR\libstdc++-6.dll
    Delete /REBOOTOK $INSTDIR\libwinpthread-1.dll
    Delete /REBOOTOK $INSTDIR\Qt5Core.dll
    Delete /REBOOTOK $INSTDIR\Qt5Gui.dll
    Delete /REBOOTOK $INSTDIR\Qt5Widgets.dll
    RMDir /r /REBOOTOK $INSTDIR\res
SectionEnd
