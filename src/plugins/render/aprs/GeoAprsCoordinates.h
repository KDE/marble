#ifndef GEOAPRSCOORDINATES_H
#define GEOAPRSCOORDINATES_H

#include <QtCore/QTime>
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

        GeoAprsCoordinates( qreal lon, qreal lat, qreal alt = 0,
                            GeoDataCoordinates::Unit unit =
                            GeoDataCoordinates::Radian,
                            int where = FromNowhere );
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
