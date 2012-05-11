#include "ReadOnlyMapDefinition.h"

#include "BilinearInterpolation.h"
#include "IntegerInterpolation.h"
#include "NearestNeighborInterpolation.h"
#include "NwwMapImage.h"
#include "SimpleMapImage.h"

ReadOnlyMapDefinition::ReadOnlyMapDefinition()
    : m_mapType( UnknownMapSource ),
      m_interpolationMethod( UnknownInterpolationMethod ),
      m_baseDirectory(),
      m_tileLevel( -1 ),
      m_cacheSizeBytes(),
      m_filename()
{
}

InterpolationMethod * ReadOnlyMapDefinition::createInterpolationMethod() const
{
    switch ( m_interpolationMethod ) {
    case IntegerInterpolationMethod:
        return new IntegerInterpolation;
    case NearestNeighborInterpolationMethod:
        return new NearestNeighborInterpolation;
    case AverageInterpolationMethod:
        return NULL;
    case BilinearInterpolationMethod:
        return new BilinearInterpolation;
    default:
        return NULL;
    }
}

ReadOnlyMapImage * ReadOnlyMapDefinition::createReadOnlyMap() const
{
    InterpolationMethod * const interpolationMethod = createInterpolationMethod();
    if ( !interpolationMethod )
        qFatal( "Unsupported interpolation method: '%i'", m_interpolationMethod );

    if ( m_mapType == NasaWorldWindMap ) {
        NwwMapImage * const mapImage = new NwwMapImage( m_baseDirectory, m_tileLevel );
        interpolationMethod->setMapImage( mapImage );
        mapImage->setInterpolationMethod( interpolationMethod );
        mapImage->setCacheSizeBytes( m_cacheSizeBytes );
        return mapImage;
    }
    else if ( m_mapType == BathymetryMap ) {
        SimpleMapImage * const mapImage = new SimpleMapImage( m_filename );
        interpolationMethod->setMapImage( mapImage );
        mapImage->setInterpolationMethod( interpolationMethod );
        return mapImage;
    }
    else {
        delete interpolationMethod;
        return NULL;
    }
}
