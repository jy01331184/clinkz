//
// Created by tianyang on 2018/11/13.
//
#include <string>
#include "clinkz.h"
#include "jni.h"
#include "skeleton.h"

void *(*oldMonitorEnterO)(void* thread,void* lockObj,bool tryLock);

void *MonitorEnterO(void* thread,void* lockObj,bool tryLock){
    size_t objectAddr = reinterpret_cast<size_t>(lockObj);
    size_t threadAddr = reinterpret_cast<size_t>(thread);
    MonitorInfo monitorInfo;
    monitorInfo.threadAddr = threadAddr;
    monitorInfo.objectAddr = objectAddr;
//    CLINKZ_LOG("newMonitorEnter obj = %d thread = %d",objectAddr,threadAddr);
    double start = getCurrentTime();
    void * result = oldMonitorEnterO(thread,lockObj,tryLock);
    double end = getCurrentTime();
    jfloat cost = end-start;
    JNIEnv *env = getEnv();

    if(env){
        std::set<MonitorInfo,MonitorInfoEq>::iterator iter = skeletonFind(monitorInfo);
        if(iter == records.end()){
            void * currentMethod = get_current_method_function(thread, 0,true);
            if(currentMethod){
                skeletonInsert(monitorInfo);
                void * method = create_jmethod_function(thread,currentMethod);
                jobject jmethod = add_local_function(env,method);
                jobject weakObj = add_local_function(env,lockObj);
                env->CallStaticVoidMethod(clinkzClass,onMonitorEnterMethod,jmethod,weakObj,cost);
                env->DeleteLocalRef(weakObj);
                env->DeleteLocalRef(jmethod);
                skeletonErase(monitorInfo);
            }
        }
    }
    return result;
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_clinkz_Clinkz_initNativeO(JNIEnv *env, jobject type,jint sdkVersion) {
    initClinkz(env);
    initSkeleton();

    long addr = 0;
    addr = find("libart.so","_ZN3art6mirror6Object16IdentityHashCodeEv");
    if(!addr){
        CLINKZ_LOG("find hashcode function fail!");
        return JNI_FALSE;
    }
    hash_function = (HASH_FUNCTION)(reinterpret_cast<void *>(addr));

    addr = find("libart.so","_ZN3art9JNIEnvExt11NewLocalRefEPNS_6mirror6ObjectE");
    if(!addr){
        CLINKZ_LOG("find newLocalRef function fail!");
        return JNI_FALSE;
    }
    add_local_function = (ADD_LOCAL_FUNCTION)(reinterpret_cast<void *>(addr));

    addr = find("libart.so","_ZNK3art6Thread16GetCurrentMethodEPjbb");
    if(!addr){
        CLINKZ_LOG("find getCurrentMethod function fail!");
        return JNI_FALSE;
    }
    get_current_method_function = (GET_CURRENT_METHOD_FUNCTION)(reinterpret_cast<void *>(addr));

    if(sizeof(void *) == 4){
        addr = find("libart.so","_ZN3art6mirror6Method19CreateFromArtMethodILNS_11PointerSizeE4ELb0EEEPS1_PNS_6ThreadEPNS_9ArtMethodE");
    } else {
        addr = find("libart.so","_ZN3art6mirror6Method19CreateFromArtMethodILNS_11PointerSizeE8ELb0EEEPS1_PNS_6ThreadEPNS_9ArtMethodE");
    }
    if(!addr){
        CLINKZ_LOG("find CreateFromArtMethod function fail!");
        return JNI_FALSE;
    }
    create_jmethod_function = (CREATE_JMETHOD_FUNCTION)(reinterpret_cast<void *>(addr));

    char * symbol = "_ZN3art7Monitor12MonitorEnterEPNS_6ThreadEPNS_6mirror6ObjectEb";
    jint result = hook("libart.so",symbol,(void *)MonitorEnterO,(void **)&oldMonitorEnterO);
    if(!result){
        CLINKZ_LOG("hook MonitorEnter fail!");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

