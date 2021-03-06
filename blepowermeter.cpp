#include <QLoggingCategory>
#include <QTimer>
#include <QDataStream>
#include <QDebug>

#include <math.h>

#include "blepowermeter.h"

BLEPowerMeter::BLEPowerMeter(QObject *parent)
    : QObject(parent)
    , m_cadence(0.f)
    , m_angle(0.f)
    , m_lastCrankEventTime(0)
    , m_accumulatedCrankRevs(0)
    , m_crankResidue(0)

{
    m_advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    m_advertisingData.setIncludePowerLevel(true);
    m_advertisingData.setLocalName("Monark 828e");
    m_advertisingData.setServices(QList<QBluetoothUuid>() << QBluetoothUuid::CyclingPower);

    m_charData.setUuid(QBluetoothUuid::CyclingPowerMeasurement);
    m_charData.setValue(QByteArray(4, 0));
    m_charData.setProperties(QLowEnergyCharacteristic::Notify);
    const QLowEnergyDescriptorData clientConfig(QBluetoothUuid::ClientCharacteristicConfiguration,
                                                QByteArray(4    , 0));
    m_charData.addDescriptor(clientConfig);

    m_featureChar.setUuid(QBluetoothUuid::CyclingPowerFeature);
    QByteArray value;
    QDataStream ds(&value, QIODevice::ReadWrite);
    ds.setByteOrder(QDataStream::LittleEndian);
    quint32 flags = 0b00000000000100000000000000000000;
    ds << flags;
    m_featureChar.setValue(value);
    m_featureChar.setProperties(QLowEnergyCharacteristic::Read);

    m_sensorLocationChar.setUuid(QBluetoothUuid::SensorLocation);
    m_sensorLocationChar.setValue(QByteArray(1,15)); //rear hub?
    m_sensorLocationChar.setProperties(QLowEnergyCharacteristic::Read);

    m_serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    m_serviceData.setUuid(QBluetoothUuid::CyclingPower);
    m_serviceData.addCharacteristic(m_charData);
    m_serviceData.addCharacteristic(m_featureChar);
    m_serviceData.addCharacteristic(m_sensorLocationChar);

    m_leController.reset(QLowEnergyController::createPeripheral());
    m_service.reset(m_leController->addService(m_serviceData));
    m_leController->startAdvertising(QLowEnergyAdvertisingParameters(), m_advertisingData,
                                   m_advertisingData);

    QObject::connect(m_leController.data(), &QLowEnergyController::disconnected,
                     this, &BLEPowerMeter::reconnect);

    m_boostTimer.setInterval(60 * 60 * 1000); // 60min
    m_boostTimer.setSingleShot(true);
    connect(&m_boostTimer, &QTimer::timeout, this, [&]() {
        setExtraPower(0);
    });
}

float BLEPowerMeter::cadence() const
{
    return m_cadence;
}

float BLEPowerMeter::pendulumAngle() const
{
    return m_angle;
}

int BLEPowerMeter::power() const
{
    return m_power;
}

int BLEPowerMeter::extraPower() const
{
    return m_extraPower;
}

void BLEPowerMeter::reconnect()
{
    m_leController->startAdvertising(QLowEnergyAdvertisingParameters(), m_advertisingData,
                                   m_advertisingData);
}

void BLEPowerMeter::setCadance(const float cadence)
{
    if (m_cadence != cadence) {
        m_cadence = cadence;
        emit cadenceChanged();
    }
}

void BLEPowerMeter::setPower(const int power)
{
    if (m_power != power) {
        m_power = power;
        emit powerChanged();
    }
}

void BLEPowerMeter::setExtraPower(const int extraPower)
{
    if (m_extraPower != extraPower) {
        m_extraPower = extraPower;
        emit extraPowerChanged();

        if (m_extraPower != 0) {
            m_boostTimer.start();
        }
    }
}

void BLEPowerMeter::setPendulumAngle(const float angle)
{
    if (m_angle != angle) {
        m_angle = angle;
        //qDebug() << Q_FUNC_INFO << m_angle;
        emit pendulumAngleChanged();
    }

    if (m_cadence > 0) {
        //qDebug() << m_cadence << m_angle << m_angle * m_cadence << "         " << m_crankResidue << m_lastCrankEventTime << m_accumulatedCrankRevs;
        quint16 power = round(m_angle * m_cadence) + m_extraPower;

        if (power > 1000.f) {
            power = 1000.f;
        }

        setPower(power);

        QByteArray value2;
        QDataStream ds(&value2, QIODevice::ReadWrite);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint16 flags = 0b0000000000100000;

        double newCrankRevs = m_crankResidue + (double)m_cadence/60 * 500/1000;
        m_crankResidue =  newCrankRevs - ((long)newCrankRevs);
        if (m_crankResidue > 1) {
            m_crankResidue = 0;
        }

        quint32 timeUpdate = (double)60/(double)m_cadence * (quint16)newCrankRevs * 1024;

        m_lastCrankEventTime += timeUpdate;
        m_accumulatedCrankRevs += newCrankRevs;

        ds << flags << power << m_accumulatedCrankRevs << m_lastCrankEventTime;
        QLowEnergyCharacteristic measChar = m_service->characteristic(QBluetoothUuid::CyclingPowerMeasurement);
        Q_ASSERT(measChar.isValid());
        m_service->writeCharacteristic(measChar, value2);
    }
}
