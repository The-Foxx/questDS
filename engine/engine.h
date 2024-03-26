////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   engine.h
//  Version:     v1.00
//  Created:     20/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base_types.h"

struct android_app;

namespace DS{
    struct engineConfig{
        static inline android_app* AppPtr;
        static inline void* JavaVMPtr;
        static inline bool IsShuttingDown = false;

    };

}

extern "C"{
    void ds_main_android(android_app* AndroidAppPtr);
    void ds_main();
    void ds_loop();

}
