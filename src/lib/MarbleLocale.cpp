//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleLocale.h"
#include "MarbleLocale_p.h"

// Qt
#include <QtCore/QLocale>


namespace Marble {

MarbleLocalePrivate::MarbleLocalePrivate()
    : m_distanceUnit( Marble::NoDistanceUnit ),
      m_measureSystem( Marble::Metric )
{
}

MarbleLocalePrivate::~MarbleLocalePrivate()
{
}

MarbleLocale::MarbleLocale()
    : d ( new MarbleLocalePrivate )
{
}

MarbleLocale::~MarbleLocale()
{
    delete d;
}

Marble::DistanceUnit MarbleLocale::distanceUnit() const
{
    if( d->m_distanceUnit == Marble::NoDistanceUnit ) {
        if ( d->m_measureSystem == Marble::Metric ) {
            return Marble::Meter;
        }
        else {
            return Marble::MilesFeet;
        }
    }
    else {
        return d->m_distanceUnit;
    }
}

void MarbleLocale::setDistanceUnit( Marble::DistanceUnit distanceUnit )
{
    d->m_distanceUnit = distanceUnit;
}

void MarbleLocale::setMeasureSystem( Marble::MeasureSystem measureSystem ) {
    d->m_measureSystem = measureSystem;
}

Marble::MeasureSystem MarbleLocale::measureSystem() const {
    return d->m_measureSystem;
}

QString MarbleLocale::languageCode() {
    const QString lang = QLocale::system().name();
    QString code;

    int index = lang.indexOf ( '_' );
    if ( index != -1 ) {
        code = lang.left ( index );
    }
    else {
        index = lang.indexOf ( '@' );
        if ( index != -1 )
            code = lang.left ( index );
        else
            code = lang;
    }

    return code;
}

}
