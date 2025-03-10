#ifndef ROBOTDATA_H
#define ROBOTDATA_H

#include <QGlobal.h>

class RobotData
{
public:
	RobotData();

	inline void setGyroX(float gyroX);
	inline void setGyroY(float gyroY);
	inline void setGyroZ(float gyroZ);
	inline void setAccX(float accX);
	inline void setAccY(float accY);
	inline void setAccZ(float accZ);
	inline void setGyroZSp(qint16 gyroZSp);
	inline void setPidP(float pidP);
	inline void setPidI(float pidI);
	inline void setPidD(float pidD);
	inline void setPidU(float pidU);
	inline void setThrL(quint16 thrL);
	inline void setThrR(quint16 thrR);
	inline void setServo(quint16 servo);

	inline float getGyroX() const;
	inline float getGyroY() const;
	inline float getGyroZ() const;
	inline float getAccX() const;
	inline float getAccY() const;
	inline float getAccZ() const;
	inline qint16 getGyroZSp() const;
	inline float getPidP() const;
	inline float getPidI() const;
	inline float getPidD() const;
	inline float getPidU() const;
	inline quint16 getThrL() const;
	inline quint16 getThrR() const;
	inline quint16 getServo() const;

private:
	float _gyroX;
	float _gyroY;
	float _gyroZ;
	float _accX;
	float _accY;
	float _accZ;
	qint16 _gyroZSp;
	float _pidP;
	float _pidI;
	float _pidD;
	float _pidU;
	quint16 _thrL;
	quint16 _thrR;
	quint16 _servo;
};

inline void RobotData::setGyroX(float gyroX) { _gyroX = gyroX; }
inline void RobotData::setGyroY(float gyroY) { _gyroY = gyroY; }
inline void RobotData::setGyroZ(float gyroZ) { _gyroZ = gyroZ; }
inline void RobotData::setAccX(float accX) { _accX = accX; }
inline void RobotData::setAccY(float accY) { _accY = accY; }
inline void RobotData::setAccZ(float accZ) { _accZ = accZ; }
inline void RobotData::setGyroZSp(qint16 gyroZSp) { _gyroZSp = gyroZSp; }
inline void RobotData::setPidP(float pidP) { _pidP = pidP; }
inline void RobotData::setPidI(float pidI) { _pidI = pidI; }
inline void RobotData::setPidD(float pidD) { _pidD = pidD; }
inline void RobotData::setPidU(float pidU) { _pidU = pidU; }
inline void RobotData::setThrL(quint16 thrL) { _thrL = thrL; }
inline void RobotData::setThrR(quint16 thrR) { _thrR = thrR; }
inline void RobotData::setServo(quint16 servo) { _servo = servo; }

inline float RobotData::getGyroX() const { return _gyroX; }
inline float RobotData::getGyroY() const { return _gyroY; }
inline float RobotData::getGyroZ() const  { return _gyroZ; }
inline float RobotData::getAccX() const { return _accX; }
inline float RobotData::getAccY() const { return _accY; }
inline float RobotData::getAccZ() const { return _accZ; }
inline qint16 RobotData::getGyroZSp() const { return _gyroZSp; }
inline float RobotData::getPidP() const { return _pidP; }
inline float RobotData::getPidI() const { return _pidI; }
inline float RobotData::getPidD() const { return _pidD; }
inline float RobotData::getPidU() const { return _pidU; }
inline quint16 RobotData::getThrL() const { return _thrL; }
inline quint16 RobotData::getThrR() const { return _thrR; }
inline quint16 RobotData::getServo() const { return _servo; }

#endif
