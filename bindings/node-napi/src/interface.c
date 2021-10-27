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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


mynapi_state_t *createState(mynapi_params_callback_t paramsCallback, mynapi_exec_callback_t execCallback, mynapi_complete_callback_t completeCallback)
{
  mynapi_state_t *state = (mynapi_state_t *)malloc(sizeof(mynapi_state_t));
  if (state == NULL) { return NULL; }
  memset(state, 0, sizeof(mynapi_state_t));
  
  state->paramsCallback = paramsCallback;
  state->execCallback = execCallback;
  state->completeCallback = completeCallback;

  return state;
}

void destroyState(mynapi_state_t *state)
{
  free(state);
}


bool paramInt(mynapi_state_t *state, napi_value nValue, mynapi_value_t *value)
{
  int val = 0;
  napi_status status = napi_get_value_int32(state->env, nValue, &val);
  if (status != napi_ok) { return false; }
  value->intValue = val;
  return true;
}


bool paramValue(mynapi_state_t *state, napi_value nValue, mynapi_value_t *value)
{
  value->value = nValue;
  return true;
}

bool paramArrayEmpty(mynapi_state_t *state, napi_value nValue, mynapi_value_t *value)
{
  value->value = nValue;
  bool isArray = false;
  napi_status status = napi_is_array(state->env, value->value, &isArray);
  if (!isArray) { return false; }
  uint32_t len = 0;
  status = napi_get_array_length(state->env, value->value, &len);
  if (len != 0) { return false; }
  return true;
}


bool mynapiParams(mynapi_state_t *state, mynapi_param_parser_t *paramParsers, unsigned int count)
{
  napi_env env = state->env;
  napi_callback_info info = state->callbackInfo;
  napi_status status;

  napi_value argv[MYNAPI_MAX_ARGS];
  //const int count = sizeof(argv) / sizeof(argv[0]);
  size_t argc = count;
  status = napi_get_cb_info(env, info, &argc, argv, &state->self, NULL);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_ARGS); return false; }
  if (argc != count) { napi_throw_error(env, NULL, ERROR_ARGS); return false; }

  unsigned int i;
  for (i = 0; i < count; i++)
  {
    if (!paramParsers[0](state, argv[i], &state->argv[i]))
    {
      napi_throw_type_error(env, NULL, ERROR_ARGS);
      return false;
    }
  }

  return true;
}


napi_value createError(napi_env env, int codeInt, const char *message)
{
  napi_status status;

  napi_value code;
  char codeStr[22];
  sprintf(codeStr, "%d", codeInt);
  status = napi_create_string_utf8(env, codeStr, NAPI_AUTO_LENGTH, &code);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }

  napi_value msg;
  status = napi_create_string_utf8(env, message, NAPI_AUTO_LENGTH, &msg);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }
  
  napi_value err;
  status = napi_create_error(env, code, msg, &err);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }

  return err;
}


// Create ret/err result value
bool completeResult(mynapi_state_t *state)
{
  napi_env env = state->env;
  napi_status status;
  bool isErr = OM_FAILED(state->result);
  bool isThrow = state->flags & OPTIONS_THROW;

  // Some methods are better mapped as returning an object
  if (state->resultValue)
  {
    // ...if the call failed and not throwing, return undefined
    if (isErr && !isThrow)
    {
      status = napi_get_undefined(state->env, &state->ret);
      if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return false; }
    }
    else
    {
      state->ret = state->resultValue;
    }
  }
  else if (state->resultStr) // otherwise, return better-mapped to a (caller-owned) string
  {
    // ...if the call failed and not throwing, return undefined
    if (isErr && !isThrow)
    {
      status = napi_get_undefined(state->env, &state->ret);
      if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return false; }
    }
    else
    {
      status = napi_create_string_utf8(state->env, state->resultStr, NAPI_AUTO_LENGTH, &state->ret); // napi_create_string_latin1
      if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return false; }
    }
  }
  else  // otherwise, the return value was a failure/success/value number
  {
    status = napi_create_int32(env, state->resultInt, &state->ret);
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return false; }
  }

  // the 'result' value always determines an error (if negative and in throwing mode)
  if (isErr && isThrow)
  {
    state->error = createError(state->env, state->result, OmErrorString(state->result));
  }
  return true;
}




void promiseExecute(napi_env env, void *data)
{
  mynapi_state_t *state = (mynapi_state_t *)data;
  state->env = env;
  state->stage = STAGE_EXEC;
  state->execCallback(state);
}



void promiseDestroy(mynapi_state_t *state, napi_env env)
{
  napi_status status;
  state->deferred = NULL;
  status = napi_delete_async_work(env, state->asyncWork);
  destroyState(state);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_DELETE); return; }
}


void promiseComplete(napi_env env, napi_status completeStatus, void *data)
{
  // completeStatus == napi_cancelled
  mynapi_state_t *state = (mynapi_state_t *)data;
  napi_status status;

  // Finish and create a result value
  state->env = env;
  state->completeStatus = completeStatus;
  state->stage = STAGE_COMPLETE;
  state->completeCallback(state);

  // Reject or resolve the deferred
  if (state->error != NULL)
  {
    status = napi_reject_deferred(env, state->deferred, state->error);
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_PROMISE); return; }
  }
  else
  {
    status = napi_resolve_deferred(env, state->deferred, state->ret);
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_PROMISE); return; }
  }

  // Clean up the promise
  promiseDestroy(state, env);
}


// Called to return the synchronous function return value, or throw an exception
napi_value syncComplete(mynapi_state_t *state)
{
  napi_env env = state->env;

  // Execute
  state->stage = STAGE_EXEC;
  state->execCallback(state);

  // Finish and create a result value
  state->stage = STAGE_COMPLETE;
  state->completeCallback(state);

  // Copy return/throw reference and remove state
  napi_value error = state->error;
  napi_value ret = state->ret;
  destroyState(state);

  // Return or throw
  if (error != NULL)
  {
    napi_status status;
    status = napi_throw(env, error);
    if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_THROW); return NULL; }
  }
  return ret;
}


napi_value promiseCreate(mynapi_state_t *state)
{
  napi_env env = state->env;
  napi_status status;

  napi_value promise;
  status = napi_create_promise(env, &state->deferred, &promise);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }

  napi_value asyncResourceName;
  status = napi_create_string_utf8(env, "libomapi", NAPI_AUTO_LENGTH, &asyncResourceName);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }
  status = napi_create_async_work(env, NULL, asyncResourceName, promiseExecute, promiseComplete, state, &state->asyncWork);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }
  status = napi_queue_async_work(env, state->asyncWork);
  if (status != napi_ok) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }

  return promise;
}


napi_value run(mynapi_options_t flags, napi_env env, napi_callback_info info, const char *name, mynapi_params_callback_t paramsCallback, mynapi_exec_callback_t execCallback, mynapi_complete_callback_t completeCallback)
{
  mynapi_state_t *state = createState(paramsCallback, execCallback, completeCallback);
  if (state == NULL) { napi_throw_error(env, NULL, ERROR_CREATE); return NULL; }
  state->flags = flags;
  state->env = env;
  state->callbackInfo = info;
  state->stage = STAGE_PARAMS;
  bool success = state->paramsCallback(state);
  if (!success)
  { 
    destroyState(state);
    return NULL; 
  }

  if (state->flags & OPTIONS_ASYNC)
  {
    return promiseCreate(state);
  }
  else
  {
    return syncComplete(state);
  }
}



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

