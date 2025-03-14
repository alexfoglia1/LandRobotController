#include "VideoProcessing.h"

#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudawarping.hpp>

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

	_algorithmsEnabled = static_cast<quint32>(VideoProcessing::Algorithm::ALL_OFF);
	_zoomStep = DigitalZoomStep::ZOOM_2X;
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

	if (_algorithmsEnabled & static_cast<quint32>(VideoProcessing::Algorithm::DIGITAL_ZOOM))
	{
		digitalZoom(gpuMat, _zoomStep);
	}

	if (_algorithmsEnabled & static_cast<quint32>(VideoProcessing::Algorithm::BILATERAL_FILTER))
	{
		cv::cuda::bilateralFilter(gpuMat, gpuMat, 9, 75, 75);
	}

	if (_algorithmsEnabled & static_cast<quint32>(VideoProcessing::Algorithm::CLAHE))
	{
		std::vector<cv::cuda::GpuMat> channels(3);
		cv::cuda::split(gpuMat, channels);

		cv::Ptr<cv::cuda::CLAHE> clahe = cv::cuda::createCLAHE(2.0, cv::Size(8, 8));

		clahe->apply(channels[0], channels[0]);
		clahe->apply(channels[1], channels[1]);
		clahe->apply(channels[2], channels[2]);

		cv::cuda::merge(channels, gpuMat);
	}

	if (_algorithmsEnabled & static_cast<quint32>(VideoProcessing::Algorithm::STABILIZATION))
	{
		stabilizeFrame(gpuMat);
	}

	gpuMat.download(output);
}


void VideoProcessing::setAlgorithmEnabled(VideoProcessing::Algorithm algo, bool enabled)
{
	quint32 mask = 0;
	if (enabled)
	{
		mask = static_cast<quint32>(algo);
		_algorithmsEnabled |= mask;
	}
	else
	{
		mask = ~(static_cast<quint32>(algo));
		_algorithmsEnabled &= mask;
	}
}


void VideoProcessing::setDigitalZoomStep(quint8 zoomStep)
{
	if (zoomStep == 0 || zoomStep > static_cast<quint8>(DigitalZoomStep::ZOOM_STEPS))
	{
		setAlgorithmEnabled(VideoProcessing::Algorithm::DIGITAL_ZOOM, false);
	}
	else
	{
		setAlgorithmEnabled(VideoProcessing::Algorithm::DIGITAL_ZOOM, true);

		_zoomStep = DigitalZoomStep(zoomStep);
	}
}


quint32 VideoProcessing::algorithmsEnabled()
{
	return _algorithmsEnabled;
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



void VideoProcessing::stabilizeFrame(cv::cuda::GpuMat& frame)
{
#if 0
	// Converti in scala di grigi
	cv::cuda::GpuMat gray;
	cv::cuda::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

	if (_prevGray.empty())
	{
		_prevGray = gray.clone();
		return;
	}

	// Optical Flow con CUDA (Farneback)
	cv::cuda::GpuMat flowXY;
	cv::Ptr<cv::cuda::FarnebackOpticalFlow> flow = cv::cuda::FarnebackOpticalFlow::create();
	flow->calc(_prevGray, gray, flowXY);
	std::vector<cv::cuda::GpuMat> flowChannels(2);
	cv::cuda::split(flowXY, flowChannels);

	// Scarica in CPU per calcolare lo spostamento medio
	cv::Mat flowX, flowY;
	flowChannels[0].download(flowX);
	flowChannels[1].download(flowY);

	double dx = cv::mean(flowX)[0];  // Spostamento medio in X
	double dy = cv::mean(flowY)[0];  // Spostamento medio in Y

	std::cout << "Shift: (" << dx << ", " << dy << ")\n";

	// Matrice di trasformazione per compensare il movimento
	cv::Mat transform = (cv::Mat_<float>(2, 3) << 1, 0, -dx, 0, 1, -dy);
	cv::cuda::GpuMat stabilized;
	cv::cuda::warpAffine(frame, stabilized, transform, frame.size());

	// Aggiorna il frame stabilizzato
	frame = stabilized.clone();
	_prevGray = gray.clone();
#endif
}