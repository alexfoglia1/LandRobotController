#include "Comm.h"



Comm::Comm(QString destIp, quint16 destPort)
{
	_addr = QHostAddress(destIp);
	_port = destPort;

	memset(&_ctrlMessage, 0x00, sizeof(CtrlMessage));
}

void Comm::setXY(qint16 x, qint16 y)
{
	_ctrlMessage.xAxis = x;
	_ctrlMessage.yAxis = y;

	transmit();
}

void Comm::setThrottle(qint16 throttle)
{
	_ctrlMessage.throttle = throttle;

	transmit();
}


void Comm::setServo(quint16 servo)
{
	_ctrlMessage.servo = servo;

	transmit();
}


void Comm::transmit()
{
	_ctrlMessage.checksum = checksum();

	QByteArray qba(static_cast<int>(sizeof(CtrlMessage)), 0x00);
	memcpy(qba.data(), &_ctrlMessage, sizeof(CtrlMessage));

	_sock.writeDatagram(qba, _addr, _port);
}

quint16 Comm::checksum()
{
	quint8* pCtrlMessage = reinterpret_cast<quint8*>(&_ctrlMessage);

	quint8 cks = 0;
	for (size_t i = 0; i < sizeof(CtrlMessage) - sizeof(quint16); i += 1)
	{
		cks ^= pCtrlMessage[i];
	}

	return cks;
}