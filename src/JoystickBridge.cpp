#include "JoystickBridge.h"

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
	//printf("%d\n", evt.button);

	if (evt.button == 0 && evt.pressed)
	{
		emit emergencyStop();
	}
	else if (evt.button == 13 && evt.pressed)
	{
		if (_servoData <= 1800)
		{
			_servoData += 200;
			emit updatedServo(_servoData);
		}
	}
	else if (evt.button == 14 && evt.pressed)
	{
		if (_servoData >= 1200)
		{
			_servoData -= 200;
			emit updatedServo(_servoData);
		}
	}
}


void JoystickBridge::onAxisEvent(const QJoystickAxisEvent& evt)
{
	qreal evtValue = saturate(evt.value, -1.0, 1.0);
	//printf("evt.axis(%d)\n", evt.axis);

	bool isValidAxisEvent = (evt.axis == 5 || evt.axis == 2 || evt.axis == 0 || evt.axis == 4);
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
			qreal fSignal = deadCenterZone(evtValue, 0.1, 1500.0, -1.0, 1.0);
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
			qreal fSignal = deadCenterZone(-evtValue, 0.1, 1500.0, -1.0, 1.0);
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
	}
}
