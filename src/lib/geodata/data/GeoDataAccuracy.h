//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef MARBLE_GEODATAACCURACY_H
#define MARBLE_GEODATAACCURACY_H

#include <QtCore/QObject>



namespace Marble
{

class GeoDataAccuracy
{
 public:
    enum Level {
        none = 0,
        Country,
        Region,
        Locality,
        PostalCode,
        Street,
        Detailed
    } level;
    qreal horizontal;
    qreal vertical;
};

}



#endif
