#![allow(non_upper_case_globals, non_camel_case_types, non_snake_case)]
#![allow(improper_ctypes)]  // warning: `extern` block uses type `u128`, which is not FFI-safe
//#![allow(dead_code)]
//#![allow(unused_imports)]

//include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
include!("../bindings.rs");

pub fn test(a: usize) -> usize {
    a
}
