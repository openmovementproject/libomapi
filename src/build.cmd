@ECHO OFF
SETLOCAL EnableDelayedExpansion
CD /D %~dp0

SET FIND_CL=
FOR %%p IN (cl.exe) DO SET "FIND_CL=%%~$PATH:p"
IF DEFINED FIND_CL (
  ECHO Build tools already on path - %ARCH%
  GOTO BUILD
)

SET ARCH=x86
IF "%1"=="" GOTO AFTER_ARCH
IF "%1"=="x86" (
  SET ARCH=x86
  GOTO AFTER_ARCH
)
IF "%1"=="x64" (
  SET ARCH=x64
  GOTO AFTER_ARCH
)
ECHO ERROR: Unknown ARCH argument set - expected x86 or x64.
GOTO ERROR
:AFTER_ARCH

ECHO Build tools not on path, looking for 'vcvarsall.bat'...
SET VCVARSALL=
FOR %%f IN (70 71 80 90 100 110 120 130 140) DO IF EXIST "!VS%%fCOMNTOOLS!\..\..\VC\vcvarsall.bat" SET VCVARSALL=!VS%%fCOMNTOOLS!\..\..\VC\vcvarsall.bat
FOR /F "usebackq tokens=*" %%f IN (`DIR /B /ON "%ProgramFiles(x86)%\Microsoft Visual Studio\????"`) DO FOR %%g IN (BuildTools Community Professional Enterprise) DO IF EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat" SET "VCVARSALL=%ProgramFiles(x86)%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat"
FOR /F "usebackq tokens=*" %%f IN (`DIR /B /ON "%ProgramFiles%\Microsoft Visual Studio\????"`) DO FOR %%g IN (BuildTools Community Professional Enterprise) DO IF EXIST "%ProgramFiles%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat" SET "VCVARSALL=%ProgramFiles%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat"
IF "%VCVARSALL%"=="" ECHO Cannot find C compiler environment for 'vcvarsall.bat'. & GOTO ERROR
ECHO Setting environment variables for C compiler... %VCVARSALL%
CALL "%VCVARSALL%" %ARCH%

:BUILD
SET SUFFIX=
IF "%ARCH%"=="x64" SET SUFFIX=64
SET FILENAME=libomapi%SUFFIX%

ECHO.
ECHO FILENAME=%FILENAME%
ECHO.
ECHO ARCH=%ARCH%
ECHO.
ECHO LIB=%LIB%
ECHO.
ECHO INCLUDE=%INCLUDE%
ECHO.
ECHO LIBPATH=%LIBPATH%
ECHO.
ECHO WINDOWSSDKDIR=%WindowsSdkDir%
ECHO.
ECHO WINDOWSSDKVERSION=%WindowsSDKVersion%
ECHO.


:COMPILE
ECHO Compiling...
rem /DNO_MMAP
cl /D_WINDLL -c /EHsc /I"..\include" /Tc"omapi-download.c" /Tc"omapi-internal.c" /Tc"omapi-main.c" /Tc"omapi-reader.c" /Tc"omapi-settings.c" /Tc"omapi-status.c" /Tp"omapi-devicefinder-win.cpp"
IF ERRORLEVEL 1 GOTO ERROR

:LINK
ECHO Linking...
link /dll /out:%FILENAME%.dll omapi-download omapi-internal omapi-main omapi-reader omapi-settings omapi-status omapi-devicefinder-win
IF ERRORLEVEL 1 GOTO ERROR

:DEF
ECHO Creating an import library .lib...
rem --- Parse the exports to create a .DEF file ---
ECHO.LIBRARY LIBOMAPI%SUFFIX%>%FILENAME%.def
ECHO.EXPORTS>>%FILENAME%.def
FOR /F "usebackq tokens=1,4 delims= " %%F IN (`dumpbin /exports %FILENAME%.dll ^| FINDSTR /X /R /C:"^  *[0-9][0-9]*  *[0-9A-F][0-9A-F]*  *[0-9A-F][0-9A-F]*  *[A-Za-z_][A-Za-z_0-9]*$"`) DO (
  rem ECHO.  %%G  @%%F>>%FILENAME%.def
  ECHO.  %%G>>%FILENAME%.def
)
lib /def:%FILENAME%.def /out:%FILENAME%.lib /machine:%ARCH%
IF ERRORLEVEL 1 GOTO ERROR

rem echo --- %FILENAME%.dll exports ---
rem dumpbin /EXPORTS %FILENAME%.dll
rem echo ---------
rem echo --- %FILENAME%.lib exports ---
rem dumpbin /EXPORTS %FILENAME%.lib
rem echo ---------
rem echo --- %FILENAME%.lib linker members ---
rem dumpbin /LINKERMEMBER %FILENAME%.lib
rem echo ---------

:TEST
ECHO Compiling test program...
cl /c /EHsc /DOMAPI_DYNLIB_IMPORT /Dtest_main=main /I"..\include" /Tc"..\examples\test.c"
IF ERRORLEVEL 1 GOTO ERROR
ECHO Linking test program...
link /out:test.exe test %FILENAME%.lib
IF ERRORLEVEL 1 GOTO ERROR
GOTO END

:ERROR
ECHO ERROR: An error occured.
pause
GOTO END

:END
ENDLOCAL
