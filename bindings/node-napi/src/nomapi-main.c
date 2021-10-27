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



// OM_EXPORT int OmStartup(int version);
MYNAPI_FUNCTION(OmStartup) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    MYNAPI_RESULT(OmStartup(MYNAPI_ARG[0].intValue));
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}


// OM_EXPORT int OmShutdown(void);
MYNAPI_FUNCTION(OmShutdown) {
  mynapi_params:   MYNAPI_PARAMS();
  mynapi_exec: {
    MYNAPI_RESULT(OmShutdown());

    // Shutdown everything else to release resources
    extern void Cleanup(napi_env env);
    Cleanup(state->env);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}


// OM_EXPORT int OmSetLogStream(int fd);
MYNAPI_FUNCTION(OmSetLogStream) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetLogStream(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}


// OM_EXPORT int OmSetLogCallback(OmLogCallback logCallback, void *reference);
// --- CALLBACK: LOG ---
static callbackState_t logState;

static void logCallbackJSFinalize(napi_env env, void* finalize_data, void* finalize_hint)
{
  free(finalize_data);         // Comment
}

static void logCallbackJS(napi_env env, napi_value js_callback, void *context, void *data)
{
  napi_status status;
  const char *message = data;
  napi_value argv[2];
  napi_value result;

  napi_value thisValue = NULL;
  if (logState.thisObj)
  {
    status = napi_get_reference_value(env, logState.thisObj, &thisValue);
    if (status != napi_ok) { return; }
  }
  else
  {
    napi_get_undefined(env, &thisValue);
  }

  napi_value referenceValue = NULL;
  if (logState.reference)
  {
    status = napi_get_reference_value(env, logState.reference, &referenceValue);
    if (status != napi_ok) { return; }
  }
  else
  {
    napi_get_undefined(env, &referenceValue);
  }
  argv[0] = referenceValue;

  status = napi_create_string_utf8(env, message, NAPI_AUTO_LENGTH, &argv[1]);
  if (status != napi_ok) { return; }

  status = napi_call_function(env, thisValue, js_callback, (int)(sizeof(argv)/sizeof(argv[0])), argv, &result);
  if (status != napi_ok) { return; }
}

//typedef void(*OmLogCallback)(void *, const char *);
void logCallback(void *reference, const char *message)
{
  napi_status status;

  if (logState.threadsafeFunction == NULL) return;

  size_t len = strlen(message) + 1;
  void *data = malloc(len);
  memcpy(data, message, len);

  status = napi_acquire_threadsafe_function(logState.threadsafeFunction);
  if (status != napi_ok) { return; }

  status = napi_call_threadsafe_function(logState.threadsafeFunction, data, napi_tsfn_blocking);
  if (status != napi_ok) { return; }
}

// int OmSetLogCallback(OmLogCallback logCallback, void *reference);
MYNAPI_FUNCTION(OmSetLogCallback) {
  mynapi_params:
     MYNAPI_PARAMS(paramValue, paramValue);
  mynapi_exec: {
    napi_status status;
    napi_value cb = MYNAPI_ARG[0].value;
    
    napi_valuetype valueType;
    status = napi_typeof(state->env, cb, &valueType);
    if (valueType == napi_null || valueType == napi_undefined)    // removing callback?
    {
      if (logState.thisObj != NULL)
      {
        napi_delete_reference(state->env, logState.thisObj);
        logState.thisObj = NULL;
      }

      if (logState.reference != NULL)
      {
        napi_delete_reference(state->env, logState.reference);
        logState.reference = NULL;
      }
    }
    else 
    {
      napi_value resourceName;
      status = napi_create_string_utf8(state->env, "OmLogCallback", NAPI_AUTO_LENGTH, &resourceName);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      status = napi_create_threadsafe_function(state->env, cb, NULL, resourceName, CALLBACK_MAX_QUEUE, 1, NULL, logCallbackJSFinalize, NULL, logCallbackJS, &logState.threadsafeFunction);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      napi_valuetype valueTypeThis;
      status = napi_typeof(state->env, state->self, &valueTypeThis);
      if (valueTypeThis != napi_null && valueTypeThis != napi_undefined)
      {
        status = napi_create_reference(state->env, state->self, 1, &logState.thisObj);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }

      napi_valuetype valueTypeReference;
      status = napi_typeof(state->env, MYNAPI_ARG[1].value, &valueTypeReference);
      if (valueTypeReference != napi_null && valueTypeReference != napi_undefined)
      {
        status = napi_create_reference(state->env, MYNAPI_ARG[1].value, 1, &logState.reference);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }
    }

  }
  mynapi_complete: 
    MYNAPI_COMPLETE_RESULT();
}



// OM_EXPORT int OmSetDeviceCallback(OmDeviceCallback deviceCallback, void *reference);
// --- CALLBACK: DEVICE ---
static callbackState_t deviceState;

typedef struct {
  int id;
  OM_DEVICE_STATUS status;
} deviceCallbackData_t;

static void deviceCallbackJSFinalize(napi_env env, void* finalize_data, void* finalize_hint)
{
  free(finalize_data);         // Comment
}

static void deviceCallbackJS(napi_env env, napi_value js_callback, void *context, void *vData)
{
  napi_status status;
  deviceCallbackData_t *data = (deviceCallbackData_t *)vData;
  napi_value argv[3];
  napi_value result;

  napi_value thisValue = NULL;
  if (deviceState.thisObj)
  {
    status = napi_get_reference_value(env, deviceState.thisObj, &thisValue);
    if (status != napi_ok) { return; }
  }
  else
  {
    napi_get_undefined(env, &thisValue);
  }

  napi_value referenceValue = NULL;
  if (deviceState.reference)
  {
    status = napi_get_reference_value(env, deviceState.reference, &referenceValue);
    if (status != napi_ok) { return; }
  }
  else
  {
    napi_get_undefined(env, &referenceValue);
  }
  argv[0] = referenceValue;

  status = napi_create_int32(env, data->id, &argv[1]);
  if (status != napi_ok) { return; }

  status = napi_create_int32(env, data->status, &argv[2]);
  if (status != napi_ok) { return; }

  status = napi_call_function(env, thisValue, js_callback, (int)(sizeof(argv)/sizeof(argv[0])), argv, &result);
  if (status != napi_ok) { return; }
}

//typedef void(*OmDeviceCallback)(void *, int, OM_DEVICE_STATUS);
static void deviceCallback(void *reference, int id, OM_DEVICE_STATUS deviceStatus)
{
  napi_status status;

  if (deviceState.threadsafeFunction == NULL) return;

  deviceCallbackData_t *data = (deviceCallbackData_t *)malloc(sizeof(deviceCallbackData_t));
  data->id = id;
  data->status = deviceStatus;

  status = napi_acquire_threadsafe_function(deviceState.threadsafeFunction);
  if (status != napi_ok) { return; }

  status = napi_call_threadsafe_function(deviceState.threadsafeFunction, data, napi_tsfn_blocking);
  if (status != napi_ok) { return; }
}

// int OmSetDeviceCallback(OmDeviceCallback deviceCallback, void *reference);
MYNAPI_FUNCTION(OmSetDeviceCallback) {
  mynapi_params:
     MYNAPI_PARAMS(paramValue, paramValue);
  mynapi_exec: {
    napi_status status;
    napi_value cb = MYNAPI_ARG[0].value;
    
    napi_valuetype valueType;
    status = napi_typeof(state->env, cb, &valueType);
    if (valueType == napi_null || valueType == napi_undefined)    // removing callback?
    {
      if (deviceState.thisObj != NULL)
      {
        napi_delete_reference(state->env, deviceState.thisObj);
        deviceState.thisObj = NULL;
      }

      if (deviceState.reference != NULL)
      {
        napi_delete_reference(state->env, deviceState.reference);
        deviceState.reference = NULL;
      }
    }
    else 
    {
      napi_value resourceName;
      status = napi_create_string_utf8(state->env, "OmDeviceCallback", NAPI_AUTO_LENGTH, &resourceName);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      status = napi_create_threadsafe_function(state->env, cb, NULL, resourceName, CALLBACK_MAX_QUEUE, 1, NULL, deviceCallbackJSFinalize, NULL, deviceCallbackJS, &deviceState.threadsafeFunction);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      napi_valuetype valueTypeThis;
      status = napi_typeof(state->env, state->self, &valueTypeThis);
      if (valueTypeThis != napi_null && valueTypeThis != napi_undefined)
      {
        status = napi_create_reference(state->env, state->self, 1, &deviceState.thisObj);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }

      napi_valuetype valueTypeReference;
      status = napi_typeof(state->env, MYNAPI_ARG[1].value, &valueTypeReference);
      if (valueTypeReference != napi_null && valueTypeReference != napi_undefined)
      {
        status = napi_create_reference(state->env, MYNAPI_ARG[1].value, 1, &deviceState.reference);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }
    }

  }
  mynapi_complete: 
    MYNAPI_COMPLETE_RESULT();
}


// ----------


// OM_EXPORT int OmGetDeviceIds(int *deviceIds, int maxDevices);
MYNAPI_FUNCTION(OmGetDeviceIds) {
  mynapi_params:   {
    MYNAPI_PARAMS();
  }
  mynapi_exec:     {
    int maxDevices = OmGetDeviceIds(NULL, 0);
    MYNAPI_RESULT(maxDevices);
    if (OM_FAILED(maxDevices)) { return false; }

    state->data = malloc(maxDevices * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    int *buffer = (int *)state->data;

    int numDevices = OmGetDeviceIds(buffer, maxDevices);
    MYNAPI_RESULT_FULL_INT(numDevices);
    if (numDevices > maxDevices) { numDevices = maxDevices; }
    MYNAPI_RESULT(numDevices);
    if (OM_FAILED(numDevices)) { return false; }
  }
  mynapi_complete: {
    if (!OM_FAILED(state->result))
    {
      napi_status status;
      int *buffer = (int *)state->data;

      status = napi_create_array(state->env, &state->resultValue);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }

      int i;
      for (i = 0; i < state->result; i++)
      {
        napi_value deviceId;
        status = napi_create_int32(state->env, buffer[i], &deviceId);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
        status = napi_set_element(state->env, state->resultValue, i, deviceId);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_SET_ELEMENT); return false; }
      }
    }
    free(state->data);
    MYNAPI_COMPLETE_RESULT();
  }
}


void CleanupMain(napi_env env)
{
  // Releases some resources (e.g. callbacks)

  // Callback: log
  if (logState.threadsafeFunction != NULL)
  {
    napi_release_threadsafe_function(logState.threadsafeFunction, napi_tsfn_abort); // napi_tsfn_release
    logState.threadsafeFunction = NULL;
  }
  if (logState.thisObj != NULL)
  {
    if (env) napi_delete_reference(env, logState.thisObj);
    logState.thisObj = NULL;
  }
  if (logState.reference != NULL)
  {
    if (env) napi_delete_reference(env, logState.reference);
    logState.reference = NULL;
  }

  // Callback: device
  if (deviceState.threadsafeFunction != NULL)
  {
    napi_release_threadsafe_function(deviceState.threadsafeFunction, napi_tsfn_abort); // napi_tsfn_release
    deviceState.threadsafeFunction = NULL;
  }
  if (deviceState.thisObj != NULL)
  {
    if (env) napi_delete_reference(env, deviceState.thisObj);
    deviceState.thisObj = NULL;
  }
  if (deviceState.reference != NULL)
  {
    if (env) napi_delete_reference(env, deviceState.reference);
    deviceState.reference = NULL;
  }
}


napi_value InitMain(napi_env env, napi_value exports)
{
  napi_status status;

  EXPORT_INT(OM_VERSION);

  // OM_DEVICE_STATUS
  EXPORT_INT(OM_DEVICE_REMOVED);
  EXPORT_INT(OM_DEVICE_CONNECTED);

  // --- FUNCTIONS ---
  EXPORT_FUNCTION(OmStartup);
  EXPORT_FUNCTION(OmShutdown);
  EXPORT_FUNCTION(OmSetLogCallback);
  EXPORT_FUNCTION(OmSetDeviceCallback);
  EXPORT_FUNCTION(OmGetDeviceIds);
  // ---------------

  // Direct OM callbacks to our code
  OmSetLogCallback(logCallback, NULL);
  OmSetDeviceCallback(deviceCallback, NULL);

  return exports;
}

