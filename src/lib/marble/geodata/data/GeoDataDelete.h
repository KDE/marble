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

#include <QString>
#include <QDateTime>

#include "GeoDataContainer.h"
#include "marble_export.h"

namespace Marble
{

class GeoDataDeletePrivate;

class MARBLE_EXPORT GeoDataDelete : public GeoDataContainer
{
public:
    GeoDataDelete();

    GeoDataDelete( const GeoDataDelete &other );

    GeoDataDelete& operator=( const GeoDataDelete &other );

    bool operator==( const GeoDataDelete &other ) const;
    bool operator!=( const GeoDataDelete &other ) const;

    ~GeoDataDelete();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

private:
    GeoDataDeletePrivate* const d;
};

}

#endif
