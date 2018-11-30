//
// Created by LG-PC on 2018-11-21.
//

//box information part
#define SCORE 1
#define x0 2
#define y0 3
#define x1 4
#define y1 5

// Threshold about state which is needed to be modified
#define bottom_threshold 0.8
#define middle_threshold 0.5

// gauge information ,which is needed to be modified nothing but gauge_init_val
#define gauge_length 10
#define gauge_init_val 0
#define weak_alarm_threshold 5
#define strong_alarm_threshold 5

#define nothing_or_detect_low_probability -1
#define True 1
#define False 0

#define weak_center_case 0
#define strong_case 1
#define weak_left_case 2
#define weak_right_case 3

#define Zero_state 0
#define Weak_center_state 1
#define Weak_left_state 2
#define Weak_right_state 3
#define Strong_state 4

struct Obs_gauge {
    int current_weak_center_gauge;
    int current_weak_left_gauge;
    int current_weak_right_gauge;
    int current_strong_gauge;
    //int current_weak_gauge, current_strong_gauge;
    //int before_weak_gauge,before_strong_gauge;
    Obs_gauge() : current_weak_center_gauge(gauge_init_val), current_weak_left_gauge(gauge_init_val),
                  current_weak_right_gauge(gauge_init_val), current_strong_gauge(gauge_init_val) {}
    //Obs_gauge() : current_weak_gauge(gauge_init_val), current_strong_gauge(gauge_init_val) {}
    //before_weak_gauge(gauge_init_val),before_strong_gauge(gauge_init_val)
};


int Isnear(float *raw_data, float threshold_you_want);
void trace_gauge(Obs_gauge *tracked_gauge);
void gauge_control(float *raw_data,Obs_gauge *stair_gauge,float threshold_you_want);

