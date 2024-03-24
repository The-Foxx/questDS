////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   log.h
//  Version:     v1.00
//  Created:     20/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base_types.h"
#include "stdio.h"

#define DSLOG_INFO(Subsystem, ...) { char Message[2000]; sprintf(Message, __VA_ARGS__); \
DS::logToConsole(Message, DS::MSS::Subsystem, DS::MMG::info); }

#define DSLOG_ERROR(Subsystem, ...) { char Message[2000]; sprintf(Message, __VA_ARGS__); \
DS::logToConsole(Message, DS::MSS::Subsystem, DS::MMG::info); }

namespace DS{
    enum class MSS : u32 {
        none,
        Engine,
        Job,
        Ren,
        Oxr,

    };

    enum class MMG : u32 {
        info,
        wanr,
        error,

    };

    void logToConsole(const char* Message, MSS System, MMG Gravity);

}
