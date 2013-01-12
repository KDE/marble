//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATAUPDATE_H
#define GEODATAUPDATE_H

#include "GeoDataObject.h"
#include "marble_export.h"

namespace Marble
{

class GeoDataUpdatePrivate;

class MARBLE_EXPORT GeoDataUpdate : public GeoDataObject
{
public:
    GeoDataUpdate();

    GeoDataUpdate( const GeoDataUpdate &other );

    GeoDataUpdate& operator=( const GeoDataUpdate &other );

    ~GeoDataUpdate();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    QString targetHref() const;
    void setTargetHref( const QString &targetHref );

private:
    GeoDataUpdatePrivate* const d;
};

}

#endif
