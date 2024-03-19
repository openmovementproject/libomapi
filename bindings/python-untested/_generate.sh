# git clone https://github.com/ctypesgen/ctypesgen

# Copy .lib so paths are relative
cp ../../src/libomapi.a .

# Copy .so for test program
cp ../../src/libomapi.so .

# Copy .dll for test program on Windows -- build with: build.cmd
cp ../../src/libomapi.dll libomapi.dll

# Copy 64.dll for test program on Windows -- build with: build.cmd x64
cp ../../src/libomapi64.dll libomapi64.dll

# Generate ctypes bindings
python3 ctypesgen/run.py -a -lomapi -o pylibomapi.py ../../include/omapi.h

# Add 64-bit library build to the list of libraries to try on Windows
sed -i -e 's/"lib%s.dll", /"lib%s.dll", "lib%s64.dll", /' pylibomapi.py
