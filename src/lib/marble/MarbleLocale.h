// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2015 Ilya Kowalewski <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLE_MARBLELOCALE_H
#define MARBLE_MARBLELOCALE_H

#include "marble_export.h"

#include <QtGlobal>

namespace Marble
{

class MarbleLocalePrivate;

/**
 * @short A class that contains all localization stuff for Marble.
 *
 * The class stores properties like the measurement system.
 */

class MARBLE_EXPORT MarbleLocale
{
 public:
    MarbleLocale();
    ~MarbleLocale();

    enum MeasurementSystem {
        MetricSystem = 0,
        ImperialSystem,
        NauticalSystem
    };

    enum MeasureUnit {
        Meter = 0,
        Kilometer,
        Milimeter,
        Centimeter,

        Foot,
        Inch,
        Yard,
        Mile,

        NauticalMile
    };

    void setMeasurementSystem( MarbleLocale::MeasurementSystem measurementSystem );
    MarbleLocale::MeasurementSystem measurementSystem() const;

    static QString languageCode();

    /**
     * @brief meterToCustomUnit Converts @p meters to @p targetValue of @p targetUnit in @p targetSystem
     */
    static void meterToTargetUnit(qreal meters, MeasurementSystem targetSystem,
                                  qreal &targetValue, MeasureUnit &targetUnit);

    /**
     * @brief unitAbbreviation Represents a short string form of @p unit, e.g. "km" for Kilometer
     */
    static QString unitAbbreviation(MeasureUnit unit);

 private:
    Q_DISABLE_COPY( MarbleLocale )
    MarbleLocalePrivate  * const d;
};

}

#endif
