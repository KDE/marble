//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATACHANGE_H
#define GEODATACHANGE_H

#include "GeoDataContainer.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataChangePrivate;

/**
 */
class GEODATA_EXPORT GeoDataChange : public GeoDataContainer
{
public:
    GeoDataChange();

    GeoDataChange( const GeoDataChange &other );

    ~GeoDataChange();

    GeoDataChange& operator=( const GeoDataChange &other );

    bool operator==( const GeoDataChange &other ) const;
    bool operator!=( const GeoDataChange &other ) const;

    GeoDataFeature * clone() const override;

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

private:
    Q_DECLARE_PRIVATE(GeoDataChange)
};

}

#endif
