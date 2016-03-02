/*
 * Copyright (c) 2016      Intel Inc
 *                         All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "orcm_config.h"
#include "orcm/constants.h"
#include "orcm/types.h"

#include <string.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "orcm_config.h"
#include "orcm/constants.h"
#include "opal/dss/dss.h"
#include "orte/mca/errmgr/errmgr.h"
#include "orte/mca/rml/rml.h"
#include "orte/util/name_fns.h"
#include "orte/runtime/orte_wait.h"
#include "orte/mca/notifier/base/base.h"
#include "orcm/runtime/orcm_globals.h"
#include "orcm/runtime/orcm_cmd_server.h"
#include "orcm/runtime/led_control_interface.h"

#define  NULL_CHECK(p) if(NULL==p) {goto ERROR;}
#define SAFE_RELEASE(p) if(NULL != p) OBJ_RELEASE(p);

static bool recv_issued=false;
static int unpack_command_subcommand(opal_buffer_t* buffer, orcm_cmd_server_flag_t *command,
                                     orcm_cmd_server_flag_t *sub_command);
static int orcm_cmd_server_set_policy(opal_buffer_t* buffer);
static int orcm_cmd_server_get_policy(opal_buffer_t* buffer, opal_buffer_t **pack_buffer, int *count);
static int pack_severity_action(opal_buffer_t **result_buff, orte_notifier_severity_t *sev,
                                char **action);
static int unpack_severity_action(opal_buffer_t *buffer, orte_notifier_severity_t *sev,
                                char **action);
static int orcm_cmd_server_set_smtp(opal_buffer_t* buffer);
static int pack_opal_value(opal_buffer_t **buffer, opal_value_t **kv);
static int unpack_opal_value(opal_buffer_t *buffer, opal_value_t **kv);
static int orcm_cmd_server_get_smtp(opal_buffer_t* buffer, opal_buffer_t **result_buff, int *count);

int orcm_cmd_server_init(void)
{
    if (!recv_issued) {
        orte_rml.recv_buffer_nb(ORTE_NAME_WILDCARD,
                                ORCM_RML_TAG_CMD_SERVER,
                                ORTE_RML_PERSISTENT,
                                orcm_cmd_server_recv,
                                NULL);
        recv_issued = true;
    }
    return ORCM_SUCCESS;
}

void orcm_cmd_server_finalize(void)
{
    if (recv_issued) {
        orte_rml.recv_cancel(ORTE_NAME_WILDCARD, ORCM_RML_TAG_CMD_SERVER);
        recv_issued = false;
    }
}

void orcm_cmd_server_recv(int status, orte_process_name_t* sender,
                      opal_buffer_t* buffer, orte_rml_tag_t tag,
                      void* cbdata)
{
    orcm_cmd_server_flag_t command, sub_command;
    opal_buffer_t *pack_buff = NULL;
    opal_buffer_t *result_buff = NULL;
    char *error = NULL;
    int rc = ORCM_SUCCESS;
    int response = ORCM_SUCCESS;
    int count = 0;

    rc = unpack_command_subcommand(buffer, &command, &sub_command);
    if (ORCM_SUCCESS != rc) {
        goto ERROR;
    }
    if (ORCM_SET_NOTIFIER_COMMAND == command) {
        pack_buff = OBJ_NEW(opal_buffer_t);
        NULL_CHECK(pack_buff);
        switch(sub_command) {
        case ORCM_SET_NOTIFIER_POLICY_COMMAND:
            response = orcm_cmd_server_set_policy(buffer);
            break;
        case ORCM_SET_NOTIFIER_SMTP_COMMAND:
            response = orcm_cmd_server_set_smtp(buffer);
            break;
        default:
            asprintf(&error,"invalid notifier set command");
            goto ERROR;
        }
        if (ORCM_SUCCESS != response) {
            goto ERROR;
        }
        if (OPAL_SUCCESS != 
            (rc = opal_dss.pack(pack_buff, &response, 1, OPAL_INT))) {
             goto ERROR;
        }
        goto RESPONSE;

    } else if (ORCM_GET_NOTIFIER_COMMAND == command) {
        pack_buff = OBJ_NEW(opal_buffer_t);
        NULL_CHECK(pack_buff);
        switch(sub_command) {
        case ORCM_GET_NOTIFIER_POLICY_COMMAND:
            response = orcm_cmd_server_get_policy(buffer, &result_buff, &count);
            break;
        case ORCM_GET_NOTIFIER_SMTP_COMMAND:
            response = orcm_cmd_server_get_smtp(buffer, &result_buff, &count);
            break;
        default:
            asprintf(&error,"invalid notifier get command");
            goto ERROR;
        }
        if (ORCM_SUCCESS != response) {
            goto ERROR;
        }
        rc = opal_dss.pack(pack_buff, &response, 1, OPAL_INT);
        if (OPAL_SUCCESS != rc) {
            goto ERROR;
        }
        rc = opal_dss.pack(pack_buff, &count, 1, OPAL_INT);
        if (OPAL_SUCCESS != rc) {
            goto ERROR;
        }
        rc = opal_dss.pack(pack_buff, &result_buff, 1, OPAL_BUFFER);
        goto RESPONSE;
    } else if (ORCM_GET_CHASSIS_ID == command || ORCM_SET_CHASSIS_ID == command){
        // unpack BMC login information..
        // LedControl lc(hostname, user, pass);
        init_led_control();
        int state;
        int seconds = 0;
        int cnt = 1;
        pack_buff = OBJ_NEW(opal_buffer_t);
        switch (sub_command){
            case ORCM_GET_CHASSIS_ID_STATE:
                state = get_chassis_id_state();
                response = ORCM_SUCCESS;
                if (OPAL_SUCCESS != (rc = opal_dss.pack(pack_buff, &response, 1, OPAL_INT))){
                    fini_led_control();
                    goto ERROR;
                }
                if (OPAL_SUCCESS != (rc = opal_dss.pack(pack_buff, &state, 1, OPAL_INT))){
                    fini_led_control();
                    goto ERROR;
                }
                break;
            case ORCM_SET_CHASSIS_ID_OFF:
                response = ORCM_ERROR;
                if (!disable_chassis_id())
                    response = ORCM_SUCCESS;
                if (OPAL_SUCCESS != (rc = opal_dss.pack(pack_buff, &response, 1, OPAL_INT))){
                    fini_led_control();
                    goto ERROR;
                }
                break;
            case ORCM_SET_CHASSIS_ID_ON:
                response = ORCM_ERROR;
                if (!enable_chassis_id())
                    response = ORCM_SUCCESS;
                if (OPAL_SUCCESS != (rc = opal_dss.pack(pack_buff, &response, 1, OPAL_INT))){
                    fini_led_control();
                    goto ERROR;
                }
                break;
            case ORCM_SET_CHASSIS_ID_TEMPORARY_ON:
                cnt = 1;
                if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &seconds,
                                                         &cnt, OPAL_INT))){
                    fini_led_control();
                    goto ERROR;
                }
                response = ORCM_ERROR;
                if (!enable_chassis_id_with_timeout(seconds))
                    response = ORCM_SUCCESS;
                if (OPAL_SUCCESS != (rc = opal_dss.pack(pack_buff, &response, 1, OPAL_INT))){
                    fini_led_control();
                    goto ERROR;
                }
                break;
            default:
                asprintf(&error,"invalid chassis-id command");
                fini_led_control();
                goto ERROR;
        }
        fini_led_control();
        goto RESPONSE;
    }

ERROR:

    if (NULL == pack_buff) {
         pack_buff = OBJ_NEW(opal_buffer_t);
    }
    if (OPAL_SUCCESS != (rc = opal_dss.pack(pack_buff, &response, 1, OPAL_INT))) {
        ORTE_ERROR_LOG(rc);
        SAFE_RELEASE(pack_buff);
        if (NULL != error) {
            free(error);
        }
        return;
    }
    if (NULL == error) {
        asprintf(&error,"notifier data buffer mismatch");
    }

    if (OPAL_SUCCESS != (rc = opal_dss.pack(pack_buff, &error, 1, OPAL_STRING))) {
        ORTE_ERROR_LOG(rc);
        SAFE_RELEASE(pack_buff);
        free(error);
        return;
    }


RESPONSE:
    if (NULL != error) {
        free(error);
    }
    if (ORTE_SUCCESS !=
        (rc = orte_rml.send_buffer_nb(sender, pack_buff,
                                      ORCM_RML_TAG_CMD_SERVER,
                                      orte_rml_send_callback, NULL))) {
        ORTE_ERROR_LOG(rc);
        SAFE_RELEASE(pack_buff);
        return;
    }
}

static int unpack_command_subcommand(opal_buffer_t* buffer, orcm_cmd_server_flag_t *command,
                                     orcm_cmd_server_flag_t *sub_command)
{
    int rc = ORCM_SUCCESS;
    int cnt = 1;
    /* unpack the command */
    cnt = 1;
    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, command,
                                              &cnt, ORCM_CMD_SERVER_T))) {
        return rc;
    }
    cnt = 1;
    /* unpack the subcommand */
    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, sub_command,
                                              &cnt, ORCM_CMD_SERVER_T))) {
        return rc;
    }
    return rc;
}

static int orcm_cmd_server_set_policy(opal_buffer_t* buffer)
{
    int rc = ORCM_SUCCESS;
    orte_notifier_severity_t sev;
    char *action = NULL;

    rc = unpack_severity_action(buffer, &sev, &action);
    if (ORCM_SUCCESS != rc) {
        return rc;
    }
    return set_notifier_policy(sev, action);
}

static int orcm_cmd_server_get_policy(opal_buffer_t* buffer, opal_buffer_t **result_buff, int *count)
{
    int rc = ORCM_SUCCESS;
    orte_notifier_severity_t sev;
    char *action = NULL;
    int pack_count = 0;

    *result_buff = OBJ_NEW(opal_buffer_t);
    if(NULL == *result_buff) {
        return ORCM_ERR_OUT_OF_RESOURCE;
    }
    for(sev = ORTE_NOTIFIER_EMERG; sev <= ORTE_NOTIFIER_DEBUG; sev++) {
        action = (char *) get_notifier_policy(sev);
        pack_count++;
        rc = pack_severity_action(result_buff, &sev, &action);
        if (ORCM_SUCCESS != rc) {
            SAFE_RELEASE(*result_buff);
            *result_buff = NULL;
            return rc;
        }
    }
    *count = pack_count;
    return rc;
}

static int pack_severity_action(opal_buffer_t **buffer, orte_notifier_severity_t *sev,
                                char **action)
{
    int rc = ORCM_SUCCESS;

    if (NULL == *action) {
        *action = orte_notifier_base.default_actions;
    }
    rc = opal_dss.pack(*buffer, sev, 1, ORTE_NOTIFIER_SEVERITY_T);
    if (OPAL_SUCCESS != rc) {
        return rc;
    }
    rc = opal_dss.pack(*buffer, action, 1, OPAL_STRING);
    return rc;
}

static int unpack_severity_action(opal_buffer_t *buffer, orte_notifier_severity_t *sev,
                                char **action)
{
    int rc = ORCM_SUCCESS;
    int cnt = 1;
        /* unpack the severity */
    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, sev,
                                              &cnt, ORTE_NOTIFIER_SEVERITY_T))) {
        return rc;
    }
    cnt = 1;
    /* unpack the action */
    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, action,
                                              &cnt, OPAL_STRING))) {
        return rc;
    }
    return rc;
}

static int orcm_cmd_server_set_smtp(opal_buffer_t* buffer)
{
    int rc = ORCM_SUCCESS;
    opal_value_t *kv = NULL;

    rc = unpack_opal_value(buffer, &kv);
    if (ORCM_SUCCESS != rc) {
        return rc;
    }
    return orte_notifier_base_set_config("smtp",kv);
}

static int pack_opal_value(opal_buffer_t **buffer, opal_value_t **kv)
{
    return opal_dss.pack(*buffer, kv, 1, OPAL_VALUE);
}

static int unpack_opal_value(opal_buffer_t *buffer, opal_value_t **kv)
{
    int cnt = 1;
    /*unpack the opal_value_t*/
    return opal_dss.unpack(buffer, kv,
                           &cnt, OPAL_VALUE);
}

static int orcm_cmd_server_get_smtp(opal_buffer_t* buffer, opal_buffer_t **result_buff, int *count)
{
    int rc = ORCM_SUCCESS;
    opal_list_t *list = NULL;
    opal_value_t *kv = NULL;
    int pack_count = 0;

    *result_buff = OBJ_NEW(opal_buffer_t);
    if(NULL == *result_buff) {
        return ORCM_ERR_OUT_OF_RESOURCE;
    }
    list = OBJ_NEW(opal_list_t);
    if(NULL == list) {
        OBJ_RELEASE(*result_buff);
        *result_buff = NULL;
        return ORCM_ERR_OUT_OF_RESOURCE;
    }
    rc = orte_notifier_base_get_config("smtp", &list);
    if (ORCM_SUCCESS != rc) {
        OBJ_RELEASE(*result_buff);
        OBJ_RELEASE(list);
        *result_buff = NULL;
        return rc;
    }
    OPAL_LIST_FOREACH(kv, list, opal_value_t) {
        pack_count++;
        rc = pack_opal_value(result_buff, &kv);
        if (ORCM_SUCCESS != rc) {
            OBJ_RELEASE(*result_buff);
            OBJ_RELEASE(list);
            *result_buff = NULL;
            return rc;
        }
    }
    *count = pack_count;
    OBJ_RELEASE(list);
    return rc;
}