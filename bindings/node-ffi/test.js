// tsc libomapi\libomapi.ts libomapi/omapi-test.ts -rootdir . --lib es2015,dom && node libomapi/omapi-test.js
// /usr/local/bin/tsc --lib es2015 libomapi.ts -rootdir .
/* eslint no-console: "off", new-cap: "off" */

var libomapi = require('./libomapi.js');
//import * as libomapi from './libomapi';

var om = new libomapi.Om();

function timeout(delay) {
    return new Promise(resolve => setTimeout(resolve, delay));
}

function DumpDevice(id) {
    console.log("----------");
    console.log("Device: " + id);
    
    console.log("OmGetBatteryLevel(" + id + ")");
    var battery = om.OmGetBatteryLevel(id);
    console.log("= " + battery);

    console.log("OmGetVersion(" + id + ")");
    var version = om.OmGetVersion(id)
    console.log("= " + version);

    /*
    (async() => {
        console.log("OmGetBatteryLevelAsync(" + id + ")");
        var batteryAsync = await om.OmGetBatteryLevelAsync(id);
        console.log("batt= " + batteryAsync);

        console.log("OmGetVersionAsync(" + id + ")");
        var versionAsync = await om.OmGetVersionAsync(id)
        console.log("ver= " + versionAsync);
    })().catch(err => {
        console.error(err.stack);
    });
    */

    console.log("OmGetDataFilename(" + id + ")");
    var filename = om.OmGetDataFilename(id)
    console.log("= " + filename);

    console.log("OmGetDevicePort(" + id + ")");
    var port = om.OmGetDevicePort(id)
    console.log("= " + port);

    console.log("OmGetDevicePath(" + id + ")");
    var path = om.OmGetDevicePath(id)
    console.log("= " + path);
    console.log("==========");
}

om.OmSetLogCallback(function(message) {
    console.log("LOGCALLBACK: " + message);
})

om.OmSetDeviceCallback(function(deviceId, status) {
    console.log("DEVICECALLBACK: " + deviceId + " (" + status + ")");
    if (status === libomapi.OM_DEVICE_STATUS.OM_DEVICE_CONNECTED) {
        DumpDevice(deviceId);
    }
})

async function start() {
    console.log("OmStartup()");
    await om.OmStartupAsync(om.OM_VERSION);
    
    console.log("OmGetDeviceIds()");
    var deviceIds = om.OmGetDeviceIds();
    console.log("IDs:", deviceIds);
    for (var i = 0; i < deviceIds.length; i++) {
        var id = deviceIds[i];
        console.log("Device: " + id);
        //await DumpDevice(id);
    }    
}


/*
async function doIdentify(id) {
    const n = 6;
    // Flash
    for (var i = 0; i < n; i += 1) {
        console.log("LED:", id, 5);  // eslint-disable-line no-console 
        await om.OmSetLedAsync(id, 5); // eslint-disable-line no-await-in-loop
        console.log("TIMEOUT1");  // eslint-disable-line no-console 
        await timeout(250); // eslint-disable-line no-await-in-loop
        console.log("LED:", id, 7);  // eslint-disable-line no-console 
        await om.OmSetLedAsync(id, 7); // eslint-disable-line no-await-in-loop
        console.log("TIMEOUT2");  // eslint-disable-line no-console 
        await timeout(250); // eslint-disable-line no-await-in-loop
    }
    // Auto
    console.log("RESET...");  // eslint-disable-line no-console 
    await om.OmSetLedAsync(id, -1);
}
*/

setTimeout(function() {
    console.log("60s timeout expired");
    console.log("OmShutdown()");
    om.OmShutdown();
}, 60 * 1000);


/*
if (deviceIds.length > 0) {
    doIdentify(deviceIds[0]);
}
*/

start();
