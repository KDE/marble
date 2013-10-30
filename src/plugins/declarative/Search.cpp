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

#include "MarbleDeclarativeWidget.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "SearchRunnerManager.h"
#include "ViewportParams.h"

#if QT_VERSION < 0x050000
  #include <QDeclarativeContext>
  typedef QDeclarativeItem QQuickItem;
  typedef QDeclarativeContext QQmlContext;
  typedef QDeclarativeComponent QQmlComponent;
#else
  #include <QQmlContext>
#endif

Search::Search( QObject* parent ) : QObject( parent ),
    m_marbleWidget( 0 ), m_runnerManager( 0 ),
    m_searchResult( 0 ), m_placemarkDelegate( 0 )
{
    // nothing to do
}

MarbleWidget *Search::map()
{
    return m_marbleWidget;
}

void Search::setMap( MarbleWidget* widget )
{
    m_marbleWidget = widget;
    connect( m_marbleWidget, SIGNAL(visibleLatLonAltBoxChanged()),
             this, SLOT(updatePlacemarks()) );
    connect( m_marbleWidget, SIGNAL(mapThemeChanged()),
             this, SLOT(updatePlacemarks()) );
    emit mapChanged();
}

QQmlComponent* Search::placemarkDelegate()
{
    return m_placemarkDelegate;
}

void Search::setPlacemarkDelegate( QQmlComponent* delegate )
{
    m_placemarkDelegate = delegate;
    emit placemarkDelegateChanged();
}

void Search::find( const QString &searchTerm )
{
    if ( !m_runnerManager && m_marbleWidget ) {
        m_runnerManager = new Marble::SearchRunnerManager( m_marbleWidget->model(), this );
        connect( m_runnerManager, SIGNAL(searchFinished(QString)),
                 this, SLOT(handleSearchResult()) );
        connect( m_runnerManager, SIGNAL(searchResultChanged(QAbstractItemModel*)),
                 this, SLOT(updateSearchModel(QAbstractItemModel*)) );
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
    m_searchResult = static_cast<Marble::MarblePlacemarkModel*>( model );
    qDeleteAll( m_placemarks.values() );
    m_placemarks.clear();

    if ( !m_placemarkDelegate ) {
        return;
    }

    QHash<int,QByteArray> const roles = model->roleNames();
    for ( int i=0; i<m_searchResult->rowCount(); ++i ) {
        QQmlContext *context = new QQmlContext( qmlContext( m_placemarkDelegate ) );
        QModelIndex const index = m_searchResult->index( i );
        QHash<int,QByteArray>::const_iterator iter = roles.constBegin();
        context->setContextProperty( "index", i );
        for ( ; iter != roles.constEnd(); ++iter ) {
            context->setContextProperty( iter.value(), m_searchResult->data( index, iter.key() ) );
        }
        QObject* component = m_placemarkDelegate->create( context );
        QGraphicsItem* graphicsItem = qobject_cast<QGraphicsItem*>( component );
        QQuickItem* item = qobject_cast<QQuickItem*>( component );
        if ( graphicsItem && item ) {
            graphicsItem->setParentItem( m_marbleWidget );
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
        QMap<int, QQuickItem*>::const_iterator iter = m_placemarks.constBegin();
        while ( iter != m_placemarks.constEnd() ) {
            qreal x(0), y(0);
            QVariant position = m_searchResult->data( m_searchResult->index( iter.key() ), Marble::MarblePlacemarkModel::CoordinateRole );
            Marble::GeoDataCoordinates const coordinates = position.value<Marble::GeoDataCoordinates>();
            bool const visible = onEarth && m_marbleWidget->viewport()->screenCoordinates( coordinates.longitude( Marble::GeoDataCoordinates::Radian ),
                                                                                           coordinates.latitude( Marble::GeoDataCoordinates::Radian), x, y );
            QQuickItem* item = iter.value();
            if ( item ) {
                item->setVisible( visible );
                if ( visible ) {
                    int shiftX( 0 ), shiftY( 0 );
                    switch( item->transformOrigin() ) {
                    case QQuickItem::TopLeft:
                    case QQuickItem::Top:
                    case QQuickItem::TopRight:
                        break;
                    case QQuickItem::Left:
                    case QQuickItem::Center:
                    case QQuickItem::Right:
                        shiftY = item->height() / 2;
                        break;
                    case QQuickItem::BottomLeft:
                    case QQuickItem::Bottom:
                    case QQuickItem::BottomRight:
                        shiftY = item->height();
                        break;
                    }

                    switch( item->transformOrigin() ) {
                    case QQuickItem::TopLeft:
                    case QQuickItem::Left:
                    case QQuickItem::BottomLeft:
                        break;
                    case QQuickItem::Top:
                    case QQuickItem::Center:
                    case QQuickItem::Bottom:
                        shiftX = item->width() / 2;
                        break;
                    case QQuickItem::TopRight:
                    case QQuickItem::Right:
                    case QQuickItem::BottomRight:
                        shiftX = item->width();
                        break;
                    }

                    item->setX( x - shiftX );
                    item->setY( y - shiftY );
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

    Marble::GeoDataLineString placemarks;
    for ( int i = 0; i < m_searchResult->rowCount(); ++i ) {
        QVariant data = m_searchResult->index( i, 0 ).data( Marble::MarblePlacemarkModel::CoordinateRole );
        if ( !data.isNull() ) {
            placemarks << data.value<Marble::GeoDataCoordinates>();
        }
    }

    if ( placemarks.size() > 1 ) {
        m_marbleWidget->centerOn( Marble::GeoDataLatLonBox::fromLineString( placemarks ) );
    }

    emit searchFinished();
}

#include "Search.moc"
