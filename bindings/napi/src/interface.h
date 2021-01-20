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

#define NAPI_EXPERIMENTAL
//#define NAPI_VERSION 3
#include <node_api.h>
#include <stdbool.h>

#include "../libomapi/include/omapi.h"

#define ERROR_ARGS          "Invalid argument"
#define ERROR_CREATE        "Cannot create value"
#define ERROR_PROMISE       "Problem resolving/rejecting promise"
#define ERROR_CLEANUP       "Unable to set clean-up function"
#define ERROR_WRAP          "Unable to wrap native function"
#define ERROR_EXPORTS       "Unable to populate exports"
#define ERROR_DELETE        "Unable to delete"
#define ERROR_SET_ELEMENT   "Unable to set array element"
#define ERROR_THROW         "Unable to throw"

#define ASSERT_SUCCEEDED_OR_THROW(result) \
   if (OM_FAILED(result)) { \
     char codeStr[22]; \
     sprintf(codeStr, "%d", result); \
     napi_throw_error(env, codeStr, OmErrorString(result)); \
     return NULL; \
   }

typedef enum
{
  STAGE_PARAMS = 0x01,
  STAGE_EXEC = 0x02,
  STAGE_COMPLETE = 0x04,
} mynapi_stage_t;

typedef enum
{
  OPTIONS_NONE  = 0x00,
  OPTIONS_ASYNC = 0x01,
  OPTIONS_THROW = 0x02,
  OPTIONS_ASYNC_THROW = 0x03,
} mynapi_options_t;

struct mynapi_state_tag;

typedef union
{
  int intValue;
  // double doubleValue;
  const char *stringValue;
  napi_value value;
} mynapi_value_t;

typedef bool (*mynapi_param_parser_t)(struct mynapi_state_tag *state, napi_value, mynapi_value_t*);

typedef bool (*mynapi_params_callback_t)(struct mynapi_state_tag *state);
typedef bool (*mynapi_exec_callback_t)(struct mynapi_state_tag *state);
typedef bool (*mynapi_complete_callback_t)(struct mynapi_state_tag *state);

#define MYNAPI_MAX_ARGS 16

typedef struct mynapi_state_tag
{
  // function name
  const char *name;

  // flags (async -- may add nothrow version?)
  mynapi_options_t flags;

  // stage of async execution (params, exec, complete)
  mynapi_stage_t stage;

  // current environment and 'this'
  napi_env env;
  napi_value self;
  
  // callback for each part of the function execution
  mynapi_params_callback_t paramsCallback;
  mynapi_exec_callback_t execCallback;
  mynapi_complete_callback_t completeCallback;

  // (params stage)
  napi_callback_info callbackInfo;
  void *data;         // function specific data
  size_t size;        // store size of function-specific data
  void *ref;          // additional function specific data
  double valueDouble; // fractional value

  // (exec stage) async/deferred handles
  napi_async_work asyncWork;
  napi_deferred deferred;

  // (complete stage)
  napi_status completeStatus;

  // result error/return
  napi_value error;       // (if set); sync: thrown exception; async reject value
  napi_value ret;         // (if no error); sync: return value; async: resolve value

  // return value
  int result;             // standard int result (failure/success/value)
  napi_value resultValue; // overrides returning an int to return a javascript object
  char *resultStr;        // overrides returning an int to return a string
  int resultInt;          // full-range signed int result value (not representing success) -- not compatible as a return value with "no-throw" functions

  // arguments
  mynapi_value_t argv[MYNAPI_MAX_ARGS];
} mynapi_state_t;



mynapi_state_t *createState(mynapi_params_callback_t paramsCallback, mynapi_exec_callback_t execCallback, mynapi_complete_callback_t completeCallback);

bool paramInt(mynapi_state_t *state, napi_value nValue, mynapi_value_t *value);
bool paramValue(mynapi_state_t *state, napi_value nValue, mynapi_value_t *value);
bool paramArrayEmpty(mynapi_state_t *state, napi_value nValue, mynapi_value_t *value);

bool mynapiParams(mynapi_state_t *state, mynapi_param_parser_t *paramParsers, unsigned int count);

napi_value createError(napi_env env, int codeInt, const char *message);

// Create ret/err result value
bool completeResult(mynapi_state_t *state);

void promiseExecute(napi_env env, void *data);

void promiseDestroy(mynapi_state_t *state, napi_env env);

void promiseComplete(napi_env env, napi_status completeStatus, void *data);

// Called to return the synchronous function return value, or throw an exception
napi_value syncComplete(mynapi_state_t *state);

napi_value promiseCreate(mynapi_state_t *state);

napi_value run(mynapi_options_t flags, napi_env env, napi_callback_info info, const char *name, mynapi_params_callback_t paramsCallback, mynapi_exec_callback_t execCallback, mynapi_complete_callback_t completeCallback);


#define mynapi_params  if (state->stage & STAGE_PARAMS) { goto _params; _params
#define mynapi_exec  } if (state->stage & STAGE_EXEC) { goto _exec; _exec
#define mynapi_complete  } if (!(state->stage & STAGE_COMPLETE)) { return true; } goto _complete; _complete

#define MYNAPI_FUNCTION(_name) \
  bool run ## _name (mynapi_state_t *state); \
  \
  napi_value napi ## _name (napi_env env, napi_callback_info info) \
  { \
    return run(OPTIONS_THROW, env, info, #_name, run ## _name, run ## _name, run ## _name); \
  } \
  \
  napi_value napi ## _name ## Async (napi_env env, napi_callback_info info) \
  { \
    return run(OPTIONS_ASYNC_THROW, env, info, #_name "Async", run ## _name, run ## _name, run ## _name); \
  } \
  \
  bool run ## _name (mynapi_state_t *state)


// initial NULL is to cope with zero va_args
#define MYNAPI_PARAMS(...) { mynapi_param_parser_t paramParsers[] = { NULL, __VA_ARGS__ }; mynapiParams(state, paramParsers + 1, sizeof(paramParsers) / sizeof(paramParsers[0]) - 1); }
#define MYNAPI_ARG state->argv
#define MYNAPI_RESULT(_val) state->resultInt = state->result = (_val)
#define MYNAPI_RESULT_FULL_INT(_val) state->resultInt = (_val)
#define MYNAPI_COMPLETE_RESULT() return completeResult(state);

#define CALLBACK_MAX_QUEUE 32   // 0=unlimited but has a bug, so use >0

typedef struct {
  napi_threadsafe_function threadsafeFunction;
  napi_ref thisObj;
  napi_ref reference;
} callbackState_t;



#define EXPORT_INT(_name) { \
    napi_value val; \
    \
    status = napi_create_int32(env, _name, &val); \
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_EXPORTS); return NULL; } \
    status = napi_set_named_property(env, exports, #_name, val); \
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_EXPORTS); return NULL; } \
  }

#define EXPORT_FUNCTION(_name) { \
    napi_value fn; \
    \
    extern napi_value napi ## _name (napi_env env, napi_callback_info info);\
    status = napi_create_function(env, NULL, 0, napi ## _name, NULL, &fn); \
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_WRAP); return NULL; } \
    status = napi_set_named_property(env, exports, #_name, fn); \
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_EXPORTS); return NULL; } \
    \
    extern napi_value napi ## _name ## Async (napi_env env, napi_callback_info info);\
    status = napi_create_function(env, NULL, 0, napi ## _name ## Async, NULL, &fn); \
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_WRAP); return NULL; } \
    status = napi_set_named_property(env, exports, #_name "Async", fn); \
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_EXPORTS); return NULL; } \
  }

