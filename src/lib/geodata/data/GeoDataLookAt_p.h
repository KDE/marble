#ifndef GEODATALOOKAT_P_H
#define GEODATALOOKAT_P_H

#include "GeoDataCoordinates.h"

namespace Marble
{

    class GeoDataLookAtPrivate
    {
    public :

    GeoDataLookAtPrivate() : m_range(0.0)
    {
        // nothing to do
    }

    GeoDataCoordinates m_coord;

    qreal m_range;
};

}

#endif
