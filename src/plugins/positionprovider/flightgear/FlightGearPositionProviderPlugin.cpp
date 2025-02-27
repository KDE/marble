// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Ralf Habacker <ralf.habacker@freenet.de>
//

#include "FlightGearPositionProviderPlugin.h"

#include <cmath>

#include <QIcon>
#include <QUdpSocket>

using namespace Marble;
/* TRANSLATOR Marble::FlightGearPositionProviderPlugin */

using namespace std;

FlightGearPositionProviderPlugin::FlightGearPositionProviderPlugin() = default;

FlightGearPositionProviderPlugin::~FlightGearPositionProviderPlugin()
{
    delete m_socket;
}

QString FlightGearPositionProviderPlugin::name() const
{
    return tr("FlightGear position provider Plugin");
}

QString FlightGearPositionProviderPlugin::nameId() const
{
    return QStringLiteral("flightgear");
}

QString FlightGearPositionProviderPlugin::guiString() const
{
    return tr("FlightGear");
}

QString FlightGearPositionProviderPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString FlightGearPositionProviderPlugin::description() const
{
    return tr("Reports the position of running flightgear application.");
}

QString FlightGearPositionProviderPlugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QList<PluginAuthor> FlightGearPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Ralf Habacker"), QStringLiteral("ralf.habacker@freenet.de"));
}

QIcon FlightGearPositionProviderPlugin::icon() const
{
    return {};
}

void FlightGearPositionProviderPlugin::initialize()
{
    m_status = PositionProviderStatusAcquiring;
    Q_EMIT statusChanged(m_status);

    m_socket = new QUdpSocket(this);
    m_socket->bind(QHostAddress::Any, 5500);

    connect(m_socket, &QIODevice::readyRead, this, &FlightGearPositionProviderPlugin::readPendingDatagrams);
}

/**
 fixed case where wrong date format is used '2404112' instead of '240412'
*/
bool fixBadGPRMC(QByteArray &line)
{
    if (!line.startsWith("$GPRMC"))
        return false;

    QStringList parts = QString(QString::fromLatin1(line)).split(QLatin1Char(','));
    if (parts[9].size() == 7) {
        parts[9].remove(4, 1);
        line = parts.join(QLatin1Char(',')).toLatin1();
        // update crc
        int crc = 0;
        for (int i = 1; i < line.size() - 3; i++) {
            crc ^= (int)line[i];
        }
        parts[11] = parts[11][0] + parts[11][1] + QString::number(crc, 16).toUpper();

        line = parts.join(QLatin1Char(',')).toLatin1();
        return true;
    }
    return false;
}

void FlightGearPositionProviderPlugin::readPendingDatagrams()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        using Iterator = QList<QByteArray>::Iterator;
        QList<QByteArray> split = datagram.split('\n');
        for (Iterator i = split.begin(); i != split.end(); i++) {
            fixBadGPRMC(*i);
            i->append("\n");
            parseNmeaSentence(QString::fromLatin1(*i));
        }
    }
}

void FlightGearPositionProviderPlugin::parseNmeaSentence(const QString &sentence)
{
    PositionProviderStatus oldStatus = m_status;
    GeoDataCoordinates oldPosition = m_position;

    if (sentence.startsWith(QLatin1StringView("$GPRMC"))) {
        QStringList const values = sentence.split(QLatin1Char(','));
        if (values.size() > 9) {
            if (values[2] == QLatin1StringView("A")) {
                m_speed = values[7].toDouble() * 0.514444; // knots => m/s
                m_track = values[8].toDouble();
                QString const date = values[9] + QLatin1Char(' ') + values[1];
                m_timestamp = QDateTime::fromString(date, QStringLiteral("ddMMyy HHmmss"));
                if (m_timestamp.date().year() <= 1930 && m_timestamp.date().year() >= 1900) {
                    m_timestamp = m_timestamp.addYears(100); // Qt range is 1900-1999 for two-digits
                }
            }
            // Flightgear submits geoposition twice in one datagram, once
            // in GPRMC and once in GPGGA. Parsing one is sufficient
        }
    } else if (sentence.startsWith(QLatin1StringView("$GPGGA"))) {
        QStringList const values = sentence.split(QLatin1Char(','));
        if (values.size() > 10) {
            if (values[6] == nullptr) {
                m_status = PositionProviderStatusAcquiring; // no fix
            } else {
                double const lat = parsePosition(values[2], values[3] == QLatin1StringView("S"));
                double const lon = parsePosition(values[4], values[5] == QLatin1StringView("W"));
                double const unitFactor = values[10] == QLatin1StringView("F") ? FT2M : 1.0;
                double const alt = unitFactor * values[9].toDouble();
                m_position.set(lon, lat, alt, GeoDataCoordinates::Degree);
                m_accuracy.level = GeoDataAccuracy::Detailed;
                m_status = PositionProviderStatusAvailable;
            }
        }
    } else {
        return;
    }

    if (m_status != oldStatus) {
        Q_EMIT statusChanged(m_status);
    }
    if (m_position != oldPosition && m_status == PositionProviderStatusAvailable) {
        Q_EMIT positionChanged(m_position, m_accuracy);
    }
}

double FlightGearPositionProviderPlugin::parsePosition(const QString &value, bool isNegative)
{
    double pos = value.toDouble();
    pos = int(pos / 100.0) + (pos - 100.0 * int(pos / 100.0)) / 60.0;
    return isNegative ? -qAbs(pos) : pos;
}

bool FlightGearPositionProviderPlugin::isInitialized() const
{
    return m_socket;
}

PositionProviderPlugin *FlightGearPositionProviderPlugin::newInstance() const
{
    return new FlightGearPositionProviderPlugin;
}

PositionProviderStatus FlightGearPositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates FlightGearPositionProviderPlugin::position() const
{
    return m_position;
}

GeoDataAccuracy FlightGearPositionProviderPlugin::accuracy() const
{
    return m_accuracy;
}

qreal FlightGearPositionProviderPlugin::speed() const
{
    return m_speed;
}

qreal FlightGearPositionProviderPlugin::direction() const
{
    return m_track;
}

QDateTime FlightGearPositionProviderPlugin::timestamp() const
{
    return m_timestamp;
}

QString FlightGearPositionProviderPlugin::error() const
{
    return {};
}

#include "moc_FlightGearPositionProviderPlugin.cpp"
