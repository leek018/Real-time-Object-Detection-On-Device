package com.example.leek.my_usb;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.hardware.usb.UsbDevice;
import android.os.Build;
import android.os.Environment;

import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
//import android.widget.EditText;
import android.widget.ImageView;
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



    public View mTextureView;
    private UVCCameraHelper mCameraHelper;
    private CameraViewInterface mUVCCameraView;
    private boolean isRequest;
    private boolean isPreview;
    private static final int PERMISSION_REQUEST_CODE = 1;

    private ImageView mImageView;
    private static Bitmap bitmap;
    private static Paint  paint;
    private static Canvas canvas;
    private int p_width = 2076;
    private int p_height = 1080;
    private int cam_width = 640;
    private int cam_height = 480;
    private float[] dum = new float[1000];


    String model_name = "mssd_300";
    //voc model path
    //stair model path "/sdcard/saved_images/stair_model"
    String path_prefix = "/sdcard/saved_images/";
    String model_path = path_prefix + "MobileNetSSD_deploy.caffemodel";
    String proto_path = path_prefix + "MobileNetSSD_deploy.prototxt";
    String device_type = "acl_opencl";

    AlertThread alertThread;

    //EditText edit;

    //temp
    static long start, end;
    static long e2e_start, e2e_end;
    static long timer[] = new long[10];
    static int OBS_INDEX = 2;// bicycle == 2, stair == 1


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
    /*
    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            edit.setText(edit.getText()+String.valueOf(msg.what));

        }
    };
    */

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i("onCreate","1");


        setContentView(R.layout.activity_main);
//        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        //edit = findViewById(R.id.edit);

        if (Build.VERSION.SDK_INT >= 23)
        {
            if (!checkPermission(MainActivity.this))
            {
                    requestPermission(MainActivity.this); // Code for permission
            }
        }



        boolean create_result = DetectManager.get_graph_space(model_name,model_path,proto_path,device_type);
        if(!create_result )
            showShortMsg("create graph failed");

        // To draw and show BBox
        mImageView = findViewById(R.id.image_view);
        bitmap = Bitmap.createBitmap(p_width, p_height, Bitmap.Config.ARGB_8888);
        mImageView.setImageBitmap(bitmap);

        canvas = new Canvas(bitmap);
        paint = new Paint();
        paint.setColor(Color.RED);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(3);


        // step.1 initialize UVCCameraHelper
        mUVCCameraView = (CameraViewInterface) mTextureView;
        mUVCCameraView = (CameraViewInterface) findViewById(R.id.camera_view);
        mUVCCameraView.setCallback(this);
        mCameraHelper = UVCCameraHelper.getInstance();
        mCameraHelper.setDefaultFrameFormat(UVCCameraHelper.FRAME_FORMAT_MJPEG);
        mCameraHelper.initUSBMonitor(this, mUVCCameraView, listener);

//        mCameraHelper.updateResolution(300, 300);
//        mCameraHelper.updateResolution(p_width, p_height);

        mCameraHelper.setOnPreviewFrameListener(new AbstractUVCCameraHandler.OnPreViewResultListener() {
            @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
            @Override
            public void onPreviewResult(byte[] nv21Yuv) {


                start = System.currentTimeMillis();
                // Detect BBox
                boolean result = DetectManager.detect(nv21Yuv,
                                    mCameraHelper.getPreviewWidth(),
                                    mCameraHelper.getPreviewHeight());

                if( result == false)
                    Log.i("error"," in obstacle");

                 DetectManager.get_out_data(dum);

                end = System.currentTimeMillis();
                timer[1] = end - start;  // Detect
                Log.i("night > Detect",  ""+timer[1]);

                float x1, y1, x2, y2;
                int n = (int)dum[0];

                start = System.currentTimeMillis();
                // Draw BBox

                canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
				for (int i=0; i<dum[0]; i++) {
                    /*
                    if ((int)dum[1 + i * 6] == 1) {
                        int state = (int) dum[1 + i * 6 + 1];
                        if (state == 0) {
                            paint.setColor(Color.WHITE);
                            alertThread.setState(AlertThread.State.NORMAL);
                        } else if (state == 1 || state == 2 || state == 3) {
                            paint.setColor(Color.YELLOW);
                            alertThread.setState(AlertThread.State.WARNING);
                        } else {
                            paint.setColor(Color.RED);
                            alertThread.setState(AlertThread.State.DANGEROUS);
                        }
                     */
                    //leek revised for tts
                    if ((int)dum[1 + i * 6] == OBS_INDEX) {
                        int state = (int) dum[1 + i * 6 + 1];

                        //handler.sendEmptyMessage(state);

                        if (state == 0) {
                            paint.setColor(Color.WHITE);
                            alertThread.setState(AlertThread.TTS_STATE.NORMAL);
                        } else if (state == 1 ) {
                            paint.setColor(Color.YELLOW);
                            alertThread.setState(AlertThread.TTS_STATE.WARNING_CENTER);

                        }else if (state == 2 ) {
                            paint.setColor(Color.YELLOW);
                            alertThread.setState(AlertThread.TTS_STATE.WARNING_LEFT);

                        }else if (state == 3) {
                            paint.setColor(Color.YELLOW);
                            alertThread.setState(AlertThread.TTS_STATE.WARNING_RIGHT);

                        } else {
                            paint.setColor(Color.RED);
                            alertThread.setState(AlertThread.TTS_STATE.DANGEROUS);

                        }
                        // class, state, x1, y1, x2, y2
                        x1 = dum[1 + i*6 + 2] * mImageView.getHeight() / cam_height * cam_width;
                        y1 = dum[1 + i*6 + 3] * mImageView.getHeight();
                        x2 = dum[1 + i*6 + 4] * mImageView.getHeight() / cam_height * cam_width;
                        y2 = dum[1 + i*6 + 5] * mImageView.getHeight();

//    					canvas.drawRect(x1, y1, x2, y2, paint);
                        canvas.drawRoundRect(x1, y1, x2, y2, 15, 15, paint);

                        Log.i("night: stair found", "x1:"+dum[1 + i*6 + 2]+" y1:"+dum[1 + i*6 + 3]+" x2:"+dum[1 + i*6 + 4]+" y2:"+dum[1 + i*6 + 5]);

                    }
				}

                end = System.currentTimeMillis();
                timer[2] = end - start;  // Draw
                Log.i("night > Draw",    ""+timer[2]);

                start = System.currentTimeMillis();
                // Release
                DetectManager.delete_out_data();
                end = System.currentTimeMillis();
                timer[3] = end - start;  // Release
                Log.i("night > Release", ""+timer[3]);

                e2e_end = System.currentTimeMillis();
                timer[0] = e2e_end - e2e_start;  // End-to-End

                Log.i("night End-to-End("+n+"/"+(int)dum[0]+")", ""+timer[0]);

                e2e_start = System.currentTimeMillis();

                /*
                2018-12-03 01:59:55.715 13160-13652/com.example.leek.my_usb I/ >> convert,resize: 12.087
                2018-12-03 01:59:55.715 13160-13652/com.example.leek.my_usb I/ >> normalize: 6.456
                2018-12-03 01:59:55.715 13160-13652/com.example.leek.my_usb I/ >> inference: 81.094
                2018-12-03 01:59:55.715 13160-13652/com.example.leek.my_usb I/ > Detect: 100
                2018-12-03 01:59:55.717 13160-13652/com.example.leek.my_usb I/ > Draw: 2
                2018-12-03 01:59:55.718 13160-13652/com.example.leek.my_usb I/ > Release: 0
                2018-12-03 01:59:55.718 13160-13652/com.example.leek.my_usb I/End-to-End: 115
                 */
            }
        });

        alertThread = new AlertThread(this);
        alertThread.start();

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
        if(alertThread.tts_obj != null){
            alertThread.tts_obj.stop();
            alertThread.tts_obj.shutdown();
        }
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
