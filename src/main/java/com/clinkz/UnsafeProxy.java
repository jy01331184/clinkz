package com.clinkz;


import android.os.Build;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

/**
 * Created by magic.yang on 17/3/20.
 */

public class UnsafeProxy {

    private static Class unsafeClass;
    private static Object unsafe;
    private static Method getIntVolatileMethod;
    private static Method putIntVolatileMethod;
    private static Method arrayBaseOffsetMethod;
    private static Method objectFieldOffsetMethod;
    private static Method getLongVolatileMethod;
    private static Method getObjectVolatileMethod;

    static {
        try {
            unsafeClass = Class.forName("sun.misc.Unsafe");
            if (Build.VERSION.SDK_INT >= 19) {
                Field theUnsafeInstance = unsafeClass.getDeclaredField("theUnsafe");
                theUnsafeInstance.setAccessible(true);
                unsafe = theUnsafeInstance.get(null);
            } else {
                Class AQSClass = Class.forName("java.util.concurrent.locks.AbstractQueuedSynchronizer");
                Field theUnsafeInstance = AQSClass.getDeclaredField("unsafe");
                theUnsafeInstance.setAccessible(true);
                unsafe = theUnsafeInstance.get(null);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    static {
        try {
            getIntVolatileMethod = unsafeClass.getDeclaredMethod("getIntVolatile", Object.class, long.class);
            getIntVolatileMethod.setAccessible(true);

            putIntVolatileMethod = unsafeClass.getDeclaredMethod("putIntVolatile", Object.class, long.class, int.class);
            putIntVolatileMethod.setAccessible(true);

            arrayBaseOffsetMethod = unsafeClass.getDeclaredMethod("arrayBaseOffset", Class.class);
            arrayBaseOffsetMethod.setAccessible(true);

            objectFieldOffsetMethod = unsafeClass.getDeclaredMethod("objectFieldOffset", Field.class);
            objectFieldOffsetMethod.setAccessible(true);

            getLongVolatileMethod = unsafeClass.getDeclaredMethod("getLongVolatile",Object.class, long.class);
            getLongVolatileMethod.setAccessible(true);

            getObjectVolatileMethod = unsafeClass.getDeclaredMethod("getObjectVolatile",Object.class,long.class);
            getObjectVolatileMethod.setAccessible(true);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static int getIntVolatile(Object var1, long var2) throws Exception {
        return ((Integer) getIntVolatileMethod.invoke(unsafe, var1, var2)).intValue();
    }

    public static void putIntVolatile(Object var1, long var2, int var4) throws Exception {
        putIntVolatileMethod.invoke(unsafe, var1, var2, var4);
    }

    public static int getIntVolatile(long var2) throws Exception {
        return ((Integer) getIntVolatileMethod.invoke(unsafe, null, var2)).intValue();
    }

    public static void putIntVolatile(long var2, int var4) throws Exception {
        putIntVolatileMethod.invoke(unsafe, null, var2, var4);
    }

    public static int getObjectAddress(Object o) throws Exception {
        Object[] objects = {o};
        int arrayBaseOffset = ((Integer) arrayBaseOffsetMethod.invoke(unsafe, Object[].class)).intValue();
        return getIntVolatile(objects, arrayBaseOffset);
    }

    public static int getArrayBaseOffset(Class cls) throws Exception{
        return ((Integer) arrayBaseOffsetMethod.invoke(unsafe, cls)).intValue();
    }

    public static long objectFieldOffset(Field field) throws Exception {
        return ((Long) objectFieldOffsetMethod.invoke(unsafe, field)).longValue();
    }

    public static void ensureClassInitialized(Class clazz) throws Exception {
        Class.forName(clazz.getName(), true, clazz.getClassLoader());
    }

    public static long getLongVolatile(long var2) throws Exception {
        return ((Long) getLongVolatileMethod.invoke(unsafe, null, var2)).longValue();
    }

    public static Object getObjectVolatile(long addr) throws Exception{
        return getObjectVolatileMethod.invoke(unsafe,null,addr);
    }

}
