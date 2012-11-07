//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// self
#include "AbstractDataPluginItem.h"

// Marble
#include "GeoDataCoordinates.h"
#include "ViewportParams.h"
#include "MarbleDebug.h"

// Qt
#include <QtGui/QAction>
#include <QtCore/QRect>
#include <QtCore/QSize>

namespace Marble
{

class AbstractDataPluginItemPrivate
{
 public:
    AbstractDataPluginItemPrivate()
        : m_favorite( false ),
          m_addedAngularResolution( 0 )
    {
    }
    
    QString m_id;
    QString m_target;
    QString m_toolTip;
    bool m_favorite;
    qreal m_addedAngularResolution;
};

AbstractDataPluginItem::AbstractDataPluginItem( QObject *parent )
    : QObject( parent ),
      BillboardGraphicsItem(),
      d( new AbstractDataPluginItemPrivate )
{
}

AbstractDataPluginItem::~AbstractDataPluginItem()
{
    delete d;
}

QString AbstractDataPluginItem::target()
{
    return d->m_target;
}

void AbstractDataPluginItem::setTarget( const QString& target )
{
    d->m_target = target;
}

QString AbstractDataPluginItem::toolTip() const
{
    return d->m_toolTip;
}

void AbstractDataPluginItem::setToolTip( const QString& toolTip )
{
    d->m_toolTip = toolTip;
}

QString AbstractDataPluginItem::id() const
{
    return d->m_id;
}

void AbstractDataPluginItem::setId( const QString& id )
{
    d->m_id = id;
}

bool AbstractDataPluginItem::isFavorite() const
{
    return d->m_favorite;
}

void AbstractDataPluginItem::setFavorite( bool favorite )
{
    if ( isFavorite() != favorite ) {
        d->m_favorite = favorite;
        emit favoriteChanged( id(), favorite );
    }
}

void AbstractDataPluginItem::toggleFavorite()
{
    setFavorite( !isFavorite() );
}

qreal AbstractDataPluginItem::addedAngularResolution() const
{
    return d->m_addedAngularResolution;
}

void AbstractDataPluginItem::setAddedAngularResolution( qreal resolution )
{
    d->m_addedAngularResolution = resolution;
}

void AbstractDataPluginItem::setSettings( const QHash<QString, QVariant>& settings )
{
    Q_UNUSED( settings )
}

QAction *AbstractDataPluginItem::action()
{
    return 0;
}

void AbstractDataPluginItem::addDownloadedFile( const QString& url, const QString& type )
{
    Q_UNUSED( url )
    Q_UNUSED( type )
}

QList<QAction*> AbstractDataPluginItem::actions()
{
    QList<QAction*> result;
    QAction* pluginAction = action();

    if ( pluginAction ) {
        result << pluginAction;
    }

    return result;
}

} // Marble namespace

#include "AbstractDataPluginItem.moc"
