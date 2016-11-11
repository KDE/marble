//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATARESOURCEMAP_H
#define GEODATARESOURCEMAP_H

#include "GeoDocument.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataAlias;
class GeoDataResourceMapPrivate;

/**
 */
class GEODATA_EXPORT GeoDataResourceMap: public GeoNode
{
public:
    GeoDataResourceMap();

    GeoDataResourceMap( const GeoDataResourceMap &other );

    GeoDataResourceMap& operator=( const GeoDataResourceMap &other );

    bool operator==( const GeoDataResourceMap &other ) const;
    bool operator!=( const GeoDataResourceMap &other ) const;

    ~GeoDataResourceMap();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    const GeoDataAlias& alias() const;

    GeoDataAlias& alias();

    void setAlias( const GeoDataAlias &alias);

    QString sourceHref() const;

    void setSourceHref( const QString& sourceHref );

    QString targetHref() const;

    void setTargetHref( const QString& targetHref );

private:
    GeoDataResourceMapPrivate* const d;
};

}

#endif // GEODATARESOURCEMAP_H
