#include "ServoControl.h"

#include <qdatetime.h>


ServoControl::ServoControl(ServoControl::ServoMode servoMode, double kp, double ki, double kd, QObject* parent) : QObject(parent)
{
	_kp = kp;
	_ki = ki;
	_kd = kd;
	_servoMode = servoMode;

	reset();
}


void ServoControl::setMode(ServoControl::ServoMode servoMode)
{
	_servoMode = servoMode;
}

ServoControl::ServoMode ServoControl::servoMode()
{
	return _servoMode;
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
	_error = -static_cast<double>(scartX - _offset);
	_integral += static_cast<double>(_error * dt);

	double P = _kp * _error;
	double I = _ki * _integral;
	double D = _kd * _derivative;

	double U = saturate(P + I + D, -500, 500);

	quint16 servoCmd = 1500 + static_cast<quint16>(U);
	// TODO : Tracker Elevation
	emit updatedServo(static_cast<quint16>(_servoMode), servoCmd, 1500);
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


void ServoControl::servoDispatch(quint16 servoAzi, quint16 servoEle)
{
	if (_lastMillis < 0)
	{
		emit updatedServo(static_cast<quint16>(_servoMode), static_cast<quint16>(joyToServo(servoAzi)), static_cast<quint16>(joyToServo(servoEle)));
	}
	else
	{
		// TODO : Tracker Elevation
		_offset += toRange(servoAzi, 1000, 2000, -1, 1);
	}
}


double ServoControl::joyToServo(quint16 joyIn)
{
	const double coeff[4] = { 2.7389e-06, -1.2324e-02, 1.8825e+01, -8.2519e+03 };

	double x = saturate(joyIn, 1000, 2000);
	double result = coeff[3] + coeff[2] * x + coeff[1] * pow(x, 2) + coeff[0] * pow(x, 3);
	return static_cast<quint16>(saturate(result, 1000, 2000));
}