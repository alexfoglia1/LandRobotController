#include "Tracker.h"

#include <cuda_runtime.h>

int Tracker::DEFAULT_ROI_HEIGHT = 200;
int Tracker::DEFAULT_ROI_WIDTH  = 200;
int Tracker::RMS_CONTRAST_VALID_THRESHOLD = 20;
float Tracker::TEMPLATE_CORR_THRESHOLD = 0.15f;
int Tracker::MAX_SCART_X = 100;
int Tracker::MAX_SCART_Y = 100;
int Tracker::MAX_COAST_TIME = 50;

Tracker::Tracker() : QThread(nullptr)
{
	_target.cx = 0;
	_target.cy = 0;
	_target.width = 0;
	_target.height = 0;
	_target.valid = false;

	_roiWidth = DEFAULT_ROI_WIDTH;
	_roiHeight = DEFAULT_ROI_HEIGHT;

	_state = Tracker::State::IDLE;
	_target.state = _state;
	_countCoast = 0;

	_templMatch = cv::cuda::createTemplateMatching(CV_8UC1, cv::TM_CCOEFF_NORMED);
	_lastDisplacement = cv::Point(0, 0);

	_synthTarget = cv::Mat(1080, 1920, CV_8UC1);
	memset(_synthTarget.ptr<uchar3>(), 0x00, 1920 * 1080);
	_synthTargetSize = cv::Size(50, 50);
	_synthTargetCoord = cv::Point(1920 / 2 - _synthTargetSize.width/2, 2 * 1080 / 3);
	_synthTargetWindowExposed = false;
}



void Tracker::setRoi(int width, int height)
{
	_roiWidth = width;
	_roiHeight = height;
}


void Tracker::enlargeRoi(int dxdy)
{
	Tracker::State trackerState = state();

	if (trackerState == Tracker::State::IDLE ||
		trackerState == Tracker::State::ACQUIRE)
	{
		if (_roiWidth < _lastFrame.cols - dxdy &&
			_roiHeight < _lastFrame.rows - dxdy)
		{
			setRoi(_roiWidth + dxdy, _roiHeight + dxdy);
		}
	}
}


void Tracker::reduceRoi(int dxdy)
{
	Tracker::State trackerState = state();

	if (trackerState == Tracker::State::IDLE ||
		trackerState == Tracker::State::ACQUIRE)
	{
		if (_roiWidth > dxdy &&
			_roiHeight > dxdy)
		{
			setRoi(_roiWidth - dxdy, _roiHeight - dxdy);
		}
	}
}


void Tracker::updateFrame(cv::Mat& frame)
{
	cv::cuda::GpuMat localFrame(frame.cols, frame.rows, frame.type());
	localFrame.upload(frame);

	if (localFrame.type() == CV_8UC3)
	{
		cv::cuda::cvtColor(localFrame, localFrame, cv::COLOR_BGR2GRAY);
	}
	else if (localFrame.type() != CV_8UC1)
	{
		return;
	}

	localFrame.copyTo(_lastFrame);
	_frameSem.release();
}


void Tracker::setState(Tracker::State state)
{
	_stateMutex.lock();
	_state = state;
	_stateMutex.unlock();
}


Tracker::State Tracker::state()
{
	State state;

	_stateMutex.lock();
	state = _state;
	_stateMutex.unlock();

	return state;
}


Tracker::Target Tracker::target()
{
	Tracker::Target target;

	_targetMutex.lock();
	target = { _target.cx, _target.cy, _target.scartX, _target.scartY, _target.width, _target.height, _target.valid, _target.correlation, _target.contrastIdx, _target.state };
	_targetMutex.unlock();

	return target;
}


void Tracker::run()
{
	while (true)
	{
		Tracker::State currentState = state();
		
		if (currentState != Tracker::State::IDLE)
		{
			if (!_lastFrame.empty())
			{
				_frameSem.acquire();

				if (_prevFrame.empty())
				{
					_prevFrame = cv::cuda::GpuMat(_lastFrame.cols, _lastFrame.rows, _lastFrame.type());
					_lastFrame.copyTo(_prevFrame);
				}
				else
				{
					cv::cuda::GpuMat localFrame = cv::cuda::GpuMat(_lastFrame.cols, _lastFrame.rows, _lastFrame.type());
					_lastFrame.copyTo(localFrame);

					process(currentState, localFrame);

					localFrame.copyTo(_prevFrame);
				}
			}
		}
		else
		{
			if (_synthTargetWindowExposed)
			{
				cv::destroyWindow("Synth Target");
				_synthTargetWindowExposed = false;
			}
			emit trackerIdle();
			msleep(10);
		}
	}
}


void Tracker::process(Tracker::State state, cv::cuda::GpuMat& lastFrame)
{
	if (state == Tracker::State::IDLE)
	{
		return;
	}
	else if (state == Tracker::State::ACQUIRE)
	{
		acquire(lastFrame);
	}
	else if (state == Tracker::State::TRACK)
	{
		track(lastFrame);
	}
	else if (state == Tracker::State::COAST)
	{
		coast(lastFrame);
	}

	updateSynthTarget();
}


void Tracker::acquire(cv::cuda::GpuMat& lastFrame)
{
	_countCoast = 0;
	_lastDisplacement.x = 0;
	_lastDisplacement.y = 0;

	// Center roi at image center
	int roiX = (lastFrame.cols / 2) - (_roiWidth / 2);
	int roiY = (lastFrame.rows / 2) - (_roiHeight / 2);

	_template = lastFrame(cv::Rect(roiX, roiY, _roiWidth, _roiHeight));

	cv::Scalar mean, stddev;
	cv::cuda::meanStdDev(_template, mean, stddev);
	double rms_contrast = stddev[0];

	_targetMutex.lock();

	_target.state = Tracker::State::ACQUIRE;

	_target.cx = roiX + _roiWidth / 2;
	_target.cy = roiY + _roiHeight / 2;
	_target.width = _roiWidth;
	_target.height = _roiHeight;
	_target.valid = rms_contrast > RMS_CONTRAST_VALID_THRESHOLD;
	_target.correlation = 0.0;
	_target.contrastIdx = rms_contrast;

	_targetMutex.unlock();

	emit acquireDone();
}


void Tracker::coreTracker(cv::cuda::GpuMat& lastFrame, Tracker::State trackerState, int maxScartX, int maxScartY, float correlationThreshold)
{
	// Matrice per il risultato del match
	cv::cuda::GpuMat d_result;
	int result_cols = lastFrame.cols - _template.cols + 1;
	int result_rows = lastFrame.rows - _template.rows + 1;
	d_result.create(result_rows, result_cols, CV_32FC1); // Il risultato è in float

	// Effettua il template matching sulla GPU
	_templMatch->match(lastFrame, _template, d_result);

	// Scarica il risultato sulla CPU per analizzare il miglior match
	cv::Mat h_result;
	d_result.download(h_result);

	// Trova il miglior match
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(h_result, &minVal, &maxVal, &minLoc, &maxLoc);

	// Lo spostamento del template dal frame0 a frame1
	cv::Point originalPosition(_target.cx - _target.width / 2, _target.cy - _target.height / 2);  // Posizione originale del template in frame0
	cv::Point displacement = maxLoc - originalPosition;

	// Center roi at tracker target
	int roiX = (_target.cx + displacement.x) - (_target.width / 2);
	int roiY = (_target.cy + displacement.y) - (_target.height / 2);

	cv::cuda::GpuMat matchedTemplate = lastFrame(cv::Rect(roiX, roiY, _roiWidth, _roiHeight));
	
	cv::Scalar mean, stddev;
	cv::cuda::meanStdDev(matchedTemplate, mean, stddev);
	double rms_contrast = stddev[0];

	double correlation = computeCorrelation(matchedTemplate);

	_targetMutex.lock();
	_target.state = trackerState;
	_target.correlation = correlation;
	_target.contrastIdx = rms_contrast;


	if (correlation > correlationThreshold &&
		displacement.x <= maxScartX &&
		displacement.y <= maxScartY &&
		rms_contrast > RMS_CONTRAST_VALID_THRESHOLD)
	{
		_template = matchedTemplate;

		_target.cx += displacement.x;
		_target.cy += displacement.y;

		_lastDisplacement.x = displacement.x;
		_lastDisplacement.y = displacement.y;

		_target.scartX = _target.cx - lastFrame.cols / 2;
		_target.scartY = _target.cy - lastFrame.rows / 2;

		if (trackerState == Tracker::State::COAST)
		{
			setState(Tracker::State::TRACK);
		}
	}
	else
	{
		// Assume lastDisplacement valid
		//_target.cx += _lastDisplacement.x;
		//_target.cy += _lastDisplacement.y;

		//_target.scartX = _target.cx - lastFrame.cols / 2;
		//_target.scartY = _target.cy - lastFrame.rows / 2;

		if (trackerState == Tracker::State::TRACK)
		{
			setState(Tracker::State::COAST);
		}
	}
	_targetMutex.unlock();

	emit targetMoved();
}


void Tracker::track(cv::cuda::GpuMat& lastFrame)
{
	_countCoast = 0;
	coreTracker(lastFrame, state(), MAX_SCART_X, MAX_SCART_Y, TEMPLATE_CORR_THRESHOLD);
}


void Tracker::coast(cv::cuda::GpuMat& lastFrame)
{
	_countCoast += 1;

	if (_countCoast < MAX_COAST_TIME)
	{
		coreTracker(lastFrame, state(), 4 * MAX_SCART_X / 5, 4 * MAX_SCART_Y / 5, 9.0f * TEMPLATE_CORR_THRESHOLD / 8.0f);
	}
	else
	{
		emit coastingFailure();
		setState(State::ACQUIRE);
	}
}


double Tracker::computeCorrelation(const cv::cuda::GpuMat& matchedTemplate)
{
	// Converti a float per evitare problemi di precisione
	cv::cuda::GpuMat matchedTemplate32f, template32f;
	matchedTemplate.convertTo(matchedTemplate32f, CV_32F);
	_template.convertTo(template32f, CV_32F);

	// Calcola le medie
	cv::Scalar mean1, stddev1, mean2, stddev2;
	cv::cuda::meanStdDev(matchedTemplate32f, mean1, stddev1);
	cv::cuda::meanStdDev(template32f, mean2, stddev2);

	// Sottrai le medie per ottenere A' e B'
	cv::cuda::GpuMat matchedTemplate32fNorm, template32fNorm;
	cv::cuda::subtract(matchedTemplate32f, mean1, matchedTemplate32fNorm);
	cv::cuda::subtract(template32f, mean2, template32fNorm);

	// Calcola il numeratore: somma(A' * B')
	cv::cuda::GpuMat templMul;
	cv::cuda::multiply(matchedTemplate32fNorm, template32fNorm, templMul);
	double num = cv::cuda::sum(templMul)[0];

	// Calcola i denominatori: sqrt(somma(A'^2) * somma(B'^2))
	cv::cuda::GpuMat d_sq1, d_sq2;
	cv::cuda::multiply(matchedTemplate32f, matchedTemplate32f, d_sq1);
	cv::cuda::multiply(template32f, template32f, d_sq2);
	double denom = sqrt(cv::cuda::sum(d_sq1)[0] * cv::cuda::sum(d_sq2)[0]);

	return denom > 0 ? num / denom : 0.0; // Se il denominatore è 0, ritorna 0 per evitare NaN
}


void Tracker::updateSynthTarget()
{
	static int direction = 1;
	if (_synthTargetCoord.x > _synthTarget.cols - _synthTargetSize.width && direction > 0)
	{
		direction = -1;
	}
	else if (_synthTargetCoord.x < _synthTargetSize.width && direction < 0)
	{
		direction = 1;
	}

	cv::rectangle(_synthTarget, cv::Rect(_synthTargetCoord.x, _synthTargetCoord.y, _synthTargetSize.width, _synthTargetSize.height), cv::Scalar(0), cv::FILLED);

	_synthTargetCoord +=  cv::Point(direction * 5, 0);

	cv::rectangle(_synthTarget, cv::Rect(_synthTargetCoord.x, _synthTargetCoord.y, _synthTargetSize.width, _synthTargetSize.height),
		_synthTargetCoord.x >= 400 && _synthTargetCoord.x <= 600 ? cv::Scalar(0) : cv::Scalar(255),
		cv::FILLED);

	cv::imshow("Synth Target", _synthTarget);
	_synthTargetWindowExposed = true;

	cv::waitKey(1);
}