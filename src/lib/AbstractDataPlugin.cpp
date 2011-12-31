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
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QRegion>

namespace Marble
{

class AbstractDataPluginPrivate
{
 public:
    AbstractDataPluginPrivate()
        : m_model( 0 ),
          m_numberOfItems( 10 )
    {
    }
    
    ~AbstractDataPluginPrivate() {
        delete m_model;
    }
    
    AbstractDataPluginModel *m_model;
    QString m_name;
    quint32 m_numberOfItems;
};

AbstractDataPlugin::AbstractDataPlugin()
    : RenderPlugin(),
      d( new AbstractDataPluginPrivate )
{
}

AbstractDataPlugin::~AbstractDataPlugin()
{
    delete d;
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

bool AbstractDataPlugin::isInitialized() const
{
    return true;
}

bool AbstractDataPlugin::render( GeoPainter *painter, ViewportParams *viewport,
             const QString& renderPos, GeoSceneLayer * layer)
{
    if ( ( 0 == d->m_model ) || !isInitialized() ) {
        return true;
    }
    
    if ( renderPos != "ALWAYS_ON_TOP" ) {
        return true;
    }

    QList<AbstractDataPluginItem*> items = d->m_model->items( viewport,
                                                              marbleModel(),
                                                              numberOfItems() );
    painter->save();
    
    // Paint the most important item at last
    for( int i = items.size() - 1; i >= 0; --i ) {
        items.at( i )->paintEvent( painter, viewport, renderPos, layer );
    }
    
    painter->restore();
    
    return true;
}

AbstractDataPluginModel *AbstractDataPlugin::model() const
{
    return d->m_model;
}

void AbstractDataPlugin::setModel( AbstractDataPluginModel* model )
{
    if ( d->m_model ) {
        disconnect( d->m_model, SIGNAL( itemsUpdated() ), this, SLOT( requestRepaint() ) );
        delete d->m_model;
    }
    d->m_model = model;

    connect( d->m_model, SIGNAL( itemsUpdated() ), this, SLOT( requestRepaint() ) );
    connect( d->m_model, SIGNAL( favoriteItemsChanged( const QStringList& ) ), this,
             SLOT( favoriteItemsChanged( const QStringList& ) ) );
}

const PluginManager* AbstractDataPlugin::pluginManager() const
{
    return marbleModel()->pluginManager();
}

QString AbstractDataPlugin::nameId() const
{
    return d->m_name;
}
    
void AbstractDataPlugin::setNameId( const QString& name )
{
    d->m_name = name;
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
    return Online;
}

void AbstractDataPlugin::requestRepaint()
{
    emit repaintNeeded( QRegion() );
}

void AbstractDataPlugin::favoriteItemsChanged( const QStringList& favoriteItems )
{
    Q_UNUSED( favoriteItems )
}

} // namespace Marble

#include "AbstractDataPlugin.moc"
