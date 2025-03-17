#include "JoystickBridge.h"
#include "DigitalZoom.h"

#ifdef __linux__
#include <cmath>
#endif

JoystickBridge::JoystickBridge(QObject* parent) : QObject(parent)
{
	_js = QJoysticks::getInstance();

	_x = 0;
	_y = 0;
	_servoData = 1500;
	_throttle = 0;
	_numButtons = 32;
	_zoomStep = 0;
	_buttonsState = new bool[_numButtons];


	memset(&_buttonsState, 0x00, sizeof(bool));

	connect(_js, &QJoysticks::buttonEvent, this, &JoystickBridge::onButtonEvent);
	connect(_js, &QJoysticks::axisEvent, this, &JoystickBridge::onAxisEvent);
}


qreal JoystickBridge::deadCenterZone(qreal axisValue, qreal deadCenter, qreal dczValue, qreal minAxisValue, qreal maxAxisValue)
{
	if (fabs(axisValue) < deadCenter)
	{
		return dczValue;
	}
	else
	{
		double axisSpan = maxAxisValue - minAxisValue;

		if (minAxisValue < 0)
		{
			if (axisValue < 0)
			{
				return mapValue(axisValue, minAxisValue, -deadCenter, 1000.0, 1500.0);
			}
			else
			{
				return mapValue(axisValue, deadCenter, maxAxisValue, 1500.0, 2000.0);
			}
		}
		else
		{
			return mapValue(axisValue, minAxisValue + deadCenter, maxAxisValue, 1000.0, 2000.0);
		}
	}
}


qreal JoystickBridge::saturate(qreal value, qreal min, qreal max)
{
	return value < min ? min : value > max ? max : value;
}


qreal JoystickBridge::mapValue(qreal value, qreal fromMin, qreal fromMax, qreal toMin, qreal toMax)
{
	qreal fromSpan = fromMax - fromMin;
	qreal fromPercentage = ((qreal)value - (qreal)fromMin) / (qreal)fromSpan;

	int toSpan = toMax - toMin;

	return toMin + fromPercentage * toSpan;
}


void JoystickBridge::onButtonEvent(const QJoystickButtonEvent& evt)
{
	printf("button(%d)\n", evt.button);

	if (evt.pressed)
	{
		_buttonsState[evt.button] = !_buttonsState[evt.button];
	}

	switch (evt.button)
	{
	case 0:  if (evt.pressed) { emit emergencyStop(); } break;
	case 1:  if (evt.pressed) { emit stopTracker(); } break;
	case 2:  if (evt.pressed) { emit toggleTracker(); } break;
	case 11: if (evt.pressed) { _zoomStep += 1; _zoomStep %= (static_cast<quint8>(DigitalZoomStep::ZOOM_STEPS) + 1); emit setZoomStep(_zoomStep); } break;
	case 12: if (evt.pressed) { _zoomStep = _zoomStep == 0 ? static_cast<quint8>(DigitalZoomStep::ZOOM_STEPS) : _zoomStep - 1; emit setZoomStep(_zoomStep); } break;
	case 13: emit setImageEnhancementAlgorithm(VideoProcessing::Algorithm::CLAHE, _buttonsState[evt.button]); break;
	case 14: emit setImageEnhancementAlgorithm(VideoProcessing::Algorithm::BILATERAL_FILTER, _buttonsState[evt.button]); break;
	case 9:  emit setImageEnhancementAlgorithm(VideoProcessing::Algorithm::STABILIZATION, _buttonsState[evt.button]); break;
	default: break;
	}

}


void JoystickBridge::onAxisEvent(const QJoystickAxisEvent& evt)
{
	qreal evtValue = saturate(evt.value, -1.0, 1.0);

	bool isValidAxisEvent = (evt.axis == 5 || evt.axis == 2 || evt.axis == 0 || evt.axis == 4 || evt.axis == 3);
	if (isValidAxisEvent)
	{
		qint16 throttle = _throttle;
		qint16 x = _x;
		quint16 servoData = _servoData;

		if (evt.axis == 5)
		{
			if (evtValue < 0) evtValue = 0;
			qreal fSignal = mapValue(evtValue, 0, 1.0, 0, 255);// deadCenterZone(-evtValue, 0.1, 1500.0, -1.0, 1.0);

			_throttle = (quint16)(fSignal);

			if (_throttle != throttle)
				emit updatedThrottle(_throttle);
		}
		else if (evt.axis == 0)
		{
			//printf("evtValue(%f)\t", evtValue);
			qreal fSignal = deadCenterZone(evtValue, 0.25, 1500.0, -1.0, 1.0);
			//printf("after dcz(%f)\t", fSignal);
			fSignal = mapValue(fSignal, 1000.0, 2000.0, -180.0, 180.0);
			//printf("to angle(%f)\t", fSignal);
			_x = (qint16)(fSignal); 
			//printf("_x(%d)\n", _x);

			if (_x != x)
				emit updateXY(_x, _y);
		}
		else if (evt.axis == 2)
		{
			qreal fSignal = deadCenterZone(-evtValue, 0.25, 1500.0, -1.0, 1.0);
			_servoData = (qint16)(fSignal);

			if (_servoData != servoData)
				emit updatedServo(_servoData);
		}
		else if (evt.axis == 4)
		{
			if (evtValue < 0) evtValue = 0;
			qreal fSignal = mapValue(evtValue, 0, 1.0, 0, 255);// deadCenterZone(-evtValue, 0.1, 1500.0, -1.0, 1.0);

			_throttle = (qint16)(fSignal);

			if (_throttle != throttle)
				emit updatedThrottle(-_throttle);
		}

		else if (evt.axis == 3)
		{
			if (evtValue == -1) emit reduceTrackerRoi();
			if (evtValue == 1) emit enlargeTrackerRoi();
		}
	}
}
