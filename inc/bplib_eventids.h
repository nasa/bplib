/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * @file
 *
 * Define  BPLib Event IDs
 */

#ifndef BPLIB_EVENTIDS_H
#define BPLIB_EVENTIDS_H

#define BPLIB_RESERVED_EID                          (0u)
#define BPLIB_VALUE_INF_EID                         (1u)
#define BPLIB_SUB_HK_ERR_EID                        (2u)
#define BPLIB_DELETE_HNDLR_ERR_EID                  (3u)
#define BPLIB_ADU_IN_INIT_SEM_ERR_EID               (4u)
#define BPLIB_ADU_IN_SUB_PIPE_ERR_EID               (5u)
#define BPLIB_ADU_OUT_EXIT_SEM_ERR_EID              (6u)
#define BPLIB_NOOP_EID                              (7u)
#define BPLIB_ADD_ALL_APPS_SUCCESS_EID              (8u)
#define BPLIB_START_ALL_APPS_SUCCESS_EID            (9u)
#define BPLIB_VERIF_BNDL_SUCCESS_EID                (10u)
#define BPLIB_INIT_BNDL_STOR_SUCCESS_EID            (11u)
#define BPLIB_VERIF_BNDL_META_SUCCESS_EID           (12u)
#define BPLIB_REBUILD_BNDL_META_SUCCESS_EID         (13u)
#define BPLIB_CLR_VOLATILE_SUCCESS_EID              (14u)
#define BPLIB_RELOAD_SVD_DATA_SUCCESS_EID           (15u)
#define BPLIB_RESET_ALL_CTRS_SUCCESS_EID            (16u)
#define BPLIB_RESET_CTR_SUCCESS_EID                 (17u)
#define BPLIB_RESET_SRC_CTRS_SUCCESS_EID            (18u)
#define BPLIB_RESET_BNDL_CTRS_SUCCESS_EID           (19u)
#define BPLIB_RESET_ERR_CTRS_SUCCESS_EID            (20u)
#define BPLIB_ADD_APP_SUCCESS_EID                   (21u)
#define BPLIB_RM_APP_SUCCESS_EID                    (22u)
#define BPLIB_SET_REGI_STAT_SUCCESS_EID             (23u)
#define BPLIB_START_APP_SUCCESS_EID                 (24u)
#define BPLIB_STOP_APP_SUCCESS_EID                  (25u)
#define BPLIB_ADD_AUTH_SRCS_SUCCESS_EID             (26u)
#define BPLIB_RM_AUTH_SRCS_SUCCESS_EID              (27u)
#define BPLIB_ADD_AUTH_CUST_SRCS_SUCESS_EID         (28u)
#define BPLIB_RM_AUTH_CUST_SRCS_SUCCESS_EID         (29u)
#define BPLIB_ADD_AUTH_CUSTODIANS_SUCCESS_EID       (30u)
#define BPLIB_RM_AUTH_CUSTODIANS_SUCCESS_EID        (31u)
#define BPLIB_ADD_AUTH_RPT_EID_SUCCESS_EID          (32u)
#define BPLIB_RM_AUTH_RPT_EID_SUCCESS_EID           (33u)
#define BPLIB_ADD_LATENCY_SUCCESS_EID               (34u)
#define BPLIB_RM_LATENCY_SUCCESS_EID                (35u)
#define BPLIB_CONTACT_SETUP_SUCCESS_EID             (36u)
#define BPLIB_CONTACT_START_SUCCESS_EID             (37u)
#define BPLIB_CONTACT_STOP_SUCCESS_EID              (38u)
#define BPLIB_CONTACT_TEARDOWN_SUCCESS_EID          (39u)
#define BPLIB_ADD_MIB_ARR_KEY_SUCCESS_EID           (40u)
#define BPLIB_RM_MIB_ARR_KEY_SUCCESS_EID            (41u)
#define BPLIB_SET_MIB_ITEM_SUCCESS_EID              (42u)
#define BPLIB_ADD_STOR_ALLOC_SUCCESS_EID            (43u)
#define BPLIB_RM_STOR_ALLOC_SUCCESS_EID             (44u)
#define BPLIB_PERFORM_SELF_TEST_SUCCESS_EID         (45u)
#define BPLIB_SEND_NODE_MIB_CFG_HK_SUCCESS_EID      (46u)
#define BPLIB_SEND_SRC_MIB_CFG_HK_SUCCESS_EID       (47u)
#define BPLIB_SEND_NODE_MIB_CTRS_HK_SUCCESS_EID     (48u)
#define BPLIB_SEND_SRC_MIB_CTRS_HK_SUCCESS_EID      (49u)
#define BPLIB_SEND_STOR_HK_SUCCESS_EID              (50u)
#define BPLIB_SEND_CHAN_CONTACT_STAT_HK_SUCCESS_EID (51u)
#define BPLIB_ADD_ALL_APPS_ERR_EID                  (52u)
#define BPLIB_START_ALL_APPS_ERR_EID                (53u)
#define BPLIB_VERIF_BNDL_ERR_EID                    (54u)
#define BPLIB_INIT_BNDL_STOR_ERR_EID                (55u)
#define BPLIB_VERIF_BNDL_META_ERR_EID               (56u)
#define BPLIB_REBUILD_BNDL_META_ERR_EID             (57u)
#define BPLIB_CLR_VOLATILE_ERR_EID                  (58u)
#define BPLIB_RELOAD_SVD_DATA_ERR_EID               (59u)
#define BPLIB_RESET_ALL_CTRS_ERR_EID                (60u)
#define BPLIB_RESET_CTR_ERR_EID                     (61u)
#define BPLIB_RESET_SRC_CTRS_ERR_EID                (62u)
#define BPLIB_RESET_BNDL_CTRS_ERR_EID               (63u)
#define BPLIB_RESET_ERR_CTRS_ERR_EID                (64u)
#define BPLIB_ADD_APP_ERR_EID                       (65u)
#define BPLIB_ADU_ADD_CHAN_ERR_EID                  (66u)
#define BPLIB_ADU_ADD_STAT_ERR_EID                  (67u)
#define BPLIB_RM_APP_ERR_EID                        (68u)
#define BPLIB_SET_REGI_STAT_ERR_EID                 (69u)
#define BPLIB_START_APP_ERR_EID                     (70u)
#define BPLIB_ADU_TOO_BIG_ERR_EID                   (71u)
#define BPLIB_ADU_START_CHAN_ERR_EID                (72u)
#define BPLIB_ADU_START_STAT_ERR_EID                (73u)
#define BPLIB_ADU_START_SUB_ERR_EID                 (74u)
#define BPLIB_STOP_APP_ERR_EID                      (75u)
#define BPLIB_ADU_STOP_CHAN_ERR_EID                 (76u)
#define BPLIB_ADU_STOP_STAT_ERR_EID                 (77u)
#define BPLIB_ADU_UNSUB_ERR_EID                     (78u)
#define BPLIB_ADD_AUTH_SRCS_ERR_EID                 (79u)
#define BPLIB_RM_AUTH_SRCS_ERR_EID                  (80u)
#define BPLIB_RM_AUTH_CUST_SRCS_ERR_EID             (81u)
#define BPLIB_ADD_AUTH_CUSTODIANS_ERR_EID           (82u)
#define BPLIB_RM_AUTH_CUSTODIANS_ERR_EID            (83u)
#define BPLIB_ADD_AUTH_RPT_EID_ERR_EID              (84u)
#define BPLIB_RM_AUTH_RPT_EID_ERR_EID               (85u)
#define BPLIB_ADD_LATENCY_ERR_EID                   (86u)
#define BPLIB_RM_LATENCY_ERR_EID                    (87u)
#define BPLIB_CONTACT_SETUP_ERR_EID                 (88u)
#define BPLIB_CONTACT_START_ERR_EID                 (89u)
#define BPLIB_CONTACT_STOP_ERR_EID                  (90u)
#define BPLIB_CONTACT_TEARDOWN_ERR_EID              (91u)
#define BPLIB_ADD_MIB_ARR_KEY_ERR_EID               (92u)
#define BPLIB_RM_MIB_ARR_KEY_ERR_EID                (93u)
#define BPLIB_SET_MIB_ITEM_ERR_EID                  (94u)
#define BPLIB_ADD_STOR_ALLOC_ERR_EID                (95u)
#define BPLIB_RM_STOR_ALLOC_ERR_EID                 (96u)
#define BPLIB_PERFORM_SELF_TEST_ERR_EID             (97u)
#define BPLIB_SEND_NODE_MIB_CFG_ERR_EID             (98u)
#define BPLIB_SEND_SRC_MIB_CFG_ERR_EID              (99u)
#define BPLIB_SEND_NODE_MIB_CTRS_ERR_EID            (100u)
#define BPLIB_SEND_SRC_MIB_CTRS_ERR_EID             (101u)
#define BPLIB_SEND_STOR_ERR_EID                     (102u)
#define BPLIB_SEND_CHAN_CONTACT_STAT_ERR_EID        (103u)

#endif /* BPLIB_EVENTIDS_H */