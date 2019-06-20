package com.clinkz;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class Utils {
    public static String getSig(Method method){
        try {
            Method getSigMethod = Method.class.getDeclaredMethod("getSignature", new Class[]{});
            getSigMethod.setAccessible(true);
            return (String) getSigMethod.invoke(method);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String getSig(Constructor method){
        try {
            System.out.println(method.getName()+":"+method.toGenericString());
            Method getSigMethod = Constructor.class.getDeclaredMethod("getSignature", new Class[]{});
            getSigMethod.setAccessible(true);
            return (String) getSigMethod.invoke(method);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return null;
    }
}
