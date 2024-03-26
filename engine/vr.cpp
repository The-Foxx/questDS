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
    static XrDebugUtilsMessengerEXT DebugMessenger = XR_NULL_HANDLE;

    void oxrEarlyInit() {
        DSLOG_INFO(Oxr, "Initializing openxr driver");

        //        Enumerate layers
        {
            XrLoaderInitInfoAndroidKHR LoaderInfo = { XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR };
            LoaderInfo.applicationVM = engineConfig::AppPtr->activity->vm;
            LoaderInfo.applicationContext = engineConfig::AppPtr->activity->clazz;

            //DSLOG_INFO(Oxr, "Got vm ptr %x", AndroidPtr);
            //DSLOG_INFO(Oxr, "Got Activity ptr %x", AndroidPtr->activity);
            DSLOG_INFO(Oxr, "Got vm ptr %lx", (unsigned long)engineConfig::JavaVMPtr);
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
            dcpy(CreateInfo.applicationInfo.applicationName, "driver_9", sizeof("driver_9") + 1);
            CreateInfo.applicationInfo.applicationVersion = 1;
            dcpy(CreateInfo.applicationInfo.engineName, "DISRUPT", sizeof("DISRUPT") + 1);
            CreateInfo.applicationInfo.engineVersion = 3380;
            CreateInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
            //            TODO(clara): Add layer support
            CreateInfo.enabledApiLayerCount = 0;
            CreateInfo.enabledApiLayerNames = nullptr;
            CreateInfo.enabledExtensionCount = WantedExtensionCount;
            CreateInfo.enabledExtensionNames = (const char* const*)ExtensionPtr;

            XrResult InstanceResult = xrCreateInstance(&CreateInfo, &vr::Instance);
            DSLOG_INFO(Oxr, "Created OpenXR instance code %s", xrResultStr(InstanceResult));

        }
        DSLOG_INFO(Oxr, "Creating OpenXR debug interface");
        {
            XrDebugUtilsMessengerCreateInfoEXT DebugCreateInfo { XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
            DebugCreateInfo.next = NULL;
            DebugCreateInfo.messageSeverities = 0x00001111;
            DebugCreateInfo.messageTypes = 0x0000000F;
            DebugCreateInfo.userCallback = &debugCallbackHandleing;
            DebugCreateInfo.userData = NULL;

            PFN_xrCreateDebugUtilsMessengerEXT CreateDebugUtilFunc;
            OXRC(xrGetInstanceProcAddr(vr::Instance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&CreateDebugUtilFunc));
            OXRC(CreateDebugUtilFunc(vr::Instance, &DebugCreateInfo, &DebugMessenger));

        }
        DSLOG_INFO(Oxr, "Detecting OpenXR runtime");
        {
            XrInstanceProperties InstanceProperties { XR_TYPE_INSTANCE_PROPERTIES };
            OXRC(xrGetInstanceProperties(vr::Instance, &InstanceProperties));

            DSLOG_INFO(Oxr, "Using runtime %s version %lx",
                     InstanceProperties.runtimeName, (u64)InstanceProperties.runtimeVersion);

        }
        DSLOG_INFO(Oxr, "Gathering OpenXR System");
        {
            XrSystemGetInfo SystemInfo { XR_TYPE_SYSTEM_GET_INFO };
            SystemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

            OXRC(xrGetSystem(vr::Instance, &SystemInfo, &vr::SystemId));
            DSLOG_INFO(Oxr, "Got system id %lx", vr::SystemId);

        }
        DSLOG_INFO(Oxr, "Configuring OpenXR Views");
        {
            u32 ViewCOnfigTypeCount = 0;
            OXRC(xrEnumerateViewConfigurations(vr::Instance, vr::SystemId, ViewCOnfigTypeCount, &ViewCOnfigTypeCount, NULL));
            XrViewConfigurationType ViewConfigTypes[ViewCOnfigTypeCount];
            OXRC(xrEnumerateViewConfigurations(vr::Instance, vr::SystemId, ViewCOnfigTypeCount, &ViewCOnfigTypeCount, ViewConfigTypes));

            DSLOG_INFO(Oxr, "Found %u views",ViewCOnfigTypeCount);

            for (u32 i = 0; i < ViewCOnfigTypeCount; i++) {
                if (ViewConfigTypes[i] == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) {
                    DSLOG_INFO(Oxr, "Using view %u", i);

                }

                XrViewConfigurationProperties ViewConfigProp { XR_TYPE_VIEW_CONFIGURATION_PROPERTIES };
                OXRC(xrGetViewConfigurationProperties(vr::Instance, vr::SystemId, ViewConfigTypes[i], &ViewConfigProp));
                DSLOG_INFO(Oxr, "HasFOVMutable %u", ViewConfigProp.fovMutable);

                u32 ViewConfigCount = 0;
                OXRC(xrEnumerateViewConfigurationViews(vr::Instance, vr::SystemId, ViewConfigTypes[i], ViewConfigCount, &ViewConfigCount,
                                                       NULL));

                DSLOG_INFO(Oxr, "Found %u view config views (yes that's an actuall openxr therm)", ViewConfigCount);

                if (ViewConfigCount != 2) {
                    DSLOG_ERROR(Oxr, "Number of OpenXR views is not 2 but %u, we do not support OpenXR devices with anything but 2 views",
                                ViewConfigCount);

                }

                if (ViewConfigCount != 0) {
                    XrViewConfigurationView ViewConfigs[ViewConfigCount];
                    for (u32 i = 0; i < ViewConfigCount; i++) {
                        ViewConfigs[i].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;

                    }

                    OXRC(xrEnumerateViewConfigurationViews(vr::Instance, vr::SystemId, ViewConfigTypes[i],
                                                           ViewConfigCount, &ViewConfigCount, ViewConfigs));

                    for (u64 i = 0; i < ViewConfigCount; i++) {
                        DSLOG_INFO(Oxr, "      View %lu, recomended w/h: %u/%u, max w/h: %u/%u, sample count: max %u min %u",
                                   i, ViewConfigs[i].recommendedImageRectWidth, ViewConfigs[i].recommendedImageRectHeight,
                                   ViewConfigs[i].maxImageRectWidth, ViewConfigs[i].maxImageRectHeight,
                                   ViewConfigs[i].recommendedSwapchainSampleCount, ViewConfigs[i].maxSwapchainSampleCount);

                    }

                    vr::eyeWidth = ViewConfigs[i].recommendedImageRectWidth;
                    vr::eyeHeight = ViewConfigs[i].recommendedImageRectHeight;

                    u32 EnviromentCount = 0;
                    OXRC(xrEnumerateEnvironmentBlendModes(vr::Instance, vr::SystemId, ViewConfigTypes[i], EnviromentCount, &EnviromentCount, NULL));
                    XrEnvironmentBlendMode Enviroments[EnviromentCount];
                    OXRC(xrEnumerateEnvironmentBlendModes(vr::Instance, vr::SystemId, ViewConfigTypes[i], EnviromentCount, &EnviromentCount, Enviroments));

                    bool isBlendModeFound = false;
                    for (u64 i = 0; i < EnviromentCount; i++) {
                        DSLOG_INFO(Oxr, "      Blend mode %lu %s", i, xrEnvironmentBlendModeStr(Enviroments[i]));
                        if (isBlendModeFound == false && Enviroments[i] == XR_ENVIRONMENT_BLEND_MODE_OPAQUE) {
                            DSLOG_INFO(Oxr, "       Chose enviroment blend mode %lu", i);

                        }

                    }

                }
                else {
                    DSLOG_ERROR(Oxr, "View count is 0, something extremelly wrong happend here");

                }

            }

        }

    }

    void oxrCheck(XrResult ExpressionResult, const char* Name) {
        if (!(XR_SUCCEEDED(ExpressionResult))) {
            DSLOG_ERROR(Oxr, "Failed openxr call with call %s witch returned %i", Name, ExpressionResult);

        }

    }

    //void ovrEarlyInit() {
        //DSLOG_INFO(Oxr, "Initializing VrApi");

    //}

    XrBool32 debugCallbackHandleing(XrDebugUtilsMessageSeverityFlagsEXT Severity,
                                    XrDebugUtilsMessageTypeFlagsEXT Type,
                                    const XrDebugUtilsMessengerCallbackDataEXT* Data, void* UserData) {
        DSLOG_INFO(Oxr, "message: %s functionName: %s id: %s",
                 Data->message, Data->functionName, Data->messageId);

        return XR_TRUE;

    }

    const char* xrEnvironmentBlendModeStr(XrEnvironmentBlendMode In) {
        switch (In) {
            case (XR_ENVIRONMENT_BLEND_MODE_OPAQUE):
                return "XR_ENVIRONMENT_BLEND_MODE_OPAQUE";
                break;
            case (XR_ENVIRONMENT_BLEND_MODE_ADDITIVE):
                return "XR_ENVIRONMENT_BLEND_MODE_ADDITIVE";
                break;
            case (XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND):
                return "XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND";
                break;
            case (XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM):
                return "XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM";
                break;
            default:
                return "XR_ENVIROMENT_BLEND_MODE_STR_FAILURE";

        }

    }

    const char* xrResultStr(XrResult In) {
        switch (In) {
            case(XR_SUCCESS):
                return "XR_SUCCESS";
                break;
            case(XR_TIMEOUT_EXPIRED):
                return "XR_TIMEOUT_EXPIRED";
                break;
            case(XR_SESSION_LOSS_PENDING):
                return "XR_SESSION_LOSS_PENDING";
                break;
            case(XR_EVENT_UNAVAILABLE):
                return "XR_EVENT_UNAVAILABLE";
                break;
            case(XR_SPACE_BOUNDS_UNAVAILABLE):
                return "XR_SPACE_BOUNDS_UNAVAILABLE";
                break;
            case(XR_SESSION_NOT_FOCUSED):
                return "XR_SESSION_NOT_FOCUSED";
                break;
            case(XR_FRAME_DISCARDED):
                return "XR_FRAME_DISCARDED";
                break;
            case(XR_ERROR_VALIDATION_FAILURE):
                return "XR_ERROR_VALIDATION_FAILURE";
                break;
            case(XR_ERROR_RUNTIME_FAILURE):
                return "XR_ERROR_RUNTIME_FAILURE";
                break;
            case(XR_ERROR_OUT_OF_MEMORY):
                return "XR_ERROR_OUT_OF_MEMORY";
                break;
            case(XR_ERROR_API_VERSION_UNSUPPORTED):
                return "XR_ERROR_API_VERSION_UNSUPPORTED";
                break;
            case(XR_ERROR_INITIALIZATION_FAILED):
                return "XR_ERROR_INITIALIZATION_FAILED";
                break;
            case(XR_ERROR_FUNCTION_UNSUPPORTED):
                return "XR_ERROR_FUNCTION_UNSUPPORTED";
                break;
            case(XR_ERROR_FEATURE_UNSUPPORTED):
                return "XR_ERROR_FEATURE_UNSUPPORTED";
                break;
            case(XR_ERROR_EXTENSION_NOT_PRESENT):
                return "XR_ERROR_EXTENSION_NOT_PRESENT";
                break;
            case(XR_ERROR_LIMIT_REACHED):
                return "XR_ERROR_LIMIT_REACHED";
                break;
            case(XR_ERROR_SIZE_INSUFFICIENT):
                return "XR_ERROR_SIZE_INSUFFICIENT";
                break;
            case(XR_ERROR_HANDLE_INVALID):
                return "XR_ERROR_HANDLE_INVALID";
                break;
            case(XR_ERROR_INSTANCE_LOST):
                return "XR_ERROR_INSTANCE_LOST";
                break;
            case(XR_ERROR_SESSION_RUNNING):
                return "XR_ERROR_SESSION_RUNNING";
                break;
            case(XR_ERROR_SESSION_NOT_RUNNING):
                return "XR_ERROR_SESSION_NOT_RUNNING";
                break;
            case(XR_ERROR_SESSION_LOST):
                return "XR_ERROR_SESSION_LOST";
                break;
            case(XR_ERROR_SYSTEM_INVALID):
                return "XR_ERROR_SYSTEM_INVALID";
                break;
            case(XR_ERROR_PATH_INVALID):
                return "XR_ERROR_PATH_INVALID";
                break;
            case(XR_ERROR_PATH_COUNT_EXCEEDED):
                return "XR_ERROR_PATH_COUNT_EXCEEDED";
                break;
            case(XR_ERROR_PATH_FORMAT_INVALID):
                return "XR_ERROR_PATH_FORMAT_INVALID";
                break;
            case(XR_ERROR_PATH_UNSUPPORTED):
                return "XR_ERROR_PATH_UNSUPPORTED";
                break;
            case(XR_ERROR_LAYER_INVALID):
                return "XR_ERROR_LAYER_INVALID";
                break;
            case(XR_ERROR_LAYER_LIMIT_EXCEEDED):
                return "XR_ERROR_LAYER_LIMIT_EXCEEDED";
                break;
            case(XR_ERROR_SWAPCHAIN_RECT_INVALID):
                return "XR_ERROR_SWAPCHAIN_RECT_INVALID";
                break;
            case(XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED):
                return "XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED";
                break;
            case(XR_ERROR_ACTION_TYPE_MISMATCH):
                return "XR_ERROR_ACTION_TYPE_MISMATCH";
                break;
            case(XR_ERROR_SESSION_NOT_READY):
                return "XR_ERROR_SESSION_NOT_READY";
                break;
            case(XR_ERROR_SESSION_NOT_STOPPING):
                return "XR_ERROR_SESSION_NOT_STOPPING";
                break;
            case(XR_ERROR_TIME_INVALID):
                return "XR_ERROR_TIME_INVALID";
                break;
            case(XR_ERROR_REFERENCE_SPACE_UNSUPPORTED):
                return "XR_ERROR_REFERENCE_SPACE_UNSUPPORTED";
                break;
            case(XR_ERROR_FILE_ACCESS_ERROR):
                return "XR_ERROR_FILE_ACCESS_ERROR";
                break;
            case(XR_ERROR_FILE_CONTENTS_INVALID):
                return "XR_ERROR_FILE_CONTENTS_INVALID";
                break;
            case(XR_ERROR_FORM_FACTOR_UNSUPPORTED):
                return "XR_ERROR_FORM_FACTOR_UNSUPPORTED";
                break;
            case(XR_ERROR_FORM_FACTOR_UNAVAILABLE):
                return "XR_ERROR_FORM_FACTOR_UNAVAILABLE";
                break;
            case(XR_ERROR_API_LAYER_NOT_PRESENT):
                return "XR_ERROR_API_LAYER_NOT_PRESENT";
                break;
            case(XR_ERROR_CALL_ORDER_INVALID):
                return "XR_ERROR_CALL_ORDER_INVALID";
                break;
            case(XR_ERROR_GRAPHICS_DEVICE_INVALID):
                return "XR_ERROR_GRAPHICS_DEVICE_INVALID";
                break;
            case(XR_ERROR_POSE_INVALID):
                return "XR_ERROR_POSE_INVALID";
                break;
            case(XR_ERROR_INDEX_OUT_OF_RANGE):
                return "XR_ERROR_INDEX_OUT_OF_RANGE";
                break;
            case(XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED):
                return "XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED";
                break;
            case(XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED):
                return "XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED";
                break;
            case(XR_ERROR_NAME_DUPLICATED):
                return "XR_ERROR_NAME_DUPLICATED";
                break;
            case(XR_ERROR_NAME_INVALID):
                return "XR_ERROR_NAME_INVALID";
                break;
            case(XR_ERROR_ACTIONSET_NOT_ATTACHED):
                return "XR_ERROR_ACTIONSET_NOT_ATTACHED";
                break;
            case(XR_ERROR_ACTIONSETS_ALREADY_ATTACHED):
                return "XR_ERROR_ACTIONSETS_ALREADY_ATTACHED";
                break;
            case(XR_ERROR_LOCALIZED_NAME_DUPLICATED):
                return "XR_ERROR_LOCALIZED_NAME_DUPLICATED";
                break;
            case(XR_ERROR_LOCALIZED_NAME_INVALID):
                return "XR_ERROR_LOCALIZED_NAME_INVALID";
                break;
            case(XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING):
                return "XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING";
                break;
            case(XR_ERROR_RUNTIME_UNAVAILABLE):
                return "XR_ERROR_RUNTIME_UNAVAILABLE";
                break;
            case(XR_ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR):
                return "XR_ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR";
                break;
            case(XR_ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR):
                return "XR_ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR";
                break;
            case(XR_ERROR_CREATE_SPATIAL_ANCHOR_FAILED_MSFT):
                return "XR_ERROR_CREATE_SPATIAL_ANCHOR_FAILED_MSFT";
                break;
            case(XR_ERROR_SECONDARY_VIEW_CONFIGURATION_TYPE_NOT_ENABLED_MSFT):
                return "XR_ERROR_SECONDARY_VIEW_CONFIGURATION_TYPE_NOT_ENABLED_MSFT";
                break;
            case(XR_ERROR_CONTROLLER_MODEL_KEY_INVALID_MSFT):
                return "XR_ERROR_CONTROLLER_MODEL_KEY_INVALID_MSFT";
                break;
            case(XR_ERROR_REPROJECTION_MODE_UNSUPPORTED_MSFT):
                return "XR_ERROR_REPROJECTION_MODE_UNSUPPORTED_MSFT";
                break;
            case(XR_ERROR_COMPUTE_NEW_SCENE_NOT_COMPLETED_MSFT):
                return "XR_ERROR_COMPUTE_NEW_SCENE_NOT_COMPLETED_MSFT";
                break;
            case(XR_ERROR_SCENE_COMPONENT_ID_INVALID_MSFT):
                return "XR_ERROR_SCENE_COMPONENT_ID_INVALID_MSFT";
                break;
            case(XR_ERROR_SCENE_COMPONENT_TYPE_MISMATCH_MSFT):
                return "XR_ERROR_SCENE_COMPONENT_TYPE_MISMATCH_MSFT";
                break;
            case(XR_ERROR_SCENE_MESH_BUFFER_ID_INVALID_MSFT):
                return "XR_ERROR_SCENE_MESH_BUFFER_ID_INVALID_MSFT";
                break;
            case(XR_ERROR_SCENE_COMPUTE_FEATURE_INCOMPATIBLE_MSFT):
                return "XR_ERROR_SCENE_COMPUTE_FEATURE_INCOMPATIBLE_MSFT";
                break;
            case(XR_ERROR_SCENE_COMPUTE_CONSISTENCY_MISMATCH_MSFT):
                return "XR_ERROR_SCENE_COMPUTE_CONSISTENCY_MISMATCH_MSFT";
                break;
            case(XR_ERROR_DISPLAY_REFRESH_RATE_UNSUPPORTED_FB):
                return "XR_ERROR_DISPLAY_REFRESH_RATE_UNSUPPORTED_FB";
                break;
            case(XR_ERROR_COLOR_SPACE_UNSUPPORTED_FB):
                return "XR_ERROR_COLOR_SPACE_UNSUPPORTED_FB";
                break;
            case(XR_ERROR_SPACE_COMPONENT_NOT_SUPPORTED_FB):
                return "XR_ERROR_SPACE_COMPONENT_NOT_SUPPORTED_FB";
                break;
            case(XR_ERROR_SPACE_COMPONENT_NOT_ENABLED_FB):
                return "XR_ERROR_SPACE_COMPONENT_NOT_ENABLED_FB";
                break;
            case(XR_ERROR_SPACE_COMPONENT_STATUS_PENDING_FB):
                return "XR_ERROR_SPACE_COMPONENT_STATUS_PENDING_FB";
                break;
            case(XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB):
                return "XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB";
                break;
            case(XR_ERROR_UNEXPECTED_STATE_PASSTHROUGH_FB):
                return "XR_ERROR_UNEXPECTED_STATE_PASSTHROUGH_FB";
                break;
            case(XR_ERROR_FEATURE_ALREADY_CREATED_PASSTHROUGH_FB):
                return "XR_ERROR_FEATURE_ALREADY_CREATED_PASSTHROUGH_FB";
                break;
            case(XR_ERROR_FEATURE_REQUIRED_PASSTHROUGH_FB):
                return "XR_ERROR_FEATURE_REQUIRED_PASSTHROUGH_FB";
                break;
            case(XR_ERROR_NOT_PERMITTED_PASSTHROUGH_FB):
                return "XR_ERROR_NOT_PERMITTED_PASSTHROUGH_FB";
                break;
            case(XR_ERROR_INSUFFICIENT_RESOURCES_PASSTHROUGH_FB):
                return "XR_ERROR_INSUFFICIENT_RESOURCES_PASSTHROUGH_FB";
                break;
            case(XR_ERROR_UNKNOWN_PASSTHROUGH_FB):
                return "XR_ERROR_UNKNOWN_PASSTHROUGH_FB";
                break;
            case(XR_ERROR_RENDER_MODEL_KEY_INVALID_FB):
                return "XR_ERROR_RENDER_MODEL_KEY_INVALID_FB";
                break;
            case(XR_RENDER_MODEL_UNAVAILABLE_FB):
                return "XR_RENDER_MODEL_UNAVAILABLE_FB";
                break;
            case(XR_ERROR_MARKER_NOT_TRACKED_VARJO):
                return "XR_ERROR_MARKER_NOT_TRACKED_VARJO";
                break;
            case(XR_ERROR_MARKER_ID_INVALID_VARJO):
                return "XR_ERROR_MARKER_ID_INVALID_VARJO";
                break;
            case(XR_ERROR_MARKER_DETECTOR_PERMISSION_DENIED_ML):
                return "XR_ERROR_MARKER_DETECTOR_PERMISSION_DENIED_ML";
                break;
            case(XR_ERROR_MARKER_DETECTOR_LOCATE_FAILED_ML):
                return "XR_ERROR_MARKER_DETECTOR_LOCATE_FAILED_ML";
                break;
            case(XR_ERROR_MARKER_DETECTOR_INVALID_DATA_QUERY_ML):
                return "XR_ERROR_MARKER_DETECTOR_INVALID_DATA_QUERY_ML";
                break;
            case(XR_ERROR_MARKER_DETECTOR_INVALID_CREATE_INFO_ML):
                return "XR_ERROR_MARKER_DETECTOR_INVALID_CREATE_INFO_ML";
                break;
            case(XR_ERROR_MARKER_INVALID_ML):
                return "XR_ERROR_MARKER_INVALID_ML";
                break;
            case(XR_ERROR_LOCALIZATION_MAP_INCOMPATIBLE_ML):
                return "XR_ERROR_LOCALIZATION_MAP_INCOMPATIBLE_ML";
                break;
            case(XR_ERROR_LOCALIZATION_MAP_UNAVAILABLE_ML):
                return "XR_ERROR_LOCALIZATION_MAP_UNAVAILABLE_ML";
                break;
            case(XR_ERROR_LOCALIZATION_MAP_FAIL_ML):
                return "XR_ERROR_LOCALIZATION_MAP_FAIL_ML";
                break;
            case(XR_ERROR_LOCALIZATION_MAP_IMPORT_EXPORT_PERMISSION_DENIED_ML):
                return "XR_ERROR_LOCALIZATION_MAP_IMPORT_EXPORT_PERMISSION_DENIED_ML";
                break;
            case(XR_ERROR_LOCALIZATION_MAP_PERMISSION_DENIED_ML):
                return "XR_ERROR_LOCALIZATION_MAP_PERMISSION_DENIED_ML";
                break;
            case(XR_ERROR_LOCALIZATION_MAP_ALREADY_EXISTS_ML):
                return "XR_ERROR_LOCALIZATION_MAP_ALREADY_EXISTS_ML";
                break;
            case(XR_ERROR_LOCALIZATION_MAP_CANNOT_EXPORT_CLOUD_MAP_ML):
                return "XR_ERROR_LOCALIZATION_MAP_CANNOT_EXPORT_CLOUD_MAP_ML";
                break;
            case(XR_ERROR_SPATIAL_ANCHOR_NAME_NOT_FOUND_MSFT):
                return "XR_ERROR_SPATIAL_ANCHOR_NAME_NOT_FOUND_MSFT";
                break;
            case(XR_ERROR_SPATIAL_ANCHOR_NAME_INVALID_MSFT):
                return "XR_ERROR_SPATIAL_ANCHOR_NAME_INVALID_MSFT";
                break;
            case(XR_SCENE_MARKER_DATA_NOT_STRING_MSFT):
                return "XR_SCENE_MARKER_DATA_NOT_STRING_MSFT";
                break;
            case(XR_ERROR_SPACE_MAPPING_INSUFFICIENT_FB):
                return "XR_ERROR_SPACE_MAPPING_INSUFFICIENT_FB";
                break;
            case(XR_ERROR_SPACE_LOCALIZATION_FAILED_FB):
                return "XR_ERROR_SPACE_LOCALIZATION_FAILED_FB";
                break;
            case(XR_ERROR_SPACE_NETWORK_TIMEOUT_FB):
                return "XR_ERROR_SPACE_NETWORK_TIMEOUT_FB";
                break;
            case(XR_ERROR_SPACE_NETWORK_REQUEST_FAILED_FB):
                return "XR_ERROR_SPACE_NETWORK_REQUEST_FAILED_FB";
                break;
            case(XR_ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB):
                return "XR_ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB";
                break;
            case(XR_ERROR_PASSTHROUGH_COLOR_LUT_BUFFER_SIZE_MISMATCH_META):
                return "XR_ERROR_PASSTHROUGH_COLOR_LUT_BUFFER_SIZE_MISMATCH_META";
                break;
            case(XR_ERROR_HINT_ALREADY_SET_QCOM):
                return "XR_ERROR_HINT_ALREADY_SET_QCOM";
                break;
            case(XR_ERROR_NOT_AN_ANCHOR_HTC):
                return "XR_ERROR_NOT_AN_ANCHOR_HTC";
                break;
            case(XR_ERROR_SPACE_NOT_LOCATABLE_EXT):
                return "XR_ERROR_SPACE_NOT_LOCATABLE_EXT";
                break;
            case(XR_ERROR_PLANE_DETECTION_PERMISSION_DENIED_EXT):
                return "XR_ERROR_PLANE_DETECTION_PERMISSION_DENIED_EXT";
                break;
            case(XR_RESULT_MAX_ENUM):
                return "XR_RESULT_MAX_ENUM";
                break;
            default:
                return "TOSTR_XRRESULT_ERROR";

        }

    }

}
