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
#define IDX_OF_STAIR 1
#define OBS_POINTER_BUFFER_SIZE 100

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
int dims[] = {1,3,300,300}; // NCHW
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
    converted.convertTo(converted,CV_32FC3);
    float* rgb_data = (float*)converted.data;

    gettimeofday(&end, NULL);
    timer[0] = getMillisecond(start, end);  // Preprocessing (convert, resize) ( ms)

    gettimeofday(&start, NULL);

    int hw = FIXED_HEIGHT * FIXED_WIDTH;
    for (int h = 0; h < FIXED_HEIGHT; h++)
    {
        for (int w = 0; w < FIXED_WIDTH; w++)
        {
            // Loop Unrolling
            global_input[       h * FIXED_WIDTH + w] = 0.007843 * (*(rgb_data  ) - 127.5);
            global_input[hw   + h * FIXED_WIDTH + w] = 0.007843 * (*(rgb_data+1) - 127.5);
            global_input[hw*2 + h * FIXED_WIDTH + w] = 0.007843 * (*(rgb_data+2) - 127.5);
            rgb_data+=3;
        }
    }


    gettimeofday(&end, NULL);
    timer[1] = getMillisecond(start, end);  // Preprocessing (Normalization)


    /* Inference */

    gettimeofday(&start, NULL);

    int res = detect(global_input,&out_data,global_graph,global_tensor_input,&global_tensor_out,&num_detected_obj,IMG_SIZE);

    gettimeofday(&end, NULL);
    timer[2] = getMillisecond(start, end);  // Inference

    LOGI("night >> convert,resize", "%.3lf", timer[0]); //  2.64 ms
    LOGI("night >> normalize",      "%.3lf", timer[1]); //  1.99 ms
    LOGI("night >> inference",      "%.3lf", timer[2]); // 41.26 ms


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
    const char *model_name  = env->GetStringUTFChars(model_name_, 0);
    const char *model_path  = env->GetStringUTFChars(model_path_, 0);
    const char *proto_path  = env->GetStringUTFChars(proto_path_, 0);
    const char *device_type = env->GetStringUTFChars(device_type_, 0);


    // TODO
    global_input = (float*)malloc(sizeof(float) *IMG_SIZE);
    if(global_input == NULL)
        return JNI_FALSE;

    int result = graph_ready(&global_graph,&global_tensor_input,dims,model_name,model_path,proto_path,nullptr);


    env->ReleaseStringUTFChars(model_name_, model_name);
    env->ReleaseStringUTFChars(model_path_, model_path);
    env->ReleaseStringUTFChars(proto_path_, proto_path);
    env->ReleaseStringUTFChars(device_type_, device_type);


    if(result == 0)
        return JNI_TRUE;
    return JNI_FALSE;
}


//extern "C"
//JNIEXPORT jboolean JNICALL
//Java_com_example_leek_my_1usb_DetectManager_get_1out_1data(JNIEnv *env, jclass type,
//                                                           jfloatArray data_of_java_) {
//    jfloat *data_of_java = env->GetFloatArrayElements(data_of_java_, NULL);
//
//    int top = -1;
//    float* temp_processed_data = &data_of_java[1];
//    float* data = out_data;
//    data_of_java[0]=num_detected_obj;
//    for (int i=0; i<num_detected_obj; i++)
//    {
////        if( data[1] > threshold ) {
//            temp_processed_data[0] = data[0]; //cls
//
//            temp_processed_data[2] = data[2]; //x1
//            temp_processed_data[3] = data[3]; //y1
//            temp_processed_data[4] = data[4]; //x2
//            temp_processed_data[5] = data[5]; //y2
//
//            if( (int)data[0] != IDX_OF_STAIR ) {
//                temp_processed_data[1] = -1; // state
//            } else {
//                gauge_control(temp_processed_data,&stair_guage);
//                temp_processed_data[1] = get_state(&stair_guage);  // state
//            }
////        }
//        data+=6;
//        temp_processed_data+=6;
//    }
//
//    if(top == -1){
//        gauge_control(nullptr, &stair_guage);
//    }
////    LOGI("gauge","weak_center_gauge %d",stair_guage.current_weak_center_gauge);
////    LOGI("gauge","weak_left_gauge %d",stair_guage.current_weak_left_gauge);
////    LOGI("gauge","weak_right_gauge %d",stair_guage.current_weak_right_gauge);
////    LOGI("gauge","strong_center_gauge %d",stair_guage.current_strong_gauge);
//    env->ReleaseFloatArrayElements(data_of_java_, data_of_java, 0);
//    return JNI_TRUE;
//
//
//}



extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_leek_my_1usb_DetectManager_get_1out_1data(JNIEnv *env, jclass type,
                                                           jfloatArray data_of_java_) {
    jfloat *data_of_java = env->GetFloatArrayElements(data_of_java_, NULL);

    int top = -1;
    float* obs_pointer_buffer[OBS_POINTER_BUFFER_SIZE] = {NULL,};
    float* temp_processed_data = &data_of_java[1];
    float* data = out_data;
    data_of_java[0]=num_detected_obj;
    for (int i=0; i<num_detected_obj; i++)
    {
        if( data[1] > threshold ) {
            if( (int)data[0] != IDX_OF_STAIR ) {
                temp_processed_data[0] = data[0];
                temp_processed_data[1] = -1;
                temp_processed_data[2] = data[2];
                temp_processed_data[3] = data[3];
                temp_processed_data[4] = data[4];
                temp_processed_data[5] = data[5];

                temp_processed_data+=6;
            } else {
                obs_pointer_buffer[++top] = data;
            }
        }
        LOGI("joapyo", "%d: %f %f %f %f", (int)data[0], data[2], data[3], data[4], data[5]);
        data+=6;
    }
    for(int i = 0 ; i<= top ; i++){
        float* loaded_obs_pointer = obs_pointer_buffer[i];
        gauge_control(loaded_obs_pointer,&stair_guage);
        temp_processed_data[0] = loaded_obs_pointer[0];
        temp_processed_data[1] = get_state(&stair_guage);
        temp_processed_data[2] = loaded_obs_pointer[2];
        temp_processed_data[3] = loaded_obs_pointer[3];
        temp_processed_data[4] = loaded_obs_pointer[4];
        temp_processed_data[5] = loaded_obs_pointer[5];
        temp_processed_data+=6;
    }
    if(top == -1){
        gauge_control(nullptr,&stair_guage);
    }
//    LOGI("gauge","weak_center_gauge %d",stair_guage.current_weak_center_gauge);
//    LOGI("gauge","weak_left_gauge %d",stair_guage.current_weak_left_gauge);
//    LOGI("gauge","weak_right_gauge %d",stair_guage.current_weak_right_gauge);
//    LOGI("gauge","strong_center_gauge %d",stair_guage.current_strong_gauge);
    env->ReleaseFloatArrayElements(data_of_java_, data_of_java, 0);
    return JNI_TRUE;


}
