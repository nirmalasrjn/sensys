#
# Copyright (c) 2013-2015 Intel Corporation. All rights reserved.
# Copyright (c) 2016      Intel Corporation. All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

sources = \
        db_postgres.h \
        db_postgres_component.c \
        db_postgres.c

# Make the output library in this directory, and name it either
# mca_<type>_<name>.la (for DSO builds) or libmca_<type>_<name>.la
# (for static builds).

if MCA_BUILD_orcm_db_postgres_DSO
component_noinst =
component_install = mca_db_postgres.la
else
component_noinst = libmca_db_postgres.la
component_install =
endif

mcacomponentdir = $(orcmlibdir)
mcacomponent_LTLIBRARIES = $(component_install)
mca_db_postgres_la_CPPFLAGS = $(opal_db_postgres_CPPFLAGS)
mca_db_postgres_la_SOURCES = $(sources)
mca_db_postgres_la_LDFLAGS = -module -avoid-version $(opal_db_postgres_LDFLAGS)
mca_db_postgres_la_LIBADD = $(opal_db_postgres_LIBS)

noinst_LTLIBRARIES = $(component_noinst)
libmca_db_postgres_la_SOURCES =$(sources)
libmca_db_postgres_la_CPPFLAGS = $(opal_db_postgres_CPPFLAGS)
libmca_db_postgres_la_LDFLAGS = -module -avoid-version $(opal_db_postgres_LDFLAGS)
libmca_db_postgres_la_LIBADD = $(opal_db_postgres_LIBS)
