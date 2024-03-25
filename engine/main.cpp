////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   main.cpp
//  Version:     v1.00
//  Created:     25/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#include "lib/android_native_app_glue.h"
#include "engine.h"

extern "C"{
    __attribute__((visibility("default"))) void android_main(android_app* AppPtr);

}

__attribute__((visibility("default"))) void android_main(android_app* AppPtr) {
    ds_main_android(AppPtr);

}

