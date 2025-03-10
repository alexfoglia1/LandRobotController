#include "VideoProcessing.h"



QMap<VideoProcessing::Format, int> VideoProcessing::_formatToSize =
{
	{VideoProcessing::Format::Y8,	1},
	{VideoProcessing::Format::BGR,	3},
	{VideoProcessing::Format::RGB,	3},
	{VideoProcessing::Format::BGRA, 4},
	{VideoProcessing::Format::RGBA, 4}
};

QMap<VideoProcessing::Format, int> VideoProcessing::_formatToCvFormat =
{
	{VideoProcessing::Format::Y8,	CV_8UC1},
	{VideoProcessing::Format::BGR,	CV_8UC3},
	{VideoProcessing::Format::RGB,	CV_8UC3},
	{VideoProcessing::Format::BGRA, CV_8UC3},
	{VideoProcessing::Format::RGBA, CV_8UC3}
};


VideoProcessing::VideoProcessing(Format inputFormat, Format outputFormat)
{
	_inputFormat = inputFormat;
	_outputFormat = outputFormat;
}


void VideoProcessing::process(cv::Mat& input, cv::Mat& output)
{
	cv::Size outputSize = cv::Size(output.cols, output.rows);

	cv::cuda::GpuMat gpuMat(outputSize, _formatToCvFormat[_outputFormat]);

	crop(input, gpuMat);

	if (_inputFormat == Format::BGR && _outputFormat == Format::RGB)
	{
		cv::cuda::cvtColor(gpuMat, gpuMat, cv::COLOR_BGR2RGB);
	}
	else if (_inputFormat == Format::RGB && _outputFormat == Format::BGR)
	{
		cv::cuda::cvtColor(gpuMat, gpuMat, cv::COLOR_RGB2BGR);
	}
	else if (_inputFormat == Format::BGRA && _outputFormat == Format::RGBA)
	{
		cv::cuda::cvtColor(gpuMat, gpuMat, cv::COLOR_BGRA2RGBA);
	}
	else if (_inputFormat == Format::RGBA && _outputFormat == Format::BGRA)
	{
		cv::cuda::cvtColor(gpuMat, gpuMat, cv::COLOR_RGBA2BGRA);
	}

	gpuMat.download(output);
}


void VideoProcessing::crop(const cv::Mat input, cv::cuda::GpuMat& output)
{
	int width_in = input.cols;
	int height_in = input.rows;

	int width_out = output.cols;
	int height_out = output.rows;

	if (width_out < width_in ||
		height_out < height_in)
	{
		int dx = width_in - width_out;
		int dy = height_in - height_out;

		if (dx >= 0 && dy >= 0)
		{
			cv::Rect roi(width_in / 2 - dx / 2, height_in / 2 - dy / 2, width_out, height_out);

			cv::Mat roiImage = input(roi);
			output.upload(roiImage);
		}
	}
	else if (width_out == width_in && height_out == height_in)
	{
		output.upload(input);
	}
	else
	{
		throw QString("Output size cannot be greater than input size");
	}
}