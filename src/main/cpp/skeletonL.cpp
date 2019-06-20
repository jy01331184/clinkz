//
// Created by tianyang on 2018/11/13.
//
#include <string>
#include "clinkz.h"
#include "jni.h"
#include "skeleton.h"

jclass methodClass;
jmethodID methodConstructor;

void *MonitorEnterL(void* thread,void* lockObj){

    size_t objectAddr = reinterpret_cast<size_t>(lockObj);
    size_t threadAddr = reinterpret_cast<size_t>(thread);
    MonitorInfo monitorInfo;
    monitorInfo.threadAddr = threadAddr;
    monitorInfo.objectAddr = objectAddr;
//    CLINKZ_LOG("newMonitorEnter obj = %d thread = %d",objectAddr,threadAddr);
    double start = getCurrentTime();
    void * result = oldMonitorEnter(thread,lockObj);
    double end = getCurrentTime();
    jfloat cost = end-start;
    JNIEnv *env = getEnv();

    if(env){
        std::set<MonitorInfo,MonitorInfoEq>::iterator iter = skeletonFind(monitorInfo);
        if(iter == records.end()){
            void * currentMethod = get_current_method_function(thread, 0,true);
            if(currentMethod){
                skeletonInsert(monitorInfo);
                jobject artMethod = add_local_function(env,currentMethod);
                jobject jmethod = env->NewObject(methodClass,methodConstructor,artMethod);
                jobject weakObj = add_local_function(env,lockObj);
                env->CallStaticVoidMethod(clinkzClass,onMonitorEnterMethod,jmethod,weakObj,cost);
                env->DeleteLocalRef(weakObj);
                env->DeleteLocalRef(artMethod);
                env->DeleteLocalRef(jmethod);
                skeletonErase(monitorInfo);
            }
        }
    }
    return result;
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_clinkz_Clinkz_initNativeL(JNIEnv *env, jobject type) {
    initClinkz(env);
    initSkeleton();

    methodClass = (jclass)(env->NewGlobalRef(env->FindClass("java/lang/reflect/Method")));
    methodConstructor = env->GetMethodID(methodClass,"<init>","(Ljava/lang/reflect/ArtMethod;)V");

    long addr = find("libart.so","_ZNK3art6mirror6Object16IdentityHashCodeEv");
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

    addr = find("libart.so","_ZNK3art6Thread16GetCurrentMethodEPjb");
    if(!addr){
        CLINKZ_LOG("find getCurrentMethod function fail!");
        return JNI_FALSE;
    }
    get_current_method_function = (GET_CURRENT_METHOD_FUNCTION)(reinterpret_cast<void *>(addr));

    char * symbol = "_ZN3art7Monitor12MonitorEnterEPNS_6ThreadEPNS_6mirror6ObjectE";
    jint result = hook("libart.so",symbol,(void *)MonitorEnterL,(void **)&oldMonitorEnter);
    if(!result){
        CLINKZ_LOG("hook MonitorEnter fail!");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

