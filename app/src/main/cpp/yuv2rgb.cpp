//
// Created by LG-PC on 2018-11-21.
//

#include "../header/yuv2rgb.h"

#ifndef MAX
#define MAX(a, b) ({__typeof__(a) _a = (a); __typeof__(b) _b = (b); _a > _b ? _a : _b; })
#define MIN(a, b) ({__typeof__(a) _a = (a); __typeof__(b) _b = (b); _a < _b ? _a : _b; })
#endif

static const int kMaxChannelValue = 262143;

static inline uint32_t YUV2RGB(int nY, int nU, int nV) {
    nY -= 16;
    nU -= 128;
    nV -= 128;
    if (nY < 0) nY = 0;

    // This is the floating point equivalent. We do the conversion in integer
    // because some Android devices do not have floating point in hardware.
    // nR = (int)(1.164 * nY + 2.018 * nU);
    // nG = (int)(1.164 * nY - 0.813 * nV - 0.391 * nU);
    // nB = (int)(1.164 * nY + 1.596 * nV);

    int nR = 1192 * nY + 1634 * nV;
    int nG = 1192 * nY - 833 * nV - 400 * nU;
    int nB = 1192 * nY + 2066 * nU;

    nR = MIN(kMaxChannelValue, MAX(0, nR));
    nG = MIN(kMaxChannelValue, MAX(0, nG));
    nB = MIN(kMaxChannelValue, MAX(0, nB));

    nR = (nR >> 10) & 0xff;
    nG = (nG >> 10) & 0xff;
    nB = (nB >> 10) & 0xff;

    return 0xff000000 | (nR << 16) | (nG << 8) | nB;
}
void ConvertYUV420SPToARGB8888(const uint8_t* const yData,const uint8_t* const uvData, float* const output, const int width, const int height) {
    const uint8_t* pY = yData;
    const uint8_t* pUV = uvData;
    float* out = output;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int nY = *pY++;
            int offset = (y >> 1) * width + 2 * (x >> 1);
#ifdef __APPLE__
            int nU = pUV[offset];
      int nV = pUV[offset + 1];
#else
            int nV = pUV[offset];
            int nU = pUV[offset + 1];
#endif

            *out++ = (float)YUV2RGB(nY, nU, nV);
        }
    }
}