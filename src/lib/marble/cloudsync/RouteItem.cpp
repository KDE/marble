//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "RouteItem.h"

#include <QUrl>
#include <QIcon>

namespace Marble {

class Q_DECL_HIDDEN RouteItem::Private {

public:
    QString m_identifier;
    QString m_name;
    QIcon m_preview;
    QUrl m_previewUrl;
    QString m_distance;
    QString m_duration;
    bool m_onCloud;
};

RouteItem::RouteItem() : d( new Private() )
{
}

RouteItem::RouteItem( const RouteItem &other  ) : d( new Private( *other.d ) )
{
}

RouteItem::~RouteItem()
{
    delete d;
}

RouteItem &RouteItem::operator=( const RouteItem &other )
{
    *d = *other.d;
    return *this;
}

bool RouteItem::operator==( const RouteItem& other ) const
{
    return identifier() == other.identifier();
}

QString RouteItem::identifier() const
{
    return d->m_identifier;
}

void RouteItem::setIdentifier( const QString &timestamp )
{
    d->m_identifier = timestamp;
}

QString RouteItem::name() const
{
    return d->m_name;
}

void RouteItem::setName( const QString &name )
{
    d->m_name = name;
}

QIcon RouteItem::preview() const
{
    return d->m_preview;
}

void RouteItem::setPreview( const QIcon &preview )
{
    d->m_preview = preview;
}

QUrl RouteItem::previewUrl() const
{
    return d->m_previewUrl;
}

void RouteItem::setPreviewUrl( const QUrl &previewUrl )
{
    d->m_previewUrl = previewUrl;
}

QString RouteItem::distance() const
{
    return d->m_distance;
}

void RouteItem::setDistance( const QString &distance )
{
    d->m_distance = distance;
}

QString RouteItem::duration() const
{
    return d->m_duration;
}

void RouteItem::setDuration( const QString &duration )
{
    d->m_duration = duration;
}

bool RouteItem::onCloud() const
{
    return d->m_onCloud;
}

void RouteItem::setOnCloud( const bool onCloud )
{
    d->m_onCloud = onCloud;
}

}
