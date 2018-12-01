package com.example.leek.my_usb;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.hardware.usb.UsbDevice;
import android.os.Build;
import android.os.Environment;
import android.speech.tts.TextToSpeech;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.widget.Toast;
import com.jiangdg.usbcamera.UVCCameraHelper;
import com.jiangdg.usbcamera.utils.FileUtils;
import com.serenegiant.usb.common.AbstractUVCCameraHandler;
import com.serenegiant.usb.widget.CameraViewInterface;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;


public class MainActivity extends AppCompatActivity implements CameraViewInterface.Callback {
    private static final String TAG = "Debug";

    /*
    private Thread tts_thread;
    private boolean tts_thread_started = false;
    private boolean feed_bbox_thread_statred = false;
    private boolean program_running = true;

    final int READY_TO_FEED = 0;
    final int SERVICE_CALL_DONE = 1;
    final int TTS_DONE = 2;
    final int WEAK_STATE = 0;
    final int STRONG_STATE = 1;
    final int NORMAL_STATE = 2;
    final int READY_STATE = 3;

    private TextToSpeech tts_object;
    public static String weak_sentence  = "계단이 앞쪽에 있습니다";
    public static String strong_sentence = "계단이 진행방향 바로 앞쪽에 있습니다";
    */

    final int cam_width = 1920;
    final int cam_height = 1080;

    public View mTextureView;
    private UVCCameraHelper mCameraHelper;
    private CameraViewInterface mUVCCameraView;
    private boolean isRequest;
    private boolean isPreview;
    private static final int PERMISSION_REQUEST_CODE = 1;

    String model_name = "mssd_300";
    String model_path ;
    String proto_path = model_path = "/sdcard/saved_images/";
    String device_type = "acl_opencl";

    //temp
    int i = 0;

    static {
        System.loadLibrary("detect-lib");
    }





    private UVCCameraHelper.OnMyDevConnectListener listener = new UVCCameraHelper.OnMyDevConnectListener() {

        @Override
        public void onAttachDev(UsbDevice device) {
            if (mCameraHelper == null || mCameraHelper.getUsbDeviceCount() == 0) {
                showShortMsg("check no usb camera");
                return;
            }
            // request open permission
            if (!isRequest) {
                isRequest = true;
                if (mCameraHelper != null) {
                    mCameraHelper.requestPermission(0);
                }
            }
        }

        @Override
        public void onDettachDev(UsbDevice device) {
            // close camera
            if (isRequest) {
                isRequest = false;
                mCameraHelper.closeCamera();
                showShortMsg(device.getDeviceName() + " is out");
            }
        }

        @Override
        public void onConnectDev(UsbDevice device, boolean isConnected) {
            if (!isConnected) {
                showShortMsg("fail to connect,please check resolution params");
                isPreview = false;
            } else {
                isPreview = true;
                showShortMsg("connecting");

            }
        }

        @Override
        public void onDisConnectDev(UsbDevice device) {
            showShortMsg("disconnecting");
        }

    };


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i("onCreate","1");
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
//                WindowManager.LayoutParams.FLAG_FULLSCREEN);
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
//                WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.activity_main);


        if (Build.VERSION.SDK_INT >= 23)
        {
            if (!checkPermission(MainActivity.this))
            {
                    requestPermission(MainActivity.this); // Code for permission
            }
        }



        boolean create_result = DetectManager.get_graph_space(model_name,model_path+"MobileNetSSD_deploy.caffemodel",proto_path+"MobileNetSSD_deploy.prototxt",device_type);
        if(!create_result )
            showShortMsg("create graph failed");

        // step.1 initialize UVCCameraHelper
        mUVCCameraView = (CameraViewInterface) mTextureView;
        mUVCCameraView = (CameraViewInterface) findViewById(R.id.camera_view);
        mUVCCameraView.setCallback(this);
        mCameraHelper = UVCCameraHelper.getInstance();
        mCameraHelper.setDefaultFrameFormat(UVCCameraHelper.FRAME_FORMAT_MJPEG);
        mCameraHelper.initUSBMonitor(this, mUVCCameraView, listener);
        mCameraHelper.updateResolution(1920, 1080);



        mCameraHelper.setOnPreviewFrameListener(new AbstractUVCCameraHandler.OnPreViewResultListener() {
            @Override
            public void onPreviewResult(byte[] nv21Yuv) {
                boolean result = DetectManager.detect(nv21Yuv,1920,1080);
                if( result == false)
                    Log.i("error"," in obstacle");
                float[] dum = new float[1000];
                 DetectManager.get_out_data(dum);
                Log.i("num",""+dum[0]);
                DetectManager.delete_out_data();

            }
        });

//        tts_thread = new Thread(new Runnable() {
//            int case_;
//            @Override
//            public void run() {
//                Log.i("thread2","I'm in tts_thread");
//                case_ = ObstacleManager.get_warning();
//                switch (case_){
//                    case WEAK_STATE:
//                        try {
//                            ttsGreater21(weak_sentence);
//                            tts_thread.join();
//                        } catch (InterruptedException e) {
//                            e.printStackTrace();
//                        }
//                        break;
//                    case STRONG_STATE:
//                        try {
//                            ttsGreater21(strong_sentence);
//                            tts_thread.join();
//                        } catch (InterruptedException e) {
//                            e.printStackTrace();
//                        }
//                        break;
//                    default:
//                        break;
//                }
//                //mMainHandler.sendEmptyMessage(TTS_DONE);
//            }
//        });
//
//        tts_object = new TextToSpeech(getApplicationContext(), new TextToSpeech.OnInitListener() {
//            @Override
//            public void onInit(int status) {
//                if(status != TextToSpeech.ERROR) {
//                    tts_object.setLanguage(Locale.KOREAN);
//                }
//            }
//        });
    }


    @Override
    protected void onStart() {
        super.onStart();
        Log.i("onStart","3");
        // step.2 register USB event broadcast
        if (mCameraHelper != null) {
            mCameraHelper.registerUSB();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.i("onStop","4");
        // step.3 unregister USB event broadcast
        if (mCameraHelper != null) {
            mCameraHelper.unregisterUSB();
        }
        DetectManager.delete_out_data();

    }



    @Override
    protected void onDestroy() {
        super.onDestroy();
        FileUtils.releaseFile();
        // step.4 release uvc camera resources
        if (mCameraHelper != null) {
            mCameraHelper.release();
        }
        DetectManager.delete_out_data();
        DetectManager.delete_graph_space();
    }

    private void showShortMsg(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }


    @Override
    public void onSurfaceCreated(CameraViewInterface view, Surface surface) {
        if (!isPreview && mCameraHelper.isCameraOpened()) {
            mCameraHelper.startPreview(mUVCCameraView);
            isPreview = true;
        }
    }

    @Override
    public void onSurfaceChanged(CameraViewInterface view, Surface surface, int width, int height) {
        Log.i("Surface", "capture~~~ ");
        //Bitmap bmp = view.captureStillImage(0,100);
    }

    @Override
    public void onSurfaceDestroy(CameraViewInterface view, Surface surface) {
        if (isPreview && mCameraHelper.isCameraOpened()) {
            mCameraHelper.stopPreview();
            isPreview = false;
        }
    }
    /*
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void ttsGreater21(String text) {
        String utteranceId=this.hashCode() + "";
        tts_object.speak(text, TextToSpeech.QUEUE_FLUSH, null, utteranceId);
    }
    */


    private boolean checkPermission(Context context) {//
        int result = ContextCompat.checkSelfPermission(context, android.Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (result == PackageManager.PERMISSION_GRANTED) {
            return true;
        } else {
            return false;
        }
    }
    private void requestPermission(Activity activity) {

        if (ActivityCompat.shouldShowRequestPermissionRationale(activity, android.Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            Toast.makeText(activity, "Write External Storage permission allows us to do store images. Please allow this permission in App Settings.", Toast.LENGTH_LONG).show();
        } else {
            ActivityCompat.requestPermissions(activity, new String[]{android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, PERMISSION_REQUEST_CODE);
        }
    }
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_CODE:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Log.e("value", "Permission Granted, Now you can use local drive .");
                } else {
                    Log.e("value", "Permission Denied, You cannot use local drive .");
                }
                break;
        }
    }

}
