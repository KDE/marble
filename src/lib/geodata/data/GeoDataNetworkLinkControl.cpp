//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataNetworkLinkControl.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataNetworkLinkControlPrivate
{
public:
    GeoDataNetworkLinkControlPrivate();

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
    GeoDataAbstractView m_abstractView;
};

GeoDataNetworkLinkControlPrivate::GeoDataNetworkLinkControlPrivate() :
    m_minRefreshPeriod( 0.0 ),
    m_maxSessionLength( 0.0 ),
    m_cookie( "" ),
    m_message( "" ),
    m_linkName( "" ),
    m_linkDescription( "" ),
    m_linkSnippet( "" ),
    m_maxLines( 2 ),
    m_expires(),
    m_update(),
    m_abstractView()
{
}

GeoDataNetworkLinkControl::GeoDataNetworkLinkControl() :
    d( new GeoDataNetworkLinkControlPrivate )
{
}

GeoDataNetworkLinkControl::GeoDataNetworkLinkControl( const Marble::GeoDataNetworkLinkControl &other ) :
    GeoDataContainer(), d( new GeoDataNetworkLinkControlPrivate( *other.d ) )
{
}

GeoDataNetworkLinkControl &GeoDataNetworkLinkControl::operator=( const GeoDataNetworkLinkControl &other )
{
    GeoDataContainer::operator =( other );
    *d = *other.d;
    return *this;
}

GeoDataNetworkLinkControl::~GeoDataNetworkLinkControl()
{
    delete d;
}

const char *GeoDataNetworkLinkControl::nodeType() const
{
    return GeoDataTypes::GeoDataNetworkLinkControlType;
}

qreal GeoDataNetworkLinkControl::minRefreshPeriod() const
{
    return d->m_minRefreshPeriod;
}

void GeoDataNetworkLinkControl::setMinRefreshPeriod( const qreal &minRefreshPeriod )
{
    d->m_minRefreshPeriod = minRefreshPeriod;
}

qreal GeoDataNetworkLinkControl::maxSessionLength() const
{
    return d->m_maxSessionLength;
}

void GeoDataNetworkLinkControl::setMaxSessionLength( const qreal &maxSessionLength )
{
    d->m_maxSessionLength = maxSessionLength;
}

QString GeoDataNetworkLinkControl::cookie() const
{
    return d->m_cookie;
}

void GeoDataNetworkLinkControl::setCookie( const QString &cookie )
{
    d->m_cookie = cookie;
}

QString GeoDataNetworkLinkControl::message() const
{
    return d->m_message;
}

void GeoDataNetworkLinkControl::setMessage( const QString &message )
{
    d->m_message = message;
}

QString GeoDataNetworkLinkControl::linkName() const
{
    return d->m_linkName;
}

void GeoDataNetworkLinkControl::setLinkName( const QString &linkName )
{
    d->m_linkName = linkName;
}

QString GeoDataNetworkLinkControl::linkDescription() const
{
    return d->m_linkDescription;
}

void GeoDataNetworkLinkControl::setLinkDescription( const QString &linkDescription )
{
    d->m_linkDescription = linkDescription;
}

QString GeoDataNetworkLinkControl::linkSnippet() const
{
    return d->m_linkSnippet;
}

void GeoDataNetworkLinkControl::setLinkSnippet( const QString &linkSnippet )
{
    d->m_linkSnippet = linkSnippet;
}

int GeoDataNetworkLinkControl::maxLines() const
{
    return d->m_maxLines;
}

void GeoDataNetworkLinkControl::setMaxLines( const int &maxLines )
{
    d->m_maxLines = maxLines;
}

QDateTime GeoDataNetworkLinkControl::expires() const
{
    return d->m_expires;
}

void GeoDataNetworkLinkControl::setExpires( const QDateTime &expires )
{
    d->m_expires = expires;
}

GeoDataUpdate &GeoDataNetworkLinkControl::update()
{
    return d->m_update;
}

const GeoDataUpdate& GeoDataNetworkLinkControl::update() const
{
    return d->m_update;
}

void GeoDataNetworkLinkControl::setUpdate( const GeoDataUpdate &update )
{
    d->m_update = update;
}

GeoDataAbstractView GeoDataNetworkLinkControl::abstractView() const
{
    return d->m_abstractView;
}

void GeoDataNetworkLinkControl::setAbstractView( const GeoDataAbstractView &abstractView )
{
    d->m_abstractView = abstractView;
}

}
