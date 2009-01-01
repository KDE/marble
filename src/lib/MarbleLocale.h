//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef __MARBLE__LOCALE_H
#define __MARBLE__LOCALE_H

#include "marble_export.h"
#include "global.h"


namespace Marble
{

class MarbleLocalePrivate;

/**
 * @short A class that contains all localization stuff for Marble.
 *
 * The class stores properties like the Distance Unit.
 */

class MARBLE_EXPORT MarbleLocale
{
 public:
    MarbleLocale();
    ~MarbleLocale();

    void setDistanceUnit( Marble::DistanceUnit distanceUnit );
    Marble::DistanceUnit distanceUnit() const;

 private:
    Q_DISABLE_COPY( MarbleLocale )
    MarbleLocalePrivate  * const d;
};

}

#endif // __MARBLE__LOCALE_H
