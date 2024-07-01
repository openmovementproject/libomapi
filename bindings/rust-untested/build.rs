// Based on: https://fitzgeraldnick.com/2016/12/14/using-libbindgen-in-build-rs.html
extern crate bindgen;

use std::env;
use std::path::Path;
use std::path::PathBuf;

fn main() {
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    
    println!("cargo:rerun-if-changed=wrapper.h");
    println!("cargo:rerun-if-changed=../../include/omapi.h");
    
    // Tell cargo to tell rustc to link the library.
    println!("cargo:rustc-link-search=native={}", Path::new(&dir).display());   // .join(".")
    println!("cargo:rustc-link-lib=omapi");

    // .clang_arg("-L.")
    // .clang_arg("-lomapi")

    // lipo -info libomapi.a target/debug/librustomapi.rlib && nm libomapi.a | grep _OmS | grep -v _OmSe

    // The bindgen::Builder is the main entry point to bindgen, and lets you build up options for the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate bindings for.
        .header("wrapper.h")
        // Default macro constant type
        .default_macro_constant_type(bindgen::MacroTypeVariation::Signed)
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings file.
    //let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    let out_path = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let out_file = out_path.join("bindings.rs");
    bindings
        .write_to_file(out_file)
        .expect("Couldn't write bindings!");
}
