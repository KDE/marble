#include "texloader.h"

#include <QtCore/QTime>
#include <QtCore/QVector>
#include <cmath>

#include "katlasdirs.h"
#include "texturetile.h"

#include <QtCore/QDebug>

TextureLoader::TextureLoader( const QString& theme ){

	setMap( theme );
}

void TextureLoader::setMap( const QString& theme ){
//	Initialize map theme.
	m_theme = theme;

	m_tile = new TextureTile( 0, 0, 0, m_theme );

//	We assume that all tiles have the same size. TODO: check to be safe
	m_tileWidth = m_tile->rawtile()->width();
	m_tileHeight = m_tile->rawtile()->height();

	delete m_tile;
}

void TextureLoader::resetTilehash(){

	QHash<int, TextureTile*>::const_iterator it = m_tilehash.constBegin();
	while (it != m_tilehash.constEnd()) {
		m_tilehash.value(it.key())->setUsed( false );
		++it;
	}
}

void TextureLoader::cleanupTilehash(){
//	Make sure that tiles which haven't been used during the last
//	rendering of the map at all get removed from the tile hash.

	QHashIterator<int, TextureTile*> it(m_tilehash);
	while (it.hasNext()) {
		it.next();
		if ((it.value())->used() == false){
//			qDebug("Removing " + QString::number(it.key()).toLatin1());
			delete m_tilehash.value(it.key());
			m_tilehash.remove(it.key());	
		}
	}
}

void TextureLoader::flush(){
//	Remove all m_tiles from m_tilehash
	QHash <int, TextureTile*>::const_iterator it;
	for( it = m_tilehash.begin(); it != m_tilehash.constEnd(); it++ ) 
		delete (*it);
	m_tilehash.clear();
}


TextureTile* TextureLoader::loadTile( int tilx, int tily, int tileLevel ){
//	Choosing the correct m_tile via Lng/Lat info 

	m_tilekey =  (tilx *1000) + tily;

	// If the m_tile hasn't been loaded into the m_tilehash yet, then do so
	if (!m_tilehash.contains( m_tilekey )){	
		m_tile = new TextureTile(tilx, tily, tileLevel, m_theme);
		m_tilehash[m_tilekey] = m_tile;
	}
	// otherwise pick the correct one from the hash
	else {
		m_tile=m_tilehash.value(m_tilekey);
		if (!m_tile->used()){
			m_tile->setUsed(true);
			m_tilehash[m_tilekey]=m_tile;
		}
	}

	return m_tile;
}

int TextureLoader::levelToRow( int level ){
	return (int)pow( 2.0, (double)( level ) );
}

int TextureLoader::levelToColumn( int level ){
	return (int)pow( 2.0, (double)( level + 1 ) );
}

int TextureLoader::rowToLevel( int row ){
	return (int)( log( row ) / log( 2 ) );
}

int TextureLoader::columnToLevel( int column ){
	return (int)( log( column / 2 ) / log( 2 ) );
}

int TextureLoader::maxCompleteTileLevel( QString theme ){

	bool noerr = true; 

	int tilelevel = -1;
	int trylevel = 0;

//	if ( m_bitmaplayer.type.toLower() == "bitmap" ){
	while ( noerr == true ){
		int nmaxit = TextureLoader::levelToRow( trylevel );
		for ( int n=0; n < nmaxit; n++) {
			int mmaxit = TextureLoader::levelToColumn( trylevel );
			for ( int m=0; m < mmaxit; m++){
				QString tilepath = KAtlasDirs::path( QString("maps/earth/%1/%2/%3/%3_%4.jpg").arg(theme).arg( trylevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') ) );
//				qDebug() << tilepath;
				noerr = QFile::exists( tilepath );
				if ( noerr == false ) break; 
			}
			if ( noerr == false ) break; 
		}	

		if ( noerr == true) tilelevel = trylevel;
		trylevel++;
	}

	if ( tilelevel == -1 ){
		qDebug("No Tiles Found!");
	}

	qDebug() << "Detected maximum complete tile level: " << tilelevel;

	return tilelevel;
}

int TextureLoader::maxPartialTileLevel( QString theme ){

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

bool TextureLoader::baseTilesAvailable( QString theme ){

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
