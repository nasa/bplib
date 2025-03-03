#include "bplib_cbor_internal.h"

/**
 * \brief     Encodes a BPLib_EID_t type
 * \param[in] Context (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] SourceData (BPLib_EID_t*) pointer to the field that needs to be encoded
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_CBOR_EncodeEID(QCBORDecodeContext* Context, BPLib_EID_t* SourceData)
{
    BPLib_Status_t ReturnStatus;

    if ((Context == NULL) || (SourceData == NULL))
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else
    {
        /*
        ** Open Outer Array (EID)
        */
        QCBOREncode_OpenArray(Context);

        /*
        ** Add outer content (URI Type)
        */
        QCBOREncode_AddUInt64(Context, SourceData->Scheme);


        /*
        ** Open Inner Array (SSP)
        */
        QCBOREncode_OpenArray(Context);

        /*
        ** Add inner content (SSP)
        */
        QCBOREncode_AddUInt64(Context, SourceData->Node);
        QCBOREncode_AddUInt64(Context, SourceData->Service);

        /*
        ** Close Inner Array (SSP)
        */
        QCBOREncode_CloseArray(Context);

        /*
        ** Close Outer Array (EID)
        */
        QCBOREncode_CloseArray(Context);
        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}



/**
 * \brief     Encodes a BPLib_CreationTimeStamp_t
 * \param[in] Context (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] CreateTimeStamp (BPLib_CreationTimeStamp_t*) pointer to the field that needs to be encoded
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_CBOR_EncodeCreationTimeStamp(QCBORDecodeContext* Context, BPLib_CreationTimeStamp_t* TimeStamp)
{
    BPLib_Status_t ReturnStatus;

    if ((Context == NULL) || (TimeStamp == NULL))
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else
    {
        /*
        ** Open Array
        */
        QCBOREncode_OpenArray(Context);

        /*
        ** Add content
        */
        QCBOREncode_AddUInt64(Context, TimeStamp->CreateTime);
        QCBOREncode_AddUInt64(Context, TimeStamp->SequenceNumber);

        /*
        ** Close Array
        */
        QCBOREncode_CloseArray(Context);
        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}



/**
 * \brief     Encodes a CRC value
 * \param[in] Context (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] SourceData (uint64_t*) pointer to the field that needs to be encoded
 * \param[in] CrcType (uint64_t) specifies the expected CRC type (None, CRC16, or CRC32C)
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_CBOR_EncodeCrcValue(QCBORDecodeContext* Context, uint64_t CrcValue, uint64_t CrcType)
{
    BPLib_Status_t ReturnStatus;
    UsefulBufC CrcInfo;
    CrcInfo.ptr = &CrcValue;

    if (Context == NULL)
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else
    {
        if (CrcType == BPLib_CRC_Type_None)
        {
            /* If CRC is none, there's nothing to do */
            ReturnStatus = BPLIB_SUCCESS;
        }
        else if (CrcType == BPLib_CRC_Type_CRC16)
        {
            /* Encode 16-bit CRC */
            CrcInfo.len = 2;
            QCBOREncode_AddBytes(Context, CrcInfo);
            ReturnStatus = BPLIB_SUCCESS;
        }
        else if (CrcType == BPLib_CRC_Type_CRC32C)
        {
            /* Encode 32-bit CRC */
            CrcInfo.len = 4;
            QCBOREncode_AddBytes(Context, CrcInfo);
            ReturnStatus = BPLIB_SUCCESS;
        }
        else
        {
            /* Unrecognized CRC type */
            ReturnStatus = BPLIB_ERROR;
        }

    }

    return ReturnStatus;
}
