////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   engine.cpp
//  Version:     v1.00
//  Created:     20/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#include "engine.h"
#include "log.h"
#include "base.h"
#include "vr.h"
#include "jni.h"
#include "lib/android_native_app_glue.h"

//TODO(clara): Remove this after we are done using usleep in the loop !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <unistd.h>

using namespace DS;

extern "C" {
    JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
        engineConfig::JavaVMPtr = vm;
        vm->AttachCurrentThread((JNIEnv**)&engineConfig::JNIEnvPtr, NULL);

    }

}

static void ds_android_handle_event(android_app* AppPtr, i32 Cmd) {
    switch (Cmd) {
        case APP_CMD_INIT_WINDOW:

            break;

        default:
            DSLOG_ERROR(Engine, "Got unhandled event from android, event %i", Cmd);
            break;

    }

}

void ds_main_android(android_app* AndroidAppPtr) {
    engineConfig::AppPtr = AndroidAppPtr;

    AndroidAppPtr->onAppCmd = ds_android_handle_event;


    ds_main();

}

void ds_main(){
    DSLOG_INFO(Engine, "Starting QuestDS Engine now !!!!!!!!");

    job_internalinit();
    oxrEarlyInit();

}

void ds_loop() {


}
