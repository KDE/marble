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

AbstractWeatherService::AbstractWeatherService( const MarbleModel *model, QObject *parent )
    : QObject( parent ),
      m_marbleModel( model ),
      m_marbleWidget( 0 )
{
    Q_ASSERT( m_marbleModel != 0 );
}

AbstractWeatherService::~AbstractWeatherService()
{
}

void AbstractWeatherService::setMarbleWidget( MarbleWidget *widget )
{
    m_marbleWidget = widget;
}

const MarbleModel* AbstractWeatherService::marbleModel() const
{
    return m_marbleModel;
}

MarbleWidget* AbstractWeatherService::marbleWidget()
{
    return m_marbleWidget;
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

#include "moc_AbstractWeatherService.cpp"
