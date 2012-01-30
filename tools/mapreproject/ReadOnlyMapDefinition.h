#ifndef READONLYMAPDEFINITION_H
#define READONLYMAPDEFINITION_H

#include "mapreproject.h"

#include <QtCore/QDebug>
#include <QtCore/QString>

class InterpolationMethod;
class ReadOnlyMapImage;

class ReadOnlyMapDefinition
{
    friend QDebug operator<<( QDebug dbg, ReadOnlyMapDefinition const & r);

public:
    ReadOnlyMapDefinition();

    ReadOnlyMapImage * createReadOnlyMap() const;

    void setBaseDirectory( QString const & baseDirectory );
    void setCacheSizeBytes( int const cacheSizeBytes );
    void setInterpolationMethod( EInterpolationMethod const interpolationMethod );
    void setFileName( QString const & fileName );
    void setMapType( MapSourceType const mapType );
    void setTileLevel( int const tileLevel );

private:
    InterpolationMethod * createInterpolationMethod() const;

    MapSourceType m_mapType;
    EInterpolationMethod m_interpolationMethod;

    // relevant for tiled maps
    QString m_baseDirectory;
    int m_tileLevel;
    int m_cacheSizeBytes;

    // relevant for non-tiled maps (only one image)
    QString m_filename;
};

QDebug operator<<( QDebug dbg, ReadOnlyMapDefinition const & r);


// inline definitions

inline void ReadOnlyMapDefinition::setBaseDirectory( QString const & baseDirectory )
{
    m_baseDirectory = baseDirectory;
}

inline void ReadOnlyMapDefinition::setCacheSizeBytes( int const cacheSizeBytes )
{
    m_cacheSizeBytes = cacheSizeBytes;
}

inline void ReadOnlyMapDefinition::setInterpolationMethod( EInterpolationMethod const interpolationMethod )
{
    m_interpolationMethod = interpolationMethod;
}

inline void ReadOnlyMapDefinition::setFileName( QString const & fileName )
{
    m_filename = fileName;
}

inline void ReadOnlyMapDefinition::setMapType( MapSourceType const mapType )
{
    m_mapType = mapType;
}

inline void ReadOnlyMapDefinition::setTileLevel( int const tileLevel )
{
    m_tileLevel = tileLevel;
}


inline QDebug operator<<( QDebug dbg, ReadOnlyMapDefinition const & r)
{
    dbg.nospace() << "("
                  << r.m_mapType << ", "
                  << r.m_interpolationMethod << ", "
                  << r.m_baseDirectory << ", "
                  << r.m_tileLevel << ", "
                  << r.m_cacheSizeBytes << ", "
                  << r.m_filename << ")";
    return dbg.space();
}

#endif
