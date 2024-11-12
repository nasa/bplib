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

#define BPLIB_RESERVED_EID                              (500u)

/* ============ */
/* NC Event IDs */
/* ============ */

/* NC success event IDs */
#define BPLIB_NC_ADD_ALL_APPS_SUCCESS_EID               (501u)
#define BPLIB_NC_START_ALL_APPS_SUCCESS_EID             (502u)
#define BPLIB_NC_VERIF_BNDL_SUCCESS_EID                 (503u)
#define BPLIB_NC_INIT_BNDL_STOR_SUCCESS_EID             (504u)
#define BPLIB_NC_VERIF_BNDL_META_SUCCESS_EID            (505u)
#define BPLIB_NC_REBUILD_BNDL_META_SUCCESS_EID          (506u)
#define BPLIB_NC_CLR_VOLATILE_SUCCESS_EID               (507u)
#define BPLIB_NC_RELOAD_SVD_DATA_SUCCESS_EID            (508u)
#define BPLIB_NC_RESET_ALL_CTRS_SUCCESS_EID             (509u)
#define BPLIB_NC_RESET_CTR_SUCCESS_EID                  (510u)
#define BPLIB_NC_RESET_SRC_CTRS_SUCCESS_EID             (511u)
#define BPLIB_NC_RESET_BNDL_CTRS_SUCCESS_EID            (512u)
#define BPLIB_NC_RESET_ERR_CTRS_SUCCESS_EID             (513u)
#define BPLIB_NC_ADD_APP_SUCCESS_EID                    (514u)
#define BPLIB_NC_RM_APP_SUCCESS_EID                     (515u)
#define BPLIB_NC_SET_REGI_STAT_SUCCESS_EID              (516u)
#define BPLIB_NC_START_APP_SUCCESS_EID                  (517u)
#define BPLIB_NC_STOP_APP_SUCCESS_EID                   (518u)
#define BPLIB_NC_ADD_AUTH_SRCS_SUCCESS_EID              (519u)
#define BPLIB_NC_RM_AUTH_SRCS_SUCCESS_EID               (520u)
#define BPLIB_NC_ADD_AUTH_CUST_SRCS_SUCCESS_EID         (521u)
#define BPLIB_NC_RM_AUTH_CUST_SRCS_SUCCESS_EID          (522u)
#define BPLIB_NC_ADD_AUTH_CUSTODIANS_SUCCESS_EID        (523u)
#define BPLIB_NC_RM_AUTH_CUSTODIANS_SUCCESS_EID         (524u)
#define BPLIB_NC_ADD_AUTH_RPT_EID_SUCCESS_EID           (525u)
#define BPLIB_NC_RM_AUTH_RPT_EID_SUCCESS_EID            (526u)
#define BPLIB_NC_ADD_LATENCY_SUCCESS_EID                (527u)
#define BPLIB_NC_RM_LATENCY_SUCCESS_EID                 (528u)
#define BPLIB_NC_CONTACT_SETUP_SUCCESS_EID              (529u)
#define BPLIB_NC_CONTACT_START_SUCCESS_EID              (530u)
#define BPLIB_NC_CONTACT_STOP_SUCCESS_EID               (531u)
#define BPLIB_NC_CONTACT_TEARDOWN_SUCCESS_EID           (532u)
#define BPLIB_NC_ADD_MIB_ARR_KEY_SUCCESS_EID            (533u)
#define BPLIB_NC_RM_MIB_ARR_KEY_SUCCESS_EID             (534u)
#define BPLIB_NC_SET_MIB_ITEM_SUCCESS_EID               (535u)
#define BPLIB_NC_ADD_STOR_ALLOC_SUCCESS_EID             (536u)
#define BPLIB_NC_RM_STOR_ALLOC_SUCCESS_EID              (537u)
#define BPLIB_NC_PERFORM_SELF_TEST_SUCCESS_EID          (538u)

/* NC error event IDs */
#define BPLIB_NC_ADD_ALL_APPS_ERR_EID                   (539u)
#define BPLIB_NC_START_ALL_APPS_ERR_EID                 (540u)
#define BPLIB_NC_VERIF_BNDL_ERR_EID                     (541u)
#define BPLIB_NC_INIT_BNDL_STOR_ERR_EID                 (542u)
#define BPLIB_NC_VERIF_BNDL_META_ERR_EID                (543u)
#define BPLIB_NC_REBUILD_BNDL_META_ERR_EID              (544u)
#define BPLIB_NC_CLR_VOLATILE_ERR_EID                   (545u)
#define BPLIB_NC_RELOAD_SVD_DATA_ERR_EID                (546u)
#define BPLIB_NC_RESET_CTR_SRC_EID_ERR_EID              (547u)
#define BPLIB_NC_RESET_CTR_UNK_SRC_CNTR_ERR_EID         (548u)
#define BPLIB_NC_RESET_CTR_UNK_NODE_CNTR_ERR_EID        (549u)
#define BPLIB_NC_RESET_SRC_CTRS_ERR_EID                 (550u)
#define BPLIB_NC_RESET_BNDL_CTRS_ERR_EID                (551u)
#define BPLIB_NC_RESET_ERR_CTRS_ERR_EID                 (552u)
#define BPLIB_NC_ADD_APP_ERR_EID                        (553u)
#define BPLIB_NC_RM_APP_ERR_EID                         (554u)
#define BPLIB_NC_SET_REGI_STAT_ERR_EID                  (555u)
#define BPLIB_NC_START_APP_ERR_EID                      (556u)
#define BPLIB_NC_STOP_APP_ERR_EID                       (557u)
#define BPLIB_NC_ADD_AUTH_SRCS_ERR_EID                  (558u)
#define BPLIB_NC_RM_AUTH_SRCS_ERR_EID                   (559u)
#define BPLIB_NC_ADD_AUTH_CUST_SRCS_ERR_EID             (560u)
#define BPLIB_NC_RM_AUTH_CUST_SRCS_ERR_EID              (561u)
#define BPLIB_NC_ADD_AUTH_CUSTODIANS_ERR_EID            (562u)
#define BPLIB_NC_RM_AUTH_CUSTODIANS_ERR_EID             (563u)
#define BPLIB_NC_ADD_AUTH_RPT_EID_ERR_EID               (564u)
#define BPLIB_NC_RM_AUTH_RPT_EID_ERR_EID                (565u)
#define BPLIB_NC_ADD_LATENCY_ERR_EID                    (566u)
#define BPLIB_NC_RM_LATENCY_ERR_EID                     (567u)
#define BPLIB_NC_CONTACT_SETUP_ERR_EID                  (568u)
#define BPLIB_NC_CONTACT_START_ERR_EID                  (569u)
#define BPLIB_NC_CONTACT_STOP_ERR_EID                   (570u)
#define BPLIB_NC_CONTACT_TEARDOWN_ERR_EID               (571u)
#define BPLIB_NC_ADD_MIB_ARR_KEY_ERR_EID                (572u)
#define BPLIB_NC_RM_MIB_ARR_KEY_ERR_EID                 (573u)
#define BPLIB_NC_SET_MIB_ITEM_ERR_EID                   (574u)
#define BPLIB_NC_ADD_STOR_ALLOC_ERR_EID                 (575u)
#define BPLIB_NC_RM_STOR_ALLOC_ERR_EID                  (576u)
#define BPLIB_NC_PERFORM_SELF_TEST_ERR_EID              (577u)

/* ============ */
/* AS event IDs */
/* ============ */

/* AS success event IDs */
#define BPLIB_AS_INIT_SUCCESS_EID                       (578u)
#define BPLIB_AS_INCREMENT_SUCCESS_EID                  (579u)
#define BPLIB_AS_DECREMENT_SUCCESS_EID                  (580u)
#define BPLIB_AS_RESET_ALL_SUCCESS_EID                  (581u)

/* AS error event IDs */
#define BPLIB_AS_INCREMENT_INVAL_EID_ERR_EID            (582u)
#define BPLIB_AS_INCREMENT_UNKNOWN_NODE_CNTR_ERR_EID    (583u)
#define BPLIB_AS_INCREMENT_UNKNOWN_SRC_CNTR_ERR_EID     (584u)
#define BPLIB_AS_DECREMENT_INVAL_EID_ERR_EID            (585u)
#define BPLIB_AS_DECREMENT_UNKNOWN_NODE_CNTR_ERR_EID    (586u)
#define BPLIB_AS_DECREMENT_UNKNOWN_SRC_CNTR_ERR_EID     (587u)
#define BPLIB_AS_RESET_SRC_INVAL_EID_ERR_EID            (588u)
#define BPLIB_AS_RESET_SRC_ERR_EID                      (589u)
#define BPLIB_AS_RESET_BNDL_INVAL_EID_ERR_EID           (590u)
#define BPLIB_AS_RESET_BNDL_NODE_ERR_EID                (591u)
#define BPLIB_AS_RESET_BNDL_SRC_ERR_EID                 (592u)
#define BPLIB_AS_RESET_ERR_ERR_EID                      (593u)
#define BPLIB_AS_RESET_ALL_INVAL_EID_ERR_EID            (594u)
#define BPLIB_AS_RESET_ALL_NODE_ERR_EID                 (595u)
#define BPLIB_AS_RESET_ALL_SRC_ERR_EID                  (596u)

#endif /* BPLIB_EVENTIDS_H */