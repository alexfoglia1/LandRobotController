#ifndef DIGITAL_ZOOM_CUH
#define DIGITAL_ZOOM_CUH

#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

__global__ void zoomIn(uchar* input, uchar* output, int width, int height, int channels);


#endif //DIGITAL_ZOOM_CUH