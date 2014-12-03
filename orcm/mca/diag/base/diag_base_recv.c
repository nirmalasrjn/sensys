/*
 * Copyright (c) 2014      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "orcm_config.h"
#include "orcm/constants.h"
#include "orcm/types.h"

#include "opal/dss/dss.h"
#include "opal/mca/mca.h"
#include "opal/util/output.h"
#include "opal/util/malloc.h"
#include "opal/mca/base/base.h"

#include "orte/types.h"
#include "orte/mca/rml/rml.h"
#include "orte/mca/errmgr/errmgr.h"
#include "orte/util/name_fns.h"

#include "orcm/runtime/orcm_globals.h"
#include "orcm/mca/diag/base/base.h"

static bool recv_issued=false;

static void orcm_diag_base_recv(int status, orte_process_name_t* sender,
                                opal_buffer_t* buffer, orte_rml_tag_t tag,
                                void* cbdata);

int orcm_diag_base_comm_start(void)
{
    if (recv_issued) {
        return ORTE_SUCCESS;
    }

    OPAL_OUTPUT_VERBOSE((5, orcm_diag_base_framework.framework_output,
                         "%s diag:base:receive start comm",
                         ORTE_NAME_PRINT(ORTE_PROC_MY_NAME)));

    orte_rml.recv_buffer_nb(ORTE_NAME_WILDCARD,
                            ORCM_RML_TAG_DIAG,
                            ORTE_RML_PERSISTENT,
                            orcm_diag_base_recv,
                            NULL);
    recv_issued = true;

    return ORCM_SUCCESS;
}

int orcm_diag_base_comm_stop(void)
{
    if (!recv_issued) {
        return ORCM_SUCCESS;
    }

    OPAL_OUTPUT_VERBOSE((5, orcm_diag_base_framework.framework_output,
                         "%s diag:base:receive stop comm",
                         ORTE_NAME_PRINT(ORTE_PROC_MY_NAME)));

    orte_rml.recv_cancel(ORTE_NAME_WILDCARD, ORCM_RML_TAG_DIAG);
    recv_issued = false;

    return ORCM_SUCCESS;
}

/* process incoming messages in order of receipt */
static void orcm_diag_base_recv(int status, orte_process_name_t *sender,
                                opal_buffer_t *buffer, orte_rml_tag_t tag,
                                void *cbdata)
{
    orcm_diag_cmd_flag_t command;
    char *dname = NULL;
    bool want_result;
    int rc, response, cnt;
    opal_buffer_t *ans = NULL;
    opal_buffer_t *buf = NULL;

    OPAL_OUTPUT_VERBOSE((5, orcm_diag_base_framework.framework_output,
                         "%s diag:base:receive processing msg",
                         ORTE_NAME_PRINT(ORTE_PROC_MY_NAME)));

    /* unpack the command */
    cnt = 1;
    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &command,
                                              &cnt, ORCM_DIAG_CMD_T))) {
        ORTE_ERROR_LOG(rc);
        return;
    }

    if (ORCM_DIAG_START_COMMAND == command) {
        /* start selected diagnostics */
        /* unpack the module name */
        cnt = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &dname,
                                                  &cnt, OPAL_STRING))) {
            ORTE_ERROR_LOG(rc);
            return;
        }
        /* unpack if sender wants diag results */
        cnt = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &want_result,
                                                  &cnt, OPAL_BOOL))) {
            ORTE_ERROR_LOG(rc);
            return;
        }
        /* unpack the (optional) options buffer */
        cnt = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &buf,
                                                  &cnt, OPAL_STRING))) {
            if (OPAL_ERR_UNPACK_READ_PAST_END_OF_BUFFER != rc) {
                ORTE_ERROR_LOG(rc);
                return;

            }
        }

        if (!want_result) {
            /* send back the immediate success, 
             * sender doesn't want to wait for diag results */
            ans = OBJ_NEW(opal_buffer_t);
            response = ORCM_SUCCESS;
            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &response, 1, OPAL_INT))) {
                ORTE_ERROR_LOG(rc);
                OBJ_RELEASE(ans);
                return;
            }
            if (ORTE_SUCCESS !=
                (rc = orte_rml.send_buffer_nb(sender, ans,
                                              ORCM_RML_TAG_DIAG,
                                              orte_rml_send_callback, NULL))) {
                ORTE_ERROR_LOG(rc);
                OBJ_RELEASE(ans);
                return;
            }
        }
        orcm_diag_base_activate(dname, want_result, sender, buf);
    } else if (ORCM_DIAG_AGG_COMMAND == command) {
        /* incoming results from children */
        /* unpack the module name */
        cnt = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &dname,
                                                  &cnt, OPAL_STRING))) {
            ORTE_ERROR_LOG(rc);
            return;
        }
        /* unpack the buffer */
        cnt = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &buf,
                                                  &cnt, OPAL_STRING))) {
            ORTE_ERROR_LOG(rc);
            return;
        }
        orcm_diag_base_log(dname, buf);
        OBJ_RELEASE(buf);
    }
    
    free(dname);
    return;
}
