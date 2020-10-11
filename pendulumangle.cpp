#include "pendulumangle.h"

#include "adc.h"
#include <QDebug>

PendulumAngle::PendulumAngle(int intervall, QObject *parent)
    : QObject(parent)
    // TODO: Move these values to some environment variable or something like that!
    , kValue(71.5101818548168f)
    , mValue(-40.1898647f)
{
    startTimer(intervall);
}

void PendulumAngle::timerEvent(QTimerEvent *)
{
    double ch1;

    ch1 = read_voltage(0x68, 1, 18, 1, 1);
    float nA = kValue * ch1 + mValue;
    if (nA < 0.f)
        nA = 0;

    emit angel(nA);
}

