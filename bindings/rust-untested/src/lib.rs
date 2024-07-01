#![allow(non_upper_case_globals, non_camel_case_types, non_snake_case)]
//#![allow(dead_code)]

//include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
include!("../bindings.rs");

pub fn test(a: usize) -> usize {
    a
}
