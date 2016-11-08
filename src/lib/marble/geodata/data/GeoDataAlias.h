//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//


#ifndef MARBLE_GEODATAALIAS_H
#define MARBLE_GEODATAALIAS_H

#include "GeoDocument.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataAliasPrivate;

/**
 */
class GEODATA_EXPORT GeoDataAlias: public GeoNode
{

public:

    GeoDataAlias();

    GeoDataAlias( const GeoDataAlias &other );

    GeoDataAlias& operator=( const GeoDataAlias &other );

    bool operator==( const GeoDataAlias &other ) const;

    bool operator!=( const GeoDataAlias &other ) const;

    ~GeoDataAlias();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    QString sourceHref() const;

    void setSourceHref( const QString& sourceHref );

    QString targetHref() const;

    void setTargetHref( const QString& targetHref );

private:
    GeoDataAliasPrivate* const d;
};

}

#endif
