////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   vr.h
//  Version:     v1.00
//  Created:     23/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base.h"
#include "openxr/openxr.h"

#define OXRC(Expression) DS::oxrCheck(Expression, #Expression)

namespace DS{
    struct vr {
        inline static XrInstance Instance = XR_NULL_HANDLE;

    };

    void oxrEarlyInit();
    void oxrCheck(XrResult ExpressionResult, const char* Name);

}
