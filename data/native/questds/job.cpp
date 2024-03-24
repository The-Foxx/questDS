////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   job.cpp
//  Version:     v1.00
//  Created:     23/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#include "job.h"
#include "pthread.h"

namespace DS{
    struct jobThread {
        u32 InternalId;
        jobThreadPriority Priotity;
        pthread_t SysThread;
        pthread_attr_t SysThreadAttr;

        void createThread();
        void startThread();
        static void* threadFunc(void* Data);

    };

    static dmem<jobThread> Workers{};

    void jobThread::startThread() {
        i32 AttrCode = pthread_attr_init(&SysThreadAttr);
        if (AttrCode != 0) {
            DSLOG_ERROR(Job, "Failed to create pthread attributes for worker %u", InternalId);

        }

//        TODO(clara): Fix this, this causes crashes for some reason !!!!!!!!!!!!!!!!!!!!!!!!
        i32 ReturnCode = 0; //pthread_create(&SysThread, &SysThreadAttr, threadFunc, this);
        if (ReturnCode != 0) {
            DSLOG_ERROR(Job, "Failed to create worker thread %u with pthread_create returning %i", InternalId, ReturnCode);

        }

        DSLOG_INFO(Job, "Started worker %u", InternalId);

    }

    void jobThread::createThread() {
        DSLOG_INFO(Job, "Initializing thread number %u", InternalId);
        if (Priotity == jobThreadPriority::Low) {
            DSLOG_INFO(Job, "Thread has low priority");

        }
        else if (Priotity == jobThreadPriority::High) {
            DSLOG_INFO(Job, "Thread has High priotiry !!");

        }
        else {
            DSLOG_ERROR(Job, "Could not determine adequate thread priority");

        }

    }

    void* jobThread::threadFunc(void* Data) {

        return NULL;

    }

    void job_internalinit() {
        DSLOG_INFO(Job, "Initializing job system");

        for (u32 i = 0; i < 2; i++) {
            jobThread Worker{};
            Worker.InternalId = Workers.size();
            Worker.Priotity = jobThreadPriority::High;

            Workers.append(Worker);
            Workers.last()->createThread();

        }

        for (u32 i = 0; i < 2; i++) {
            jobThread Worker{};
            Worker.InternalId = Workers.size();
            Worker.Priotity = jobThreadPriority::Low;

            Workers.append(Worker);
            Workers.last()->createThread();

        }

        for (u32 i = 0; i < Workers.size(); i++) {
            Workers.getPtr(i)->startThread();

        }

    }

}
