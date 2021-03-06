/*
 * Copyright (c) 2015-2016  Intel Corporation. All rights reserved.
 * Copyright (c) 2016      Intel Corporation. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/mib_api.h>
#include "net-snmp/library/transform_oids.h"

#include <iostream>
#include <list>
#include <map>
#include "orcm/util/vardata.h"
#include "orcm/util/string_utils.h"
#include <stdexcept>

#ifndef SNMP_COLLECTOR_H
#define SNMP_COLLECTOR_H

#define STRING_BUFFER_SIZE 1024
#define SUCCESS 0

enum auth_type {MD5, SHA1};
enum sec_type {NOAUTH, AUTHNOPRIV, AUTHPRIV};
enum priv_protocol {DES, AES, NOPRIV};

class RuntimeMetrics;

using std::runtime_error;

class snmpCollector {
    public:
        snmpCollector();
        snmpCollector(std::string hostname, std::string username);
        snmpCollector(std::string hostname, std::string username, std::string password);
        snmpCollector(std::string hostname, std::string username, std::string password, auth_type auth);
        snmpCollector(std::string hostname, std::string username, std::string password, auth_type auth, sec_type sec);
        snmpCollector(std::string hostname, std::string username, std::string password, auth_type auth, sec_type sec, priv_protocol priv);
        ~snmpCollector();

        void dump_pdu(netsnmp_pdu *p);
        void setOIDs(std::string strOIDs);
        void updateOIDs();
        void setLocation(std::string location);
        std::vector<vardata> collectData();
        std::string getHostname() { return hostname; };
        std::list<std::string> getOIDsList() { return oidList; };
        void setRuntimeMetrics(RuntimeMetrics* metrics);

    private:
        struct snmp_session session;
        netsnmp_pdu *pdu;
        netsnmp_pdu *response;
        oid anOID[MAX_OID_LEN];
        size_t anOID_len;
        RuntimeMetrics* runtime_metrics_;

        std::string hostname, username, password, location;
        std::list<std::string> oidList;

        void setSecurityLevel(sec_type sec);
        void setPrivacyLevel(priv_protocol priv);
        void setMD5Authentication(std::string password);
        void setSHA1Authentication(std::string password);
        void storeCharsAndLength(std::string s, char **c_str, size_t *len);
        void setAuthentication(std::string password);
        std::list<std::string> splitString(std::string input, char delimiter);
        std::vector<vardata> packCollectedData(netsnmp_pdu *response);

};

class invalidPassword : public runtime_error {
    public:
        invalidPassword() : runtime_error( "Unable to generate encrypted password" ) {}
};

class invalidOIDParsing: public runtime_error {
    public:
        invalidOIDParsing() : runtime_error("Unable to parse OID object or string") {}
};

class invalidSession: public runtime_error {
    public:
        invalidSession() : runtime_error("Unable to create SNMP session") {}
};

class snmpTimeout: public runtime_error {
    public:
        snmpTimeout(string &device) : runtime_error(
            string("Connection to SNMP device timed out: ")
            + string(device)) {}
};

class dataCollectionError: public runtime_error {
    public:
        dataCollectionError(string &device) : runtime_error(
            string ("Error during data collection. Possible misconfigured SNMP device (")
            + device + string(")")) {}
};

class packetError: public runtime_error {
    public:
        packetError() : runtime_error("Error in packet") {}
};

#endif /* SNMP_COLLECTOR_H */
