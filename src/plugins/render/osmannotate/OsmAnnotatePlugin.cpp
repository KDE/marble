//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmAnnotatePlugin.h"

#include <QtGui/QFileDialog>

//#include <Phonon/MediaObject>
//#include <Phonon/VideoWidget>

#include <QtGui/QAction>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "AreaAnnotation.h"
#include "GeoDataDocument.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataParser.h"
#include "GeoPainter.h"
#include "GeoWriter.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "osm/OsmBoundsGraphicsItem.h"
#include "PlacemarkTextAnnotation.h"
#include "PointScreenGraphicsItem.h"
#include "TextAnnotation.h"

namespace Marble
{

OsmAnnotatePlugin::OsmAnnotatePlugin()
        : RenderPlugin(),
          m_itemModel( 0 ),
          m_networkAccessManager( 0 ),
          m_isInitialized( false )
{
}

OsmAnnotatePlugin::~OsmAnnotatePlugin()
{
    delete m_itemModel;
    delete m_networkAccessManager;
}

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
    QStringList positions;
    positions.append( "HOVERS_ABOVE_SURFACE" );
    positions.append( "USER_TOOLS" );
    return positions;
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
    return tr( "This is a render and interaction plugin used for annotating OSM data." );
}

QIcon OsmAnnotatePlugin::icon () const
{
    return QIcon();
}


void OsmAnnotatePlugin::initialize ()
{
    widgetInitalised= false;
    m_tmp_lineString = 0;
    m_itemModel = new QList<GeoGraphicsItem*>();
    m_addingPlacemark = false;
    m_drawingPolygon = false;

    m_actions = 0;
    m_toolbarActions = 0;

    PointScreenGraphicsItem* topLeft = new PointScreenGraphicsItem();
    PointScreenGraphicsItem* bottomRight = new PointScreenGraphicsItem();

    topLeft->setVisible(false);
    bottomRight->setVisible(false);

    m_selectionBox.first = topLeft;
    m_selectionBox.second = bottomRight;
    m_isInitialized = true;
}

bool OsmAnnotatePlugin::isInitialized () const
{
    return m_isInitialized;
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
    if ( renderPos != "HOVERS_ABOVE_SURFACE" && renderPos != "USER_TOOLS" ) {
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

    if( renderPos == "HOVERS_ABOVE_SURFACE" ) {
        painter->autoMapQuality();

        //so the user can keep track of the current polygon drawing
        if( m_tmp_lineString ) {
            painter->drawPolyline( *m_tmp_lineString );
        }

        if( m_itemModel ) {
            QListIterator<GeoGraphicsItem*> it( *m_itemModel );

            while( it.hasNext() ) {
                GeoGraphicsItem* i = it.next();
                if( i->flags() & GeoGraphicsItem::ItemIsVisible ) {
                    i->paint( painter, viewport, renderPos, layer );
                }
            }
        }
    }

    QListIterator<TmpGraphicsItem*> i(model);

    while(i.hasNext()) {
        TmpGraphicsItem* tmp= i.next();
        tmp->paint(painter, viewport, renderPos, layer);
    }

    if( m_selectionBox.first->visible() ) {
        m_selectionBox.first->paint( painter, viewport, renderPos, layer );
        m_selectionBox.second->paint( painter, viewport, renderPos, layer );
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

void OsmAnnotatePlugin::receiveNetworkReply( QNetworkReply *reply )
{
    if( reply->error() == QNetworkReply::NoError ) {
        readOsmFile( reply, false );
    } else {
        m_errorMessage.showMessage( tr("Error while trying to download the "
                                            "OSM file from the server. The "
                                            "error was:\n %1" ).arg(reply->errorString()) );
    }
}

void OsmAnnotatePlugin::loadOsmFile()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(0, tr("Open File"),
                            QString(),
                            tr("All Supported Files (*.osm);;Open Street Map Data (*.osm)"));

    if ( ! filename.isNull() ) {

        QFile file( filename );
        if ( !file.exists() ) {
            qDebug( "File does not exist!" );
            return;
        }

        // Open file in right mode
        file.open( QIODevice::ReadOnly );

        readOsmFile( &file, true );

        file.close();
    }
}

void OsmAnnotatePlugin::downloadOsmFile()
{
    if( !m_selectionBox.first->visible() ) {
        m_errorMessage.showMessage( tr("Please select an area before trying to "
                                       "download an OSM file" ) );
        return;
    }
    QPointF topLeft = m_selectionBox.first->position();
    QPointF bottomRight = m_selectionBox.second->position();

    qreal lonTop, latTop;
    qreal lonBottom, latBottom;

    GeoDataCoordinates topLeftCoordinates;
    GeoDataCoordinates bottomRightCoordinates;

    bool topIsOnGlobe = m_marbleWidget->geoCoordinates( topLeft.x(),
                                                        topLeft.y(),
                                                        lonTop, latTop,
                                                        GeoDataCoordinates::Radian);
    bool bottomIsOnGlobe = m_marbleWidget->geoCoordinates( bottomRight.x(),
                                                           bottomRight.y(),
                                                           lonBottom, latBottom,
                                                           GeoDataCoordinates::Radian );

    if( ! ( topIsOnGlobe && bottomIsOnGlobe ) ) {
        m_errorMessage.showMessage( tr("One of the selection points is not on"
                                       " the Globe. Please only select a region"
                                       " on the globe.") );
        return;
    }

    topLeftCoordinates = GeoDataCoordinates( lonTop, latTop, 0,
                                             GeoDataCoordinates::Radian );

    bottomRightCoordinates = GeoDataCoordinates( lonBottom, latBottom, 0,
                                                 GeoDataCoordinates::Radian );

    GeoDataLineString tempString;
    tempString.append( topLeftCoordinates );
    tempString.append( bottomRightCoordinates );

    GeoDataLatLonAltBox bounds = GeoDataLatLonAltBox::fromLineString( tempString );

    QString request;
    request = QString("http://api.openstreetmap.org/api/0.6/map?bbox=%1,%2,%3,%4")
              .arg(bounds.west(GeoDataCoordinates::Degree) )
              .arg(bounds.south(GeoDataCoordinates::Degree) )
              .arg(bounds.east( GeoDataCoordinates::Degree) )
              .arg( bounds.north( GeoDataCoordinates::Degree ) );

    QNetworkRequest networkRequest;
    networkRequest.setUrl(QUrl(request) );

    if( ! m_networkAccessManager ) {
        m_networkAccessManager = new QNetworkAccessManager( this ) ;
        connect( m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
                 this, SLOT(receiveNetworkReply(QNetworkReply*)) );
    }

    m_networkAccessManager->get( networkRequest );
}

void OsmAnnotatePlugin::saveAnnotationFile()
{
    GeoDataDocument document;

    QList<TextAnnotation*> allAnnotations = annotations();

    TextAnnotation* annotation;
    foreach( annotation, allAnnotations ) {
        document.append( annotation->toGeoData() );
    }

    QString filename;
    filename = QFileDialog::getSaveFileName( 0, tr("Save Annotation File"),
                            QString(),
                            tr("All Supported Files (*.kml);;KML file (*.kml)"));

    if ( ! filename.isNull() ) {

        GeoWriter writer;
        //FIXME: a better way to do this?
        writer.setDocumentType( "http://earth.google.com/kml/2.2" );

        QFile file( filename );

        // Open file in right mode
        file.open( QIODevice::ReadWrite );

        if ( !writer.write( &file, document ) ) {
            qDebug( "Could not write the file." );
        }
    }
}

void OsmAnnotatePlugin::loadAnnotationFile()
{
    //load the file here
    QString filename;
    filename = QFileDialog::getOpenFileName(0, tr("Open Annotation File"),
                            QString(),
                            tr("All Supported Files (*.kml);;Kml Annotation file (*.kml)"));

    if ( ! filename.isNull() ) {

        GeoDataParser parser( GeoData_KML );

        QFile file( filename );
        if ( !file.exists() ) {
            qDebug( "File does not exist!" );
            return;
        }

        // Open file in right mode
        file.open( QIODevice::ReadOnly );

        if ( !parser.read( &file ) ) {
            qDebug( "Could not parse file!" );
            return;
        }

        GeoDataDocument* document = dynamic_cast<GeoDataDocument*>(parser.releaseDocument() );
        Q_ASSERT( document );

        file.close();

        QVector<GeoDataFeature*>::ConstIterator it = document->constBegin();
        for( ; it < document->constEnd(); ++it ) {
            PlacemarkTextAnnotation* annotation = new PlacemarkTextAnnotation();
            annotation->setName( (*it)->name() );
            annotation->setDescription( (*it)->description() );
            // annotation->setCoordinate( GeoDataPlacemark((*it)).coordinate() );
            /** @todo: line above replaced with the four below to have it compile.
                Needs verification that behavior stays the same */
            GeoDataPlacemark* placemark = dynamic_cast<GeoDataPlacemark*>(*it);
            if ( placemark ) {
                annotation->setCoordinate( placemark->coordinate() );
            }
            model.append( annotation );
        }

        delete document;
        emit repaintNeeded(QRegion());
    }
}

void OsmAnnotatePlugin::selectArea( bool startSelectingArea )
{
    if ( !startSelectingArea ) {
        //finished selecting
        m_selectionBox.first->setVisible( false );
        m_selectionBox.second->setVisible( false );
        return;
    }

    //move the 2 squares to the middle
    QPointF middle( m_marbleWidget->size().width() /2 ,
                    m_marbleWidget->size().height() /2 );
    m_selectionBox.first->setPosition( middle - QPointF( 20, 20 ) );
    m_selectionBox.second->setPosition( middle + QPointF( 20, 20 ) );

    m_selectionBox.first->setVisible( true );
    m_selectionBox.second->setVisible( true );

    emit repaintNeeded(QRegion());
}

bool    OsmAnnotatePlugin::eventFilter(QObject* watched, QEvent* event)
{
    MarbleWidget* marbleWidget = dynamic_cast<MarbleWidget*>( watched );
    Q_ASSERT( marbleWidget );

    //FIXME why is the QEvent::MousePress not working? caught somewhere else?
    //does this mean we need to centralize the event handling?

    //so far only accept mouse events
    if( event->type() != QEvent::MouseButtonPress
        && event->type() != QEvent::MouseButtonRelease
        && event->type() != QEvent::MouseMove )
    {
        return false;
    }

    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
    Q_ASSERT( mouseEvent );


    GeoDataCoordinates coordinates;

    //Pass event on to Geo Graphics Items if there is one
    // Only working in Geo Coordinates for hit tests

//    FIXME: currently waiting for a decision on http://reviewboard.kde.org/r/1264/
//    bool isOnGlobe = marbleWidget->geoCoordinates(mouseEvent->pos(), coordinates );

    qreal lon, lat;
    bool isOnGlobe = marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                                   mouseEvent->pos().y(),
                                                   lon, lat,
                                                   GeoDataCoordinates::Radian );

    coordinates = GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Radian );

    if( !isOnGlobe ) {
        //don't handle this event
        return false;
    }

    //Pass the event to Graphics Items
    QList<TmpGraphicsItem*>::ConstIterator itemIterator = model.constBegin();
    for( ; itemIterator < model.constEnd() ; ++itemIterator ) {
        QListIterator<QRegion> it ( (*itemIterator)->regions() );

        while ( it.hasNext() ) {
            QRegion p = it.next();
            if( p.contains( mouseEvent->pos() ) ) {
                if( (*itemIterator)->sceneEvent( event ) ) {
                    return true;
                }
            }
        }
    }

    //FIXME: finish cleaning this up

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

    QActionGroup* nonExclusiveGroup = new QActionGroup(0);
    nonExclusiveGroup->setExclusive( false );

    QAction*    addPlacemark;
    QAction*    drawPolygon;
    QAction*    beginSeparator;
    QAction*    endSeparator;
    QAction*    loadOsmFile;
    QAction*    saveAnnotationFile;
    QAction*    loadAnnotationFile;
    QAction*    enableInputAction;
    QAction*    selectArea;
    QAction*    downloadOsm;

    addPlacemark = new QAction(this);
    addPlacemark->setText( tr("Add Placemark") );
    addPlacemark->setCheckable( true );
    connect( addPlacemark, SIGNAL( toggled(bool)),
             this, SLOT(setAddingPlacemark(bool)) );
    connect( this, SIGNAL(placemarkAdded()) ,
             addPlacemark, SLOT(toggle()) );

    drawPolygon = new QAction( this );
    drawPolygon->setText( tr("Draw Polygon") );
    drawPolygon->setCheckable( true );
    connect( drawPolygon, SIGNAL(toggled(bool)),
             this, SLOT(setDrawingPolygon(bool)) );

    loadOsmFile = new QAction( this );
    loadOsmFile->setText( tr("Load Osm File") );
    connect( loadOsmFile, SIGNAL(triggered()),
             this, SLOT(loadOsmFile()) );

    saveAnnotationFile = new QAction( this );
    saveAnnotationFile->setText( tr("Save Annotation File") );
    connect( saveAnnotationFile, SIGNAL(triggered()),
             this, SLOT(saveAnnotationFile()) );

    loadAnnotationFile = new QAction( this );
    loadAnnotationFile->setText( tr("Load Annotation File" ) );
    connect( loadAnnotationFile, SIGNAL(triggered()),
             this, SLOT(loadAnnotationFile()) );

    beginSeparator = new QAction( this );
    beginSeparator->setSeparator( true );
    endSeparator = new QAction ( this );
    endSeparator->setSeparator( true );

    enableInputAction = new QAction(this);
    enableInputAction->setToolTip(tr("Enable Marble Input"));
    enableInputAction->setCheckable(true);
    enableInputAction->setChecked( true );
    enableInputAction->setIcon( QIcon( MarbleDirs::path("bitmaps/hand.png") ) );
    connect( enableInputAction, SIGNAL(toggled(bool)),
                       widget, SLOT( setInputEnabled(bool)) );

    selectArea = new QAction( this );
    selectArea->setText( tr("Select Map Area") );
    selectArea->setCheckable( true );
    connect( selectArea, SIGNAL(triggered(bool)),
             this, SLOT(selectArea(bool)) );


    downloadOsm = new QAction( this );
    downloadOsm->setText( tr("Download Osm File") );
    downloadOsm->setToolTip(tr("Download Osm File for selected area"));
    connect( downloadOsm, SIGNAL(triggered()),
             this, SLOT(downloadOsmFile()) );


    initial->addAction( enableInputAction );
    initial->addAction( beginSeparator );

    group->addAction( addPlacemark );
    group->addAction( drawPolygon );
    group->addAction( loadOsmFile );
    group->addAction( saveAnnotationFile );
    group->addAction( loadAnnotationFile );
    group->addAction( endSeparator );

    nonExclusiveGroup->addAction( selectArea );
    nonExclusiveGroup->addAction( downloadOsm );

    actions->append( initial );
    actions->append( group );
    actions->append( nonExclusiveGroup );

    toolbarActions->append( initial );
    toolbarActions->append( group );
    toolbarActions->append( nonExclusiveGroup );

    //delete the old groups if they exist
    delete m_actions;
    delete m_toolbarActions;

    m_actions = actions;
    m_toolbarActions = toolbarActions;

    emit actionGroupsChanged();
}

void OsmAnnotatePlugin::readOsmFile( QIODevice *device, bool flyToFile )
{
    GeoDataParser parser( GeoData_OSM );

    if ( !parser.read( device ) ) {
        qDebug( "Could not parse file!" );
        return;
    }
    QList<GeoGraphicsItem*>* model = parser.releaseModel();
    Q_ASSERT( model );

    m_itemModel->append(*model);

    // now zoom to the newly added OSM file
    if( flyToFile && m_itemModel->size() > 0 ) {
        OsmBoundsGraphicsItem* item;
        // mostly guaranteed that the first item will be a bounds item
        // if not then don't centre on anything
        item = dynamic_cast<OsmBoundsGraphicsItem*>( model->first() );
        if( item ) {
            m_marbleWidget->centerOn( item->latLonBox() );
        }
    }

    model->clear();
    delete model;

    emit repaintNeeded(QRegion());
}

QList<TextAnnotation*> OsmAnnotatePlugin::annotations() const
{
    QList<TextAnnotation*> tmpAnnotations;
    TmpGraphicsItem* item;
    foreach( item, model ) {
        TextAnnotation* annotation = dynamic_cast<TextAnnotation*>(item);
        if( annotation ) {
            tmpAnnotations.append( annotation );
        }
    }

    return tmpAnnotations;
}

}

Q_EXPORT_PLUGIN2( OsmAnnotatePlugin, Marble::OsmAnnotatePlugin )

#include "OsmAnnotatePlugin.moc"
