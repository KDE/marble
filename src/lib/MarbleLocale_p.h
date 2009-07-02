//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009    Torsten Rahn <tackat@kde.org>"
//

//
// Description: AbstractTile contains the base class for a single quadtile 
//


#ifndef MARBLE_LOCALE_P_H
#define MARBLE_LOCALE_P_H


namespace Marble
{


class MarbleLocalePrivate {
 public:
    MarbleLocalePrivate();
    virtual ~MarbleLocalePrivate();

    Marble::DistanceUnit m_distanceUnit;
    Marble::MeasureSystem m_measureSystem;
};

}

#endif // MARBLE_LOCALE_P_H
