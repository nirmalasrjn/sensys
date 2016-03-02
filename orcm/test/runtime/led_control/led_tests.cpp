/*
 * Copyright (c) 2016  Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <iostream>

#include "orcm/runtime/led_control.h"

#include "led_tests.h"
#include "led_tests_mocking.h"

#define HOSTNAME "hostame"
#define USER "user"
#define PASS "password"

using namespace std;

void ut_control_led_tests::SetUp(){
    cout<<"Starting test"<<endl;
    ipmi_open_session_count = 0;
    is_remote_node = false;
}

void ut_control_led_tests::TearDown(){
    cout<<"Finishing test"<<endl;
    ASSERT_EQ(0, ipmi_open_session_count);
}

TEST_F(ut_control_led_tests, new_led_object){
    LedControl lc;
}

TEST_F(ut_control_led_tests, new_led_object_remote){
    LedControl lc(HOSTNAME, USER, PASS);
    ASSERT_EQ(0, lc.disableChassisID());
    ASSERT_TRUE(is_remote_node);
}

TEST_F(ut_control_led_tests, enable_chassis_id_n_seconds){
    LedControl lc;
    ASSERT_EQ(0, lc.enableChassisID(20));
    ASSERT_EQ(LED_TEMPORARY_ON, lc.getChassisIDState());
}

TEST_F(ut_control_led_tests, enable_chassis_id_indefinitely){
    LedControl lc;
    ASSERT_EQ(0, lc.enableChassisID());
    ASSERT_EQ(LED_INDEFINITE_ON, lc.getChassisIDState());
}

TEST_F(ut_control_led_tests, disable_chassis_id){
    LedControl lc;
    ASSERT_EQ(0, lc.disableChassisID());
    ASSERT_EQ(LED_OFF, lc.getChassisIDState());
}

TEST_F(ut_control_led_tests, wrong_remote_data){
    LedControl lc(HOSTNAME, "", PASS);
    ASSERT_NE(0, lc.disableChassisID());
}