#include "texloader.h"
#include <QTime>
#include <QVector>
#include <cmath>

#include "katlasdirs.h"

#include <QDebug>

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

	width=rawtile->width();
	height=rawtile->height();
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
//	qDebug("Delete Tile");
}

TextureLoader::TextureLoader( const QString& fileprefix ){

	setMap( fileprefix );

}

void TextureLoader::setMap( const QString& fileprefix ){
	m_fileprefix = fileprefix;

	m_texlevel = 1;

	m_oldtexlevel = 0;

	m_oldlat=65535;
	
	tilx=65535;
	tily=65535;

	tile = new TileContainer( KAtlasDirs::path( QString("%1%2_0x0.jpg").arg(m_fileprefix).arg(m_texlevel) ) );
	tilw = tile->width;
	tilh = tile->height;
	delete tile;

	texpixw = (int)(21600.0f / (float)(m_texlevel) / (float)(tilw));
	texpixh = (int)(21600.0f / (float)(m_texlevel) / (float)(tilh));

	rad2pixw = (21600.0f / M_PI / (float)(texpixw));
	rad2pixh = (21600.0f / M_PI / (float)(texpixh));

	maxfullalpha = (int)(43200.0f / (float)(texpixw));
	maxhalfalpha = (float)(21600.0f / (float)(texpixw));
	maxquatalpha = (int)(10800.0f / (float)(texpixw));
	maxquatbeta = 10800.0f / (float)(texpixh);
	maxhalfbeta = 2.0f * maxquatbeta;
}

void TextureLoader::resetTilehash(){

	QHash<int, TileContainer*>::const_iterator it = tilehash.constBegin();
	while (it != tilehash.constEnd()) {
		tilehash[it.key()]->used=false;
		++it;
	}

	tilx = 65535;
	tily = 65535;
	tilxw = 65535;
	tilyh = 65535;
}

void TextureLoader::cleanupTilehash(){

	QHashIterator<int, TileContainer*> it(tilehash);
	while (it.hasNext()) {
		it.next();
		if ((it.value())->used == false){
//			qDebug("Removing " + QString::number(it.key()).toLatin1());
			delete tilehash[it.key()];
			tilehash.remove(it.key());	
		}
	}
}

void TextureLoader::setN(const int n){
	m_n = n; m_ninv = 1.0f/(float)(n);
}

inline void TextureLoader::getPixelValue(const float& radlng, const float& radlat, QRgb& pixelvalue){

// The origin is in the upper left corner
// lng: 360 = 43200
// lat: 180 = 21600

// Convert rad to pixel...
	int lng = (int)(maxhalfalpha + radlng * rad2pixw);
	int lat = (int)(maxquatbeta + radlat * rad2pixh);

	if ( lng >= maxfullalpha ) lng = (int)(radlng * rad2pixw); // necessary to prevent crash if radalpha = -pi
	if ( lat >= maxhalfbeta ) lat = (int)(radlat * rad2pixh + maxquatbeta); // necessary to prevent crash

// Calculate the position on the respective Tile

	bool newtile = false;

	posx = lng - tilxw; // the position on the tile measured from the left tile border 

	if ( posx >= tilw || posx < 0 ) {
		tilx = lng / tilw; // Counts the tiles left from the current tile ("tileposition") 
		tily = lat / tilh; // Counts the tiles on the top from the current tile
		loadTile();
		newtile = true;
		posx = lng - tilxw;
	}

// Scanline optimization for posy
	if (lat != m_oldlat){
		posy = lat - tilyh;

		if ( newtile == false && (posy >= tilh || posy < 0) ) {
//			tilx = lng / tilw;
			tily = lat / tilh;
			loadTile();
			posy = lat - tilyh;
		}

		m_oldlat = lat;
	}

	if (tile->depth == 8)
		pixelvalue = tile->jumpTable8[posy][posx];
	else
		pixelvalue = tile->jumpTable32[posy][posx];
}

// Interpolate skipped points
void TextureLoader::getPixelValueApprox(const float& lng, const float& lat, QRgb* line, const int& x){
	avglat = lat-m_prevlat;
	avglat *= m_ninv;
	avglng = lng - m_prevlng;
	float curAvgLat = m_prevlat;

	if (fabs(avglng) > M_PI){

		avglng = TWOPI - fabs(avglng);
		avglng *= m_ninv;

		if (m_prevlng > lng){
			float curAvgLng = m_prevlng;
			for (int j=1; j < m_n; j++){
				curAvgLat += avglat;
				curAvgLng += avglng;
				float evallng = curAvgLng;
				if (curAvgLng >= M_PI) evallng -= TWOPI;
				getPixelValue( -evallng, curAvgLat, line[x-m_n+j]);
			}
		}

		if (m_prevlng < lng){
			float curAvgLng = lng + m_n*avglng;
			for (int j=1; j < m_n; j++){
				curAvgLat += avglat;
				curAvgLng -= avglng;
				float evallng = curAvgLng;
				if (curAvgLng >= M_PI) evallng -= TWOPI;
				getPixelValue( -evallng, curAvgLat, line[x-m_n+j]);
			}
		}
	}

	else {

		float curAvgLng = m_prevlng;
		avglng *= m_ninv;
		for (int j=1; j < m_n; j++) {
			curAvgLat += avglat;
			curAvgLng += avglng;
			getPixelValue( -curAvgLng, curAvgLat, line[x-m_n+j]);
		}
	}	

}

void TextureLoader::prePixelValueApprox(const float& radlng, const float& radlat, QRgb* line, const int& x){
	m_prevlat = radlat;
	m_prevlng = radlng;
	getPixelValue(-radlng, radlat, line[x]);
}

inline void TextureLoader::flush(){
// Remove all tiles from tilehash
	foreach(TileContainer* tile, tilehash)
		delete tile;
	tilehash.clear();	
}


inline void TextureLoader::loadTile(){
// Choosing the correct tile via Lng/Lat into 
	tilxw = tilx * tilw;
	tilyh = tily * tilh;

	tilekey =  (tilx << 8) + tily;
//	tilekey =  (tilx *100) + tily;

	// If tile hasn't been loaded into the tilehash yet, then do so
	if (!tilehash.contains( tilekey )){	
		m_filename = KAtlasDirs::path( QString("%1%2_%3x%4.jpg").arg(m_fileprefix).arg(m_texlevel).arg(tilx).arg(tily) );
		tile = new TileContainer(m_filename);
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

		texpixw = (int)(21600.0f / (float)(m_texlevel) / (float)(tilw));
		texpixh = (int)(21600.0f / (float)(m_texlevel) / (float)(tilh));	

		rad2pixw = (21600.0f / M_PI / (float)(texpixw));
		rad2pixh = (21600.0f / M_PI / (float)(texpixh));

		maxfullalpha = (int)(43200.0f / (float)(texpixw));
		maxhalfalpha = (float)(21600.0f / (float)(texpixw));
		maxquatalpha = (int)(10800.0f / (float)(texpixw));
		maxquatbeta = (float)(10800.0f / (float)(texpixh));
		maxhalfbeta = 2.0f * maxquatbeta;
	}
}
