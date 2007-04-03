#include "texloader.h"

#include <QtCore/QTime>
#include <QtCore/QVector>
#include <cmath>

#include "katlasdirs.h"
#include "texturetile.h"

#include <QtCore/QDebug>

const float TWOPI = 2 * M_PI;

TextureLoader::TextureLoader( const QString& theme ){

	setMap( theme );
	m_depth = -1;
}

void TextureLoader::setMap( const QString& theme ){
//	Initialize map theme.
	m_theme = theme;

	m_texlevel = 0;

	m_oldtexlevel = -1;
	
	tilx=65535;
	tily=65535;

	m_tile = new TextureTile( 0, 0, m_texlevel, m_theme );

//	We assume that all tiles have the same size. TODO: check to be safe
	m_tilw = m_tile->rawtile()->width();
	m_tilh = m_tile->rawtile()->height();
	delete m_tile;

	setTexLevel(0);
}

void TextureLoader::resetTilehash(){

	QHash<int, TextureTile*>::const_iterator it = m_tilehash.constBegin();
	while (it != m_tilehash.constEnd()) {
		m_tilehash.value(it.key())->setUsed( false );
		++it;
	}

	tilx = 65535;
	tily = 65535;
	m_tilxw = 65535;
	m_tilyh = 65535;

	normhalfalpha = maxhalfalpha - m_tilxw;
	normquatbeta = maxquatbeta - m_tilyh;
	normfullalpha = maxfullalpha - m_tilxw;
	normhalfbeta = maxhalfbeta - m_tilyh;
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

void TextureLoader::setN(const int n){
//	Define the number of points to be skipped for interpolation 
	m_n = n; m_ninv = 1.0f/(float)(n);
}

inline void TextureLoader::getPixelValue(const float& radlng, const float& radlat, QRgb* line){

//	Convert lng and lat measured in radiant 
//	to pixel position on the current m_tile ...
	posx = (int)(normhalfalpha + radlng * rad2pixw);
	posy = (int)(normquatbeta + radlat * rad2pixh);

	if ( posx >= m_tilw || posx < 0 ) {
		// necessary to prevent e.g. crash if TextureMapper::radalpha = -pi
		if ( posx > normfullalpha ) posx = normfullalpha;
		if ( posy > normhalfbeta  ) posy = normhalfbeta;

//		The origin (0, 0) is in the upper left corner
//		lng: 360 deg = 43200 pixel
//		lat: 180 deg = 21600 pixel
		int lng = posx + m_tilxw;
		int lat = posy + m_tilyh;

		tilx = lng / m_tilw; // Counts the m_tiles left from the current m_tile ("m_tileposition") 
		tily = lat / m_tilh; // Counts the m_tiles on the top from the current m_tile

		loadTile();

		posx = lng - m_tilxw;
		posy = lat - m_tilyh;
	}
	else {
		if ( posy >= m_tilh || posy < 0 ) {

			if ( posy > normhalfbeta ) posy = normhalfbeta;
			int lat = posy + m_tilyh;
			tily = lat / m_tilh;
			loadTile();
			posy = lat - m_tilyh;
		}
	}

	if (m_tile->depth() == 8)
		*line = m_tile->jumpTable8[posy][posx];
	else
		*line = m_tile->jumpTable32[posy][posx];
}

void TextureLoader::getPixelValueApprox(const float& lng, const float& lat, QRgb* line){
//	This method executes the interpolation for skipped pixels in a scanline.
//	We rather might want to move this into TextureMapper.

	float avglat = ( lat-m_prevlat ) * m_ninv;
	float avglng = lng-m_prevlng;

	if (fabs(avglng) > M_PI){

		avglng = ( TWOPI - fabs(avglng) ) * m_ninv;

		if (m_prevlng < lng){
			for (int j=1; j < m_n; j++){
				m_prevlat += avglat;
				m_prevlng -= avglng;
				if (m_prevlng <= -M_PI) m_prevlng += TWOPI;
				getPixelValue( m_prevlng, m_prevlat, line );
				line++;
			}
		}
		// if (m_prevlng > lng)
		else { 
			float curAvgLng = lng - m_n*avglng;
			for (int j=1; j < m_n; j++){
				m_prevlat += avglat;
				curAvgLng += avglng;
				float evallng = curAvgLng;
				if (curAvgLng <= -M_PI) evallng += TWOPI;
				getPixelValue( evallng, m_prevlat, line);
				line++;
			}
		}
	}

	else {

		avglng *= m_ninv;
		for (int j=1; j < m_n; j++) {
			m_prevlat += avglat;
			m_prevlng += avglng;
			getPixelValue( m_prevlng, m_prevlat, line);
			line++;
		}
	}	
}

void TextureLoader::prePixelValueApprox(const float& radlng, const float& radlat, QRgb* line){
//	This method prepares the interpolation for skipped pixels in a scanline.
//	We rather might move this into TextureMapper.

	m_prevlat = radlat;
	m_prevlng = radlng;
	getPixelValue(radlng, radlat, line);
}

inline void TextureLoader::flush(){
//	Remove all m_tiles from m_tilehash
	QHash <int, TextureTile*>::const_iterator it;
	for( it = m_tilehash.begin(); it != m_tilehash.constEnd(); it++ ) 
		delete (*it);
	m_tilehash.clear();
}


inline void TextureLoader::loadTile(){
//	Choosing the correct m_tile via Lng/Lat info 
	m_tilxw = tilx * m_tilw;
	m_tilyh = tily * m_tilh;

	normhalfalpha = maxhalfalpha - m_tilxw;
	normquatbeta = maxquatbeta - m_tilyh;
	normfullalpha = maxfullalpha - m_tilxw;
	normhalfbeta = maxhalfbeta - m_tilyh;

	m_tilekey =  (tilx *1000) + tily;

	// If the m_tile hasn't been loaded into the m_tilehash yet, then do so
	if (!m_tilehash.contains( m_tilekey )){	
		m_tile = new TextureTile(tilx, tily, m_texlevel, m_theme);
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
}

void TextureLoader::setTexLevel(const int texlevel){

	m_texlevel=texlevel;
	if ( m_texlevel != m_oldtexlevel){
		flush();
		m_oldtexlevel=m_texlevel;

		texpixw = (int)(4320000.0f / (float)( TextureLoader::levelToColumn( m_texlevel ) ) / (float)(m_tilw));
		texpixh = (int)(2160000.0f / (float)( TextureLoader::levelToRow( m_texlevel ) ) / (float)(m_tilh));

		rad2pixw = (2160000.0f / M_PI / (float)(texpixw));
		rad2pixh = (2160000.0f / M_PI / (float)(texpixh));


		maxfullalpha = (int)(4320000.0f / (float)(texpixw)) - 1;
		maxhalfalpha = (float)(2160000.0f / (float)(texpixw));
		maxquatalpha = (int)(1080000.0f / (float)(texpixw));
		maxquatbeta = (float)(1080000.0f / (float)(texpixh));
		maxhalfbeta = (int) ( 2.0f * maxquatbeta) - 1;

		normhalfalpha = maxhalfalpha - m_tilxw;
		normquatbeta = maxquatbeta - m_tilyh;
		normfullalpha = maxfullalpha - m_tilxw;
		normhalfbeta = maxhalfbeta - m_tilyh;
	}
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

	for ( int m = 0; m < 2; ++m ){
		QString tilepath = KAtlasDirs::path( QString("maps/earth/%1/%2/%3/%3_%4.jpg").arg(theme).arg( 0 ).arg( 0, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') ) );

		noerr = QFile::exists( tilepath );

		if ( noerr == false ) break; 
	}

//	qDebug() << "Mandatory most basic tile level is fully available: " << noerr;

	return noerr;
}
