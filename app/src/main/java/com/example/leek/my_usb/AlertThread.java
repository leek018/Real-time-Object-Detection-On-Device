package com.example.leek.my_usb;

import android.content.Context;
import android.os.Vibrator;
import android.util.Log;


public class AlertThread extends Thread {

    public enum State {
        NORMAL, WARNING, DANGEROUS
    }

    Context context;
    AlertHandler handler;
    Vibrator vibrator;
    State state = State.NORMAL;

    public AlertThread (Context context) {
        this.context = context;
        vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
    }

    @Override
    public void run() {
//        Looper.prepare();
//        handler = new AlertHandler(context);
//        Looper.loop();
        while(true) {
            try {
                if (state == State.WARNING) {
                    vibrator.vibrate(80);
                    sleep(500);
                } else if (state == State.DANGEROUS) {
                    vibrator.vibrate(50);
                    sleep(10);
                    vibrator.vibrate(50);
                    sleep(10);
                    vibrator.vibrate(50);
                    sleep(10);
                    vibrator.vibrate(20);
                    sleep(10);
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void setState(State state) {
        this.state = state;
    }
}
