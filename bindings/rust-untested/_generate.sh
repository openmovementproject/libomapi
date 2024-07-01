#!/bin/sh

set -e

### https://rust-lang.github.io/rust-bindgen/command-line-usage.html
# cargo install bindgen-cli
~/.cargo/bin/bindgen wrapper.h -o bindings.rs --default-macro-constant-type signed

# Copy .lib so paths are relative
if [ -e ../../src/libomapi.a ]; then
 cp ../../src/libomapi.a .
fi

# Copy .so for test program
if [ -e ../../src/libomapi.so ]; then
 cp ../../src/libomapi.so .
fi

# Copy .dylib for test program
if [ -e ../../src/libomapi.dylib ]; then
 cp ../../src/libomapi.dylib .
fi

# Copy .dll for test program on Windows -- build with: build.cmd
if [ -e ../../src/libomapi.dll ]; then
 cp ../../src/libomapi.dll .
fi

# Copy 64-bit .dll for test program on Windows -- build with: build.cmd x64
if [ -e ../../src/libomapi.dll ]; then
 cp ../../src/libomapi64.dll .
fi

#nm libomapi.a
