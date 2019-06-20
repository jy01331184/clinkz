//
// Created by tianyang on 2018/11/13.
//
#ifndef NATIVE_SKELETON_H
#define NATIVE_SKELETON_H

#include <cstdio>
#include <pthread.h>
#include "set"
#include "jni.h"

enum SizeShiftsAndMasks {  // private marker to avoid generate-operator-out.py from processing.
    // Number of bits to encode the state, currently just fat or thin/unlocked or hash code.
    kStateSize = 2,
    kReadBarrierStateSize = 1,
    kMarkBitStateSize = 1,
    // Number of bits to encode the thin lock owner.
            kThinLockOwnerSize = 16,
    // Remaining bits are the recursive lock count.
            kThinLockCountSize = 32 - kThinLockOwnerSize - kStateSize - kReadBarrierStateSize -
                                 kMarkBitStateSize,
    // Thin lock bits. Owner in lowest bits.

    kThinLockOwnerShift = 0,

    kThinLockCountShift = kThinLockOwnerSize + kThinLockOwnerShift,

    // State in the highest bits.
    kStateShift = kReadBarrierStateSize + kThinLockCountSize + kThinLockCountShift + kMarkBitStateSize,
    kStateMask = (1 << kStateSize) - 1,
    kStateThinOrUnlocked = 0,
    kStateFat = 1,
    kStateHash = 2,
    kStateForwardingAddress = 3
    // Forwarding address shift.
};
enum LockState {
    kUnlocked = 11,    // No lock owners.
    kThinLocked = 12,  // Single uncontended owner.
    kFatLocked = 13,   // See associated monitor.
    kHashCode = 14,    // Lock word contains an identity hash.
    kForwardingAddress = 15,  // Lock word contains the forwarding address of an object.
};

static uint32_t getState(uint32_t value) {
    if (value == 0) {
        return kUnlocked;
    }
    uint32_t internal_state = (value >> kStateShift) & kStateMask;
    switch (internal_state) {
        case kStateThinOrUnlocked:
            return kThinLocked;
        case kStateHash:
            return kHashCode;
        case kStateForwardingAddress:
            return kForwardingAddress;
        default:
            return kFatLocked;
    }
}



static void *(*oldMonitorEnter)(void* thread,void* lockObj);

typedef jobject (*ADD_LOCAL_FUNCTION)(void* env,void* obj);
static ADD_LOCAL_FUNCTION add_local_function;

typedef void* (*GET_CURRENT_METHOD_FUNCTION)(void* thread,void* dex_pc,bool abort_on_error);
static GET_CURRENT_METHOD_FUNCTION get_current_method_function;

typedef jint (*HASH_FUNCTION)(void *obj);
static HASH_FUNCTION hash_function;

typedef void* (*CREATE_JMETHOD_FUNCTION)(void* thread,void* method);
static CREATE_JMETHOD_FUNCTION create_jmethod_function;

typedef struct _MonitorInfo{
    size_t objectAddr;
    size_t threadAddr;
} MonitorInfo;

struct MonitorInfoEq {
    bool operator() (const MonitorInfo &lhs, const MonitorInfo &rhs) const {
        return (lhs.objectAddr + lhs.threadAddr) < (rhs.objectAddr + rhs.threadAddr);
    }
};
static pthread_mutex_t pthreadMutex;

static double getCurrentTime(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000.0;
}
static std::set<MonitorInfo,MonitorInfoEq> records;

static std::set<MonitorInfo,MonitorInfoEq>::iterator skeletonFind(MonitorInfo& monitorInfo){
    pthread_mutex_lock(&pthreadMutex);
    std::set<MonitorInfo,MonitorInfoEq>::iterator iter = records.find(monitorInfo);
    pthread_mutex_unlock(&pthreadMutex);
    return iter;
};

static void skeletonInsert(MonitorInfo& monitorInfo){
    pthread_mutex_lock(&pthreadMutex);
    records.insert(monitorInfo);
    pthread_mutex_unlock(&pthreadMutex);
}

static void skeletonErase(MonitorInfo& monitorInfo){
    pthread_mutex_lock(&pthreadMutex);
    records.erase(monitorInfo);
    pthread_mutex_unlock(&pthreadMutex);
}

static void initSkeleton(){
    pthread_mutexattr_t pthreadMutexattr;
    pthread_mutexattr_settype(&pthreadMutexattr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&pthreadMutex,&pthreadMutexattr);
}

#endif //NATIVE_SKELETON_H