#include "DigitalZoom.h"
#include <opencv2/imgproc.hpp>

__global__ void zoomKernel(
    const uchar3* input, uchar3* output,
    int zoomWidth, int zoomHeight,
    int width, int height,
    size_t inputPitch, size_t outputPitch)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x > width || y > height) return;

    // Calcola la posizione originale
    float scaleX = (float)zoomWidth / width;
    float scaleY = (float)zoomHeight / height;
    int srcX = __min((int)(x * scaleX), zoomWidth);
    int srcY = __min((int)(y * scaleY), zoomHeight);

    // Calcola l'indice considerando il pitch (larghezza effettiva della riga in byte)
    uchar3* inputRow = (uchar3*)((char*)input + srcY * inputPitch);
    uchar3* outputRow = (uchar3*)((char*)output + y * outputPitch);

    outputRow[x] = inputRow[srcX];
}


void digitalZoom(cv::cuda::GpuMat& gpuMat, DigitalZoomStep zoomStep)
{
    dim3 gridSize(gpuMat.cols, gpuMat.rows);

    uchar3* output;
    size_t outputPitch;
    cudaMallocPitch(&output, &outputPitch, gpuMat.cols * sizeof(uchar3), gpuMat.rows);

    float enlargeSize = powf(1.25f, static_cast<float>(zoomStep));
    dim3 blockSize(16, 16);

    int roiWidth = gpuMat.cols * (1.0f / (enlargeSize));
    int roiHeight = gpuMat.rows * (1.0f / (enlargeSize));
    int roiX = gpuMat.cols / 2 - roiWidth / 2;
    int roiY = gpuMat.rows / 2 - roiHeight / 2;

    cv::cuda::GpuMat inputRoi = gpuMat(cv::Rect(roiX, roiY, roiWidth, roiHeight));

    uchar3* input = inputRoi.ptr<uchar3>();
    zoomKernel << <gridSize, blockSize >> > (input, output,
        roiWidth, roiHeight,
        gpuMat.cols, gpuMat.rows,
        inputRoi.step, gpuMat.step);

    cudaError err = cudaMemcpy2D(gpuMat.ptr<uchar>(), gpuMat.step,
        output, outputPitch,
        gpuMat.cols * sizeof(uchar3), gpuMat.rows,
        cudaMemcpyDeviceToDevice);
    
    if (err != cudaSuccess)
    {
        std::cerr << "cudaMemcpy failed: " << cudaGetErrorString(err) << std::endl;
    }

    cudaFree(output);

    cudaDeviceSynchronize();
}