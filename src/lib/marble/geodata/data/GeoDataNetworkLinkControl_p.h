// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATANETWORKLINKCONTROL_P_H
#define GEODATANETWORKLINKCONTROL_P_H

#include "GeoDataContainer_p.h"

#include "GeoDataTypes.h"
#include "GeoDataUpdate.h"
#include <QDateTime>

namespace Marble
{

class GeoDataNetworkLinkControlPrivate : public GeoDataContainerPrivate
{
public:
    GeoDataNetworkLinkControlPrivate();
    GeoDataNetworkLinkControlPrivate(const GeoDataNetworkLinkControlPrivate &other);
    ~GeoDataNetworkLinkControlPrivate() override;
    GeoDataNetworkLinkControlPrivate &operator=(const GeoDataNetworkLinkControlPrivate &other);

    qreal m_minRefreshPeriod;
    qreal m_maxSessionLength;
    QString m_cookie;
    QString m_message;
    QString m_linkName;
    QString m_linkDescription;
    QString m_linkSnippet;
    int m_maxLines;
    QDateTime m_expires;
    GeoDataUpdate m_update;
    GeoDataAbstractView *m_abstractView;
};

GeoDataNetworkLinkControlPrivate::GeoDataNetworkLinkControlPrivate()
    : m_minRefreshPeriod(0.0)
    , m_maxSessionLength(0.0)
    , m_maxLines(2)
    , m_expires()
    , m_update()
    , m_abstractView(nullptr)
{
}

GeoDataNetworkLinkControlPrivate::GeoDataNetworkLinkControlPrivate(const GeoDataNetworkLinkControlPrivate &other)
    : GeoDataContainerPrivate(other)
    , m_minRefreshPeriod(other.m_minRefreshPeriod)
    , m_maxSessionLength(other.m_maxSessionLength)
    , m_cookie(other.m_cookie)
    , m_message(other.m_message)
    , m_linkName(other.m_linkName)
    , m_linkDescription(other.m_linkDescription)
    , m_linkSnippet(other.m_linkSnippet)
    , m_maxLines(other.m_maxLines)
    , m_expires(other.m_expires)
    , m_update(other.m_update)
    , m_abstractView(other.m_abstractView ? other.m_abstractView->copy() : nullptr)
{
}

GeoDataNetworkLinkControlPrivate::~GeoDataNetworkLinkControlPrivate()
{
    delete m_abstractView;
}

GeoDataNetworkLinkControlPrivate &GeoDataNetworkLinkControlPrivate::operator=(const GeoDataNetworkLinkControlPrivate &other)
{
    GeoDataContainerPrivate::operator=(other);
    m_minRefreshPeriod = other.m_minRefreshPeriod;
    m_maxSessionLength = other.m_maxSessionLength;
    m_cookie = other.m_cookie;
    m_message = other.m_message;
    m_linkName = other.m_linkName;
    m_linkDescription = other.m_linkDescription;
    m_linkSnippet = other.m_linkSnippet;
    m_maxLines = other.m_maxLines;
    m_expires = other.m_expires;
    m_update = other.m_update;

    delete m_abstractView;
    m_abstractView = other.m_abstractView ? other.m_abstractView->copy() : nullptr;

    return *this;
}

}

#endif
