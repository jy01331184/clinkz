//
// Created by tianyang on 2018/11/13.
//

#ifndef NATIVE_RUBICK_H
#define NATIVE_RUBICK_H

#include "jni.h"
#include "android/log.h"
extern jclass clinkzClass;
extern jmethodID onMonitorEnterMethod;
extern JavaVM *javaVM;

extern void initClinkz(JNIEnv * env);
extern int hook(char *libSo, char *targetSymbol, void *newFunc, void **oldFunc);
extern long find(char *libSo, char *targetSymbol);

extern JNIEnv* getEnv();

#define  CLINKZ_LOG_TAG    "clinkz"
#define  CLINKZ_LOG(...)  __android_log_print(ANDROID_LOG_DEBUG,CLINKZ_LOG_TAG,__VA_ARGS__)


#endif //NATIVE_RUBICK_H

