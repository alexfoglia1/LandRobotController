#include "ServoControl.h"

#include <qdatetime.h>


ServoControl::ServoControl(double kp, double ki, double kd, QObject* parent) : QObject(parent)
{
	_kp = kp;
	_ki = ki;
	_kd = kd;

	reset();
}

void ServoControl::setKp(double kp)
{
	_kp = kp;
}

void ServoControl::setKi(double ki)
{
	_ki = ki;
}


void ServoControl::setKd(double kd)
{
	_kd = kd;
}

void ServoControl::reset()
{
	_offset = 0.0;
	_error = 0.0;
	_integral = 0.0;
	_derivative = 0.0;

	_lastMillis = -1;
}


void ServoControl::targetMoved(int scartX, int scartY)
{
	Q_UNUSED(scartY); // BEH PER ORA

	qint64 now = QDateTime::currentMSecsSinceEpoch();

	if (_lastMillis < 0)
	{
		_lastMillis = now;
		return;
	}

	double dt = static_cast<double>(now - _lastMillis) * 1e-3;

	_derivative = (static_cast<double>(scartX) - _error) / dt;
	_error = -static_cast<double>(scartX + _offset);
	_integral += static_cast<double>(_error * dt);

	double P = _kp * _error;
	double I = _ki * _integral;
	double D = _kd * _derivative;

	double U = saturate(P + I + D, -500, 500);

	quint16 servoCmd = 1500 + static_cast<quint16>(U);
	emit updatedServo(servoCmd);
}


double ServoControl::saturate(double in, double outMin, double outMax)
{
	return in < outMin ? outMin : in > outMax ? outMax : in;
}


double ServoControl::toRange(double in, double inMin, double inMax, double outMin, double outMax)
{
	double inSpan = inMax - inMin;
	double inPercentage = (in - inMin) / inSpan;

	int outSpan = outMax - outMin;

	return outMin + inPercentage * outSpan;
}


void ServoControl::servoDispatch(quint16 servo)
{
	if (_lastMillis < 0)
	{
		emit updatedServo(servo);
	}
	else
	{
		_offset = toRange(servo, 1000, 2000, -50, 50);
	}
}