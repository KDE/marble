// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012, 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "RoutingRunner.h"

#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "MarbleGlobal.h"
#include "MarbleLocale.h"

#include <QString>
#include <QTime>
#include <QVariant>

namespace Marble
{

RoutingRunner::RoutingRunner(QObject *parent)
    : QObject(parent)
{
}

const QString RoutingRunner::lengthString(qreal length) const
{
    MarbleLocale::MeasurementSystem const measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();

    int precision = 0;
    QString distanceUnit = tr("m");

    switch (measurementSystem) {
    case MarbleLocale::ImperialSystem:
        precision = 1;
        distanceUnit = tr("mi");
        length *= METER2KM * KM2MI;
        if (length < 0.1) {
            length = 10 * qRound(length * 528);
            precision = 0;
            distanceUnit = tr("ft");
        }
        break;
    case MarbleLocale::MetricSystem:
        if (length >= 1000) {
            length *= METER2KM;
            distanceUnit = tr("km");
            precision = 1;
        } else if (length >= 200) {
            length = 50 * qRound(length / 50);
        } else if (length >= 100) {
            length = 25 * qRound(length / 25);
        } else {
            length = 10 * qRound(length / 10);
        }
        break;
    case MarbleLocale::NauticalSystem: {
        length *= METER2KM * KM2NM;
        distanceUnit = tr("nm");
        precision = length < 2.0 ? 2 : 1;
    } break;
    }

    return QStringLiteral("%1 %2").arg(length, 0, 'f', precision).arg(distanceUnit);
}

const QString RoutingRunner::durationString(const QTime &duration) const
{
    const QString hoursString = duration.toString("hh");
    const QString minutesString = duration.toString("mm");
    const QString timeString = tr("%1:%2 h", "journey duration").arg(hoursString, minutesString);
    return timeString;
}

const QString RoutingRunner::nameString(const QString &name, qreal length, const QTime &duration) const
{
    const QString result = QStringLiteral("%1; %2 (%3)");
    return result.arg(lengthString(length), durationString(duration), name);
}

const GeoDataExtendedData RoutingRunner::routeData(qreal length, const QTime &duration) const
{
    GeoDataExtendedData result;
    GeoDataData lengthData;
    lengthData.setName(QStringLiteral("length"));
    lengthData.setValue(length);
    result.addValue(lengthData);
    GeoDataData durationData;
    durationData.setName(QStringLiteral("duration"));
    durationData.setValue(duration.toString(Qt::ISODate));
    result.addValue(durationData);
    return result;
}

}

#include "moc_RoutingRunner.cpp"
