#!/bin/sh

set -e

#git clone https://github.com/ctypesgen/ctypesgen
# --- or ---
#python -m venv .
#source ./bin/activate
#pip install -r requirements.txt
if [ ! -e ./ctypesgen ]; then
  git clone https://github.com/ctypesgen/ctypesgen
fi



# Generate ctypes bindings
python3 ctypesgen/run.py -a -lomapi -o pylibomapi.py ../../include/omapi.h
#ctypesgen -a -lomapi -o pylibomapi.py ../../include/omapi.h

# Option: Customize WindowsLibraryLoader / name_formats to add the 64-bit library build to the list of libraries to try on Windows
#sed -i '.bak' -e 's/"lib%s.dll", /"lib%s.dll", "lib%s64.dll", /' pylibomapi.py

# Option: Conditionally change `# Begin libraries` / `_libs["omapi"] = load_library("omapi")` on 64-bit Windows to load "omapi64" instead.
#         load_library("omapi" + ("64" if sys.platform == "win32" and sys.maxsize > 2**32 else ""))
sed -i '.bak' -e 's/load_library("omapi")/load_library("omapi" + ("64" if sys.platform == "win32" and sys.maxsize > 2**32 else ""))/' pylibomapi.py

# Alternative: Could rename the 64-bit library to '*.dll'/'*'/'*lib.dll'

# Alternative: Use add_library_search_dirs() to use a subdirectory for the 64-bit library


# Copy .lib so paths are relative
if [ -e ../../src/libomapi.a ]; then
 cp ../../src/libomapi.a .
fi

# Copy .so for test program -- build with: make libomapi.so
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

# Copy stub .lib for test program on Windows -- build with: build.cmd
if [ -e ../../src/libomapi.lib ]; then
 cp ../../src/libomapi.lib .
fi

# Copy 64-bit .dll for test program on Windows -- build with: build.cmd x64
if [ -e ../../src/libomapi64.dll ]; then
 cp ../../src/libomapi64.dll .
fi

# Copy stub .lib for test program on Windows -- build with: build.cmd x64
if [ -e ../../src/libomapi64.lib ]; then
 cp ../../src/libomapi64.lib .
fi


#nm libomapi.a | grep _OmS | grep -v _OmSe

#dumpbin /exports libomapi.dll
#dumpbin /exports libomapi64.dll

