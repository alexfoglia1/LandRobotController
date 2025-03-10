#ifndef VIDEOPROCESSING_H
#define VIDEOPROCESSING_H

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <qmap.h>

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

	VideoProcessing(Format inputFormat, Format outputFormat);

	void process(cv::Mat& input, cv::Mat& output);

private:
	static QMap<Format, int> _formatToSize;
	static QMap<Format, int> _formatToCvFormat;

	Format _inputFormat;
	Format _outputFormat;

	void crop(const cv::Mat input, cv::cuda::GpuMat& output);

	/** GPU Operations **/
	
};


#endif
