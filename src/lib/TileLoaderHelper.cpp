/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>"
 * Copyright 2008       Patrick Spendrin <ps_ml@gmx.de>"
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TileLoaderHelper.h"

#include <cmath>

#include <QtCore/QDebug>

#include "GeoSceneTexture.h"
#include "global.h"

int TileLoaderHelper::levelToRow( int level )
{
    if ( level < 0 ) {
        qDebug() << QString( "TileLoaderHelper::levelToRow(): Invalid level: %1" )
            .arg( level );
        return 0;
    }
    return (int)std::pow( 2.0, (double)( level ) );
}

int TileLoaderHelper::levelToColumn( int level )
{
    if ( level < 0 ) {
        qDebug() << QString( "TileLoaderHelper::levelToColumn(): Invalid level: %1" )
            .arg( level );
        return 0;
    }
    return (int)std::pow( 2.0, (double)( level + 1 ) );
}

int TileLoaderHelper::rowToLevel( int row )
{
    if ( row < 1 )    {
        qDebug() << QString( "TileLoaderHelper::rowToLevel(): Invalid number of rows: %1" )
            .arg( row );
        return 0;
    }
    return (int)( std::log( (double)row ) / std::log( (double)2.0 ) );
}

int TileLoaderHelper::columnToLevel( int column )
{
    if ( column < 2 ) {
        qDebug() << QString( "TileLoaderHelper::columnToLevel(): Invalid number of columns: %1" )
        .arg( column );
        return 0;
    }
    return (int)( std::log( (double)(column / 2) ) / std::log( (double)2.0 ) );
}

QString TileLoaderHelper::relativeTileFileName( GeoSceneTexture *textureLayer, int level, int x,
                                                int y )
{
    QString relFileName;
    if ( textureLayer ) {
        const QString suffix = textureLayer->fileFormat().toLower();
        switch ( textureLayer->storageLayoutMode() ) {
        case GeoSceneTexture::Marble:
            // FIXME: use format information from GeoSceneTexturex
            relFileName = QString( "%1/%2/%3/%3_%4.%5" )
                .arg( themeStr( textureLayer ) )
                .arg( level )
                .arg( y, tileDigits, 10, QChar('0') )
                .arg( x, tileDigits, 10, QChar('0') )
                .arg( suffix );
            break;
        case GeoSceneTexture::OpenStreetMap:
            // FIXME: use format information from GeoSceneTexturex
            relFileName = QString( "%1/%2/%3/%4.%5" )
                .arg( themeStr( textureLayer ) )
                .arg( level )
                .arg( x )
                .arg( y )
                .arg( suffix );
            break;
        }
    }
    return relFileName;
}

QString TileLoaderHelper::themeStr( GeoSceneTexture *textureLayer )
{
    QString oldThemeStr;

    if ( textureLayer ) {
         oldThemeStr = "maps/" + textureLayer->sourceDir();
    }

    return oldThemeStr;
}
