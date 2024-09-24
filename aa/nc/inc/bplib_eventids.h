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

#define BPLIB_RESERVED_EID                          0
#define BPLIB_INIT_INF_EID                          1
#define BPLIB_CC_ERR_EID                            2
#define BPLIB_MID_ERR_EID                           3
#define BPLIB_CMD_LEN_ERR_EID                       4
#define BPLIB_PIPE_ERR_EID                          5
#define BPLIB_VALUE_INF_EID                         6
#define BPLIB_CR_CMD_PIPE_ERR_EID                   7
#define BPLIB_CR_WKP_PIPE_ERR_EID                   8
#define BPLIB_SUB_HK_ERR_EID                        9
#define BPLIB_SUB_CMD_ERR_EID                       10
#define BPLIB_SUB_WKP_ERR_EID                       11
#define BPLIB_TBL_REG_ERR_EID                       12
#define BPLIB_TBL_LD_ERR_EID                        13
#define BPLIB_TBL_ADDR_ERR_EID                      14
#define BPLIB_TBL_MNG_ERR_EID                       15
#define BPLIB_FWP_INIT_ERR_EID                      16
#define BPLIB_EXIT_CRIT_EID                         17
#define BPLIB_DELETE_HNDLR_ERR_EID                  18

#define BPLIB_ADU_IN_INIT_SEM_ERR_EID               19
#define BPLIB_ADU_IN_EXIT_SEM_ERR_EID               20
#define BPLIB_ADU_IN_CREATE_ERR_EID                 21
#define BPLIB_ADU_IN_RUN_ERR_EID                    22
#define BPLIB_ADU_IN_INIT_SEM_TK_ERR_EID            23
#define BPLIB_ADU_IN_INIT_INF_EID                   24
#define BPLIB_ADU_IN_EXIT_CRIT_EID                  25
#define BPLIB_ADU_IN_NO_ID_ERR_EID                  26
#define BPLIB_ADU_IN_INV_ID_ERR_EID                 27
#define BPLIB_ADU_IN_CR_PIPE_ERR_EID                28
#define BPLIB_ADU_IN_SUB_PIPE_ERR_EID               29
#define BPLIB_ADU_IN_UNK_EXIT_CRIT_EID              30

#define BPLIB_ADU_OUT_INIT_SEM_ERR_EID              31
#define BPLIB_ADU_OUT_EXIT_SEM_ERR_EID              32
#define BPLIB_ADU_OUT_CREATE_ERR_EID                33
#define BPLIB_ADU_OUT_RUN_ERR_EID                   34
#define BPLIB_ADU_OUT_INIT_SEM_TK_ERR_EID           35
#define BPLIB_ADU_OUT_INIT_INF_EID                  36
#define BPLIB_ADU_OUT_EXIT_CRIT_EID                 37
#define BPLIB_ADU_OUT_NO_ID_ERR_EID                 38
#define BPLIB_ADU_OUT_INV_ID_ERR_EID                39
#define BPLIB_ADU_OUT_UNK_EXIT_CRIT_EID             40

#define BPLIB_TIME_INIT_ERR_EID                     41
#define BPLIB_TIME_WKP_ERR_EID                      42

#define BPLIB_NOOP_SUCCESS_EID                      43
#define BPLIB_ADD_ALL_APPS_SUCCESS_EID              44
#define BPLIB_START_ALL_APPS_SUCCESS_EID            45
#define BPLIB_VERIF_BNDL_SUCCESS_EID                46
#define BPLIB_INIT_BNDL_STOR_SUCCESS_EID            47
#define BPLIB_VERIF_BNDL_META_SUCCESS_EID           48
#define BPLIB_REBUILD_BNDL_META_SUCCESS_EID         49
#define BPLIB_CLR_VOLATILE_SUCCESS_EID              50
#define BPLIB_RELOAD_SVD_DATA_SUCCESS_EID           51
#define BPLIB_RESET_ALL_CTRS_SUCCESS_EID            52
#define BPLIB_RESET_CTR_SUCCESS_EID                 53
#define BPLIB_RESET_SRC_CTRS_SUCCESS_EID            54
#define BPLIB_RESET_BNDL_CTRS_SUCCESS_EID           55
#define BPLIB_RESET_ERR_CTRS_SUCCESS_EID            56
#define BPLIB_ADD_APP_SUCCESS_EID                   57
#define BPLIB_RM_APP_SUCCESS_EID                    58
#define BPLIB_SET_REGI_STAT_SUCCESS_EID             59
#define BPLIB_START_APP_SUCCESS_EID                 60
#define BPLIB_STOP_APP_SUCCESS_EID                  61
#define BPLIB_ADD_AUTH_SRCS_SUCCESS_EID             62
#define BPLIB_RM_AUTH_SRCS_SUCCESS_EID              63
#define BPLIB_ADD_AUTH_CUST_SRCS_SUCESS_EID         64
#define BPLIB_RM_AUTH_CUST_SRCS_SUCCESS_EID         65
#define BPLIB_ADD_AUTH_CUSTODIANS_SUCCESS_EID       66
#define BPLIB_RM_AUTH_CUSTODIANS_SUCCESS_EID        67
#define BPLIB_ADD_AUTH_RPT_EID_SUCCESS_EID          68
#define BPLIB_RM_AUTH_RPT_EID_SUCCESS_EID           69
#define BPLIB_ADD_LATENCY_SUCCESS_EID               70
#define BPLIB_RM_LATENCY_SUCCESS_EID                71
#define BPLIB_CONTACT_SETUP_SUCCESS_EID             72
#define BPLIB_CONTACT_STOP_SUCCESS_EID              73
#define BPLIB_CONTACT_TEARDOWN_SUCCESS_EID          74
#define BPLIB_ADD_MIB_ARR_KEY_SUCCESS_EID           75
#define BPLIB_RM_MIB_ARR_KEY_SUCCESS_EID            76
#define BPLIB_SET_MIB_ITEM_SUCCESS_EID              77
#define BPLIB_ADD_STOR_ALLOC_SUCCESS_EID            78
#define BPLIB_RM_STOR_ALLOC_SUCCESS_EID             79
#define BPLIB_PERFORM_SELF_TEST_SUCCESS_EID         80
#define BPLIB_SEND_NODE_MIB_CFG_SUCCESS_EID         81
#define BPLIB_SEND_SRC_MIB_CFG_SUCCESS_EID          82
#define BPLIB_SEND_NODE_MIB_CTRS_SUCCESS_EID        83
#define BPLIB_SEND_SRC_MIB_CTRS_SUCCESS_EID         84
#define BPLIB_SEND_STOR_SUCCESS_EID                 85
#define BPLIB_SEND_CHAN_CONTACT_STAT_SUCCESS_EID    86

#endif /* BPLIB_EVENTIDS_H */