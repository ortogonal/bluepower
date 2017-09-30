#ifndef BLEPOWERMETER_H
#define BLEPOWERMETER_H

#include <QObject>
#include <QScopedPointer>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>

class BLEPowerMeter : public QObject
{
    Q_OBJECT
public:
    explicit BLEPowerMeter(QObject *parent = 0);

signals:
    void power(float newPower, float newCadance);

public slots:
    void setPendulumAngle(const float angle);
    void setCadance(const float cadence);

private slots:
    void reconnect();

private:
    QScopedPointer<QLowEnergyService> m_service;
    QLowEnergyAdvertisingData m_advertisingData;
    QLowEnergyCharacteristicData m_charData;
    QLowEnergyCharacteristicData m_featureChar;
    QLowEnergyCharacteristicData m_sensorLocationChar;
    QLowEnergyServiceData m_serviceData;
    QScopedPointer<QLowEnergyController> m_leController;

    float m_cadence;
    float m_angle;
    quint16 m_lastCrankEventTime;
    quint16 m_accumulatedCrankRevs;
    double m_crankResidue;



};

#endif // BLEPOWERMETER_H
