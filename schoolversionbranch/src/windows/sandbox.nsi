Name "sandbox"

OutFile "sandbox_2006_xx_xx_setup.exe"

InstallDir $PROGRAMFILES\sandbox

InstallDirRegKey HKLM "Software\sandbox" "Install_Dir"

SetCompressor /SOLID lzma
XPStyle on

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "sandbox (required)"

  SectionIn RO
  
  SetOutPath $INSTDIR
  
  File /r "..\..\*.*"
  
  WriteRegStr HKLM SOFTWARE\sandbox "Install_Dir" "$INSTDIR"
  
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\sandbox" "DisplayName" "sandbox"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\sandbox" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\sandbox" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\sandbox" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "Visual C++ redistributable runtime"

  ExecWait '"$INSTDIR\bin\vcredist_x86.exe"'
  
SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\sandbox"
  
  SetOutPath "$INSTDIR"
  
  CreateShortCut "$INSTDIR\sandbox.lnk"                "$INSTDIR\sandbox.bat" "" "$INSTDIR\sandbox.bat" 0
  CreateShortCut "$SMPROGRAMS\sandbox\sandbox.lnk" "$INSTDIR\sandbox.bat" "" "$INSTDIR\sandbox.bat" 0
  CreateShortCut "$SMPROGRAMS\sandbox\Uninstall.lnk"   "$INSTDIR\uninstall.exe"   "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\sandbox\README.lnk"      "$INSTDIR\README.html"     "" "$INSTDIR\README.html" 0
  
SectionEnd

Section "Uninstall"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\sandbox"
  DeleteRegKey HKLM SOFTWARE\sandbox

  RMDir /r "$SMPROGRAMS\sandbox"
  RMDir /r "$INSTDIR"

SectionEnd
