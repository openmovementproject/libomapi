@echo off
rem   wsl ./_generate.sh

IF EXIST ../../src/libomapi.a COPY ../../src/libomapi.a .
IF EXIST ../../src/libomapi.so COPY ../../src/libomapi.so .
IF EXIST ../../src/libomapi.dylib COPY ../../src/libomapi.dylib .
IF EXIST ../../src/libomapi.dll COPY ../../src/libomapi.dll .
IF EXIST ../../src/libomapi.lib COPY ../../src/libomapi.lib .
IF EXIST ../../src/libomapi64.dll COPY ../../src/libomapi64.dll .
IF EXIST ../../src/libomapi64.lib COPY ../../src/libomapi64.lib .

rem winget install llvm.llvm
rem rustup update
rem cargo install bindgen-cli
bindgen wrapper.h -o bindings.rs --default-macro-constant-type signed
