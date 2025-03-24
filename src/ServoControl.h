#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

#include <QObject.h>

class ServoControl : public QObject
{
	Q_OBJECT

public:
	enum class ServoMode
	{
		POSITION = 0x00,
		VELOCITY
	};

	ServoControl(ServoMode servoMode, double kp, double ki, double kd, QObject* parent=nullptr);

	void setMode(ServoMode servoMode);
	ServoMode servoMode();

	void setKp(double kp);
	void setKi(double ki);
	void setKd(double kd);

signals:
	void updatedServo(quint16 servoMode, quint16 servoAzi, quint16 servoEle);

public slots:
	void reset();
	void targetMoved(int scartX, int scartY);
	void servoDispatch(quint16 servoAzi, quint16 servoEle);

private:
	double _kp;
	double _ki;
	double _kd;
	double _error;
	double _derivative;
	double _integral;
	double _offset;
	ServoMode _servoMode;

	qint64 _lastMillis;

	double saturate(double in, double outMin, double outMax);
	double toRange(double in, double inMin, double inMax, double outMin, double outMax);

	double joyToServo(quint16 joyIn);

};

#endif
