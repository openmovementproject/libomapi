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



// OM_EXPORT const char *OmErrorString(int status);
MYNAPI_FUNCTION(OmErrorString) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     state->resultStr = (char *)OmErrorString(MYNAPI_ARG[0].intValue);
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}


// (not a real OMAPI function)
MYNAPI_FUNCTION(OmLog) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue);

    size_t sz;
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[0].value, NULL, 0, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
    state->data = malloc(sz + 1);
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }

    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[0].value, state->data, sz, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }

    // printf("LOG: %s\n", (char *)state->data);

    extern void logCallback(void *reference, const char *message);
    logCallback(NULL, state->data);
  }
  mynapi_exec:     MYNAPI_RESULT(0);
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); free(state->data);
}


// --- OM_DATETIME handling functions ---

// OM_EXPORT OM_DATETIME OmDateTimeFromString(const char *value);
MYNAPI_FUNCTION(OmDateTimeFromString) {
  mynapi_params:   {
    napi_status status;
    MYNAPI_PARAMS(paramValue);

    size_t sz;
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[0].value, NULL, 0, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
    state->data = malloc(sz + 1);
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[0].value, state->data, sz, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    OM_DATETIME time = OmDateTimeFromString(state->data);
    free(state->data);
    MYNAPI_RESULT(0);
    MYNAPI_RESULT_FULL_INT(time);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT char *OmDateTimeToString(OM_DATETIME value, char *buffer);
MYNAPI_FUNCTION(OmDateTimeToString) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->resultStr = (char *)malloc(OM_DATETIME_BUFFER_SIZE + 1);
    state->resultStr[0] = '\0';
    OmDateTimeToString(MYNAPI_ARG[0].intValue, state->resultStr);
    MYNAPI_RESULT(0);
  }
  mynapi_complete: {
    MYNAPI_COMPLETE_RESULT();
    free(state->resultStr);
  }
}

// #define OM_DATETIME_FROM_YMDHMS(year, month, day, hours, minutes, seconds) ( (((OM_DATETIME)((year) % 100) & 0x3f) << 26) | (((OM_DATETIME)(month)        & 0x0f) << 22) | (((OM_DATETIME)(day)          & 0x1f) << 17) | (((OM_DATETIME)(hours)        & 0x1f) << 12) | (((OM_DATETIME)(minutes)      & 0x3f) <<  6) | (((OM_DATETIME)(seconds)      & 0x3f)      ) )
MYNAPI_FUNCTION(OM_DATETIME_FROM_YMDHMS) {
  mynapi_params:   MYNAPI_PARAMS(paramInt, paramInt, paramInt, paramInt, paramInt, paramInt);
  mynapi_exec:     MYNAPI_RESULT(OM_DATETIME_FROM_YMDHMS(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue, MYNAPI_ARG[2].intValue, MYNAPI_ARG[3].intValue, MYNAPI_ARG[4].intValue, MYNAPI_ARG[5].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OM_DATETIME_YEAR(dateTime)    ((unsigned int)((unsigned char)(((dateTime) >> 26) & 0x3f)) + 2000) /**< Extract the year from a packed date/time value. @hideinitializer */
MYNAPI_FUNCTION(OM_DATETIME_YEAR) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OM_DATETIME_YEAR(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OM_DATETIME_MONTH(dateTime)   ((unsigned char)(((dateTime) >> 22) & 0x0f))  /**< Extract the month (1-12) from a packed date/time value. @hideinitializer */
MYNAPI_FUNCTION(OM_DATETIME_MONTH) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OM_DATETIME_MONTH(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OM_DATETIME_DAY(dateTime)     ((unsigned char)(((dateTime) >> 17) & 0x1f))  /**< Extract the day (1-31) from a packed date/time value. @hideinitializer */
MYNAPI_FUNCTION(OM_DATETIME_DAY) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OM_DATETIME_DAY(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OM_DATETIME_HOURS(dateTime)   ((unsigned char)(((dateTime) >> 12) & 0x1f))  /**< Extract the hours (0-23) from a packed date/time value. @hideinitializer */
MYNAPI_FUNCTION(OM_DATETIME_HOURS) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OM_DATETIME_HOURS(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OM_DATETIME_MINUTES(dateTime) ((unsigned char)(((dateTime) >>  6) & 0x3f))  /**< Extract the minutes (0-59) from a packed date/time value. @hideinitializer */
MYNAPI_FUNCTION(OM_DATETIME_MINUTES) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OM_DATETIME_MINUTES(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// #define OM_DATETIME_SECONDS(dateTime) ((unsigned char)(((dateTime)      ) & 0x3f))  /**< Extract the seconds (0-59) from a packed date/time value. @hideinitializer */
MYNAPI_FUNCTION(OM_DATETIME_SECONDS) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OM_DATETIME_SECONDS(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}



void Cleanup(napi_env env)
{
  // External clean-up functions
  extern void CleanupMain(napi_env env);
  extern void CleanupStatus(napi_env env);
  extern void CleanupSettings(napi_env env);
  extern void CleanupDownload(napi_env env);
  extern void CleanupReader(napi_env env);

  // Clean-up in reverse order
  CleanupReader(env);
  CleanupDownload(env);
  CleanupSettings(env);
  CleanupStatus(env);
  CleanupMain(env); // Do this last
}

void napiCleanup(void *arg)
{
  Cleanup(NULL);
}



napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;

  // Register clean-up hook
  status = napi_add_env_cleanup_hook(env, napiCleanup, NULL);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CLEANUP); return NULL; }

  // OM_RETURN_VALUE
  EXPORT_INT(OM_TRUE);
  EXPORT_INT(OM_FALSE);
  EXPORT_INT(OM_OK);
  EXPORT_INT(OM_E_FAIL);
  EXPORT_INT(OM_E_UNEXPECTED);
  EXPORT_INT(OM_E_NOT_VALID_STATE);
  EXPORT_INT(OM_E_OUT_OF_MEMORY);
  EXPORT_INT(OM_E_INVALID_ARG);
  EXPORT_INT(OM_E_POINTER);
  EXPORT_INT(OM_E_NOT_IMPLEMENTED);
  EXPORT_INT(OM_E_ABORT);
  EXPORT_INT(OM_E_ACCESS_DENIED);
  EXPORT_INT(OM_E_UNEXPECTED_RESPONSE);
  EXPORT_INT(OM_E_LOCKED);
  // OM_DATETIME constants
  EXPORT_INT(OM_DATETIME_MIN_VALID);
  EXPORT_INT(OM_DATETIME_MAX_VALID);
  EXPORT_INT(OM_DATETIME_ZERO);
  EXPORT_INT(OM_DATETIME_INFINITE);
  EXPORT_INT(OM_DATETIME_BUFFER_SIZE);

  // Functions
  EXPORT_FUNCTION(OmErrorString);
  EXPORT_FUNCTION(OmLog);

  EXPORT_FUNCTION(OmDateTimeFromString);
  EXPORT_FUNCTION(OmDateTimeToString);
  EXPORT_FUNCTION(OM_DATETIME_FROM_YMDHMS);
  EXPORT_FUNCTION(OM_DATETIME_YEAR);
  EXPORT_FUNCTION(OM_DATETIME_MONTH);
  EXPORT_FUNCTION(OM_DATETIME_DAY);
  EXPORT_FUNCTION(OM_DATETIME_HOURS);
  EXPORT_FUNCTION(OM_DATETIME_MINUTES);
  EXPORT_FUNCTION(OM_DATETIME_SECONDS);


  // External init functions
  extern napi_value InitMain(napi_env env, napi_value exports);
  extern napi_value InitStatus(napi_env env, napi_value exports);
  extern napi_value InitSettings(napi_env env, napi_value exports);
  extern napi_value InitDownload(napi_env env, napi_value exports);
  extern napi_value InitReader(napi_env env, napi_value exports);
  
  // Call other InitX() functions here
  if (exports) exports = InitMain(env, exports);
  if (exports) exports = InitStatus(env, exports);
  if (exports) exports = InitSettings(env, exports);
  if (exports) exports = InitDownload(env, exports);
  if (exports) exports = InitReader(env, exports);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
