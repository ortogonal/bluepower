#ifndef PENDULUMANGLE_H
#define PENDULUMANGLE_H

#include <QObject>

class PendulumAngle : public QObject
{
    Q_OBJECT
public:
    explicit PendulumAngle(int intervall = 250, QObject *parent = 0);

signals:
    void angel(float newAngle);

public slots:

protected:
    void timerEvent(QTimerEvent *event);

private:
    float kValue;
    float mValue;
};

#endif // PENDULUMANGLE_H
