#ifndef CADENCEIO_H
#define CADENCEIO_H

#include <QObject>
#include <QThread>
#include <QTime>

class CadenceIO : public QThread
{
    Q_OBJECT
public:
    explicit CadenceIO(QObject *parent = 0);

    void exportGpio(int gpio);
    void setDirection();
    int gpioOpen(unsigned int gpio);

    void run() override;

signals:
    void cadence(float c);

public slots:

private:
    int fd;
    QTime m_int;
};

#endif // CADENCEIO_H
