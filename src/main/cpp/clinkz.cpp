//
// Created by tianyang on 2018/11/14.
//
#include <dlfcn.h>
#include <cstdio>
#include "clinkz.h"
JavaVM *javaVM;
jclass clinkzClass;
jmethodID onMonitorEnterMethod;
typedef long (*PUDGE_FIND_FUNCTION)(char *libSo, char *targetSymbol);
PUDGE_FIND_FUNCTION pudgeFindFunction = 0;
typedef int (*PUDGE_HOOK_FUNCTION)(char *libSo, char *targetSymbol, void *newFunc, void **oldFunc);
PUDGE_HOOK_FUNCTION pudgeHookFunction = 0;
typedef int (*PUDGE_SEARCH_FUNCTION)(int addr, int target, int maxSearch);
PUDGE_SEARCH_FUNCTION pudgeSearchFunction = 0;

const char * PUDGE_SO = "libpudge.so";

void initClinkz(JNIEnv *env) {
    void *handle = dlopen(PUDGE_SO, RTLD_LAZY);
    if (handle) {
        CLINKZ_LOG("initClinkz dlopen pudge success");
        pudgeFindFunction = (PUDGE_FIND_FUNCTION)dlsym(handle, "_ZN5pudge15findFuncAddressEPcS0_");
        pudgeHookFunction = (PUDGE_HOOK_FUNCTION)dlsym(handle, "_ZN5pudge12hookFunctionEPcS0_PvPS1_");
        pudgeSearchFunction = (PUDGE_SEARCH_FUNCTION)dlsym(handle, "_ZN5pudge6searchEiii");
        CLINKZ_LOG("pudgeHookFunction %p,pudgeSearchFunction %p",pudgeHookFunction,pudgeSearchFunction);
    } else {
        CLINKZ_LOG("initClinkz dlopen pudge fail");
    }

    clinkzClass = static_cast<jclass>(env->NewGlobalRef(env->FindClass("com/clinkz/Clinkz")));
    onMonitorEnterMethod = env->GetStaticMethodID(clinkzClass,"nativeMonitorEnter","(Ljava/lang/reflect/Method;Ljava/lang/Object;F)V");
}

int hook(char *libSo, char *targetSymbol, void *newFunc, void **oldFunc) {
    if (pudgeHookFunction) {
        return pudgeHookFunction(libSo, targetSymbol, newFunc, oldFunc);
    }
    return 0;
}

long find(char *libSo, char *targetSymbol){
    if(pudgeFindFunction){
        return pudgeFindFunction(libSo,targetSymbol);
    }
    return 0;
}

JNIEnv *getEnv() {
    JNIEnv *env;
    if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        javaVM->AttachCurrentThread(&env, 0);
    }
    return env;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    JNIEnv *env = NULL;
    javaVM = vm;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_6;
}

