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
    Q_PROPERTY(float cadence READ cadence WRITE setCadance NOTIFY cadenceChanged)
    Q_PROPERTY(float pendulumAngle READ pendulumAngle WRITE setPendulumAngle NOTIFY pendulumAngleChanged)
    Q_PROPERTY(int power READ power NOTIFY powerChanged)
    Q_PROPERTY(int extraPower READ extraPower WRITE setExtraPower NOTIFY extraPowerChanged)
public:
    explicit BLEPowerMeter(QObject *parent = 0);

    float cadence() const;
    float pendulumAngle() const;
    int power() const;
    int extraPower() const;

public slots:
    void setPendulumAngle(const float angle);
    void setCadance(const float cadence);
    void setPower(const int power);
    void setExtraPower(const int extraPower);

signals:
    void power(float newPower, float newCadance);
    void cadenceChanged();
    void pendulumAngleChanged();
    void powerChanged();
    void extraPowerChanged();

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

    float m_cadence = 0;
    float m_angle = 0;
    int m_power = 0;
    int m_extraPower = 0;
    quint16 m_lastCrankEventTime;
    quint16 m_accumulatedCrankRevs;
    double m_crankResidue;



};

#endif // BLEPOWERMETER_H
