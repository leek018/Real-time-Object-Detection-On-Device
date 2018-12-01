//
// Created by LG-PC on 2018-11-21.
//

#include <cstddef>
#include "../include/leek_include/Obs_util.h"

//raw_data->[class_index,score(probabilty),x0,y0,x1,y1]
// threshold_you_want : if object's score surpassed threshold, I will think object is detected correctly
int Isnear(float *raw_data, float threshold_you_want ) {
    if(raw_data[SCORE] < threshold_you_want )
        return nothing_or_detect_low_probability;
    float obj_right = raw_data[X1];
    float obj_left = raw_data[X0];
    float obj_bottom = raw_data[Y1];
    int across_center = (obj_right - middle_threshold) * (obj_left - middle_threshold) > 0 ? False : True;
    int far_away =  obj_bottom < bottom_threshold ? True : False;
    if ((across_center == True && far_away == True) || (across_center == False && far_away == False))
        return weak_case;
    else if (across_center == True && far_away == False)
        return strong_case;
    else
        return nothing_or_detect_low_probability;
}
/*
void trace_gauge(Obs_gauge *stair_gauge)
{
    stair_gauge->before_weak_gauge = stair_gauge->current_weak_gauge;
    stair_gauge->before_strong_gauge = stair_gauge->current_strong_gauge;
}
 */
void gauge_control(float *raw_data,Obs_gauge *stair_gauge,float threshold_you_want) {
    int case_;
    int current_weak_temp = stair_gauge->current_weak_gauge;
    int current_strong_temp = stair_gauge->current_strong_gauge;

    case_ = Isnear(raw_data,threshold_you_want);
    if (case_ == weak_case) {
        stair_gauge->current_weak_gauge = (current_weak_temp < gauge_length) ? ++current_weak_temp : gauge_length;
        stair_gauge->current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
    } else if (case_ == strong_case) {
        stair_gauge->current_weak_gauge = (current_weak_temp > gauge_init_val) ? --current_weak_temp : gauge_init_val;
        stair_gauge->current_strong_gauge = (current_strong_temp < gauge_length) ? ++current_strong_temp : gauge_length;
    } else {
        stair_gauge->current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
        stair_gauge->current_weak_gauge = (current_weak_temp > gauge_init_val) ? --current_weak_temp : gauge_init_val;
    }

    //trace_gauge(stair_gauge);
}

int get_state(Obs_gauge *stair_gauge)
{
    if( stair_gauge->current_weak_gauge >= gauge_length/2 )
        return Weak_center_state;
    else if( stair_gauge->current_strong_gauge >= gauge_length/2 )
        return Strong_state;
    else
        return Zero_state;
}