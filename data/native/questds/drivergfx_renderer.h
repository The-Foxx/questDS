////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   drivergfx_renderer.h
//  Version:     v1.00
//  Created:     23/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#define VK_NO_PROTOTYPES
#include "vulkan/vulkan_core.h"

namespace DS{
    struct driverGfx {
        static inline VkInstance Instance = VK_NULL_HANDLE;
        static inline VkPhysicalDevice PhysDevice = VK_NULL_HANDLE;
        static inline VkDevice Device = VK_NULL_HANDLE;
        static inline VkAllocationCallbacks* AllocCb = NULL;

    };

}
