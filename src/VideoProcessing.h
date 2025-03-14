#ifndef VIDEOPROCESSING_H
#define VIDEOPROCESSING_H

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafilters.hpp>

#include <qmap.h>

#include "DigitalZoom.h"

class VideoProcessing
{
public:
	enum class Format
	{
		Y8,
		RGB,
		BGR,
		BGRA,
		RGBA
	};

	enum class Algorithm : quint32
	{
		ALL_OFF          = 0x00000000,
		BILATERAL_FILTER = 0x00000001,
		CLAHE            = 0x00000002,
		DIGITAL_ZOOM	 = 0x00000004,
		STABILIZATION    = 0x00000008,
		ALL              = 0xFFFFFFFF,
	};

	VideoProcessing(Format inputFormat, Format outputFormat);

	void process(cv::Mat& input, cv::Mat& output);
	void setAlgorithmEnabled(Algorithm algo, bool enabled);
	void setDigitalZoomStep(quint8 zoomStep);
	quint32 algorithmsEnabled();

private:
	static QMap<Format, int> _formatToSize;
	static QMap<Format, int> _formatToCvFormat;
	cv::cuda::GpuMat _prevGray;

	Format _inputFormat;
	Format _outputFormat;
	quint32 _algorithmsEnabled;
	DigitalZoomStep _zoomStep;

	void crop(const cv::Mat input, cv::cuda::GpuMat& output);
	void stabilizeFrame(cv::cuda::GpuMat& frame);

	/** GPU Operations **/
	
};


#endif
