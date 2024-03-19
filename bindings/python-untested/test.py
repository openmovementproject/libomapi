from ctypes import c_int, byref
import pylibomapi as om


# Test device
def testDevice(deviceId):
    print("TEST: Testing device #%d" % deviceId)
    errors = 0

    # Set the LED to blue to indicate testing
    result = om.OmSetLed(deviceId, om.OM_LED_BLUE)
    if om.OM_FAILED(result):
        print("WARNING: OmSetLed() " + om.OmErrorString(result))
        errors += 1

    # Check hardware and firmware versions
    firmwareVersion = c_int()
    hardwareVersion = c_int()
    result = om.OmGetVersion(deviceId, byref(firmwareVersion), byref(hardwareVersion))
    if om.OM_FAILED(result):
        print("WARNING: OmGetVersion() " + om.OmErrorString(result))
        errors += 1
    else:
        print("CHECK #%d: Firmware %d, Hardware %d" % (deviceId, firmwareVersion.value, hardwareVersion.value))

    # Check battery level
    result = om.OmGetBatteryLevel(deviceId)
    if om.OM_FAILED(result):
        print("WARNING: OmGetBatteryLevel() " + om.OmErrorString(result))
        errors += 1
    else:
        print("CHECK #%d: Battery at %d%% (%s)" % (deviceId, result, "charged" if result >= 100 else "charging"))

    # Get accelerometer readings
    accelerometer = [ c_int(), c_int(), c_int() ]
    result = om.OmGetAccelerometer(deviceId, byref(accelerometer[0]), byref(accelerometer[1]), byref(accelerometer[2]))
    if om.OM_FAILED(result):
        print("WARNING: OmGetAccelerometer() " + om.OmErrorString(result))
        errors += 1
    else:
        print("CHECK #%d: Accelerometer at (x = %d, y = %d, z = %d)" % (deviceId, accelerometer[0].value, accelerometer[1].value, accelerometer[2].value))

    # Check self-test
    result = om.OmSelfTest(deviceId)
    if om.OM_FAILED(result):
        print("WARNING: OmSelfTest() " + om.OmErrorString(result))
        errors += 1
    else:
        if result == 0:
            print("CHECK #%d: Self-test: OK" % deviceId)
        else:
            errors += 1
            print("CHECK #%d: Self-test: FAILED (diagnostic 0x%04x)" % (deviceId, result))

    # Check memory health
    result = om.OmGetMemoryHealth(deviceId)
    if om.OM_FAILED(result):
        print("WARNING: OmGetMemoryHealth() " + om.OmErrorString(result))
        errors += 1
    else:
        if result <= om.OM_MEMORY_HEALTH_ERROR:
            errors += 1
            print("CHECK #%d: Memory health: FAILED (at least one plane has (or is near to having) no free blocks)" % deviceId)
        elif result <= om.OM_MEMORY_HEALTH_WARNING:
            print("CHECK #%d: Memory health: WARNING (only %d free blocks on worst plane)" % (deviceId, result))
        else:
            print("CHECK #%d: Memory health: OK (at least %d free blocks on each plane)" % (deviceId, result))

    # Check battery health
    result = om.OmGetBatteryHealth(deviceId)
    if om.OM_FAILED(result):
        print("WARNING: OmGetBatteryHealth() " + om.OmErrorString(result))
        errors += 1
    else:
        if result > 500:
            print("CHECK #%d: Battery health: NOTICE (%d cycles)" % (deviceId, result))
        else:
            print("CHECK #%d: Battery health: OK (%d cycles)" % (deviceId, result))

    # Set the LED to WHITE if successful, or RED otherwise
    if errors > 0:
        result = om.OmSetLed(deviceId, om.OM_LED_RED)
    else:
        result = om.OmSetLed(deviceId, om.OM_LED_WHITE)
    if om.OM_FAILED(result):
        print("WARNING: OmSetLed() " + om.OmErrorString(result))


# Log callback
@om.OmLogCallback
def logCallback(ref, message):
    print("OMLOG: " + message)

# Device callback
@om.OmDeviceCallback
def deviceCallback(ref, deviceId, status):
    if status == om.OM_DEVICE_CONNECTED:
        print("OMDEVICE: CONNECTED: " + str(deviceId))
    elif status == om.OM_DEVICE_DISCONNECTED:
        print("OMDEVICE: DISCONNECTED: " + str(deviceId))


# Set the log callback
print("OmSetLogCallback()")
if om.OM_FAILED(om.OmSetLogCallback(logCallback, None)):
    print("ERROR: OmSetLogCallback()")
    sys.exit(-1)

# Set the device callback
print("OmSetDeviceCallback()")
if om.OM_FAILED(om.OmSetDeviceCallback(deviceCallback, None)):
    print("ERROR: OmSetDeviceCallback()")
    sys.exit(-1)


# Start the API
print("OmStartup()")
if om.OM_FAILED(om.OmStartup(om.OM_VERSION)):
    print("ERROR: OmStartup()")
    sys.exit(-1)

# Query the current number of devices attached
result = om.OmGetDeviceIds(None, 0)
if om.OM_FAILED(result):
    print("ERROR: OmGetDeviceIds() " + om.OmErrorString(result))
    sys.exit(-1)

numDevices = result
print("TEST: Found %d devices." % numDevices)

# Get the currently-attached devices ids
deviceIds = (c_int * numDevices)()
result = om.OmGetDeviceIds(deviceIds, numDevices)

if om.OM_FAILED(result):
    print("ERROR: OmGetDeviceIds() " + om.OmErrorString(result))
    sys.exit(-1)

# Cope with fewer devices being returned (if some were just removed).
if (result < numDevices):
    numDevices = result

if numDevices == 0:
    print("TEST: No devices found.")

# For each device currently connected...
for i in range(numDevices):
    print("TEST %d/%d #%d: Device already CONNECTED" % (i + 1, numDevices, deviceIds[i]))
    testDevice(deviceIds[i])

# Free our list of device ids
del deviceIds

# Shutdown the API
print("OmShutdown()")
if om.OM_FAILED(om.OmShutdown()):
    print("ERROR: OmStartup()")
    sys.exit(-1)


