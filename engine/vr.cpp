////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   vr.cpp
//  Version:     v1.00
//  Created:     23/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#include "lib/android_native_app_glue.h"
#include "vr.h"
#include "string.h"
#include "engine.h"
#define XR_USE_PLATFORM_ANDROID
#include "jni.h"
#include "openxr/openxr_platform.h"

namespace DS{
    void oxrEarlyInit() {
        DSLOG_INFO(Oxr, "Initializing openxr driver");

        //        Enumerate layers
        {
            XrLoaderInitInfoAndroidKHR LoaderInfo = { XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR };
            LoaderInfo.applicationVM = engineConfig::JavaVMPtr;
            //LoaderInfo.applicationContext = engineConfig::AppPtr->activity->clazz;

            //DSLOG_INFO(Oxr, "Got vm ptr %x", AndroidPtr);
            //DSLOG_INFO(Oxr, "Got Activity ptr %x", AndroidPtr->activity);
            DSLOG_INFO(Oxr, "Got vm ptr %x", engineConfig::JavaVMPtr);
            //DSLOG_INFO(Oxr, "Got clazz ptr %x", engineConfig::AppPtr->activity->clazz);
            //LoaderInfo.applicationVM = AndroidPtr->activity->vm;
            //LoaderInfo.applicationContext = AndroidPtr->activity->clazz;

            PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR;
            xrGetInstanceProcAddr(
                XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)&xrInitializeLoaderKHR);
            XrResult LoaderResult = xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)&LoaderInfo);
            
            DSLOG_INFO(Oxr, "Got code %i from loader initialization", LoaderResult);

            u32 LayerCount;
            OXRC(xrEnumerateApiLayerProperties(0, &LayerCount, NULL));
            DSLOG_INFO(Oxr, "Detecting runtime layers, %u currentelly present", LayerCount);
            if (LayerCount != 0) {
                //                TODO(clara): Implement api layers enumeration

            }

        }

        //        Enumerate extensions
        u32 ExtensionCount = 0;
        u32 WantedExtensionCount = 0;
        char WantedExtension[8][XR_MAX_EXTENSION_NAME_SIZE];
        {
            OXRC(xrEnumerateInstanceExtensionProperties(NULL, 0, &ExtensionCount, NULL));
            DSLOG_INFO(Oxr, "Found %u extensions", ExtensionCount);

            XrExtensionProperties ExtensionList[ExtensionCount];
            for (u32 i = 0; i < ExtensionCount; i++) {

                //                The next line is the reason why i hate openxr
                ExtensionList[i].type = XR_TYPE_EXTENSION_PROPERTIES;

            }

            OXRC(xrEnumerateInstanceExtensionProperties(NULL, ExtensionCount, &ExtensionCount, &ExtensionList[0]));
            for (u32 i = 0; i < ExtensionCount; i++) {
                DSLOG_INFO(Oxr, "    Extension %u %s ver %u", i,
                         ExtensionList[i].extensionName, ExtensionList[i].extensionVersion);

            }

            DSLOG_INFO(Oxr, "Selecting OpenXR extensions to pass to runtime");
            //            TODO(clara): Check if the extension is present on the runtime
            dcpy(WantedExtension[WantedExtensionCount], "XR_KHR_vulkan_enable2", strlen("XR_KHR_vulkan_enable2") + 1);
            WantedExtensionCount++;
            dcpy(WantedExtension[WantedExtensionCount], "XR_EXT_debug_utils", strlen("XR_EXT_debug_utils") + 1);
            WantedExtensionCount++;

            for (u64 i = 0; i < WantedExtensionCount; i++) {
                DSLOG_INFO(Oxr, "    Selected %s", WantedExtension[i]);

            }

        }

        //        Instance creation
        DSLOG_INFO(Oxr, "Creating instance");
        {
            char* ExtensionPtr[WantedExtensionCount];
            for (u64 i = 0; i < WantedExtensionCount; i++) {
                ExtensionPtr[i] = WantedExtension[i];

            }

            XrInstanceCreateInfo CreateInfo { XR_TYPE_INSTANCE_CREATE_INFO };
            CreateInfo.next = NULL;
            CreateInfo.createFlags = 0;
            dcpy(CreateInfo.applicationInfo.applicationName, "Myth Editor", sizeof("Myth Editor") + 1);
            CreateInfo.applicationInfo.applicationVersion = XR_MAKE_VERSION(5, 0, 0);
            dcpy(CreateInfo.applicationInfo.engineName, "DISRUPT", sizeof("DISRUPT") + 1);
            CreateInfo.applicationInfo.engineVersion = XR_MAKE_VERSION(3, 3, 80);
            CreateInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
            //            TODO(clara): Add layer support
            CreateInfo.enabledApiLayerCount = 0;
            CreateInfo.enabledApiLayerNames = nullptr;
            CreateInfo.enabledExtensionCount = WantedExtensionCount;
            CreateInfo.enabledExtensionNames = (const char* const*)ExtensionPtr;

            OXRC(xrCreateInstance(&CreateInfo, &vr::Instance));

        }

    }

    void oxrCheck(XrResult ExpressionResult, const char* Name) {
        if (!(XR_SUCCEEDED(ExpressionResult))) {
            DSLOG_ERROR(Oxr, "Failed openxr call with call %s witch returned %i", Name, ExpressionResult);

        }

    }

    void ovrEarlyInit() {
        DSLOG_INFO(Oxr, "Initializing VrApi");

    }

}
