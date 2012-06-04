//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "AbstractWeatherService.h"

namespace Marble
{

AbstractWeatherService::AbstractWeatherService( QObject *parent )
    : QObject( parent )
{
}

AbstractWeatherService::~AbstractWeatherService()
{
}

void AbstractWeatherService::setFavoriteItems( const QStringList& favorite )
{
    if ( m_favoriteItems != favorite) {
        m_favoriteItems = favorite;
    }
}

QStringList AbstractWeatherService::favoriteItems() const
{
    return m_favoriteItems;
}

void AbstractWeatherService::parseFile( const QByteArray& file )
{
    Q_UNUSED( file );
}

} // namespace Marble

#include "AbstractWeatherService.moc"
