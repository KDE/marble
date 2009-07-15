//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmAnnotatePlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QtGui/QPushButton>
#include <QtGui/QPainterPath>
#include <QtGui/QFileDialog>

//#include <Phonon/MediaObject>
//#include <Phonon/VideoWidget>

#include <QtCore/QDebug>
#include <QtGui/QAction>
#include "ViewportParams.h"
#include "AbstractProjection.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataDocument.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "TextAnnotation.h"
#include "GeoDataParser.h"
#include "AreaAnnotation.h"
#include "MarbleWidget.h"

namespace Marble
{

QStringList OsmAnnotatePlugin::backendTypes() const
{
    return QStringList( "osmannotation" );
}

QString OsmAnnotatePlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList OsmAnnotatePlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString OsmAnnotatePlugin::name() const
{
    return tr( "OSM Annotation Plugin" );
}

QString OsmAnnotatePlugin::guiString() const
{
    return tr( "&OSM Annotation Plugin" );
}

QString OsmAnnotatePlugin::nameId() const
{
    return QString( "osm-annotation-plugin" );
}

QString OsmAnnotatePlugin::description() const
{
    return tr( "This is a render and interaciton plugin used for annotating OSM data." );
}

QIcon OsmAnnotatePlugin::icon () const
{
    return QIcon();
}


void OsmAnnotatePlugin::initialize ()
{

    //Setup the model
    GeoDataCoordinates madrid( -13.7, 40.4, 0.0, GeoDataCoordinates::Degree );
    TextAnnotation* annon = new TextAnnotation();

    annon->setCoordinate(madrid);
    
    //FIXME memory leak withouth a model to do memory management
    model.append(annon);

    widgetInitalised= false;
    m_tmp_lineString = 0;
    m_itemModel = 0;
}

bool OsmAnnotatePlugin::isInitialized () const
{
    return true;
}

bool OsmAnnotatePlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    if( !widgetInitalised ) {
        MarbleWidget* marbleWidget = (MarbleWidget*) painter->device();
        QList<QActionGroup*> actionGroups = setupActions( marbleWidget );

        QListIterator<QActionGroup*> it(actionGroups);

        while( it.hasNext() ) {
            marbleWidget->registerActions( it.next() );
        }

        connect(this, SIGNAL(redraw()),
                marbleWidget, SLOT(repaint()) );

        widgetInitalised = true;
    }
    painter->autoMapQuality();

    //so the user can keep track of the current polygon drawing
    if( m_tmp_lineString ) {
        painter->drawPolyline( *m_tmp_lineString );
    }
    
    QListIterator<TmpGraphicsItem*> i(model);
    
    while(i.hasNext()) {
        TmpGraphicsItem* tmp= i.next();
        tmp->paint(painter, viewport, renderPos, layer);
    }

    if( m_itemModel ) {
        QListIterator<GeoGraphicsItem*> it( *m_itemModel );

        while( it.hasNext() ) {
            GeoGraphicsItem* i = it.next();
            i->paint( painter, viewport, renderPos, layer );
        }
    }

    return true;
}

void OsmAnnotatePlugin::setAddingPlacemark( bool b)
{
    m_addingPlacemark = b;
}

void OsmAnnotatePlugin::setDrawingPolygon(bool b)
{
    m_drawingPolygon = b;
    if( !b ) {
        //stopped drawing the polygon
        if ( m_tmp_lineString != 0 ) {
            AreaAnnotation* area = new AreaAnnotation();
            GeoDataPolygon poly( Tessellate );
            poly.setOuterBoundary( GeoDataLinearRing(*m_tmp_lineString) );
            delete m_tmp_lineString;
            m_tmp_lineString = 0;

            area->setGeometry( poly );

            model.append(area);

            //FIXME only redraw the new polygon
            emit(redraw());
        }
    }
}

void OsmAnnotatePlugin::loadOsmFile()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(0, tr("Open File"),
                            QString(),
                            tr("All Supported Files (*.osm);;Open Street Map Data (*.osm)"));

    if ( ! filename.isNull() ) {

        GeoDataParser parser( GeoData_OSM );

        QFile file( filename );
        if ( !file.exists() ) {
            qWarning( "File does not exist!" );
            return;
        }

        // Open file in right mode
        file.open( QIODevice::ReadOnly );

        if ( !parser.read( &file ) ) {
            qWarning( "Could not parse file!" );
            //do not quit on a failed read!
            //return
        }
        QList<GeoGraphicsItem*>* model = parser.releaseModel();
        Q_ASSERT( model );

        m_itemModel = model;

        file.close();

        qDebug() << "size of container is " << model->size();
    }
}

bool    OsmAnnotatePlugin::eventFilter(QObject* watched, QEvent* event)
{
    MarbleWidget* marbleWidget = (MarbleWidget*) watched;
    //FIXME why is the QEvent::MousePress not working? caught somewhere else?
    //does this mean we need to centralise the event handling?

    // Catch the mouse button press
    if ( event->type() == QEvent::MouseButtonPress ) {
        QMouseEvent* mouseEvent = (QMouseEvent*) event;

        // deal with adding a placemark
        if ( mouseEvent->button() == Qt::LeftButton
             && m_addingPlacemark )
        {
            //Add a placemark on the screen
            qreal lon, lat;

            bool valid = ((MarbleWidget*)watched)->geoCoordinates(((QMouseEvent*)event)->pos().x(),
                                                                  ((QMouseEvent*)event)->pos().y(),
                                                                  lon, lat, GeoDataCoordinates::Radian);
            if ( valid ) {
                GeoDataCoordinates point( lon, lat );
                TextAnnotation* t = new TextAnnotation();
                t->setCoordinate(point);
                model.append(t);

                //FIXME only repaint the new placemark
                ( ( MarbleWidget* ) watched)->repaint();
                //FIXME: enable a way to disable adding a placemark
                //using signals and slots
//                m_addPlacemark->setChecked( false );
                return true;
            }


        }

        // deal with drawing a polygon
        if ( mouseEvent->button() == Qt::LeftButton
             && m_drawingPolygon )
        {
            qreal lon, lat;

            bool valid = ((MarbleWidget*)watched)->geoCoordinates( mouseEvent->pos().x(),
                                                                   mouseEvent->pos().y(),
                                                                   lon, lat, GeoDataCoordinates::Radian);
            if ( valid ) {
                if ( m_tmp_lineString == 0 ) {
                    m_tmp_lineString = new GeoDataLineString( Tessellate );
                }

                m_tmp_lineString->append(GeoDataCoordinates(lon, lat));

                //FIXME only repaint the line string so far
                marbleWidget->repaint();

            }
            return true;
        }

        //deal with clicking
        if ( mouseEvent->button() == Qt::LeftButton ) {
            qreal lon, lat;

        bool valid = ((MarbleWidget*)watched)->geoCoordinates(((QMouseEvent*)event)->pos().x(),
                                                              ((QMouseEvent*)event)->pos().y(),
                                                              lon, lat, GeoDataCoordinates::Radian);
        //if the event is in an item change cursor
        // FIXME make this more effecient by using the bsptree
        QListIterator<TmpGraphicsItem*> i(model);
        while(i.hasNext()) {
            TmpGraphicsItem* item = i.next();
            if(valid) {
                //FIXME check against all regions!
                QListIterator<QRegion> it ( item->regions() );

                while ( it.hasNext() ) {
                    QRegion p = it.next();
                    if( p.contains( mouseEvent->pos() ) ) {
                        return item->sceneEvent( event );
                    }
                }
            }

        }

        }
    }

//    // this stuff below is for hit tests. Just a sample mouse over for all bounding boxes
//    if ( event->type() == QEvent::MouseMove ||
//         event->type() == QEvent::MouseButtonPress ||
//         event->type() == QEvent::MouseButtonRelease ) {
//        QMouseEvent* mouseEvent = (QMouseEvent*) event;
//
//
//
//
//    }
    return false;
}

QList<QActionGroup*> OsmAnnotatePlugin::setupActions(MarbleWidget* widget)
{
    QList<QActionGroup*> result;

    QActionGroup* initial = new QActionGroup(0);
    initial->setExclusive( false );

    QActionGroup* group = new QActionGroup(0);
    group->setExclusive( true );

    QAction*    m_addPlacemark;
    QAction*    m_drawPolygon;
    QAction*    m_drawLine;
    QAction*    m_beginSeperator;
    QAction*    m_endSeperator;
    QAction*    m_loadOsmFile;
    QAction*    m_enableInputAction;

    m_addPlacemark = new QAction(this);
    m_addPlacemark->setText( "Add Placemark" );
    m_addPlacemark->setCheckable( true );
    connect( m_addPlacemark, SIGNAL( toggled(bool)),
             this, SLOT(setAddingPlacemark(bool)) );

    m_drawPolygon = new QAction( this );
    m_drawPolygon->setText( "Draw Polygon" );
    m_drawPolygon->setCheckable( true );
    connect( m_drawPolygon, SIGNAL(toggled(bool)),
             this, SLOT(setDrawingPolygon(bool)) );

    m_loadOsmFile = new QAction( this );
    m_loadOsmFile->setText( "Load Osm File" );
    connect( m_loadOsmFile, SIGNAL(triggered()),
             this, SLOT(loadOsmFile()) );

    m_beginSeperator = new QAction( this );
    m_beginSeperator->setSeparator( true );
    m_endSeperator = new QAction ( this );
    m_endSeperator->setSeparator( true );

    m_enableInputAction = new QAction(this);
    m_enableInputAction->setToolTip(tr("Enable Marble Input"));
    m_enableInputAction->setCheckable(true);
    m_enableInputAction->setChecked( true );
    m_enableInputAction->setIcon( QIcon( MarbleDirs::path("bitmaps/hand.png") ) );
    connect( m_enableInputAction, SIGNAL(toggled(bool)),
                       widget, SLOT( setInputEnabled(bool)) );

    initial->addAction( m_enableInputAction );
    initial->addAction( m_beginSeperator );

    group->addAction( m_addPlacemark );
    group->addAction( m_drawPolygon );
    group->addAction( m_loadOsmFile );
    group->addAction( m_endSeperator );

    result.append( initial );
    result.append( group );
    return result;
}

}

Q_EXPORT_PLUGIN2( OsmAnnotatePlugin, Marble::OsmAnnotatePlugin )

#include "OsmAnnotatePlugin.moc"
