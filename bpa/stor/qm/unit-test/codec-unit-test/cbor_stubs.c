/****************************************************************************
**
** Copyright (C) 2021 Intel Corporation
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
**
****************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cbor header
 */

// Include cbor.h from /usr/local/include/tinycbor if installed or have the
// files in the inc folder temporarily, just for stubs.
#include "cbor.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for _cbor_value_begin_string_iteration()
 * ----------------------------------------------------
 */
CborError _cbor_value_begin_string_iteration(CborValue *value)
{
    UT_GenStub_SetupReturnBuffer(_cbor_value_begin_string_iteration, CborError);

    UT_GenStub_AddParam(_cbor_value_begin_string_iteration, CborValue *, value);

    UT_GenStub_Execute(_cbor_value_begin_string_iteration, Basic, NULL);

    return UT_GenStub_GetReturnValue(_cbor_value_begin_string_iteration, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for _cbor_value_copy_string()
 * ----------------------------------------------------
 */
CborError _cbor_value_copy_string(const CborValue *value, void *buffer, size_t *buflen, CborValue *next)
{
    UT_GenStub_SetupReturnBuffer(_cbor_value_copy_string, CborError);

    UT_GenStub_AddParam(_cbor_value_copy_string, const CborValue *, value);
    UT_GenStub_AddParam(_cbor_value_copy_string, void *, buffer);
    UT_GenStub_AddParam(_cbor_value_copy_string, size_t *, buflen);
    UT_GenStub_AddParam(_cbor_value_copy_string, CborValue *, next);

    UT_GenStub_Execute(_cbor_value_copy_string, Basic, NULL);

    return UT_GenStub_GetReturnValue(_cbor_value_copy_string, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for _cbor_value_decode_int64_internal()
 * ----------------------------------------------------
 */
uint64_t _cbor_value_decode_int64_internal(const CborValue *value)
{
    UT_GenStub_SetupReturnBuffer(_cbor_value_decode_int64_internal, uint64_t);

    UT_GenStub_AddParam(_cbor_value_decode_int64_internal, const CborValue *, value);

    UT_GenStub_Execute(_cbor_value_decode_int64_internal, Basic, NULL);

    return UT_GenStub_GetReturnValue(_cbor_value_decode_int64_internal, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for _cbor_value_dup_string()
 * ----------------------------------------------------
 */
CborError _cbor_value_dup_string(const CborValue *value, void **buffer, size_t *buflen, CborValue *next)
{
    UT_GenStub_SetupReturnBuffer(_cbor_value_dup_string, CborError);

    UT_GenStub_AddParam(_cbor_value_dup_string, const CborValue *, value);
    UT_GenStub_AddParam(_cbor_value_dup_string, void **, buffer);
    UT_GenStub_AddParam(_cbor_value_dup_string, size_t *, buflen);
    UT_GenStub_AddParam(_cbor_value_dup_string, CborValue *, next);

    UT_GenStub_Execute(_cbor_value_dup_string, Basic, NULL);

    return UT_GenStub_GetReturnValue(_cbor_value_dup_string, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for _cbor_value_finish_string_iteration()
 * ----------------------------------------------------
 */
CborError _cbor_value_finish_string_iteration(CborValue *value)
{
    UT_GenStub_SetupReturnBuffer(_cbor_value_finish_string_iteration, CborError);

    UT_GenStub_AddParam(_cbor_value_finish_string_iteration, CborValue *, value);

    UT_GenStub_Execute(_cbor_value_finish_string_iteration, Basic, NULL);

    return UT_GenStub_GetReturnValue(_cbor_value_finish_string_iteration, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for _cbor_value_get_string_chunk()
 * ----------------------------------------------------
 */
CborError _cbor_value_get_string_chunk(const CborValue *value, const void **bufferptr, size_t *len, CborValue *next)
{
    UT_GenStub_SetupReturnBuffer(_cbor_value_get_string_chunk, CborError);

    UT_GenStub_AddParam(_cbor_value_get_string_chunk, const CborValue *, value);
    UT_GenStub_AddParam(_cbor_value_get_string_chunk, const void **, bufferptr);
    UT_GenStub_AddParam(_cbor_value_get_string_chunk, size_t *, len);
    UT_GenStub_AddParam(_cbor_value_get_string_chunk, CborValue *, next);

    UT_GenStub_Execute(_cbor_value_get_string_chunk, Basic, NULL);

    return UT_GenStub_GetReturnValue(_cbor_value_get_string_chunk, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for _cbor_value_get_string_chunk_size()
 * ----------------------------------------------------
 */
CborError _cbor_value_get_string_chunk_size(const CborValue *value, size_t *len)
{
    UT_GenStub_SetupReturnBuffer(_cbor_value_get_string_chunk_size, CborError);

    UT_GenStub_AddParam(_cbor_value_get_string_chunk_size, const CborValue *, value);
    UT_GenStub_AddParam(_cbor_value_get_string_chunk_size, size_t *, len);

    UT_GenStub_Execute(_cbor_value_get_string_chunk_size, Basic, NULL);

    return UT_GenStub_GetReturnValue(_cbor_value_get_string_chunk_size, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_byte_string()
 * ----------------------------------------------------
 */
CborError cbor_encode_byte_string(CborEncoder *encoder, const uint8_t *string, size_t length)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_byte_string, CborError);

    UT_GenStub_AddParam(cbor_encode_byte_string, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_byte_string, const uint8_t *, string);
    UT_GenStub_AddParam(cbor_encode_byte_string, size_t, length);

    UT_GenStub_Execute(cbor_encode_byte_string, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_byte_string, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_float_as_half_float()
 * ----------------------------------------------------
 */
CborError cbor_encode_float_as_half_float(CborEncoder *encoder, float value)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_float_as_half_float, CborError);

    UT_GenStub_AddParam(cbor_encode_float_as_half_float, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_float_as_half_float, float, value);

    UT_GenStub_Execute(cbor_encode_float_as_half_float, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_float_as_half_float, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_floating_point()
 * ----------------------------------------------------
 */
CborError cbor_encode_floating_point(CborEncoder *encoder, CborType fpType, const void *value)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_floating_point, CborError);

    UT_GenStub_AddParam(cbor_encode_floating_point, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_floating_point, CborType, fpType);
    UT_GenStub_AddParam(cbor_encode_floating_point, const void *, value);

    UT_GenStub_Execute(cbor_encode_floating_point, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_floating_point, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_int()
 * ----------------------------------------------------
 */
CborError cbor_encode_int(CborEncoder *encoder, int64_t value)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_int, CborError);

    UT_GenStub_AddParam(cbor_encode_int, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_int, int64_t, value);

    UT_GenStub_Execute(cbor_encode_int, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_int, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_negative_int()
 * ----------------------------------------------------
 */
CborError cbor_encode_negative_int(CborEncoder *encoder, uint64_t absolute_value)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_negative_int, CborError);

    UT_GenStub_AddParam(cbor_encode_negative_int, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_negative_int, uint64_t, absolute_value);

    UT_GenStub_Execute(cbor_encode_negative_int, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_negative_int, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_simple_value()
 * ----------------------------------------------------
 */
CborError cbor_encode_simple_value(CborEncoder *encoder, uint8_t value)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_simple_value, CborError);

    UT_GenStub_AddParam(cbor_encode_simple_value, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_simple_value, uint8_t, value);

    UT_GenStub_Execute(cbor_encode_simple_value, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_simple_value, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_tag()
 * ----------------------------------------------------
 */
CborError cbor_encode_tag(CborEncoder *encoder, CborTag tag)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_tag, CborError);

    UT_GenStub_AddParam(cbor_encode_tag, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_tag, CborTag, tag);

    UT_GenStub_Execute(cbor_encode_tag, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_tag, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_text_string()
 * ----------------------------------------------------
 */
CborError cbor_encode_text_string(CborEncoder *encoder, const char *string, size_t length)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_text_string, CborError);

    UT_GenStub_AddParam(cbor_encode_text_string, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_text_string, const char *, string);
    UT_GenStub_AddParam(cbor_encode_text_string, size_t, length);

    UT_GenStub_Execute(cbor_encode_text_string, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_text_string, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encode_uint()
 * ----------------------------------------------------
 */
CborError cbor_encode_uint(CborEncoder *encoder, uint64_t value)
{
    UT_GenStub_SetupReturnBuffer(cbor_encode_uint, CborError);

    UT_GenStub_AddParam(cbor_encode_uint, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encode_uint, uint64_t, value);

    UT_GenStub_Execute(cbor_encode_uint, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encode_uint, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encoder_close_container()
 * ----------------------------------------------------
 */
CborError cbor_encoder_close_container(CborEncoder *parentEncoder, const CborEncoder *containerEncoder)
{
    UT_GenStub_SetupReturnBuffer(cbor_encoder_close_container, CborError);

    UT_GenStub_AddParam(cbor_encoder_close_container, CborEncoder *, parentEncoder);
    UT_GenStub_AddParam(cbor_encoder_close_container, const CborEncoder *, containerEncoder);

    UT_GenStub_Execute(cbor_encoder_close_container, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encoder_close_container, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encoder_close_container_checked()
 * ----------------------------------------------------
 */
CborError cbor_encoder_close_container_checked(CborEncoder *parentEncoder, const CborEncoder *containerEncoder)
{
    UT_GenStub_SetupReturnBuffer(cbor_encoder_close_container_checked, CborError);

    UT_GenStub_AddParam(cbor_encoder_close_container_checked, CborEncoder *, parentEncoder);
    UT_GenStub_AddParam(cbor_encoder_close_container_checked, const CborEncoder *, containerEncoder);

    UT_GenStub_Execute(cbor_encoder_close_container_checked, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encoder_close_container_checked, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encoder_create_array()
 * ----------------------------------------------------
 */
CborError cbor_encoder_create_array(CborEncoder *parentEncoder, CborEncoder *arrayEncoder, size_t length)
{
    UT_GenStub_SetupReturnBuffer(cbor_encoder_create_array, CborError);

    UT_GenStub_AddParam(cbor_encoder_create_array, CborEncoder *, parentEncoder);
    UT_GenStub_AddParam(cbor_encoder_create_array, CborEncoder *, arrayEncoder);
    UT_GenStub_AddParam(cbor_encoder_create_array, size_t, length);

    UT_GenStub_Execute(cbor_encoder_create_array, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encoder_create_array, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encoder_create_map()
 * ----------------------------------------------------
 */
CborError cbor_encoder_create_map(CborEncoder *parentEncoder, CborEncoder *mapEncoder, size_t length)
{
    UT_GenStub_SetupReturnBuffer(cbor_encoder_create_map, CborError);

    UT_GenStub_AddParam(cbor_encoder_create_map, CborEncoder *, parentEncoder);
    UT_GenStub_AddParam(cbor_encoder_create_map, CborEncoder *, mapEncoder);
    UT_GenStub_AddParam(cbor_encoder_create_map, size_t, length);

    UT_GenStub_Execute(cbor_encoder_create_map, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_encoder_create_map, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encoder_init()
 * ----------------------------------------------------
 */
void cbor_encoder_init(CborEncoder *encoder, uint8_t *buffer, size_t size, int flags)
{
    UT_GenStub_AddParam(cbor_encoder_init, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encoder_init, uint8_t *, buffer);
    UT_GenStub_AddParam(cbor_encoder_init, size_t, size);
    UT_GenStub_AddParam(cbor_encoder_init, int, flags);

    UT_GenStub_Execute(cbor_encoder_init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_encoder_init_writer()
 * ----------------------------------------------------
 */
void cbor_encoder_init_writer(CborEncoder *encoder, CborEncoderWriteFunction writer, void *arg)
{
    UT_GenStub_AddParam(cbor_encoder_init_writer, CborEncoder *, encoder);
    UT_GenStub_AddParam(cbor_encoder_init_writer, CborEncoderWriteFunction, writer);
    UT_GenStub_AddParam(cbor_encoder_init_writer, void *, arg);

    UT_GenStub_Execute(cbor_encoder_init_writer, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_error_string()
 * ----------------------------------------------------
 */
const char *cbor_error_string(CborError error)
{
    UT_GenStub_SetupReturnBuffer(cbor_error_string, const char *);

    UT_GenStub_AddParam(cbor_error_string, CborError, error);

    UT_GenStub_Execute(cbor_error_string, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_error_string, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_parser_init()
 * ----------------------------------------------------
 */
CborError cbor_parser_init(const uint8_t *buffer, size_t size, uint32_t flags, CborParser *parser, CborValue *it)
{
    UT_GenStub_SetupReturnBuffer(cbor_parser_init, CborError);

    UT_GenStub_AddParam(cbor_parser_init, const uint8_t *, buffer);
    UT_GenStub_AddParam(cbor_parser_init, size_t, size);
    UT_GenStub_AddParam(cbor_parser_init, uint32_t, flags);
    UT_GenStub_AddParam(cbor_parser_init, CborParser *, parser);
    UT_GenStub_AddParam(cbor_parser_init, CborValue *, it);

    UT_GenStub_Execute(cbor_parser_init, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_parser_init, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_parser_init_reader()
 * ----------------------------------------------------
 */
CborError cbor_parser_init_reader(const struct CborParserOperations *ops, CborParser *parser, CborValue *it,
                                  void *token)
{
    UT_GenStub_SetupReturnBuffer(cbor_parser_init_reader, CborError);

    UT_GenStub_AddParam(cbor_parser_init_reader, const struct CborParserOperations *, ops);
    UT_GenStub_AddParam(cbor_parser_init_reader, CborParser *, parser);
    UT_GenStub_AddParam(cbor_parser_init_reader, CborValue *, it);
    UT_GenStub_AddParam(cbor_parser_init_reader, void *, token);

    UT_GenStub_Execute(cbor_parser_init_reader, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_parser_init_reader, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_advance()
 * ----------------------------------------------------
 */
CborError cbor_value_advance(CborValue *it)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_advance, CborError);

    UT_GenStub_AddParam(cbor_value_advance, CborValue *, it);

    UT_GenStub_Execute(cbor_value_advance, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_advance, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_advance_fixed()
 * ----------------------------------------------------
 */
CborError cbor_value_advance_fixed(CborValue *it)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_advance_fixed, CborError);

    UT_GenStub_AddParam(cbor_value_advance_fixed, CborValue *, it);

    UT_GenStub_Execute(cbor_value_advance_fixed, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_advance_fixed, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_calculate_string_length()
 * ----------------------------------------------------
 */
CborError cbor_value_calculate_string_length(const CborValue *value, size_t *length)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_calculate_string_length, CborError);

    UT_GenStub_AddParam(cbor_value_calculate_string_length, const CborValue *, value);
    UT_GenStub_AddParam(cbor_value_calculate_string_length, size_t *, length);

    UT_GenStub_Execute(cbor_value_calculate_string_length, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_calculate_string_length, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_enter_container()
 * ----------------------------------------------------
 */
CborError cbor_value_enter_container(const CborValue *it, CborValue *recursed)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_enter_container, CborError);

    UT_GenStub_AddParam(cbor_value_enter_container, const CborValue *, it);
    UT_GenStub_AddParam(cbor_value_enter_container, CborValue *, recursed);

    UT_GenStub_Execute(cbor_value_enter_container, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_enter_container, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_get_half_float_as_float()
 * ----------------------------------------------------
 */
CborError cbor_value_get_half_float_as_float(const CborValue *value, float *result)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_get_half_float_as_float, CborError);

    UT_GenStub_AddParam(cbor_value_get_half_float_as_float, const CborValue *, value);
    UT_GenStub_AddParam(cbor_value_get_half_float_as_float, float *, result);

    UT_GenStub_Execute(cbor_value_get_half_float_as_float, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_get_half_float_as_float, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_get_int64_checked()
 * ----------------------------------------------------
 */
CborError cbor_value_get_int64_checked(const CborValue *value, int64_t *result)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_get_int64_checked, CborError);

    UT_GenStub_AddParam(cbor_value_get_int64_checked, const CborValue *, value);
    UT_GenStub_AddParam(cbor_value_get_int64_checked, int64_t *, result);

    UT_GenStub_Execute(cbor_value_get_int64_checked, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_get_int64_checked, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_get_int_checked()
 * ----------------------------------------------------
 */
CborError cbor_value_get_int_checked(const CborValue *value, int *result)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_get_int_checked, CborError);

    UT_GenStub_AddParam(cbor_value_get_int_checked, const CborValue *, value);
    UT_GenStub_AddParam(cbor_value_get_int_checked, int *, result);

    UT_GenStub_Execute(cbor_value_get_int_checked, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_get_int_checked, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_leave_container()
 * ----------------------------------------------------
 */
CborError cbor_value_leave_container(CborValue *it, const CborValue *recursed)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_leave_container, CborError);

    UT_GenStub_AddParam(cbor_value_leave_container, CborValue *, it);
    UT_GenStub_AddParam(cbor_value_leave_container, const CborValue *, recursed);

    UT_GenStub_Execute(cbor_value_leave_container, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_leave_container, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_map_find_value()
 * ----------------------------------------------------
 */
CborError cbor_value_map_find_value(const CborValue *map, const char *string, CborValue *element)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_map_find_value, CborError);

    UT_GenStub_AddParam(cbor_value_map_find_value, const CborValue *, map);
    UT_GenStub_AddParam(cbor_value_map_find_value, const char *, string);
    UT_GenStub_AddParam(cbor_value_map_find_value, CborValue *, element);

    UT_GenStub_Execute(cbor_value_map_find_value, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_map_find_value, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_reparse()
 * ----------------------------------------------------
 */
CborError cbor_value_reparse(CborValue *it)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_reparse, CborError);

    UT_GenStub_AddParam(cbor_value_reparse, CborValue *, it);

    UT_GenStub_Execute(cbor_value_reparse, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_reparse, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_skip_tag()
 * ----------------------------------------------------
 */
CborError cbor_value_skip_tag(CborValue *it)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_skip_tag, CborError);

    UT_GenStub_AddParam(cbor_value_skip_tag, CborValue *, it);

    UT_GenStub_Execute(cbor_value_skip_tag, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_skip_tag, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_text_string_equals()
 * ----------------------------------------------------
 */
CborError cbor_value_text_string_equals(const CborValue *value, const char *string, bool *result)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_text_string_equals, CborError);

    UT_GenStub_AddParam(cbor_value_text_string_equals, const CborValue *, value);
    UT_GenStub_AddParam(cbor_value_text_string_equals, const char *, string);
    UT_GenStub_AddParam(cbor_value_text_string_equals, bool *, result);

    UT_GenStub_Execute(cbor_value_text_string_equals, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_text_string_equals, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_to_pretty_advance()
 * ----------------------------------------------------
 */
CborError cbor_value_to_pretty_advance(FILE *out, CborValue *value)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_to_pretty_advance, CborError);

    UT_GenStub_AddParam(cbor_value_to_pretty_advance, FILE *, out);
    UT_GenStub_AddParam(cbor_value_to_pretty_advance, CborValue *, value);

    UT_GenStub_Execute(cbor_value_to_pretty_advance, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_to_pretty_advance, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_to_pretty_advance_flags()
 * ----------------------------------------------------
 */
CborError cbor_value_to_pretty_advance_flags(FILE *out, CborValue *value, int flags)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_to_pretty_advance_flags, CborError);

    UT_GenStub_AddParam(cbor_value_to_pretty_advance_flags, FILE *, out);
    UT_GenStub_AddParam(cbor_value_to_pretty_advance_flags, CborValue *, value);
    UT_GenStub_AddParam(cbor_value_to_pretty_advance_flags, int, flags);

    UT_GenStub_Execute(cbor_value_to_pretty_advance_flags, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_to_pretty_advance_flags, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_to_pretty_stream()
 * ----------------------------------------------------
 */
CborError cbor_value_to_pretty_stream(CborStreamFunction streamFunction, void *token, CborValue *value, int flags)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_to_pretty_stream, CborError);

    UT_GenStub_AddParam(cbor_value_to_pretty_stream, CborStreamFunction, streamFunction);
    UT_GenStub_AddParam(cbor_value_to_pretty_stream, void *, token);
    UT_GenStub_AddParam(cbor_value_to_pretty_stream, CborValue *, value);
    UT_GenStub_AddParam(cbor_value_to_pretty_stream, int, flags);

    UT_GenStub_Execute(cbor_value_to_pretty_stream, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_to_pretty_stream, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_validate()
 * ----------------------------------------------------
 */
CborError cbor_value_validate(const CborValue *it, uint32_t flags)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_validate, CborError);

    UT_GenStub_AddParam(cbor_value_validate, const CborValue *, it);
    UT_GenStub_AddParam(cbor_value_validate, uint32_t, flags);

    UT_GenStub_Execute(cbor_value_validate, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_validate, CborError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for cbor_value_validate_basic()
 * ----------------------------------------------------
 */
CborError cbor_value_validate_basic(const CborValue *it)
{
    UT_GenStub_SetupReturnBuffer(cbor_value_validate_basic, CborError);

    UT_GenStub_AddParam(cbor_value_validate_basic, const CborValue *, it);

    UT_GenStub_Execute(cbor_value_validate_basic, Basic, NULL);

    return UT_GenStub_GetReturnValue(cbor_value_validate_basic, CborError);
}
