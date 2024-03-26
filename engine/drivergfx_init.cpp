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

#include "vr.h"
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

        u32 WantedDeviceExtensionCount = 0;
        char WantedDeviceExtensions[64][VK_MAX_EXTENSION_NAME_SIZE];
        char* WantedDeviceExtensionPtr[64];

        volkInitialize();

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
        DSLOG_INFO(Ren, "Gathering OpenXR Vulkan requirements");
        {
            XrGraphicsRequirementsVulkan2KHR XrRequirements { XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR };
            PFN_xrGetVulkanGraphicsRequirements2KHR VkRequirementFuncPtr;
            OXRC(xrGetInstanceProcAddr(vr::Instance, "xrGetVulkanGraphicsRequirements2KHR", (PFN_xrVoidFunction*)&VkRequirementFuncPtr));
            OXRC(VkRequirementFuncPtr(vr::Instance, vr::SystemId, &XrRequirements));

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
            XrCreateInfo.next = NULL;
            XrCreateInfo.systemId = vr::SystemId;
            XrCreateInfo.createFlags = 0;
            XrCreateInfo.pfnGetInstanceProcAddr = vkGetInstanceProcAddr;
            XrCreateInfo.vulkanCreateInfo = &CreateInfo;
            XrCreateInfo.vulkanAllocator = driverGfx::AllocCb;

            PFN_xrCreateVulkanInstanceKHR InstanceFunc;
            OXRC(xrGetInstanceProcAddr(vr::Instance, "xrCreateVulkanInstanceKHR", (PFN_vkVoidFunction*) & InstanceFunc));
            VkResult InstanceResult;
            XrResult InstanceXrResult = InstanceFunc(vr::Instance, &XrCreateInfo, &driverGfx::Instance, &InstanceResult);
            DSLOG_INFO(Ren, "Created Vulkan instance with code %i", (int)InstanceResult);
            volkLoadInstance(driverGfx::Instance);

        }
        DSLOG_INFO(Ren, "Enumerating physical devices");
        {
            XrVulkanGraphicsDeviceGetInfoKHR DeviceInfo { XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR };
            DeviceInfo.next = NULL;
            DeviceInfo.systemId = vr::SystemId;
            DeviceInfo.vulkanInstance = driverGfx::Instance;

            PFN_xrGetVulkanGraphicsDevice2KHR GetPhysDevice;
            OXRC(xrGetInstanceProcAddr(vr::Instance, "xrGetVulkanGraphicsDevice2KHR", (PFN_xrVoidFunction*)&GetPhysDevice));
            OXRC(GetPhysDevice(vr::Instance, &DeviceInfo, &driverGfx::PhysDevice));

        }
        DSLOG_INFO(Ren, "Enumerating physical device queues");
        {
            u32 QueuePropertieCount = 0;
            VKC(vkGetPhysicalDeviceQueueFamilyProperties(driverGfx::PhysDevice, &QueuePropertieCount, NULL));
            DSLOG_INFO(Ren, "Found %u queues", QueuePropertieCount);
            VkQueueFamilyProperties QueueProperties[QueuePropertieCount];
            VKC(vkGetPhysicalDeviceQueueFamilyProperties(driverGfx::PhysDevice, &QueuePropertieCount, QueueProperties));

            //            TODO(clara): I should write better queue enumerating and finding code
            for (u32 i = 0; i < QueuePropertieCount; i++) {
                DSLOG_INFO(Ren, "    Found queue %u count %u timestampBits %x min transfer w %u h %u d %u bits %x",
                         i, QueueProperties[i].queueCount, QueueProperties[i].timestampValidBits, QueueProperties[i].minImageTransferGranularity.width,
                         QueueProperties[i].minImageTransferGranularity.height, QueueProperties[i].minImageTransferGranularity.depth,
                         QueueProperties[i].queueFlags);

                if (QueueProperties[i] .queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    DSLOG_INFO(Ren, "     Queue %u is capable of rendering", i);

                }
                if (QueueProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
                    DSLOG_INFO(Ren, "     Queue %u is capable of transfering data", i);

                }
                if (QueueProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                    DSLOG_INFO(Ren, "     Queue %u is capable of compute,", i);

                }
                if (QueueProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
                    DSLOG_INFO(Ren, "     Queue %u is capable of sparce binding", i);

                }
                if (QueueProperties[i].queueFlags & VK_QUEUE_PROTECTED_BIT) {
                    DSLOG_INFO(Ren, "     Queue %u is protected", i);

                }

            }

        }
        DSLOG_INFO(Ren, "Enumerating device layers");
        {
            u32 ExtensionCount = 0;
            VKC(vkEnumerateDeviceExtensionProperties(driverGfx::PhysDevice, NULL, &ExtensionCount, NULL));
            DSLOG_INFO(Ren, "Found %u device extensions", ExtensionCount);
            VkExtensionProperties Extensions[ExtensionCount];
            VKC(vkEnumerateDeviceExtensionProperties(driverGfx::PhysDevice, NULL, &ExtensionCount, Extensions));
            for (u32 i = 0; i < ExtensionCount; i++) {
                DSLOG_INFO(Ren, "   Extension %u %s specVer %u", i, Extensions[i].extensionName, Extensions[i].specVersion);

            }

        }
        DSLOG_INFO(Ren, "Creating Vulkan device");
        {
            VkDeviceCreateInfo VkDeviceInfo { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
            VkDeviceInfo.pNext = NULL;
            VkDeviceInfo.flags = 0;

            static float QueuePriority = 1.0F;

            VkDeviceQueueCreateInfo QueueCreate { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            QueueCreate.pNext = NULL;
            QueueCreate.flags = 0;
            QueueCreate.queueFamilyIndex = 0; //TODO(clara): This is only going to work on the quest 3 and maybe 2
            QueueCreate.queueCount = 1;
            QueueCreate.pQueuePriorities = &QueuePriority;

            VkDeviceInfo.queueCreateInfoCount = 1;
            VkDeviceInfo.pQueueCreateInfos = &QueueCreate;

            VkDeviceInfo.enabledLayerCount = 0;
            VkDeviceInfo.ppEnabledLayerNames = NULL;
            if (WantedDeviceExtensionCount != 0) {
                for (u32 i = 0; i < WantedDeviceExtensionCount; i++) {
                    WantedDeviceExtensionPtr[i] = WantedDeviceExtensions[i];

                }

                VkDeviceInfo.enabledExtensionCount = 0;
                VkDeviceInfo.ppEnabledExtensionNames = WantedDeviceExtensionPtr;

            }
            else {
                VkDeviceInfo.enabledExtensionCount = 0;
                VkDeviceInfo.ppEnabledExtensionNames = NULL;

            }

            VkDeviceInfo.pEnabledFeatures = NULL;

            XrVulkanDeviceCreateInfoKHR XrDeviceInfo { XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR };
            XrDeviceInfo.next = NULL;
            XrDeviceInfo.systemId = vr::SystemId;
            XrDeviceInfo.createFlags = 0;
            XrDeviceInfo.pfnGetInstanceProcAddr = vkGetInstanceProcAddr;
            XrDeviceInfo.vulkanPhysicalDevice = driverGfx::PhysDevice;
            XrDeviceInfo.vulkanCreateInfo = &VkDeviceInfo;
            XrDeviceInfo.vulkanAllocator = driverGfx::AllocCb;

            VkResult VkDeviceResult;
            PFN_xrCreateVulkanDeviceKHR DeviceFuncion;
            OXRC(xrGetInstanceProcAddr(vr::Instance, "xrCreateVulkanDeviceKHR", (PFN_vkVoidFunction*) & DeviceFuncion));
            XrResult XrDeviceResult = DeviceFuncion(vr::Instance, &XrDeviceInfo, &driverGfx::Device, &VkDeviceResult);
            DSLOG_INFO(Ren, "Got code xr %i vk %i from OpenXR Vulkan device initialization", XrDeviceResult, VkDeviceResult);

        }

    }

}
