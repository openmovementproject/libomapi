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


// OM_EXPORT int OmGetDelays(int deviceId, OM_DATETIME *startTime, OM_DATETIME *stopTime);
MYNAPI_FUNCTION(OmGetDelays) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->data = malloc(2 * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    OM_DATETIME startTime, stopTime;
    MYNAPI_RESULT(OmGetDelays(MYNAPI_ARG[0].intValue, &startTime, &stopTime));
    int *params = (int *)state->data;
    params[0] = (int)startTime;
    params[1] = (int)stopTime;
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

// OM_EXPORT int OmSetDelays(int deviceId, OM_DATETIME startTime, OM_DATETIME stopTime);
MYNAPI_FUNCTION(OmSetDelays) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetDelays(MYNAPI_ARG[0].intValue, (OM_DATETIME)MYNAPI_ARG[1].intValue, (OM_DATETIME)MYNAPI_ARG[2].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetSessionId(int deviceId, unsigned int *sessionId);
MYNAPI_FUNCTION(OmGetSessionId) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     {
    unsigned int sessionId;
    MYNAPI_RESULT(OmGetSessionId(MYNAPI_ARG[0].intValue, &sessionId));
    MYNAPI_RESULT_FULL_INT(sessionId);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmSetSessionId(int deviceId, unsigned int sessionId);
MYNAPI_FUNCTION(OmSetSessionId) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetSessionId(MYNAPI_ARG[0].intValue, (unsigned int)MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetMetadata(int deviceId, char *metadata);
MYNAPI_FUNCTION(OmGetMetadata) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->resultStr = (char *)malloc(OM_METADATA_SIZE + 1);
    state->resultStr[0] = '\0';
    MYNAPI_RESULT(OmGetMetadata(MYNAPI_ARG[0].intValue, state->resultStr));
  }
  mynapi_complete: {
    MYNAPI_COMPLETE_RESULT();
    free(state->resultStr);
  }
}

// OM_EXPORT int OmSetMetadata(int deviceId, const char *metadata, int size);
MYNAPI_FUNCTION(OmSetMetadata) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramInt, paramValue);
    size_t sz;
    state->data = malloc(OM_METADATA_SIZE + 1);
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[1].value, state->data, OM_METADATA_SIZE + 1, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     MYNAPI_RESULT(OmSetMetadata(MYNAPI_ARG[0].intValue, (char *)state->data, strlen((char *)state->data)));
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); free(state->data);
}

// OM_EXPORT int OmGetLastConfigTime(int deviceId, OM_DATETIME *time);
MYNAPI_FUNCTION(OmGetLastConfigTime) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     {
    OM_DATETIME time;
    MYNAPI_RESULT(OmGetLastConfigTime(MYNAPI_ARG[0].intValue, &time));
    MYNAPI_RESULT_FULL_INT(time);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmEraseDataAndCommit(int deviceId, OM_ERASE_LEVEL eraseLevel);
MYNAPI_FUNCTION(OmEraseDataAndCommit) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmEraseDataAndCommit(MYNAPI_ARG[0].intValue, (OM_ERASE_LEVEL)MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OmClearDataAndCommit(_deviceId) OmEraseDataAndCommit((_deviceId), OM_ERASE_QUICKFORMAT)
MYNAPI_FUNCTION(OmClearDataAndCommit) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmEraseDataAndCommit(MYNAPI_ARG[0].intValue, OM_ERASE_QUICKFORMAT));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OmCommit(_deviceId) OmEraseDataAndCommit((_deviceId), OM_ERASE_NONE)
MYNAPI_FUNCTION(OmCommit) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmEraseDataAndCommit(MYNAPI_ARG[0].intValue, OM_ERASE_NONE));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetAccelConfig(int deviceId, int *rate, int *range);
MYNAPI_FUNCTION(OmGetAccelConfig) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->data = malloc(2 * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    int *params = (int *)state->data;
    MYNAPI_RESULT(OmGetAccelConfig(MYNAPI_ARG[0].intValue, &params[0], &params[1]));
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

// OM_EXPORT int OmSetAccelConfig(int deviceId, int rate, int range);
MYNAPI_FUNCTION(OmSetAccelConfig) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetAccelConfig(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue, MYNAPI_ARG[2].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetMaxSamples(int deviceId, int *value);
MYNAPI_FUNCTION(OmGetMaxSamples) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     {
    int maxSamples;
    MYNAPI_RESULT(OmGetMaxSamples(MYNAPI_ARG[0].intValue, &maxSamples));
    MYNAPI_RESULT_FULL_INT(maxSamples);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmSetMaxSamples(int deviceId, int value);
MYNAPI_FUNCTION(OmSetMaxSamples) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmSetMaxSamples(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}


void CleanupSettings(napi_env env)
{
  ;
}

napi_value InitSettings(napi_env env, napi_value exports)
{
  napi_status status;

  // OM_ACCEL_DEFAULT
  EXPORT_INT(OM_ACCEL_DEFAULT_RATE);
  EXPORT_INT(OM_ACCEL_DEFAULT_RANGE);
  // OM_METADATA_SIZE
  EXPORT_INT(OM_METADATA_SIZE);
  // OM_ERASE_LEVEL
  EXPORT_INT(OM_ERASE_NONE);
  EXPORT_INT(OM_ERASE_DELETE);
  EXPORT_INT(OM_ERASE_QUICKFORMAT);
  EXPORT_INT(OM_ERASE_WIPE);

  // --- FUNCTIONS ---
  EXPORT_FUNCTION(OmGetDelays);
  EXPORT_FUNCTION(OmSetDelays);
  EXPORT_FUNCTION(OmGetSessionId);
  EXPORT_FUNCTION(OmSetSessionId);
  EXPORT_FUNCTION(OmGetMetadata);
  EXPORT_FUNCTION(OmSetMetadata);
  EXPORT_FUNCTION(OmGetLastConfigTime);
  EXPORT_FUNCTION(OmEraseDataAndCommit);
  EXPORT_FUNCTION(OmClearDataAndCommit);
  EXPORT_FUNCTION(OmCommit);
  EXPORT_FUNCTION(OmGetAccelConfig);
  EXPORT_FUNCTION(OmSetAccelConfig);
  EXPORT_FUNCTION(OmGetMaxSamples);
  EXPORT_FUNCTION(OmSetMaxSamples);
  // ---------------

  return exports;
}

