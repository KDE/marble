#ifndef GEODATALOOKAT_P_H
#define GEODATALOOKAT_P_H

// Marble
#include "GeoDataCoordinates.h"

// Qt
#include <QtCore/QAtomicInt>

namespace Marble
{

class GeoDataLookAtPrivate
{
 public :
    GeoDataLookAtPrivate()
        : m_coordinates(),
          m_range( 0.0 ),
          ref( 1 )
    {
    }

    GeoDataCoordinates m_coordinates;
    qreal m_range;
    
    QAtomicInt ref;
};

} // namespace Marble

#endif
