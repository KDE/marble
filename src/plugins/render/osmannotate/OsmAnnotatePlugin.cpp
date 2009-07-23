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
#include "AreaAnnotation.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataDocument.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "MarbleWidget.h"
#include "PlacemarkTextAnnotation.h"

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
    return QStringList( "HOVERS_ABOVE_SURFACE" );
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
    widgetInitalised= false;
    m_tmp_lineString = 0;
    m_itemModel = 0;
    m_addingPlacemark = false;
    m_drawingPolygon = false;

    m_actions = 0;
    m_toolbarActions = 0;
}

bool OsmAnnotatePlugin::isInitialized () const
{
    return true;
}

QList<QActionGroup*>* OsmAnnotatePlugin::actionGroups() const
{
    return m_actions;
}

QList<QActionGroup*>* OsmAnnotatePlugin::toolbarActionGroups() const
{
    return m_toolbarActions;
}

bool OsmAnnotatePlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    if ( renderPos != "HOVERS_ABOVE_SURFACE" ) {
        return true;
    }
    
    if( !widgetInitalised ) {
        MarbleWidget* marbleWidget = (MarbleWidget*) painter->device();
        m_marbleWidget = marbleWidget;
        setupActions( marbleWidget );

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
            if( i->flags() & GeoGraphicsItem::ItemIsVisable ) {
                i->paint( painter, viewport, renderPos, layer );
            }
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

void OsmAnnotatePlugin::saveOsmFile()
{
    TmpGraphicsItem* item;
    QListIterator<TmpGraphicsItem*> it(model);
    while( it.hasNext() ) {
        item = it.next();
        qDebug() << "Saving item!";
//        implement the XML writer here
//        qDebug() << item;
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
                PlacemarkTextAnnotation* t = new PlacemarkTextAnnotation();
                t->setCoordinate(point);
                model.append(t);

                //FIXME only repaint the new placemark
                ( ( MarbleWidget* ) watched)->repaint();
                emit placemarkAdded();

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

void OsmAnnotatePlugin::setupActions(MarbleWidget* widget)
{
    QList<QActionGroup*>* toolbarActions = new QList<QActionGroup*>();
    QList<QActionGroup*>* actions = new QList<QActionGroup*>();

    QActionGroup* initial = new QActionGroup(0);
    initial->setExclusive( false );

    QActionGroup* group = new QActionGroup(0);
    group->setExclusive( true );

    QAction*    m_addPlacemark;
    QAction*    m_drawPolygon;
    QAction*    m_drawLine;
    QAction*    m_beginSeparator;
    QAction*    m_endSeparator;
    QAction*    m_loadOsmFile;
    QAction*    m_saveOsmFile;
    QAction*    m_enableInputAction;

    m_addPlacemark = new QAction(this);
    m_addPlacemark->setText( "Add Placemark" );
    m_addPlacemark->setCheckable( true );
    connect( m_addPlacemark, SIGNAL( toggled(bool)),
             this, SLOT(setAddingPlacemark(bool)) );
    connect( this, SIGNAL(placemarkAdded()) ,
             m_addPlacemark, SLOT(toggle()) );

    m_drawPolygon = new QAction( this );
    m_drawPolygon->setText( "Draw Polygon" );
    m_drawPolygon->setCheckable( true );
    connect( m_drawPolygon, SIGNAL(toggled(bool)),
             this, SLOT(setDrawingPolygon(bool)) );

    m_loadOsmFile = new QAction( this );
    m_loadOsmFile->setText( "Load Osm File" );
    connect( m_loadOsmFile, SIGNAL(triggered()),
             this, SLOT(loadOsmFile()) );

    m_saveOsmFile = new QAction( this );
    m_saveOsmFile->setText( "Save Osm File" );
    connect( m_saveOsmFile, SIGNAL(triggered()),
             this, SLOT(saveOsmFile()) );

    m_beginSeparator = new QAction( this );
    m_beginSeparator->setSeparator( true );
    m_endSeparator = new QAction ( this );
    m_endSeparator->setSeparator( true );

    m_enableInputAction = new QAction(this);
    m_enableInputAction->setToolTip(tr("Enable Marble Input"));
    m_enableInputAction->setCheckable(true);
    m_enableInputAction->setChecked( true );
    m_enableInputAction->setIcon( QIcon( MarbleDirs::path("bitmaps/hand.png") ) );
    connect( m_enableInputAction, SIGNAL(toggled(bool)),
                       widget, SLOT( setInputEnabled(bool)) );

    initial->addAction( m_enableInputAction );
    initial->addAction( m_beginSeparator );

    group->addAction( m_addPlacemark );
    group->addAction( m_drawPolygon );
    group->addAction( m_loadOsmFile );
    group->addAction( m_saveOsmFile );
    group->addAction( m_endSeparator );

    actions->append( initial );
    actions->append( group );

    toolbarActions->append( initial );
    toolbarActions->append( group );

    //delete the old groups if they exist
    delete m_actions;
    delete m_toolbarActions;

    m_actions = actions;
    m_toolbarActions = toolbarActions;

    emit actionGroupsChanged();
}

}

Q_EXPORT_PLUGIN2( OsmAnnotatePlugin, Marble::OsmAnnotatePlugin )

#include "OsmAnnotatePlugin.moc"
