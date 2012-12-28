//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MapThemeModel.h"

#include "MapThemeManager.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"

#include <QtCore/QModelIndex>
#include <QtCore/QDebug>
#include <QtGui/QStandardItemModel>

MapThemeModel::MapThemeModel( QObject *parent ) : QSortFilterProxyModel( parent ),
    m_themeManager( new Marble::MapThemeManager( this ) ), m_mapThemeFilters( MapThemeModel::AnyTheme )
{
    setSourceModel( m_themeManager->mapThemeModel() );
    handleChangedThemes();
    connect( m_themeManager, SIGNAL( themesChanged() ), this, SLOT( handleChangedThemes() ) );
}

int MapThemeModel::count()
{
    return rowCount();
}

QString MapThemeModel::name( const QString &id )
{
    for ( int i=0; i<rowCount(); ++i ) {
        if ( data( index( i, 0, QModelIndex() ), Qt::UserRole + 1 ).toString() == id ) {
            return data( index( i, 0, QModelIndex() ) ).toString();
        }
    }
    return QString();
}

int MapThemeModel::indexOf(const QString &id)
{
    for ( int i=0; i<rowCount(); ++i ) {
        if ( data( index( i, 0, QModelIndex() ), Qt::UserRole + 1 ).toString() == id ) {
            return i;
        }
    }
    return -1;
}

MapThemeModel::MapThemeFilters MapThemeModel::mapThemeFilter() const
{
    return m_mapThemeFilters;
}

void MapThemeModel::setMapThemeFilter( MapThemeFilters filters )
{
    if ( filters != m_mapThemeFilters ) {
        m_mapThemeFilters = filters;
        emit mapThemeFilterChanged();
    }
}

bool MapThemeModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
     QModelIndex const index = sourceModel()->index( sourceRow, 0, sourceParent );
     QString const themeId = index.data( Qt::UserRole + 1 ).toString();
     if ( m_mapThemeFilters & MapThemeModel::HighZoom && m_streetMapThemeIds.contains( themeId ) ) {
        return false;
     }

     if ( m_mapThemeFilters & MapThemeModel::LowZoom && !m_streetMapThemeIds.contains( themeId ) ) {
        return false;
     }

     if ( m_mapThemeFilters & MapThemeModel::Terrestrial && themeId.startsWith( QLatin1String( "earth/" ) ) ) {
        return false;
     }

     if ( m_mapThemeFilters & MapThemeModel::Extraterrestrial && !themeId.startsWith( QLatin1String( "earth/" ) ) ) {
        return false;
     }

     return true;
}

void MapThemeModel::handleChangedThemes()
{
    /** @todo Extend .dgml spec by categories to simplify this
      * The map theme model items should specify the planet and a set of
      * categories/tags (arbitrary strings) to simplify filtering for specific
      * map theme properties.
      * E.g. the item for earth/openstreetmap/openstreetmap.dgml should have
      * the planet set to earth and categories/tags like "OpenStreetMap, street map"
      */

    m_streetMapThemeIds.clear();
    QStringList const themes = m_themeManager->mapThemeIds();
    foreach( const QString &theme, themes ) {
        Marble::GeoSceneDocument* document = m_themeManager->loadMapTheme( theme );
        if ( document && document->head()->zoom()->maximum() > 3000 ) {
            m_streetMapThemeIds << document->head()->mapThemeId();
            delete document;
        }
    }

    reset();
}

#include "MapThemeModel.moc"
