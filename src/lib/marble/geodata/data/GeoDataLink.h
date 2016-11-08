//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>


#ifndef MARBLE_GEODATALINK_H
#define MARBLE_GEODATALINK_H

#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataLinkPrivate;

/**
 */
class GEODATA_EXPORT GeoDataLink: public GeoDataObject
{
public:
    enum RefreshMode {
        OnChange,
        OnInterval,
        OnExpire
    };

    enum ViewRefreshMode {
        Never,
        OnStop,
        OnRequest,
        OnRegion
    };

    GeoDataLink();

    GeoDataLink( const GeoDataLink &other );

    GeoDataLink& operator=( const GeoDataLink &other );
    bool operator==( const GeoDataLink &other ) const;
    bool operator!=( const GeoDataLink &other ) const;

    ~GeoDataLink();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    QString href() const;

    void setHref( const QString& href );

    RefreshMode refreshMode() const;

    void setRefreshMode( RefreshMode refreshMode );

    qreal refreshInterval() const;

    void setRefreshInterval( qreal refreshInterval );

    ViewRefreshMode viewRefreshMode() const;

    void setViewRefreshMode(ViewRefreshMode refreshMode);

    qreal viewRefreshTime() const;

    void setViewRefreshTime(qreal viewRefreshTime);

    qreal viewBoundScale() const;

    void setViewBoundScale(qreal viewBoundScale);

    QString viewFormat() const;

    void setViewFormat(const QString &viewFormat);

    QString httpQuery() const;

    void setHttpQuery(const QString &httpQuery);

private:
    GeoDataLinkPrivate* const d;
};

}

#endif
