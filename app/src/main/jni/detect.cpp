//
// Created by LG-PC on 2018-11-22.
//

#include <jni.h>
#include <android/log.h>
#include "../include/leek_include/mssd.h"
#include "../include/leek_include/Obs_util.h"
#include <../include/opencv2/opencv.hpp>

#include <sys/time.h>

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,__VA_ARGS__)
#define FIXED_WIDTH 300
#define FIXED_HEIGHT 300
#define CHANNEL 3
#define IMG_SIZE FIXED_WIDTH * FIXED_HEIGHT * CHANNEL
#define IDX_OF_STAIR 21

//global variable


#define getMillisecond(start, end) \
	(end.tv_sec-start.tv_sec)*1000 + \
    (end.tv_usec-start.tv_usec)/1000.0
struct timeval start, end;
double timer[10];


Obs_gauge stair_guage;
graph_t global_graph= NULL;
tensor_t global_tensor_input = NULL;
tensor_t global_tensor_out = NULL;
int dims[] = {1,3,300,300};
int num_detected_obj = 0;


float* out_data = NULL;
float* global_input = NULL;
float threshold = 0.5;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_leek_my_1usb_DetectManager_detect(JNIEnv *env, jclass type, jbyteArray nv21Yuv_,
                                                   jint width, jint height) {
    num_detected_obj = 0;
    jbyte* const i = env->GetByteArrayElements(nv21Yuv_, NULL);

    /* Preprocessing */

    gettimeofday(&start, NULL);

    cv::Mat yuv(height+height/2, width, CV_8UC1,(uchar *)i);
    cv::Mat converted(height, width, CV_8UC3);
    cv::cvtColor(yuv, converted, CV_YUV2BGR_NV21);
    cv::resize(converted,converted,cv::Size(FIXED_HEIGHT,FIXED_WIDTH));
    //cv::imwrite("/sdcard/saved_images/leek.jpg",converted);
    converted.convertTo(converted,CV_32FC3);
    float* rgb_data = (float*)converted.data;

    gettimeofday(&end, NULL);
    timer[0] = getMillisecond(start, end);  // Preprocessing (convert, resize)


    /*
    cv::Mat img = cv::imread("/sdcard/saved_images/ssd_dog.jpg");
    if( img.empty())
        return -1;
    cv::resize(img,img,cv::Size(FIXED_HEIGHT,FIXED_WIDTH));
    img.convertTo(img,CV_32FC3);
    float *rgb_data = (float*)img.data;
     */

    gettimeofday(&start, NULL);

    int hw = FIXED_HEIGHT * FIXED_WIDTH;
    float mean[3] = {127.5,127.5,127.5};
    for (int h = 0; h < FIXED_HEIGHT; h++)
    {
        for (int w = 0; w < FIXED_WIDTH; w++)
        {
//            for (int c = 0; c < 3; c++)
//            {
//                global_input[c * hw + h * FIXED_WIDTH + w] = 0.007843* (*rgb_data - mean[c]);
//                rgb_data++;
//            }
            // Loop Unrolling (10->7ms)
            global_input[       h * FIXED_WIDTH + w] = 0.007843* (*(rgb_data  ) - mean[0]);
            global_input[hw   + h * FIXED_WIDTH + w] = 0.007843* (*(rgb_data+1) - mean[1]);
            global_input[hw*2 + h * FIXED_WIDTH + w] = 0.007843* (*(rgb_data+2) - mean[2]);
            rgb_data+=3;
        }
    }


    gettimeofday(&end, NULL);
    timer[1] = getMillisecond(start, end);  // Preprocessing (Normalization)


    /* Inference */

    gettimeofday(&start, NULL);

    int res = detect(global_input,&out_data,global_graph,global_tensor_input,&global_tensor_out,&num_detected_obj,IMG_SIZE);
    //post_process_ssd("/sdcard/saved_images/leek.jpg",threshold,out_data,num_detected_obj,"/sdcard/saved_images/leek_processed.jpg");

    gettimeofday(&end, NULL);
    timer[2] = getMillisecond(start, end);  // Inference

    LOGI("night >> convert,resize", "%.3lf", timer[0]); // ~10 ms
    LOGI("night >> normalize",      "%.3lf", timer[1]); // ~7 ms
    LOGI("night >> inference",      "%.3lf", timer[2]); // ~80 ms


    env->ReleaseByteArrayElements(nv21Yuv_, i, 0);
    if(res == 0)
        return JNI_TRUE;
    return JNI_FALSE;

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_leek_my_1usb_DetectManager_delete_1graph_1space(JNIEnv *env, jclass type) {

    // TODO
    LOGI("detect", "graph_delete");
    const char* model_name = "mssd_300";
    graph_finish(global_input,global_graph,global_tensor_input,model_name);
    global_input = NULL;
    global_tensor_input = NULL;
    global_graph = NULL;
    if(global_graph !=NULL | global_tensor_input != NULL | global_graph !=NULL)
        return JNI_FALSE;
    return JNI_TRUE;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_leek_my_1usb_DetectManager_delete_1out_1data(JNIEnv *env, jclass type) {
    delete_out_tensor(global_tensor_out);
}



extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_leek_my_1usb_DetectManager_get_1graph_1space(JNIEnv *env, jclass type,
                                                              jstring model_name_,
                                                              jstring model_path_,
                                                              jstring proto_path_,
                                                              jstring device_type_) {
    const char *model_name = env->GetStringUTFChars(model_name_, 0);
    const char *model_path = env->GetStringUTFChars(model_path_, 0);
    const char *proto_path = env->GetStringUTFChars(proto_path_, 0);
    const char *device_type = env->GetStringUTFChars(device_type_, 0);


    // TODO
    global_input = (float*)malloc(sizeof(float)*IMG_SIZE);
    if(global_input == NULL)
        return JNI_FALSE;
//    int result = graph_ready(&global_graph,&global_tensor_input,dims,model_name,model_path,proto_path,device_type);
    int result = graph_ready(&global_graph,&global_tensor_input,dims,model_name,model_path,proto_path,nullptr);


    env->ReleaseStringUTFChars(model_name_, model_name);
    env->ReleaseStringUTFChars(model_path_, model_path);
    env->ReleaseStringUTFChars(proto_path_, proto_path);
    env->ReleaseStringUTFChars(device_type_, device_type);

    if(result == 0 ){
        return JNI_TRUE;
    }
    return JNI_FALSE;
}



extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_leek_my_1usb_DetectManager_get_1out_1data(JNIEnv *env, jclass type,
                                                           jfloatArray data_of_java_) {
    jfloat *data_of_java = env->GetFloatArrayElements(data_of_java_, NULL);

    int top = -1;
    float* obs_pointer_buffer[IDX_OF_STAIR] = {NULL,};
    float* temp_processed_data = &data_of_java[1];
    float* data = out_data;
    data_of_java[0]=num_detected_obj;
    if( num_detected_obj == 0){
        gauge_control(NULL,&stair_guage);
        env->ReleaseFloatArrayElements(data_of_java_, data_of_java, 0);
        return JNI_FALSE;
    }
    for (int i=0;i<num_detected_obj;i++)
    {
        if( data[1] > threshold) {
            if( data[0] != IDX_OF_STAIR ) {
                temp_processed_data[0] = data[0];
                temp_processed_data[1] = -1;
                temp_processed_data[2] = data[2];
                temp_processed_data[3] = data[3];
                temp_processed_data[4] = data[4];
                temp_processed_data[5] = data[5];
            }
            else{
                obs_pointer_buffer[++top] = data;
            }
        }
        data+=6;
        temp_processed_data+=6;
    }
    if(top == -1){
        gauge_control(nullptr,&stair_guage);
        env->ReleaseFloatArrayElements(data_of_java_, data_of_java, 0);
        return JNI_TRUE;
    }
    for(int i = 0 ; i<= top ; i++){
        float* loaded_obs_pointer = obs_pointer_buffer[i];
        gauge_control(loaded_obs_pointer,&stair_guage);
        temp_processed_data[0]=loaded_obs_pointer[0];
        temp_processed_data[1]=get_state(&stair_guage);
        temp_processed_data[2]=loaded_obs_pointer[2];
        temp_processed_data[3]=loaded_obs_pointer[3];
        temp_processed_data[4]=loaded_obs_pointer[4];
        temp_processed_data[5]=loaded_obs_pointer[5];
        temp_processed_data+=6;
    }
    env->ReleaseFloatArrayElements(data_of_java_, data_of_java, 0);
    return JNI_TRUE;

    
}
