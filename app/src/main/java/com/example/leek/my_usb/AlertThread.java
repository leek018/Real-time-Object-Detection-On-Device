package com.example.leek.my_usb;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.os.Vibrator;
import android.util.Log;
import android.speech.tts.TextToSpeech;

import java.util.Locale;


public class AlertThread extends Thread {

    /*
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
    */

    /***
     *
     * leek revised for tts
     */


    public enum TTS_STATE{
        NORMAL, WARNING_CENTER, WARNING_LEFT,WARNING_RIGHT,DANGEROUS
    }

    Context context;
    AlertHandler handler;
    TTS_STATE current_state = TTS_STATE.NORMAL;
    TTS_STATE before_state = TTS_STATE.NORMAL;
    TextToSpeech tts_obj;
    private String[] alert_string = {"계단이 앞에 있습니다","계단이 왼쪽에 있습니다","계단이 오른쪽에 있습니다","계단이 바로 앞에 있습니다"};

    public AlertThread (Context context) {
        this.context = context;
        tts_obj = new TextToSpeech(this.context, new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int status) {
                if(status != TextToSpeech.ERROR) {
                    tts_obj.setLanguage(Locale.KOREAN);
                }
            }
        });
    }

    @Override
    public synchronized void run() {
//        Looper.prepare();
//        handler = new AlertHandler(context);
//        Looper.loop();
        while(true) {
            //try {
                if( current_state != before_state ){
                    if (current_state == TTS_STATE.WARNING_CENTER ) {
                        if(!tts_obj.isSpeaking()){
                            ttsGreater21(alert_string[0]);
                            before_state = current_state;
                        }
                    } else if (current_state == TTS_STATE.WARNING_LEFT) {
                        if(!tts_obj.isSpeaking()){
                            ttsGreater21(alert_string[1]);
                            before_state = current_state;
                        }
                    }else if(current_state == TTS_STATE.WARNING_RIGHT){
                        if(!tts_obj.isSpeaking()){
                            ttsGreater21(alert_string[2]);
                            before_state = current_state;
                        }
                    }else if(current_state == TTS_STATE.DANGEROUS){
                        if(!tts_obj.isSpeaking()){
                            ttsGreater21(alert_string[3]);
                            before_state = current_state;
                        }
                    }else{
                        before_state = current_state;
                    }
                }
                //below statement is commented for some reason
                //before_state = current_state;
        }
    }


    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private  void ttsGreater21(String text) {
        String utteranceId=this.hashCode() + "";
        tts_obj.speak(text, TextToSpeech.QUEUE_FLUSH, null, utteranceId);
    }

    public void setState(TTS_STATE state) {
        this.current_state = state;
    }

}
