////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   job.h
//  Version:     v1.00
//  Created:     23/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base.h"

namespace DS{
    enum class jobThreadPriority : u32 {
        none = 0,
        Low,
        High,

    };

    void job_internalinit();

}
