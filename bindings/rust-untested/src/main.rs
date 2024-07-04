// Example test code for libomapi bindings

// cargo run -q

//use std::ffi::CString;
use std::ffi::CStr;
use std::ffi::c_void;
use std::ffi::c_char;
//use std::ptr;
use std::ptr::null_mut;
use std::process;

unsafe extern "C" fn log_callback(_reference: *mut c_void, message: *const c_char) {
    let c_msg = CStr::from_ptr(message);
    let r_msg = c_msg.to_str().expect("String encoding");       // .to_owned() if retaining after scope
    println!("OMLOG: {}", r_msg);
}

unsafe extern "C" fn device_callback(_reference: *mut c_void, device_id: i32, status: i32) {
    if status == rustomapi::OM_DEVICE_STATUS_OM_DEVICE_REMOVED {
        println!("OMDEVICE: CONNECTED: {}", device_id);
    } else if status == rustomapi::OM_DEVICE_STATUS_OM_DEVICE_REMOVED {
        println!("OMDEVICE: DISCONNECTED: {}", device_id);
    }
}

fn test_device(device_id: i32)
{
    let mut errors = 0;

    println!("TEST: Testing device #{}", device_id);

    // Set the LED to blue to indicate testing
    unsafe {
        let result = rustomapi::OmSetLed(device_id, rustomapi::OM_LED_STATE_OM_LED_BLUE);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmSetLed() {}", err_msg);
            errors += 1
        }
    }

    // Check hardware and firmware versions
    unsafe {
        let mut firmware_version: i32 = 0;
        let mut hardware_version: i32 = 0;
        let result = rustomapi::OmGetVersion(device_id, &mut firmware_version, &mut hardware_version);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmGetVersion() {}", err_msg);
            errors += 1
        } else {
            println!("CHECK #{}: Firmware {}, Hardware {}", device_id, firmware_version, hardware_version);
        }
    }

    // Check battery level


    unsafe {
        let result = rustomapi::OmGetBatteryLevel(device_id);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmGetBatteryLevel() {}", err_msg);
            errors += 1
        } else {
            let state = if result >= 100 { "charged" } else { "charging" };
            println!("CHECK #{}: Battery at {}% ({})", device_id, result, state);
        }
    }
    
    // Get accelerometer readings
    unsafe {
        let mut accelerometer: [i32; 3] = [0; 3];
        let result = rustomapi::OmGetAccelerometer(device_id, &mut accelerometer[0], &mut accelerometer[1], &mut accelerometer[2]);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmGetAccelerometer() {}", err_msg);
            errors += 1
        } else {
            println!("CHECK #{}: Accelerometer at (x = {}, y = {}, z = {})", device_id, accelerometer[0], accelerometer[1], accelerometer[2]);
        }
    }

    // Check self-test
    unsafe {
        let result = rustomapi::OmSelfTest(device_id);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmSelfTest() {}", err_msg);
            errors += 1;
        } else {
            if result == 0 {
                println!("CHECK #{}: Self-test: OK", device_id);
            } else {
                errors += 1;
                println!("CHECK #{}: Self-test: FAILED (diagnostic {:#06x})", device_id, result);
            }
        }
    }

    // Check memory health
    unsafe {
        let result = rustomapi::OmGetMemoryHealth(device_id);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmGetMemoryHealth() {}", err_msg);
            errors += 1;
        } else {
            if result <= rustomapi::OM_MEMORY_HEALTH_ERROR {
                errors += 1;
                println!("CHECK #{}: Memory health: FAILED (at least one plane has (or is near to having) no free blocks)", device_id);
            } else if result <= rustomapi::OM_MEMORY_HEALTH_WARNING {
                println!("CHECK #{}: Memory health: WARNING (only {} free blocks on worst plane)", device_id, result);
            } else {
                println!("CHECK #{}: Memory health: OK (at least {} free blocks on each plane)", device_id, result);
            }
        }
    }

    // Check battery health
    unsafe {
        let result = rustomapi::OmGetBatteryHealth(device_id);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmGetBatteryHealth() {}", err_msg);
            errors += 1;
        } else {
            if result > 500 {
                println!("CHECK #{}: Battery health: NOTICE ({} cycles)", device_id, result);
            } else {
                println!("CHECK #{}: Battery health: OK ({} cycles)", device_id, result);
            }
        }
    }

    // Set the LED to WHITE if successful, or RED otherwise
    let result = if errors > 0 {
        unsafe {
            rustomapi::OmSetLed(device_id, rustomapi::OM_LED_STATE_OM_LED_RED)
        }
    } else {
        unsafe {
            rustomapi::OmSetLed(device_id, rustomapi::OM_LED_STATE_OM_LED_WHITE)
        }
    };

    if rustomapi::OM_FAILED(result) {
        unsafe {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("WARNING: OmSetLed() {}", err_msg);
        }
    }
}


fn main() {
    println!(
        "Testing...: {:?}",
        rustomapi::test(123)
    );

    // Set the log callback
    println!("OmSetLogCallback()");
    unsafe {
        if rustomapi::OM_FAILED(rustomapi::OmSetLogCallback(Some(log_callback), null_mut())) {
            println!("ERROR: OmSetLogCallback()");
            process::exit(-1);
        }
    }

    // Set the device callback
    println!("OmSetDeviceCallback()");
    unsafe {
        if rustomapi::OM_FAILED(rustomapi::OmSetDeviceCallback(Some(device_callback), null_mut())) {
            println!("ERROR: OmSetDeviceCallback()");
            process::exit(-1);
        }
    }

    unsafe {
        println!("OmStartup()...");
        rustomapi::OmStartup(rustomapi::OM_VERSION);
    }

    // Query the current number of devices attached
    let mut num_devices = unsafe {
        let result = rustomapi::OmGetDeviceIds(null_mut(), 0);
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("ERROR: OmGetDeviceIds() {}", err_msg);
            process::exit(-1);
        }
        result as usize
    };
    println!("TEST: Found {} devices.", num_devices);
    
    // Get the currently-attached devices ids
    let mut device_ids: Vec<i32> = Vec::with_capacity(num_devices);
    unsafe {
        let uninitialized = device_ids.spare_capacity_mut();
        let result = rustomapi::OmGetDeviceIds(
            uninitialized.as_mut_ptr().cast(),
            uninitialized.len() as i32,
        );
        if rustomapi::OM_FAILED(result) {
            let err_msg = CStr::from_ptr(rustomapi::OmErrorString(result)).to_str().expect("CStr");
            println!("ERROR: OmGetDeviceIds() {}", err_msg);
            process::exit(-1);
        }
        num_devices = result as usize;
        device_ids.set_len(num_devices);
    };
    let device_ids = device_ids.as_slice();

    if num_devices == 0 {
        println!("TEST: No devices found.");
    }

    // For each device currently connected...
    for (i, device_id) in device_ids.iter().enumerate() {
        println!("TEST {}/{} #{}: Device already CONNECTED", i + 1, num_devices, device_id);
        test_device(*device_id);
    }

    unsafe {
        println!("OmShutdown()...");
        rustomapi::OmShutdown();
    }

}

