@echo off
"%SystemRoot%\system32\net.exe" stop zero > nul 2>&1
echo Installing Zero And Random Device Driver...
pushd "%~dp0"
.\w32verc.exe
if errorlevel 6000 (
	"%SystemRoot%\system32\rundll32.exe" setupapi.dll,InstallHinfSection DefaultInstall 132 win7\zero.inf
) else if errorlevel 3790 (
	"%SystemRoot%\system32\rundll32.exe" setupapi.dll,InstallHinfSection DefaultInstall 132 winnet\zero.inf
) else (
	"%SystemRoot%\system32\rundll32.exe" setupapi.dll,InstallHinfSection DefaultInstall 132 win2k\zero.inf
)
popd
echo.
echo Loading Zero And Random Device Driver...
"%SystemRoot%\system32\net.exe" start zero > nul 2>&1
if errorlevel 1 (
  echo.
  echo Error installing/loading the driver. Check %SystemRoot%\setupapi.log for details.
) else (
  echo.
  echo The Zero And Random Device Driver was successfully loaded into the kernel.
)
echo.
pause
