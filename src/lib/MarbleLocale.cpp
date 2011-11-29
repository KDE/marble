//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>
//

#include "MarbleLocale.h"
#include "MarbleLocale_p.h"

// Qt
#include <QtCore/QLocale>


namespace Marble
{

MarbleLocalePrivate::MarbleLocalePrivate()
    : m_distanceUnit( Meter ),
      m_measureSystem( Metric )
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

DistanceUnit MarbleLocale::distanceUnit() const
{
    return d->m_distanceUnit;
}

void MarbleLocale::setDistanceUnit( DistanceUnit distanceUnit )
{
    d->m_distanceUnit = distanceUnit;
}

void MarbleLocale::setMeasureSystem( MeasureSystem measureSystem )
{
    d->m_measureSystem = measureSystem;
}

MeasureSystem MarbleLocale::measureSystem() const
{
    return d->m_measureSystem;
}

QString MarbleLocale::languageCode()
{
    const QString lang = QLocale::system().name();
    QString code;

    int index = lang.indexOf ( '_' );
    if ( lang == "C" ) {
        code = "en";
    }
    else if ( index != -1 ) {
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
