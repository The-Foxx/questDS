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

#include "base.h"
#define VK_NO_PROTOTYPES
#include "vulkan/vulkan_core.h"

namespace DS{
    struct driverGfx {
        static inline VkInstance Instance = VK_NULL_HANDLE;
        static inline VkPhysicalDevice PhysDevice = VK_NULL_HANDLE;
        static inline VkDevice Device = VK_NULL_HANDLE;
//        TODO(clara): This should be deduced from enumeration but we only target the quest so this is file for now
        static inline u32 GraphicsQueueIndex = 0;
        static inline VkAllocationCallbacks* AllocCb = NULL;

    };

}
