/*
 * Copyright (c) 2016 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "mysensor.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

void mySensor::init()
{
    std::cout << "On init in plugin" << std::endl;
}

void mySensor::sample(dataContainer &dc)
{
    int random;
    random = rand() % 100;
    std::cout << "On sample, storing random number " << random << std::endl;
    // Storing integer data into the dataContainer object
    dc.put("intValue_1", random, "ints");
}

void mySensor::finalize()
{
    std::cout << "On finalize" << std::endl;
}

export_plugin(mySensor, "MySensor");
