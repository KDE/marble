//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MarbleDeclarativeRunnerManager.h"

#include "PluginManager.h"

namespace Marble
{
namespace Declarative
{

MarbleRunnerManager::MarbleRunnerManager( QObject *parent ) :
    QObject( parent ), m_runnerManager( 0 ), m_searchResult( 0 )
{
    // nothing to do
}

void MarbleRunnerManager::search( const QString &term )
{
    if ( !m_runnerManager ) {
        /** @todo FIXME: Ideally we'd share the plugin manager with MarbleWidget,
          * but we do not have access to it here
          */
        PluginManager* pluginManager = new PluginManager( this );
        m_runnerManager = new Marble::MarbleRunnerManager( pluginManager, this );
        connect( m_runnerManager, SIGNAL( searchFinished( QString ) ),
                 this, SIGNAL( searchFinished() ) );
        connect( m_runnerManager, SIGNAL( searchResultChanged( QAbstractItemModel* ) ),
                 this, SLOT( searchModelChanged( QAbstractItemModel* ) ) );
    }

    m_runnerManager->findPlacemarks( term );
}

QList<QObject*> MarbleRunnerManager::searchResult() const
{
    QList<QObject*> result;
    for ( int i = 0; m_searchResult && i < m_searchResult->rowCount(); ++i ) {
        QString name = m_searchResult->data( m_searchResult->index( i ), Qt::DisplayRole ).toString();
        QVariant data = m_searchResult->data( m_searchResult->index( i ), MarblePlacemarkModel::CoordinateRole );
        GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>( data );
        qreal lon = pos.longitude( GeoDataCoordinates::Degree );
        qreal lat = pos.latitude( GeoDataCoordinates::Degree );
        result << new Placemark( name, new Coordinate( lon, lat ) );
    }

    return result;
}

void MarbleRunnerManager::searchModelChanged( QAbstractItemModel *model )
{
    m_searchResult = static_cast<MarblePlacemarkModel*>( model );
}

}
}

#include "MarbleDeclarativeRunnerManager.moc"
