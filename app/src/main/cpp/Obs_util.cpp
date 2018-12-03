//
// Created by leek on 2018-11-21.
//


#include "../include/leek_include/Obs_util.h"

//raw_data->[class_index,score(probabilty),x0,y0,x1,y1]
// threshold_you_want : if object's score surpassed threshold, I will think object is detected correctly
int Isnear(float *raw_data ) {
    if( raw_data == nullptr )
        return nothing_or_detect_low_probability;
    float obj_right = raw_data[X1];
    float obj_left = raw_data[X0];
    float obj_bottom = raw_data[Y1];

    //add middle area
    float middle_right = middle_threshold + middle_add_area;
    float middle_left = middle_threshold - middle_add_area;

    int right_area = obj_left > middle_right ? True : False;
    int left_area = obj_right < middle_left ? True : False;
    //int across_center = (obj_right - middle_threshold) * (obj_left - middle_threshold) > 0 ? False : True;
    int far_away =  obj_bottom < bottom_threshold ? True : False;

    if (right_area) {
        if(far_away) return weak_right_case;
        return nothing_or_detect_low_probability;
    }
    else if(left_area) {
        if(far_away) return weak_left_case;
        return weak_center_case;
    }
    else {  //middle area
        if(far_away) return weak_center_case;
        return strong_case;
    }

    /*
    if ((across_center == True && far_away == True) || (across_center == False && far_away == False))
        return weak_center_case;
    else if (across_center == True && far_away == False)
        return strong_case;
    else
        return nothing_or_detect_low_probability;
        */
}
/*
void trace_gauge(Obs_gauge *stair_gauge)
{
    stair_gauge->before_weak_gauge = stair_gauge->current_weak_gauge;
    stair_gauge->before_strong_gauge = stair_gauge->current_strong_gauge;
}
 */
void gauge_control(float *raw_data,Obs_gauge *stair_gauge) {
    int case_;
    int current_weak_center_temp = stair_gauge->current_weak_center_gauge;
    int current_weak_left_temp = stair_gauge->current_weak_left_gauge;
    int current_weak_right_temp = stair_gauge->current_weak_right_gauge;

    //int current_weak_temp = stair_gauge->current_weak_gauge;

    int current_strong_temp = stair_gauge->current_strong_gauge;
    case_ = Isnear(raw_data);
    if (case_ == weak_center_case) {
        stair_gauge->current_weak_center_gauge = (current_weak_center_temp < gauge_length) ? ++current_weak_center_temp : gauge_length;
        stair_gauge->current_weak_left_gauge = (current_weak_left_temp > gauge_init_val) ? --current_weak_left_temp : gauge_init_val;
        stair_gauge->current_weak_right_gauge = (current_weak_right_temp > gauge_init_val) ? --current_weak_right_temp : gauge_init_val;
        stair_gauge->current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
    } else if (case_ == weak_left_case) {
        stair_gauge->current_weak_center_gauge = (current_weak_center_temp > gauge_length) ? --current_weak_center_temp : gauge_init_val;
        stair_gauge->current_weak_left_gauge = (current_weak_left_temp < gauge_init_val) ? ++current_weak_left_temp : gauge_length;
        stair_gauge->current_weak_right_gauge = (current_weak_right_temp > gauge_init_val) ? --current_weak_right_temp : gauge_init_val;
        stair_gauge->current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
    }else if (case_ == weak_right_case) {
        stair_gauge->current_weak_center_gauge = (current_weak_center_temp > gauge_length) ? --current_weak_center_temp : gauge_init_val;
        stair_gauge->current_weak_left_gauge = (current_weak_left_temp > gauge_init_val) ? --current_weak_left_temp : gauge_init_val;
        stair_gauge->current_weak_right_gauge = (current_weak_right_temp < gauge_init_val) ? ++current_weak_right_temp : gauge_length;

        stair_gauge->current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
    }else if (case_ == strong_case) {
        stair_gauge->current_weak_center_gauge = (current_weak_center_temp > gauge_length) ? --current_weak_center_temp : gauge_init_val;
        stair_gauge->current_weak_left_gauge = (current_weak_left_temp > gauge_init_val) ? --current_weak_left_temp : gauge_init_val;
        stair_gauge->current_weak_right_gauge = (current_weak_right_temp > gauge_init_val) ? --current_weak_right_temp : gauge_init_val;
        stair_gauge->current_strong_gauge = (current_strong_temp < gauge_init_val) ? ++current_strong_temp : gauge_length;
    } else {
        stair_gauge->current_weak_center_gauge = (current_weak_center_temp > gauge_length) ? --current_weak_center_temp : gauge_init_val;
        stair_gauge->current_weak_left_gauge = (current_weak_left_temp > gauge_init_val) ? --current_weak_left_temp : gauge_init_val;
        stair_gauge->current_weak_right_gauge = (current_weak_right_temp > gauge_init_val) ? --current_weak_right_temp : gauge_init_val;
        stair_gauge->current_strong_gauge = (current_strong_temp > gauge_init_val) ? --current_strong_temp : gauge_init_val;
    }
    //trace_gauge(stair_gauge);
}

int get_state(Obs_gauge *stair_gauge)
{
    if( stair_gauge->current_weak_center_gauge >= gauge_length/2 )
        return Weak_center_state;
    else if( stair_gauge->current_weak_right_gauge >= gauge_length/2)
        return Weak_right_state;
    else if(stair_gauge->current_weak_left_gauge >= gauge_length/2)
        return Weak_right_state;
    else if( stair_gauge->current_strong_gauge >= gauge_length/2 )
        return Strong_state;
    else
        return Zero_state;
}
