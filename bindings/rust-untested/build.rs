// Based on: https://fitzgeraldnick.com/2016/12/14/using-libbindgen-in-build-rs.html
extern crate bindgen;

// # See: https://rust-lang.github.io/rust-bindgen/command-line-usage.html
// sudo apt install libclang-dev   # winget install llvm.llvm
// rustup update
// cargo install bindgen-cli
// ~/.cargo/bin/bindgen wrapper.h -o bindings.rs --default-macro-constant-type signed
// cargo build
//
// #lipo -info libomapi.a target/debug/librustomapi.rlib
// #nm libomapi.a | grep _OmS | grep -v _OmSe
//
// #dumpbin /exports libomapi.dll
// #dumpbin /exports libomapi64.dll

use std::env;
use std::fs;
use std::path::Path;
use std::path::PathBuf;

// If the destination file doesn't exist and the source file exists, or if the source file is newer than the destination file, copy the source file to the destination
fn copy_if_newer<P: AsRef<Path>>(source: P, destination: P) {
    if (destination.as_ref().exists() && source.as_ref().exists() && fs::metadata(&source).unwrap().modified().unwrap() > fs::metadata(&destination).unwrap().modified().unwrap())
        || (!destination.as_ref().exists() && source.as_ref().exists())
    {
        fs::copy(&source, &destination).expect("Error copying file");
    }
}

fn main() {
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    println!("cargo:rerun-if-changed=wrapper.h");
    println!("cargo:rerun-if-changed=../../include/omapi.h");
    
    // Tell cargo to tell rustc to link the library.
    println!("cargo:rustc-link-search=native={}", Path::new(&dir).display());   // .join(".")

    // Windows uses 64-bit version of the dynamic library: libomapi64.dll
    #[cfg(target_os = "windows")]
    {
        println!("cargo:rustc-link-lib=dylib=libomapi64");
        copy_if_newer("../../src/libomapi64.lib", "libomapi64.lib");
        copy_if_newer("../../src/libomapi64.dll", "libomapi64.dll");
    }
    // Non-windows will use a static library: (omapi ->) libomapi.a
    #[cfg(not(target_os = "windows"))]
    {
        println!("cargo:rustc-link-lib=static=omapi");
        copy_if_newer("../../src/libomapi.a", "libomapi.a");

        //println!("cargo:rustc-link-lib=dylib=omapi");
        copy_if_newer("../../src/libomapi.so", "libomapi.so");
        copy_if_newer("../../src/libomapi.dylib", "libomapi.dylib");
    }
    // Linux dynamically needs libudev
    #[cfg(target_os = "linux")]
    {
        println!("cargo:rustc-link-lib=dylib=udev"); // sudo apt install libudev-dev
    }
    // macOS needs the frameworks: Cocoa, IOKit, DiskArbitration
    #[cfg(target_os = "macos")]
    {
        println!("cargo:rustc-link-lib=framework=Cocoa");
        println!("cargo:rustc-link-lib=framework=IOKit");
        println!("cargo:rustc-link-lib=framework=DiskArbitration");
    }

    // The bindgen::Builder is the main entry point to bindgen, and lets you build up options for the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate bindings for.
        .header("wrapper.h")                    // wrapper.h
        // Default macro constant type
        .default_macro_constant_type(bindgen::MacroTypeVariation::Signed)  // --default-macro-constant-type signed
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings file.
    //let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    let out_path = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let out_file = out_path.join("bindings.rs");    // -o bindings.rs
    bindings
        .write_to_file(out_file)
        .expect("Couldn't write bindings!");
}
