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
#include "GeoDataCamera.h"
#include "GeoDataLookAt.h"

namespace Marble
{

class GeoDataNetworkLinkControlPrivate
{
public:
    GeoDataNetworkLinkControlPrivate();

    GeoDataNetworkLinkControlPrivate( const GeoDataNetworkLinkControlPrivate &other );

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
    m_abstractView( 0 )
{
}

GeoDataNetworkLinkControlPrivate::GeoDataNetworkLinkControlPrivate( const GeoDataNetworkLinkControlPrivate &other ) :
    m_minRefreshPeriod( other.m_minRefreshPeriod ),
    m_maxSessionLength( other.m_maxSessionLength ),
    m_cookie( other.m_cookie ),
    m_message( other.m_message ),
    m_linkName( other.m_linkName ),
    m_linkDescription( other.m_linkDescription ),
    m_linkSnippet( other.m_linkSnippet ),
    m_maxLines( other.m_maxLines ),
    m_expires( other.m_expires ),
    m_update( other.m_update ),
    m_abstractView( other.m_abstractView ? other.m_abstractView->copy() : 0 )
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
    d->m_abstractView = other.d->m_abstractView ? other.d->m_abstractView->copy() : 0;
    return *this;
}


bool GeoDataNetworkLinkControl::operator==( const GeoDataNetworkLinkControl &other ) const
{
    if ( !GeoDataContainer::equals(other) ||
         d->m_minRefreshPeriod != other.d->m_minRefreshPeriod ||
         d->m_maxSessionLength != other.d->m_maxSessionLength ||
         d->m_cookie != other.d->m_cookie ||
         d->m_message != other.d->m_message ||
         d->m_linkName != other.d->m_linkName ||
         d->m_linkDescription != other.d->m_linkDescription ||
         d->m_linkSnippet != other.d->m_linkSnippet ||
         d->m_maxLines != other.d->m_maxLines ||
         d->m_expires != other.d->m_expires ||
         d->m_update != other.d->m_update ) {
        return false;
    }

    if ( !d->m_abstractView && !other.d->m_abstractView ) {
        return true;
    } else if ( (!d->m_abstractView && other.d->m_abstractView) ||
                (d->m_abstractView && !other.d->m_abstractView) ) {
        return false;
    }

    if ( d->m_abstractView->nodeType() != other.d->m_abstractView->nodeType() ) {
        return false;
    }

    if ( d->m_abstractView->nodeType() == GeoDataTypes::GeoDataCameraType ) {
        GeoDataCamera *thisCam = dynamic_cast<GeoDataCamera*>( d->m_abstractView );
        GeoDataCamera *otherCam = dynamic_cast<GeoDataCamera*>( other.d->m_abstractView );
        Q_ASSERT(thisCam && otherCam);

        if ( *thisCam != *otherCam ) {
            return false;
        }
    } else if ( d->m_abstractView->nodeType() == GeoDataTypes::GeoDataLookAtType ) {
        GeoDataLookAt *thisLookAt = dynamic_cast<GeoDataLookAt*>( d->m_abstractView );
        GeoDataLookAt *otherLookAt = dynamic_cast<GeoDataLookAt*>( other.d->m_abstractView );
        Q_ASSERT(thisLookAt && otherLookAt);

        if ( *thisLookAt != *otherLookAt ) {
            return false;
        }
    }

    return true;
}

bool GeoDataNetworkLinkControl::operator!=( const GeoDataNetworkLinkControl &other ) const
{
    return !this->operator==( other );
}

GeoDataNetworkLinkControl::~GeoDataNetworkLinkControl()
{
    delete d->m_abstractView;
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

GeoDataAbstractView *GeoDataNetworkLinkControl::abstractView() const
{
    return d->m_abstractView;
}

void GeoDataNetworkLinkControl::setAbstractView( GeoDataAbstractView *abstractView )
{
    d->m_abstractView = abstractView;
    d->m_abstractView->setParent( this );
}

}
