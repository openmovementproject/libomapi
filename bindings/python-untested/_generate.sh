# git clone https://github.com/ctypesgen/ctypesgen

# Copy .lib so paths are relative
cp ../../src/libomapi.a .

# Copy .so for test program
cp ../../src/libomapi.so .

# Copy .dylib for test program
#cp ../../src/libomapi.dylib .

# Copy .dll for test program on Windows -- build with: build.cmd
cp ../../src/libomapi.dll libomapi.dll

# Copy 64.dll for test program on Windows -- build with: build.cmd x64
cp ../../src/libomapi64.dll libomapi64.dll

# Generate ctypes bindings
python3 ctypesgen/run.py -a -lomapi -o pylibomapi.py ../../include/omapi.h


# Option: Customize WindowsLibraryLoader / name_formats to add the 64-bit library build to the list of libraries to try on Windows
sed -i -e 's/"lib%s.dll", /"lib%s.dll", "lib%s64.dll", /' pylibomapi.py

# Option: Conditionally change `# Begin libraries` / `_libs["omapi"] = load_library("omapi")` on 64-bit Windows to load "omapi64" instead.
#         load_library("omapi" + ("64" if sys.platform == "win32" and sys.maxsize > 2**32 else ""))
#sed -i -e 's/load_library("omapi")/load_library("omapi" + ("64" if sys.platform == "win32" and sys.maxsize > 2**32 else ""))/' pylibomapi.py

# Alternative: Could rename the 64-bit library to '*.dll'/'*'/'*lib.dll'

# Alternative: Use add_library_search_dirs() to use a subdirectory for the 64-bit library
