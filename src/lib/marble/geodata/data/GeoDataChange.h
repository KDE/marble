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

#include <QString>
#include <QDateTime>

#include "GeoDataContainer.h"
#include "marble_export.h"

namespace Marble
{

class GeoDataChangePrivate;

class MARBLE_EXPORT GeoDataChange : public GeoDataContainer
{
public:
    GeoDataChange();

    GeoDataChange( const GeoDataChange &other );

    GeoDataChange& operator=( const GeoDataChange &other );

    bool operator==( const GeoDataChange &other ) const;
    bool operator!=( const GeoDataChange &other ) const;

    ~GeoDataChange();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

private:
    GeoDataChangePrivate* const d;
};

}

#endif
