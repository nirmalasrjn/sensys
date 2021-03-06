# -*- shell-script -*-
#
# Copyright (c) 2013-2016 Intel Corporation. All rights reserved.
# Copyright (c) 2016      Intel Corporation. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

AC_DEFUN([GREI_CONFIG_FILES],[
    AC_CONFIG_FILES([
    orcm/test/Makefile
    orcm/test/gtest_example/Makefile
    orcm/test/mca/Makefile
    orcm/test/mca/dispatch/Makefile
    orcm/test/mca/dispatch/dfg/Makefile
    orcm/test/mca/dispatch/base/Makefile
    orcm/test/mca/sensor/Makefile
    orcm/test/mca/sensor/ipmi/Makefile
    orcm/test/mca/sensor/ipmi_ts/Makefile
    orcm/test/mca/sensor/base/Makefile
    orcm/test/mca/sensor/coretemp/Makefile
    orcm/test/mca/sensor/freq/Makefile
    orcm/test/mca/sensor/sigar/Makefile
    orcm/test/mca/sensor/componentpower/Makefile
    orcm/test/mca/sensor/mcedata/Makefile
    orcm/test/mca/sensor/nodepower/Makefile
    orcm/test/mca/sensor/resusage/Makefile
    orcm/test/mca/sensor/syslog/Makefile
    orcm/test/mca/sensor/snmp/Makefile
    orcm/test/mca/sensor/file/Makefile
    orcm/test/mca/sensor/errcounts/Makefile
    orcm/test/mca/sensor/heartbeat/Makefile
    orcm/test/mca/sensor/udsensors/Makefile
    orcm/test/mca/analytics/Makefile
    orcm/test/mca/analytics/base/Makefile
    orcm/test/mca/analytics/util/Makefile
    orcm/test/mca/analytics/window/Makefile
    orcm/test/mca/analytics/aggregate/Makefile
    orcm/test/mca/analytics/genex/Makefile
    orcm/test/mca/analytics/cott/Makefile
    orcm/test/mca/analytics/factory/Makefile
    orcm/test/mca/analytics/extension/Makefile
    orcm/test/mca/analytics/average/Makefile
    orcm/test/mca/parser/Makefile
    orcm/test/mca/parser/pugi/Makefile
    orcm/test/mca/parser/base/Makefile
    orcm/test/mca/sst/Makefile
    orcm/test/mca/sst/tool/Makefile
    orcm/test/mca/sst/orcmd/Makefile
    orcm/test/tools/Makefile
    orcm/test/tools/octl/Makefile
    orcm/test/tools/orcmd/Makefile
    orcm/test/util/Makefile
    orcm/test/runtime/Makefile
    orcm/test/runtime/led_control/Makefile
    orcm/test/runtime/orcm_globals/Makefile
    orcm/test/runtime/cmd_server/Makefile
    orcm/test/runtime/orcm_finalize/Makefile
    orcm/test/mca/analytics/spatial/Makefile
    orcm/test/mca/db/Makefile
    orcm/test/mca/db/print/Makefile
    orcm/test/mca/db/base/Makefile
    orcm/test/mca/db/postgres/Makefile
    orcm/test/mca/db/zeromq/Makefile
    orcm/test/mca/diag/Makefile
    orcm/test/mca/diag/ethtest/Makefile
    orcm/test/mca/db/odbc/Makefile
    orcm/test/mca/cfgi/Makefile
    orcm/test/mca/cfgi/file10/Makefile
    orcm/test/mca/cfgi/file30/Makefile
    ])
])
