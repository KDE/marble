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
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeContext>

namespace Marble
{

class AbstractDataPluginPrivate
{
 public:
    AbstractDataPluginPrivate()
        : m_model( 0 ),
          m_numberOfItems( 10 ),
          m_delegate( 0 ),
          m_delegateParent( 0 )
    {
    }
    
    ~AbstractDataPluginPrivate() {
        delete m_model;
    }
    
    AbstractDataPluginModel *m_model;
    quint32 m_numberOfItems;
    QDeclarativeComponent* m_delegate;
    QGraphicsItem* m_delegateParent;
    QMap<AbstractDataPluginItem*,QDeclarativeItem*> m_delegateInstances;
};

AbstractDataPlugin::AbstractDataPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
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

bool AbstractDataPlugin::render( GeoPainter *painter, ViewportParams *viewport,
             const QString& renderPos, GeoSceneLayer * layer)
{
    if ( !d->m_model || !isInitialized() ) {
        return true;
    }
    
    if ( renderPos != "ALWAYS_ON_TOP" ) {
        return true;
    }

    if ( d->m_delegate ) {
        handleViewportChange( painter, viewport );
    } else {
        QList<AbstractDataPluginItem*> items = d->m_model->items( viewport,
                                                                  marbleModel(),
                                                                  numberOfItems() );
        painter->save();

        // Paint the most important item at last
        for( int i = items.size() - 1; i >= 0; --i ) {
            items.at( i )->paintEvent( painter, viewport, renderPos, layer );
        }

        painter->restore();
    }
    
    return true;
}

AbstractDataPluginModel *AbstractDataPlugin::model() const
{
    return d->m_model;
}

void AbstractDataPlugin::setModel( AbstractDataPluginModel* model )
{
    if ( d->m_model ) {
        disconnect( d->m_model, SIGNAL( itemsUpdated() ), this, SIGNAL( repaintNeeded() ) );
        delete d->m_model;
    }
    d->m_model = model;

    connect( d->m_model, SIGNAL( itemsUpdated() ), this, SIGNAL( repaintNeeded() ) );
    connect( d->m_model, SIGNAL( favoriteItemsChanged( const QStringList& ) ), this,
             SLOT( favoriteItemsChanged( const QStringList& ) ) );
    connect( d->m_model, SIGNAL( favoriteItemsOnlyChanged() ), this,
                         SIGNAL( favoriteItemsOnlyChanged() ) );
}

const PluginManager* AbstractDataPlugin::pluginManager() const
{
    return marbleModel()->pluginManager();
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

void AbstractDataPlugin::setDelegate( QDeclarativeComponent *delegate, QGraphicsItem* parent )
{
    qDeleteAll( d->m_delegateInstances.values() );
    d->m_delegateInstances.clear();

    d->m_delegate = delegate;
    d->m_delegateParent = parent;
}

void AbstractDataPlugin::setFavoriteItemsOnly( bool favoriteOnly )
{
    if ( d->m_model && d->m_model->isFavoriteItemsOnly() != favoriteOnly ) {
        d->m_model->setFavoriteItemsOnly( favoriteOnly );
    }
}

bool AbstractDataPlugin::isFavoriteItemsOnly() const
{
    return d->m_model->isFavoriteItemsOnly();
}

void AbstractDataPlugin::handleViewportChange( GeoPainter* painter, ViewportParams* viewport )
{
    QList<AbstractDataPluginItem*> orphane = d->m_delegateInstances.keys();
    QList<AbstractDataPluginItem*> const items = d->m_model->items( viewport, marbleModel(), numberOfItems() );
    foreach( AbstractDataPluginItem* item, items ) {
        qreal x, y;
        Marble::GeoDataCoordinates const coordinates = item->coordinate();
        bool const visible = viewport->screenCoordinates( coordinates.longitude(), coordinates.latitude(), x, y );

        if ( !d->m_delegateInstances.contains( item ) ) {
            if ( !visible ) {
                // We don't have, but don't need it either. Shouldn't happen though as the model checks for it already.
                continue;
            }

            // Create a new QML object instance using the delegate as the factory. The original
            // data plugin item is set as the context object, i.e. all its properties are available
            // to QML directly with their names
            QDeclarativeContext *context = new QDeclarativeContext( qmlContext( d->m_delegate ) );
            context->setContextObject( item );
            QObject* component = d->m_delegate->create( context );
            QDeclarativeItem* newItem = qobject_cast<QDeclarativeItem*>( component );
            QGraphicsItem* graphicsItem = qobject_cast<QGraphicsItem*>( component );
            if ( graphicsItem && newItem ) {
                graphicsItem->setParentItem( d->m_delegateParent );
            }

            if ( newItem ) {
                d->m_delegateInstances[item] = newItem;
            } else {
                mDebug() << "Failed to create delegate";
                continue;
            }
        } else if ( !visible ) {
            // Previously visible but not anymore => needs to be deleted. Orphane list takes care of it later.
            // Shouldn't happen though as the model checks for it already.
            continue;
        }

        Q_ASSERT( visible );
        QDeclarativeItem* declarativeItem = d->m_delegateInstances[item];
        Q_ASSERT( declarativeItem );

        // Make sure we have a valid bounding rect for collision detection
        item->setProjection( viewport, painter );
        item->setSize( QSizeF( declarativeItem->boundingRect().size() ) );

        int shiftX( 0 ), shiftY( 0 );
        switch( declarativeItem->transformOrigin() ) {
        case QDeclarativeItem::TopLeft:
        case QDeclarativeItem::Top:
        case QDeclarativeItem::TopRight:
            break;
        case QDeclarativeItem::Left:
        case QDeclarativeItem::Center:
        case QDeclarativeItem::Right:
            shiftY = declarativeItem->height() / 2;
            break;
        case QDeclarativeItem::BottomLeft:
        case QDeclarativeItem::Bottom:
        case QDeclarativeItem::BottomRight:
            shiftY = declarativeItem->height();
            break;
        }

        switch( declarativeItem->transformOrigin() ) {
        case QDeclarativeItem::TopLeft:
        case QDeclarativeItem::Left:
        case QDeclarativeItem::BottomLeft:
            break;
        case QDeclarativeItem::Top:
        case QDeclarativeItem::Center:
        case QDeclarativeItem::Bottom:
            shiftX = declarativeItem->width() / 2;
            break;
        case QDeclarativeItem::TopRight:
        case QDeclarativeItem::Right:
        case QDeclarativeItem::BottomRight:
            shiftX = declarativeItem->width();
            break;
        }

        declarativeItem->setPos( x - shiftX, y - shiftY );
        orphane.removeOne( item );
    }

    // Cleanup
    foreach( AbstractDataPluginItem* item, orphane ) {
        Q_ASSERT( d->m_delegateInstances.contains( item ) );
        delete d->m_delegateInstances[item];
        d->m_delegateInstances.remove( item );
    }
}

void AbstractDataPlugin::favoriteItemsChanged( const QStringList& favoriteItems )
{
    Q_UNUSED( favoriteItems )
}

} // namespace Marble

#include "AbstractDataPlugin.moc"
