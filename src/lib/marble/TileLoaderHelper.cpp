/**
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 * Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
 * Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
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

#include "MarbleDebug.h"

#include "MarbleGlobal.h"

namespace Marble
{

int TileLoaderHelper::levelToRow( int levelZeroRows, int level )
{
    if ( level < 0 ) {
        mDebug() << QString( "TileLoaderHelper::levelToRow(): Invalid level: %1" )
            .arg( level );
        return 0;
    }
    return levelZeroRows << level;
}

int TileLoaderHelper::levelToColumn( int levelZeroColumns, int level )
{
    if ( level < 0 ) {
        mDebug() << QString( "TileLoaderHelper::levelToColumn(): Invalid level: %1" )
            .arg( level );
        return 0;
    }
    return levelZeroColumns << level;
}

int TileLoaderHelper::rowToLevel( int levelZeroRows, int row )
{
    if ( row < levelZeroRows ) {
        mDebug() << QString( "TileLoaderHelper::rowToLevel(): Invalid number of rows: %1" )
            .arg( row );
        return 0;
    }
    return (int)( std::log( (qreal)(row / levelZeroRows) ) / std::log( (qreal)2.0 ) );
}

int TileLoaderHelper::columnToLevel( int levelZeroColumns, int column )
{
    if ( column < levelZeroColumns ) {
        mDebug() << QString( "TileLoaderHelper::columnToLevel(): Invalid number of columns: %1" )
        .arg( column );
        return 0;
    }
    return (int)( std::log( (qreal)(column / levelZeroColumns) ) / std::log( (qreal)2.0 ) );
}

}
