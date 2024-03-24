////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   drivergfx_init.cpp
//  Version:     v1.00
//  Created:     23/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#include "drivergfx_init.h"
#include "drivergfx_renderer.h"
#include "drivergfx_util.h"
#include "lib/volk.h"
#include "openxr/openxr.h"
#ifndef XR_USE_GRAPHICS_API_VULKAN
#define XR_USE_GRAPHICS_API_VULKAN
#endif
#include "openxr/openxr_platform.h"

namespace DS{
    void driverInitVk() {
        DSLOG_INFO(Ren, "Starting renderer");

        u32 WantedInstanceLayerCount = 0;
        char WantedInstanceLayers[64][VK_MAX_EXTENSION_NAME_SIZE];
        char* WantedInstanceLayerPtr[64];
        u32 WantedInstanceExtensionCount = 0;
        char WantedInstanceExtensions[64][VK_MAX_EXTENSION_NAME_SIZE];
        char* WantedInstanceExtensionPtr[64];

        DSLOG_INFO(Ren, "Enumerating instance capabilities");
        {
            u32 LayerCount = 0;
            VKC(vkEnumerateInstanceLayerProperties(&LayerCount, NULL));
            DSLOG_INFO(Ren, "Found %u layers", LayerCount);
            VkLayerProperties Layers[LayerCount];
            VKC(vkEnumerateInstanceLayerProperties(&LayerCount, Layers));
            for (u32 i = 0; i < LayerCount; i++) {
                DSLOG_INFO(Ren, "    Found layer %s implVer %u", Layers[i].layerName, Layers[i].implementationVersion);

            }

            u32 ExtensionCount = 0;
            VKC(vkEnumerateInstanceExtensionProperties(NULL, &ExtensionCount, NULL));
            VkExtensionProperties Extensions[ExtensionCount];
            DSLOG_INFO(Ren, "Found %u extensions", ExtensionCount);
            VKC(vkEnumerateInstanceExtensionProperties(NULL, &ExtensionCount, Extensions));
            for (u32 i = 0; i < ExtensionCount; i++) {
                DSLOG_INFO(Ren, "    Found extension %s ver %u", Extensions[i].extensionName, Extensions[i].specVersion);

            }

        }
        DSLOG_INFO(Ren, "Creating instance");
        {
            VkApplicationInfo AppInfo;
            AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            AppInfo.pNext = NULL;
            static const char* VkAppName = "driver_9_vr";
            AppInfo.pApplicationName = VkAppName;
            AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            static const char* VkEngineName = "QuestDS Engine";
            AppInfo.pEngineName = VkEngineName;
            AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            AppInfo.apiVersion = VK_API_VERSION_1_1;

            VkInstanceCreateInfo CreateInfo;
            CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            CreateInfo.pNext = NULL;
            CreateInfo.flags = 0;
            CreateInfo.pApplicationInfo = &AppInfo;
            if (WantedInstanceLayerCount == 0) {
                CreateInfo.enabledLayerCount = 0;
                CreateInfo.ppEnabledLayerNames = NULL;

            }
            else {
                for (u32 i = 0; i < WantedInstanceLayerCount; i++) {
                    WantedInstanceLayerPtr[i] = WantedInstanceLayers[i];

                }

                CreateInfo.enabledLayerCount = WantedInstanceLayerCount;
                CreateInfo.ppEnabledLayerNames = (const char* const*)WantedInstanceLayerPtr;

            }

            if (WantedInstanceExtensionCount == 0) {
                CreateInfo.enabledExtensionCount = 0;
                CreateInfo.ppEnabledExtensionNames = NULL;

            }
            else {
                for (u32 i = 0; i < WantedInstanceExtensionCount; i++) {
                    WantedInstanceExtensionPtr[i] = WantedInstanceExtensions[i];

                }

                CreateInfo.enabledExtensionCount = WantedInstanceExtensionCount;
                CreateInfo.ppEnabledExtensionNames = (const char* const*)WantedInstanceExtensionPtr;

            }

            XrVulkanInstanceCreateInfoKHR XrCreateInfo;
            XrCreateInfo.type = XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR;

//            TODO(clara): Implement openxr vulkan init code here

            VkResult InstanceResult = VK_SUCCESS;
            //XrResult InstanceXrResult = xrCreateVulkanInstanceKHR(
            DSLOG_INFO(Ren, "Created Vulkan instance with code %i", (int)InstanceResult);

        }

    }

}
