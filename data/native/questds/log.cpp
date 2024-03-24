////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   log.cpp
//  Version:     v1.00
//  Created:     23/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#include "log.h"
#include "android/log.h"

namespace DS{
    void logToConsole(const char* Message, MSS System, MMG Gravity) {
        __android_log_print(ANDROID_LOG_INFO, "QuestDS Engine", "%s", Message);

    }

}
