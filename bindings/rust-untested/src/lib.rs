#![allow(non_upper_case_globals)]  // Globals should be upper-case (some generated globals are not)
#![allow(non_camel_case_types)]  // Types should be upper camel case (but the library enums are not)
#![allow(non_snake_case)]  // Fields should be snake case (but the library struct fields are not)
//#![allow(improper_ctypes)]  // warning: `extern` block uses type `u128`, which is not FFI-safe
//#![allow(dead_code)]
//#![allow(unused_imports)]

//include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
include!("../bindings.rs");

// pub fn test(a: usize) -> usize {
//     a
// }

/// From a macro to check the specified return value for success. Returns `true` for a successful (non-negative) value, `false` otherwise.
pub fn OM_SUCCEEDED(value: i32) -> bool {
    value >= 0
}

/// From a macro to check the specified return value for failure. Returns `true` for a failed (negative) value, `false` otherwise.
pub fn OM_FAILED(value: i32) -> bool {
    value < 0
}
