package com.example.leek.my_usb;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;

public class AlertHandler extends Handler {

    public static final int MSG_WARNING = 0;

    Context context;
    Vibrator vibrator;

    public AlertHandler(Context context){
        this.context = context;
        vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
    }

    @Override
    public void handleMessage(Message msg) {
        super.handleMessage(msg);

        if (msg.what == MSG_WARNING) {
            vibrator.vibrate(200);
        }
    }
}
