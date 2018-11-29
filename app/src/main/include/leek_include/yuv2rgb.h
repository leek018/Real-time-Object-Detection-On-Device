//
// Created by LG-PC on 2018-11-20.
//

#ifndef MY_USB_PROJECT_ZERO_YUV2RGB_H
#define MY_USB_PROJECT_ZERO_YUV2RGB_H

#include <stdint.h>

void ConvertYUV420SPToARGB8888(const uint8_t* const pY,
                               const uint8_t* const pUV, float* const output,
                               const int width, const int height);

#endif //MY_USB_PROJECT_ZERO_YUV2RGB_H
