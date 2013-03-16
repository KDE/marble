//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//

#include "AnnotatePlugin.h"


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
#include "KmlElementDictionary.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "PlacemarkTextAnnotation.h"

#include <QtGui/QFileDialog>

#include <QtGui/QAction>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Marble
{

AnnotatePlugin::AnnotatePlugin(const MarbleModel *model)
        : RenderPlugin(model),
          m_widgetInitialized( false ),
          m_marbleWidget( 0 ),
          m_AnnotationDocument( new GeoDataDocument ),
          m_tmp_placemark( 0 ),
          m_selectedItem( 0 ),
          m_addingPlacemark( false ),
          m_drawingPolygon( false ),
          m_removingItem( false ),
//          m_networkAccessManager( 0 ),
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

AnnotatePlugin::~AnnotatePlugin()
{
    if( m_marbleWidget != 0 ) {
        m_marbleWidget->model()->treeModel()->removeDocument( m_AnnotationDocument );
    }
    delete m_AnnotationDocument;
//    delete m_networkAccessManager;
}

QStringList AnnotatePlugin::backendTypes() const
{
    return QStringList( "annotation" );
}

QString AnnotatePlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList AnnotatePlugin::renderPosition() const
{
    QStringList positions;
    positions.append( "ALWAYS_ON_TOP" );
    return positions;
}

QString AnnotatePlugin::name() const
{
    return tr( "Annotation" );
}

QString AnnotatePlugin::guiString() const
{
    return tr( "&Annotation" );
}

QString AnnotatePlugin::nameId() const
{
    return QString( "annotation" );
}

QString AnnotatePlugin::description() const
{
    return tr( "Draws annotations on maps with placemarks or polygons." );
}

QString AnnotatePlugin::version() const
{
    return "1.0";
}

QString AnnotatePlugin::copyrightYears() const
{
    return "2009, 2013";
}

QList<PluginAuthor> AnnotatePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Andrew Manson", "<g.real.ate@gmail.com>" )
            << PluginAuthor( "Thibaut Gridel", "<tgridel@free.fr>" );
}

QIcon AnnotatePlugin::icon () const
{
    return QIcon( ":/icons/draw-placemark.png");
}


void AnnotatePlugin::initialize ()
{
    if( !m_isInitialized ) {
        m_widgetInitialized= false;
        delete m_tmp_placemark;
        m_tmp_placemark = 0;
        delete m_selectedItem;
        m_selectedItem = 0;
        m_addingPlacemark = false;
        m_drawingPolygon = false;
        m_removingItem = false;
        m_isInitialized = true;
    }
}

bool AnnotatePlugin::isInitialized () const
{
    return m_isInitialized;
}

QString AnnotatePlugin::runtimeTrace() const
{
    return QString("Annotate Items: %1").arg( m_AnnotationDocument->size() );
}
const QList<QActionGroup*>* AnnotatePlugin::actionGroups() const
{
    return &m_actions;
}

const QList<QActionGroup*>* AnnotatePlugin::toolbarActionGroups() const
{
    return &m_toolbarActions;
}

bool AnnotatePlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED(renderPos);
    Q_UNUSED(layer);

    painter->autoMapQuality();

    QListIterator<SceneGraphicsItem*> i( m_graphicsItems );

    while(i.hasNext()) {
        SceneGraphicsItem* tmp= i.next();
        tmp->paint(painter, viewport);
    }

    return true;
}

void AnnotatePlugin::enableModel( bool enabled )
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

void AnnotatePlugin::setAddingPlacemark( bool b)
{
    m_addingPlacemark = b;
}

void AnnotatePlugin::setDrawingPolygon(bool b)
{
    m_drawingPolygon = b;
    if( b ) {
        m_tmp_placemark = new GeoDataPlacemark;
        GeoDataPolygon *poly = new GeoDataPolygon( Tessellate );
        m_tmp_placemark->setGeometry( poly );
        m_tmp_placemark->setParent( m_AnnotationDocument );
        m_tmp_placemark->setStyleUrl( "#polygon" );
        m_marbleWidget->model()->treeModel()->addFeature( m_AnnotationDocument, m_tmp_placemark );
    }
    if( !b ) {
        //stopped drawing the polygon
        GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( m_tmp_placemark->geometry() );
        if ( poly && poly->outerBoundary().size() != 0 ) {
            AreaAnnotation* area = new AreaAnnotation( m_tmp_placemark );
            m_graphicsItems.append( area );
            m_marbleWidget->update();
        }
        else {
            m_marbleWidget->model()->treeModel()->removeFeature( m_tmp_placemark );
            delete m_tmp_placemark;
        }
        m_tmp_placemark = 0;
    }
}

void AnnotatePlugin::setRemovingItems( bool toggle )
{
    m_removingItem = toggle;
}

//void AnnotatePlugin::receiveNetworkReply( QNetworkReply *reply )
//{
//    if( reply->error() == QNetworkReply::NoError ) {
//        readOsmFile( reply, false );
//    } else {
//        m_errorMessage.showMessage( tr("Error while trying to download the "
//                                            "OSM file from the server. The "
//                                            "error was:\n %1" ).arg(reply->errorString()) );
//    }
//}

//void AnnotatePlugin::downloadOsmFile()
//{
//    QPointF topLeft(0,0);
//    QPointF bottomRight(m_marbleWidget->size().width(), m_marbleWidget->size().height());

//    qreal lonTop, latTop;
//    qreal lonBottom, latBottom;

//    GeoDataCoordinates topLeftCoordinates;
//    GeoDataCoordinates bottomRightCoordinates;

//    bool topIsOnGlobe = m_marbleWidget->geoCoordinates( topLeft.x(),
//                                                        topLeft.y(),
//                                                        lonTop, latTop,
//                                                        GeoDataCoordinates::Radian);
//    bool bottomIsOnGlobe = m_marbleWidget->geoCoordinates( bottomRight.x(),
//                                                           bottomRight.y(),
//                                                           lonBottom, latBottom,
//                                                           GeoDataCoordinates::Radian );

//    if( ! ( topIsOnGlobe && bottomIsOnGlobe ) ) {
//        m_errorMessage.showMessage( tr("One of the selection points is not on"
//                                       " the Globe. Please only select a region"
//                                       " on the globe.") );
//        return;
//    }

//    topLeftCoordinates = GeoDataCoordinates( lonTop, latTop, 0,
//                                             GeoDataCoordinates::Radian );

//    bottomRightCoordinates = GeoDataCoordinates( lonBottom, latBottom, 0,
//                                                 GeoDataCoordinates::Radian );

//    GeoDataLineString tempString;
//    tempString.append( topLeftCoordinates );
//    tempString.append( bottomRightCoordinates );

//    GeoDataLatLonAltBox bounds = GeoDataLatLonAltBox::fromLineString( tempString );

//    QString request;
//    request = QString("http://api.openstreetmap.org/api/0.6/map?bbox=%1,%2,%3,%4")
//              .arg(bounds.west(GeoDataCoordinates::Degree) )
//              .arg(bounds.south(GeoDataCoordinates::Degree) )
//              .arg(bounds.east( GeoDataCoordinates::Degree) )
//              .arg( bounds.north( GeoDataCoordinates::Degree ) );

//    QNetworkRequest networkRequest;
//    networkRequest.setUrl(QUrl(request) );

//    if( ! m_networkAccessManager ) {
//        m_networkAccessManager = new QNetworkAccessManager( this ) ;
//        connect( m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
//                 this, SLOT(receiveNetworkReply(QNetworkReply*)) );
//    }

//    m_networkAccessManager->get( networkRequest );
//}

void AnnotatePlugin::clearAnnotations()
{
    m_selectedItem = 0;
    delete m_tmp_placemark;
    m_tmp_placemark = 0;
    qDeleteAll( m_graphicsItems );
    m_graphicsItems.clear();
    m_marbleWidget->model()->treeModel()->removeDocument( m_AnnotationDocument );
    m_AnnotationDocument->clear();
    m_marbleWidget->model()->treeModel()->addDocument( m_AnnotationDocument );
}

void AnnotatePlugin::saveAnnotationFile()
{
    QString filename;
    filename = QFileDialog::getSaveFileName( 0, tr("Save Annotation File"),
                            QString(),
                            tr("All Supported Files (*.kml);;KML file (*.kml)"));

    if ( ! filename.isNull() ) {

        GeoWriter writer;
        //FIXME: a better way to do this?
        writer.setDocumentType( kml::kmlTag_nameSpace22 );

        QFile file( filename );

        // Open file in right mode
        file.open( QIODevice::WriteOnly );

        if ( !writer.write( &file, m_AnnotationDocument ) ) {
            mDebug() << "Could not write the file " << filename;
        }
        file.close();
    }
}

void AnnotatePlugin::loadAnnotationFile()
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
        mDebug() << "File " << filename << " does not exist!";
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    GeoDataParser parser( GeoData_KML );
    if ( !parser.read( &file ) ) {
        mDebug() << "Could not parse file " << filename;
        return;
    }

    GeoDataDocument* document = dynamic_cast<GeoDataDocument*>(parser.releaseDocument() );
    Q_ASSERT( document );

    file.close();

    foreach( GeoDataFeature *feature, document->featureList() ) {
        if( feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
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
    }
    m_marbleWidget->centerOn( document->latLonAltBox() );

    delete document;
    emit repaintNeeded(QRegion());
}

bool AnnotatePlugin::eventFilter(QObject* watched, QEvent* event)
{
    if( !m_widgetInitialized ) {
        MarbleWidget* marbleWidget = qobject_cast<MarbleWidget*>( watched );
        if( marbleWidget ) {
            m_marbleWidget = marbleWidget;
            setupActions( marbleWidget );
            m_marbleWidget->model()->treeModel()->addDocument( m_AnnotationDocument );
            m_widgetInitialized = true;
        }
    }

    if( !m_marbleWidget ) {
        return false;
    }
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
    bool isOnGlobe = m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
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
                    m_selectedItem = 0;
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
        m_marbleWidget->model()->treeModel()->removeFeature( m_tmp_placemark );
        GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( m_tmp_placemark->geometry() );
        poly->outerBoundary().append(GeoDataCoordinates(lon, lat));
        m_marbleWidget->model()->treeModel()->addFeature( m_AnnotationDocument, m_tmp_placemark );
        return true;
    }

    return false;
}

void AnnotatePlugin::setupActions(MarbleWidget* widget)
{
    qDeleteAll( m_actions );
    m_actions.clear();
    m_toolbarActions.clear();

    if( widget ) {
        QActionGroup* group = new QActionGroup(0);
        group->setExclusive( false );

        QActionGroup* nonExclusiveGroup = new QActionGroup(0);
        nonExclusiveGroup->setExclusive( false );


        QAction* enableInputAction = new QAction(this);
        enableInputAction->setToolTip(tr("Enable Moving Map"));
        enableInputAction->setCheckable(true);
        enableInputAction->setChecked( true );
        enableInputAction->setIcon( QIcon( ":/icons/hand.png") );
        connect( enableInputAction, SIGNAL(toggled(bool)),
                 widget, SLOT(setInputEnabled(bool)) );

        QAction* addPlacemark= new QAction(this);
        addPlacemark->setToolTip( tr("Add Placemark") );
        addPlacemark->setCheckable( true );
        addPlacemark->setIcon( QIcon( ":/icons/draw-placemark.png") );
        connect( addPlacemark, SIGNAL(toggled(bool)),
                 this, SLOT(setAddingPlacemark(bool)) );
        connect( this, SIGNAL(placemarkAdded()) ,
                 addPlacemark, SLOT(toggle()) );

        QAction* drawPolygon = new QAction( this );
        drawPolygon->setToolTip( tr("Add Polygon") );
        drawPolygon->setCheckable( true );
        drawPolygon->setIcon( QIcon( ":/icons/draw-polygon.png") );
        connect( drawPolygon, SIGNAL(toggled(bool)),
                 this, SLOT(setDrawingPolygon(bool)) );

        QAction* removeItem = new QAction( this );
        removeItem->setToolTip( tr("Remove Item") );
        removeItem->setCheckable( true );
        removeItem->setIcon( QIcon( ":/icons/edit-delete-shred.png") );
        connect( removeItem, SIGNAL(toggled(bool)),
                 this, SLOT(setRemovingItems(bool)) );
        connect( this, SIGNAL(itemRemoved()),
                 removeItem, SLOT(toggle()) );

        QAction* loadAnnotationFile = new QAction( this );
        loadAnnotationFile->setToolTip( tr("Load Annotation File" ) );
        loadAnnotationFile->setIcon( QIcon( ":/icons/document-import.png") );
        connect( loadAnnotationFile, SIGNAL(triggered()),
                 this, SLOT(loadAnnotationFile()) );

        QAction* saveAnnotationFile = new QAction( this );
        saveAnnotationFile->setToolTip( tr("Save Annotation File") );
        saveAnnotationFile->setIcon( QIcon( ":/icons/document-export.png") );
        connect( saveAnnotationFile, SIGNAL(triggered()),
                 this, SLOT(saveAnnotationFile()) );

        QAction* clearAnnotations = new QAction( this );
        clearAnnotations->setToolTip( tr("Clear all Annotations") );
        clearAnnotations->setIcon( QIcon( ":/icons/remove.png") );
        connect( clearAnnotations, SIGNAL(triggered()),
                 this, SLOT(clearAnnotations()) );

        QAction* beginSeparator = new QAction( this );
        beginSeparator->setSeparator( true );
        QAction* endSeparator = new QAction ( this );
        endSeparator->setSeparator( true );


//        QAction* downloadOsm = new QAction( this );
//        downloadOsm->setText( tr("Download Osm File") );
//        downloadOsm->setToolTip(tr("Download Osm File for selected area"));
//        connect( downloadOsm, SIGNAL(triggered()),
//                 this, SLOT(downloadOsmFile()) );


        group->addAction( enableInputAction );
        group->addAction( beginSeparator );
        group->addAction( addPlacemark );
        group->addAction( drawPolygon );
        group->addAction( removeItem );
        group->addAction( loadAnnotationFile );
        group->addAction( saveAnnotationFile );
        group->addAction( clearAnnotations );
        group->addAction( endSeparator );

//        nonExclusiveGroup->addAction( downloadOsm );

        m_actions.append( group );
        m_actions.append( nonExclusiveGroup );

        m_toolbarActions.append( group );
        m_toolbarActions.append( nonExclusiveGroup );
    }

    emit actionGroupsChanged();
}

//void AnnotatePlugin::readOsmFile( QIODevice *device, bool flyToFile )
//{
//}

}

Q_EXPORT_PLUGIN2( AnnotatePlugin, Marble::AnnotatePlugin )

#include "AnnotatePlugin.moc"
