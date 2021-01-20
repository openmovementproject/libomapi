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


// Wrapped handle so that we can determine whether the reader has been closed (reader == NULL)
typedef struct {
  OmReaderHandle handle;
} omreader_t;


static void finalizeOmReader(napi_env env, void* finalize_data, void* finalize_hint) {
  omreader_t *omreader = (omreader_t *)finalize_data;
  if (omreader->handle != NULL)
  {
    OmReaderClose(omreader->handle);
    omreader->handle = NULL;
  }
  free(omreader);
}


// OM_EXPORT OmReaderHandle OmReaderOpen(const char *binaryFilename);
MYNAPI_FUNCTION(OmReaderOpen) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue);

    state->ref = malloc(sizeof(omreader_t));
    if (!state->ref) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    memset(state->ref, 0, sizeof(omreader_t));
    omreader_t *omreader = (omreader_t *)state->ref;

    size_t sz;
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[0].value, NULL, 0, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
    state->data = malloc(sz + 1);
    if (!state->data) { MYNAPI_RESULT(OM_E_OUT_OF_MEMORY); return false; }
    status = napi_get_value_string_utf8(state->env, MYNAPI_ARG[0].value, state->data, sz, &sz);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    napi_status status;
    omreader_t *omreader = (omreader_t *)state->ref;
    omreader->handle = OmReaderOpen(state->data);
    free(state->data);

    // If successful, wrap and return the omreader_t
    if (omreader->handle != NULL) {
      status = napi_create_external(state->env, (void *)omreader, finalizeOmReader, NULL, &state->resultValue);
      if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_CREATE); }
    } else {
      status = napi_get_null(state->env, &state->resultValue);
      if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_CREATE); }
    }

  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}


// OM_EXPORT int OmReaderDataRange(OmReaderHandle reader, int *dataBlockSize, int *dataOffsetBlocks, int *dataNumBlocks, OM_DATETIME *startTime, OM_DATETIME *endTime);
MYNAPI_FUNCTION(OmReaderDataRange) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec: {
    omreader_t *omreader = (omreader_t *)state->ref;
    state->data = malloc(5 * sizeof(int));
    int *params = (int *)state->data;
    int dataBlockSize = 0;
    int dataOffsetBlocks = 0;
    int dataNumBlocks = 0;
    OM_DATETIME startTime = 0;
    OM_DATETIME endTime = 0;
    MYNAPI_RESULT(OmReaderDataRange(omreader->handle, &dataBlockSize, &dataOffsetBlocks, &dataNumBlocks, &startTime, &endTime));
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

// OM_EXPORT const char *OmReaderMetadata(OmReaderHandle reader, int *deviceId, unsigned int *sessionId);
MYNAPI_FUNCTION(OmReaderMetadata) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec: {
    omreader_t *omreader = (omreader_t *)state->ref;
    state->data = malloc(2 * sizeof(int));
    int *params = (int *)state->data;
    int deviceId = 0;
    int sessionId = 0;
    const char *metadata = OmReaderMetadata(omreader->handle, &deviceId, &sessionId);
    state->resultStr = (char *)malloc(OM_METADATA_SIZE + 1);
    memcpy(state->resultStr, metadata, OM_METADATA_SIZE);
    state->resultStr[OM_METADATA_SIZE] = '\0';
    MYNAPI_RESULT(OmGetMetadata(MYNAPI_ARG[0].intValue, state->resultStr));
    params[0] = deviceId;
    params[1] = sessionId;
  }
  mynapi_complete: {
    const char *metadata = state->resultStr;
    state->resultStr = NULL;  // don't use as return (using array)
    int *params = (int *)state->data;
    napi_status status;

    status = napi_create_array(state->env, &state->resultValue);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }

    napi_value metadataVal;
    status = napi_create_string_utf8(state->env, metadata, NAPI_AUTO_LENGTH, &metadataVal);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
    status = napi_set_element(state->env, state->resultValue, 0, metadataVal);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }

    int i;
    for (i = 0; i < 2; i++)
    {
      napi_value val;
      status = napi_create_int32(state->env, params[i], &val);
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
      status = napi_set_element(state->env, state->resultValue, 1 + i, val); // start at 1
      if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
    }

    MYNAPI_COMPLETE_RESULT();
    free(state->data);
  }
}

// OM_EXPORT int OmReaderDataBlockPosition(OmReaderHandle reader);
MYNAPI_FUNCTION(OmReaderDataBlockPosition) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    MYNAPI_RESULT(OmReaderDataBlockPosition(omreader->handle));
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}

// OM_EXPORT int OmReaderDataBlockSeek(OmReaderHandle reader, int dataBlockNumber);
MYNAPI_FUNCTION(OmReaderDataBlockSeek) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue, paramInt);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    MYNAPI_RESULT(OmReaderDataBlockSeek(omreader->handle, MYNAPI_ARG[1].intValue));
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}

// OM_EXPORT int OmReaderNextBlock(OmReaderHandle reader);
MYNAPI_FUNCTION(OmReaderNextBlock) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    MYNAPI_RESULT(OmReaderNextBlock(omreader->handle));
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}

// OM_EXPORT short *OmReaderBuffer(OmReaderHandle reader);
// This wrapped version takes the parameters OmReaderBuffer(reader, arrayBuffer)
MYNAPI_FUNCTION(OmReaderBuffer) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue, paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }

    napi_get_arraybuffer_info(state->env, MYNAPI_ARG[1].value, &state->data, &state->size);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    short *samples = OmReaderBuffer(omreader->handle);
    const int maxSize = OM_MAX_SAMPLES * sizeof(short);
    size_t sz = state->size;
    if (sz > maxSize) sz = maxSize;
    if (samples == NULL) sz = 0;
    if (samples != NULL) memcpy(state->data, samples, sz);
    MYNAPI_RESULT(sz);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}

// OM_EXPORT OM_DATETIME OmReaderTimestamp(OmReaderHandle reader, int index, unsigned short *fractional);
MYNAPI_FUNCTION(OmReaderTimestamp) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue, paramInt);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    unsigned short fractionalInt = 0;
    OM_DATETIME timestampInt = OmReaderTimestamp(omreader->handle, MYNAPI_ARG[1].intValue, &fractionalInt);
    double timestamp = timestampInt + ((double)fractionalInt / 65536);
    state->valueDouble = timestamp;
  }
  mynapi_complete: {
    napi_status status;
    status = napi_create_double(state->env, state->valueDouble, &state->resultValue);
    if (status != napi_ok) { napi_throw_error(state->env, NULL, ERROR_CREATE); }
    MYNAPI_COMPLETE_RESULT(); 
  }
}

// OM_EXPORT int OmReaderGetValue(OmReaderHandle reader, OM_READER_VALUE_TYPE valueType);
MYNAPI_FUNCTION(OmReaderGetValue) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue, paramInt);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    MYNAPI_RESULT(OmReaderGetValue(omreader->handle, (OM_READER_VALUE_TYPE)MYNAPI_ARG[1].intValue));
    state->result = OM_OK;  // return uses state->resultInt (from MYNAPI_RESULT)
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}

// OM_EXPORT OM_READER_HEADER_PACKET *OmReaderRawHeaderPacket(OmReaderHandle reader);
// This wrapped version takes the parameters OmReaderRawHeaderPacket(reader, arrayBuffer)
MYNAPI_FUNCTION(OmReaderRawHeaderPacket) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue, paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }

    napi_get_arraybuffer_info(state->env, MYNAPI_ARG[1].value, &state->data, &state->size);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    OM_READER_HEADER_PACKET *packet = OmReaderRawHeaderPacket(omreader->handle);
    size_t sz = state->size;
    if (sz > OM_MAX_HEADER_SIZE) sz = OM_MAX_HEADER_SIZE;
    if (packet == NULL) sz = 0;
    if (packet != NULL) memcpy(state->data, packet, sz);
    MYNAPI_RESULT(sz);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}

// OM_EXPORT OM_READER_DATA_PACKET *OmReaderRawDataPacket(OmReaderHandle reader);
// This wrapped version takes the parameters OmReaderRawDataPacket(reader, arrayBuffer)
MYNAPI_FUNCTION(OmReaderRawDataPacket) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue, paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }

    napi_get_arraybuffer_info(state->env, MYNAPI_ARG[1].value, &state->data, &state->size);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    OM_READER_DATA_PACKET *packet = OmReaderRawDataPacket(omreader->handle);
    size_t sz = state->size;
    if (sz > OM_MAX_DATA_SIZE) sz = OM_MAX_DATA_SIZE;
    if (packet == NULL) sz = 0;
    if (packet != NULL) memcpy(state->data, packet, sz);
    MYNAPI_RESULT(sz);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}

// OM_EXPORT void OmReaderClose(OmReaderHandle reader);
MYNAPI_FUNCTION(OmReaderClose) {
  mynapi_params: {
    napi_status status;
    MYNAPI_PARAMS(paramValue);
    status = napi_get_value_external(state->env, MYNAPI_ARG[0].value, &state->ref);
    if (status != napi_ok) { napi_throw_type_error(state->env, NULL, ERROR_ARGS); }
  }
  mynapi_exec:     {
    omreader_t *omreader = (omreader_t *)state->ref;
    OmReaderClose(omreader->handle);
    omreader->handle = NULL;  // prevent finalizer trying to close again
    MYNAPI_RESULT(0);
  }
  mynapi_complete: MYNAPI_COMPLETE_RESULT(); 
}


void CleanupReader(napi_env env)
{
  ;
}

napi_value InitReader(napi_env env, napi_value exports)
{
  napi_status status;

  // OM_MAX_(reader)
  EXPORT_INT(OM_MAX_SAMPLES);
  EXPORT_INT(OM_MAX_HEADER_SIZE);
  EXPORT_INT(OM_MAX_DATA_SIZE);
  // OM_READER_VALUE_TYPE
  EXPORT_INT(OM_VALUE_DEVICEID);
  EXPORT_INT(OM_VALUE_SESSIONID);
  EXPORT_INT(OM_VALUE_SEQUENCEID);
  EXPORT_INT(OM_VALUE_LIGHT);
  EXPORT_INT(OM_VALUE_TEMPERATURE);
  EXPORT_INT(OM_VALUE_EVENTS);
  EXPORT_INT(OM_VALUE_BATTERY);
  EXPORT_INT(OM_VALUE_SAMPLERATE);
  EXPORT_INT(OM_VALUE_LIGHT_LOG10LUXTIMES10POWER3);
  EXPORT_INT(OM_VALUE_TEMPERATURE_MC);
  EXPORT_INT(OM_VALUE_BATTERY_MV);
  EXPORT_INT(OM_VALUE_BATTERY_PERCENT);
  EXPORT_INT(OM_VALUE_AXES);
  EXPORT_INT(OM_VALUE_SCALE_ACCEL);
  EXPORT_INT(OM_VALUE_SCALE_GYRO);
  EXPORT_INT(OM_VALUE_SCALE_MAG);
  EXPORT_INT(OM_VALUE_ACCEL_AXIS);
  EXPORT_INT(OM_VALUE_GYRO_AXIS);
  EXPORT_INT(OM_VALUE_MAG_AXIS);

  // --- FUNCTIONS ---
  EXPORT_FUNCTION(OmReaderOpen);
  EXPORT_FUNCTION(OmReaderDataRange);
  EXPORT_FUNCTION(OmReaderMetadata);
  EXPORT_FUNCTION(OmReaderDataBlockPosition);
  EXPORT_FUNCTION(OmReaderDataBlockSeek);
  EXPORT_FUNCTION(OmReaderNextBlock);
  EXPORT_FUNCTION(OmReaderBuffer);
  EXPORT_FUNCTION(OmReaderTimestamp);
  EXPORT_FUNCTION(OmReaderGetValue);
  EXPORT_FUNCTION(OmReaderRawHeaderPacket);
  EXPORT_FUNCTION(OmReaderRawDataPacket);
  EXPORT_FUNCTION(OmReaderClose);

  // ---------------

  return exports;
}

