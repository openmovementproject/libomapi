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


// OM_EXPORT int OmGetDataFileSize(int deviceId);
MYNAPI_FUNCTION(OmGetDataFileSize) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmGetDataFileSize(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}

// OM_EXPORT int OmGetDataFilename(int deviceId, char *filenameBuffer);
MYNAPI_FUNCTION(OmGetDataFilename) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->resultStr = (char *)malloc(OM_MAX_PATH + 1);
    state->resultStr[0] = '\0';
    MYNAPI_RESULT(OmGetDataFilename(MYNAPI_ARG[0].intValue, state->resultStr));
  }
  mynapi_complete: {
    MYNAPI_COMPLETE_RESULT();
    free(state->resultStr);
  }
}

// OM_EXPORT int OmGetDataRange(int deviceId, int *dataBlockSize, int *dataOffsetBlocks, int *dataNumBlocks, OM_DATETIME *startTime, OM_DATETIME *endTime);
MYNAPI_FUNCTION(OmGetDataRange) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->data = malloc(5 * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    int *params = (int *)state->data;
    int dataBlockSize = 0;
    int dataOffsetBlocks = 0;
    int dataNumBlocks = 0;
    OM_DATETIME startTime = 0;
    OM_DATETIME endTime = 0;
    MYNAPI_RESULT(OmGetDataRange(MYNAPI_ARG[0].intValue, &dataBlockSize, &dataOffsetBlocks, &dataNumBlocks, &startTime, &endTime));
    params[0] = dataBlockSize;
    params[1] = dataOffsetBlocks;
    params[2] = dataNumBlocks;
    params[3] = (int)startTime;
    params[4] = (int)endTime;
  }
  mynapi_complete: {
    int *params = (int *)state->data;
    napi_status status;

    status = napi_create_array(state->env, &state->resultValue);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }

    int i;
    for (i = 0; i < 5; i++)
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

// OM_EXPORT int OmBeginDownloading(int deviceId, int dataOffsetBlocks, int dataLengthBlocks, const char *destinationFile);
MYNAPI_FUNCTION(OmBeginDownloading) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramInt, paramInt, paramInt, paramValue);

    size_t sz;
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[3].value, NULL, 0, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
    state->data = malloc(sz + 1);
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[3].value, state->data, sz, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     MYNAPI_RESULT(OmBeginDownloading(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue, MYNAPI_ARG[2].intValue, state->data));
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); free(state->data);
}

// OM_EXPORT int OmBeginDownloadingReference(int deviceId, int dataOffsetBlocks, int dataLengthBlocks, const char *destinationFile, void *reference);
MYNAPI_FUNCTION(OmBeginDownloadingReference) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramInt, paramInt, paramInt, paramValue, paramValue);

    size_t sz;
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[3].value, NULL, 0, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
    state->data = malloc(sz + 1);
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[3].value, state->data, sz, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    void *reference = NULL;
    // TODO: The js reference object is currently ignored -- should create our own tracking structure, ref-count the js object, then return that in the callback
    MYNAPI_RESULT(OmBeginDownloadingReference(MYNAPI_ARG[0].intValue, MYNAPI_ARG[1].intValue, MYNAPI_ARG[2].intValue, state->data, reference));
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); free(state->data);
}

// OM_EXPORT int OmQueryDownload(int deviceId, OM_DOWNLOAD_STATUS *downloadStatus, int *downloadValue);
MYNAPI_FUNCTION(OmQueryDownload) {
  mynapi_params: MYNAPI_PARAMS(paramInt);
  mynapi_exec: {
    state->data = malloc(2 * sizeof(int));
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    int *params = (int *)state->data;
    OM_DOWNLOAD_STATUS downloadStatus = 0;
    int downloadValue = 0;
    MYNAPI_RESULT(OmQueryDownload(MYNAPI_ARG[0].intValue, &downloadStatus, &downloadValue));
    params[0] = (int)downloadStatus;
    params[1] = (int)downloadValue;
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

// OM_EXPORT int OmWaitForDownload(int deviceId, OM_DOWNLOAD_STATUS *downloadStatus, int *downloadValue);
// (probably not a good idea for the way node works, possibly could make it 'await' only, but would tie up the thread from the pool)

// OM_EXPORT int OmCancelDownload(int deviceId);
MYNAPI_FUNCTION(OmCancelDownload) {
  mynapi_params:   MYNAPI_PARAMS(paramInt);
  mynapi_exec:     MYNAPI_RESULT(OmCancelDownload(MYNAPI_ARG[0].intValue));
  mynapi_complete: MYNAPI_COMPLETE_RESULT();
}



// OM_EXPORT int OmSetDownloadCallback(OmDownloadCallback downloadCallback, void *reference);
// --- CALLBACK: DOWNLOAD ---
static callbackState_t downloadState;

typedef struct {
  int id;
  OM_DOWNLOAD_STATUS status;
  int value;
} downloadCallbackData_t;

static void downloadCallbackJSFinalize(napi_env env, void* finalize_data, void* finalize_hint)
{
  free(finalize_data);         // Comment
}

static void downloadCallbackJS(napi_env env, napi_value js_callback, void *context, void *vData)
{
  napi_status status;
  downloadCallbackData_t *data = (downloadCallbackData_t *)vData;
  napi_value argv[4];
  napi_value result;

  napi_value thisValue = NULL;
  if (downloadState.thisObj)
  {
    status = napi_get_reference_value(env, downloadState.thisObj, &thisValue);
    if (status != napi_ok) { return; }
  }
  else
  {
    napi_get_undefined(env, &thisValue);
  }

  napi_value referenceValue = NULL;
  if (downloadState.reference)
  {
    status = napi_get_reference_value(env, downloadState.reference, &referenceValue);
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

  status = napi_create_int32(env, data->value, &argv[3]);
  if (status != napi_ok) { return; }

  status = napi_call_function(env, thisValue, js_callback, (int)(sizeof(argv)/sizeof(argv[0])), argv, &result);
  if (status != napi_ok) { return; }
}

//typedef void(*OmDownloadCallback)(void *, int, OM_DOWNLOAD_STATUS, int);
static void downloadCallback(void *reference, int id, OM_DOWNLOAD_STATUS downloadStatus, int value)
{
  napi_status status;

  if (downloadState.threadsafeFunction == NULL) return;

  downloadCallbackData_t *data = (downloadCallbackData_t *)malloc(sizeof(downloadCallbackData_t));
  data->id = id;
  data->status = downloadStatus;
  data->value = value;

  status = napi_acquire_threadsafe_function(downloadState.threadsafeFunction);
  if (status != napi_ok) { return; }

  status = napi_call_threadsafe_function(downloadState.threadsafeFunction, data, napi_tsfn_blocking);
  if (status != napi_ok) { return; }
}

// int OmSetDownloadCallback(OmDownloadCallback downloadCallback, void *reference);
MYNAPI_FUNCTION(OmSetDownloadCallback) {
  mynapi_params:
     MYNAPI_PARAMS(paramValue, paramValue);
  mynapi_exec: {
    napi_status status;
    napi_value cb = MYNAPI_ARG[0].value;
    
    napi_valuetype valueType;
    status = napi_typeof(state->env, cb, &valueType);
    if (valueType == napi_null || valueType == napi_undefined)    // removing callback?
    {
      if (downloadState.thisObj != NULL)
      {
        napi_delete_reference(state->env, downloadState.thisObj);
        downloadState.thisObj = NULL;
      }

      if (downloadState.reference != NULL)
      {
        napi_delete_reference(state->env, downloadState.reference);
        downloadState.reference = NULL;
      }
    }
    else 
    {
      napi_value resourceName;
      status = napi_create_string_utf8(state->env, "OmDownloadCallback", NAPI_AUTO_LENGTH, &resourceName);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      status = napi_create_threadsafe_function(state->env, cb, NULL, resourceName, CALLBACK_MAX_QUEUE, 1, NULL, downloadCallbackJSFinalize, NULL, downloadCallbackJS, &downloadState.threadsafeFunction);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      napi_valuetype valueTypeThis;
      status = napi_typeof(state->env, state->self, &valueTypeThis);
      if (valueTypeThis != napi_null && valueTypeThis != napi_undefined)
      {
        status = napi_create_reference(state->env, state->self, 1, &downloadState.thisObj);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }

      napi_valuetype valueTypeReference;
      status = napi_typeof(state->env, MYNAPI_ARG[1].value, &valueTypeReference);
      if (valueTypeReference != napi_null && valueTypeReference != napi_undefined)
      {
        status = napi_create_reference(state->env, MYNAPI_ARG[1].value, 1, &downloadState.reference);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }
    }

  }
  mynapi_complete: 
    MYNAPI_COMPLETE_RESULT();
}




// OM_EXPORT int OmSetDownloadChunkCallback(OmDownloadChunkCallback downloadChunkCallback, void *reference);
// --- CALLBACK: DOWNLOAD CHUNK ---
static callbackState_t downloadChunkState;

typedef struct {
  int id;
  void *bufferPointer;
  int filePosition;
  int bufferSize;
} downloadChunkCallbackData_t;

static void downloadChunkCallbackJSFinalize(napi_env env, void* finalize_data, void* finalize_hint)
{
  free(finalize_data);         // Comment
}

static void downloadChunkCallbackJS(napi_env env, napi_value js_callback, void *context, void *vData)
{
  napi_status status;
  downloadChunkCallbackData_t *data = (downloadChunkCallbackData_t *)vData;
  napi_value argv[5];
  napi_value result;

  napi_value thisValue = NULL;
  if (downloadChunkState.thisObj)
  {
    status = napi_get_reference_value(env, downloadChunkState.thisObj, &thisValue);
    if (status != napi_ok) { return; }
  }
  else
  {
    napi_get_undefined(env, &thisValue);
  }

  napi_value referenceValue = NULL;
  if (downloadChunkState.reference)
  {
    status = napi_get_reference_value(env, downloadChunkState.reference, &referenceValue);
    if (status != napi_ok) { return; }
  }
  else
  {
    napi_get_undefined(env, &referenceValue);
  }
  argv[0] = referenceValue;

  status = napi_create_int32(env, data->id, &argv[1]);
  if (status != napi_ok) { return; }

// TODO: This should be a pointer to the data buffer used for the (inspectable) chunked-download
  status = napi_get_undefined(env, &argv[2]);
  if (status != napi_ok) { return; }

  status = napi_create_int32(env, data->filePosition, &argv[3]);
  if (status != napi_ok) { return; }

  status = napi_create_int32(env, data->bufferSize, &argv[4]);
  if (status != napi_ok) { return; }

  status = napi_call_function(env, thisValue, js_callback, (int)(sizeof(argv)/sizeof(argv[0])), argv, &result);
  if (status != napi_ok) { return; }
}

// typedef void(*OmDownloadChunkCallback)(void *, int, void *, int, int);
static void downloadChunkCallback(void *reference, int id, void *bufferPointer, int filePosition, int bufferSize)
{
  napi_status status;

  if (downloadChunkState.threadsafeFunction == NULL) return;

  downloadChunkCallbackData_t *data = (downloadChunkCallbackData_t *)malloc(sizeof(downloadChunkCallbackData_t));
  data->id = id;
  data->bufferPointer = bufferPointer;
  data->filePosition = filePosition;
  data->bufferSize = bufferSize;

  status = napi_acquire_threadsafe_function(downloadChunkState.threadsafeFunction);
  if (status != napi_ok) { return; }

  status = napi_call_threadsafe_function(downloadChunkState.threadsafeFunction, data, napi_tsfn_blocking);
  if (status != napi_ok) { return; }
}

// int OmSetDownloadChunkCallback(OmDownloadChunkCallback downloadChunkCallback, void *reference);
MYNAPI_FUNCTION(OmSetDownloadChunkCallback) {
  mynapi_params:
     MYNAPI_PARAMS(paramValue, paramValue);
  mynapi_exec: {
    napi_status status;
    napi_value cb = MYNAPI_ARG[0].value;
    
    napi_valuetype valueType;
    status = napi_typeof(state->env, cb, &valueType);
    if (valueType == napi_null || valueType == napi_undefined)    // removing callback?
    {
      if (downloadChunkState.thisObj != NULL)
      {
        napi_delete_reference(state->env, downloadChunkState.thisObj);
        downloadChunkState.thisObj = NULL;
      }

      if (downloadChunkState.reference != NULL)
      {
        napi_delete_reference(state->env, downloadChunkState.reference);
        downloadChunkState.reference = NULL;
      }
    }
    else 
    {
      napi_value resourceName;
      status = napi_create_string_utf8(state->env, "OmDownloadChunkCallback", NAPI_AUTO_LENGTH, &resourceName);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      status = napi_create_threadsafe_function(state->env, cb, NULL, resourceName, CALLBACK_MAX_QUEUE, 1, NULL, downloadChunkCallbackJSFinalize, NULL, downloadChunkCallbackJS, &downloadChunkState.threadsafeFunction);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }

      napi_valuetype valueTypeThis;
      status = napi_typeof(state->env, state->self, &valueTypeThis);
      if (valueTypeThis != napi_null && valueTypeThis != napi_undefined)
      {
        status = napi_create_reference(state->env, state->self, 1, &downloadChunkState.thisObj);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }

      napi_valuetype valueTypeReference;
      status = napi_typeof(state->env, MYNAPI_ARG[1].value, &valueTypeReference);
      if (valueTypeReference != napi_null && valueTypeReference != napi_undefined)
      {
        status = napi_create_reference(state->env, MYNAPI_ARG[1].value, 1, &downloadChunkState.reference);
        if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); return false; }
      }
    }

  }
  mynapi_complete: 
    MYNAPI_COMPLETE_RESULT();
}



// ----------


void CleanupDownload(napi_env env)
{
  // Callback: download
  if (downloadState.threadsafeFunction != NULL)
  {
    napi_release_threadsafe_function(downloadState.threadsafeFunction, napi_tsfn_abort); // napi_tsfn_release
    downloadState.threadsafeFunction = NULL;
  }
  if (downloadState.thisObj != NULL)
  {
    if (env) napi_delete_reference(env, downloadState.thisObj);
    downloadState.thisObj = NULL;
  }
  if (downloadState.reference != NULL)
  {
    if (env) napi_delete_reference(env, downloadState.reference);
    downloadState.reference = NULL;
  }

  // Callback: download
  if (downloadChunkState.threadsafeFunction != NULL)
  {
    napi_release_threadsafe_function(downloadChunkState.threadsafeFunction, napi_tsfn_abort); // napi_tsfn_release
    downloadChunkState.threadsafeFunction = NULL;
  }
  if (downloadChunkState.thisObj != NULL)
  {
    if (env) napi_delete_reference(env, downloadChunkState.thisObj);
    downloadChunkState.thisObj = NULL;
  }
  if (downloadChunkState.reference != NULL)
  {
    if (env) napi_delete_reference(env, downloadChunkState.reference);
    downloadChunkState.reference = NULL;
  }
}

napi_value InitDownload(napi_env env, napi_value exports)
{
  napi_status status;

  // OM_MAX_PATH
  EXPORT_INT(OM_MAX_PATH);
  // OM_DOWNLOAD_STATUS;
  EXPORT_INT(OM_DOWNLOAD_NONE);
  EXPORT_INT(OM_DOWNLOAD_ERROR);
  EXPORT_INT(OM_DOWNLOAD_PROGRESS);
  EXPORT_INT(OM_DOWNLOAD_COMPLETE);
  EXPORT_INT(OM_DOWNLOAD_CANCELLED);

  // --- FUNCTIONS ---
  EXPORT_FUNCTION(OmGetDataFileSize);
  EXPORT_FUNCTION(OmGetDataFilename);
  EXPORT_FUNCTION(OmGetDataRange);
  EXPORT_FUNCTION(OmBeginDownloading);
  EXPORT_FUNCTION(OmBeginDownloadingReference);
  EXPORT_FUNCTION(OmQueryDownload);
  // EXPORT_FUNCTION(OmWaitForDownload);
  EXPORT_FUNCTION(OmCancelDownload);
  EXPORT_FUNCTION(OmSetDownloadCallback);
  EXPORT_FUNCTION(OmSetDownloadChunkCallback);
  // ---------------

  // Direct OM callbacks to our code
  OmSetDownloadCallback(downloadCallback, NULL);
  OmSetDownloadChunkCallback(downloadChunkCallback, NULL);

  return exports;
}

