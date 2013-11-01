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
#include <QEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QRegion>
#if QT_VERSION < 0x050000
  #include <QDeclarativeComponent>
  #include <QDeclarativeContext>
  #include <QDeclarativeItem>
  typedef QDeclarativeComponent QQmlComponent;
  typedef QDeclarativeContext QQmlContext;
  typedef QDeclarativeItem QQuickItem;
#else
  #include <QQmlComponent>
  #include <QQmlContext>
  #include <QQuickItem>
  #include <QGraphicsItem>
#endif

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
      m_updateTimer.setSingleShot( true );
    }
    
    ~AbstractDataPluginPrivate() {
        delete m_model;
    }
    
    AbstractDataPluginModel *m_model;
    quint32 m_numberOfItems;
    QQmlComponent* m_delegate;
    QGraphicsItem* m_delegateParent;
    QMap<AbstractDataPluginItem*,QQuickItem*> m_delegateInstances;
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

    if ( d->m_delegate ) {
        handleViewportChange( viewport );
    } else {
        QList<AbstractDataPluginItem*> items = d->m_model->items( viewport, numberOfItems() );
        painter->save();

        // Paint the most important item at last
        for( int i = items.size() - 1; i >= 0; --i ) {
            items.at( i )->paintEvent( painter, viewport );
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

void AbstractDataPlugin::setDelegate( QQmlComponent *delegate, QGraphicsItem* parent )
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
    return d->m_model && d->m_model->isFavoriteItemsOnly();
}

QObject *AbstractDataPlugin::favoritesModel()
{
    return d->m_model ? d->m_model->favoritesModel() : 0;
}

void AbstractDataPlugin::handleViewportChange( const ViewportParams *viewport )
{
    QList<AbstractDataPluginItem*> orphane = d->m_delegateInstances.keys();
    QList<AbstractDataPluginItem*> const items = d->m_model->items( viewport, numberOfItems() );
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
            QQmlContext *context = new QQmlContext( qmlContext( d->m_delegate ) );
            context->setContextObject( item );
            QList<QByteArray> const dynamicProperties = item->dynamicPropertyNames();
            foreach( const QByteArray &property, dynamicProperties ) {
                context->setContextProperty( property, item->property( property ) );
            }

            QObject* component = d->m_delegate->create( context );
            QQuickItem* newItem = qobject_cast<QQuickItem*>( component );
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
        QQuickItem* declarativeItem = d->m_delegateInstances[item];
        Q_ASSERT( declarativeItem );

        // Make sure we have a valid bounding rect for collision detection
        item->setProjection( viewport );
        item->setSize( QSizeF( declarativeItem->boundingRect().size() ) );

        int shiftX( 0 ), shiftY( 0 );
        switch( declarativeItem->transformOrigin() ) {
        case QQuickItem::TopLeft:
        case QQuickItem::Top:
        case QQuickItem::TopRight:
            break;
        case QQuickItem::Left:
        case QQuickItem::Center:
        case QQuickItem::Right:
            shiftY = declarativeItem->height() / 2;
            break;
        case QQuickItem::BottomLeft:
        case QQuickItem::Bottom:
        case QQuickItem::BottomRight:
            shiftY = declarativeItem->height();
            break;
        }

        switch( declarativeItem->transformOrigin() ) {
        case QQuickItem::TopLeft:
        case QQuickItem::Left:
        case QQuickItem::BottomLeft:
            break;
        case QQuickItem::Top:
        case QQuickItem::Center:
        case QQuickItem::Bottom:
            shiftX = declarativeItem->width() / 2;
            break;
        case QQuickItem::TopRight:
        case QQuickItem::Right:
        case QQuickItem::BottomRight:
            shiftX = declarativeItem->width();
            break;
        }

        declarativeItem->setX( x - shiftX );
        declarativeItem->setY( y - shiftY );
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

void AbstractDataPlugin::delayedUpdate()
{
  if ( !d->m_updateTimer.isActive() )
  {
    d->m_updateTimer.start( 500 );
  }
}

} // namespace Marble

#include "AbstractDataPlugin.moc"
