@echo off
net stop zero > nul 2>&1
echo Installing Zero And Random Device Driver...
pushd "%~dp0"
"%SystemRoot%\system32\rundll32.exe" setupapi.dll,InstallHinfSection DefaultInstall 132 .\zero.inf
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
