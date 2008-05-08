/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "GeoSceneTexture.h"


GeoSceneTexture::GeoSceneTexture( const QString& name )
    : GeoSceneAbstractDataset( name ),
      m_sourceDir( "" ),
      m_installMap( "" ),
      m_storageLayoutMode( Marble ),
      m_customStorageLayout( "" ),
      m_levelZeroColumns( 0 ),
      m_levelZeroRows( 0 )
{
    /* NOOP */
}

GeoSceneTexture::~GeoSceneTexture()
{
    /* NOOP */
}

QString GeoSceneTexture::sourceDir() const
{
    return m_sourceDir;
}

void GeoSceneTexture::setSourceDir( const QString& sourceDir )
{
    m_sourceDir = sourceDir;
}

QString GeoSceneTexture::installMap() const
{
    return m_installMap;
}

void GeoSceneTexture::setInstallMap( const QString& installMap )
{
    m_installMap = installMap;
}

GeoSceneTexture::StorageLayoutMode GeoSceneTexture::storageLayoutMode() const
{
    return m_storageLayoutMode;
}

void GeoSceneTexture::setStorageLayoutMode( const StorageLayoutMode mode )
{
    m_storageLayoutMode = mode;
}

QString GeoSceneTexture::customStorageLayout()const
{
    return m_customStorageLayout;
}

void GeoSceneTexture::setCustomStorageLayout( const QString& layout )
{
    m_customStorageLayout = layout;
}

int GeoSceneTexture::levelZeroColumns() const
{
   return m_levelZeroColumns;
}

void GeoSceneTexture::setLevelZeroColumns( const int columns )
{
    m_levelZeroColumns = columns;
}

int GeoSceneTexture::levelZeroRows() const
{
    return m_levelZeroRows;
}

void GeoSceneTexture::setLevelZeroRows( const int rows )
{
    m_levelZeroRows = rows;
}

QString GeoSceneTexture::type()
{
    return "texture";
}
