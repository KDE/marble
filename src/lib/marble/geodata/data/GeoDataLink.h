// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>

#ifndef MARBLE_GEODATALINK_H
#define MARBLE_GEODATALINK_H

#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataLinkPrivate;

/**
 */
class GEODATA_EXPORT GeoDataLink : public GeoDataObject
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

    GeoDataLink(const GeoDataLink &other);

    GeoDataLink &operator=(const GeoDataLink &other);
    bool operator==(const GeoDataLink &other) const;
    bool operator!=(const GeoDataLink &other) const;

    ~GeoDataLink() override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    QString href() const;

    void setHref(const QString &href);

    RefreshMode refreshMode() const;

    void setRefreshMode(RefreshMode refreshMode);

    qreal refreshInterval() const;

    void setRefreshInterval(qreal refreshInterval);

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
    GeoDataLinkPrivate *const d;
};

}

#endif
