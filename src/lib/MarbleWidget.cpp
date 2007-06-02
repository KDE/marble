//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "MarbleWidget.h"

#include <cmath>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QTime>
#include <QtGui/QSizePolicy>
#include <QtGui/QRegion>

#include "Quaternion.h"
#include "texcolorizer.h"
#include "clippainter.h"
#include "katlasviewinputhandler.h"
#include "katlasviewpopupmenu.h"
#include "katlastilecreatordialog.h"

#include "measuretool.h"


#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   static long double sqrt(int a) { return sqrt((long double)a); }
# endif
#endif


MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget(parent)
{
    m_model = new MarbleModel( this );
    construct( parent );
}


MarbleWidget::MarbleWidget(MarbleModel *model, QWidget *parent)
    : QWidget(parent)
{
    m_model = model;
    construct( parent );
}

void MarbleWidget::construct(QWidget *parent)
{
    setMinimumSize( 200, 300 );
    setFocusPolicy( Qt::WheelFocus );
    setFocus( Qt::OtherFocusReason );

    connect( m_model, SIGNAL( creatingTilesStart( const QString&, const QString& ) ),
             this,    SLOT( creatingTilesStart( const QString&, const QString& ) ) );
    connect( m_model, SIGNAL( creatingTilesProgress( int ) ),
             this,    SLOT( creatingTilesProgress( int ) ) );

    connect( m_model, SIGNAL(themeChanged()), this, SLOT(update()) );

    // Set background: black.
    QPalette p = palette();
    p.setColor( QPalette::Window, Qt::black );
    setPalette( p );
    setBackgroundRole( QPalette::Window );
    setAutoFillBackground( true );

    //	setAttribute(Qt::WA_NoSystemBackground);

    m_pCanvasImage = new QImage( parent->width(), parent->height(),
				 QImage::Format_ARGB32_Premultiplied );
    m_model->setCanvasImage( m_pCanvasImage );

    m_inputhandler = new KAtlasViewInputHandler( this, m_model );
    installEventFilter( m_inputhandler );
    setMouseTracking( true );

    m_popupmenu = new KAtlasViewPopupMenu( this, m_model );
    connect( m_inputhandler, SIGNAL( lmbRequest( int, int ) ),
	     m_popupmenu,    SLOT( showLmbMenu( int, int ) ) );	
    connect( m_inputhandler, SIGNAL( rmbRequest( int, int ) ),
	     m_popupmenu,    SLOT( showRmbMenu( int, int ) ) );	

    m_pMeasureTool = new MeasureTool( this );

    connect( m_popupmenu,    SIGNAL( addMeasurePoint( float, float ) ),
	     m_pMeasureTool, SLOT( addMeasurePoint( float, float ) ) );	
    connect( m_popupmenu,    SIGNAL( removeMeasurePoints() ),
	     m_pMeasureTool, SLOT( removeMeasurePoints( ) ) );	

    m_logzoom  = 0;
    m_zoomStep = 40;
    goHome();

    m_minimumzoom = 950;
    m_maximumzoom = 2200;
}


void MarbleWidget::zoomView(int zoom)
{
    // Prevent infinite loops.
    if ( zoom  == m_logzoom )
	return;

    m_logzoom = zoom;

    emit zoomChanged(zoom);

    int radius = fromLogScale(zoom);

    if ( radius == m_model->radius() )
	return;
	
    m_model->setRadius(radius);
    repaint();

    setActiveRegion();
}


void MarbleWidget::zoomViewBy(int zoomstep)
{
    // Prevent infinite loops

    int zoom = m_model->radius();
    int tryZoom = toLogScale(zoom) + zoomstep;
    //	qDebug() << QString::number(tryZoom) << " " << QString::number(minimumzoom);
    if ( tryZoom >= m_minimumzoom && tryZoom <= m_maximumzoom ) {
	zoom = tryZoom;
	zoomView(zoom);
    }
}


void MarbleWidget::zoomIn()
{
    zoomViewBy( m_zoomStep );
}

void MarbleWidget::zoomOut()
{
    zoomViewBy( -m_zoomStep );
}

void MarbleWidget::rotateBy(const float& phi, const float& theta)
{
    m_model->rotateBy( phi, theta );

    repaint();
}

void MarbleWidget::centerOn(const float& phi, const float& theta)
{
    m_model->rotateTo( phi, theta );

    repaint();
}

void MarbleWidget::centerOn(const QModelIndex& index)
{

    PlaceMarkModel* model = (PlaceMarkModel*) m_model->getPlaceMarkModel();
    if (model == 0) qDebug( "model null" );

    PlaceMark* mark = model->placeMark( index );

    m_model->placeContainer()->clearSelected();

    if ( mark != 0 ){
	float  lon;
        float  lat;

	mark->coordinate( lon, lat );
	centerOn( -lat * 180.0 / M_PI, -lon * 180.0 / M_PI );
	mark->setSelected( 1 );
	m_crosshair.setEnabled( true );
    }
    else 
	m_crosshair.setEnabled( false );

    m_model->placeContainer()->clearTextPixmaps();
    m_model->placeContainer()->sort();

    repaint();
}


void MarbleWidget::moveLeft()
{
    rotateBy( 0, moveStep() );
}

void MarbleWidget::moveRight()
{
    rotateBy( 0, -moveStep() );
}


void MarbleWidget::moveUp()
{
    rotateBy( moveStep(), 0 );
}

void MarbleWidget::moveDown()
{
    rotateBy( -moveStep(), 0 );
}

void MarbleWidget::resizeEvent (QResizeEvent*)
{
    //	Redefine the area where the mousepointer becomes a navigationarrow
    setActiveRegion();
    delete m_pCanvasImage;

    m_pCanvasImage = new QImage( width(), height(),
				 QImage::Format_ARGB32_Premultiplied );
    m_model->setCanvasImage( m_pCanvasImage );
    m_model->resize();

    repaint();
}


bool MarbleWidget::globeSphericals(int x, int y, float& alpha, float& beta)
{

    int radius = m_model->radius(); 
    int imgrx  = width() >> 1;
    int imgry  = height() >> 1;

    const float  radiusf = 1.0 / (float)(radius);

    if ( radius > sqrt((x - imgrx)*(x - imgrx) + (y - imgry)*(y - imgry)) ) {

	float qy = radiusf * (float)(y - imgry);
	float qr = 1.0 - qy * qy;
	float qx = (float)(x - imgrx) * radiusf;

	float qr2z = qr - qx * qx;
	float qz = (qr2z > 0.0) ? sqrt( qr2z ) : 0.0;	

	Quaternion  qpos( 0, qx, qy, qz );
	qpos.rotateAroundAxis( m_model->getPlanetAxis() );
	qpos.getSpherical( alpha, beta );

	return true;
    }
    else {
	return false;
    }
}

void MarbleWidget::setActiveRegion()
{
    int zoom = m_model->radius(); 

    m_activeRegion = QRegion( 25, 25, width() - 50, height() - 50, 
                              QRegion::Rectangle );

    if ( zoom < sqrt( width() * width() + height() * height() ) / 2 ) {
	m_activeRegion &= QRegion( width() / 2 - zoom, height() / 2 - zoom, 
                                   2 * zoom, 2 * zoom, QRegion::Ellipse );
    }
}

const QRegion MarbleWidget::activeRegion()
{
    return m_activeRegion;
}


void MarbleWidget::paintEvent(QPaintEvent *evt)
{
    //	Debugging Active Region
    //	painter.setClipRegion(activeRegion);

    //	if(m_model->needsUpdate() || m_pCanvasImage->isNull() || m_pCanvasImage->size() != size())
    //	{

    int   radius = m_model->radius();
    bool  clip = ( radius > m_pCanvasImage->width()/2
                   || radius > m_pCanvasImage->height()/2 ) ? true : false;

    // Paint the globe itself.
    ClipPainter painter( this, clip); 
    // QPainter painter(this);
    // painter.setClipRect(10, 10, m_pCanvasImage->width() - 1 , m_pCanvasImage->height()-1 );
    // painter.setClipping( true );
    // painter.clearNodeCount();

    QRect  dirty = evt->rect();
    m_model->paintGlobe(&painter,dirty);
	
    // Draw the scale.
    painter.drawPixmap( 10, m_pCanvasImage->height() - 40,
                        m_mapscale.drawScaleBarPixmap( m_model->radius(),
                                                       m_pCanvasImage-> width() / 2 - 20 ) );

    // Draw the wind rose.
    painter.drawPixmap( m_pCanvasImage->width() - 60, 10,
			m_windrose.drawWindRosePixmap( m_pCanvasImage->width(),
						       m_pCanvasImage->height(),
                                                       m_model->northPoleY() ) );

    // Draw the crosshair.
    m_crosshair.paintCrossHair( &painter, 
				m_pCanvasImage->width(),
                                m_pCanvasImage->height() );

    m_pMeasureTool->paintMeasurePoints( &painter, m_pCanvasImage->width() / 2,
					m_pCanvasImage->height() / 2,
					radius, m_model->getPlanetAxis(),
                                        true );
#if 0
      else
      {
      // Draw cached pixmap to widget
      QPainter pixmapPainter(this);
      QRect rect(0, 0, width(), height());
      pixmapPainter.drawImage(rect, m_pCanvasImage, rect);
      }
#endif
}


void MarbleWidget::goHome()
{
    // m_model->rotateTo(0, 0);
    m_model->rotateTo( 54.8, -9.4 );
    zoomView( 1050 ); // default 1050

    update(); // not obsolete in case the zoomlevel stays unaltered
}


// This slot will called when the Globe starts to create the tiles.

void MarbleWidget::creatingTilesStart( const QString &name, const QString &description )
{
    qDebug("MarbleWidget::creatingTilesStart called... ");

    m_tileCreatorDlg = new KAtlasTileCreatorDialog( this );

    m_tileCreatorDlg->setSummary( name, description );

    // The process itself is started by a timer, so an exec() is ok here.
    m_tileCreatorDlg->exec();
    qDebug("MarbleWidget::creatingTilesStart exits... ");
}

// This slot will be called during the tile creation progress.  When
// the progress goes to 100, the dialog should be closed.

void MarbleWidget::creatingTilesProgress( int progress )
{
    m_tileCreatorDlg->setProgress( progress );

    if ( progress == 100 )
        delete m_tileCreatorDlg;
}


float MarbleWidget::moveStep()
{
    if ( m_model->radius() < sqrt( width() * width() + height() * height() ) )
	return 0.1f;
    else
	return atanf( (float)width() 
                      / (float)( 2 * m_model->radius() ) ) * 0.2f;
}

int MarbleWidget::fromLogScale(int zoom)
{
    zoom = (int) pow(M_E, ((float)zoom / 200.));
    // zoom = (int) pow(2, ((float)zoom/200));
    return zoom;
}

int MarbleWidget::toLogScale(int zoom)
{
    zoom = (int)(200.0f * logf( (float)zoom ) );
    return zoom;
}

#ifndef Q_OS_MACX
#include "MarbleWidget.moc"
#endif
