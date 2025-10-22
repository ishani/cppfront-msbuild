@echo off

cd /D "%~dp0"
set GEN_DIR=%~dp0_gen
set CPPFRONT_EXE_PATH=%GEN_DIR%\cppfront.exe
set CLANGFMT_EXE_PATH=%GEN_DIR%\clang-format.exe
set DISPATCH_EXE_PATH=%GEN_DIR%\dispatch.exe
echo Building cppfront : %CPPFRONT_EXE_PATH%


for /f "usebackq tokens=*" %%i in (`%GEN_DIR%\\vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Llvm.Clang -property installationPath`) do (
  set InstallDir=%%i
)

:: launch a VS build environment
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

  call "%InstallDir%\Common7\Tools\vsdevcmd.bat" %*

  if exist "%CPPFRONT_EXE_PATH%" (
    del "%CPPFRONT_EXE_PATH%"
  )

  copy "%InstallDir%\VC\Tools\Llvm\x64\bin\clang-format.exe" %CLANGFMT_EXE_PATH%

  echo Building %DISPATCH_EXE_PATH% ...
  cd cpp2-dispatch
  clang++ dispatch.cpp -I ./cli11 -I ./simplecpp -O1 -std=c++20 -o "%DISPATCH_EXE_PATH%"

  echo Building %CPPFRONT_EXE_PATH% ...
  cd ../../ext/cppfront/source
  clang++ cppfront.cpp -O0 -std=c++20  -o "%CPPFRONT_EXE_PATH%"


  if exist "%CPPFRONT_EXE_PATH%" (
    exit 0
  )

  echo Compilation failed
  exit 2
)

echo Unable to find Visual Studio components required to build cppfront (Clang required)
exit 1