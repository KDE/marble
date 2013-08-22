//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef GEOAPRSCOORDINATES_H
#define GEOAPRSCOORDINATES_H

#include <QTime>
#include "GeoDataCoordinates.h"

namespace Marble
{
    class GeoAprsCoordinates : public GeoDataCoordinates
    {
      public:
        // must match AprsObject's SeenFrom
        enum SeenFrom {
            FromNowhere = 0x00,
            FromTTY = 0x01,
            FromTCPIP = 0x02,
            FromFile = 0x04,
            Directly = 0x08
        };

        GeoAprsCoordinates( qreal lon, qreal lat, int where );
        ~GeoAprsCoordinates();

        void  addSeenFrom( int where );
        int   seenFrom() const;
        void  setTimestamp( const QTime &t );        
        const QTime &timestamp() const;

      private:
        int   m_seenFrom;
        QTime m_timestamp;
    };
}

#endif /* GEOAPRSCOORDINATES_H */
