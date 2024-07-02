// Example test code for libomapi bindings

// cargo run -q

fn main() {
    println!(
        "Testing...: {:?}",
        rustomapi::test(123)
    );

    unsafe {
        println!("OmStartup()...");
        rustomapi::OmStartup(rustomapi::OM_VERSION);
    }

    unsafe {
        println!("OmShutdown()...");
        rustomapi::OmShutdown();
    }

}
