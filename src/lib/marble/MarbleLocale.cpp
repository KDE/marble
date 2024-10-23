// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2015 Ilya Kowalewski <illya.kovalevskyy@gmail.com>
//

#include "MarbleLocale.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "MarbleLocale_p.h"

// Qt
#include <QLocale>

namespace Marble
{

MarbleLocalePrivate::MarbleLocalePrivate()
    : m_measurementSystem(MarbleLocale::MetricSystem)
{
}

MarbleLocalePrivate::~MarbleLocalePrivate() = default;

MarbleLocale::MarbleLocale()
    : d(new MarbleLocalePrivate)
{
}

MarbleLocale::~MarbleLocale()
{
    delete d;
}

void MarbleLocale::setMeasurementSystem(MarbleLocale::MeasurementSystem measurementSystem)
{
    d->m_measurementSystem = measurementSystem;
}

MarbleLocale::MeasurementSystem MarbleLocale::measurementSystem() const
{
    return d->m_measurementSystem;
}

void MarbleLocale::meterToTargetUnit(qreal meters, MeasurementSystem targetSystem, qreal &targetValue, MeasureUnit &targetUnit)
{
    targetValue = meters;

    switch (targetSystem) {
    case MetricSystem: {
        if (targetValue > 1000.0) {
            targetValue *= METER2KM;
            targetUnit = Kilometer;
            break;
        }

        if (targetValue < 1.0 && targetValue >= 0.01) {
            targetValue *= M2CM;
            targetUnit = Centimeter;
            break;
        }

        if (targetValue < 0.01) {
            targetValue *= M2MM;
            targetUnit = Milimeter;
            break;
        }

        targetUnit = Meter;
        break;
    }

    case ImperialSystem: {
        // meters into feet
        qreal ftValue = targetValue * M2FT;

        if (ftValue < 1) {
            targetValue *= M2IN;
            targetUnit = Inch;
            break;
        }

        if (ftValue >= 3 && ftValue < 5280.0) {
            targetValue *= M2YD;
            targetUnit = Yard;
            break;
        }

        if (ftValue >= 5280.0) {
            targetValue *= METER2KM * KM2MI;
            targetUnit = Mile;
            break;
        }

        targetValue *= M2FT;
        targetUnit = Foot;
        break;
    }

    case NauticalSystem: {
        targetValue *= METER2KM * KM2NM;
        targetUnit = NauticalMile;
        break;
    }

    default:
        qWarning() << "Unknown measurement system!";
    }
}

QString MarbleLocale::unitAbbreviation(MeasureUnit unit)
{
    switch (unit) {
    case Meter:
        return QObject::tr("m", "means meter");
    case Milimeter:
        return QObject::tr("mm", "means milimeters");
    case Kilometer:
        return QObject::tr("km", "means kilometers");
    case Centimeter:
        return QObject::tr("cm", "means centimeters");
    case Foot:
        return QObject::tr("ft", "means feet");
    case Inch:
        return QObject::tr("in", "means inches");
    case Yard:
        return QObject::tr("yd", "means yards");
    case Mile:
        return QObject::tr("mi", "means miles");
    case NauticalMile:
        return QObject::tr("nm", "means nautical miles");
    default:
        return {};
    }
}

QString MarbleLocale::languageCode()
{
    const QString lang = QLocale::system().name();
    QString code;

    int index = lang.indexOf(QLatin1Char('_'));
    if (lang == QLatin1StringView("C")) {
        code = QStringLiteral("en");
    } else if (index != -1) {
        code = lang.left(index);
    } else {
        index = lang.indexOf(QLatin1Char('@'));
        if (index != -1)
            code = lang.left(index);
        else
            code = lang;
    }

    return code;
}

}
