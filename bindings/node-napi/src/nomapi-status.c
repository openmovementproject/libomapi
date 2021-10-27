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


// Open Movement API - Node N-API Wrapper
// Dan Jackson, 2018

#include "interface.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// OM_EXPORT int OmGetVersion(int deviceId, int *firmwareVersion, int *hardwareVersion);
MYNAPI_FUNCTION(OmGetVersion) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->data = malloc(2 * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    int *params = (int *)state->data;
    MYNAPI_RESULT(OmGetVersion(MYNAPI_ARG[0].intValue, &params[0], &params[1]));
  }
  mynapi_complete: {
    int *params = (int *)state->data;
    napi_status status;

    status = napi_create_array(state->env, &state->resultValue);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }

    int i;
    for (i = 0; i < 2; i++)
    {
      napi_value val;
      status = napi_create_int32(state->env, params[i], &val);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
      status = napi_set_element(state->env, state->resultValue, i, val);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
    }

    MYNAPI_COMPLETE_RESULT();
    free(state->data);
  }
}

// OM_EXPORT int OmGetDeviceSerial(int deviceId, char *serialBuffer);
MYNAPI_FUNCTION(OmGetDeviceSerial) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->resultStr = (char *)malloc(OM_MAX_PATH + 1);
    state->resultStr[0] = '\0';
    MYNAPI_RESULT(OmGetDeviceSerial(MYNAPI_ARG[0].intValue, state->resultStr));
  }
  mynapi_complete: {
    MYNAPI_COMPLETE_RESULT();
    free(state->resultStr);
  }
}

// OM_EXPORT int OmGetDevicePort(int deviceId, char *portBuffer);
MYNAPI_FUNCTION(OmGetDevicePort) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->resultStr = (char *)malloc(OM_MAX_PATH + 1);
    state->resultStr[0] = '\0';
    MYNAPI_RESULT(OmGetDevicePort(MYNAPI_ARG[0].intValue, state->resultStr));
  }
  mynapi_complete: {
    MYNAPI_COMPLETE_RESULT();
    free(state->resultStr);
  }
}

// OM_EXPORT int OmGetDevicePath(int deviceId, char *pathBuffer);
MYNAPI_FUNCTION(OmGetDevicePath) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->resultStr = (char *)malloc(OM_MAX_PATH + 1);
    state->resultStr[0] = '\0';
    MYNAPI_RESULT(OmGetDevicePath(MYNAPI_ARG[0].intValue, state->resultStr));
  }
  mynapi_complete: {
    MYNAPI_COMPLETE_RESULT();
    free(state->resultStr);
  }
}

// OM_EXPORT int OmGetBatteryLevel(int deviceId);
MYNAPI_FUNCTION(OmGetBatteryLevel) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmGetBatteryLevel(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmSelfTest(int deviceId);
MYNAPI_FUNCTION(OmSelfTest) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSelfTest(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetMemoryHealth(int deviceId);
MYNAPI_FUNCTION(OmGetMemoryHealth) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmGetMemoryHealth(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetBatteryHealth(int deviceId);
MYNAPI_FUNCTION(OmGetBatteryHealth) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmGetBatteryHealth(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetAccelerometer(int deviceId, int *x, int *y, int *z);
MYNAPI_FUNCTION(OmGetAccelerometer) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->data = malloc(3 * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    int *params = (int *)state->data;
    MYNAPI_RESULT(OmGetAccelerometer(MYNAPI_ARG[0].intValue, &params[0], &params[1], &params[2]));
  }
  mynapi_complete: {
    int *params = (int *)state->data;
    napi_status status;

    status = napi_create_array(state->env, &state->resultValue);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }

    int i;
    for (i = 0; i < 3; i++)
    {
      napi_value val;
      status = napi_create_int32(state->env, params[i], &val);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
      status = napi_set_element(state->env, state->resultValue, i, val);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
    }

    MYNAPI_COMPLETE_RESULT();
    free(state->data);
  }
}

// OM_EXPORT int OmGetTime(int deviceId, OM_DATETIME *time);
MYNAPI_FUNCTION(OmGetTime) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     {
    OM_DATETIME time;
    MYNAPI_RESULT(OmGetTime(MYNAPI_ARG[0].intValue, &time));
    MYNAPI_RESULT_FULL_INT(time);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmSetTime(int deviceId, OM_DATETIME time);
MYNAPI_FUNCTION(OmSetTime) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetTime(MYNAPI_ARG[0].intValue, (OM_DATETIME)MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmSetLed(int deviceId, OM_LED_STATE ledState);
MYNAPI_FUNCTION(OmSetLed) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetLed(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmIsLocked(int deviceId, int *hasLockCode);
MYNAPI_FUNCTION(OmIsLocked) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->data = malloc(1 * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    int *params = (int *)state->data;
    MYNAPI_RESULT(OmIsLocked(MYNAPI_ARG[0].intValue, &params[0]));
  }
  mynapi_complete: {
    int *params = (int *)state->data;
    napi_status status;

    status = napi_create_array(state->env, &state->resultValue);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }

    int i;
    for (i = 0; i < 1; i++)
    {
      napi_value val;
      status = napi_create_int32(state->env, params[i], &val);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
      status = napi_set_element(state->env, state->resultValue, i, val);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
    }

    MYNAPI_COMPLETE_RESULT();
    free(state->data);
  }
}

// OM_EXPORT int OmSetLock(int deviceId, unsigned short code);
MYNAPI_FUNCTION(OmSetLock) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetLock(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmUnlock(int deviceId, unsigned short code);
MYNAPI_FUNCTION(OmUnlock) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmUnlock(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmSetEcc(int deviceId, int state);
MYNAPI_FUNCTION(OmSetEcc) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetEcc(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetEcc(int deviceId);
MYNAPI_FUNCTION(OmGetEcc) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmGetEcc(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmCommand(int deviceId, const char *command, char *buffer, size_t bufferSize, const char *expected, unsigned int timeoutMs, char **parseParts, int parseMax);
// TODO: implement


void CleanupStatus(napi_env env)
{
  ;
}

napi_value InitStatus(napi_env env, napi_value exports)
{
  napi_status status;

  // OM_MEMORY_HEALTH
  EXPORT_INT(OM_MEMORY_HEALTH_ERROR);
  EXPORT_INT(OM_MEMORY_HEALTH_WARNING);
  // OM_LED_STATE
  EXPORT_INT(OM_LED_AUTO);
  EXPORT_INT(OM_LED_OFF);
  EXPORT_INT(OM_LED_BLUE);
  EXPORT_INT(OM_LED_GREEN);
  EXPORT_INT(OM_LED_CYAN);
  EXPORT_INT(OM_LED_RED);
  EXPORT_INT(OM_LED_MAGENTA);
  EXPORT_INT(OM_LED_YELLOW);
  EXPORT_INT(OM_LED_WHITE);

  // --- FUNCTIONS ---
  EXPORT_FUNCTION(OmGetVersion);
  EXPORT_FUNCTION(OmGetDeviceSerial);
  EXPORT_FUNCTION(OmGetDevicePort);
  EXPORT_FUNCTION(OmGetDevicePath);
  EXPORT_FUNCTION(OmGetBatteryLevel);
  EXPORT_FUNCTION(OmSelfTest);
  EXPORT_FUNCTION(OmGetMemoryHealth);
  EXPORT_FUNCTION(OmGetBatteryHealth);
  EXPORT_FUNCTION(OmGetAccelerometer);
  EXPORT_FUNCTION(OmGetTime);
  EXPORT_FUNCTION(OmSetTime);
  EXPORT_FUNCTION(OmSetLed);
  EXPORT_FUNCTION(OmIsLocked);
  EXPORT_FUNCTION(OmSetLock);
  EXPORT_FUNCTION(OmUnlock);
  EXPORT_FUNCTION(OmSetEcc);
  EXPORT_FUNCTION(OmGetEcc);
  //EXPORT_FUNCTION(OmCommand);
  // ---------------

  return exports;
}
