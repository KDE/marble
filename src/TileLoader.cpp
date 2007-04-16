/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright (C) 2005 Torsten Rahn (rahn@kde.org)
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

#include "TileLoader.h"

#include <cmath>

#include "katlasdirs.h"
#include "TextureTile.h"

#include <QtCore/QDebug>

#ifdef Q_CC_MSVC
double log(int i)
{
    return log((double)i);
}
#endif

TileLoader::TileLoader( const QString& theme ){

	setMap( theme );
}

void TileLoader::setMap( const QString& theme ){
//	Initialize map theme.
        flush();

	m_theme = theme;

	m_tile = new TextureTile( 0, 0, 0, m_theme );

//	We assume that all tiles have the same size. TODO: check to be safe
	m_tileWidth = m_tile->rawtile()->width();
	m_tileHeight = m_tile->rawtile()->height();

	delete m_tile;
}

void TileLoader::resetTilehash(){

	QHash<int, TextureTile*>::const_iterator it = m_tileHash.constBegin();
	while (it != m_tileHash.constEnd()) {
		m_tileHash.value(it.key())->setUsed( false );
		++it;
	}
}

void TileLoader::cleanupTilehash(){
//	Make sure that tiles which haven't been used during the last
//	rendering of the map at all get removed from the tile hash.

	QHashIterator<int, TextureTile*> it(m_tileHash);
	while (it.hasNext()) {
		it.next();
		if ((it.value())->used() == false){
//			qDebug("Removing " + QString::number(it.key()).toLatin1());
			delete m_tileHash.value(it.key());
			m_tileHash.remove(it.key());	
		}
	}
}

void TileLoader::flush(){
//	Remove all tiles from m_tileHash
	QHash <int, TextureTile*>::const_iterator it;
	for( it = m_tileHash.begin(); it != m_tileHash.constEnd(); ++it ) 
		delete (*it);
	m_tileHash.clear();
}


TextureTile* TileLoader::loadTile( int tilx, int tily, int tileLevel ){
//	Choosing the correct tile via Lng/Lat info 

	m_tileId =  (tilx *1000) + tily;

	// If the tile hasn't been loaded into the m_tileHash yet, then do so
	if (!m_tileHash.contains( m_tileId )){	
		m_tile = new TextureTile(tilx, tily, tileLevel, m_theme);
		m_tileHash[m_tileId] = m_tile;
	}
	// otherwise pick the correct one from the hash
	else {
		m_tile=m_tileHash.value(m_tileId);
		if (!m_tile->used()){
			m_tile->setUsed(true);
			m_tileHash[m_tileId]=m_tile;
		}
	}

	return m_tile;
}

int TileLoader::levelToRow( int level ){
	return (int)pow( 2.0, (double)( level ) );
}

int TileLoader::levelToColumn( int level ){
	return (int)pow( 2.0, (double)( level + 1 ) );
}

int TileLoader::rowToLevel( int row ){
	return (int)( log( row ) / log( 2 ) );
}

int TileLoader::columnToLevel( int column ){
	return (int)( log( column / 2 ) / log( 2 ) );
}

int TileLoader::maxCompleteTileLevel( const QString& theme ){

	bool noerr = true; 

	int tilelevel = -1;
	int trylevel = 0;

//	if ( m_bitmaplayer.type.toLower() == "bitmap" ){
	while ( noerr == true ){
		int nmaxit = TileLoader::levelToRow( trylevel );
		for ( int n=0; n < nmaxit; ++n) {
			int mmaxit = TileLoader::levelToColumn( trylevel );
			for ( int m=0; m < mmaxit; ++m){
				QString tilepath = KAtlasDirs::path( QString("maps/earth/%1/%2/%3/%3_%4.jpg").arg(theme).arg( trylevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') ) );
//				qDebug() << tilepath;
				noerr = QFile::exists( tilepath );
				if ( noerr == false ) break; 
			}
			if ( noerr == false ) break; 
		}	

		if ( noerr == true) tilelevel = trylevel;
		++trylevel;
	}

	if ( tilelevel == -1 ){
		qDebug("No Tiles Found!");
	}

	qDebug() << "Detected maximum complete tile level: " << tilelevel;

	return tilelevel;
}

int TileLoader::maxPartialTileLevel( const QString& theme ){

	QString tilepath = KAtlasDirs::path( QString("maps/earth/%1").arg(theme) );
	QStringList leveldirs = ( QDir( tilepath ) ).entryList( QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );

	int maxtilelevel = -1;

	QString str;
	bool ok = true;

	QStringList::const_iterator constIterator;
	for (constIterator = leveldirs.constBegin(); constIterator != leveldirs.constEnd();
		++constIterator){
		int value = (*constIterator).toInt( &ok, 10 );
//		qDebug() << "Value: " << value  << "Ok: " << ok;
		if ( ok && value > maxtilelevel ) maxtilelevel = value;
	}

	qDebug() << "Detected maximum tile level that contains data: " << maxtilelevel;

	return maxtilelevel;
}

bool TileLoader::baseTilesAvailable( const QString& theme ){

	bool noerr = true; 

	int n = 0;

	// Check whether the two tiles from the lowest texture level are available

	for ( int m = 0; m < 2; ++m ){
		QString tilepath = KAtlasDirs::path( QString("maps/earth/%1/%2/%3/%3_%4.jpg").arg(theme).arg( 0 ).arg( 0, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') ) );

		noerr = QFile::exists( tilepath );

		if ( noerr == false ) break; 
	}

//	qDebug() << "Mandatory most basic tile level is fully available: " << noerr;

	return noerr;
}
