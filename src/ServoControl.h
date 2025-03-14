#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

#include <QObject.h>

class ServoControl : public QObject
{
	Q_OBJECT

public:
	ServoControl(double kp, double ki, double kd, QObject* parent=nullptr);

	void setKp(double kp);
	void setKi(double ki);
	void setKd(double kd);


signals:
	void updatedServo(quint16 servo);

public slots:
	void reset();
	void targetMoved(int scartX, int scartY);
	void servoDispatch(quint16 servo);

private:
	double _kp;
	double _ki;
	double _kd;
	double _error;
	double _derivative;
	double _integral;
	double _offset;

	qint64 _lastMillis;

	double saturate(double in, double outMin, double outMax);
	double toRange(double in, double inMin, double inMax, double outMin, double outMax);

};

#endif
