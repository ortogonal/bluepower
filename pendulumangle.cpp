#include "pendulumangle.h"

#include "adc.h"
#include <QTime>
#include <QDebug>

PendulumAngle::PendulumAngle(int intervall, QObject *parent)
    : QObject(parent)
    // TODO: Move these values to some environment variable or something like that!
    , kValue(71.312434036f)
    , mValue(-39.0069600936f)
{
    startTimer(intervall);
}

void PendulumAngle::timerEvent(QTimerEvent *)
{
    double ch1;

    QTime myTimer;
    myTimer.start();
    ch1 = read_voltage(0x68, 1, 18, 1, 1);

    int nMilliseconds = myTimer.elapsed();

    float nA = kValue * ch1 + mValue;
    if (nA < 0.f)
        nA = 0;

    emit angel(nA);
}

