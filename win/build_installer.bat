@echo off
setlocal EnableDelayedExpansion

set QTVERSION="6"

if not defined QTBINPATH (
	for /f "delims=" %%a in ('dir /b C:\Qt\%QTVERSION%.*') do set QTFULLVERSION=%%a
	for /f "delims=" %%a in ('dir /b C:\Qt\!QTFULLVERSION!\mingw*') do set QTBINPATH=%%a
	set QTBINPATH=C:\Qt\!QTFULLVERSION!\!QTBINPATH!\bin
)
echo Using Qt Version %QTFULLVERSION%
if not defined QTLIBPATH (
	for /f "delims=" %%a in ('dir /b C:\Qt\Tools\mingw*') do set QTLIBPATH=%%a
	set QTLIBPATH=C:\Qt\Tools\!QTLIBPATH!\bin
)
echo Using mingw libraries from %QTLIBPATH%
if not defined WIXPATH (
	for /f "delims=" %%a in ('dir /b "C:\Program Files (x86)\Wix Toolset*"') do set WIXPATH=%%a
	set WIXPATH=C:\Program Files ^(x86^)\!WIXPATH!\bin
)
if not defined SSLPATH (set SSLPATH=C:\Qt\Tools\OpenSSL\Win_x64\bin)
if not defined RTAUDIOLIB (set RTAUDIOLIB="C:\Program Files (x86)\RtAudio\bin\librtaudio.dll")

set PATH=%PATH%;%WIXPATH%
del deploy /s /q
rmdir deploy /s /q
mkdir deploy

copy ..\LICENSE.md deploy\
xcopy ..\LICENSES deploy\LICENSES\

REM create RTF file with licenses' text
set LICENSEPATH=deploy\license.rtf
echo {\rtf1\ansi\deff0 {\fonttbl {\f0 Calibri;}} \f0\fs22>%LICENSEPATH%
for %%f in (..\LICENSE.md ..\LICENSES\GPL-3.0.txt ..\LICENSES\LGPL-3.0-only.txt) do (
  for /f "delims=" %%x in ('type %%f') do (
    echo %%x\line>>%LICENSEPATH%
  )
  echo \par >>%LICENSEPATH%
)
echo }>>%LICENSEPATH%

copy dialog.bmp deploy\
if exist ..\builddir\release\qjacktrip.exe (set JACKTRIP=..\builddir\release\qjacktrip.exe) else (set JACKTRIP=..\builddir\qjacktrip.exe)
copy %JACKTRIP% deploy\
cd deploy
set "WIXDEFINES="
for /f "tokens=*" %%a in ('%QTLIBPATH%\objdump -p qjacktrip.exe ^| findstr Qt%QTVERSION%Core.dll') do set DYNAMIC_QT=%%a
if defined DYNAMIC_QT (
	echo Including Qt Files
	%QTBINPATH%\windeployqt qjacktrip.exe

	Rem windeployqt takes care of this now
	if %QTVERSION%=="5" (
		copy "%QTLIBPATH%\libgcc_s_seh-1.dll" .\
		copy "%QTLIBPATH%\libstdc++-6.dll" .\
		copy "%QTLIBPATH%\libwinpthread-1.dll" .\
	)
	copy "%SSLPATH%\libcrypto-1_1-x64.dll" .\
	copy "%SSLPATH%\libssl-1_1-x64.dll" .\
	set WIXDEFINES=!WIXDEFINES! -ddynamic
)
for /f "tokens=*" %%a in ('%QTLIBPATH%\objdump -p qjacktrip.exe ^| findstr librtaudio.dll') do set RTAUDIO=%%a
if defined RTAUDIO (
	echo Including librtaudio
	copy %RTAUDIOLIB% .\
	set WIXDEFINES=%WIXDEFINES% -drtaudio
)
set WIXDEFINES=%WIXDEFINES% -dqt%QTVERSION%

copy ..\qjacktrip.wxs .\
copy ..\files.wxs .\
.\qjacktrip --test-gui
if %ERRORLEVEL% NEQ 0 (
	echo You need to build qjacktrip with gui support to build the installer.
	exit /b 1
)
rem Get our version number
for /f "tokens=*" %%a in ('.\qjacktrip -v ^| findstr VERSION') do for %%b in (%%~a) do set VERSION=%%b
for /f "tokens=1 delims=-" %%a in ("%VERSION%") do set VERSION=%%a
echo Version=%VERSION%
candle.exe -arch x64 -ext WixUIExtension -ext WixUtilExtension -dVersion=%VERSION%%WIXDEFINES% qjacktrip.wxs files.wxs
light.exe -ext WixUIExtension -ext WixUtilExtension -o QJackTrip.msi qjacktrip.wixobj files.wixobj
endlocal
