/*
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "db_odbc_test.h"

#include "gtest/gtest.h"
#include "orcm/test/mca/analytics/util/analytics_util.h"

const char *hostname = "RandomName";
const char *data_group = "coretemp";
struct timeval start_time;
struct timeval end_time;
double d_value = 10;

const char *type = "type 1";
const char *subtype = "type 2";
const char *test_result = "SUCCESS";
int component_index = 0;

const char *severity = "critical";
const char *version = "version 1";
const char *vendor = "vendor 1";
const char *description = "this is a test";

static mca_db_odbc_module_t* create_odbc_mod(const char* odbcdsn,
                                             const char *table, const char *user)
{
    mca_db_odbc_module_t *mod = (mca_db_odbc_module_t*)malloc(sizeof(mca_db_odbc_module_t));
    mod->autocommit = true;
    mod->odbcdsn = (NULL != odbcdsn) ? strdup(odbcdsn) : NULL;
    mod->table = (NULL != table) ? strdup(table) : NULL;
    mod->user = (NULL != user) ? strdup(user) : NULL;
    mod->results_sets = OBJ_NEW(opal_pointer_array_t);
    mod->dbhandle = NULL;
    mod->envhandle = NULL;
    return mod;
}

static opal_list_t *create_list_with_mandatory_fields(opal_data_type_t hostname_type, void *hostname,
                                                      opal_data_type_t data_group_type, void *data_group,
                                                      opal_data_type_t ctime_type, void *ctime)
{
    opal_list_t *input_list = OBJ_NEW(opal_list_t);
    orcm_value_t *mv = NULL;

    if (NULL != hostname) {
        mv = analytics_util::load_orcm_value("hostname", hostname, hostname_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != data_group) {
        mv = analytics_util::load_orcm_value("data_group", data_group, data_group_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != ctime) {
        mv = analytics_util::load_orcm_value("ctime", ctime, ctime_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }

    return input_list;
}

static opal_list_t*
create_list_with_mandatory_fields_diag(opal_data_type_t hostname_type, void *hostname,
                                       opal_data_type_t diag_type_type, void *diag_type,
                                       opal_data_type_t diag_subtype_type, void *diag_subtype,
                                       opal_data_type_t start_time_type, void *start_time,
                                       opal_data_type_t test_result_type, void *test_result,
                                       opal_data_type_t end_time_type, void *end_time,
                                       opal_data_type_t component_index_type, void *component_index)
{
    opal_list_t *input_list = OBJ_NEW(opal_list_t);
    orcm_value_t *mv = NULL;

    if (NULL != hostname) {
        mv = analytics_util::load_orcm_value("hostname", hostname, hostname_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != diag_type) {
        mv = analytics_util::load_orcm_value("diag_type", diag_type, diag_type_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != diag_subtype) {
        mv = analytics_util::load_orcm_value("diag_subtype", diag_subtype, diag_subtype_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != start_time) {
        mv = analytics_util::load_orcm_value("start_time", start_time, start_time_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != test_result) {
        mv = analytics_util::load_orcm_value("test_result", test_result, test_result_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != end_time) {
        mv = analytics_util::load_orcm_value("end_time", end_time, end_time_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != component_index) {
        mv = analytics_util::load_orcm_value("component_index",
                                             component_index, component_index_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    return input_list;
}

static opal_list_t*
create_list_with_mandatory_fields_event(opal_data_type_t ctime_type, void *ctime,
                                        opal_data_type_t severity_type, void *severity,
                                        opal_data_type_t type_type, void *type,
                                        opal_data_type_t version_type, void *version,
                                        opal_data_type_t vendor_type, void *vendor,
                                        opal_data_type_t description_type, void *description)
{
    opal_list_t *input_list = OBJ_NEW(opal_list_t);
    orcm_value_t *mv = NULL;

    if (NULL != ctime) {
        mv = analytics_util::load_orcm_value("ctime", ctime, ctime_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != severity) {
        mv = analytics_util::load_orcm_value("severity", severity, severity_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != type) {
        mv = analytics_util::load_orcm_value("type", type, type_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != version) {
        mv = analytics_util::load_orcm_value("version", version, version_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != vendor) {
        mv = analytics_util::load_orcm_value("vendor", vendor, vendor_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }
    if (NULL != description) {
        mv = analytics_util::load_orcm_value("description", description, description_type, "");
        if (NULL != mv) {
            opal_list_append(input_list, (opal_list_item_t*)mv);
        }
    }

    return input_list;
}

TEST(db_odbc, init_fail_connection)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    rc = mca_db_odbc_module.api.init((orcm_db_base_module_t*)mod);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    ASSERT_EQ(ORCM_ERR_CONNECTION_FAILED, rc);
}

TEST(db_odbc, finalize_null_table)
{
    mca_db_odbc_module_t *mod = create_odbc_mod(NULL, NULL, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
}

TEST(db_odbc, store_unsupported_type)
{
    orcm_db_data_type_t unknown_type = (orcm_db_data_type_t)4;
    int rc = mca_db_odbc_module.api.store_new(NULL, unknown_type, NULL, NULL);
    ASSERT_EQ(ORCM_ERR_NOT_IMPLEMENTED, rc);
}

TEST(db_odbc, store_data_sample_invalid_data_group_type)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&start_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_DOUBLE, (void*)(&d_value), OPAL_TIMEVAL, (void*)(&start_time));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_ENV_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_data_sample_invalid_opal_time)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)data_group, OPAL_TIME, NULL);
    orcm_value_t *mv = OBJ_NEW(orcm_value_t);
    mv->value.key = strdup(analytics_util::cppstr_to_cstr("ctime"));
    mv->value.type = OPAL_TIME;
    mv->value.data.tv.tv_sec = LONG_MAX;
    opal_list_append(input_list, (opal_list_item_t*)mv);
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_ENV_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_data_sample_string_time_invalid_hostname_type)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mod->autocommit = false;
    const char *ctime_local = "2015-12-31 12:00:00";
    opal_list_t *input_list =  create_list_with_mandatory_fields(OPAL_DOUBLE, (void*)(&d_value),
                               OPAL_STRING, (void*)data_group, OPAL_STRING, (void*)ctime_local);
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_ENV_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_data_sample_invalid_time_type)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)data_group, OPAL_DOUBLE, (void*)(&d_value));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_ENV_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_data_sample_handle_alloc_fail)
{
    int rc = ORCM_SUCCESS;
    gettimeofday(&start_time, NULL);
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)data_group, OPAL_TIMEVAL, (void*)(&start_time));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_ENV_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, store_node_feature_no_time_stamp)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mod->autocommit = false;
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)data_group, OPAL_TIMEVAL, NULL);
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_INVENTORY_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_node_feature_invalid_hostname_type)
{
    int rc = ORCM_SUCCESS;
    gettimeofday(&start_time, NULL);
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_DOUBLE, (void*)(&d_value),
                              OPAL_STRING, NULL, OPAL_TIMEVAL, (void*)(&start_time));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_INVENTORY_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_node_feature_invalid_ctime_type)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, NULL, OPAL_DOUBLE, (void*)(&d_value));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_INVENTORY_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_node_feature_no_feature)
{
    int rc = ORCM_SUCCESS;
    gettimeofday(&start_time, NULL);
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, NULL, OPAL_TIMEVAL, (void*)(&start_time));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_INVENTORY_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_SUCCESS, rc);
}

TEST(db_odbc, store_node_feature_handle_alloc_fail)
{
    int rc = ORCM_SUCCESS;
    gettimeofday(&start_time, NULL);
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = create_list_with_mandatory_fields(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)data_group, OPAL_TIMEVAL, (void*)(&start_time));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_INVENTORY_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, store_diag_invalid_hostname_type)
{
    int rc = ORCM_SUCCESS;
    gettimeofday(&start_time, NULL);
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mod->autocommit = false;
    gettimeofday(&end_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_DOUBLE, (void*)(&d_value),
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_TIMEVAL, (void*)(&start_time), OPAL_STRING, (void*)test_result,
                              OPAL_TIMEVAL, (void*)(&end_time), OPAL_INT, (void*)(&component_index));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_invalid_type_type)
{
    int rc = ORCM_SUCCESS;
    gettimeofday(&start_time, NULL);
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&end_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_DOUBLE, (void*)(&d_value), OPAL_STRING, (void*)subtype,
                              OPAL_TIMEVAL, (void*)(&start_time), OPAL_STRING, (void*)test_result,
                              OPAL_TIMEVAL, (void*)(&end_time), OPAL_INT, (void*)(&component_index));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_invalid_subtype_type)
{
    int rc = ORCM_SUCCESS;
    gettimeofday(&start_time, NULL);
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&end_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_DOUBLE, (void*)(&d_value),
                              OPAL_TIMEVAL, (void*)(&start_time), OPAL_STRING, (void*)test_result,
                              OPAL_TIMEVAL, (void*)(&end_time), OPAL_INT, (void*)(&component_index));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_invalid_opal_time_start)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&end_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_TIMEVAL, NULL, OPAL_STRING, (void*)test_result,
                              OPAL_TIMEVAL, (void*)(&end_time), OPAL_INT, (void*)(&component_index));
    orcm_value_t *mv = OBJ_NEW(orcm_value_t);
    mv->value.key = strdup(analytics_util::cppstr_to_cstr("start_time"));
    mv->value.type = OPAL_TIME;
    mv->value.data.tv.tv_sec = LONG_MAX;
    opal_list_append(input_list, (opal_list_item_t*)mv);
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_string_start_time_invalid_test_result_type)
{
    int rc = ORCM_SUCCESS;
    const char *start_time_local = "2015-12-31 12:00:00";
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&end_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_STRING, (void*)start_time_local, OPAL_DOUBLE, (void*)(&d_value),
                              OPAL_TIMEVAL, (void*)(&end_time), OPAL_INT, (void*)(&component_index));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_invalid_start_time_type)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&end_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_DOUBLE, (void*)(&d_value), OPAL_STRING, (void*)test_result,
                              OPAL_TIMEVAL, (void*)(&end_time), OPAL_INT, (void*)(&component_index));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_invalid_opal_time_end)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&start_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_TIMEVAL, (void*)(&start_time), OPAL_STRING, (void*)test_result,
                              OPAL_TIMEVAL, NULL, OPAL_INT, (void*)(&component_index));
    orcm_value_t *mv = OBJ_NEW(orcm_value_t);
    mv->value.key = strdup(analytics_util::cppstr_to_cstr("end_time"));
    mv->value.type = OPAL_TIME;
    mv->value.data.tv.tv_sec = LONG_MAX;
    opal_list_append(input_list, (opal_list_item_t*)mv);
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_string_end_time_invalid_component_index_type)
{
    int rc = ORCM_SUCCESS;
    const char *end_time_local = "2015-12-31 12:00:00";
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&start_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_TIMEVAL, (void*)(&start_time), OPAL_STRING, (void*)test_result,
                              OPAL_STRING, (void*)(end_time_local), OPAL_DOUBLE, (void*)(&d_value));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_invalid_end_time_type)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&start_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_TIMEVAL, (void*)(&start_time), OPAL_STRING, (void*)test_result,
                              OPAL_DOUBLE, (void*)(&d_value), OPAL_INT, (void*)(&component_index));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_diag_handle_alloc_fail)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&start_time, NULL);
    gettimeofday(&end_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_diag(OPAL_STRING, (void*)hostname,
                              OPAL_STRING, (void*)type, OPAL_STRING, (void*)subtype,
                              OPAL_TIMEVAL, (void*)(&start_time), OPAL_STRING, (void*)test_result,
                              OPAL_TIMEVAL, (void*)(&end_time), OPAL_INT, (void*)(&component_index));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_DIAG_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, store_event_invalid_timeval)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mod->autocommit = false;
    start_time.tv_sec = LONG_MAX;
    start_time.tv_usec = 0;
    opal_list_t *input_list = create_list_with_mandatory_fields_event(OPAL_TIMEVAL, (void*)(&start_time),
                              OPAL_STRING, (void*)severity, OPAL_STRING, (void*)type,
                              OPAL_STRING, (void*)version, OPAL_STRING, (void*)vendor,
                              OPAL_STRING, (void*)description);
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_EVENT_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, store_event_multiple_branch)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&start_time, NULL);
    opal_list_t *input_list = create_list_with_mandatory_fields_event(OPAL_TIMEVAL, (void*)(&start_time),
                              OPAL_STRING, (void*)severity, OPAL_STRING, (void*)type,
                              OPAL_DOUBLE, (void*)(&d_value), OPAL_DOUBLE, (void*)(&d_value),
                              OPAL_DOUBLE, (void*)(&d_value));
    rc = mca_db_odbc_module.api.store_new((orcm_db_base_module_t*)mod,
                                           ORCM_DB_EVENT_DATA, input_list, NULL);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, record_data_samples_invalid_time)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    start_time.tv_sec = LONG_MAX;
    start_time.tv_usec = 0;
    opal_list_t *input_list = OBJ_NEW(opal_list_t);
    rc = mca_db_odbc_module.api.record_data_samples((orcm_db_base_module_t*)mod, hostname,
                                                    &start_time, data_group, input_list);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERR_BAD_PARAM, rc);
}

TEST(db_odbc, record_data_samples_handle_alloc_fail)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    gettimeofday(&start_time, NULL);
    opal_list_t *input_list = OBJ_NEW(opal_list_t);
    rc = mca_db_odbc_module.api.record_data_samples((orcm_db_base_module_t*)mod, hostname,
                                                    &start_time, data_group, input_list);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, update_node_features_handle_alloc_fail)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = OBJ_NEW(opal_list_t);
    rc = mca_db_odbc_module.api.update_node_features((orcm_db_base_module_t*)mod,
                                                     hostname, input_list);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, record_diag_handle_alloc_fail)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    opal_list_t *input_list = OBJ_NEW(opal_list_t);
    gettimeofday(&start_time, NULL);
    gettimeofday(&end_time, NULL);
    rc = mca_db_odbc_module.api.record_diag_test((orcm_db_base_module_t*)mod, hostname, type,
                                                 subtype, localtime(&(start_time.tv_sec)),
                                                 localtime(&(end_time.tv_sec)), &component_index,
                                                 test_result, input_list);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    OBJ_RELEASE(input_list);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, commit_fail)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mca_db_odbc_module.api.init((orcm_db_base_module_t*)mod);
    rc = mca_db_odbc_module.api.commit((orcm_db_base_module_t*)mod);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, rollback_fail)
{
    int rc = ORCM_SUCCESS;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mca_db_odbc_module.api.init((orcm_db_base_module_t*)mod);
    rc = mca_db_odbc_module.api.rollback((orcm_db_base_module_t*)mod);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, get_num_row_empty_results)
{
    int rc = ORCM_SUCCESS;
    int num_rows = 0;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mca_db_odbc_module.api.init((orcm_db_base_module_t*)mod);
    rc = mca_db_odbc_module.api.get_num_rows((orcm_db_base_module_t*)mod, 0, &num_rows);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    ASSERT_EQ(ORCM_ERROR, rc);
}

TEST(db_odbc, get_num_row_invalid_results)
{
    int rc = ORCM_SUCCESS;
    int num_rows = 0;
    mca_db_odbc_module_t *mod = create_odbc_mod("odbc", "random_table", "randomuser:randompw");
    mca_db_odbc_module.api.init((orcm_db_base_module_t*)mod);
    const char *result = "result";
    opal_pointer_array_add(mod->results_sets, (void*)result);
    rc = mca_db_odbc_module.api.get_num_rows((orcm_db_base_module_t*)mod, 0, &num_rows);
    mca_db_odbc_module.api.finalize((orcm_db_base_module_t*)mod);
    ASSERT_EQ(ORCM_ERROR, rc);
}