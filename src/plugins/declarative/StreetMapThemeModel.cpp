//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "StreetMapThemeModel.h"

#include "MapThemeManager.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"

#include <QtCore/QModelIndex>

StreetMapThemeModel::StreetMapThemeModel( Marble::MapThemeManager *themeManager )
{
    /** @todo Extend .dgml spec by categories to simplify this
      * The map theme model items should specify the planet and a set of
      * categories/tags (arbitrary strings) to simplify filtering for specific
      * map theme properties.
      * E.g. the item for earth/openstreetmap/openstreetmap.dgml should have
      * the planet set to earth and categories/tags like "OpenStreetMap, street map"
      */

    QStringList const themes = themeManager->mapThemeIds();
    foreach( const QString &theme, themes ) {
        if ( theme.startsWith( "earth/" ) ) {
            Marble::GeoSceneDocument* document = themeManager->loadMapTheme( theme );
            if ( document && document->head()->zoom()->maximum() > 3000 ) {
                m_streetMapThemeIds << document->head()->mapThemeId();
                delete document;
            }
        }
    }
}

int StreetMapThemeModel::count()
{
    return rowCount();
}

QString StreetMapThemeModel::name( const QString &id )
{
    for ( int i=0; i<rowCount(); ++i ) {
        if ( data( index( i, 0, QModelIndex() ), Qt::UserRole + 1 ).toString() == id ) {
            return data( index( i, 0, QModelIndex() ) ).toString();
        }
    }
    return QString();
}

int StreetMapThemeModel::indexOf(const QString &id)
{
    for ( int i=0; i<rowCount(); ++i ) {
        if ( data( index( i, 0, QModelIndex() ), Qt::UserRole + 1 ).toString() == id ) {
            return i;
        }
    }
    return -1;
}

bool StreetMapThemeModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
     QModelIndex const index = sourceModel()->index( sourceRow, 0, sourceParent );
     return m_streetMapThemeIds.contains( index.data( Qt::UserRole + 1 ).toString() );
}

#include "StreetMapThemeModel.moc"
