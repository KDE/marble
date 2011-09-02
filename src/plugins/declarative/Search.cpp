//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Search.h"

#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"

namespace Marble
{
namespace Declarative
{

Search::Search( QObject* parent ) : QObject( parent ),
    m_marbleWidget( 0 ), m_runnerManager( 0 ),
    m_searchResult( 0 ), m_placemarkDelegate( 0 ),
    m_delegateParent( 0 )
{
    // nothing to do
}

void Search::setMarbleWidget( Marble::MarbleWidget* widget )
{
    m_marbleWidget = widget;
    connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox ) ),
             this, SLOT( updatePlacemarks() ) );
}

QDeclarativeComponent* Search::placemarkDelegate()
{
    return m_placemarkDelegate;
}

void Search::setPlacemarkDelegate( QDeclarativeComponent* delegate )
{
    m_placemarkDelegate = delegate;
    emit placemarkDelegateChanged();
}

void Search::setDelegateParent( QGraphicsItem* parent )
{
    m_delegateParent = parent;
}

void Search::find( const QString &searchTerm )
{
    if ( !m_runnerManager && m_marbleWidget ) {
        m_runnerManager = new Marble::MarbleRunnerManager( m_marbleWidget->model()->pluginManager(), this );
        connect( m_runnerManager, SIGNAL( searchFinished( QString ) ),
                 this, SIGNAL( searchFinished() ) );
        connect( m_runnerManager, SIGNAL( searchResultChanged( QAbstractItemModel* ) ),
                 this, SLOT( updateSearchModel( QAbstractItemModel* ) ) );
    }

    if ( m_runnerManager ) {
        m_runnerManager->findPlacemarks( searchTerm );
    }
}

QObject* Search::searchResultModel()
{
    return m_searchResult;
}

void Search::updateSearchModel( QAbstractItemModel *model )
{
    m_searchResult = static_cast<MarblePlacemarkModel*>( model );
    qDeleteAll( m_placemarks.values() );
    m_placemarks.clear();

    if ( !m_placemarkDelegate ) {
        return;
    }

    for ( int i=0; i<m_searchResult->rowCount(); ++i ) {
        QDeclarativeContext *context = new QDeclarativeContext( qmlContext( m_placemarkDelegate ) );
        QObject* component = m_placemarkDelegate->create( context );
        QGraphicsItem* graphicsItem = qobject_cast<QGraphicsItem*>( component );
        QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>( component );
        if ( graphicsItem && item ) {
            QVariant position = m_searchResult->data( m_searchResult->index( i ), MarblePlacemarkModel::CoordinateRole );
            GeoDataCoordinates const coordinates = qVariantValue<GeoDataCoordinates>( position );
            context->setContextProperty( "longitude", QVariant( coordinates.longitude( GeoDataCoordinates::Degree ) ) );
            context->setContextProperty( "latitude", QVariant( coordinates.latitude( GeoDataCoordinates::Degree ) ) );
            context->setContextProperty( "hit", QVariant( QString::number( i+1 ) ) );
            context->setContextProperty( "name", m_searchResult->data( m_searchResult->index( i ), Qt::DisplayRole ) );
            graphicsItem->setParentItem( m_delegateParent );
            m_placemarks[i] = item;
        } else {
            delete component;
        }
    }
    updatePlacemarks();
}

void Search::updatePlacemarks()
{
    if ( m_marbleWidget ) {
        QMap<int, QDeclarativeItem*>::const_iterator iter = m_placemarks.constBegin();
        while ( iter != m_placemarks.constEnd() ) {
            qreal x(0), y(0);
            QVariant position = m_searchResult->data( m_searchResult->index( iter.key() ), MarblePlacemarkModel::CoordinateRole );
            GeoDataCoordinates const coordinates = qVariantValue<GeoDataCoordinates>( position );
            bool const visible = m_marbleWidget->screenCoordinates( coordinates.longitude( GeoDataCoordinates::Degree ), coordinates.latitude( GeoDataCoordinates::Degree ), x, y );
            QDeclarativeItem* item = iter.value();
            if ( item ) {
                item->setVisible( visible );
                if ( visible ) {
                    item->setPos( x - item->width() / 2.0, y - item->height() / 2.0 );
                }
            }
            ++iter;
        }
    }
}

}
}

#include "Search.moc"
