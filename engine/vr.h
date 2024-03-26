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
        inline static XrSystemId SystemId = 0;
        inline static bool eyeChangeRes = false;
        inline static u32 eyeWidth = 0;
        inline static u32 eyeHeight = 0;
        inline static XrSession Session;
        inline static XrReferenceSpaceType WantedSpaceType = (XrReferenceSpaceType)0;
        inline static XrSpace Spade;
        inline static XrSwapchain Swapchain;

    };

    void oxrEarlyInit();
    void oxrCheck(XrResult ExpressionResult, const char* Name);
    XrBool32 debugCallbackHandleing(XrDebugUtilsMessageSeverityFlagsEXT Severity,
                                    XrDebugUtilsMessageTypeFlagsEXT Type,
                                    const XrDebugUtilsMessengerCallbackDataEXT* Data, void* UserData);
    void oxrInit();

    const char* xrReferenceSpaceTypeStr(XrReferenceSpaceType In);
    const char* xrEnvironmentBlendModeStr(XrEnvironmentBlendMode In);
    const char* xrResultStr(XrResult In);

}
