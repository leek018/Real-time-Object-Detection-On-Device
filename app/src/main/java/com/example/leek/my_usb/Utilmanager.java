package com.example.leek.my_usb;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class Utilmanager {

    /*** for YUV2RGB functionality check using java
     * requset permission -> decode-> bitmap -> save -> check
     * decodeYUV420SP(rgb,nv21Yuv,cam_width,cam_height);
     * bmp = Bitmap.createBitmap(cam_width,cam_height,Bitmap.Config.ARGB_8888);
     * bmp.setPixels(rgb,0,cam_width,0,0,cam_width,cam_height);
     * saveToInternalStorage(bmp);
    ***/
    /***
    private static int YUV2RGB(int y, int u, int v) {
        // Adjust and check YUV values
        y = (y - 16) < 0 ? 0 : (y - 16);
        u -= 128;
        v -= 128;

        // This is the floating point equivalent. We do the conversion in integer
        // because some Android devices do not have floating point in hardware.
        // nR = (int)(1.164 * nY + 2.018 * nU);
        // nG = (int)(1.164 * nY - 0.813 * nV - 0.391 * nU);
        // nB = (int)(1.164 * nY + 1.596 * nV);
        int y1192 = 1192 * y;
        int r = (y1192 + 1634 * v);
        int g = (y1192 - 833 * v - 400 * u);
        int b = (y1192 + 2066 * u);

        // Clipping RGB values to be inside boundaries [ 0 , kMaxChannelValue ]
        r = r > kMaxChannelValue ? kMaxChannelValue : (r < 0 ? 0 : r);
        g = g > kMaxChannelValue ? kMaxChannelValue : (g < 0 ? 0 : g);
        b = b > kMaxChannelValue ? kMaxChannelValue : (b < 0 ? 0 : b);

        return 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
    }

    public static void decodeYUV420SP(int[] output, byte[] input, int width, int height) {
        final int frameSize = width * height;
        for (int j = 0, yp = 0; j < height; j++) {
            int uvp = frameSize + (j >> 1) * width;
            int u = 0;
            int v = 0;

            for (int i = 0; i < width; i++, yp++) {
                int y = 0xff & input[yp];
                if ((i & 1) == 0) {
                    v = 0xff & input[uvp++];
                    u = 0xff & input[uvp++];
                }

                output[yp] = YUV2RGB(y, u, v);
            }
        }
    }
    private void saveToInternalStorage(Bitmap bitmapImage){
        String root = Environment.getExternalStorageDirectory().getAbsolutePath();
        Log.i("external",""+root);
        File myDir = new File(root + "/saved_images");
        if(!myDir.mkdirs())
        {
            Log.i("mkdir","can't");
            return;
        }
        String fname = "Image.jpg";
        File file = new File (myDir, fname);
        if (file.exists ()) file.delete ();
        try {
            FileOutputStream out = new FileOutputStream(file);
            bitmapImage.compress(Bitmap.CompressFormat.JPEG, 90, out);

            out.flush();
            out.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }


     try {
     AssetManager assetFiles = getAssets();

     // MyHtmlFiles is the name of folder from inside our assets folder
     String[] files = assetFiles.list("model");

     // Initialize streams
     InputStream in = null;
     OutputStream out = null;

     for (int i = 0; i < files.length; i++) {
     Log.i("files",""+files[i]);
     // Currently we will copy the files to the root directory
     // but you should create specific directory for your app
     Log.i("model_path",""+model_path);
     in = assetFiles.open("model/"+files[i]);
     out = new FileOutputStream(
     model_path+"/"
     + files[i]);
     Utilmanager.copyAssetFiles(in, out);
     }


     } catch (FileNotFoundException e) {
     e.printStackTrace();
     showShortMsg("File_saving_problem");
     } catch (NullPointerException e) {
     e.printStackTrace();
     } catch (Exception e) {
     e.printStackTrace();
     }


    ***/



    private static final int BUFFER_SIZE = 1024;
    public static void copyAssetFiles(InputStream in, OutputStream out) {
        try {

            byte[] buffer = new byte[BUFFER_SIZE];
            int read;

            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }

            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;

        } catch (IOException e) {
            e.printStackTrace();
        } catch (NullPointerException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }



    /*
    public static void requset_permssion(Context context, Activity activity){
        if (Build.VERSION.SDK_INT >= 23)
        {
            if (!checkPermission(context))
            {
                requestPermission(activity); // Code for permission
                // Code for above or equal 23 API Oriented Device
                // Your Permission granted already .Do next code
            }
        }
    }
    private static boolean checkPermission(Context context) {//
        int result = ContextCompat.checkSelfPermission(context, android.Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (result == PackageManager.PERMISSION_GRANTED) {
            return true;
        } else {
            return false;
        }
    }
    private static void requestPermission(Activity activity) {

        if (ActivityCompat.shouldShowRequestPermissionRationale(activity, android.Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            Toast.makeText(activity, "Write External Storage permission allows us to do store images. Please allow this permission in App Settings.", Toast.LENGTH_LONG).show();
        } else {
            ActivityCompat.requestPermissions(activity, new String[]{android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, PERMISSION_REQUEST_CODE);
        }
    }
    private static void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
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
    */
}
