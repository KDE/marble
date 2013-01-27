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
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "GeoPainter.h"
#include "GeoWriter.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "PlacemarkTextAnnotation.h"

namespace Marble
{

OsmAnnotatePlugin::OsmAnnotatePlugin()
        : RenderPlugin( 0 ),
          m_AnnotationDocument( new GeoDataDocument ),
          m_networkAccessManager( 0 ),
          m_isInitialized( false )
{
}

OsmAnnotatePlugin::OsmAnnotatePlugin(const MarbleModel *model)
        : RenderPlugin(model),
          m_marbleWidget( 0 ),
          m_AnnotationDocument( new GeoDataDocument ),
          m_networkAccessManager( 0 ),
          m_isInitialized( false )
{
    Q_UNUSED(model);

    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );
    connect( this, SIGNAL(visibilityChanged(bool,QString)), SLOT(enableModel(bool)) );

    m_AnnotationDocument->setName( tr("Annotations") );
    m_AnnotationDocument->setDocumentRole( UserDocument );
    GeoDataStyle style;
    GeoDataPolyStyle polyStyle;
    polyStyle.setColor( QColor( 0, 255, 255, 80 ) );
    style.setStyleId( "polygon" );
    style.setPolyStyle( polyStyle );
    m_AnnotationDocument->addStyle( style );
}

OsmAnnotatePlugin::~OsmAnnotatePlugin()
{
    m_marbleWidget->model()->treeModel()->removeDocument( m_AnnotationDocument );
    delete m_AnnotationDocument;
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
    positions.append( "ALWAYS_ON_TOP" );
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

QString OsmAnnotatePlugin::version() const
{
    return "1.0";
}

QString OsmAnnotatePlugin::copyrightYears() const
{
    return "2009";
}

QList<PluginAuthor> OsmAnnotatePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Andrew Manson", "<g.real.ate@gmail.com>" );
}

QIcon OsmAnnotatePlugin::icon () const
{
    return QIcon();
}


void OsmAnnotatePlugin::initialize ()
{
    widgetInitalised= false;
    m_tmp_lineString = 0;
    m_tmp_linearRing = 0;
    m_selectedItem = 0;
    m_addingPlacemark = false;
    m_drawingPolygon = false;
    m_removingItem = false;
    m_isInitialized = true;
}

bool OsmAnnotatePlugin::isInitialized () const
{
    return m_isInitialized;
}

QString OsmAnnotatePlugin::runtimeTrace() const
{
    return QString("Annotate Items: %1").arg( m_AnnotationDocument->size() );
}
const QList<QActionGroup*>* OsmAnnotatePlugin::actionGroups() const
{
    return &m_actions;
}

const QList<QActionGroup*>* OsmAnnotatePlugin::toolbarActionGroups() const
{
    return &m_toolbarActions;
}

bool OsmAnnotatePlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    if( !widgetInitalised ) {
        MarbleWidget* marbleWidget = (MarbleWidget*) painter->device();
        m_marbleWidget = marbleWidget;
        setupActions( marbleWidget );
        m_marbleWidget->model()->treeModel()->addDocument( m_AnnotationDocument );
        widgetInitalised = true;
    }

    painter->autoMapQuality();

    //so the user can keep track of the current polygon drawing
    if( m_tmp_lineString ) {
        painter->drawPolyline( *m_tmp_lineString );
    }
    if( m_tmp_linearRing ) {
        painter->drawPolyline( *m_tmp_linearRing );
    }

    QListIterator<SceneGraphicsItem*> i( m_graphicsItems );

    while(i.hasNext()) {
        SceneGraphicsItem* tmp= i.next();
        tmp->paint(painter, viewport);
    }

    return true;
}

void OsmAnnotatePlugin::enableModel( bool enabled )
{
    if( enabled ) {
        if( m_marbleWidget ) {
            setupActions( m_marbleWidget );
            m_marbleWidget->model()->treeModel()->addDocument( m_AnnotationDocument );
        }
    } else {
        setupActions( 0 );
        if( m_marbleWidget ) {
            m_marbleWidget->model()->treeModel()->removeDocument( m_AnnotationDocument );
        }
    }
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
        if ( m_tmp_linearRing != 0 ) {
            GeoDataPlacemark *placemark = new GeoDataPlacemark;
            GeoDataPolygon *poly = new GeoDataPolygon( Tessellate );
            poly->setOuterBoundary( GeoDataLinearRing(*m_tmp_linearRing) );
            placemark->setGeometry( poly );
            placemark->setParent( m_AnnotationDocument );
            placemark->setStyleUrl( "#polygon" );

            delete m_tmp_linearRing;
            m_tmp_linearRing = 0;

            AreaAnnotation* area = new AreaAnnotation( placemark );

            m_marbleWidget->model()->treeModel()->addFeature( m_AnnotationDocument, placemark );
            m_graphicsItems.append( area );
        }
    }
}

void OsmAnnotatePlugin::setRemovingItems( bool toggle )
{
    m_removingItem = toggle;
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

void OsmAnnotatePlugin::downloadOsmFile()
{
    QPointF topLeft(0,0);
    QPointF bottomRight(m_marbleWidget->size().width(), m_marbleWidget->size().height());

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

void OsmAnnotatePlugin::clearAnnotations()
{
    qDeleteAll( m_graphicsItems );
    m_graphicsItems.clear();
    while( m_AnnotationDocument->size() > 0 ) {
        m_marbleWidget->model()->treeModel()->removeFeature( m_AnnotationDocument, 0 );
    }
}

void OsmAnnotatePlugin::saveAnnotationFile()
{
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
        file.open( QIODevice::WriteOnly );

        if ( !writer.write( &file, m_AnnotationDocument ) ) {
            qDebug( "Could not write the file." );
        }
        file.close();
    }
}

void OsmAnnotatePlugin::loadAnnotationFile()
{
    //load the file here
    QString filename;
    filename = QFileDialog::getOpenFileName(0, tr("Open Annotation File"),
                            QString(),
                            tr("All Supported Files (*.kml);;Kml Annotation file (*.kml)"));

    if ( filename.isNull() ) {
        return;
    }


    QFile file( filename );
    if ( !file.exists() ) {
        qDebug( "File does not exist!" );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    GeoDataParser parser( GeoData_KML );
    if ( !parser.read( &file ) ) {
        qDebug( "Could not parse file!" );
        return;
    }

    GeoDataDocument* document = dynamic_cast<GeoDataDocument*>(parser.releaseDocument() );
    Q_ASSERT( document );

    file.close();

    foreach( GeoDataFeature *feature, document->featureList() ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( feature );
        if( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType ) {
            GeoDataPlacemark *newPlacemark = new GeoDataPlacemark( *placemark );
            PlacemarkTextAnnotation* annotation = new PlacemarkTextAnnotation( newPlacemark );
            m_graphicsItems.append( annotation );
            m_marbleWidget->model()->treeModel()->addFeature( m_AnnotationDocument, newPlacemark );
        }
        else if( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            GeoDataPlacemark *newPlacemark = new GeoDataPlacemark( *placemark );
            newPlacemark->setParent( m_AnnotationDocument );
            newPlacemark->setStyleUrl( placemark->styleUrl() );
            AreaAnnotation* annotation = new AreaAnnotation( newPlacemark );
            m_graphicsItems.append( annotation );
            m_marbleWidget->model()->treeModel()->addFeature( m_AnnotationDocument, newPlacemark );
        }
    }
    m_marbleWidget->centerOn( document->latLonAltBox() );

    delete document;
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
    qreal lon, lat;
    bool isOnGlobe = marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                                   mouseEvent->pos().y(),
                                                   lon, lat,
                                                   GeoDataCoordinates::Radian );
    coordinates = GeoDataCoordinates( lon, lat );
    if( !isOnGlobe ) {
        return false;
    }

    // handle easily the mousemove
    if( event->type() == QEvent::MouseMove && m_selectedItem ) {
        if( m_selectedItem->sceneEvent( event ) ) {
            m_marbleWidget->model()->treeModel()->updateFeature( m_selectedItem->placemark() );
            return true;
        }
    }

    //Pass the event to Graphics Items
    foreach( SceneGraphicsItem *item, m_graphicsItems ) {
        QListIterator<QRegion> it ( item->regions() );

        while ( it.hasNext() ) {
            QRegion p = it.next();
            if( p.contains( mouseEvent->pos() ) ) {
                // deal with removing items
                if( mouseEvent->button() == Qt::LeftButton && m_removingItem ) {
                    m_graphicsItems.removeAll( item );
                    m_marbleWidget->model()->treeModel()->removeFeature( item->feature() );
                    delete item->feature();
                    delete item;
                    emit itemRemoved();
                }
                else {
                    if( item->sceneEvent( event ) ) {
                        if( event->type() == QEvent::MouseButtonPress ) {
                            m_selectedItem = item;
                        } else {
                            m_selectedItem = 0;
                        }
                        m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );
                        return true;
                    }
                }
            }
        }
    }

    //FIXME: finish cleaning this up

    // deal with adding a placemark
    if ( mouseEvent->button() == Qt::LeftButton
         && m_addingPlacemark ) {
        //Add a placemark on the screen
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        placemark->setCoordinate( coordinates );
        PlacemarkTextAnnotation* t = new PlacemarkTextAnnotation( placemark );
        m_marbleWidget->model()->treeModel()->addFeature( m_AnnotationDocument, placemark );
        m_graphicsItems.append( t );
        emit placemarkAdded();
        return true;
    }

    // deal with drawing a polygon
    if ( mouseEvent->button() == Qt::LeftButton
         && mouseEvent->type() == QEvent::MouseButtonPress
         && m_drawingPolygon ) {
        if ( m_tmp_linearRing == 0 ) {
            m_tmp_linearRing = new GeoDataLinearRing( Tessellate );
        }

        m_tmp_linearRing->append(GeoDataCoordinates(lon, lat));

        //FIXME only repaint the line string so far
        marbleWidget->update();
        return true;
    }

    return false;
}

void OsmAnnotatePlugin::setupActions(MarbleWidget* widget)
{
    qDeleteAll( m_actions );
    m_actions.clear();
    m_toolbarActions.clear();

    if( widget ) {
        QActionGroup* group = new QActionGroup(0);
        group->setExclusive( false );

        QActionGroup* nonExclusiveGroup = new QActionGroup(0);
        nonExclusiveGroup->setExclusive( false );

        QAction*    enableInputAction;
        QAction*    addPlacemark;
        QAction*    drawPolygon;
        QAction*    removeItem;
        QAction*    beginSeparator;
        QAction*    endSeparator;
        QAction*    loadAnnotationFile;
        QAction*    saveAnnotationFile;
        QAction*    clearAnnotations;
        QAction*    downloadOsm;

        enableInputAction = new QAction(this);
        enableInputAction->setToolTip(tr("Enable Marble Input"));
        enableInputAction->setCheckable(true);
        enableInputAction->setChecked( true );
        enableInputAction->setIcon( QIcon( ":/icons/hand.png") );
        connect( enableInputAction, SIGNAL(toggled(bool)),
                 widget, SLOT(setInputEnabled(bool)) );

        addPlacemark = new QAction(this);
        addPlacemark->setToolTip( tr("Add Placemark") );
        addPlacemark->setCheckable( true );
        addPlacemark->setIcon( QIcon( ":/icons/draw-placemark.png") );
        connect( addPlacemark, SIGNAL(toggled(bool)),
                 this, SLOT(setAddingPlacemark(bool)) );
        connect( this, SIGNAL(placemarkAdded()) ,
                 addPlacemark, SLOT(toggle()) );

        drawPolygon = new QAction( this );
        drawPolygon->setToolTip( tr("Draw Polygon") );
        drawPolygon->setCheckable( true );
        drawPolygon->setIcon( QIcon( ":/icons/draw-polygon.png") );
        connect( drawPolygon, SIGNAL(toggled(bool)),
                 this, SLOT(setDrawingPolygon(bool)) );

        removeItem = new QAction( this );
        removeItem->setToolTip( tr("Remove Item") );
        removeItem->setCheckable( true );
        removeItem->setIcon( QIcon( ":/icons/edit-delete-shred.png") );
        connect( removeItem, SIGNAL(toggled(bool)),
                 this, SLOT(setRemovingItems(bool)) );
        connect( this, SIGNAL(itemRemoved()),
                 removeItem, SLOT(toggle()) );

        loadAnnotationFile = new QAction( this );
        loadAnnotationFile->setToolTip( tr("Load Annotation File" ) );
        loadAnnotationFile->setIcon( QIcon( ":/icons/document-import.png") );
        connect( loadAnnotationFile, SIGNAL(triggered()),
                 this, SLOT(loadAnnotationFile()) );

        saveAnnotationFile = new QAction( this );
        saveAnnotationFile->setToolTip( tr("Save Annotation File") );
        saveAnnotationFile->setIcon( QIcon( ":/icons/document-export.png") );
        connect( saveAnnotationFile, SIGNAL(triggered()),
                 this, SLOT(saveAnnotationFile()) );

        clearAnnotations = new QAction( this );
        clearAnnotations->setToolTip( tr("Clear Annotations") );
        clearAnnotations->setIcon( QIcon( ":/icons/remove.png") );
        connect( clearAnnotations, SIGNAL(triggered()),
                 this, SLOT(clearAnnotations()) );

        beginSeparator = new QAction( this );
        beginSeparator->setSeparator( true );
        endSeparator = new QAction ( this );
        endSeparator->setSeparator( true );


        downloadOsm = new QAction( this );
        downloadOsm->setText( tr("Download Osm File") );
        downloadOsm->setToolTip(tr("Download Osm File for selected area"));
        connect( downloadOsm, SIGNAL(triggered()),
                 this, SLOT(downloadOsmFile()) );


        group->addAction( enableInputAction );
        group->addAction( beginSeparator );
        group->addAction( addPlacemark );
        group->addAction( drawPolygon );
        group->addAction( removeItem );
        group->addAction( loadAnnotationFile );
        group->addAction( saveAnnotationFile );
        group->addAction( clearAnnotations );
        group->addAction( endSeparator );

        nonExclusiveGroup->addAction( downloadOsm );

        m_actions.append( group );
        m_actions.append( nonExclusiveGroup );

        m_toolbarActions.append( group );
        m_toolbarActions.append( nonExclusiveGroup );
    }

    emit actionGroupsChanged();
}

void OsmAnnotatePlugin::readOsmFile( QIODevice *device, bool flyToFile )
{
}

}

Q_EXPORT_PLUGIN2( OsmAnnotatePlugin, Marble::OsmAnnotatePlugin )

#include "OsmAnnotatePlugin.moc"
