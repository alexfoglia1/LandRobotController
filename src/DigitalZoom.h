#ifndef DIGITAL_ZOOM_CUH
#define DIGITAL_ZOOM_CUH

#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

enum class DigitalZoomStep
{
	ZOOM_2X  = 0x01,
	ZOOM_4X  = 0x02,
	ZOOM_8X  = 0x03,
	ZOOM_16X = 0x04,

	ZOOM_STEPS = ZOOM_16X
};

float getDigitalZoomScale(DigitalZoomStep zoomStep);

void digitalZoom(cv::cuda::GpuMat& gpuMat, DigitalZoomStep zoomStep);


#endif //DIGITAL_ZOOM_CUH