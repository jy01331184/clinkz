package com.clinkz;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.lang.reflect.Method;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class MainActivity extends Activity {

    NumberFormat numberFormat = new DecimalFormat("00.00");
    ExecutorService service = Executors.newFixedThreadPool(4);
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        service.execute(new Runnable() {
            @Override
            public void run() {
                test456();
                test123();
            }
        });
        findViewById(R.id.btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean result = Clinkz.init(getApplicationContext(), new Clinkz.ClinkzListener() {
                    @Override
                    public void onMonitorEnter(Method method, Object object, float waitTime) {
                        Log.i("System.out","[" + Thread.currentThread().getName() + "] :" + method + " => " + object + " cost:" + numberFormat.format(waitTime));
                    }
                });
                System.out.println("Clinkz init:"+result);
            }
        });

        findViewById(R.id.btn2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                test456();
                test123();
            }
        });
        findViewById(R.id.con).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                for (int i = 0; i < 10; i++) {
                    service.execute(new Runnable() {
                        @Override
                        public void run() {
                            //test456();
                        }
                    });
                }

            }
        });
    }

    Random random = new Random();

    public void test456() {
        synchronized (random) {
            final String str;
            try {
                str = (""+ UnsafeProxy.getObjectAddress(random));
                Log.i("TEST222",str);
            } catch (Exception e) {
                e.printStackTrace();
            }

            //System.out.println("invoke test:" + Thread.currentThread().getName() + "-->" + hashCode());
        }

    }

    public void test123() {
        synchronized (random) {
            final String str = (""+random.nextInt(888));
            Log.i("TEST",str);
            //System.out.println("invoke test:" + Thread.currentThread().getName() + "-->" + hashCode());
        }

    }
}
