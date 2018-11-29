//
// Created by LG-PC on 2018-11-21.
//

#ifndef MY_USB_PROJECT_ZERO_OBS_UTIL_H
#define MY_USB_PROJECT_ZERO_OBS_UTIL_H

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

int Isnear(box *bbox);
void trace_gauge(/*Obs_gauge *tracked_gauge*/);
void gauge_control(box* src);
#endif //MY_USB_PROJECT_ZERO_OBS_UTIL_H
