package com.clinkz;

import android.content.Context;
import android.os.Build;

import java.lang.reflect.Method;

public class Clinkz {

    static {
        System.loadLibrary("clinkz");
    }

    private static boolean init = false;
    private static ClinkzListener clinkzListener;

    public static boolean init(Context context, ClinkzListener listener) {
        if (init) {
            throw new IllegalStateException("Clinkz already inited");
        }
        init = true;

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            return false;
        } else {
            clinkzListener = listener;
            switch (Build.VERSION.SDK_INT){
                case Build.VERSION_CODES.LOLLIPOP:
                case Build.VERSION_CODES.LOLLIPOP_MR1:
                    return initNativeL();
                case Build.VERSION_CODES.M:
                case Build.VERSION_CODES.N:
                case Build.VERSION_CODES.N_MR1:
                    return initNativeM(Build.VERSION.SDK_INT);
                case Build.VERSION_CODES.O:
                case Build.VERSION_CODES.O_MR1:
                case Build.VERSION_CODES.P:
                    return initNativeO(Build.VERSION.SDK_INT);
            }
        }

        return false;
    }


    /**
     * called by clinkz c++
     */
    private static void nativeMonitorEnter(Method method, Object object, float waitTime) {
        if (clinkzListener != null) {
            clinkzListener.onMonitorEnter(method, object, waitTime);
        }
    }

    private static native boolean initNativeL();

    private static native boolean initNativeM(int sdkVersion);

    private static native boolean initNativeO(int sdkVersion);

    interface ClinkzListener {
        void onMonitorEnter(Method method, Object object, float waitTime);
    }
}
