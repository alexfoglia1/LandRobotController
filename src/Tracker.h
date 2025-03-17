#ifndef TRACKER_H
#define TRACKER_H

#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/cudawarping.hpp>

#include <qthread.h>
#include <qmutex.h>
#include <qsemaphore.h>

class Tracker : public QThread
{
	Q_OBJECT
public:
	static int DEFAULT_ROI_WIDTH;
	static int DEFAULT_ROI_HEIGHT;
	static int RMS_CONTRAST_VALID_THRESHOLD;
	static float TEMPLATE_CORR_THRESHOLD;
	static int MAX_SCART_X;
	static int MAX_SCART_Y;
	static int MAX_COAST_TIME;

	enum class State
	{
		IDLE = 0,
		ACQUIRE,
		TRACK,
		COAST
	};

	struct Target
	{
		int cx;
		int cy;
		int scartX;
		int scartY;
		int width;
		int height;
		bool valid;
		double correlation;
		double contrastIdx;
		State state;
	};

	Tracker();

	void setRoi(int width, int height);
	void enlargeRoi(int dxdy);
	void reduceRoi(int dxdy);
	void setState(State state);
	void updateFrame(cv::Mat& frame);
	Target target();

	State state();

signals:
	void trackerIdle();
	void acquireDone();
	void targetMoved();
	void coastingFailure();

protected:
	void run() override;	

private:
	QSemaphore _frameSem;
	QMutex _stateMutex;
	QMutex _targetMutex;

	cv::cuda::GpuMat _prevFrame;
	cv::cuda::GpuMat _lastFrame;
	cv::cuda::GpuMat _template;
	cv::Ptr<cv::cuda::TemplateMatching> _templMatch;
	Target _target;
	int _roiWidth;
	int _roiHeight;
	State _state;
	cv::Point _lastDisplacement;
	int _countCoast;
	bool _synthTargetWindowExposed;

	cv::Mat _synthTarget;
	cv::Point _synthTargetCoord;
	cv::Size _synthTargetSize;

	void process(State state, cv::cuda::GpuMat& frame);
	void acquire(cv::cuda::GpuMat& frame);
	void track(cv::cuda::GpuMat& frame);
	void coast(cv::cuda::GpuMat& frame);
	void coreTracker(cv::cuda::GpuMat& frame, Tracker::State trackerState, int maxScartX, int maxScartY, float correlationThreshold);
	void updateSynthTarget();

	double computeCorrelation(const cv::cuda::GpuMat& matchedTemplate);

};

#endif
