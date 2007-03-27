#include "texloader.h"
#include <QtCore/QTime>
#include <QtCore/QVector>
#include <cmath>

#include "katlasdirs.h"

#include <QtCore/QDebug>

const float TWOPI = 2 * M_PI;

static uint **jumpTableFromQImage32( QImage &img )
{
	const int height = img.height();
	const int bpl = img.bytesPerLine()/4;
	uint *data = (QRgb*)(img.scanLine(0));
	uint **jumpTable = new uint*[height];
	for ( int y = 0; y < height; ++y ) {
		jumpTable[ y ] = data;
		data += bpl;
	}
	return jumpTable;
}

static uchar **jumpTableFromQImage8( QImage &img )
{
	const int height = img.height();
	const int bpl = img.bytesPerLine();
	uchar *data = img.bits();
	uchar **jumpTable = new uchar*[height];
	for ( int y = 0; y < height; ++y ) {
		jumpTable[ y ] = data;
		data += bpl;
	}
	return jumpTable;
}


TileContainer::TileContainer(const QString& filename){

	used=true;
	rawtile=new QImage(filename);
	if ( rawtile->isNull() ){
		qDebug() << QString( "Bitmap tile " + filename + " could not be found. Please run texissor." ); 
		exit(-1);
	}
	depth = rawtile->depth();

	switch ( depth ) {
	case 32:
//		qDebug("32");
		jumpTable32=jumpTableFromQImage32(*rawtile);
		break;
	case 8:
//		qDebug("8");
		jumpTable8=jumpTableFromQImage8(*rawtile);
		break;
	default:
		qDebug() << QString("Color depth %1 of a tile could not be retrieved. Exiting.").arg(depth);
		exit(-1);
	}
}

TileContainer::~TileContainer(){
	switch ( depth ) {
	case 32:
		delete [] jumpTable32;
		break;
	case 8:
		delete [] jumpTable8;
		break;
	default:
		qDebug("Color depth of a tile could not be retrieved. Exiting.");
		exit(-1);
	}
	delete rawtile;
}

TextureLoader::TextureLoader( const QString& fileprefix ){

	setMap( fileprefix );
	m_depth = -1;
}

void TextureLoader::setMap( const QString& fileprefix ){
//	Initialize map theme.
	m_fileprefix = fileprefix;

	m_texlevel = 1;

	m_oldtexlevel = 0;
	
	tilx=65535;
	tily=65535;

	tile = new TileContainer( KAtlasDirs::path( QString("%1%2_0x0.jpg").arg(m_fileprefix).arg(m_texlevel) ) );

//	We assume that all tiles have the same size. TODO: check to be safe
	m_tilw = tile->rawtile->width();
	m_tilh = tile->rawtile->height();
	delete tile;

	setTexLevel(1);
}

void TextureLoader::resetTilehash(){

	QHash<int, TileContainer*>::const_iterator it = tilehash.constBegin();
	while (it != tilehash.constEnd()) {
		tilehash.value(it.key())->used=false;
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

	QHashIterator<int, TileContainer*> it(tilehash);
	while (it.hasNext()) {
		it.next();
		if ((it.value())->used == false){
//			qDebug("Removing " + QString::number(it.key()).toLatin1());
			delete tilehash.value(it.key());
			tilehash.remove(it.key());	
		}
	}
}

void TextureLoader::setN(const int n){
//	Define the number of points to be skipped for interpolation 
	m_n = n; m_ninv = 1.0f/(float)(n);
}

inline void TextureLoader::getPixelValue(const float& radlng, const float& radlat, QRgb* line){

//	The origin (0, 0) is in the upper left corner
//	lng: 360 deg = 43200 pixel
//	lat: 180 deg = 21600 pixel

//	Convert rad to pixel...
	posx = (int)(normhalfalpha + radlng * rad2pixw);
	posy = (int)(normquatbeta + radlat * rad2pixh);

	// necessary to prevent e.g. crash if TextureMapper::radalpha = -pi
	if ( posx > normfullalpha ) posx = normfullalpha;
	if ( posy > normhalfbeta ) posy = normhalfbeta;

//	qDebug() << "posx:" << posx << "posy:" << posy;
	if ( posx >= m_tilw || posx < 0 ) {
		int lng = posx + m_tilxw;
		int lat = posy + m_tilyh;
		tilx = lng / m_tilw; // Counts the tiles left from the current tile ("tileposition") 
		tily = lat / m_tilh; // Counts the tiles on the top from the current tile
//		qDebug() << "tilx:" << tilx << "tily:" << tily;
		loadTile();
		posx = lng - m_tilxw;
		posy = lat - m_tilyh;
	}
	else	if ( posy >= m_tilh || posy < 0 ) {
		int lat = posy + m_tilyh;
		tily = lat / m_tilh;
		loadTile();
		posy = lat - m_tilyh;
	}

	switch ( depth() ) {
	case 8:
		*line = tile->jumpTable8[posy][posx];
		break;
	case 32:
		*line = tile->jumpTable32[posy][posx];
		break;
	default:
		if (tile->depth == 8)
			*line = tile->jumpTable8[posy][posx];
		else
			*line = tile->jumpTable32[posy][posx];
		break;
	}
}

void TextureLoader::getPixelValueApprox(const float& lng, const float& lat, QRgb* line){
//	This method executes the interpolation for skipped pixels in a scanline.
//	We rather might move this into TextureMapper.

	avglat = lat-m_prevlat;
	avglat *= m_ninv;
	avglng = lng-m_prevlng;

	if (fabs(avglng) > M_PI){

		avglng = TWOPI - fabs(avglng);
		avglng *= m_ninv;

		if (m_prevlng < lng){
			for (int j=1; j < m_n; j++){
				m_prevlat += avglat;
				m_prevlng -= avglng;
				if (m_prevlng <= -M_PI) m_prevlng += TWOPI;
				getPixelValue( m_prevlng, m_prevlat, line);
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
//	Remove all tiles from tilehash
	QHash <int, TileContainer*>::const_iterator it;
	for( it = tilehash.begin(); it != tilehash.constEnd(); it++ ) 
		delete (*it);
	tilehash.clear();
}


inline void TextureLoader::loadTile(){
//	Choosing the correct tile via Lng/Lat info 
	m_tilxw = tilx * m_tilw;
	m_tilyh = tily * m_tilh;

	normhalfalpha = maxhalfalpha - m_tilxw;
	normquatbeta = maxquatbeta - m_tilyh;
	normfullalpha = maxfullalpha - m_tilxw;
	normhalfbeta = maxhalfbeta - m_tilyh;

	tilekey =  (tilx << 8) + tily;
//	tilekey =  (tilx *100) + tily;

	// If the tile hasn't been loaded into the tilehash yet, then do so
	if (!tilehash.contains( tilekey )){	
		m_filename = KAtlasDirs::path( QString("%1%2_%3x%4.jpg").arg(m_fileprefix).arg(m_texlevel).arg(tilx).arg(tily) );
		tile = new TileContainer(m_filename);
		if ( m_depth == -1 ) m_depth = tile->depth;
		if ( m_depth != tile->depth ) m_depth = 0;
		tilehash[tilekey]=tile;
	}
	// otherwise pick the correct one from the hash
	else {
		tile=tilehash.value(tilekey);
		if (!tile->used){
			tile->used=true;
			tilehash[tilekey]=tile;
		}
	}
}

void TextureLoader::setTexLevel(const int texlevel){
	m_texlevel=texlevel;
	if ( m_texlevel != m_oldtexlevel){
		flush();
		m_oldtexlevel=m_texlevel;

		texpixw = (int)(21600.0f / (float)(m_texlevel) / (float)(m_tilw));
		texpixh = (int)(21600.0f / (float)(m_texlevel) / (float)(m_tilh));	

		rad2pixw = (21600.0f / M_PI / (float)(texpixw));
		rad2pixh = (21600.0f / M_PI / (float)(texpixh));


		maxfullalpha = (int)(43200.0f / (float)(texpixw)) - 1;
		maxhalfalpha = (float)(21600.0f / (float)(texpixw));
		maxquatalpha = (int)(10800.0f / (float)(texpixw));
		maxquatbeta = (float)(10800.0f / (float)(texpixh));
		maxhalfbeta = (int) ( 2.0f * maxquatbeta) - 1;

		normhalfalpha = maxhalfalpha - m_tilxw;
		normquatbeta = maxquatbeta - m_tilyh;
		normfullalpha = maxfullalpha - m_tilxw;
		normhalfbeta = maxhalfbeta - m_tilyh;
	}
}
