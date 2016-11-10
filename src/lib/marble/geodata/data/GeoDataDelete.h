//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATADELETE_H
#define GEODATADELETE_H

#include "GeoDataContainer.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataDeletePrivate;

/**
 */
class GEODATA_EXPORT GeoDataDelete : public GeoDataContainer
{
public:
    GeoDataDelete();

    GeoDataDelete( const GeoDataDelete &other );

    ~GeoDataDelete();

    GeoDataDelete& operator=( const GeoDataDelete &other );

    bool operator==( const GeoDataDelete &other ) const;
    bool operator!=( const GeoDataDelete &other ) const;

    GeoDataFeature * clone() const override;

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

private:
    Q_DECLARE_PRIVATE(GeoDataDelete)
};

}

#endif
