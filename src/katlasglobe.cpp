#include <QDebug>
#include <QSortFilterProxyModel>
#include <QTime>
#include <QTimer>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <cmath>
#include "katlasglobe.h"
#include "pntmap.h"
#include "texcolorizer.h"
#include "tilescissor.h"
#include "katlasdirs.h"
#include "katlastilecreatordialog.h"
#include "placemarkmanager.h"
#include "xmlhandler.h"

const float rad2int = 21600.0 / M_PI;


KAtlasGlobe::KAtlasGlobe( QWidget* parent ):m_parent(parent){

	m_justModified = false;

	m_maptheme = new MapTheme();

	QStringList m_mapthemedirs = MapTheme::findMapThemes( "maps" );
	QString selectedmap;

	if ( m_mapthemedirs.count() == 0 ) {
		qDebug() << "Couldn't find any maps! Exiting ...";
		exit(-1);
	}

	if ( m_mapthemedirs.count() >= 1 ){
		QStringList tmp = m_mapthemedirs.filter("etopo2.dgml");
		if ( tmp.count() >= 1 )
			selectedmap = tmp[0];
		else
			selectedmap = m_mapthemedirs[0];
	}

	m_maptheme->open( KAtlasDirs::path( "maps/" + selectedmap) );
	if ( m_maptheme->maxTileLevel() < 1 ){
		qDebug("Base tiles not available. Creating Tiles ... ");

		KAtlasTileCreatorDialog tilecreatordlg( m_parent );
		tilecreatordlg.setSummary( m_maptheme->name(), m_maptheme->description() );

		TileScissor tilecreator( m_maptheme->prefix(), m_maptheme->installMap(), m_maptheme->bitmaplayer().dem);
		QObject::connect( &tilecreator, SIGNAL( progress( int ) ), &tilecreatordlg, SLOT( setProgress( int ) ) );

		QTimer::singleShot( 0, &tilecreator, SLOT( createTiles() ) );

		tilecreatordlg.exec();
	}
	m_maptheme->detectMaxTileLevel();

	texmapper = new TextureMapper( "maps/" + m_maptheme->tilePrefix() + "_" );
	texmapper->setMaxTileLevel( m_maptheme->maxTileLevel() );

	m_placemarkpainter->setLabelColor( m_maptheme->labelColor() );
//	m_placecontainer ->clearTextPixmaps();

	veccomposer = new VectorComposer();
	texcolorizer = new TextureColorizer(KAtlasDirs::path("seacolors.leg"), KAtlasDirs::path("landcolors.leg"));

	placemarkmanager = new PlaceMarkManager();
	m_placemarkmodel = new PlaceMarkModel();

	QSortFilterProxyModel* sortmodel = new QSortFilterProxyModel( this );
	sortmodel->setSourceModel( m_placemarkmodel );

//	m_placemarkmodel -> sort( 0, Qt::AscendingOrder );
	sortmodel -> sort( 0, Qt::AscendingOrder );
	m_placemarkmodel = ( QAbstractItemModel* )sortmodel;

	m_placecontainer = new PlaceContainer("placecontainer");

	m_placemarkpainter = new PlaceMarkPainter();
	m_radius = 2000;
	m_rotAxis = Quaternion(1.0, 0.0, 0.0, 0.0);

	m_coastimg = new QImage(10,10,QImage::Format_ARGB32_Premultiplied);

	m_centered = false;
	m_centeredItem = 0;
}

void KAtlasGlobe::setMapTheme( const QString& selectedmap ){

	m_maptheme->open( KAtlasDirs::path( selectedmap) );
	if ( m_maptheme->maxTileLevel() < 1 ){
		qDebug("Base tiles not available. Creating Tiles ... ");

		KAtlasTileCreatorDialog tilecreatordlg( m_parent );
		tilecreatordlg.setSummary( m_maptheme->name(), m_maptheme->description() );

		TileScissor tilecreator( m_maptheme->prefix(), m_maptheme->installMap(), m_maptheme->bitmaplayer().dem);
		QObject::connect( &tilecreator, SIGNAL( progress( int ) ), &tilecreatordlg, SLOT( setProgress( int ) ) );

		QTimer::singleShot( 0, &tilecreator, SLOT( createTiles() ) );

		tilecreatordlg.exec();
	}
	m_maptheme->detectMaxTileLevel();

	m_placemarkpainter->setLabelColor( m_maptheme->labelColor() );
	m_placecontainer ->clearTextPixmaps();

	texmapper->setMap( "maps/" + m_maptheme->tilePrefix() + "_" );
	texmapper->setMaxTileLevel( m_maptheme->maxTileLevel() );
	m_parent->update();

	m_justModified = true;
}

void KAtlasGlobe::resize(){
	*m_coastimg = QImage(m_canvasimg->width(),m_canvasimg->height(),QImage::Format_ARGB32_Premultiplied);
	
	texmapper->resizeMap(m_canvasimg);
	veccomposer->resizeMap(m_coastimg);

	m_justModified = true;
}

void KAtlasGlobe::paintGlobe(ClipPainter* painter, QRect dirty){
//	QTime *timer = new QTime();
//	timer->restart();
	if ( needsUpdate() || m_canvasimg->isNull() || m_justModified == true ){

// Workaround
//	m_rotAxis.display();
		texmapper->mapTexture(m_canvasimg, m_radius, m_rotAxis);

//	qDebug() << "Texture-Mapping:" << timer->elapsed();
//	timer->restart();

		if ( m_maptheme->bitmaplayer().dem == "true" ){
			*m_coastimg = QImage(m_canvasimg->width(),m_canvasimg->height(),QImage::Format_ARGB32_Premultiplied);

//		qDebug() << "Scale & Fill: " << timer->elapsed();
//		timer->restart();

			veccomposer->drawTextureMap(m_coastimg, m_radius, m_rotAxis); // Create VectorMap

//		qDebug() << "Vectors: " << timer->elapsed();
//		timer->restart();

			texcolorizer->colorize(m_canvasimg, m_coastimg, m_radius); // Recolorize the heightmap using the VectorMap

//		qDebug() << "Colorizing: " << timer->elapsed();
//		timer->restart();
		}
	}

	painter->drawImage(dirty, *m_canvasimg, dirty); // paint Map on Widget

	if ( m_maptheme->vectorlayer().enabled == true ){
		veccomposer->paintVectorMap(painter, m_radius, m_rotAxis); // Add further Vectors
//		qDebug() << "2. Vectors: " << timer->elapsed();
//		timer->restart();
	}
	

	if (m_centered == true) m_placemarkpainter->paintPlaceMark(painter, m_canvasimg->width()/2, m_canvasimg->height()/2, m_placemarkmodel, m_centeredItem);

	m_placemarkpainter->paintPlaceFolder(painter, m_canvasimg->width()/2, m_canvasimg->height()/2, m_radius, m_placecontainer, m_rotAxis);
//	qDebug() << "Placemarks: " << timer->elapsed();
//	timer->restart();
	m_rotAxisUpdated = m_rotAxis;
	m_radiusUpdated = m_radius;
	m_justModified = false;
}

void KAtlasGlobe::setCanvasImage(QImage* canvasimg){
	m_canvasimg = canvasimg;
}

void KAtlasGlobe::zoom(const int& radius){
	*m_canvasimg = QImage(m_canvasimg->width(),m_canvasimg->height(),QImage::Format_ARGB32_Premultiplied);;
	m_radius = radius;
}

void KAtlasGlobe::rotateTo(const uint& phi, const uint& theta, const uint& psi){
	m_rotAxis.createFromEuler((float)(phi)/rad2int,(float)(theta)/rad2int,(float)(psi)/rad2int);
//	m_rotAxis.display();
}

void KAtlasGlobe::rotateTo(const float& phi, const float& theta){
//	m_rotAxis.createFromEuler( M_PI-(phi + 180.0) * M_PI / 180.0, M_PI+(theta + 180.0) * M_PI / 180.0, 0.0);
	m_rotAxis.createFromEuler( (phi + 180.0) * M_PI / 180.0, (theta + 180.0) * M_PI / 180.0, 0.0);
//	m_rotAxis.display();
	m_centered = false;
}



void KAtlasGlobe::rotateBy(const Quaternion& incRot){
//	m_rotAxis.createFromEuler((float)(phi)/rad2int,(float)(theta)/rad2int,(float)(psi)/rad2int);
	m_rotAxis = incRot * m_rotAxis;
//	m_rotAxis.display();
	m_centered = false;
}

void KAtlasGlobe::rotateBy(const float& phi, const float& theta){
	Quaternion rotPhi(1.0, phi, 0.0, 0.0);
	Quaternion rotTheta(1.0, 0.0, theta, 0.0);
	m_rotAxis = rotTheta * m_rotAxis;
	m_rotAxis *= rotPhi;
	m_rotAxis.normalize();

	m_centered = false;
}

int KAtlasGlobe::northPoleY(){
	GeoPoint northpole( 0.0f, (float)( -M_PI*0.5 ) );
	Quaternion qpolepos = northpole.getQuatPoint();
	Quaternion invRotAxis = m_rotAxis.inverse();
//	invRotAxis.display();

	qpolepos.rotateAroundAxis(invRotAxis);

	return (int)( m_radius * qpolepos.v[Q_Y] );
}

int KAtlasGlobe::northPoleZ(){
	GeoPoint northpole( 0.0f, (float)( -M_PI*0.5 ) );
	Quaternion qpolepos = northpole.getQuatPoint();
	Quaternion invRotAxis = m_rotAxis.inverse();
//	invRotAxis.display();

	qpolepos.rotateAroundAxis(invRotAxis);

	return (int)( m_radius * qpolepos.v[Q_Z] );
}

bool KAtlasGlobe::screenCoordinates( const float lng, const float lat, int& x, int& y ){
	
	Quaternion qpos = GeoPoint( lng, lat ).getQuatPoint();
	Quaternion invRotAxis = m_rotAxis.inverse();

	qpos.rotateAroundAxis(invRotAxis);

	x = (int)(  m_radius * qpos.v[Q_X] );
	y = (int)( -m_radius * qpos.v[Q_Y] );

	if ( qpos.v[Q_Z] >= 0.0 )
		return true;
	else 
		return false;
}

void KAtlasGlobe::addPlaceMarkFile( QString filename ){ 

	KAtlasXmlHandler handler( m_placecontainer );

	QFile file( filename );

	QXmlInputSource source( &file );

	QXmlSimpleReader reader;
	reader.setContentHandler( &handler );

	reader.parse( source );

	qDebug() << "Number of placemarks: " << m_placecontainer->size();
}
