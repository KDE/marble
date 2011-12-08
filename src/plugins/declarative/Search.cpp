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
    connect( m_marbleWidget, SIGNAL( themeChanged( QString ) ),
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
        m_runnerManager->setModel( m_marbleWidget->model() );
        connect( m_runnerManager, SIGNAL( searchFinished( QString ) ),
                 this, SLOT( handleSearchResult() ) );
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

    QHash<int,QByteArray> const roles = model->roleNames();
    for ( int i=0; i<m_searchResult->rowCount(); ++i ) {
        QDeclarativeContext *context = new QDeclarativeContext( qmlContext( m_placemarkDelegate ) );
        QModelIndex const index = m_searchResult->index( i );
        QHash<int,QByteArray>::const_iterator iter = roles.constBegin();
        context->setContextProperty( "index", i );
        for ( ; iter != roles.constEnd(); ++iter ) {
            context->setContextProperty( iter.value(), m_searchResult->data( index, iter.key() ) );
        }
        QObject* component = m_placemarkDelegate->create( context );
        QGraphicsItem* graphicsItem = qobject_cast<QGraphicsItem*>( component );
        QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>( component );
        if ( graphicsItem && item ) {
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
        bool const onEarth = m_marbleWidget->model()->planetId() == "earth";
        QMap<int, QDeclarativeItem*>::const_iterator iter = m_placemarks.constBegin();
        while ( iter != m_placemarks.constEnd() ) {
            qreal x(0), y(0);
            QVariant position = m_searchResult->data( m_searchResult->index( iter.key() ), MarblePlacemarkModel::CoordinateRole );
            GeoDataCoordinates const coordinates = qVariantValue<GeoDataCoordinates>( position );
            bool const visible = onEarth && m_marbleWidget->screenCoordinates( coordinates.longitude( GeoDataCoordinates::Degree ), coordinates.latitude( GeoDataCoordinates::Degree ), x, y );
            QDeclarativeItem* item = iter.value();
            if ( item ) {
                item->setVisible( visible );
                if ( visible ) {
                    int shiftX( 0 ), shiftY( 0 );
                    switch( item->transformOrigin() ) {
                    case QDeclarativeItem::TopLeft:
                    case QDeclarativeItem::Top:
                    case QDeclarativeItem::TopRight:
                        break;
                    case QDeclarativeItem::Left:
                    case QDeclarativeItem::Center:
                    case QDeclarativeItem::Right:
                        shiftY = item->height() / 2;
                        break;
                    case QDeclarativeItem::BottomLeft:
                    case QDeclarativeItem::Bottom:
                    case QDeclarativeItem::BottomRight:
                        shiftY = item->height();
                        break;
                    }

                    switch( item->transformOrigin() ) {
                    case QDeclarativeItem::TopLeft:
                    case QDeclarativeItem::Left:
                    case QDeclarativeItem::BottomLeft:
                        break;
                    case QDeclarativeItem::Top:
                    case QDeclarativeItem::Center:
                    case QDeclarativeItem::Bottom:
                        shiftX = item->width() / 2;
                        break;
                    case QDeclarativeItem::TopRight:
                    case QDeclarativeItem::Right:
                    case QDeclarativeItem::BottomRight:
                        shiftX = item->width();
                        break;
                    }

                    item->setPos( x - shiftX, y - shiftY );
                }
            }
            ++iter;
        }
    }
}

void Search::handleSearchResult()
{
    Q_ASSERT( m_marbleWidget ); // search wouldn't be started without
    Q_ASSERT( m_searchResult ); // search wouldn't be finished without

    GeoDataLineString placemarks;
    for ( int i = 0; i < m_searchResult->rowCount(); ++i ) {
        QVariant data = m_searchResult->index( i, 0 ).data( MarblePlacemarkModel::CoordinateRole );
        if ( !data.isNull() ) {
            placemarks << qVariantValue<GeoDataCoordinates>( data );
        }
    }

    if ( placemarks.size() > 1 ) {
        m_marbleWidget->centerOn( GeoDataLatLonBox::fromLineString( placemarks ) );
    }

    emit searchFinished();
}

}
}

#include "Search.moc"
