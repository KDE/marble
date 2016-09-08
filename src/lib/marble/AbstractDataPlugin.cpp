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
#include "AbstractDataPlugin.h"

// Marble
#include "AbstractDataPluginModel.h"
#include "AbstractDataPluginItem.h"
#include "GeoPainter.h"
#include "GeoSceneLayer.h"
#include "MarbleModel.h"
#include "ViewportParams.h"
#include "MarbleDebug.h"

// Qt
#include <QTimer>
#include <QRegion>

namespace Marble
{

class AbstractDataPluginPrivate
{
 public:
    AbstractDataPluginPrivate()
        : m_model( 0 ),
          m_numberOfItems( 10 )
    {
      m_updateTimer.setSingleShot( true );
    }
    
    ~AbstractDataPluginPrivate() {
        delete m_model;
    }
    
    AbstractDataPluginModel *m_model;
    quint32 m_numberOfItems;
    QTimer m_updateTimer;
};

AbstractDataPlugin::AbstractDataPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      d( new AbstractDataPluginPrivate )
{
  connect( &d->m_updateTimer, SIGNAL(timeout()), this, SIGNAL(repaintNeeded()) );
}

AbstractDataPlugin::~AbstractDataPlugin()
{
    delete d;
}

bool AbstractDataPlugin::isInitialized() const
{
    return model() != 0;
}

QStringList AbstractDataPlugin::backendTypes() const
{
    return QStringList( name() );
}

QString AbstractDataPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList AbstractDataPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

bool AbstractDataPlugin::render( GeoPainter *painter, ViewportParams *viewport,
             const QString& renderPos, GeoSceneLayer * layer)
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    QList<AbstractDataPluginItem*> items = d->m_model->items( viewport, numberOfItems() );
    painter->save();

    // Paint the most important item at last
    for( int i = items.size() - 1; i >= 0; --i ) {
        items.at( i )->paintEvent( painter, viewport );
    }

    painter->restore();

    return true;
}

AbstractDataPluginModel *AbstractDataPlugin::model()
{
    return d->m_model;
}

const AbstractDataPluginModel *AbstractDataPlugin::model() const
{
    return d->m_model;
}

void AbstractDataPlugin::setModel( AbstractDataPluginModel* model )
{
    if ( d->m_model ) {
        disconnect( d->m_model, SIGNAL(itemsUpdated()), this, SLOT(delayedUpdate()) );
        delete d->m_model;
    }
    d->m_model = model;

    connect( d->m_model, SIGNAL(itemsUpdated()), this, SLOT(delayedUpdate()) );
    connect( d->m_model, SIGNAL(favoriteItemsChanged(QStringList)), this,
             SLOT(favoriteItemsChanged(QStringList)) );
    connect( d->m_model, SIGNAL(favoriteItemsOnlyChanged()), this,
                         SIGNAL(favoriteItemsOnlyChanged()) );

    emit favoritesModelChanged();
}

quint32 AbstractDataPlugin::numberOfItems() const
{
    return d->m_numberOfItems;
}
    
void AbstractDataPlugin::setNumberOfItems( quint32 number )
{
    bool changed = ( number != d->m_numberOfItems );
    d->m_numberOfItems = number;

    if ( changed )
        emit changedNumberOfItems( number );
}

QList<AbstractDataPluginItem *> AbstractDataPlugin::whichItemAt( const QPoint& curpos )
{
    if ( d->m_model && enabled() && visible()) {
        return d->m_model->whichItemAt( curpos );
    }
    else {
        return QList<AbstractDataPluginItem *>();
    }
}

RenderPlugin::RenderType AbstractDataPlugin::renderType() const
{
    return OnlineRenderType;
}

void AbstractDataPlugin::setFavoriteItemsOnly( bool favoriteOnly )
{
    if ( d->m_model && d->m_model->isFavoriteItemsOnly() != favoriteOnly ) {
        d->m_model->setFavoriteItemsOnly( favoriteOnly );
    }
}

bool AbstractDataPlugin::isFavoriteItemsOnly() const
{
    return d->m_model && d->m_model->isFavoriteItemsOnly();
}

QObject *AbstractDataPlugin::favoritesModel()
{
    return d->m_model ? d->m_model->favoritesModel() : 0;
}

void AbstractDataPlugin::favoriteItemsChanged( const QStringList& favoriteItems )
{
  Q_UNUSED( favoriteItems )
}

void AbstractDataPlugin::delayedUpdate()
{
  if ( !d->m_updateTimer.isActive() )
  {
    d->m_updateTimer.start( 500 );
  }
}

} // namespace Marble

#include "moc_AbstractDataPlugin.cpp"
