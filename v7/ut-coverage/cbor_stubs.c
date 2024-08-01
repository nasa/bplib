/* ===========================================================================
 * Copyright (c) 2016-2018, The Linux Foundation.
 * Copyright (c) 2018-2024, Laurence Lundblade.
 * Copyright (c) 2021, Arm Limited.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors, nor the name "Laurence Lundblade" may be used to
 *       endorse or promote products derived from this software without
 *       specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ========================================================================= */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in qcbor_encode
 * header
 */

#include "qcbor_common.h"
#include "qcbor_private.h"
#include "UsefulBuf.h"
#include "utgenstub.h"


#define QCBOR_MAX_TAGS_PER_ITEM QCBOR_MAX_TAGS_PER_ITEM1
#define QCBOR_TAGSPEC_NUM_TYPES 4

typedef struct _QCBOREncodeContext QCBOREncodeContext;
typedef struct _QCBORDecodeContext QCBORDecodeContext;

typedef struct {
   uint8_t   uNumUsed;
   uint8_t   uNumAllocated;
   uint64_t *puTags;
} QCBORTagListOut;

typedef struct {
   uint8_t         uNumTags;
   const uint64_t *puTags;
} QCBORTagListIn;

typedef UsefulBuf (* QCBORStringAllocate)(void   *pAllocateCxt,
                                          void   *pOldMem,
                                          size_t  uNewSize);
                                          
                                                                                  
typedef enum {
   /** See QCBORDecode_Init() */
   QCBOR_DECODE_MODE_NORMAL = 0,
   /** See QCBORDecode_Init() */
   QCBOR_DECODE_MODE_MAP_STRINGS_ONLY = 1,
   /** See QCBORDecode_Init() */
   QCBOR_DECODE_MODE_MAP_AS_ARRAY = 2
   /* This is stored in uint8_t in places; never add values > 255 */
} QCBORDecodeMode;

typedef struct {
   /* One of QCBOR_TAGSPEC_MATCH_xxx */
   uint8_t uTagRequirement;
   /* The tagged type translated into QCBOR_TYPE_XXX. Used to match
    * explicit tagging */
   uint8_t uTaggedTypes[QCBOR_TAGSPEC_NUM_TYPES];
   /* The types of the content, which are used to match implicit
    * tagging */
   uint8_t uAllowedContentTypes[QCBOR_TAGSPEC_NUM_TYPES];
} QCBOR_Private_TagSpec;

typedef struct _QCBORItem {
   /** Tells what element of the @c val union to use. One of @ref
    *  QCBOR_TYPE_INT64, @ref QCBOR_TYPE_ARRAY, ...*/
   uint8_t  uDataType;

   /** Tells what element of the @c label union to use. One of
    *  @ref QCBOR_TYPE_INT64, @ref QCBOR_TYPE_BYTE_STRING, ...*/
   uint8_t  uLabelType;

   /** Holds the nesting depth for arrays and map. 0 is the top level
    *  with no arrays or maps entered. */
   uint8_t  uNestingLevel;

   /** Holds the nesting level of the next item after this one.  If
    *  less than @c uNestingLevel, this item was the last one in an
    *  arry or map and it closed out at least one nesting level. */
   uint8_t  uNextNestLevel;

   /** 1 if a @c val that is a string is allocated with string
    * allocator, 0 if not. Always 0 unless an allocator has been set
    * up by calling QCBORDecode_SetMemPool() or
    * QCBORDecode_SetUpAllocator(). */
   uint8_t  uDataAlloc;

   /** 1 if a @c label that is a string is allocated with string
    * allocator, 0 if not. Always 0 unless an allocator has been set
    * up by calling QCBORDecode_SetMemPool() or
    * QCBORDecode_SetUpAllocator(). */
   uint8_t  uLabelAlloc;

   /** The union holding the item's value. Select union member based
    *  on @c uDataType. */
   union {
      /** The value for @c uDataType @ref QCBOR_TYPE_INT64. */
      int64_t     int64;
      /** The value for @c uDataType @ref QCBOR_TYPE_UINT64. */
      uint64_t    uint64;
      /** The value for @c uDataType @ref QCBOR_TYPE_BYTE_STRING and
       *  @ref QCBOR_TYPE_TEXT_STRING. Also
       *  for many tags whose content is a string such @ref QCBOR_TYPE_DAYS_STRING
       *  and @ref QCBOR_TYPE_URI. */
      UsefulBufC  string;
      /** The "value" for @c uDataType @ref QCBOR_TYPE_ARRAY or @ref
       *  QCBOR_TYPE_MAP, the number of items in the array or map.  It
       *  is @c UINT16_MAX when decoding indefinite-lengths maps and
       *  arrays. Detection of the end of a map or array is best done
       *  with @c uNestLevel and @c uNextNestLevel so as to work for
       *  both definite and indefinite length maps and arrays. */
      uint16_t    uCount;
#ifndef USEFULBUF_DISABLE_ALL_FLOAT
      /** The value for @c uDataType @ref QCBOR_TYPE_DOUBLE. */
      double      dfnum;
      /** The value for @c uDataType @ref QCBOR_TYPE_FLOAT. */
      float       fnum;
#endif /* USEFULBUF_DISABLE_ALL_FLOAT */
      /** The value for @c uDataType @ref QCBOR_TYPE_DATE_EPOCH, the
       *  number of seconds after or before Jan 1, 1970. This has a
       *  range of 500 billion years. Floating-point dates are
       *  converted to this integer + fractional value. If the input
       *  value is beyond the 500 billion-year range (e.g., +/i
       *  infinity, large floating point values, NaN)
       *  @ref QCBOR_ERR_DATE_OVERFLOW will be returned. If the input
       *  is floating-point and QCBOR has been compiled with
       *  floating-point disabled, one of the various floating-point
       *  disabled errors will be returned. */
      struct {
         int64_t  nSeconds;
#ifndef USEFULBUF_DISABLE_ALL_FLOAT
         double   fSecondsFraction;
#endif /* USEFULBUF_DISABLE_ALL_FLOAT */
      } epochDate;

      /** The value for @c uDataType @ref QCBOR_TYPE_DAYS_EPOCH -- the
       *  number of days before or after Jan 1, 1970. */
      int64_t     epochDays;
      /** No longer used. Was the value for @ref QCBOR_TYPE_DATE_STRING,
       * but now that value is in @c string. This will be removed in QCBOR 2.0. */
      UsefulBufC  dateString;
      /** The value for @c uDataType @ref QCBOR_TYPE_POSBIGNUM and
       * @ref QCBOR_TYPE_NEGBIGNUM.  */
      UsefulBufC  bigNum;
      /** See @ref QCBOR_TYPE_UKNOWN_SIMPLE */
      uint8_t     uSimple;
#ifndef QCBOR_DISABLE_EXP_AND_MANTISSA
      /**
       * @anchor expAndMantissa
       *
       * This holds the value for big floats and decimal fractions.
       * The use of the fields in this structure depends on @c
       * uDataType.
       *
       * When @c uDataType indicates a decimal fraction, the
       * @c nExponent is base 10. When it indicates a big float, it
       * is base 2.
       *
       * When @c uDataType indicates a big number, then the @c bigNum
       * member of @c Mantissa is valid. Otherwise the @c nInt member
       * of @c Mantissa is valid.
       *
       * See @ref QCBOR_TYPE_DECIMAL_FRACTION,
       * @ref QCBOR_TYPE_DECIMAL_FRACTION_POS_BIGNUM,
       * @ref QCBOR_TYPE_DECIMAL_FRACTION_NEG_BIGNUM,
       * @ref QCBOR_TYPE_BIGFLOAT, @ref QCBOR_TYPE_BIGFLOAT_POS_BIGNUM,
       * and @ref QCBOR_TYPE_BIGFLOAT_NEG_BIGNUM.
       *
       * Also see QCBOREncode_AddTDecimalFraction(),
       * QCBOREncode_AddTBigFloat(),
       * QCBOREncode_AddTDecimalFractionBigNum() and
       * QCBOREncode_AddTBigFloatBigNum().
       */
      struct {
         int64_t nExponent;
         union {
            int64_t    nInt;
            UsefulBufC bigNum;
         } Mantissa;
      } expAndMantissa;
#endif /* QCBOR_DISABLE_EXP_AND_MANTISSA */
      uint64_t    uTagV;  /* Used internally during decoding */

   } val;

   /** Union holding the different label types selected based on @c uLabelType */
   union {
      /** The label for @c uLabelType @ref QCBOR_TYPE_BYTE_STRING and
       *  @ref QCBOR_TYPE_TEXT_STRING */
      UsefulBufC  string;
      /** The label for @c uLabelType for @ref QCBOR_TYPE_INT64 */
      int64_t     int64;
      /** The label for @c uLabelType for @ref QCBOR_TYPE_UINT64 */
      uint64_t    uint64;
   } label;

#ifndef QCBOR_DISABLE_TAGS
   /**
    * The tags numbers for which the item is the tag content.  Tags
    * nest, so index 0 in the array is the tag on the data item
    * itself, index 1 is the tag that applies to the tag in index
    * 0. The end of the list is indicated by @ref CBOR_TAG_INVALID16
    *
    * Tag nesting is uncommon and rarely deep. This implementation
    * only allows nesting to a depth of @ref QCBOR_MAX_TAGS_PER_ITEM,
    * usually 4.
    *
    * Tag numbers in the array below and equal to @ref
    * QCBOR_LAST_UNMAPPED_TAG are unmapped and can be used
    * directly. Tag numbers above this must be translated through
    * QCBORDecode_GetNthTag().
    *
    * See also the large number of functions like
    * QCBORDecode_GetEpochDate() and QCBORDecode_GetBignum() in
    * qcbor_spiffy_decode.h for a way to decode tagged types without
    * having to reference this array. Also see @ref Tags-Overview.
    */
   uint16_t uTags[QCBOR_MAX_TAGS_PER_ITEM];
#endif

} QCBORItem;

typedef QCBORError (*QCBORItemCallback)(void            *pCallbackCtx,
                                        const QCBORItem *pItem);


/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddB64Text()
 * ----------------------------------------------------
 */
void QCBOREncode_AddB64Text(QCBOREncodeContext *pCtx,
                                   UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddB64Text, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddB64Text, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddB64Text, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddB64TextToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddB64TextToMap(QCBOREncodeContext *pCtx,
                                        const char *szLabel,
                                        UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddB64TextToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddB64TextToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddB64TextToMap, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddB64TextToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddB64TextToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddB64TextToMapN(QCBOREncodeContext *pCtx,
                                         int64_t nLabel, UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddB64TextToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddB64TextToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddB64TextToMapN, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddB64TextToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddB64URLText()
 * ----------------------------------------------------
 */
void QCBOREncode_AddB64URLText(QCBOREncodeContext *pCtx,
                                      UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddB64URLText, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddB64URLText, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddB64URLText, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddB64URLTextToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddB64URLTextToMap(QCBOREncodeContext *pCtx,
                                           const char *szLabel,
                                           UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddB64URLTextToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddB64URLTextToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddB64URLTextToMap, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddB64URLTextToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddB64URLTextToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddB64URLTextToMapN(QCBOREncodeContext *pCtx,
                                            int64_t nLabel,
                                            UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddB64URLTextToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddB64URLTextToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddB64URLTextToMapN, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddB64URLTextToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBigFloat()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBigFloat(QCBOREncodeContext *pCtx, int64_t nMantissa,
                                    int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddBigFloat, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloat, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloat, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddBigFloat, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBigFloatBigNum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBigFloatBigNum(QCBOREncodeContext *pCtx,
                                          UsefulBufC Mantissa, bool bIsNegative,
                                          int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNum, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNum, UsefulBufC, Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNum, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNum, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddBigFloatBigNum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBigFloatBigNumToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBigFloatBigNumToMap(QCBOREncodeContext *pCtx,
                                               const char *szLabel,
                                               UsefulBufC Mantissa,
                                               bool bIsNegative,
                                               int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMap, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMap, UsefulBufC, Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMap, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMap, int64_t,
                      nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddBigFloatBigNumToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBigFloatBigNumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBigFloatBigNumToMapN(QCBOREncodeContext *pCtx,
                                                int64_t nLabel,
                                                UsefulBufC Mantissa,
                                                bool bIsNegative,
                                                int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMapN, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMapN, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatBigNumToMapN, int64_t,
                      nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddBigFloatBigNumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBigFloatToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBigFloatToMap(QCBOREncodeContext *pCtx,
                                         const char *szLabel, int64_t nMantissa,
                                         int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMap, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMap, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddBigFloatToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBigFloatToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBigFloatToMapN(QCBOREncodeContext *pCtx,
                                          int64_t nLabel, int64_t nMantissa,
                                          int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMapN, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddBigFloatToMapN, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddBigFloatToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBinaryUUID()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBinaryUUID(QCBOREncodeContext *pCtx,
                                      UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUID, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUID, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBinaryUUID, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBinaryUUIDToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBinaryUUIDToMap(QCBOREncodeContext *pCtx,
                                           const char *szLabel,
                                           UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUIDToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUIDToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUIDToMap, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBinaryUUIDToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBinaryUUIDToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBinaryUUIDToMapN(QCBOREncodeContext *pCtx,
                                            int64_t nLabel, UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUIDToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUIDToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBinaryUUIDToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBinaryUUIDToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBool()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBool(QCBOREncodeContext *pCtx, bool b) {

  UT_GenStub_AddParam(QCBOREncode_AddBool, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBool, bool, b);

  UT_GenStub_Execute(QCBOREncode_AddBool, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBoolToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBoolToMap(QCBOREncodeContext *pCtx,
                                     const char *szLabel, bool b) {

  UT_GenStub_AddParam(QCBOREncode_AddBoolToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBoolToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBoolToMap, bool, b);

  UT_GenStub_Execute(QCBOREncode_AddBoolToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBoolToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBoolToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                      bool b) {

  UT_GenStub_AddParam(QCBOREncode_AddBoolToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBoolToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBoolToMapN, bool, b);

  UT_GenStub_Execute(QCBOREncode_AddBoolToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBytes()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBytes(QCBOREncodeContext *pCtx, UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBytes, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBytes, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBytes, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBytesLenOnly()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBytesLenOnly(QCBOREncodeContext *pCtx,
                                        UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnly, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnly, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBytesLenOnly, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBytesLenOnlyToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBytesLenOnlyToMap(QCBOREncodeContext *pCtx,
                                             const char *szLabel,
                                             UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnlyToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnlyToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnlyToMap, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBytesLenOnlyToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBytesLenOnlyToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBytesLenOnlyToMapN(QCBOREncodeContext *pCtx,
                                              int64_t nLabel,
                                              UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnlyToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnlyToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBytesLenOnlyToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBytesLenOnlyToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBytesToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBytesToMap(QCBOREncodeContext *pCtx,
                                      const char *szLabel, UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBytesToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBytesToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBytesToMap, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBytesToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddBytesToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddBytesToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                       UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddBytesToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddBytesToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddBytesToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddBytesToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDateEpoch()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDateEpoch(QCBOREncodeContext *pCtx, int64_t nDate) {

  UT_GenStub_AddParam(QCBOREncode_AddDateEpoch, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDateEpoch, int64_t, nDate);

  UT_GenStub_Execute(QCBOREncode_AddDateEpoch, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDateEpochToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDateEpochToMap(QCBOREncodeContext *pCtx,
                                          const char *szLabel, int64_t nDate) {

  UT_GenStub_AddParam(QCBOREncode_AddDateEpochToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDateEpochToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDateEpochToMap, int64_t, nDate);

  UT_GenStub_Execute(QCBOREncode_AddDateEpochToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDateEpochToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDateEpochToMapN(QCBOREncodeContext *pCtx,
                                           int64_t nLabel, int64_t nDate) {

  UT_GenStub_AddParam(QCBOREncode_AddDateEpochToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDateEpochToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDateEpochToMapN, int64_t, nDate);

  UT_GenStub_Execute(QCBOREncode_AddDateEpochToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDateString()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDateString(QCBOREncodeContext *pCtx,
                                      const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddDateString, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDateString, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddDateString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDateStringToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDateStringToMap(QCBOREncodeContext *pCtx,
                                           const char *szLabel,
                                           const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddDateStringToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDateStringToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDateStringToMap, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddDateStringToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDateStringToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDateStringToMapN(QCBOREncodeContext *pCtx,
                                            int64_t nLabel,
                                            const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddDateStringToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDateStringToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDateStringToMapN, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddDateStringToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDecimalFraction()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDecimalFraction(QCBOREncodeContext *pCtx,
                                           int64_t nMantissa,
                                           int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddDecimalFraction, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFraction, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFraction, int64_t, nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddDecimalFraction, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDecimalFractionBigNum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDecimalFractionBigNum(QCBOREncodeContext *pCtx,
                                                 UsefulBufC Mantissa,
                                                 bool bIsNegative,
                                                 int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNum,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNum, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNum, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNum, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddDecimalFractionBigNum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDecimalFractionBigNumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDecimalFractionBigNumToMapN(
    QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Mantissa,
    bool bIsNegative, int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapN, int64_t,
                      nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapN, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapN, bool,
                      bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapN, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddDecimalFractionBigNumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDecimalFractionBigNumToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDecimalFractionBigNumToMapSZ(
    QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Mantissa,
    bool bIsNegative, int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapSZ,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapSZ, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapSZ, bool,
                      bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionBigNumToMapSZ, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddDecimalFractionBigNumToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDecimalFractionToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDecimalFractionToMap(QCBOREncodeContext *pCtx,
                                                const char *szLabel,
                                                int64_t nMantissa,
                                                int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMap, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMap, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMap, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddDecimalFractionToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDecimalFractionToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDecimalFractionToMapN(QCBOREncodeContext *pCtx,
                                                 int64_t nLabel,
                                                 int64_t nMantissa,
                                                 int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMapN, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddDecimalFractionToMapN, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddDecimalFractionToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDouble()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDouble(QCBOREncodeContext *pCtx, double dNum) {

  UT_GenStub_AddParam(QCBOREncode_AddDouble, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDouble, double, dNum);

  UT_GenStub_Execute(QCBOREncode_AddDouble, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDoubleNoPreferred()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDoubleNoPreferred(QCBOREncodeContext *pCtx,
                                             double dNum) {

  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferred, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferred, double, dNum);

  UT_GenStub_Execute(QCBOREncode_AddDoubleNoPreferred, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDoubleNoPreferredToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDoubleNoPreferredToMap(QCBOREncodeContext *pCtx,
                                                  const char *szLabel,
                                                  double dNum) {

  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferredToMap,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferredToMap, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferredToMap, double, dNum);

  UT_GenStub_Execute(QCBOREncode_AddDoubleNoPreferredToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDoubleNoPreferredToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDoubleNoPreferredToMapN(QCBOREncodeContext *pCtx,
                                                   int64_t nLabel,
                                                   double dNum) {

  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferredToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferredToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleNoPreferredToMapN, double, dNum);

  UT_GenStub_Execute(QCBOREncode_AddDoubleNoPreferredToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDoubleToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDoubleToMap(QCBOREncodeContext *pCtx,
                                       const char *szLabel, double dNum) {

  UT_GenStub_AddParam(QCBOREncode_AddDoubleToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleToMap, double, dNum);

  UT_GenStub_Execute(QCBOREncode_AddDoubleToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddDoubleToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddDoubleToMapN(QCBOREncodeContext *pCtx,
                                        int64_t nLabel, double dNum) {

  UT_GenStub_AddParam(QCBOREncode_AddDoubleToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddDoubleToMapN, double, dNum);

  UT_GenStub_Execute(QCBOREncode_AddDoubleToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddEncoded()
 * ----------------------------------------------------
 */
void QCBOREncode_AddEncoded(QCBOREncodeContext *pCtx, UsefulBufC Encoded) {
  UT_GenStub_AddParam(QCBOREncode_AddEncoded, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddEncoded, UsefulBufC, Encoded);

  UT_GenStub_Execute(QCBOREncode_AddEncoded, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddEncodedToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddEncodedToMap(QCBOREncodeContext *pCtx,
                                        const char *szLabel,
                                        UsefulBufC Encoded) {

  UT_GenStub_AddParam(QCBOREncode_AddEncodedToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddEncodedToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddEncodedToMap, UsefulBufC, Encoded);

  UT_GenStub_Execute(QCBOREncode_AddEncodedToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddEncodedToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddEncodedToMapN(QCBOREncodeContext *pCtx,
                                         int64_t nLabel, UsefulBufC Encoded) {

  UT_GenStub_AddParam(QCBOREncode_AddEncodedToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddEncodedToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddEncodedToMapN, UsefulBufC, Encoded);

  UT_GenStub_Execute(QCBOREncode_AddEncodedToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddFloat()
 * ----------------------------------------------------
 */
void QCBOREncode_AddFloat(QCBOREncodeContext *pCtx, float fNum) {

  UT_GenStub_AddParam(QCBOREncode_AddFloat, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddFloat, float, fNum);

  UT_GenStub_Execute(QCBOREncode_AddFloat, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddFloatNoPreferred()
 * ----------------------------------------------------
 */
void QCBOREncode_AddFloatNoPreferred(QCBOREncodeContext *pCtx,
                                            float fNum) {

  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferred, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferred, float, fNum);

  UT_GenStub_Execute(QCBOREncode_AddFloatNoPreferred, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddFloatNoPreferredToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddFloatNoPreferredToMap(QCBOREncodeContext *pCtx,
                                                 const char *szLabel,
                                                 float fNum) {

  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferredToMap,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferredToMap, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferredToMap, float, fNum);

  UT_GenStub_Execute(QCBOREncode_AddFloatNoPreferredToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddFloatNoPreferredToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddFloatNoPreferredToMapN(QCBOREncodeContext *pCtx,
                                                  int64_t nLabel, float fNum) {

  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferredToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferredToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddFloatNoPreferredToMapN, float, fNum);

  UT_GenStub_Execute(QCBOREncode_AddFloatNoPreferredToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddFloatToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddFloatToMap(QCBOREncodeContext *pCtx,
                                      const char *szLabel, float fNum) {

  UT_GenStub_AddParam(QCBOREncode_AddFloatToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddFloatToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddFloatToMap, float, fNum);

  UT_GenStub_Execute(QCBOREncode_AddFloatToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddFloatToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddFloatToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                       float dNum) {

  UT_GenStub_AddParam(QCBOREncode_AddFloatToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddFloatToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddFloatToMapN, float, dNum);

  UT_GenStub_Execute(QCBOREncode_AddFloatToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddInt64()
 * ----------------------------------------------------
 */
void QCBOREncode_AddInt64(QCBOREncodeContext *pCtx, int64_t nNum) {
  UT_GenStub_AddParam(QCBOREncode_AddInt64, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddInt64, int64_t, nNum);

  UT_GenStub_Execute(QCBOREncode_AddInt64, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddInt64ToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddInt64ToMap(QCBOREncodeContext *pCtx,
                                      const char *szLabel, int64_t uNum) {

  UT_GenStub_AddParam(QCBOREncode_AddInt64ToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddInt64ToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddInt64ToMap, int64_t, uNum);

  UT_GenStub_Execute(QCBOREncode_AddInt64ToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddInt64ToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddInt64ToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                       int64_t uNum) {

  UT_GenStub_AddParam(QCBOREncode_AddInt64ToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddInt64ToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddInt64ToMapN, int64_t, uNum);

  UT_GenStub_Execute(QCBOREncode_AddInt64ToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddMIMEData()
 * ----------------------------------------------------
 */
void QCBOREncode_AddMIMEData(QCBOREncodeContext *pCtx,
                                    UsefulBufC MIMEData) {

  UT_GenStub_AddParam(QCBOREncode_AddMIMEData, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddMIMEData, UsefulBufC, MIMEData);

  UT_GenStub_Execute(QCBOREncode_AddMIMEData, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddMIMEDataToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddMIMEDataToMap(QCBOREncodeContext *pCtx,
                                         const char *szLabel,
                                         UsefulBufC MIMEData) {

  UT_GenStub_AddParam(QCBOREncode_AddMIMEDataToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddMIMEDataToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddMIMEDataToMap, UsefulBufC, MIMEData);

  UT_GenStub_Execute(QCBOREncode_AddMIMEDataToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddMIMEDataToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddMIMEDataToMapN(QCBOREncodeContext *pCtx,
                                          int64_t nLabel, UsefulBufC MIMEData) {

  UT_GenStub_AddParam(QCBOREncode_AddMIMEDataToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddMIMEDataToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddMIMEDataToMapN, UsefulBufC, MIMEData);

  UT_GenStub_Execute(QCBOREncode_AddMIMEDataToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddNULL()
 * ----------------------------------------------------
 */
void QCBOREncode_AddNULL(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_AddNULL, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_AddNULL, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddNULLToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddNULLToMap(QCBOREncodeContext *pCtx,
                                     const char *szLabel) {

  UT_GenStub_AddParam(QCBOREncode_AddNULLToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddNULLToMap, const char *, szLabel);

  UT_GenStub_Execute(QCBOREncode_AddNULLToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddNULLToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddNULLToMapN(QCBOREncodeContext *pCtx,
                                      int64_t nLabel) {

  UT_GenStub_AddParam(QCBOREncode_AddNULLToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddNULLToMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBOREncode_AddNULLToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddNegativeBignum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddNegativeBignum(QCBOREncodeContext *pCtx,
                                          UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignum, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignum, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddNegativeBignum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddNegativeBignumToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddNegativeBignumToMap(QCBOREncodeContext *pCtx,
                                               const char *szLabel,
                                               UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignumToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignumToMap, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignumToMap, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddNegativeBignumToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddNegativeBignumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddNegativeBignumToMapN(QCBOREncodeContext *pCtx,
                                                int64_t nLabel,
                                                UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignumToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignumToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddNegativeBignumToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddNegativeBignumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddPositiveBignum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddPositiveBignum(QCBOREncodeContext *pCtx,
                                          UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignum, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignum, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddPositiveBignum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddPositiveBignumToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddPositiveBignumToMap(QCBOREncodeContext *pCtx,
                                               const char *szLabel,
                                               UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignumToMap, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignumToMap, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignumToMap, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddPositiveBignumToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddPositiveBignumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddPositiveBignumToMapN(QCBOREncodeContext *pCtx,
                                                int64_t nLabel,
                                                UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignumToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignumToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddPositiveBignumToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddPositiveBignumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddRegex()
 * ----------------------------------------------------
 */
void QCBOREncode_AddRegex(QCBOREncodeContext *pCtx, UsefulBufC Regex) {

  UT_GenStub_AddParam(QCBOREncode_AddRegex, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddRegex, UsefulBufC, Regex);

  UT_GenStub_Execute(QCBOREncode_AddRegex, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddRegexToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddRegexToMap(QCBOREncodeContext *pCtx,
                                      const char *szLabel, UsefulBufC Regex) {

  UT_GenStub_AddParam(QCBOREncode_AddRegexToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddRegexToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddRegexToMap, UsefulBufC, Regex);

  UT_GenStub_Execute(QCBOREncode_AddRegexToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddRegexToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddRegexToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                       UsefulBufC Regex) {

  UT_GenStub_AddParam(QCBOREncode_AddRegexToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddRegexToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddRegexToMapN, UsefulBufC, Regex);

  UT_GenStub_Execute(QCBOREncode_AddRegexToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddSZString()
 * ----------------------------------------------------
 */
inline void QCBOREncode_AddSZString(QCBOREncodeContext *pMe,
                                           const char *szString) {

  UT_GenStub_AddParam(QCBOREncode_AddSZString, QCBOREncodeContext *, pMe);
  UT_GenStub_AddParam(QCBOREncode_AddSZString, const char *, szString);

  UT_GenStub_Execute(QCBOREncode_AddSZString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddSZStringToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddSZStringToMap(QCBOREncodeContext *pCtx,
                                         const char *szLabel,
                                         const char *szString) {

  UT_GenStub_AddParam(QCBOREncode_AddSZStringToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddSZStringToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddSZStringToMap, const char *, szString);

  UT_GenStub_Execute(QCBOREncode_AddSZStringToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddSZStringToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddSZStringToMapN(QCBOREncodeContext *pCtx,
                                          int64_t nLabel,
                                          const char *szString) {

  UT_GenStub_AddParam(QCBOREncode_AddSZStringToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddSZStringToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddSZStringToMapN, const char *, szString);

  UT_GenStub_Execute(QCBOREncode_AddSZStringToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddSimple()
 * ----------------------------------------------------
 */
void QCBOREncode_AddSimple(QCBOREncodeContext *pMe,
                                  const uint64_t uNum) {

  UT_GenStub_AddParam(QCBOREncode_AddSimple, QCBOREncodeContext *, pMe);
  UT_GenStub_AddParam(QCBOREncode_AddSimple, const uint64_t, uNum);

  UT_GenStub_Execute(QCBOREncode_AddSimple, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddSimpleToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddSimpleToMap(QCBOREncodeContext *pMe,
                                       const char *szLabel,
                                       const uint8_t uSimple) {

  UT_GenStub_AddParam(QCBOREncode_AddSimpleToMap, QCBOREncodeContext *, pMe);
  UT_GenStub_AddParam(QCBOREncode_AddSimpleToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddSimpleToMap, const uint8_t, uSimple);

  UT_GenStub_Execute(QCBOREncode_AddSimpleToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddSimpleToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddSimpleToMapN(QCBOREncodeContext *pMe,
                                        const int64_t nLabel,
                                        const uint8_t uSimple) {

  UT_GenStub_AddParam(QCBOREncode_AddSimpleToMapN, QCBOREncodeContext *, pMe);
  UT_GenStub_AddParam(QCBOREncode_AddSimpleToMapN, const int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddSimpleToMapN, const uint8_t, uSimple);

  UT_GenStub_Execute(QCBOREncode_AddSimpleToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTB64Text()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTB64Text(QCBOREncodeContext *pCtx,
                                    uint8_t uTagRequirement,
                                    UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTB64Text, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTB64Text, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTB64Text, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddTB64Text, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTB64TextToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTB64TextToMapN(QCBOREncodeContext *pCtx,
                                          int64_t nLabel,
                                          uint8_t uTagRequirement,
                                          UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapN, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddTB64TextToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTB64TextToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTB64TextToMapSZ(QCBOREncodeContext *pCtx,
                                           const char *szLabel,
                                           uint8_t uTagRequirement,
                                           UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTB64TextToMapSZ, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddTB64TextToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTB64URLText()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTB64URLText(QCBOREncodeContext *pCtx,
                                       uint8_t uTagRequirement,
                                       UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTB64URLText, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLText, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLText, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddTB64URLText, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTB64URLTextToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTB64URLTextToMapN(QCBOREncodeContext *pCtx,
                                             int64_t nLabel,
                                             uint8_t uTagRequirement,
                                             UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapN, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddTB64URLTextToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTB64URLTextToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTB64URLTextToMapSZ(QCBOREncodeContext *pCtx,
                                              const char *szLabel,
                                              uint8_t uTagRequirement,
                                              UsefulBufC B64Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTB64URLTextToMapSZ, UsefulBufC, B64Text);

  UT_GenStub_Execute(QCBOREncode_AddTB64URLTextToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBigFloat()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBigFloat(QCBOREncodeContext *pCtx,
                                     uint8_t uTagRequirement, int64_t nMantissa,
                                     int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTBigFloat, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloat, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloat, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloat, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTBigFloat, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBigFloatBigNum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBigFloatBigNum(QCBOREncodeContext *pCtx,
                                           uint8_t uTagRequirement,
                                           UsefulBufC Mantissa,
                                           bool bIsNegative,
                                           int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNum, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNum, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNum, UsefulBufC, Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNum, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNum, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTBigFloatBigNum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBigFloatBigNumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBigFloatBigNumToMapN(
    QCBOREncodeContext *pCtx, int64_t nLabel, uint8_t uTagRequirement,
    UsefulBufC Mantissa, bool bIsNegative, int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapN, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapN, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapN, int64_t,
                      nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTBigFloatBigNumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBigFloatBigNumToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBigFloatBigNumToMapSZ(
    QCBOREncodeContext *pCtx, const char *szLabel, uint8_t uTagRequirement,
    UsefulBufC Mantissa, bool bIsNegative, int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapSZ,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapSZ, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapSZ, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatBigNumToMapSZ, int64_t,
                      nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTBigFloatBigNumToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBigFloatToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBigFloatToMapN(QCBOREncodeContext *pCtx,
                                           int64_t nLabel,
                                           uint8_t uTagRequirement,
                                           int64_t nMantissa,
                                           int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapN, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapN, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTBigFloatToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBigFloatToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBigFloatToMapSZ(QCBOREncodeContext *pCtx,
                                            const char *szLabel,
                                            uint8_t uTagRequirement,
                                            int64_t nMantissa,
                                            int64_t nBase2Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapSZ, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTBigFloatToMapSZ, int64_t, nBase2Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTBigFloatToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBinaryUUID()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBinaryUUID(QCBOREncodeContext *pCtx,
                                       uint8_t uTagRequirement,
                                       UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUID, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUID, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUID, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTBinaryUUID, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBinaryUUIDToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBinaryUUIDToMapN(QCBOREncodeContext *pCtx,
                                             int64_t nLabel,
                                             uint8_t uTagRequirement,
                                             UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTBinaryUUIDToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTBinaryUUIDToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTBinaryUUIDToMapSZ(QCBOREncodeContext *pCtx,
                                              const char *szLabel,
                                              uint8_t uTagRequirement,
                                              UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTBinaryUUIDToMapSZ, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTBinaryUUIDToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDateEpoch()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDateEpoch(QCBOREncodeContext *pCtx,
                                      uint8_t uTagRequirement, int64_t nDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDateEpoch, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpoch, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpoch, int64_t, nDate);

  UT_GenStub_Execute(QCBOREncode_AddTDateEpoch, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDateEpochToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDateEpochToMapN(QCBOREncodeContext *pCtx,
                                            int64_t nLabel,
                                            uint8_t uTagRequirement,
                                            int64_t nDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapN, int64_t, nDate);

  UT_GenStub_Execute(QCBOREncode_AddTDateEpochToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDateEpochToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDateEpochToMapSZ(QCBOREncodeContext *pCtx,
                                             const char *szLabel,
                                             uint8_t uTagRequirement,
                                             int64_t nDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDateEpochToMapSZ, int64_t, nDate);

  UT_GenStub_Execute(QCBOREncode_AddTDateEpochToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDateString()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDateString(QCBOREncodeContext *pCtx,
                                       uint8_t uTagRequirement,
                                       const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDateString, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDateString, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDateString, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddTDateString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDateStringToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDateStringToMapN(QCBOREncodeContext *pCtx,
                                             int64_t nLabel,
                                             uint8_t uTagRequirement,
                                             const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapN, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddTDateStringToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDateStringToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDateStringToMapSZ(QCBOREncodeContext *pCtx,
                                              const char *szLabel,
                                              uint8_t uTagRequirement,
                                              const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDateStringToMapSZ, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddTDateStringToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDaysEpoch()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDaysEpoch(QCBOREncodeContext *pCtx,
                                      uint8_t uTagRequirement, int64_t nDays) {

  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpoch, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpoch, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpoch, int64_t, nDays);

  UT_GenStub_Execute(QCBOREncode_AddTDaysEpoch, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDaysEpochToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDaysEpochToMapN(QCBOREncodeContext *pCtx,
                                            int64_t nLabel,
                                            uint8_t uTagRequirement,
                                            int64_t nDays) {

  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapN, int64_t, nDays);

  UT_GenStub_Execute(QCBOREncode_AddTDaysEpochToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDaysEpochToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDaysEpochToMapSZ(QCBOREncodeContext *pCtx,
                                             const char *szLabel,
                                             uint8_t uTagRequirement,
                                             int64_t nDays) {

  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysEpochToMapSZ, int64_t, nDays);

  UT_GenStub_Execute(QCBOREncode_AddTDaysEpochToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDaysString()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDaysString(QCBOREncodeContext *pCtx,
                                       uint8_t uTagRequirement,
                                       const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDaysString, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysString, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysString, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddTDaysString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDaysStringToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDaysStringToMapN(QCBOREncodeContext *pCtx,
                                             int64_t nLabel,
                                             uint8_t uTagRequirement,
                                             const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapN, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddTDaysStringToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDaysStringToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDaysStringToMapSZ(QCBOREncodeContext *pCtx,
                                              const char *szLabel,
                                              uint8_t uTagRequirement,
                                              const char *szDate) {

  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDaysStringToMapSZ, const char *, szDate);

  UT_GenStub_Execute(QCBOREncode_AddTDaysStringToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDecimalFraction()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDecimalFraction(QCBOREncodeContext *pCtx,
                                            uint8_t uTagRequirement,
                                            int64_t nMantissa,
                                            int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFraction, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFraction, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFraction, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFraction, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTDecimalFraction, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDecimalFractionBigNum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDecimalFractionBigNum(QCBOREncodeContext *pCtx,
                                                  uint8_t uTagRequirement,
                                                  UsefulBufC Mantissa,
                                                  bool bIsNegative,
                                                  int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNum,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNum, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNum, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNum, bool, bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNum, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTDecimalFractionBigNum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDecimalFractionBigNumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDecimalFractionBigNumToMapN(
    QCBOREncodeContext *pCtx, int64_t nLabel, uint8_t uTagRequirement,
    UsefulBufC Mantissa, bool bIsNegative, int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapN, int64_t,
                      nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapN, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapN, bool,
                      bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapN, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTDecimalFractionBigNumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDecimalFractionBigNumToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDecimalFractionBigNumToMapSZ(
    QCBOREncodeContext *pCtx, const char *szLabel, uint8_t uTagRequirement,
    UsefulBufC Mantissa, bool bIsNegative, int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapSZ,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapSZ,
                      const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapSZ, UsefulBufC,
                      Mantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapSZ, bool,
                      bIsNegative);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionBigNumToMapSZ, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTDecimalFractionBigNumToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDecimalFractionToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDecimalFractionToMapN(QCBOREncodeContext *pCtx,
                                                  int64_t nLabel,
                                                  uint8_t uTagRequirement,
                                                  int64_t nMantissa,
                                                  int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapN, int64_t,
                      nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapN, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTDecimalFractionToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTDecimalFractionToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTDecimalFractionToMapSZ(QCBOREncodeContext *pCtx,
                                                   const char *szLabel,
                                                   uint8_t uTagRequirement,
                                                   int64_t nMantissa,
                                                   int64_t nBase10Exponent) {

  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapSZ,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapSZ, int64_t,
                      nMantissa);
  UT_GenStub_AddParam(QCBOREncode_AddTDecimalFractionToMapSZ, int64_t,
                      nBase10Exponent);

  UT_GenStub_Execute(QCBOREncode_AddTDecimalFractionToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTMIMEData()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTMIMEData(QCBOREncodeContext *pCtx,
                                     uint8_t uTagRequirement,
                                     UsefulBufC MIMEData) {

  UT_GenStub_AddParam(QCBOREncode_AddTMIMEData, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEData, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEData, UsefulBufC, MIMEData);

  UT_GenStub_Execute(QCBOREncode_AddTMIMEData, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTMIMEDataToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTMIMEDataToMapN(QCBOREncodeContext *pCtx,
                                           int64_t nLabel,
                                           uint8_t uTagRequirement,
                                           UsefulBufC MIMEData) {

  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapN, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapN, UsefulBufC, MIMEData);

  UT_GenStub_Execute(QCBOREncode_AddTMIMEDataToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTMIMEDataToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTMIMEDataToMapSZ(QCBOREncodeContext *pCtx,
                                            const char *szLabel,
                                            uint8_t uTagRequirement,
                                            UsefulBufC MIMEData) {

  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapSZ, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTMIMEDataToMapSZ, UsefulBufC, MIMEData);

  UT_GenStub_Execute(QCBOREncode_AddTMIMEDataToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTNegativeBignum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTNegativeBignum(QCBOREncodeContext *pCtx,
                                           uint8_t uTagRequirement,
                                           UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignum, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignum, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignum, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTNegativeBignum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTNegativeBignumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTNegativeBignumToMapN(QCBOREncodeContext *pCtx,
                                                 int64_t nLabel,
                                                 uint8_t uTagRequirement,
                                                 UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTNegativeBignumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTNegativeBignumToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTNegativeBignumToMapSZ(QCBOREncodeContext *pCtx,
                                                  const char *szLabel,
                                                  uint8_t uTagRequirement,
                                                  UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapSZ,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTNegativeBignumToMapSZ, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTNegativeBignumToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTPositiveBignum()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTPositiveBignum(QCBOREncodeContext *pCtx,
                                           uint8_t uTagRequirement,
                                           UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignum, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignum, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignum, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTPositiveBignum, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTPositiveBignumToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTPositiveBignumToMapN(QCBOREncodeContext *pCtx,
                                                 int64_t nLabel,
                                                 uint8_t uTagRequirement,
                                                 UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapN, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTPositiveBignumToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTPositiveBignumToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTPositiveBignumToMapSZ(QCBOREncodeContext *pCtx,
                                                  const char *szLabel,
                                                  uint8_t uTagRequirement,
                                                  UsefulBufC Bytes) {

  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapSZ,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTPositiveBignumToMapSZ, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_AddTPositiveBignumToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTRegex()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTRegex(QCBOREncodeContext *pCtx,
                                  uint8_t uTagRequirement, UsefulBufC Regex) {

  UT_GenStub_AddParam(QCBOREncode_AddTRegex, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTRegex, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTRegex, UsefulBufC, Regex);

  UT_GenStub_Execute(QCBOREncode_AddTRegex, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTRegexToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTRegexToMapN(QCBOREncodeContext *pCtx,
                                        int64_t nLabel, uint8_t uTagRequirement,
                                        UsefulBufC Regex) {

  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapN, UsefulBufC, Regex);

  UT_GenStub_Execute(QCBOREncode_AddTRegexToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTRegexToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTRegexToMapSZ(QCBOREncodeContext *pCtx,
                                         const char *szLabel,
                                         uint8_t uTagRequirement,
                                         UsefulBufC Regex) {

  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapSZ, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTRegexToMapSZ, UsefulBufC, Regex);

  UT_GenStub_Execute(QCBOREncode_AddTRegexToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTURI()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTURI(QCBOREncodeContext *pCtx,
                                uint8_t uTagRequirement, UsefulBufC URI) {

  UT_GenStub_AddParam(QCBOREncode_AddTURI, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTURI, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTURI, UsefulBufC, URI);

  UT_GenStub_Execute(QCBOREncode_AddTURI, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTURIToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTURIToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                      uint8_t uTagRequirement, UsefulBufC URI) {

  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapN, UsefulBufC, URI);

  UT_GenStub_Execute(QCBOREncode_AddTURIToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTURIToMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTURIToMapSZ(QCBOREncodeContext *pCtx,
                                       const char *szLabel,
                                       uint8_t uTagRequirement,
                                       UsefulBufC URI) {

  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapSZ, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBOREncode_AddTURIToMapSZ, UsefulBufC, URI);

  UT_GenStub_Execute(QCBOREncode_AddTURIToMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTag()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTag(QCBOREncodeContext *pCtx, uint64_t uTag) {

  UT_GenStub_AddParam(QCBOREncode_AddTag, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTag, uint64_t, uTag);

  UT_GenStub_Execute(QCBOREncode_AddTag, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddText()
 * ----------------------------------------------------
 */
void QCBOREncode_AddText(QCBOREncodeContext *pCtx, UsefulBufC Text) {

  UT_GenStub_AddParam(QCBOREncode_AddText, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddText, UsefulBufC, Text);

  UT_GenStub_Execute(QCBOREncode_AddText, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTextToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTextToMap(QCBOREncodeContext *pCtx,
                                     const char *szLabel, UsefulBufC Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTextToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTextToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTextToMap, UsefulBufC, Text);

  UT_GenStub_Execute(QCBOREncode_AddTextToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddTextToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddTextToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                      UsefulBufC Text) {

  UT_GenStub_AddParam(QCBOREncode_AddTextToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddTextToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddTextToMapN, UsefulBufC, Text);

  UT_GenStub_Execute(QCBOREncode_AddTextToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddUInt64()
 * ----------------------------------------------------
 */
void QCBOREncode_AddUInt64(QCBOREncodeContext *pCtx, uint64_t uNum) {

  UT_GenStub_AddParam(QCBOREncode_AddUInt64, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddUInt64, uint64_t, uNum);

  UT_GenStub_Execute(QCBOREncode_AddUInt64, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddUInt64ToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddUInt64ToMap(QCBOREncodeContext *pCtx,
                                       const char *szLabel, uint64_t uNum) {

  UT_GenStub_AddParam(QCBOREncode_AddUInt64ToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddUInt64ToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddUInt64ToMap, uint64_t, uNum);

  UT_GenStub_Execute(QCBOREncode_AddUInt64ToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddUInt64ToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddUInt64ToMapN(QCBOREncodeContext *pCtx,
                                        int64_t nLabel, uint64_t uNum) {

  UT_GenStub_AddParam(QCBOREncode_AddUInt64ToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddUInt64ToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddUInt64ToMapN, uint64_t, uNum);

  UT_GenStub_Execute(QCBOREncode_AddUInt64ToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddURI()
 * ----------------------------------------------------
 */
void QCBOREncode_AddURI(QCBOREncodeContext *pCtx, UsefulBufC URI) {

  UT_GenStub_AddParam(QCBOREncode_AddURI, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddURI, UsefulBufC, URI);

  UT_GenStub_Execute(QCBOREncode_AddURI, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddURIToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddURIToMap(QCBOREncodeContext *pCtx,
                                    const char *szLabel, UsefulBufC URI) {

  UT_GenStub_AddParam(QCBOREncode_AddURIToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddURIToMap, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_AddURIToMap, UsefulBufC, URI);

  UT_GenStub_Execute(QCBOREncode_AddURIToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddURIToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddURIToMapN(QCBOREncodeContext *pCtx, int64_t nLabel,
                                     UsefulBufC URI) {

  UT_GenStub_AddParam(QCBOREncode_AddURIToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddURIToMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_AddURIToMapN, UsefulBufC, URI);

  UT_GenStub_Execute(QCBOREncode_AddURIToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddUndef()
 * ----------------------------------------------------
 */
void QCBOREncode_AddUndef(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_AddUndef, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_AddUndef, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddUndefToMap()
 * ----------------------------------------------------
 */
void QCBOREncode_AddUndefToMap(QCBOREncodeContext *pCtx,
                                      const char *szLabel) {

  UT_GenStub_AddParam(QCBOREncode_AddUndefToMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddUndefToMap, const char *, szLabel);

  UT_GenStub_Execute(QCBOREncode_AddUndefToMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_AddUndefToMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_AddUndefToMapN(QCBOREncodeContext *pCtx,
                                       int64_t nLabel) {

  UT_GenStub_AddParam(QCBOREncode_AddUndefToMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_AddUndefToMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBOREncode_AddUndefToMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_BstrWrap()
 * ----------------------------------------------------
 */
void QCBOREncode_BstrWrap(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_BstrWrap, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_BstrWrap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_BstrWrapInMap()
 * ----------------------------------------------------
 */
void QCBOREncode_BstrWrapInMap(QCBOREncodeContext *pCtx,
                                      const char *szLabel) {

  UT_GenStub_AddParam(QCBOREncode_BstrWrapInMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_BstrWrapInMap, const char *, szLabel);

  UT_GenStub_Execute(QCBOREncode_BstrWrapInMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_BstrWrapInMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_BstrWrapInMapN(QCBOREncodeContext *pCtx,
                                       int64_t nLabel) {

  UT_GenStub_AddParam(QCBOREncode_BstrWrapInMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_BstrWrapInMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBOREncode_BstrWrapInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CancelBstrWrap()
 * ----------------------------------------------------
 */
void QCBOREncode_CancelBstrWrap(QCBOREncodeContext *pCtx) {
  UT_GenStub_AddParam(QCBOREncode_CancelBstrWrap, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_CancelBstrWrap, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CloseArray()
 * ----------------------------------------------------
 */
void QCBOREncode_CloseArray(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_CloseArray, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_CloseArray, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CloseArrayIndefiniteLength()
 * ----------------------------------------------------
 */
void QCBOREncode_CloseArrayIndefiniteLength(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_CloseArrayIndefiniteLength,
                      QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_CloseArrayIndefiniteLength, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CloseBstrWrap()
 * ----------------------------------------------------
 */
void QCBOREncode_CloseBstrWrap(QCBOREncodeContext *pCtx,
                                      UsefulBufC *pWrappedCBOR) {

  UT_GenStub_AddParam(QCBOREncode_CloseBstrWrap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_CloseBstrWrap, UsefulBufC *, pWrappedCBOR);

  UT_GenStub_Execute(QCBOREncode_CloseBstrWrap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CloseBstrWrap2()
 * ----------------------------------------------------
 */
void QCBOREncode_CloseBstrWrap2(QCBOREncodeContext *pCtx, bool bIncludeCBORHead,
                                UsefulBufC *pWrappedCBOR) {
  UT_GenStub_AddParam(QCBOREncode_CloseBstrWrap2, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_CloseBstrWrap2, bool, bIncludeCBORHead);
  UT_GenStub_AddParam(QCBOREncode_CloseBstrWrap2, UsefulBufC *, pWrappedCBOR);

  UT_GenStub_Execute(QCBOREncode_CloseBstrWrap2, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CloseBytes()
 * ----------------------------------------------------
 */
void QCBOREncode_CloseBytes(QCBOREncodeContext *pCtx, size_t uAmount) {
  UT_GenStub_AddParam(QCBOREncode_CloseBytes, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_CloseBytes, size_t, uAmount);

  UT_GenStub_Execute(QCBOREncode_CloseBytes, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CloseMap()
 * ----------------------------------------------------
 */
void QCBOREncode_CloseMap(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_CloseMap, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_CloseMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_CloseMapIndefiniteLength()
 * ----------------------------------------------------
 */
void QCBOREncode_CloseMapIndefiniteLength(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_CloseMapIndefiniteLength,
                      QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_CloseMapIndefiniteLength, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_EncodeHead()
 * ----------------------------------------------------
 */
UsefulBufC QCBOREncode_EncodeHead(UsefulBuf Buffer, uint8_t uMajorType,
                                  uint8_t uMinLen, uint64_t uNumber) {
  UT_GenStub_SetupReturnBuffer(QCBOREncode_EncodeHead, UsefulBufC);

  UT_GenStub_AddParam(QCBOREncode_EncodeHead, UsefulBuf, Buffer);
  UT_GenStub_AddParam(QCBOREncode_EncodeHead, uint8_t, uMajorType);
  UT_GenStub_AddParam(QCBOREncode_EncodeHead, uint8_t, uMinLen);
  UT_GenStub_AddParam(QCBOREncode_EncodeHead, uint64_t, uNumber);

  UT_GenStub_Execute(QCBOREncode_EncodeHead, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBOREncode_EncodeHead, UsefulBufC);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Finish()
 * ----------------------------------------------------
 */
QCBORError QCBOREncode_Finish(QCBOREncodeContext *pCtx,
                              UsefulBufC *pEncodedCBOR) {
  UT_GenStub_SetupReturnBuffer(QCBOREncode_Finish, QCBORError);

  UT_GenStub_AddParam(QCBOREncode_Finish, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_Finish, UsefulBufC *, pEncodedCBOR);

  UT_GenStub_Execute(QCBOREncode_Finish, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBOREncode_Finish, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_FinishGetSize()
 * ----------------------------------------------------
 */
QCBORError QCBOREncode_FinishGetSize(QCBOREncodeContext *pCtx,
                                     size_t *uEncodedLen) {
  UT_GenStub_SetupReturnBuffer(QCBOREncode_FinishGetSize, QCBORError);

  UT_GenStub_AddParam(QCBOREncode_FinishGetSize, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_FinishGetSize, size_t *, uEncodedLen);

  UT_GenStub_Execute(QCBOREncode_FinishGetSize, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBOREncode_FinishGetSize, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_GetErrorState()
 * ----------------------------------------------------
 */
QCBORError QCBOREncode_GetErrorState(QCBOREncodeContext *pCtx) {
  UT_GenStub_SetupReturnBuffer(QCBOREncode_GetErrorState, QCBORError);

  UT_GenStub_AddParam(QCBOREncode_GetErrorState, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_GetErrorState, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBOREncode_GetErrorState,
                                   QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Init()
 * ----------------------------------------------------
 */
void QCBOREncode_Init(QCBOREncodeContext *pCtx, UsefulBuf Storage) {
  UT_GenStub_AddParam(QCBOREncode_Init, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_Init, UsefulBuf, Storage);

  UT_GenStub_Execute(QCBOREncode_Init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_IsBufferNULL()
 * ----------------------------------------------------
 */
int QCBOREncode_IsBufferNULL(QCBOREncodeContext *pCtx) {
  UT_GenStub_SetupReturnBuffer(QCBOREncode_IsBufferNULL, int);

  UT_GenStub_AddParam(QCBOREncode_IsBufferNULL, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_IsBufferNULL, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBOREncode_IsBufferNULL, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenArray()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenArray(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_OpenArray, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_OpenArray, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenArrayInMap()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenArrayInMap(QCBOREncodeContext *pCtx,
                                       const char *szLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenArrayInMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenArrayInMap, const char *, szLabel);

  UT_GenStub_Execute(QCBOREncode_OpenArrayInMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenArrayInMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenArrayInMapN(QCBOREncodeContext *pCtx,
                                        int64_t nLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenArrayInMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenArrayInMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBOREncode_OpenArrayInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenArrayIndefiniteLength()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenArrayIndefiniteLength(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_OpenArrayIndefiniteLength,
                      QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_OpenArrayIndefiniteLength, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenArrayIndefiniteLengthInMap()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenArrayIndefiniteLengthInMap(QCBOREncodeContext *pCtx,
                                                       const char *szLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenArrayIndefiniteLengthInMap,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenArrayIndefiniteLengthInMap, const char *,
                      szLabel);

  UT_GenStub_Execute(QCBOREncode_OpenArrayIndefiniteLengthInMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenArrayIndefiniteLengthInMapN()
 * ----------------------------------------------------
 */
void
QCBOREncode_OpenArrayIndefiniteLengthInMapN(QCBOREncodeContext *pCtx,
                                            int64_t nLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenArrayIndefiniteLengthInMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenArrayIndefiniteLengthInMapN, int64_t,
                      nLabel);

  UT_GenStub_Execute(QCBOREncode_OpenArrayIndefiniteLengthInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenBytes()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenBytes(QCBOREncodeContext *pCtx, UsefulBuf *pPlace) {
  UT_GenStub_AddParam(QCBOREncode_OpenBytes, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenBytes, UsefulBuf *, pPlace);

  UT_GenStub_Execute(QCBOREncode_OpenBytes, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenBytesInMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenBytesInMapN(QCBOREncodeContext *pCtx,
                                        int64_t nLabel, UsefulBuf *pPlace) {

  UT_GenStub_AddParam(QCBOREncode_OpenBytesInMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenBytesInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBOREncode_OpenBytesInMapN, UsefulBuf *, pPlace);

  UT_GenStub_Execute(QCBOREncode_OpenBytesInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenBytesInMapSZ()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenBytesInMapSZ(QCBOREncodeContext *pCtx,
                                         const char *szLabel,
                                         UsefulBuf *pPlace) {

  UT_GenStub_AddParam(QCBOREncode_OpenBytesInMapSZ, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenBytesInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBOREncode_OpenBytesInMapSZ, UsefulBuf *, pPlace);

  UT_GenStub_Execute(QCBOREncode_OpenBytesInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenMap()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenMap(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_OpenMap, QCBOREncodeContext *, pCtx);

  UT_GenStub_Execute(QCBOREncode_OpenMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenMapInMap()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenMapInMap(QCBOREncodeContext *pCtx,
                                     const char *szLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenMapInMap, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenMapInMap, const char *, szLabel);

  UT_GenStub_Execute(QCBOREncode_OpenMapInMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenMapInMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenMapInMapN(QCBOREncodeContext *pCtx,
                                      int64_t nLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenMapInMapN, QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenMapInMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBOREncode_OpenMapInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenMapIndefiniteLength()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenMapIndefiniteLength(QCBOREncodeContext *pCtx) {

  UT_GenStub_AddParam(QCBOREncode_OpenMapIndefiniteLength, QCBOREncodeContext *,
                      pCtx);

  UT_GenStub_Execute(QCBOREncode_OpenMapIndefiniteLength, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenMapIndefiniteLengthInMap()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenMapIndefiniteLengthInMap(QCBOREncodeContext *pCtx,
                                                     const char *szLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenMapIndefiniteLengthInMap,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenMapIndefiniteLengthInMap, const char *,
                      szLabel);

  UT_GenStub_Execute(QCBOREncode_OpenMapIndefiniteLengthInMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_OpenMapIndefiniteLengthInMapN()
 * ----------------------------------------------------
 */
void QCBOREncode_OpenMapIndefiniteLengthInMapN(QCBOREncodeContext *pCtx,
                                                      int64_t nLabel) {

  UT_GenStub_AddParam(QCBOREncode_OpenMapIndefiniteLengthInMapN,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_OpenMapIndefiniteLengthInMapN, int64_t,
                      nLabel);

  UT_GenStub_Execute(QCBOREncode_OpenMapIndefiniteLengthInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Private_AddBuffer()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_AddBuffer(QCBOREncodeContext *pCtx, uint8_t uMajorType,
                                   UsefulBufC Bytes) {
  UT_GenStub_AddParam(QCBOREncode_Private_AddBuffer, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_Private_AddBuffer, uint8_t, uMajorType);
  UT_GenStub_AddParam(QCBOREncode_Private_AddBuffer, UsefulBufC, Bytes);

  UT_GenStub_Execute(QCBOREncode_Private_AddBuffer, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Private_AddExpMantissa()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_AddExpMantissa(QCBOREncodeContext *pCtx, uint64_t uTag,
                                        UsefulBufC BigNumMantissa,
                                        bool bBigNumIsNegative,
                                        int64_t nMantissa, int64_t nExponent) {
  UT_GenStub_AddParam(QCBOREncode_Private_AddExpMantissa, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_Private_AddExpMantissa, uint64_t, uTag);
  UT_GenStub_AddParam(QCBOREncode_Private_AddExpMantissa, UsefulBufC,
                      BigNumMantissa);
  UT_GenStub_AddParam(QCBOREncode_Private_AddExpMantissa, bool,
                      bBigNumIsNegative);
  UT_GenStub_AddParam(QCBOREncode_Private_AddExpMantissa, int64_t, nMantissa);
  UT_GenStub_AddParam(QCBOREncode_Private_AddExpMantissa, int64_t, nExponent);

  UT_GenStub_Execute(QCBOREncode_Private_AddExpMantissa, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Private_AddPreferredDouble()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_AddPreferredDouble(QCBOREncodeContext *pMe,
                                            const double dNum) {
  UT_GenStub_AddParam(QCBOREncode_Private_AddPreferredDouble,
                      QCBOREncodeContext *, pMe);
  UT_GenStub_AddParam(QCBOREncode_Private_AddPreferredDouble, const double,
                      dNum);

  UT_GenStub_Execute(QCBOREncode_Private_AddPreferredDouble, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Private_AddPreferredFloat()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_AddPreferredFloat(QCBOREncodeContext *pMe,
                                           const float fNum) {
  UT_GenStub_AddParam(QCBOREncode_Private_AddPreferredFloat,
                      QCBOREncodeContext *, pMe);
  UT_GenStub_AddParam(QCBOREncode_Private_AddPreferredFloat, const float, fNum);

  UT_GenStub_Execute(QCBOREncode_Private_AddPreferredFloat, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Private_AppendCBORHead()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_AppendCBORHead(QCBOREncodeContext *pMe,
                                        const uint8_t uMajorType,
                                        const uint64_t uArgument,
                                        const uint8_t uMinLen) {
  UT_GenStub_AddParam(QCBOREncode_Private_AppendCBORHead, QCBOREncodeContext *,
                      pMe);
  UT_GenStub_AddParam(QCBOREncode_Private_AppendCBORHead, const uint8_t,
                      uMajorType);
  UT_GenStub_AddParam(QCBOREncode_Private_AppendCBORHead, const uint64_t,
                      uArgument);
  UT_GenStub_AddParam(QCBOREncode_Private_AppendCBORHead, const uint8_t,
                      uMinLen);

  UT_GenStub_Execute(QCBOREncode_Private_AppendCBORHead, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Private_CloseMapOrArray()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_CloseMapOrArray(QCBOREncodeContext *pCtx,
                                         uint8_t uMajorType) {
  UT_GenStub_AddParam(QCBOREncode_Private_CloseMapOrArray, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_Private_CloseMapOrArray, uint8_t, uMajorType);

  UT_GenStub_Execute(QCBOREncode_Private_CloseMapOrArray, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for
 * QCBOREncode_Private_CloseMapOrArrayIndefiniteLength()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_CloseMapOrArrayIndefiniteLength(
    QCBOREncodeContext *pCtx, uint8_t uMajorType) {
  UT_GenStub_AddParam(QCBOREncode_Private_CloseMapOrArrayIndefiniteLength,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_Private_CloseMapOrArrayIndefiniteLength,
                      uint8_t, uMajorType);

  UT_GenStub_Execute(QCBOREncode_Private_CloseMapOrArrayIndefiniteLength, Basic,
                     NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBOREncode_Private_OpenMapOrArray()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_OpenMapOrArray(QCBOREncodeContext *pCtx,
                                        uint8_t uMajorType) {
  UT_GenStub_AddParam(QCBOREncode_Private_OpenMapOrArray, QCBOREncodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBOREncode_Private_OpenMapOrArray, uint8_t, uMajorType);

  UT_GenStub_Execute(QCBOREncode_Private_OpenMapOrArray, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for
 * QCBOREncode_Private_OpenMapOrArrayIndefiniteLength()
 * ----------------------------------------------------
 */
void QCBOREncode_Private_OpenMapOrArrayIndefiniteLength(
    QCBOREncodeContext *pCtx, uint8_t uMajorType) {
  UT_GenStub_AddParam(QCBOREncode_Private_OpenMapOrArrayIndefiniteLength,
                      QCBOREncodeContext *, pCtx);
  UT_GenStub_AddParam(QCBOREncode_Private_OpenMapOrArrayIndefiniteLength,
                      uint8_t, uMajorType);

  UT_GenStub_Execute(QCBOREncode_Private_OpenMapOrArrayIndefiniteLength, Basic,
                     NULL);
}



/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in qcbor_decode
 * header
 */

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Finish()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_Finish(QCBORDecodeContext *pCtx) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_Finish, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_Finish, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_Finish, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_Finish, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetAndResetError()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_GetAndResetError(QCBORDecodeContext *pCtx) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_GetAndResetError, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_GetAndResetError, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_GetAndResetError, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_GetAndResetError,
                                   QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetError()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_GetError(QCBORDecodeContext *pCtx) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_GetError, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_GetError, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_GetError, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_GetError, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetNext()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_GetNext(QCBORDecodeContext *pCtx,
                               QCBORItem *pDecodedItem) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_GetNext, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_GetNext, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetNext, QCBORItem *, pDecodedItem);

  UT_GenStub_Execute(QCBORDecode_GetNext, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_GetNext, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetNextWithTags()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_GetNextWithTags(QCBORDecodeContext *pCtx,
                                       QCBORItem *pDecodedItem,
                                       QCBORTagListOut *pTagList) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_GetNextWithTags, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_GetNextWithTags, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetNextWithTags, QCBORItem *, pDecodedItem);
  UT_GenStub_AddParam(QCBORDecode_GetNextWithTags, QCBORTagListOut *, pTagList);

  UT_GenStub_Execute(QCBORDecode_GetNextWithTags, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_GetNextWithTags, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetNthTag()
 * ----------------------------------------------------
 */
uint64_t QCBORDecode_GetNthTag(QCBORDecodeContext *pCtx, const QCBORItem *pItem,
                               uint32_t uIndex) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_GetNthTag, uint64_t);

  UT_GenStub_AddParam(QCBORDecode_GetNthTag, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetNthTag, const QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_GetNthTag, uint32_t, uIndex);

  UT_GenStub_Execute(QCBORDecode_GetNthTag, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_GetNthTag, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetNthTagOfLast()
 * ----------------------------------------------------
 */
uint64_t QCBORDecode_GetNthTagOfLast(const QCBORDecodeContext *pCtx,
                                     uint32_t uIndex) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_GetNthTagOfLast, uint64_t);

  UT_GenStub_AddParam(QCBORDecode_GetNthTagOfLast, const QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetNthTagOfLast, uint32_t, uIndex);

  UT_GenStub_Execute(QCBORDecode_GetNthTagOfLast, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_GetNthTagOfLast, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Init()
 * ----------------------------------------------------
 */
void QCBORDecode_Init(QCBORDecodeContext *pCtx, UsefulBufC EncodedCBOR,
                      QCBORDecodeMode nMode) {
  UT_GenStub_AddParam(QCBORDecode_Init, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Init, UsefulBufC, EncodedCBOR);
  UT_GenStub_AddParam(QCBORDecode_Init, QCBORDecodeMode, nMode);

  UT_GenStub_Execute(QCBORDecode_Init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_IsNotWellFormedError()
 * ----------------------------------------------------
 */
bool QCBORDecode_IsNotWellFormedError(QCBORError uErr) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_IsNotWellFormedError, bool);

  UT_GenStub_AddParam(QCBORDecode_IsNotWellFormedError, QCBORError, uErr);

  UT_GenStub_Execute(QCBORDecode_IsNotWellFormedError, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_IsNotWellFormedError,
                                   bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_IsTagged()
 * ----------------------------------------------------
 */
bool QCBORDecode_IsTagged(QCBORDecodeContext *pCtx, const QCBORItem *pItem,
                          uint64_t uTag) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_IsTagged, bool);

  UT_GenStub_AddParam(QCBORDecode_IsTagged, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_IsTagged, const QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_IsTagged, uint64_t, uTag);

  UT_GenStub_Execute(QCBORDecode_IsTagged, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_IsTagged, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_IsUnrecoverableError()
 * ----------------------------------------------------
 */
bool QCBORDecode_IsUnrecoverableError(QCBORError uErr) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_IsUnrecoverableError, bool);

  UT_GenStub_AddParam(QCBORDecode_IsUnrecoverableError, QCBORError, uErr);

  UT_GenStub_Execute(QCBORDecode_IsUnrecoverableError, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_IsUnrecoverableError,
                                   bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_PartialFinish()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_PartialFinish(QCBORDecodeContext *pCtx,
                                     size_t *puConsumed) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_PartialFinish, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_PartialFinish, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_PartialFinish, size_t *, puConsumed);

  UT_GenStub_Execute(QCBORDecode_PartialFinish, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_PartialFinish, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_PeekNext()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_PeekNext(QCBORDecodeContext *pCtx,
                                QCBORItem *pDecodedItem) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_PeekNext, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_PeekNext, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_PeekNext, QCBORItem *, pDecodedItem);

  UT_GenStub_Execute(QCBORDecode_PeekNext, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_PeekNext, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_SetCallerConfiguredTagList()
 * ----------------------------------------------------
 */
void QCBORDecode_SetCallerConfiguredTagList(QCBORDecodeContext *pCtx,
                                            const QCBORTagListIn *pTagList) {
  UT_GenStub_AddParam(QCBORDecode_SetCallerConfiguredTagList,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_SetCallerConfiguredTagList,
                      const QCBORTagListIn *, pTagList);

  UT_GenStub_Execute(QCBORDecode_SetCallerConfiguredTagList, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_SetError()
 * ----------------------------------------------------
 */
void QCBORDecode_SetError(QCBORDecodeContext *pCtx, QCBORError uError) {

  UT_GenStub_AddParam(QCBORDecode_SetError, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_SetError, QCBORError, uError);

  UT_GenStub_Execute(QCBORDecode_SetError, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_SetMemPool()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_SetMemPool(QCBORDecodeContext *pCtx, UsefulBuf MemPool,
                                  bool bAllStrings) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_SetMemPool, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_SetMemPool, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_SetMemPool, UsefulBuf, MemPool);
  UT_GenStub_AddParam(QCBORDecode_SetMemPool, bool, bAllStrings);

  UT_GenStub_Execute(QCBORDecode_SetMemPool, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_SetMemPool, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_SetUpAllocator()
 * ----------------------------------------------------
 */
void QCBORDecode_SetUpAllocator(QCBORDecodeContext *pCtx,
                                QCBORStringAllocate pfAllocateFunction,
                                void *pAllocateContext, bool bAllStrings) {
  UT_GenStub_AddParam(QCBORDecode_SetUpAllocator, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_SetUpAllocator, QCBORStringAllocate,
                      pfAllocateFunction);
  UT_GenStub_AddParam(QCBORDecode_SetUpAllocator, void *, pAllocateContext);
  UT_GenStub_AddParam(QCBORDecode_SetUpAllocator, bool, bAllStrings);

  UT_GenStub_Execute(QCBORDecode_SetUpAllocator, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Tell()
 * ----------------------------------------------------
 */
uint32_t QCBORDecode_Tell(QCBORDecodeContext *pCtx) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_Tell, uint32_t);

  UT_GenStub_AddParam(QCBORDecode_Tell, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_Tell, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_Tell, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_VGetNext()
 * ----------------------------------------------------
 */
void QCBORDecode_VGetNext(QCBORDecodeContext *pCtx, QCBORItem *pDecodedItem) {
  UT_GenStub_AddParam(QCBORDecode_VGetNext, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_VGetNext, QCBORItem *, pDecodedItem);

  UT_GenStub_Execute(QCBORDecode_VGetNext, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_VGetNextConsume()
 * ----------------------------------------------------
 */
void QCBORDecode_VGetNextConsume(QCBORDecodeContext *pCtx,
                                 QCBORItem *pDecodedItem) {
  UT_GenStub_AddParam(QCBORDecode_VGetNextConsume, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_VGetNextConsume, QCBORItem *, pDecodedItem);

  UT_GenStub_Execute(QCBORDecode_VGetNextConsume, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_VPeekNext()
 * ----------------------------------------------------
 */
void QCBORDecode_VPeekNext(QCBORDecodeContext *pCtx, QCBORItem *pDecodedItem) {
  UT_GenStub_AddParam(QCBORDecode_VPeekNext, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_VPeekNext, QCBORItem *, pDecodedItem);

  UT_GenStub_Execute(QCBORDecode_VPeekNext, Basic, NULL);
}

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in
 * qcbor_spiffy_decode header
 */
/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterArray()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterArray(QCBORDecodeContext *pCtx, QCBORItem *pItem) {

  UT_GenStub_AddParam(QCBORDecode_EnterArray, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_EnterArray, QCBORItem *, pItem);

  UT_GenStub_Execute(QCBORDecode_EnterArray, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterArrayFromMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterArrayFromMapN(QCBORDecodeContext *pMe, int64_t uLabel) {
  UT_GenStub_AddParam(QCBORDecode_EnterArrayFromMapN, QCBORDecodeContext *,
                      pMe);
  UT_GenStub_AddParam(QCBORDecode_EnterArrayFromMapN, int64_t, uLabel);

  UT_GenStub_Execute(QCBORDecode_EnterArrayFromMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterArrayFromMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterArrayFromMapSZ(QCBORDecodeContext *pMe,
                                     const char *szLabel) {
  UT_GenStub_AddParam(QCBORDecode_EnterArrayFromMapSZ, QCBORDecodeContext *,
                      pMe);
  UT_GenStub_AddParam(QCBORDecode_EnterArrayFromMapSZ, const char *, szLabel);

  UT_GenStub_Execute(QCBORDecode_EnterArrayFromMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterBstrWrapped()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterBstrWrapped(QCBORDecodeContext *pCtx,
                                  uint8_t uTagRequirement, UsefulBufC *pBstr) {
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrapped, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrapped, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrapped, UsefulBufC *, pBstr);

  UT_GenStub_Execute(QCBORDecode_EnterBstrWrapped, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterBstrWrappedFromMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterBstrWrappedFromMapN(QCBORDecodeContext *pCtx,
                                          int64_t nLabel,
                                          uint8_t uTagRequirement,
                                          UsefulBufC *pBstr) {
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapN, UsefulBufC *,
                      pBstr);

  UT_GenStub_Execute(QCBORDecode_EnterBstrWrappedFromMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterBstrWrappedFromMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterBstrWrappedFromMapSZ(QCBORDecodeContext *pCtx,
                                           const char *szLabel,
                                           uint8_t uTagRequirement,
                                           UsefulBufC *pBstr) {
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_EnterBstrWrappedFromMapSZ, UsefulBufC *,
                      pBstr);

  UT_GenStub_Execute(QCBORDecode_EnterBstrWrappedFromMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterMap()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterMap(QCBORDecodeContext *pCtx, QCBORItem *pItem) {

  UT_GenStub_AddParam(QCBORDecode_EnterMap, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_EnterMap, QCBORItem *, pItem);

  UT_GenStub_Execute(QCBORDecode_EnterMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterMapFromMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterMapFromMapN(QCBORDecodeContext *pCtx, int64_t nLabel) {
  UT_GenStub_AddParam(QCBORDecode_EnterMapFromMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_EnterMapFromMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBORDecode_EnterMapFromMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_EnterMapFromMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_EnterMapFromMapSZ(QCBORDecodeContext *pCtx,
                                   const char *szLabel) {
  UT_GenStub_AddParam(QCBORDecode_EnterMapFromMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_EnterMapFromMapSZ, const char *, szLabel);

  UT_GenStub_Execute(QCBORDecode_EnterMapFromMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_ExitArray()
 * ----------------------------------------------------
 */
void QCBORDecode_ExitArray(QCBORDecodeContext *pCtx) {

  UT_GenStub_AddParam(QCBORDecode_ExitArray, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_ExitArray, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_ExitBstrWrapped()
 * ----------------------------------------------------
 */
void QCBORDecode_ExitBstrWrapped(QCBORDecodeContext *pCtx) {
  UT_GenStub_AddParam(QCBORDecode_ExitBstrWrapped, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_ExitBstrWrapped, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_ExitMap()
 * ----------------------------------------------------
 */
void QCBORDecode_ExitMap(QCBORDecodeContext *pCtx) {

  UT_GenStub_AddParam(QCBORDecode_ExitMap, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_ExitMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetArray()
 * ----------------------------------------------------
 */
void QCBORDecode_GetArray(QCBORDecodeContext *pCtx, QCBORItem *pItem,
                                 UsefulBufC *pEncodedCBOR) {

  UT_GenStub_AddParam(QCBORDecode_GetArray, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetArray, QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_GetArray, UsefulBufC *, pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_GetArray, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetArrayFromMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetArrayFromMapN(QCBORDecodeContext *pCtx,
                                         int64_t nLabel, QCBORItem *pItem,
                                         UsefulBufC *pEncodedCBOR) {

  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapN, QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapN, UsefulBufC *, pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_GetArrayFromMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetArrayFromMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetArrayFromMapSZ(QCBORDecodeContext *pCtx,
                                          const char *szLabel, QCBORItem *pItem,
                                          UsefulBufC *pEncodedCBOR) {

  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapSZ, QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_GetArrayFromMapSZ, UsefulBufC *,
                      pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_GetArrayFromMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetB64()
 * ----------------------------------------------------
 */
void QCBORDecode_GetB64(QCBORDecodeContext *pCtx,
                               uint8_t uTagRequirement, UsefulBufC *pB64Text) {

  UT_GenStub_AddParam(QCBORDecode_GetB64, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetB64, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetB64, UsefulBufC *, pB64Text);

  UT_GenStub_Execute(QCBORDecode_GetB64, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetB64InMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetB64InMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                     uint8_t uTagRequirement,
                                     UsefulBufC *pB64Text) {

  UT_GenStub_AddParam(QCBORDecode_GetB64InMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetB64InMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetB64InMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetB64InMapN, UsefulBufC *, pB64Text);

  UT_GenStub_Execute(QCBORDecode_GetB64InMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetB64InMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetB64InMapSZ(QCBORDecodeContext *pCtx,
                                      const char *szLabel,
                                      uint8_t uTagRequirement,
                                      UsefulBufC *pB64Text) {

  UT_GenStub_AddParam(QCBORDecode_GetB64InMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetB64InMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetB64InMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetB64InMapSZ, UsefulBufC *, pB64Text);

  UT_GenStub_Execute(QCBORDecode_GetB64InMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetB64URL()
 * ----------------------------------------------------
 */
void QCBORDecode_GetB64URL(QCBORDecodeContext *pCtx,
                                  uint8_t uTagRequirement,
                                  UsefulBufC *pB64Text) {

  UT_GenStub_AddParam(QCBORDecode_GetB64URL, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetB64URL, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetB64URL, UsefulBufC *, pB64Text);

  UT_GenStub_Execute(QCBORDecode_GetB64URL, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetB64URLInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetB64URLInMapN(QCBORDecodeContext *pCtx,
                                        int64_t nLabel, uint8_t uTagRequirement,
                                        UsefulBufC *pB64Text) {

  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapN, UsefulBufC *, pB64Text);

  UT_GenStub_Execute(QCBORDecode_GetB64URLInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetB64URLInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetB64URLInMapSZ(QCBORDecodeContext *pCtx,
                                         const char *szLabel,
                                         uint8_t uTagRequirement,
                                         UsefulBufC *pB64Text) {

  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetB64URLInMapSZ, UsefulBufC *, pB64Text);

  UT_GenStub_Execute(QCBORDecode_GetB64URLInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBigFloat()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBigFloat(QCBORDecodeContext *pCtx, uint8_t uTagRequirement,
                             int64_t *pnMantissa, int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetBigFloat, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloat, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloat, int64_t *, pnMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloat, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetBigFloat, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBigFloatBig()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBigFloatBig(QCBORDecodeContext *pCtx,
                                uint8_t uTagRequirement,
                                UsefulBuf MantissaBuffer, UsefulBufC *pMantissa,
                                bool *pbMantissaIsNegative,
                                int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBig, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBig, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBig, UsefulBuf, MantissaBuffer);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBig, UsefulBufC *, pMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBig, bool *, pbMantissaIsNegative);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBig, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetBigFloatBig, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBigFloatBigInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBigFloatBigInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                      uint8_t uTagRequirement,
                                      UsefulBuf MantissaBuffer,
                                      UsefulBufC *pMantissa,
                                      bool *pbMantissaIsNegative,
                                      int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapN, UsefulBuf,
                      MantissaBuffer);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapN, UsefulBufC *,
                      pMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapN, bool *,
                      pbMantissaIsNegative);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapN, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetBigFloatBigInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBigFloatBigInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBigFloatBigInMapSZ(
    QCBORDecodeContext *pCtx, const char *szLabel, uint8_t uTagRequirement,
    UsefulBuf MantissaBuffer, UsefulBufC *pMantissa, bool *pbMantissaIsNegative,
    int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapSZ, UsefulBuf,
                      MantissaBuffer);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapSZ, UsefulBufC *,
                      pMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapSZ, bool *,
                      pbMantissaIsNegative);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatBigInMapSZ, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetBigFloatBigInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBigFloatInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBigFloatInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                   uint8_t uTagRequirement, int64_t *pnMantissa,
                                   int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapN, int64_t *, pnMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapN, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetBigFloatInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBigFloatInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBigFloatInMapSZ(QCBORDecodeContext *pCtx,
                                    const char *szLabel,
                                    uint8_t uTagRequirement,
                                    int64_t *pnMantissa, int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapSZ, int64_t *, pnMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetBigFloatInMapSZ, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetBigFloatInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBignum()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBignum(QCBORDecodeContext *pCtx, uint8_t uTagRequirement,
                           UsefulBufC *pValue, bool *pbIsNegative) {
  UT_GenStub_AddParam(QCBORDecode_GetBignum, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBignum, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBignum, UsefulBufC *, pValue);
  UT_GenStub_AddParam(QCBORDecode_GetBignum, bool *, pbIsNegative);

  UT_GenStub_Execute(QCBORDecode_GetBignum, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBignumInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBignumInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                 uint8_t uTagRequirement, UsefulBufC *pValue,
                                 bool *pbIsNegative) {
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapN, UsefulBufC *, pValue);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapN, bool *, pbIsNegative);

  UT_GenStub_Execute(QCBORDecode_GetBignumInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBignumInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBignumInMapSZ(QCBORDecodeContext *pCtx, const char *szLabel,
                                  uint8_t uTagRequirement, UsefulBufC *pValue,
                                  bool *pbIsNegative) {
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapSZ, UsefulBufC *, pValue);
  UT_GenStub_AddParam(QCBORDecode_GetBignumInMapSZ, bool *, pbIsNegative);

  UT_GenStub_Execute(QCBORDecode_GetBignumInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBinaryUUID()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBinaryUUID(QCBORDecodeContext *pCtx,
                                      uint8_t uTagRequirement,
                                      UsefulBufC *pUUID) {

  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUID, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUID, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUID, UsefulBufC *, pUUID);

  UT_GenStub_Execute(QCBORDecode_GetBinaryUUID, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBinaryUUIDInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBinaryUUIDInMapN(QCBORDecodeContext *pCtx,
                                            int64_t nLabel,
                                            uint8_t uTagRequirement,
                                            UsefulBufC *pUUID) {

  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapN, UsefulBufC *, pUUID);

  UT_GenStub_Execute(QCBORDecode_GetBinaryUUIDInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBinaryUUIDInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBinaryUUIDInMapSZ(QCBORDecodeContext *pCtx,
                                             const char *szLabel,
                                             uint8_t uTagRequirement,
                                             UsefulBufC *pUUID) {

  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetBinaryUUIDInMapSZ, UsefulBufC *, pUUID);

  UT_GenStub_Execute(QCBORDecode_GetBinaryUUIDInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBool()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBool(QCBORDecodeContext *pCtx, bool *pbBool) {
  UT_GenStub_AddParam(QCBORDecode_GetBool, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBool, bool *, pbBool);

  UT_GenStub_Execute(QCBORDecode_GetBool, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBoolInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBoolInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                               bool *pbBool) {
  UT_GenStub_AddParam(QCBORDecode_GetBoolInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBoolInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBoolInMapN, bool *, pbBool);

  UT_GenStub_Execute(QCBORDecode_GetBoolInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetBoolInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetBoolInMapSZ(QCBORDecodeContext *pCtx, const char *szLabel,
                                bool *pbBool) {
  UT_GenStub_AddParam(QCBORDecode_GetBoolInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetBoolInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetBoolInMapSZ, bool *, pbBool);

  UT_GenStub_Execute(QCBORDecode_GetBoolInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetByteString()
 * ----------------------------------------------------
 */
void QCBORDecode_GetByteString(QCBORDecodeContext *pCtx,
                                      UsefulBufC *pBytes) {

  UT_GenStub_AddParam(QCBORDecode_GetByteString, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetByteString, UsefulBufC *, pBytes);

  UT_GenStub_Execute(QCBORDecode_GetByteString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetByteStringInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetByteStringInMapN(QCBORDecodeContext *pCtx,
                                            int64_t nLabel,
                                            UsefulBufC *pBytes) {

  UT_GenStub_AddParam(QCBORDecode_GetByteStringInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetByteStringInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetByteStringInMapN, UsefulBufC *, pBytes);

  UT_GenStub_Execute(QCBORDecode_GetByteStringInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetByteStringInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetByteStringInMapSZ(QCBORDecodeContext *pCtx,
                                             const char *szLabel,
                                             UsefulBufC *pBytes) {

  UT_GenStub_AddParam(QCBORDecode_GetByteStringInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetByteStringInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetByteStringInMapSZ, UsefulBufC *, pBytes);

  UT_GenStub_Execute(QCBORDecode_GetByteStringInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDateString()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDateString(QCBORDecodeContext *pCtx,
                                      uint8_t uTagRequirement,
                                      UsefulBufC *pDateString) {

  UT_GenStub_AddParam(QCBORDecode_GetDateString, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDateString, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDateString, UsefulBufC *, pDateString);

  UT_GenStub_Execute(QCBORDecode_GetDateString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDateStringInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDateStringInMapN(QCBORDecodeContext *pCtx,
                                            int64_t nLabel,
                                            uint8_t uTagRequirement,
                                            UsefulBufC *pDateString) {

  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapN, UsefulBufC *,
                      pDateString);

  UT_GenStub_Execute(QCBORDecode_GetDateStringInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDateStringInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDateStringInMapSZ(QCBORDecodeContext *pCtx,
                                             const char *szLabel,
                                             uint8_t uTagRequirement,
                                             UsefulBufC *pDateString) {

  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDateStringInMapSZ, UsefulBufC *,
                      pDateString);

  UT_GenStub_Execute(QCBORDecode_GetDateStringInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDaysString()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDaysString(QCBORDecodeContext *pCtx,
                                      uint8_t uTagRequirement,
                                      UsefulBufC *pDateString) {

  UT_GenStub_AddParam(QCBORDecode_GetDaysString, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDaysString, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDaysString, UsefulBufC *, pDateString);

  UT_GenStub_Execute(QCBORDecode_GetDaysString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDaysStringInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDaysStringInMapN(QCBORDecodeContext *pCtx,
                                            int64_t nLabel,
                                            uint8_t uTagRequirement,
                                            UsefulBufC *pDateString) {

  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapN, UsefulBufC *,
                      pDateString);

  UT_GenStub_Execute(QCBORDecode_GetDaysStringInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDaysStringInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDaysStringInMapSZ(QCBORDecodeContext *pCtx,
                                             const char *szLabel,
                                             uint8_t uTagRequirement,
                                             UsefulBufC *pDateString) {

  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDaysStringInMapSZ, UsefulBufC *,
                      pDateString);

  UT_GenStub_Execute(QCBORDecode_GetDaysStringInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDecimalFraction()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDecimalFraction(QCBORDecodeContext *pCtx,
                                    uint8_t uTagRequirement,
                                    int64_t *pnMantissa, int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFraction, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFraction, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFraction, int64_t *, pnMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFraction, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetDecimalFraction, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDecimalFractionBig()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDecimalFractionBig(
    QCBORDecodeContext *pCtx, uint8_t uTagRequirement, UsefulBuf MantissaBuffer,
    UsefulBufC *pMantissa, bool *pbMantissaIsNegative, int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBig, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBig, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBig, UsefulBuf,
                      MantissaBuffer);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBig, UsefulBufC *,
                      pMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBig, bool *,
                      pbMantissaIsNegative);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBig, int64_t *, pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetDecimalFractionBig, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDecimalFractionBigInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDecimalFractionBigInMapN(
    QCBORDecodeContext *pCtx, int64_t nLabel, uint8_t uTagRequirement,
    UsefulBuf MantissaBuffer, UsefulBufC *pbMantissaIsNegative,
    bool *pbIsNegative, int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapN, UsefulBuf,
                      MantissaBuffer);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapN, UsefulBufC *,
                      pbMantissaIsNegative);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapN, bool *,
                      pbIsNegative);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapN, int64_t *,
                      pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetDecimalFractionBigInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDecimalFractionBigInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDecimalFractionBigInMapSZ(
    QCBORDecodeContext *pCtx, const char *szLabel, uint8_t uTagRequirement,
    UsefulBuf MantissaBuffer, UsefulBufC *pMantissa, bool *pbMantissaIsNegative,
    int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapSZ, UsefulBuf,
                      MantissaBuffer);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapSZ, UsefulBufC *,
                      pMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapSZ, bool *,
                      pbMantissaIsNegative);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionBigInMapSZ, int64_t *,
                      pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetDecimalFractionBigInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDecimalFractionInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDecimalFractionInMapN(QCBORDecodeContext *pCtx,
                                          int64_t nLabel,
                                          uint8_t uTagRequirement,
                                          int64_t *pnMantissa,
                                          int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapN, int64_t *,
                      pnMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapN, int64_t *,
                      pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetDecimalFractionInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDecimalFractionInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDecimalFractionInMapSZ(QCBORDecodeContext *pMe,
                                           const char *szLabel,
                                           uint8_t uTagRequirement,
                                           int64_t *pnMantissa,
                                           int64_t *pnExponent) {
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapSZ,
                      QCBORDecodeContext *, pMe);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapSZ, int64_t *,
                      pnMantissa);
  UT_GenStub_AddParam(QCBORDecode_GetDecimalFractionInMapSZ, int64_t *,
                      pnExponent);

  UT_GenStub_Execute(QCBORDecode_GetDecimalFractionInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDouble()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDouble(QCBORDecodeContext *pCtx, double *pValue) {

  UT_GenStub_AddParam(QCBORDecode_GetDouble, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDouble, double *, pValue);

  UT_GenStub_Execute(QCBORDecode_GetDouble, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleConvert()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleConvert(QCBORDecodeContext *pCtx,
                                         uint32_t uConvertTypes,
                                         double *pdValue) {

  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvert, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvert, uint32_t, uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvert, double *, pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleConvert, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleConvertAll()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleConvertAll(QCBORDecodeContext *pCtx,
                                     uint32_t uConvertTypes, double *pdValue) {
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAll, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAll, uint32_t, uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAll, double *, pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleConvertAll, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleConvertAllInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleConvertAllInMapN(QCBORDecodeContext *pCtx,
                                           int64_t nLabel,
                                           uint32_t uConvertTypes,
                                           double *pdValue) {
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapN, double *, pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleConvertAllInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleConvertAllInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleConvertAllInMapSZ(QCBORDecodeContext *pCtx,
                                            const char *szLabel,
                                            uint32_t uConvertTypes,
                                            double *pdValue) {
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertAllInMapSZ, double *,
                      pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleConvertAllInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleConvertInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleConvertInMapN(QCBORDecodeContext *pCtx,
                                               int64_t nLabel,
                                               uint32_t uConvertTypes,
                                               double *pdValue) {

  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapN, double *, pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleConvertInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleConvertInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleConvertInMapSZ(QCBORDecodeContext *pCtx,
                                                const char *szLabel,
                                                uint32_t uConvertTypes,
                                                double *pdValue) {

  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleConvertInMapSZ, double *, pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleConvertInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleInMapN(QCBORDecodeContext *pCtx,
                                        int64_t nLabel, double *pdValue) {

  UT_GenStub_AddParam(QCBORDecode_GetDoubleInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleInMapN, double *, pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetDoubleInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetDoubleInMapSZ(QCBORDecodeContext *pCtx,
                                         const char *szLabel, double *pdValue) {

  UT_GenStub_AddParam(QCBORDecode_GetDoubleInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetDoubleInMapSZ, double *, pdValue);

  UT_GenStub_Execute(QCBORDecode_GetDoubleInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetEpochDate()
 * ----------------------------------------------------
 */
void QCBORDecode_GetEpochDate(QCBORDecodeContext *pCtx, uint8_t uTagRequirement,
                              int64_t *pnTime) {
  UT_GenStub_AddParam(QCBORDecode_GetEpochDate, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDate, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDate, int64_t *, pnTime);

  UT_GenStub_Execute(QCBORDecode_GetEpochDate, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetEpochDateInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetEpochDateInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                    uint8_t uTagRequirement, int64_t *pnTime) {
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapN, int64_t *, pnTime);

  UT_GenStub_Execute(QCBORDecode_GetEpochDateInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetEpochDateInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetEpochDateInMapSZ(QCBORDecodeContext *pCtx,
                                     const char *szLabel,
                                     uint8_t uTagRequirement, int64_t *pnTime) {
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDateInMapSZ, int64_t *, pnTime);

  UT_GenStub_Execute(QCBORDecode_GetEpochDateInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetEpochDays()
 * ----------------------------------------------------
 */
void QCBORDecode_GetEpochDays(QCBORDecodeContext *pCtx, uint8_t uTagRequirement,
                              int64_t *pnDays) {
  UT_GenStub_AddParam(QCBORDecode_GetEpochDays, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDays, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDays, int64_t *, pnDays);

  UT_GenStub_Execute(QCBORDecode_GetEpochDays, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetEpochDaysInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetEpochDaysInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                    uint8_t uTagRequirement, int64_t *pnDays) {
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapN, int64_t *, pnDays);

  UT_GenStub_Execute(QCBORDecode_GetEpochDaysInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetEpochDaysInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetEpochDaysInMapSZ(QCBORDecodeContext *pCtx,
                                     const char *szLabel,
                                     uint8_t uTagRequirement, int64_t *pnDays) {
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetEpochDaysInMapSZ, int64_t *, pnDays);

  UT_GenStub_Execute(QCBORDecode_GetEpochDaysInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64(QCBORDecodeContext *pCtx, int64_t *pnValue) {

  UT_GenStub_AddParam(QCBORDecode_GetInt64, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64Convert()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64Convert(QCBORDecodeContext *pCtx,
                                        uint32_t uConvertTypes,
                                        int64_t *pnValue) {

  UT_GenStub_AddParam(QCBORDecode_GetInt64Convert, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64Convert, uint32_t, uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetInt64Convert, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64Convert, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64ConvertAll()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64ConvertAll(QCBORDecodeContext *pCtx,
                                    uint32_t uConvertTypes, int64_t *pnValue) {
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAll, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAll, uint32_t, uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAll, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64ConvertAll, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64ConvertAllInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64ConvertAllInMapN(QCBORDecodeContext *pCtx,
                                          int64_t nLabel,
                                          uint32_t uConvertTypes,
                                          int64_t *pnValue) {
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapN, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64ConvertAllInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64ConvertAllInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64ConvertAllInMapSZ(QCBORDecodeContext *pCtx,
                                           const char *szLabel,
                                           uint32_t uConvertTypes,
                                           int64_t *pnValue) {
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertAllInMapSZ, int64_t *,
                      pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64ConvertAllInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64ConvertInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64ConvertInMapN(QCBORDecodeContext *pCtx,
                                              int64_t nLabel,
                                              uint32_t uConvertTypes,
                                              int64_t *pnValue) {

  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapN, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64ConvertInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64ConvertInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64ConvertInMapSZ(QCBORDecodeContext *pCtx,
                                               const char *szLabel,
                                               uint32_t uConvertTypes,
                                               int64_t *pnValue) {

  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetInt64ConvertInMapSZ, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64ConvertInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64InMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64InMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                       int64_t *pnValue) {

  UT_GenStub_AddParam(QCBORDecode_GetInt64InMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64InMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetInt64InMapN, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64InMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetInt64InMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetInt64InMapSZ(QCBORDecodeContext *pCtx,
                                        const char *szLabel, int64_t *pnValue) {

  UT_GenStub_AddParam(QCBORDecode_GetInt64InMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetInt64InMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetInt64InMapSZ, int64_t *, pnValue);

  UT_GenStub_Execute(QCBORDecode_GetInt64InMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetItemInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetItemInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                               uint8_t uQcborType, QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapN, uint8_t, uQcborType);
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapN, QCBORItem *, pItem);

  UT_GenStub_Execute(QCBORDecode_GetItemInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetItemInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetItemInMapSZ(QCBORDecodeContext *pCtx, const char *szLabel,
                                uint8_t uQcborType, QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapSZ, uint8_t, uQcborType);
  UT_GenStub_AddParam(QCBORDecode_GetItemInMapSZ, QCBORItem *, pItem);

  UT_GenStub_Execute(QCBORDecode_GetItemInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetItemsInMap()
 * ----------------------------------------------------
 */
void QCBORDecode_GetItemsInMap(QCBORDecodeContext *pCtx, QCBORItem *pItemList) {
  UT_GenStub_AddParam(QCBORDecode_GetItemsInMap, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetItemsInMap, QCBORItem *, pItemList);

  UT_GenStub_Execute(QCBORDecode_GetItemsInMap, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetItemsInMapWithCallback()
 * ----------------------------------------------------
 */
void QCBORDecode_GetItemsInMapWithCallback(QCBORDecodeContext *pCtx,
                                           QCBORItem *pItemList,
                                           void *pCallbackCtx,
                                           QCBORItemCallback pfCB) {
  UT_GenStub_AddParam(QCBORDecode_GetItemsInMapWithCallback,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetItemsInMapWithCallback, QCBORItem *,
                      pItemList);
  UT_GenStub_AddParam(QCBORDecode_GetItemsInMapWithCallback, void *,
                      pCallbackCtx);
  UT_GenStub_AddParam(QCBORDecode_GetItemsInMapWithCallback, QCBORItemCallback,
                      pfCB);

  UT_GenStub_Execute(QCBORDecode_GetItemsInMapWithCallback, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetMIMEMessage()
 * ----------------------------------------------------
 */
void QCBORDecode_GetMIMEMessage(QCBORDecodeContext *pCtx,
                                       uint8_t uTagRequirement,
                                       UsefulBufC *pMessage, bool *pbIsTag257) {

  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessage, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessage, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessage, UsefulBufC *, pMessage);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessage, bool *, pbIsTag257);

  UT_GenStub_Execute(QCBORDecode_GetMIMEMessage, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetMIMEMessageInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetMIMEMessageInMapN(QCBORDecodeContext *pCtx,
                                             int64_t nLabel,
                                             uint8_t uTagRequirement,
                                             UsefulBufC *pMessage,
                                             bool *pbIsTag257) {

  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapN, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapN, UsefulBufC *, pMessage);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapN, bool *, pbIsTag257);

  UT_GenStub_Execute(QCBORDecode_GetMIMEMessageInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetMIMEMessageInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetMIMEMessageInMapSZ(QCBORDecodeContext *pCtx,
                                              const char *szLabel,
                                              uint8_t uTagRequirement,
                                              UsefulBufC *pMessage,
                                              bool *pbIsTag257) {

  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapSZ, uint8_t,
                      uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapSZ, UsefulBufC *,
                      pMessage);
  UT_GenStub_AddParam(QCBORDecode_GetMIMEMessageInMapSZ, bool *, pbIsTag257);

  UT_GenStub_Execute(QCBORDecode_GetMIMEMessageInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetMap()
 * ----------------------------------------------------
 */
void QCBORDecode_GetMap(QCBORDecodeContext *pCtx, QCBORItem *pItem,
                               UsefulBufC *pEncodedCBOR) {

  UT_GenStub_AddParam(QCBORDecode_GetMap, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetMap, QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_GetMap, UsefulBufC *, pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_GetMap, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetMapFromMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetMapFromMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                       QCBORItem *pItem,
                                       UsefulBufC *pEncodedCBOR) {

  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapN, QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapN, UsefulBufC *, pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_GetMapFromMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetMapFromMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetMapFromMapSZ(QCBORDecodeContext *pCtx,
                                        const char *szLabel, QCBORItem *pItem,
                                        UsefulBufC *pEncodedCBOR) {

  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapSZ, QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_GetMapFromMapSZ, UsefulBufC *, pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_GetMapFromMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetNull()
 * ----------------------------------------------------
 */
void QCBORDecode_GetNull(QCBORDecodeContext *pCtx) {

  UT_GenStub_AddParam(QCBORDecode_GetNull, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_GetNull, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetNullInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetNullInMapN(QCBORDecodeContext *pCtx,
                                      int64_t nLabel) {

  UT_GenStub_AddParam(QCBORDecode_GetNullInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetNullInMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBORDecode_GetNullInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetNullInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetNullInMapSZ(QCBORDecodeContext *pCtx,
                                       const char *szLabel) {

  UT_GenStub_AddParam(QCBORDecode_GetNullInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetNullInMapSZ, const char *, szLabel);

  UT_GenStub_Execute(QCBORDecode_GetNullInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetRegex()
 * ----------------------------------------------------
 */
void QCBORDecode_GetRegex(QCBORDecodeContext *pCtx,
                                 uint8_t uTagRequirement, UsefulBufC *pRegex) {

  UT_GenStub_AddParam(QCBORDecode_GetRegex, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetRegex, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetRegex, UsefulBufC *, pRegex);

  UT_GenStub_Execute(QCBORDecode_GetRegex, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetRegexInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetRegexInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                       uint8_t uTagRequirement,
                                       UsefulBufC *pRegex) {

  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapN, UsefulBufC *, pRegex);

  UT_GenStub_Execute(QCBORDecode_GetRegexInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetRegexInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetRegexInMapSZ(QCBORDecodeContext *pCtx,
                                        const char *szLabel,
                                        uint8_t uTagRequirement,
                                        UsefulBufC *pRegex) {

  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetRegexInMapSZ, UsefulBufC *, pRegex);

  UT_GenStub_Execute(QCBORDecode_GetRegexInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetTextString()
 * ----------------------------------------------------
 */
void QCBORDecode_GetTextString(QCBORDecodeContext *pCtx,
                                      UsefulBufC *pText) {

  UT_GenStub_AddParam(QCBORDecode_GetTextString, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetTextString, UsefulBufC *, pText);

  UT_GenStub_Execute(QCBORDecode_GetTextString, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetTextStringInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetTextStringInMapN(QCBORDecodeContext *pCtx,
                                            int64_t nLabel, UsefulBufC *pText) {

  UT_GenStub_AddParam(QCBORDecode_GetTextStringInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetTextStringInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetTextStringInMapN, UsefulBufC *, pText);

  UT_GenStub_Execute(QCBORDecode_GetTextStringInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetTextStringInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetTextStringInMapSZ(QCBORDecodeContext *pCtx,
                                             const char *szLabel,
                                             UsefulBufC *pText) {

  UT_GenStub_AddParam(QCBORDecode_GetTextStringInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetTextStringInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetTextStringInMapSZ, UsefulBufC *, pText);

  UT_GenStub_Execute(QCBORDecode_GetTextStringInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64(QCBORDecodeContext *pCtx, uint64_t *puValue) {

  UT_GenStub_AddParam(QCBORDecode_GetUInt64, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64, uint64_t *, puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64Convert()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64Convert(QCBORDecodeContext *pCtx,
                                         uint32_t uConvertTypes,
                                         uint64_t *puValue) {

  UT_GenStub_AddParam(QCBORDecode_GetUInt64Convert, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64Convert, uint32_t, uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64Convert, uint64_t *, puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64Convert, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64ConvertAll()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64ConvertAll(QCBORDecodeContext *pCtx,
                                     uint32_t uConvertTypes,
                                     uint64_t *puValue) {
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAll, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAll, uint32_t, uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAll, uint64_t *, puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64ConvertAll, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64ConvertAllInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64ConvertAllInMapN(QCBORDecodeContext *pCtx,
                                           int64_t nLabel,
                                           uint32_t uConvertTypes,
                                           uint64_t *puValue) {
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapN, uint64_t *,
                      puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64ConvertAllInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64ConvertAllInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64ConvertAllInMapSZ(QCBORDecodeContext *pCtx,
                                            const char *szLabel,
                                            uint32_t uConvertTypes,
                                            uint64_t *puValue) {
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertAllInMapSZ, uint64_t *,
                      puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64ConvertAllInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64ConvertInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64ConvertInMapN(QCBORDecodeContext *pCtx,
                                               int64_t nLabel,
                                               uint32_t uConvertTypes,
                                               uint64_t *puValue) {

  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapN, uint64_t *, puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64ConvertInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64ConvertInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64ConvertInMapSZ(QCBORDecodeContext *pCtx,
                                                const char *szLabel,
                                                uint32_t uConvertTypes,
                                                uint64_t *puValue) {

  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64ConvertInMapSZ, uint64_t *, puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64ConvertInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64InMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64InMapN(QCBORDecodeContext *pCtx,
                                        int64_t nLabel, uint64_t *puValue) {

  UT_GenStub_AddParam(QCBORDecode_GetUInt64InMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64InMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64InMapN, uint64_t *, puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64InMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUInt64InMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUInt64InMapSZ(QCBORDecodeContext *pCtx,
                                         const char *szLabel,
                                         uint64_t *puValue) {

  UT_GenStub_AddParam(QCBORDecode_GetUInt64InMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64InMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetUInt64InMapSZ, uint64_t *, puValue);

  UT_GenStub_Execute(QCBORDecode_GetUInt64InMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetURI()
 * ----------------------------------------------------
 */
void QCBORDecode_GetURI(QCBORDecodeContext *pCtx,
                               uint8_t uTagRequirement, UsefulBufC *pURI) {

  UT_GenStub_AddParam(QCBORDecode_GetURI, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetURI, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetURI, UsefulBufC *, pURI);

  UT_GenStub_Execute(QCBORDecode_GetURI, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetURIInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetURIInMapN(QCBORDecodeContext *pCtx, int64_t nLabel,
                                     uint8_t uTagRequirement,
                                     UsefulBufC *pURI) {

  UT_GenStub_AddParam(QCBORDecode_GetURIInMapN, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetURIInMapN, int64_t, nLabel);
  UT_GenStub_AddParam(QCBORDecode_GetURIInMapN, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetURIInMapN, UsefulBufC *, pURI);

  UT_GenStub_Execute(QCBORDecode_GetURIInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetURIInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetURIInMapSZ(QCBORDecodeContext *pCtx,
                                      const char *szLabel,
                                      uint8_t uTagRequirement,
                                      UsefulBufC *pURI) {

  UT_GenStub_AddParam(QCBORDecode_GetURIInMapSZ, QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetURIInMapSZ, const char *, szLabel);
  UT_GenStub_AddParam(QCBORDecode_GetURIInMapSZ, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_GetURIInMapSZ, UsefulBufC *, pURI);

  UT_GenStub_Execute(QCBORDecode_GetURIInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUndefined()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUndefined(QCBORDecodeContext *pCtx) {

  UT_GenStub_AddParam(QCBORDecode_GetUndefined, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_GetUndefined, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUndefinedInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUndefinedInMapN(QCBORDecodeContext *pCtx,
                                           int64_t nLabel) {

  UT_GenStub_AddParam(QCBORDecode_GetUndefinedInMapN, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUndefinedInMapN, int64_t, nLabel);

  UT_GenStub_Execute(QCBORDecode_GetUndefinedInMapN, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_GetUndefinedInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_GetUndefinedInMapSZ(QCBORDecodeContext *pCtx,
                                            const char *szLabel) {

  UT_GenStub_AddParam(QCBORDecode_GetUndefinedInMapSZ, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_GetUndefinedInMapSZ, const char *, szLabel);

  UT_GenStub_Execute(QCBORDecode_GetUndefinedInMapSZ, Basic, NULL);

}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_EnterBoundedMapOrArray()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_EnterBoundedMapOrArray(QCBORDecodeContext *pCtx,
                                                uint8_t uType,
                                                QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_EnterBoundedMapOrArray,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_EnterBoundedMapOrArray, uint8_t,
                      uType);
  UT_GenStub_AddParam(QCBORDecode_Private_EnterBoundedMapOrArray, QCBORItem *,
                      pItem);

  UT_GenStub_Execute(QCBORDecode_Private_EnterBoundedMapOrArray, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_ExitBoundedMapOrArray()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_ExitBoundedMapOrArray(QCBORDecodeContext *pCtx,
                                               uint8_t uType) {
  UT_GenStub_AddParam(QCBORDecode_Private_ExitBoundedMapOrArray,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_ExitBoundedMapOrArray, uint8_t,
                      uType);

  UT_GenStub_Execute(QCBORDecode_Private_ExitBoundedMapOrArray, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetArrayOrMap()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetArrayOrMap(QCBORDecodeContext *pCtx, uint8_t uType,
                                       QCBORItem *pItem,
                                       UsefulBufC *pEncodedCBOR) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetArrayOrMap, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetArrayOrMap, uint8_t, uType);
  UT_GenStub_AddParam(QCBORDecode_Private_GetArrayOrMap, QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_Private_GetArrayOrMap, UsefulBufC *,
                      pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_Private_GetArrayOrMap, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetDoubleConvert()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetDoubleConvert(QCBORDecodeContext *pCtx,
                                          uint32_t uConvertTypes,
                                          double *pValue, QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvert,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvert, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvert, double *, pValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvert, QCBORItem *, pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetDoubleConvert, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetDoubleConvertInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetDoubleConvertInMapN(QCBORDecodeContext *pCtx,
                                                int64_t nLabel,
                                                uint32_t uConvertTypes,
                                                double *pdValue,
                                                QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapN, int64_t,
                      nLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapN, double *,
                      pdValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapN, QCBORItem *,
                      pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetDoubleConvertInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetDoubleConvertInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetDoubleConvertInMapSZ(QCBORDecodeContext *pCtx,
                                                 const char *szLabel,
                                                 uint32_t uConvertTypes,
                                                 double *pdValue,
                                                 QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapSZ, double *,
                      pdValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetDoubleConvertInMapSZ, QCBORItem *,
                      pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetDoubleConvertInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetInt64Convert()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetInt64Convert(QCBORDecodeContext *pCtx,
                                         uint32_t uConvertTypes,
                                         int64_t *pnValue, QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64Convert, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64Convert, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64Convert, int64_t *, pnValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64Convert, QCBORItem *, pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetInt64Convert, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetInt64ConvertInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetInt64ConvertInMapN(QCBORDecodeContext *pCtx,
                                               int64_t nLabel,
                                               uint32_t uConvertTypes,
                                               int64_t *pnValue,
                                               QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapN, int64_t,
                      nLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapN, int64_t *,
                      pnValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapN, QCBORItem *,
                      pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetInt64ConvertInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetInt64ConvertInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetInt64ConvertInMapSZ(QCBORDecodeContext *pCtx,
                                                const char *szLabel,
                                                uint32_t uConvertTypes,
                                                int64_t *pnValue,
                                                QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapSZ, int64_t *,
                      pnValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetInt64ConvertInMapSZ, QCBORItem *,
                      pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetInt64ConvertInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetMIME()
 * ----------------------------------------------------
 */
QCBORError QCBORDecode_Private_GetMIME(uint8_t uTagRequirement,
                                       const QCBORItem *pItem,
                                       UsefulBufC *pMessage, bool *pbIsTag257) {
  UT_GenStub_SetupReturnBuffer(QCBORDecode_Private_GetMIME, QCBORError);

  UT_GenStub_AddParam(QCBORDecode_Private_GetMIME, uint8_t, uTagRequirement);
  UT_GenStub_AddParam(QCBORDecode_Private_GetMIME, const QCBORItem *, pItem);
  UT_GenStub_AddParam(QCBORDecode_Private_GetMIME, UsefulBufC *, pMessage);
  UT_GenStub_AddParam(QCBORDecode_Private_GetMIME, bool *, pbIsTag257);

  UT_GenStub_Execute(QCBORDecode_Private_GetMIME, Basic, NULL);

  return UT_GenStub_GetReturnValue(QCBORDecode_Private_GetMIME, QCBORError);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetTaggedString()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetTaggedString(QCBORDecodeContext *pCtx,
                                         QCBOR_Private_TagSpec TagSpec,
                                         UsefulBufC *pBstr) { 
  
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedString, QCBORDecodeContext *,
                      pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedString,
                      QCBOR_Private_TagSpec, TagSpec);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedString, UsefulBufC *, pBstr);

  UT_GenStub_Execute(QCBORDecode_Private_GetTaggedString, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetTaggedStringInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetTaggedStringInMapN(QCBORDecodeContext *pCtx,
                                               int64_t nLabel,
                                               QCBOR_Private_TagSpec TagSpec,
                                               UsefulBufC *pString) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapN, int64_t,
                      nLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapN,
                      QCBOR_Private_TagSpec, TagSpec);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapN, UsefulBufC *,
                      pString);

  UT_GenStub_Execute(QCBORDecode_Private_GetTaggedStringInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetTaggedStringInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetTaggedStringInMapSZ(QCBORDecodeContext *pCtx,
                                                const char *szLabel,
                                                QCBOR_Private_TagSpec TagSpec,
                                                UsefulBufC *pString) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapSZ,
                      QCBOR_Private_TagSpec, TagSpec);
  UT_GenStub_AddParam(QCBORDecode_Private_GetTaggedStringInMapSZ, UsefulBufC *,
                      pString);

  UT_GenStub_Execute(QCBORDecode_Private_GetTaggedStringInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetUInt64Convert()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetUInt64Convert(QCBORDecodeContext *pCtx,
                                          uint32_t uConvertTypes,
                                          uint64_t *puValue, QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64Convert,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64Convert, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64Convert, uint64_t *,
                      puValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64Convert, QCBORItem *, pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetUInt64Convert, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetUInt64ConvertInMapN()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetUInt64ConvertInMapN(QCBORDecodeContext *pCtx,
                                                int64_t nLabel,
                                                uint32_t uConvertTypes,
                                                uint64_t *puValue,
                                                QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapN,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapN, int64_t,
                      nLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapN, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapN, uint64_t *,
                      puValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapN, QCBORItem *,
                      pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetUInt64ConvertInMapN, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_GetUInt64ConvertInMapSZ()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_GetUInt64ConvertInMapSZ(QCBORDecodeContext *pCtx,
                                                 const char *szLabel,
                                                 uint32_t uConvertTypes,
                                                 uint64_t *puValue,
                                                 QCBORItem *pItem) {
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapSZ,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapSZ, const char *,
                      szLabel);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapSZ, uint32_t,
                      uConvertTypes);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapSZ, uint64_t *,
                      puValue);
  UT_GenStub_AddParam(QCBORDecode_Private_GetUInt64ConvertInMapSZ, QCBORItem *,
                      pItem);

  UT_GenStub_Execute(QCBORDecode_Private_GetUInt64ConvertInMapSZ, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Private_SearchAndGetArrayOrMap()
 * ----------------------------------------------------
 */
void QCBORDecode_Private_SearchAndGetArrayOrMap(QCBORDecodeContext *pCtx,
                                                QCBORItem *pTarget,
                                                QCBORItem *pItem,
                                                UsefulBufC *pEncodedCBOR) {
  UT_GenStub_AddParam(QCBORDecode_Private_SearchAndGetArrayOrMap,
                      QCBORDecodeContext *, pCtx);
  UT_GenStub_AddParam(QCBORDecode_Private_SearchAndGetArrayOrMap, QCBORItem *,
                      pTarget);
  UT_GenStub_AddParam(QCBORDecode_Private_SearchAndGetArrayOrMap, QCBORItem *,
                      pItem);
  UT_GenStub_AddParam(QCBORDecode_Private_SearchAndGetArrayOrMap, UsefulBufC *,
                      pEncodedCBOR);

  UT_GenStub_Execute(QCBORDecode_Private_SearchAndGetArrayOrMap, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for QCBORDecode_Rewind()
 * ----------------------------------------------------
 */
void QCBORDecode_Rewind(QCBORDecodeContext *pCtx) {
  UT_GenStub_AddParam(QCBORDecode_Rewind, QCBORDecodeContext *, pCtx);

  UT_GenStub_Execute(QCBORDecode_Rewind, Basic, NULL);
}
