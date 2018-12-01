package com.example.leek.my_usb;

public class DetectManager {
    public static native boolean get_graph_space(String model_name,String model_path,String proto_path,String device_type);
    public static native void delete_out_data();
    public static native boolean delete_graph_space();
    public static native boolean detect(byte[] nv21Yuv,int width,int height);
    public static native boolean get_out_data(float[] data_of_java);


}
