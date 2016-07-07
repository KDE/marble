//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn    <tackat@kde.org>
// Copyright 2007      Inge Wallin     <ingwa@kde.org>
// Copyright 2015      Ilya Kowalewski <illya.kovalevskyy@gmail.com>
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
