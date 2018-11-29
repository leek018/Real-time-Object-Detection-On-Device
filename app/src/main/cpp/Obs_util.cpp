//
// Created by LG-PC on 2018-11-21.
//


#include <cstddef>
#include "../header/Obs_util.h"

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
void trace_gauge(Obs_gauge *stair_gauge)
{
    /*
    tracked_gauge->before_weak_gauge = tracked_gauge->current_weak_gauge;
    tracked_gauge->before_strong_gauge = tracked_gauge->current_strong_gauge;
     */
    stair_gauge->before_weak_gauge = stair_gauge->current_weak_gauge;
    stair_gauge->before_strong_gauge = stair_gauge->current_strong_gauge;
}
void gauge_control(box* src,Obs_gauge *stair_gauge) {
    int case_;
    int current_weak_temp = stair_gauge->current_weak_gauge;
    int current_strong_temp = stair_gauge->current_strong_gauge;
    box *b = src;

    case_ = Isnear(b);
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
    trace_gauge();
}
