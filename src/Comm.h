#ifndef COMM_H
#define COMM_H

#include <stdint.h>
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

typedef struct
{
    int16_t xAxis;
    int16_t yAxis;
    int16_t throttle;
    uint16_t servo;
    uint16_t checksum;
} CtrlMessage;


class Comm : public QObject
{
    Q_OBJECT

public:
    Comm(QString destIp, quint16 destPort);

public slots:
    void setXY(qint16 x, qint16 y);
    void setThrottle(qint16 throttle);
    void setServo(quint16 servo);

    void transmit();



private:
    CtrlMessage _ctrlMessage;
    QUdpSocket _sock;
    QHostAddress _addr;
    quint16 _port;

    quint16 checksum();
};

#endif
