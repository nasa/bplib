#define BPLIB_STOR_CACHE_H

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_stor_cache header
 */

#include "bplib_stor_cache.h"
#include "utgenstub.h"
#include "bplib_api_types.h"
#include "bplib_eid.h"
#include "bplib_mem.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CacheInit()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t *Inst)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CacheInit, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_CacheInit, BPLib_Instance_t *, Inst);

    UT_GenStub_Execute(BPLib_STOR_CacheInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CacheInit, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_Destroy()
 * ----------------------------------------------------
 */
void BPLib_STOR_Destroy(BPLib_Instance_t *Inst)
{
    UT_GenStub_AddParam(BPLib_STOR_Destroy, BPLib_Instance_t *, Inst);

    UT_GenStub_Execute(BPLib_STOR_Destroy, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_EgressForDestEID()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_EgressForDestEID(BPLib_Instance_t *Inst, uint16_t EgressID, BPLib_EID_Pattern_t *DestEID,
                                           size_t MaxBundles, size_t *NumEgressed)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_EgressForDestEID, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_EgressForDestEID, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_STOR_EgressForDestEID, uint16_t, EgressID);
    UT_GenStub_AddParam(BPLib_STOR_EgressForDestEID, BPLib_EID_Pattern_t *, DestEID);
    UT_GenStub_AddParam(BPLib_STOR_EgressForDestEID, size_t, MaxBundles);
    UT_GenStub_AddParam(BPLib_STOR_EgressForDestEID, size_t *, NumEgressed);

    UT_GenStub_Execute(BPLib_STOR_EgressForDestEID, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_EgressForDestEID, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_GarbageCollect()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t *Inst, size_t *NumDiscarded)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_GarbageCollect, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_GarbageCollect, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_STOR_GarbageCollect, size_t *, NumDiscarded);

    UT_GenStub_Execute(BPLib_STOR_GarbageCollect, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_GarbageCollect, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_StoreBundle()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t *Inst, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_StoreBundle, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_StoreBundle, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_STOR_StoreBundle, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_STOR_StoreBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_StoreBundle, BPLib_Status_t);
}
