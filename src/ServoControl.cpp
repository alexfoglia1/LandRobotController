#include "ServoControl.h"

#include <qdatetime.h>


ServoControl::ServoControl(ServoControl::ServoMode servoMode, double kpAzi, double kiAzi, double kdAzi, double kpEle, double kiEle, double kdEle, QObject* parent) : QObject(parent)
{
	_kpAzi = kpAzi;
	_kiAzi = kiAzi;
	_kdAzi = kdAzi;
	_kpEle = kpEle;
	_kiEle = kiEle;
	_kdEle = kdEle;
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

void ServoControl::setKpAzi(double kp)
{
	_kpAzi = kp;
}

void ServoControl::setKiAzi(double ki)
{
	_kiAzi = ki;
}


void ServoControl::setKdAzi(double kd)
{
	_kdAzi = kd;
}


void ServoControl::setKpEle(double kp)
{
	_kpEle = kp;
}

void ServoControl::setKiEle(double ki)
{
	_kiEle = ki;
}


void ServoControl::setKdEle(double kd)
{
	_kdEle = kd;
}


void ServoControl::reset()
{
	_offsetAzi = 0.0;
	_errorAzi = 0.0;
	_integralAzi = 0.0;
	_derivativeAzi = 0.0;

	_offsetEle = 0.0;
	_errorEle = 0.0;
	_integralEle = 0.0;
	_derivativeEle = 0.0;

	_lastMillis = -1;
}


double ServoControl::pidController(double y, double ysp, double& err, double& integral, double& derivative, double offset, double kp, double ki, double kd, double dt)
{
	double err_k = (ysp - y);
	
	derivative = (err_k - err) / dt;
	err = err_k;
	integral += (err_k * dt);

	double P = kp * err_k;
	double I = ki * integral;
	double D = kd * derivative;

	double U = saturate(P + I + D, -500, 500);

	return U;
}


void ServoControl::targetMoved(int scartX, int scartY)
{
	qint64 now = QDateTime::currentMSecsSinceEpoch();

	if (_lastMillis < 0)
	{
		_lastMillis = now;
		return;
	}
	double dt = static_cast<double>(now - _lastMillis) * 1e-3;

	double pidAzi = pidController(scartX, 0, _errorAzi, _integralAzi, _derivativeAzi, _offsetAzi, _kpAzi, _kiAzi, _kdEle, dt);
	double pidEle = pidController(scartY, 0, _errorEle, _integralEle, _derivativeEle, _offsetEle, _kpEle, _kiAzi, _kdEle, dt);

	quint16 servoCmdAzi = 1500 + static_cast<quint16>(pidAzi);
	quint16 servoCmdEle = 1500 + static_cast<quint16>(pidEle);
	// TODO : Tracker Elevation
	emit updatedServo(static_cast<quint16>(_servoMode), servoCmdAzi, servoCmdEle);
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
		_offsetAzi += toRange(servoAzi, 1000, 2000, -1, 1);
		_offsetEle += toRange(servoEle, 1000, 2000, -1, 1);
	}
}


double ServoControl::joyToServo(quint16 joyIn)
{
	const double coeff[4] = { 2.7389e-06, -1.2324e-02, 1.8825e+01, -8.2519e+03 };

	double x = saturate(joyIn, 1000, 2000);
	double result = coeff[3] + coeff[2] * x + coeff[1] * pow(x, 2) + coeff[0] * pow(x, 3);
	return static_cast<quint16>(saturate(result, 1000, 2000));
}