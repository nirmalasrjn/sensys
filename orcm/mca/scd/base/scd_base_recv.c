/*
 * Copyright (c) 2014-2016 Intel Corporation. All rights reserved.
 * Copyright (c) 2016      Intel Corporation. All rights reserved.
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
#include "orte/runtime/orte_wait.h"

#include "orcm/util/attr.h"
#include "orcm/runtime/orcm_globals.h"
#include "orcm/mca/scd/base/base.h"

#include "orcm/mca/db/db.h"
#include "orcm/mca/db/base/base.h"

#include "orcm/util/utils.h"
#include "orcm/util/logical_group.h"
#include "opal/mca/installdirs/installdirs.h"

#include <sys/wait.h>

/* Macros to help with code coverage */
#define ON_NULL_RETURN(x,y) \
    if(NULL==x){ORTE_ERROR_LOG(y);return y;}
#define ON_NULL_GOTO(x,label) \
    if(NULL==x){ORTE_ERROR_LOG(ORCM_ERR_OUT_OF_RESOURCE);goto label;}
#define ON_FAILURE_GOTO(x,label) \
    if(ORCM_SUCCESS!=x){ORTE_ERROR_LOG(x);goto label;}

static bool recv_issued=false;

static void orcm_scd_base_recv(int status, orte_process_name_t* sender,
                        opal_buffer_t* buffer, orte_rml_tag_t tag,
                        void* cbdata);

void open_callback(int dbhandle, int status, opal_list_t *in, opal_list_t *out, void *cbdata);
void close_callback(int dbhandle, int status, opal_list_t *in, opal_list_t *out, void *cbdata);
void fetch_callback(int dbhandle, int status, opal_list_t *in, opal_list_t *out, void *cbdata);
char* get_plugin_from_sensor_name(const char* sensor_name);
int get_inventory_list(opal_list_t *filters, opal_list_t **results);
void orcm_scd_base_fetch_recv(int status, orte_process_name_t* sender,
                              opal_buffer_t* buffer, orte_rml_tag_t tag,
                              void* cbdata);
int build_filter_list(opal_buffer_t* buffer, opal_list_t **filter_list);

int orcm_scd_base_comm_start(void)
{
    if (recv_issued) {
        return ORTE_SUCCESS;
    }

    OPAL_OUTPUT_VERBOSE((5, orcm_scd_base_framework.framework_output,
                         "%s scd:base:receive start comm",
                         ORTE_NAME_PRINT(ORTE_PROC_MY_NAME)));

    orte_rml.recv_buffer_nb(ORTE_NAME_WILDCARD,
                            ORCM_RML_TAG_SCD,
                            ORTE_RML_PERSISTENT,
                            orcm_scd_base_recv,
                            NULL);
    /* setup to to receive 'fetch' commands */
    orte_rml.recv_buffer_nb(ORTE_NAME_WILDCARD, ORCM_RML_TAG_ORCMD_FETCH, ORTE_RML_PERSISTENT,
                            orcm_scd_base_fetch_recv, NULL);

    recv_issued = true;

    return ORTE_SUCCESS;
}


int orcm_scd_base_comm_stop(void)
{
    if (!recv_issued) {
        return ORTE_SUCCESS;
    }

    OPAL_OUTPUT_VERBOSE((5, orcm_scd_base_framework.framework_output,
                         "%s scd:base:receive stop comm",
                         ORTE_NAME_PRINT(ORTE_PROC_MY_NAME)));

    orte_rml.recv_cancel(ORTE_NAME_WILDCARD, ORCM_RML_TAG_SCD);
    recv_issued = false;

    return ORTE_SUCCESS;
}

/* set the default action for a given signal */
static void set_handler_default(int sig)
{
    struct sigaction act;
    act.sa_handler = SIG_DFL;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(sig, &act, (struct sigaction *)0);
}

static int orcm_scd_base_fork_to_launch(char *exec, char **argv)
{
    sigset_t sigs;
    int s_pid = fork();

    if (s_pid < 0) {
        ORTE_ERROR_LOG(ORTE_ERR_SYS_LIMITS_CHILDREN);
        return ORTE_ERR_SYS_LIMITS_CHILDREN;
    } else if (s_pid == 0) {
        /* I am the child - I am going to exec the executable */

        /* Set signal handlers back to the default.  Do this close
           to the execve() because the event library may (and likely
           will) reset them.  If we don't do this, the event
           library may have left some set that, at least on some
           OS's, don't get reset via fork() or exec().  Hence, the
           orted could be unkillable (for example). */
        set_handler_default(SIGTERM);
        set_handler_default(SIGINT);
        set_handler_default(SIGHUP);
        set_handler_default(SIGPIPE);
        set_handler_default(SIGCHLD);

        /* Unblock all signals, for many of the same reasons that
           we set the default handlers, above.  This is noticable
           on Linux where the event library blocks SIGTERM, but we
           don't want that blocked by the orted (or, more
           specifically, we don't want it to be blocked by the
           orted and then inherited by the ORTE processes that it
           forks, making them unkillable by SIGTERM). */
        sigprocmask(0, 0, &sigs);
        sigprocmask(SIG_UNBLOCK, &sigs, 0);

        execve(exec, argv, orte_launch_environ);

        /* if I get here, the execve must be failed! */
        ORTE_ERROR_LOG(errno);
        fprintf(stderr, "orcmsched fails when launching an executable:%s\n", strerror(errno));
        exit(1);
    } else {

        /* I am the parent - No need to wait for the child to terminate.
         * If the launching failed, the child process will log the error
         * into syslog
         */
        return ORCM_SUCCESS;
    }
}

static int orcm_scd_base_unpack_exec_buffer(opal_buffer_t *buf, char **exec_name, char **exec_argv)
{
    int rc = ORCM_SUCCESS;
    int num_element = 1;
    bool exec_argv_set = false;

    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buf, exec_name, &num_element, OPAL_STRING))) {
        SAFEFREE(*exec_name);
        return rc;
    }
    num_element = 1;
    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buf, &exec_argv_set, &num_element, OPAL_BOOL))) {
        SAFEFREE(*exec_name);
        return rc;
    }
    num_element = 1;
    if (exec_argv_set &&
        OPAL_SUCCESS != (rc = opal_dss.unpack(buf, exec_argv, &num_element, OPAL_STRING))) {
        SAFEFREE(*exec_name);
        SAFEFREE(*exec_argv);
        return rc;
    }

    return ORCM_SUCCESS;
}

static char *orcm_scd_truncate_slash(char *actual_exec_name)
{
    char *ptr = strrchr(actual_exec_name, '/');
    if (NULL == ptr) {
        return actual_exec_name;
    }

    if (1 == strlen(ptr)) {
        return NULL;
    }

    return ptr + 1;
}

static int orcm_scd_base_get_full_exec_name(char *exec_name, char **exec_full_name)
{
    int rc = ORCM_SUCCESS;
    char *actual_exec_name = NULL;
    char *final_exec_name = NULL;
    char *ptr = NULL;

    if (NULL == orcm_event_exec_path || '0' == *orcm_event_exec_path) {
        return ORCM_ERROR;
    }

    if (ORCM_SUCCESS != (rc = orcm_logical_group_parse_string(exec_name, &actual_exec_name))) {
        SAFEFREE(actual_exec_name);
        return rc;
    }
    if (NULL == actual_exec_name) {
        return ORCM_ERROR;
    }
    if (NULL == (final_exec_name = orcm_scd_truncate_slash(actual_exec_name))) {
        return ORCM_ERROR;
    }

    ptr = strrchr(orcm_event_exec_path, '/');
    /* last character is '/' */
    if (NULL != ptr && 1 == strlen(ptr)) {
        asprintf(exec_full_name, "%s%s", orcm_event_exec_path, final_exec_name);
    } else {
        asprintf(exec_full_name, "%s/%s", orcm_event_exec_path, final_exec_name);
    }

    if (NULL == *exec_full_name) {
        SAFEFREE(actual_exec_name);
        return ORCM_ERR_OUT_OF_RESOURCE;
    }

    SAFEFREE(actual_exec_name);
    return ORCM_SUCCESS;
}

static int orcm_scd_base_free_exec(char *exec_name, char *exec_argv,
                                   char *exec_full_name, char **exec_argv_list, int rc)
{
    SAFEFREE(exec_name);
    SAFEFREE(exec_argv);
    SAFEFREE(exec_full_name);
    opal_argv_free(exec_argv_list);
    return rc;
}

static int orcm_scd_base_launch_exec(opal_buffer_t *buf)
{
    int rc = ORCM_SUCCESS;
    char *exec_name = NULL;
    char *exec_argv = NULL;
    char *exec_full_name = NULL;
    char **exec_argv_list = NULL;

    if (ORCM_SUCCESS != (rc = orcm_scd_base_unpack_exec_buffer(buf, &exec_name, &exec_argv))) {
        return rc;
    }

    if (NULL != exec_argv && NULL == (exec_argv_list = opal_argv_split(exec_argv, ','))) {
        return orcm_scd_base_free_exec(exec_name, exec_argv, NULL, NULL, ORCM_ERR_BAD_PARAM);
    }

    if (ORCM_SUCCESS != (rc = orcm_scd_base_get_full_exec_name(exec_name, &exec_full_name))) {
        return orcm_scd_base_free_exec(exec_name, exec_argv, NULL, exec_argv_list, rc);
    }

    if (NULL != exec_argv_list &&
        OPAL_SUCCESS != (rc = opal_argv_insert_element(&exec_argv_list, 0, exec_full_name))) {
        return orcm_scd_base_free_exec(exec_name, exec_argv, exec_full_name, exec_argv_list, rc);
    }

    rc = orcm_scd_base_fork_to_launch(exec_full_name, exec_argv_list);
    return orcm_scd_base_free_exec(exec_name, exec_argv, exec_full_name, exec_argv_list, rc);
}

/* process incoming messages in order of receipt */
static void orcm_scd_base_recv(int status, orte_process_name_t* sender,
                               opal_buffer_t* buffer, orte_rml_tag_t tag,
                               void* cbdata)
{
    orcm_scd_cmd_flag_t command, sub_command;
    int rc, i, j, cnt, result;
    int32_t int_param;
    int32_t *int_param_ptr;
    float float_param;
    float *float_param_ptr;
    bool bool_param;
    bool *bool_param_ptr;
    orcm_alloc_t *alloc, **allocs;
    opal_buffer_t *ans, *rmbuf;
    orcm_session_t *session;
    orcm_queue_t *q;
    orcm_node_t **nodes;
    orcm_session_id_t sessionid;
    bool per_session, found;
    int success = OPAL_SUCCESS;

    OPAL_OUTPUT_VERBOSE((5, orcm_scd_base_framework.framework_output,
                         "%s scd:base:receive processing msg from %s",
                         ORTE_NAME_PRINT(ORTE_PROC_MY_NAME),
                         ORTE_NAME_PRINT(sender)));

    /* always pass some answer back to the caller so they
     * don't hang
     */
    ans = OBJ_NEW(opal_buffer_t);

    /* unpack the command */
    cnt = 1;
    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &command,
                                              &cnt, ORCM_SCD_CMD_T))) {
        ORTE_ERROR_LOG(rc);
        goto answer;
    }

    if (ORCM_SESSION_REQ_COMMAND == command) {
        /* session request - this comes in the form of a
         * requested allocation to support the session
         */
        cnt = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &alloc,
                                                  &cnt, ORCM_ALLOC))) {
            ORTE_ERROR_LOG(rc);
            goto answer;
        }
        /* assign a session to it */
        session = OBJ_NEW(orcm_session_t);
        session->alloc = alloc;
        session->id = orcm_scd_base_get_next_session_id();
        alloc->id = session->id;

        /* send session id back to sender */
        if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &session->id,
                                                1, ORCM_ALLOC_ID_T))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }
        if (ORTE_SUCCESS != (rc = orte_rml.send_buffer_nb(sender, ans,
                                                          ORCM_RML_TAG_SCD,
                                                          orte_rml_send_callback,
                                                          NULL))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }

        /* pass it to the scheduler */
        ORCM_ACTIVATE_SCD_STATE(session, ORCM_SESSION_STATE_INIT);

        return;
    } else if (ORCM_SESSION_INFO_COMMAND == command) {
        /* pack the number of queues we have */
        cnt = opal_list_get_size(&orcm_scd_base.queues);
        if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &cnt, 1, OPAL_INT))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }

        /* for each queue, */
        OPAL_LIST_FOREACH(q, &orcm_scd_base.queues, orcm_queue_t) {
            /* pack the name */
            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &q->name,
                                                    1, OPAL_STRING))) {
                ORTE_ERROR_LOG(rc);
                OBJ_RELEASE(ans);
                return;
            }

            /* pack the count of sessions on the queue */
            cnt = (int)opal_list_get_size(&q->sessions);
            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &cnt, 1, OPAL_INT))) {
                ORTE_ERROR_LOG(rc);
                OBJ_RELEASE(ans);
                return;
            }
            if (0 < cnt) {
                /* pack all the sessions on the queue */
                allocs = (orcm_alloc_t**)malloc(cnt * sizeof(orcm_alloc_t*));
                if (!allocs) {
                    ORTE_ERROR_LOG(ORTE_ERR_OUT_OF_RESOURCE);
                    return;
                }
                i = 0;
                OPAL_LIST_FOREACH(session, &q->sessions, orcm_session_t) {
                    allocs[i] = session->alloc;
                    i++;
                }
                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, allocs,
                                                        i, ORCM_ALLOC))) {
                    ORTE_ERROR_LOG(rc);
                    OBJ_RELEASE(ans);
                    return;
                }
                free(allocs);
            }
        }
        /* send back results */
        if (ORTE_SUCCESS !=
            (rc = orte_rml.send_buffer_nb(sender, ans,
                                          ORCM_RML_TAG_SCD,
                                          orte_rml_send_callback, NULL))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }

        return;
    } else if (ORCM_SESSION_CANCEL_COMMAND == command) {
        /* session cancel - this comes in the form of a
         * session id to be cancelled
         */
        cnt = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &sessionid,
                                                  &cnt, ORCM_ALLOC_ID_T))) {
            ORTE_ERROR_LOG(rc);
            goto answer;
        }

        session = OBJ_NEW(orcm_session_t);
        session->id = sessionid;
        /* pass it to the scheduler */
        ORCM_ACTIVATE_SCD_STATE(session, ORCM_SESSION_STATE_CANCEL);

        /* send confirmation back to sender */
        result = 0;
        if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }
        if (ORTE_SUCCESS !=
            (rc = orte_rml.send_buffer_nb(sender, ans,
                                          ORCM_RML_TAG_SCD,
                                          orte_rml_send_callback, NULL))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }
        return;
    } else if (ORCM_NODE_INFO_COMMAND == command) {
        /* pack the number of nodes we have */
        cnt = orcm_scd_base.nodes.lowest_free;
        if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &cnt, 1, OPAL_INT))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }

        if (0 < cnt) {
            /* pack all the nodes */
            nodes = (orcm_node_t**)malloc(cnt * sizeof(orcm_node_t*));
            if (!nodes) {
                ORTE_ERROR_LOG(ORTE_ERR_OUT_OF_RESOURCE);
                return;
            }
            i = 0;
            for (j = 0; j < orcm_scd_base.nodes.lowest_free; j++) {
                if (NULL == (nodes[i] =
                             (orcm_node_t*)opal_pointer_array_get_item(&orcm_scd_base.nodes,
                                                                       j))) {
                    continue;
                }
                OPAL_OUTPUT_VERBOSE((5, orcm_scd_base_framework.framework_output,
                                     "%s scd:base:receive PACKING NODE: %s (%s)",
                                     ORTE_NAME_PRINT(ORTE_PROC_MY_NAME),
                                     nodes[i]->name,
                                     ORTE_NAME_PRINT(&nodes[i]->daemon)));
                i++;
            }

            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, nodes, i, ORCM_NODE))) {
                ORTE_ERROR_LOG(rc);
                OBJ_RELEASE(ans);
                return;
            }
            free(nodes);
        }

        /* send back results */
        if (ORTE_SUCCESS != (rc = orte_rml.send_buffer_nb(sender, ans,
                                                          ORCM_RML_TAG_SCD,
                                                          orte_rml_send_callback,
                                                          NULL))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }

        return;
    } else if (ORCM_SET_POWER_COMMAND == command) {
        cnt = 1;

        /* unpack the subcommand */
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &sub_command,
                                                  &cnt, ORCM_SCD_CMD_T))) {
            ORTE_ERROR_LOG(rc);
            goto answer;
        }

        /* unpack the bool (tells us if this is global or per_session) */
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &per_session,
                                                  &cnt, OPAL_BOOL))) {
            ORTE_ERROR_LOG(rc);
            goto answer;
        }

        if (true == per_session) {
            //per session
            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &sessionid,
                                                      &cnt, ORCM_ALLOC_ID_T))) {
                ORTE_ERROR_LOG(rc);
                goto answer;
            }

            //let's find the session
            found = false;
            /* for each queue, */
            OPAL_LIST_FOREACH(q, &orcm_scd_base.queues, orcm_queue_t) {
                OPAL_LIST_FOREACH(session, &q->sessions, orcm_session_t) {
                    alloc = session->alloc;
                    if(alloc->id == sessionid) { //found the session
                        found = true;
                        switch(sub_command) {
                        case ORCM_SET_POWER_BUDGET_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &int_param,
                                                                      &cnt, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_POWER_BUDGET_KEY,
                                                        ORTE_ATTR_GLOBAL, &int_param, OPAL_INT32);
                        break;
                        case ORCM_SET_POWER_MODE_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &int_param,
                                                                      &cnt, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_POWER_MODE_KEY,
                                                        ORTE_ATTR_GLOBAL, &int_param, OPAL_INT32);
                        break;
                        case ORCM_SET_POWER_WINDOW_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &int_param,
                                                                      &cnt, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_POWER_WINDOW_KEY,
                                                        ORTE_ATTR_GLOBAL, &int_param, OPAL_INT32);
                        break;
                        case ORCM_SET_POWER_OVERAGE_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &int_param,
                                                                      &cnt, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_OVERAGE_LIMIT_KEY,
                                                        ORTE_ATTR_GLOBAL, &int_param, OPAL_INT32);
                        break;
                        case ORCM_SET_POWER_UNDERAGE_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &int_param,
                                                                      &cnt, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_UNDERAGE_LIMIT_KEY,
                                                        ORTE_ATTR_GLOBAL, &int_param, OPAL_INT32);
                        break;
                        case ORCM_SET_POWER_OVERAGE_TIME_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &int_param,
                                                                      &cnt, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_OVERAGE_TIME_LIMIT_KEY,
                                                        ORTE_ATTR_GLOBAL, &int_param, OPAL_INT32);
                        break;
                        case ORCM_SET_POWER_UNDERAGE_TIME_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &int_param,
                                                                      &cnt, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_UNDERAGE_TIME_LIMIT_KEY,
                                                        ORTE_ATTR_GLOBAL, &int_param, OPAL_INT32);
                        break;
                        case ORCM_SET_POWER_FREQUENCY_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &float_param,
                                                                      &cnt, OPAL_FLOAT))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_MANUAL_FREQUENCY_KEY,
                                                        ORTE_ATTR_GLOBAL, &float_param, OPAL_FLOAT);
                        break;
                        case ORCM_SET_POWER_STRICT_COMMAND:
                            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &bool_param,
                                                                      &cnt, OPAL_BOOL))) {
                                ORTE_ERROR_LOG(rc);
                                goto answer;
                            }
                            result = orte_set_attribute(&alloc->constraints, ORCM_PWRMGMT_FREQ_STRICT_KEY,
                                                        ORTE_ATTR_GLOBAL, &bool_param, OPAL_BOOL);
                        break;
                        default:
                            result = ORTE_ERR_BAD_PARAM;
                        }
                        if(!strncmp(q->name, "running", 8)) {
                            //session is currently running, send request to the RM
                            rmbuf = OBJ_NEW(opal_buffer_t);
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(rmbuf, &command,
                                            1, ORCM_RM_CMD_T))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(rmbuf);
                                result = rc;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(rmbuf, &alloc,
                                                        1, ORCM_ALLOC))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(rmbuf);
                                result = rc;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (ORTE_SUCCESS != (rc = orte_rml.send_buffer_nb(ORTE_PROC_MY_SCHEDULER,
                                                      rmbuf,
                                                      ORCM_RML_TAG_RM,
                                                      orte_rml_send_callback,
                                                      NULL))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(rmbuf);
                                result = rc;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                        }
                    }
                    if (true == found) {
                        break;
                    }
                }
                if (true == found) {
                    break;
                }
            }
        }

        /* send confirmation back to sender */
        if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }
        if (ORTE_SUCCESS !=
            (rc = orte_rml.send_buffer_nb(sender, ans,
                                          ORCM_RML_TAG_SCD,
                                          orte_rml_send_callback, NULL))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }
        return;
    } else if (ORCM_GET_POWER_COMMAND == command) {
        /* unpack the subcommand */
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &sub_command,
                                                  &cnt, ORCM_SCD_CMD_T))) {
            ORTE_ERROR_LOG(rc);
            goto answer;
        }

        /* unpack the bool (tells us if this is global or per_session) */
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &per_session,
                                                  &cnt, OPAL_BOOL))) {
            ORTE_ERROR_LOG(rc);
            goto answer;
        }

        if (true == per_session) {
            //per session
            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &sessionid,
                                                      &cnt, ORCM_ALLOC_ID_T))) {
                ORTE_ERROR_LOG(rc);
                goto answer;
            }

            //let's find the session
            found = false;
            /* for each queue, */
            OPAL_LIST_FOREACH(q, &orcm_scd_base.queues, orcm_queue_t) {
                OPAL_LIST_FOREACH(session, &q->sessions, orcm_session_t) {
                    alloc = session->alloc;
                    if(alloc->id == sessionid) { //found the session
                        found = true;
                        switch(sub_command) {
                        case ORCM_GET_POWER_BUDGET_COMMAND:
                            int_param_ptr = &int_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_POWER_BUDGET_KEY,
                                                            (void**)&int_param_ptr, OPAL_INT32)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &int_param, 1, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_MODE_COMMAND:
                            int_param_ptr = &int_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_POWER_MODE_KEY,
                                                            (void**)&int_param_ptr, OPAL_INT32)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &int_param, 1, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_WINDOW_COMMAND:
                            int_param_ptr = &int_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_POWER_WINDOW_KEY,
                                                            (void**)&int_param_ptr, OPAL_INT32)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (result = opal_dss.pack(ans, &rc, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &int_param, 1, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_OVERAGE_COMMAND:
                            int_param_ptr = &int_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_OVERAGE_LIMIT_KEY,
                                                            (void**)&int_param_ptr, OPAL_INT32)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &int_param, 1, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_UNDERAGE_COMMAND:
                            int_param_ptr = &int_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_UNDERAGE_LIMIT_KEY,
                                                            (void**)&int_param_ptr, OPAL_INT32)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &int_param, 1, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_OVERAGE_TIME_COMMAND:
                            int_param_ptr = &int_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_OVERAGE_TIME_LIMIT_KEY,
                                                            (void**)&int_param_ptr, OPAL_INT32)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &int_param, 1, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_UNDERAGE_TIME_COMMAND:
                            int_param_ptr = &int_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_CAP_UNDERAGE_TIME_LIMIT_KEY,
                                                            (void**)&int_param_ptr, OPAL_INT32)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &int_param, 1, OPAL_INT32))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_FREQUENCY_COMMAND:
                            float_param_ptr = &float_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_MANUAL_FREQUENCY_KEY,
                                                            (void**)&float_param_ptr, OPAL_FLOAT)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &float_param, 1, OPAL_FLOAT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                        case ORCM_GET_POWER_STRICT_COMMAND:
                            bool_param_ptr = &bool_param;
                            if (false == orte_get_attribute(&alloc->constraints, ORCM_PWRMGMT_FREQ_STRICT_KEY,
                                                            (void**)&bool_param_ptr, OPAL_BOOL)) {
                                result = ORTE_ERR_BAD_PARAM;
                                if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &result, 1, OPAL_INT))) {
                                    ORTE_ERROR_LOG(rc);
                                    OBJ_RELEASE(ans);
                                    return;
                                }
                                goto answer;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &success, 1, OPAL_INT))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                            if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &bool_param, 1, OPAL_BOOL))) {
                                ORTE_ERROR_LOG(rc);
                                OBJ_RELEASE(ans);
                                return;
                            }
                        break;
                       default:
                           rc = ORTE_ERR_BAD_PARAM;
                           if (OPAL_SUCCESS != (rc = opal_dss.pack(ans, &rc, 1, OPAL_INT))) {
                               ORTE_ERROR_LOG(rc);
                               OBJ_RELEASE(ans);
                               return;
                           }
                       }
                    }
                    if (true == found) {
                        break;
                    }
                }
                if (true == found) {
                    break;
                }
            }
        }

        if (ORTE_SUCCESS !=
            (rc = orte_rml.send_buffer_nb(sender, ans,
                                          ORCM_RML_TAG_SCD,
                                          orte_rml_send_callback, NULL))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }
        return;
    } else if (ORCM_DISPATCH_LAUNCH_EXEC_COMMAND == command) {
        if (ORTE_SUCCESS != (rc = orte_rml.send_buffer_nb(sender, ans,
                                                          ORCM_RML_TAG_SCD,
                                                          orte_rml_send_callback,
                                                          NULL))) {
            ORTE_ERROR_LOG(rc);
            OBJ_RELEASE(ans);
            return;
        }
        if (ORCM_SUCCESS != (rc = orcm_scd_base_launch_exec(buffer))) {
            ORTE_ERROR_LOG(rc);
        }
        return;
    }

 answer:
    if (ORTE_SUCCESS != (rc = orte_rml.send_buffer_nb(sender, ans,
                                                      ORCM_RML_TAG_SCD,
                                                      orte_rml_send_callback,
                                                      NULL))) {
        ORTE_ERROR_LOG(rc);
        OBJ_RELEASE(ans);
        return;
    }
}

typedef struct {
    int dbhandle;
    int session_handle;
    int number_of_rows;
    int status;
    bool active;
} fetch_cb_data, *fetch_cb_data_ptr;

void open_callback(int dbhandle, int status, opal_list_t *in, opal_list_t *out, void *cbdata)
{
    fetch_cb_data* data = (fetch_cb_data*)cbdata;
    data->dbhandle = dbhandle;
    data->status = status;
    data->session_handle = -1;
    data->number_of_rows = -1;
    data->active = false;
}

void close_callback(int dbhandle, int status, opal_list_t *in, opal_list_t *out, void *cbdata)
{
    fetch_cb_data* data = (fetch_cb_data*)cbdata;
    data->status = status;
    data->active = false;
}

void fetch_callback(int dbhandle, int status, opal_list_t *in, opal_list_t *out, void *cbdata)
{
    fetch_cb_data* data = (fetch_cb_data*)cbdata;
    if(ORCM_SUCCESS == status && dbhandle == data->dbhandle) {
        if(NULL != out && 0 != opal_list_get_size(out)) {
            opal_value_t *handle_object = (opal_value_t*)opal_list_get_first(out);
            if(NULL != handle_object && OPAL_INT == handle_object->type) {
                data->session_handle = handle_object->data.integer;
            } else {
                status = ORCM_ERROR;
            }
        } else {
            status = ORCM_ERROR;
        }
    }
    if(NULL != in) {
        OBJ_RELEASE(in);
    }
    if(NULL != out) {
        OBJ_RELEASE(out);
    }
    data->status = status;
    data->active = false;
}

char* get_plugin_from_sensor_name(const char* sensor_name)
{
    char* pos1 = strchr(sensor_name, '_');
    if(NULL != pos1) {
        char* pos2 = strchr(++pos1, '_');
        if(NULL != pos2) {
            size_t length = (size_t)(--pos2) - (size_t)pos1 + 2;
            char* plugin = (char*)malloc(length);
            if(NULL != plugin) {
                strncpy(plugin, pos1, length - 1);
                plugin[length - 1] = '\0';
            }
            return plugin;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

int build_filter_list(opal_buffer_t* buffer,opal_list_t **filter_list)
{
    int n = 1;
    uint8_t operation = 0;
    int rc = ORCM_SUCCESS;
    char* tmp_str = NULL;
    char* tmp_key = NULL;
    orcm_db_filter_t *tmp_filter = NULL;
    uint16_t filters_list_count = 0;

    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &filters_list_count, &n, OPAL_UINT16))) {
        ORTE_ERROR_LOG(rc);
        return rc;
    }
    OPAL_OUTPUT_VERBOSE((4, orcm_scd_base_framework.framework_output, "Filters list count in buffer: %d", filters_list_count));
    for(uint16_t i = 0; i < filters_list_count; ++i) {
        n = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &tmp_key, &n, OPAL_STRING))) {
            opal_output(0, "Retrieved key from unpack: %s", tmp_key);
            ORTE_ERROR_LOG(rc);
            return ORCM_ERROR;
        }
        OPAL_OUTPUT_VERBOSE((4, orcm_scd_base_framework.framework_output,  "Retrieved key: %s", tmp_key));
        n = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &operation, &n, OPAL_UINT8))) {
            opal_output(0, "Retrieved operation from unpack: %s", tmp_key);
            ORTE_ERROR_LOG(rc);
            return ORCM_ERROR;
        }
        OPAL_OUTPUT_VERBOSE((4, orcm_scd_base_framework.framework_output, "Retrieved operation from unpack: %d", operation));
        n = 1;
        if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &tmp_str, &n, OPAL_STRING))) {
            opal_output(0, "Retrieved string from unpack: %s", tmp_str);
            ORTE_ERROR_LOG(rc);
            return ORCM_ERROR;
        }
        OPAL_OUTPUT_VERBOSE((4, orcm_scd_base_framework.framework_output, "Retrieved string from unpack: %s", tmp_str));
        if (NULL == *filter_list) {
            *filter_list = OBJ_NEW(opal_list_t);
        }
        tmp_filter = OBJ_NEW(orcm_db_filter_t);
        tmp_filter->value.type = OPAL_STRING;
        tmp_filter->value.key = tmp_key;
        tmp_filter->value.data.string = tmp_str;
        tmp_filter->op = (orcm_db_comparison_op_t)operation;
        opal_list_append(*filter_list, &tmp_filter->value.super);
    }
    return rc;
}

#define SAFE_FREE(x) if(NULL!=x) { free(x); x = NULL; }
#define SAFE_OBJ_RELEASE(x) if(NULL!=x) { OBJ_RELEASE(x); x = NULL; }

int get_inventory_list(opal_list_t *filters, opal_list_t **results)
{
    int rv = ORCM_SUCCESS;
    int num_rows = 0;
    opal_list_t *fetch_output = OBJ_NEW(opal_list_t);
    fetch_cb_data data;
    opal_list_t *row = NULL;
    opal_value_t *string_row = NULL;
    opal_value_t *string_row_header = OBJ_NEW(opal_value_t);
    char* tmp = NULL;
    char* col1_data = NULL;
    char* col2_data = NULL;
    char* col3_data = NULL;

    data.dbhandle = -1;

    data.active = true;
    orcm_db.open(NULL, NULL, open_callback, &data);
    ORTE_WAIT_FOR_COMPLETION(data.active);

    if(ORCM_SUCCESS != data.status) {
        opal_output(0, "Failed to open database to retrieve inventory");
        if(NULL != filters) {
            SAFE_OBJ_RELEASE(filters);
        }
        rv = data.status;
        goto clean_exit;
    }

    data.active = true;
    orcm_db.fetch(data.dbhandle, "node_features_view", filters, fetch_output, fetch_callback, &data);
    ORTE_WAIT_FOR_COMPLETION(data.active);

    if(ORCM_SUCCESS != data.status || -1 == data.session_handle) {
        opal_output(0, "Failed to fetch the inventory database");
        rv = data.status;
        goto clean_exit;
    }

    data.status = orcm_db.get_num_rows(data.dbhandle, data.session_handle, &num_rows);
    if(ORCM_SUCCESS != data.status) {
        opal_output(0, "Failed to get number of inventory rows in the inventory database");
        rv = data.status;
        goto clean_exit;
    }

    if(0 < num_rows) {
        *results = OBJ_NEW(opal_list_t);
        string_row_header->type = OPAL_STRING;
        string_row_header->data.string = strdup("\"Node Name\",\"Source Plugin Name\",\"Sensor Name\"");
        opal_list_append(*results, &string_row_header->super);
        for(int i = 0; i < num_rows; ++i) {
            row = OBJ_NEW(opal_list_t);
            string_row = OBJ_NEW(opal_value_t);
            opal_value_t *item = NULL;

            data.status = orcm_db.get_next_row(data.dbhandle, data.session_handle, row);

            if(ORCM_SUCCESS != data.status) {
                opal_output(0, "Failed to get inventory row %d in the inventory database", i);
                rv = data.status;
                goto error_exit;
            }
            OPAL_LIST_FOREACH(item, row, opal_value_t){
                if(0 == strcmp(item->key, "hostname")) {
                    col1_data = item->data.string;
                } else if(0 == strcmp(item->key, "feature")) {
                    SAFE_FREE(col2_data);
                    col2_data = get_plugin_from_sensor_name(item->data.string);
                } else if (0 == strcmp(item->key, "value")) {
                    col3_data = item->data.string;
                }
            }
            string_row->type = OPAL_STRING;
            asprintf(&tmp,"\"%s\",\"%s\",\"%s\"", col1_data, col2_data, col3_data);
            string_row->data.string = strdup(tmp);
            opal_list_append(*results, &string_row->super);
            SAFE_FREE(tmp);
            SAFE_FREE(col2_data);
            SAFE_OBJ_RELEASE(row);
        }
    }
    goto clean_exit;

error_exit:
    SAFE_FREE(tmp);
    SAFE_OBJ_RELEASE(row);
    SAFE_OBJ_RELEASE(string_row_header);
    SAFE_OBJ_RELEASE(*results);

clean_exit:
    data.status = orcm_db.close_result_set(data.dbhandle, data.session_handle);

    if(ORCM_SUCCESS != data.status) {
        opal_output(0, "Failed to close the inventory database results handle");
        SAFE_OBJ_RELEASE(*results);
    }
    data.active = true;
    orcm_db.close(data.dbhandle, close_callback, &data);
    ORTE_WAIT_FOR_COMPLETION(data.active);
    if(ORCM_SUCCESS != data.status) {
        opal_output(0, "Failed to close the inventory database handle");
    }

    return rv;
}
#undef SAFE_OBJ_RELEASE
#undef SAFE_FREE

void orcm_scd_base_fetch_recv(int status, orte_process_name_t* sender,
                              opal_buffer_t* buffer, orte_rml_tag_t tag,
                              void* cbdata)
{
    int rc = ORCM_SUCCESS;
    int n = 1;
    orcm_rm_cmd_flag_t command = 0;
    opal_list_t *filter_list = NULL;
    uint16_t filters_list_count = 0;
    char* tmp_str = NULL;
    char* tmp_key = NULL;
    opal_value_t *tmp_value = NULL;
    uint8_t operation = 0;
    orcm_db_filter_t *tmp_filter = NULL;
    opal_list_t *results_list = NULL;
    uint16_t results_count;
    opal_buffer_t *response_buffer = NULL;
    int returned_status = 0;

    if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &command, &n, ORCM_RM_CMD_T))) {
        ORTE_ERROR_LOG (rc);
        return;
    }
    switch(command) {
        case ORCM_GET_DB_SENSOR_INVENTORY_COMMAND:
            /* Build filter list */
            n = 1;
            if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &filters_list_count, &n, OPAL_UINT16))) {
                ORTE_ERROR_LOG(rc);
                return;
            }
            for(uint16_t i = 0; i < filters_list_count; ++i) {
                n = 1;
                if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &tmp_key, &n, OPAL_STRING))) {
                    ORTE_ERROR_LOG(rc);
                    if(NULL != filter_list) {
                        OBJ_RELEASE(filter_list);
                    }
                    return;
                }
                n = 1;
                if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &operation, &n, OPAL_UINT8))) {
                    ORTE_ERROR_LOG(rc);
                    if(NULL != filter_list) {
                        OBJ_RELEASE(filter_list);
                    }
                    return;
                }
                n = 1;
                if (OPAL_SUCCESS != (rc = opal_dss.unpack(buffer, &tmp_str, &n, OPAL_STRING))) {
                    ORTE_ERROR_LOG(rc);
                    if(NULL != filter_list) {
                        OBJ_RELEASE(filter_list);
                    }
                    return;
                }
                if(NULL == filter_list) {
                    filter_list = OBJ_NEW(opal_list_t);
                }
                tmp_filter = OBJ_NEW(orcm_db_filter_t);
                tmp_filter->value.type = OPAL_STRING;
                tmp_filter->value.key = tmp_key;
                tmp_filter->value.data.string = tmp_str;
                tmp_filter->op = (orcm_db_comparison_op_t)operation;
                opal_list_append(filter_list, &tmp_filter->value.super);
            }
            returned_status = get_inventory_list(filter_list, &results_list);
            if (NULL != filter_list) {
                OBJ_RELEASE(filter_list);
            }
            response_buffer = OBJ_NEW(opal_buffer_t);
            if (OPAL_SUCCESS != (rc = opal_dss.pack(response_buffer, &returned_status, 1, OPAL_INT))) {
                ORTE_ERROR_LOG(rc);
                goto send_buffer;
            }
            opal_output(0, "rc: %d returned_status: %d results_list %p", rc, returned_status, (void *)results_list);
            if(ORCM_SUCCESS == rc && 0 == returned_status && NULL != results_list) {
                results_count = (uint16_t)opal_list_get_size(results_list);
                if (OPAL_SUCCESS != (rc = opal_dss.pack(response_buffer, &results_count, 1, OPAL_UINT16))) {
                    ORTE_ERROR_LOG(rc);
                    goto send_buffer;
                }
                OPAL_LIST_FOREACH(tmp_value, results_list, opal_value_t) {
                    if (OPAL_SUCCESS != (rc = opal_dss.pack(response_buffer, &tmp_value->data.string, 1, OPAL_STRING))) {
                        ORTE_ERROR_LOG(rc);
                        goto send_buffer;
                    }
                }
            }
send_buffer:
            if (ORTE_SUCCESS != (rc = orte_rml.send_buffer_nb(sender, response_buffer,
                                                              ORCM_RML_TAG_ORCMD_FETCH,
                                                              orte_rml_send_callback, cbdata))) {
                ORTE_ERROR_LOG(rc);
                OBJ_RELEASE(response_buffer);
                return;
            }
            if(NULL != results_list) {
                OBJ_RELEASE(results_list);
            }
            break;
        default:
            opal_output(0, "%s: COMMAND UNKNOWN", ORTE_NAME_PRINT(ORTE_PROC_MY_NAME));
    }
}
