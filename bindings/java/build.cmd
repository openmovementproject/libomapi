@ECHO OFF
rem Daniel Jackson, 2006-2012
SETLOCAL
CD /D %~dp0

ECHO Locating Java binaries...
IF NOT "%JAVA_HOME%"=="" (
  ECHO JAVA_HOME=%JAVA_HOME%
  GOTO JAVA_COMPILER
)

rem FOR %%X IN (javac.exe) do set JAVA_HOME=%%~dp$PATH:X..

FOR /F "tokens=*" %%G IN ('DIR /B /O:N "%PROGRAMFILES%\Java" ^| findstr /r "^jdk-*"') DO set JAVA_HOME=%PROGRAMFILES%\Java\%%G
IF NOT "%JAVA_HOME%"=="" (
  ECHO JAVA_HOME=%JAVA_HOME%
  GOTO JAVA_COMPILER
)

ECHO ERROR: Java binaries not found (JAVA_HOME not set, not located in standard location)
GOTO END

:JAVA_COMPILER
ECHO Compiling Java files...
if not exist class mkdir class
"%JAVA_HOME%\bin\javac.exe" -classpath class -d class -sourcepath src src/openmovement/JOM.java
IF ERRORLEVEL 1 GOTO ERROR

ECHO Creating JNI header file...
"%JAVA_HOME%\bin\javac.exe" -h jni -classpath class -d c src/openmovement/JOMAPI.java
IF ERRORLEVEL 1 GOTO ERROR

SET FIND_CL=
FOR %%p IN (cl.exe) DO SET "FIND_CL=%%~$PATH:p"
IF DEFINED FIND_CL (
  ECHO Build tools already on path - not changing target platform.
  GOTO BUILD
)

ECHO Build tools not on path, looking for 'vcvarsall.bat'...
SET ARCH=%1
IF NOT %ARCH%!==! GOTO PLATFORM_SPECIFIED
echo Platform not specified, auto-detecting...
SET ARCH=x86
IF NOT "%ProgramFiles(x86)%"=="" SET ARCH=x64
:PLATFORM_SPECIFIED
echo Platform is: %ARCH%

SET VCVARSALL=
FOR %%f IN (70 71 80 90 100 110 120 130 140) DO IF EXIST "!VS%%fCOMNTOOLS!\..\..\VC\vcvarsall.bat" SET VCVARSALL=!VS%%fCOMNTOOLS!\..\..\VC\vcvarsall.bat
FOR /F "usebackq tokens=*" %%f IN (`DIR /B /ON "%ProgramFiles(x86)%\Microsoft Visual Studio\????"`) DO FOR %%g IN (Community Professional Enterprise) DO IF EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat" SET "VCVARSALL=%ProgramFiles(x86)%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat"
FOR /F "usebackq tokens=*" %%f IN (`DIR /B /ON "%ProgramFiles%\Microsoft Visual Studio\????"`) DO FOR %%g IN (Community Professional Enterprise) DO IF EXIST "%ProgramFiles%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat" SET "VCVARSALL=%ProgramFiles%\Microsoft Visual Studio\%%f\%%g\VC\Auxiliary\Build\vcvarsall.bat"
IF "%VCVARSALL%"=="" ECHO Cannot find C compiler environment for 'vcvarsall.bat'. & GOTO ERROR
ECHO Setting environment variables for C compiler... %VCVARSALL%
CALL "%VCVARSALL%" %ARCH%

ECHO Compiling JNI file...
cl -c /D WIN32 /EHsc /I "%JAVA_HOME%\include" /I "%JAVA_HOME%\include\win32" /I "..\..\include" /Tc"c\JOMAPI.c" /Tp"..\..\src\omapi-devicefinder-win.cpp" /Tc"..\..\src\omapi-download.c" /Tc"..\..\src\omapi-internal.c" /Tc"..\..\src\omapi-main.c" /Tc"..\..\src\omapi-reader.c" /Tc"..\..\src\omapi-settings.c" /Tc"..\..\src\omapi-status.c"
IF ERRORLEVEL 1 GOTO ERROR

ECHO Linking JNI files... %VSCMD_ARG_TGT_ARCH%
SET POSTFIX=
IF /I %VSCMD_ARG_TGT_ARCH%!==x86! SET POSTFIX=32
IF /I %VSCMD_ARG_TGT_ARCH%!==x64! SET POSTFIX=64
rem  "%JAVA_HOME%\lib\jvm.lib" 
if not exist bin mkdir bin
link /dll /defaultlib:user32.lib JOMAPI omapi-devicefinder-win omapi-download omapi-internal omapi-main omapi-reader omapi-settings omapi-status /out:bin\JOMAPI%POSTFIX%.dll
IF ERRORLEVEL 1 GOTO ERROR

rem ECHO Copying DLL file...
rem copy /Y JOMAPI%POSTFIX%.dll "%JAVA_HOME%\bin"


rem GOTO END


:MAKE_JAR
"%JAVA_HOME%\bin\jar" cvfe lib\JOMAPI.jar openmovement.JOM -C class openmovement\JOMAPI.class -C class openmovement\JOM.class -C class openmovement\JOMAPIListener.class
IF ERRORLEVEL 1 GOTO ERROR

:RUN_TEST
ECHO Running test program...

"%JAVA_HOME%\bin\java" -Djava.library.path="./bin" -jar lib\JOMAPI.jar openmovement.JOM
IF ERRORLEVEL 1 GOTO ERROR
GOTO END


:ERROR
ECHO ERROR: An error occured.
pause
GOTO END

:END
ENDLOCAL
