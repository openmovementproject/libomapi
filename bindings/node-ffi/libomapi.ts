/* 
 * Copyright (c) 2009-, Newcastle University, UK.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met: 
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE. 
 */

// Node JavaScript Open Movement API
// Wrapper for "libomapi", see "omapi.h" for more detailed documentation.
// Dan Jackson



//  tsc && node test.js

// Access static properties from object instance:
//   tsc --lib es2015 libomapi.ts && node -e "libomapi = require('./libomapi'); let om = new libomapi.Om(); console.log(om.constructor.OM_DEVICE_STATUS.OM_DEVICE_CONNECTED);"

// Timezone checking:
//   TZ='America/New_York' nodejs
//   libomapi = require('./libomapi');
//   // om = new libomapi.Om(true);
//   t = libomapi.Om.OM_DATETIME_FROM_YMDHMS(2063, 12, 31, 23, 59, 59);
//   t == libomapi.Om.OM_DATETIME_MAX_VALID
//   e = libomapi.Om.OmDateTimeUnpackEpoch(t);
//   Dutc = libomapi.Om.OmDateTimeUnpackUTC(t);
//   Dlocal = libomapi.Om.OmDateTimeUnpackLocal(t);
//   t == libomapi.Om.OmDateTimePackEpoch(e);
//   t == libomapi.Om.OmDateTimePackUTC(Dutc);
//   t == libomapi.Om.OmDateTimePackLocal(Dlocal);
//   t == libomapi.Om.OmDateTimePackUTC(libomapi.Om.LocalDateReinterpretAsUTC(Dlocal));
//   t == libomapi.Om.OmDateTimePackEpoch(libomapi.Om.LocalDateReinterpretAsEpoch(Dlocal));

// / Error checking wrapper -- throws type
// / Date/time wrappers (always convert to JavaScript Date()?)
// Missing config commands
// Async. wrappers



// Node Foreign Function Interface (moved to dynamically require when instantiating)
// import * as FFI from 'ffi';
// import * as ref from 'ref';
// import * as ArrayType from 'ref-array';
// import * as Struct from 'ref-struct';

export enum OM_DEVICE_STATUS { 
    OM_DEVICE_REMOVED = 0, 
    OM_DEVICE_CONNECTED = 1,
};

export enum OM_DOWNLOAD_STATUS {
    OM_DOWNLOAD_NONE = 0,
    OM_DOWNLOAD_ERROR = 1,
    OM_DOWNLOAD_PROGRESS = 2,
    OM_DOWNLOAD_COMPLETE = 3,
    OM_DOWNLOAD_CANCELLED = 4,
};

export enum OM_RETURN_VALUE { 
    OM_TRUE  = 1,
    OM_FALSE = 0,
    OM_OK                    = 0,
    OM_E_FAIL                = -1,
    OM_E_UNEXPECTED          = -2,
    OM_E_NOT_VALID_STATE     = -3,
    OM_E_OUT_OF_MEMORY       = -4,
    OM_E_INVALID_ARG         = -5,
    OM_E_POINTER             = -6,
    OM_E_NOT_IMPLEMENTED     = -7,
    OM_E_ABORT               = -8,
    OM_E_ACCESS_DENIED       = -9,
    OM_E_UNEXPECTED_RESPONSE = -11,
    OM_E_LOCKED              = -12,
};
//OM_SUCCEEDED(value) ((value) >= 0)
//OM_FAILED(value) ((value) < 0)    
export type OM_RETURN = number|OM_RETURN_VALUE;

export enum OM_LED_STATE {
    OM_LED_AUTO    = -1,
    OM_LED_OFF     = 0,
    OM_LED_BLUE    = 1,
    OM_LED_GREEN   = 2,
    OM_LED_CYAN    = 3,
    OM_LED_RED     = 4,
    OM_LED_MAGENTA = 5,
    OM_LED_YELLOW  = 6,
    OM_LED_WHITE   = 7,
};

export enum OM_ERASE_LEVEL {
	OM_ERASE_NONE        = 0, 
	OM_ERASE_DELETE      = 1, 
	OM_ERASE_QUICKFORMAT = 2, 
	OM_ERASE_WIPE        = 3, 
};

export type OM_DATETIME = number;

export type OmLogCallback = (msg: string) => void;
export type OmDeviceCallback = (did: number, stat: OM_DEVICE_STATUS) => void;
export type OmDownloadCallback = (did: number, stat: OM_DOWNLOAD_STATUS, value: number) => void;

export type OmVersionType = { firmwareVersion: number, hardwareVersion: number };

export class OmError extends Error {
    //public value: OM_RETURN_VALUE;
    public constructor(message: string) {
        super(message)
    }
}

export class Om {

	// Dynamically require() when instantiating
    private FFI: any = null;
    private ref: any = null;
    private ArrayType: any = null;
    // private Struct: any = null;

    libraryFilename: string;
    private lib: any = null; //FFI.Library;

    logCallback: OmLogCallback;
    deviceCallback: OmDeviceCallback;
    downloadCallback: OmDownloadCallback;

	// --- Bring OM enums values in to the class for convenience ---
	public static OM_DEVICE_STATUS = {
		OM_DEVICE_REMOVED           : OM_DEVICE_STATUS.OM_DEVICE_REMOVED, 
		OM_DEVICE_CONNECTED         : OM_DEVICE_STATUS.OM_DEVICE_CONNECTED,
    };
    public static OM_DOWNLOAD_STATUS = {
        OM_DOWNLOAD_NONE            : OM_DOWNLOAD_STATUS.OM_DOWNLOAD_NONE,
        OM_DOWNLOAD_ERROR           : OM_DOWNLOAD_STATUS.OM_DOWNLOAD_ERROR,
        OM_DOWNLOAD_PROGRESS        : OM_DOWNLOAD_STATUS.OM_DOWNLOAD_PROGRESS,
        OM_DOWNLOAD_COMPLETE        : OM_DOWNLOAD_STATUS.OM_DOWNLOAD_COMPLETE,
        OM_DOWNLOAD_CANCELLED       : OM_DOWNLOAD_STATUS.OM_DOWNLOAD_CANCELLED,
    };
	public static OM_RETURN_VALUE = {
		OM_TRUE                     : OM_RETURN_VALUE.OM_TRUE, 
		OM_FALSE                    : OM_RETURN_VALUE.OM_FALSE, 
		OM_OK                       : OM_RETURN_VALUE.OM_OK, 
		OM_E_FAIL                   : OM_RETURN_VALUE.OM_E_FAIL, 
		OM_E_UNEXPECTED             : OM_RETURN_VALUE.OM_E_UNEXPECTED, 
		OM_E_NOT_VALID_STATE        : OM_RETURN_VALUE.OM_E_NOT_VALID_STATE, 
		OM_E_OUT_OF_MEMORY          : OM_RETURN_VALUE.OM_E_OUT_OF_MEMORY, 
		OM_E_INVALID_ARG            : OM_RETURN_VALUE.OM_E_INVALID_ARG, 
		OM_E_POINTER                : OM_RETURN_VALUE.OM_E_POINTER, 
		OM_E_NOT_IMPLEMENTED        : OM_RETURN_VALUE.OM_E_NOT_IMPLEMENTED, 
		OM_E_ABORT                  : OM_RETURN_VALUE.OM_E_ABORT, 
		OM_E_ACCESS_DENIED          : OM_RETURN_VALUE.OM_E_ACCESS_DENIED, 
		OM_E_UNEXPECTED_RESPONSE    : OM_RETURN_VALUE.OM_E_UNEXPECTED_RESPONSE, 
		OM_E_LOCKED                 : OM_RETURN_VALUE.OM_E_LOCKED, 
	};
	public static OM_LED_STATE = {
		OM_LED_AUTO                 : OM_LED_STATE.OM_LED_AUTO, 
		OM_LED_OFF                  : OM_LED_STATE.OM_LED_OFF, 
		OM_LED_BLUE                 : OM_LED_STATE.OM_LED_BLUE, 
		OM_LED_GREEN                : OM_LED_STATE.OM_LED_GREEN, 
		OM_LED_CYAN                 : OM_LED_STATE.OM_LED_CYAN, 
		OM_LED_RED                  : OM_LED_STATE.OM_LED_RED, 
		OM_LED_MAGENTA              : OM_LED_STATE.OM_LED_MAGENTA, 
		OM_LED_YELLOW               : OM_LED_STATE.OM_LED_YELLOW, 
		OM_LED_WHITE                : OM_LED_STATE.OM_LED_WHITE, 
	};
	public static OM_ERASE_LEVEL = {
		OM_ERASE_NONE               : OM_ERASE_LEVEL.OM_ERASE_NONE, 
		OM_ERASE_DELETE             : OM_ERASE_LEVEL.OM_ERASE_DELETE, 
		OM_ERASE_QUICKFORMAT        : OM_ERASE_LEVEL.OM_ERASE_QUICKFORMAT, 
		OM_ERASE_WIPE               : OM_ERASE_LEVEL.OM_ERASE_WIPE, 
	};
	// ---------
	
    public static OM_VERSION: number = 107; // V1.07
    public static OM_MAX_PATH: number = 256;

    public static OM_METADATA_SIZE: number = 448;
    public static OM_ACCEL_DEFAULT_RATE: number = 100;
    public static OM_ACCEL_DEFAULT_RANGE: number = 8;

    // Error check
	private OM_ASSERT_INITIALIZED(): void {
		if (this.lib === null) {
			throw new OmError("OM error: dynamic library not loaded.");
		}
	}
    private static OM_ASSERT_SUCCEEDED_VALUE(value: OM_RETURN): number {
        if (value >= 0) {
            return value;
        }
        throw new OmError("OM error (" + value + ") " + OM_RETURN_VALUE[value]);
    }
    private static OM_ASSERT_SUCCEEDED_OR_REJECT(value: OM_RETURN, reject: any): boolean {
        if (value < 0) {
            reject(new OmError("OM error (" + value + ") " + OM_RETURN_VALUE[value]));
            return false;
        }
        return true;
    }

 	public static OM_DATETIME_FROM_YMDHMS(year: number, month: number, day: number, hours: number, minutes: number, seconds: number): OM_DATETIME { 
        let value: OM_DATETIME;
		// NOTE: Year part changed from bit-shift-left/or to multiply/add to avoid a signed 32-bit interger overflow
        value = ((((year % 100) & 0x3f) * (1<<26)) + (((month & 0x0f) << 22) | ((day & 0x1f) << 17) | ((hours & 0x1f) << 12) | ((minutes & 0x3f) << 6) | (seconds & 0x3f)));
        return value;
	}

	// NOTE: Year part changed from bit-shift-right to divide/floor to avoid a signed 32-bit interger overflow
	public static OM_DATETIME_YEAR(dateTime: OM_DATETIME): number    { return (Math.floor(dateTime / (1<<26)) & 0x3f) + 2000; }
	public static OM_DATETIME_MONTH(dateTime: OM_DATETIME): number   { return ((dateTime >> 22) & 0x0f); }
	public static OM_DATETIME_DAY(dateTime: OM_DATETIME): number     { return ((dateTime >> 17) & 0x1f); }
	public static OM_DATETIME_HOURS(dateTime: OM_DATETIME): number   { return ((dateTime >> 12) & 0x1f); }
	public static OM_DATETIME_MINUTES(dateTime: OM_DATETIME): number { return ((dateTime >>  6) & 0x3f); }
	public static OM_DATETIME_SECONDS(dateTime: OM_DATETIME): number { return ((dateTime      ) & 0x3f); }

	public static OM_DATETIME_ZERO: OM_DATETIME = 0x00000000;
	public static OM_DATETIME_INFINITE: OM_DATETIME = 0xffffffff;
	// NOTE: Year part changed from bit-shift-left/or to multiply/add to avoid a signed 32-bit interger overflow
	public static OM_DATETIME_MIN_VALID: OM_DATETIME = (((( 0) & 0x3f) * (1<<26)) + (((( 1) & 0x0f) << 22) | ((( 1) & 0x1f) << 17) | ((( 0) & 0x1f) << 12) | ((( 0) & 0x3f) << 6) | ((( 0) & 0x3f))));
	public static OM_DATETIME_MAX_VALID: OM_DATETIME = ((((63) & 0x3f) * (1<<26)) + ((((12) & 0x0f) << 22) | (((31) & 0x1f) << 17) | (((23) & 0x1f) << 12) | (((59) & 0x3f) << 6) | (((59) & 0x3f))));

	public static OM_JSDATE_ZERO = new Date(1999, 0); // new Date(Date.UTC(1999, 12-1, 31, 23, 59, 59, 999));
	public static OM_JSDATE_INFINITE = new Date(2065, 0); // new Date(Date.UTC(2064, 1-1, 1, 0, 0, 0, 0));
	
	public static OM_JSDATE_LOCAL_ZERO = new Date(Date.UTC(1999, 12-1, 31, 23, 59, 59, 999));
	public static OM_JSDATE_LOCAL_INFINITE = new Date(Date.UTC(2064, 1-1, 1, 0, 0, 0, 0));
	
	// Convert to JavaScript as seconds since the Unix epoch (will be an epoch with unspecified time zone)
	public static OmDateTimeUnpackEpoch(value: OM_DATETIME, fractional: number = 0): number {
		if (value < Om.OM_DATETIME_MIN_VALID) { return -8640000000000; }  // Date.Minimum / 1000
		if (value > Om.OM_DATETIME_MAX_VALID) { return 8640000000000; }   // Date.Maximum / 1000
        let year = Om.OM_DATETIME_YEAR(value);
        let month = Om.OM_DATETIME_MONTH(value);
        let day = Om.OM_DATETIME_DAY(value);
        let hours = Om.OM_DATETIME_HOURS(value);
        let minutes = Om.OM_DATETIME_MINUTES(value);
        let seconds = Om.OM_DATETIME_SECONDS(value);
        let milliseconds = 1000 * fractional / 0x10000; // 'fractional' device units
        return Date.UTC(year, month - 1, day, hours, minutes, seconds, milliseconds) / 1000.0;
	}

	// Convert to JavaScript Date (interpreting as UTC time)
	public static OmDateTimeUnpackUTC(value: OM_DATETIME, fractional: number = 0): Date {
		let epochSeconds = this.OmDateTimeUnpackEpoch(value, fractional);
        return new Date(1000 * epochSeconds);
	}

	// Convert to JavaScript Date (in local time zone)
	// NOTE: Not recommended, packed times are in an unspecified timezone and should be represented without reference to a time zone (e.g. even if the zone matched, DST will cause incorrect times to be shown).
	public static OmDateTimeUnpackLocal(value: OM_DATETIME, fractional: number = 0): Date {
		if (value < Om.OM_DATETIME_MIN_VALID) { return new Date(-8640000000000000); }  // Date.Minimum
		if (value > Om.OM_DATETIME_MAX_VALID) { return new Date(8640000000000000); }   // Date.Maximum
        let year = Om.OM_DATETIME_YEAR(value);
        let month = Om.OM_DATETIME_MONTH(value);
        let day = Om.OM_DATETIME_DAY(value);
        let hours = Om.OM_DATETIME_HOURS(value);
        let minutes = Om.OM_DATETIME_MINUTES(value);
        let seconds = Om.OM_DATETIME_SECONDS(value);
        let milliseconds = 1000 * fractional / 0x10000; // 'fractional' device units
        return new Date(year, month - 1, day, hours, minutes, seconds, milliseconds);
	}
	
	// Convert to packed date/time using UTC values
	public static OmDateTimePackUTC(value: Date): OM_DATETIME {
		if (value.getUTCFullYear() < 2000) { return Om.OM_DATETIME_ZERO; }
		if (value.getUTCFullYear() >= 2064) { return Om.OM_DATETIME_INFINITE; }
		return Om.OM_DATETIME_FROM_YMDHMS(
            value.getUTCFullYear(), 
            value.getUTCMonth() + 1, 
            value.getUTCDate(), 
            value.getUTCHours(), 
            value.getUTCMinutes(), 
            value.getUTCSeconds()
        );
	}
	
	// Convert to packed date/time (from Unix epoch)
	public static OmDateTimePackEpoch(epochSeconds: number): OM_DATETIME {
		let value = new Date(1000 * epochSeconds);
		return this.OmDateTimePackUTC(value);
	}
	
	// Convert to packed date/time using local values
	public static OmDateTimePackLocal(value: Date): OM_DATETIME {
		if (value.getFullYear() < 2000) { return Om.OM_DATETIME_ZERO; }
		if (value.getFullYear() >= 2064) { return Om.OM_DATETIME_INFINITE; }
		return Om.OM_DATETIME_FROM_YMDHMS(
            value.getFullYear(), 
            value.getMonth() + 1, 
            value.getDate(), 
            value.getHours(), 
            value.getMinutes(), 
            value.getSeconds()
        );
	}
	
	
	// Re-interpret (NOT convert with time zone offset) a local date/time's components as a Unix epoch seconds.
	// (to represent OM date/times which have no time zone information)
	public static LocalDateReinterpretAsEpoch(value: Date): number {
        return Date.UTC(
			value.getFullYear(), 
			value.getMonth(), 
			value.getDate(),
			value.getHours(),
			value.getMinutes(),
			value.getSeconds(), 
			value.getMilliseconds()
		) / 1000;
	}
	
	
	// Re-interpret (NOT convert with time zone offset) a local date/time's components as a UTC date/time
	// (where UTC date/times are used as a substitute for OM date/times which have no time zone information)
	public static LocalDateReinterpretAsUTC(value: Date): Date {
        return new Date(1000 * this.LocalDateReinterpretAsEpoch(value));
	}
	
	
    // NULL-terminated char array to string 
    private static BufferToString(buf: Buffer): string {
        let len = 0;
        for (let i = 0; i < buf.length; i++, len++) {
            if (buf[i] == 0) { break; }
        }
        let value = buf.toString('utf8', 0, len);
        return value;
    }

    // String to NULL-terminated char array 
    private static StringToBuffer(str: string, bufferSize?: number): Buffer {
        let source = Buffer.from(str, 'utf8');
        let len = source.length + 1;
        if (bufferSize) {
            len = bufferSize;
        }
        let buf = new Buffer(len);
        for (let i = 0; i < len; i++) {
            if (i < source.length) {
                buf[i] = source[i];
            } else {
                buf[i] = 0;
            }
        }
        return buf;
    }

    constructor(allowLoadFailure = false) {
        // Library file name
        //if (process.arch == 'x64')
        this.libraryFilename = __dirname + '/lib/binding/libomapi';

        // Library imports
        this.lib = null;
		
		try {
			// Node Foreign Function Interface 
			this.FFI = require('ffi');
			this.ref = require('ref');
			this.ArrayType = require('ref-array')
			// this.Struct = require('ref-struct');

			this.lib = new this.FFI.Library(this.libraryFilename, {
				'OmStartup': [ 'int', [ 'int' ] ],
				'OmShutdown': [ 'int', [ ] ],
				
				'OmSetLogCallback': [ 'int', [ 'pointer', 'pointer' ] ],

				'OmSetDeviceCallback': [ 'int', [ 'pointer', 'pointer' ] ],
                'OmGetDeviceIds': [ 'int', [ 'pointer', 'int' ] ], // this.ArrayType(this.ref.types.int)
                
                'OmSetDownloadCallback': [ 'int', [ 'pointer', 'pointer' ] ],
                'OmBeginDownloading': [ 'int', [ 'int', 'int', 'int', this.ref.refType(this.ref.types.char) ] ],
                'OmCancelDownload': [ 'int', [ 'int' ] ],
                
                'OmSetSessionId': [ 'int', [ 'int', 'int' ] ],
                'OmSetDelays': [ 'int', [ 'int', 'int', 'int' ] ],
                'OmSetMetadata': [ 'int', [ 'int', this.ref.refType(this.ref.types.char), 'int' ] ],
                'OmSetAccelConfig': [ 'int', [ 'int', 'int', 'int' ] ],
                'OmSetMaxSamples': [ 'int', [ 'int', 'int' ] ],
                'OmEraseDataAndCommit': [ 'int', [ 'int', 'int' ] ],
				'OmGetBatteryLevel': [ 'int', [ 'int' ] ],
				'OmGetTime': [ 'int', [ this.ref.refType('int') ] ],
				'OmSetTime': [ 'int', [ 'int', 'int' ] ],
				'OmSetLed': [ 'int', [ 'int', 'int' ] ],
				'OmGetVersion': [ 'int', [ 'int', this.ref.refType('int'), this.ref.refType('int') ] ],

				'OmGetDataFilename': [ 'int', [ 'int', this.ref.refType(this.ref.types.char)] ],
				'OmGetDevicePort': [ 'int', [ 'int', this.ref.refType(this.ref.types.char)] ],
				'OmGetDevicePath': [ 'int', [ 'int', this.ref.refType(this.ref.types.char)] ],

			});
			
		} catch (e) {
			
			// "Win32 error" see https://msdn.microsoft.com/en-us/library/windows/desktop/ms681382(v=vs.85).aspx
			// let win32Errors = { 
			//     126: "ERROR_MOD_NOT_FOUND: The specified module could not be found.",
			//     127: "ERROR_PROC_NOT_FOUND: The specified procedure could not be found.",
			//     193: "ERROR_BAD_EXE_FORMAT is not a valid Win32 application",
			// };
			
			console.log("ERROR: " + e.message + " when loading: " + this.libraryFilename + "{.dll|.dylib|.so}");
			
			if (e.message == "Dynamic Linking Error: Win32 error 126") {
				console.log("ERROR: ERROR_MOD_NOT_FOUND: The specified module could not be found.");
				console.log("ERROR: Cannot find library: " + this.libraryFilename + ".dll");
			} else if (e.message == "Dynamic Linking Error: Win32 error 127") {
				console.log("ERROR: ERROR_PROC_NOT_FOUND: The specified procedure could not be found.");
				console.log("ERROR: Possible problem with library build: " + this.libraryFilename + ".dll");
			} else if (e.message == "Dynamic Linking Error: Win32 error 193") {
				console.log("ERROR: ERROR_BAD_EXE_FORMAT is not a valid Win32 application");
				console.log("ERROR: Possibly incorrect architecture (currently " + process.arch + ") in library: " + this.libraryFilename + ".dll");
			}
			
			if (!allowLoadFailure) {
				throw e;
			}
		}

    }

    OmStartup(version: number = Om.OM_VERSION) {
		this.OM_ASSERT_INITIALIZED();
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmStartup(version));
    }

    // Experimental to see if this fixes deadlock on Mac with initial devices
    async OmStartupAsync(version: number = Om.OM_VERSION) {
		this.OM_ASSERT_INITIALIZED();
        return new Promise<void>((resolve, reject) => {
            this.lib.OmStartup.async(version, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }

    OmShutdown() {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmShutdown());
    }

    OmSetLogCallback(callback: OmLogCallback) {
        this.logCallback = this.FFI.Callback('void', [ this.ref.refType(this.ref.types.void), 'string'], function(reference: any, message: string) {
            console.log("OMLOGCALLBACK: " + message);
            callback(message);
        });
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetLogCallback(this.logCallback, null));
    }

    OmSetDeviceCallback(callback: OmDeviceCallback) {
        this.deviceCallback = this.FFI.Callback('void', [ this.ref.refType(this.ref.types.void), 'int', 'int'], function(reference: any, deviceId: number, inStatus: number) {
            let status: OM_DEVICE_STATUS = <OM_DEVICE_STATUS>inStatus;      // (cast not actually required)
            console.log("OMDEVICECALLBACK: deviceId=" + deviceId + " status=" + OM_DEVICE_STATUS[status] + "");
            callback(deviceId, status);
        });
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetDeviceCallback(this.deviceCallback, null));
    }

    OmGetDeviceIds(): number[] {
        let maxDevices = 4096;
        let outDeviceIds = new Buffer(maxDevices * 4);
console.log("A");
        let numDevices = Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmGetDeviceIds(outDeviceIds, maxDevices));
        if (numDevices > maxDevices) { numDevices = maxDevices; }

        let deviceIds = new Array<number>(numDevices);
        for (let i = 0; i < numDevices; i++) {
            deviceIds[i] = outDeviceIds.readInt32LE(i);
        }

        return deviceIds; 
    }

    OmSetDownloadCallback(callback: OmDownloadCallback) {
        this.downloadCallback = this.FFI.Callback('void', [ this.ref.refType(this.ref.types.void), 'int', 'int', 'int'], function(reference: any, deviceId: number, inStatus: number, inValue: number) {
            let status: OM_DOWNLOAD_STATUS = <OM_DOWNLOAD_STATUS>inStatus;      // (cast not actually required)
            console.log("OMDOWNLOADCALLBACK: dId=" + deviceId + " stat=" + OM_DOWNLOAD_STATUS[status] + " val=" + inValue);
            callback(deviceId, status, inValue);
        });
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetDownloadCallback(this.downloadCallback, null));
    }

    OmBeginDownloading(deviceId: number, dataOffsetBlocks: number, dataLengthBlocks: number, destinationFile: string) {
        let destinationFileBuffer = Om.StringToBuffer(destinationFile);
        const retVal = this.lib.OmBeginDownloading(
            deviceId, dataOffsetBlocks, dataLengthBlocks, destinationFileBuffer
        );
        Om.OM_ASSERT_SUCCEEDED_VALUE(retVal);
    }

    async OmBeginDownloadingAsync(deviceId: number, dataOffsetBlocks: number, dataLengthBlocks: number, destinationFile: string): Promise<void> {
        let destinationFileBuffer = Om.StringToBuffer(destinationFile);
        return new Promise<void>((resolve, reject) => {
            this.lib.OmBeginDownloading.async(
                deviceId, dataOffsetBlocks, dataLengthBlocks, destinationFileBuffer, 
                (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }
    
    OmCancelDownload(deviceId: number) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmCancelDownload(deviceId));
    }

    async OmCancelDownloadAsync(deviceId: number): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmCancelDownload.async(deviceId, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }
    

    OmGetBatteryLevel(deviceId: number): number {
        return Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmGetBatteryLevel(deviceId));
    }

    async OmGetBatteryLevelAsync(deviceId: number): Promise<number> {
        return new Promise<number>((resolve, reject) => {
            this.lib.OmGetBatteryLevel.async(deviceId, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve(res);
                }
            });
        });
    }


    OmGetTime(deviceId: number): OM_DATETIME {
        let outTime = this.ref.alloc('int');
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmGetTime(deviceId, outTime));
        let time = outTime.deref();
        return time;
    }

    async OmGetTimeAsync(deviceId: number): Promise<OM_DATETIME> {
        return new Promise<OM_DATETIME>((resolve, reject) => {
            let outTime = this.ref.alloc('int');
            this.lib.OmGetTime.async(deviceId, outTime, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    let time = outTime.deref();
                    resolve(time);
                }
            });
        });
    }


    OmSetTime(deviceId: number, time: OM_DATETIME) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetTime(deviceId, time));
    }

    async OmSetTimeAsync(deviceId: number, time: OM_DATETIME): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmSetTime.async(deviceId, time, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }


    OmSetLed(deviceId: number, led: OM_LED_STATE) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetLed(deviceId, led));
    }

    async OmSetLedAsync(deviceId: number, led: OM_LED_STATE): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmSetLed.async(deviceId, led, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }

	OmGetVersion(deviceId: number): OmVersionType {
        let outFirmwareVersion = this.ref.alloc('int');
        let outHardwareVersion = this.ref.alloc('int');
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmGetVersion(deviceId, outFirmwareVersion, outHardwareVersion));
        let firmwareVersion = outFirmwareVersion.deref();
        let hardwareVersion = outHardwareVersion.deref();
        return {
            firmwareVersion: firmwareVersion,
            hardwareVersion: hardwareVersion,
        };
    }

    async OmGetVersionAsync(deviceId: number): Promise<OmVersionType> {
        return new Promise<OmVersionType>((resolve, reject) => {
            let outFirmwareVersion = this.ref.alloc('int');
            let outHardwareVersion = this.ref.alloc('int');
            this.lib.OmGetVersion.async(deviceId, outFirmwareVersion, outHardwareVersion, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    let firmwareVersion = outFirmwareVersion.deref();
                    let hardwareVersion = outHardwareVersion.deref();
                    resolve({
						firmwareVersion: firmwareVersion,
						hardwareVersion: hardwareVersion,
					});
                }
            });
        });
    }


    OmSetSessionId(deviceId: number, sessionId: number) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetSessionId(deviceId, sessionId));
    }

    async OmSetSessionIdAsync(deviceId: number, sessionId: number): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmSetSessionId.async(deviceId, sessionId, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }


    OmSetDelays(deviceId: number, startTime: OM_DATETIME, stopTime: OM_DATETIME) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetDelays(deviceId, startTime, stopTime));
    }

    async OmSetDelaysAsync(deviceId: number, startTime: OM_DATETIME, stopTime: OM_DATETIME): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmSetDelays.async(deviceId, startTime, stopTime, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }


    OmSetMetadata(deviceId: number, metadata: string) {
        let metadataBuffer = Om.StringToBuffer(metadata);
        let size = metadataBuffer.length;
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetMetadata(deviceId, metadataBuffer, size));
    }

    OmSetMetadataAsync(deviceId: number, metadata: string): Promise<void> {
        let metadataBuffer = Om.StringToBuffer(metadata);
        let size = metadataBuffer.length;
        return new Promise<void>((resolve, reject) => {
            this.lib.OmSetMetadata.async(deviceId, metadataBuffer, size, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }


    OmSetAccelConfig(deviceId: number, rate: number, range: number) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetAccelConfig(deviceId, rate, range));
    }

    async OmSetAccelConfigAsync(deviceId: number, rate: number, range: number): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmSetAccelConfig.async(deviceId, rate, range, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }


    OmSetMaxSamples(deviceId: number, maxSamples: number) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmSetMaxSamples(deviceId, maxSamples));
    }

    async OmSetMaxSamplesAsync(deviceId: number, maxSamples: number): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmSetMaxSamples.async(deviceId, maxSamples, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }


    OmEraseDataAndCommit(deviceId: number, eraseLevel: OM_ERASE_LEVEL) {
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmEraseDataAndCommit(deviceId, eraseLevel));
    }

    async OmEraseDataAndCommitAsync(deviceId: number, eraseLevel: OM_ERASE_LEVEL): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this.lib.OmEraseDataAndCommit.async(deviceId, eraseLevel, (err: any, res: number) => {
                if (err) {
                    reject(err);
                } else if (Om.OM_ASSERT_SUCCEEDED_OR_REJECT(res, reject)) {
                    resolve();
                }
            });
        });
    }


    OmGetDataFilename(deviceId: number): string {
        let outFilename = new Buffer(Om.OM_MAX_PATH + 1);
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmGetDataFilename(deviceId, outFilename));
        return Om.BufferToString(outFilename); 
    }

    OmGetDevicePort(deviceId: number): string {
        let devicePort = new Buffer(Om.OM_MAX_PATH + 1);
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmGetDevicePort(deviceId, devicePort));
        return Om.BufferToString(devicePort); 
    }

    OmGetDevicePath(deviceId: number): string {
        let devicePath = new Buffer(Om.OM_MAX_PATH + 1);
        Om.OM_ASSERT_SUCCEEDED_VALUE(this.lib.OmGetDevicePath(deviceId, devicePath));
        return Om.BufferToString(devicePath); 
    }
    
}


