#include <jni.h>
#include <string>
#define distance_threshold 0.8
#define gauge_length 10
#define gauge_init_val 0
#define nothing_or_detect_small -1
#define True 1
#define False 0
#define weak_alarm_point 5
#define strong_alarm_point 5
#define weak_case 0
#define strong_case 1

#define Weak_state 0
#define Strong_state 1
#define Normal_state 2
#define Ready_state 3



struct Obs_gauge {
    int current_weak_gauge, current_strong_gauge;
    int before_weak_gauge,before_strong_gauge;
    Obs_gauge() : current_weak_gauge(gauge_init_val), current_strong_gauge(gauge_init_val),
                  before_weak_gauge(gauge_init_val),before_strong_gauge(gauge_init_val) {}
};
struct box {
    float x, y, w, h;
};

Obs_gauge stair_gauge;


int Isnear(box *bbox) {
    if(bbox == NULL)
        return nothing_or_detect_small;
    float bbox_right = bbox->x + (bbox->w) / 2.;
    float bbox_left = bbox->x - (bbox->w) / 2.;
    int across_center = (bbox_right - 0.5) * (bbox_left - 0.5) > 0 ? False : True;
    int far_away = (bbox->y + (bbox->h) / 2.) < distance_threshold ? True : False;
    if ((across_center == True && far_away == True) || (across_center == False && far_away == False))
        return weak_case;
    else if (across_center == True && far_away == False)
        return strong_case;
    else
        return nothing_or_detect_small;
}
void trace_gauge(/*Obs_gauge *tracked_gauge*/)
{
    /*
    tracked_gauge->before_weak_gauge = tracked_gauge->current_weak_gauge;
    tracked_gauge->before_strong_gauge = tracked_gauge->current_strong_gauge;
     */
    stair_gauge.before_weak_gauge = stair_gauge.current_weak_gauge;
    stair_gauge.before_strong_gauge = stair_gauge.current_strong_gauge;
}
void gauge_control(box* src) {
    int case_;
    int current_weak_temp = stair_gauge.current_weak_gauge;
    int current_strong_temp = stair_gauge.current_strong_gauge;
    box *b = src;

    case_ = Isnear(b);
    if (case_ == weak_case) {
        stair_gauge.current_weak_gauge = (current_weak_temp < gauge_length) ? ++current_weak_temp : gauge_length;
        stair_gauge.current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
    } else if (case_ == strong_case) {
        stair_gauge.current_weak_gauge = (current_weak_temp > gauge_init_val) ? --current_weak_temp : gauge_init_val;
        stair_gauge.current_strong_gauge = (current_strong_temp < gauge_length) ? ++current_strong_temp : gauge_length;
    } else {
        stair_gauge.current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
        stair_gauge.current_weak_gauge = (current_weak_temp > gauge_init_val) ? --current_weak_temp : gauge_init_val;
    }
    trace_gauge();
}
extern "C" JNIEXPORT jstring

JNICALL
Java_com_example_leek_my_1usb_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_leek_my_1usb_ObstacleManager_feed_1image(JNIEnv *env, jclass type,
                                                          jbyteArray nv21Yuv_) {
    jbyte *nv21Yuv = env->GetByteArrayElements(nv21Yuv_, NULL);

    // TODO

    env->ReleaseByteArrayElements(nv21Yuv_, nv21Yuv, 0);
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_leek_my_1usb_ObstacleManager_GetDegreeOfWarning(JNIEnv *env, jclass type) {

    // TODO
    if(stair_gauge.before_weak_gauge == weak_alarm_point-1 && stair_gauge.current_weak_gauge == weak_alarm_point)
        return Weak_state;
    else if( stair_gauge.before_strong_gauge == strong_alarm_point-1 && stair_gauge.current_strong_gauge == strong_alarm_point)
        return Strong_state;
    else if( stair_gauge.current_strong_gauge == gauge_init_val && stair_gauge.current_weak_gauge == gauge_init_val)
        return Normal_state;
    else
        return Ready_state;

}