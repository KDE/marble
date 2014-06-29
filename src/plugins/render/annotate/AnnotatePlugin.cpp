//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009       Andrew Manson           <g.real.ate@gmail.com>
// Copyright 2013       Thibaut Gridel          <tgridel@free.fr>
// Copyright 2014       Calin-Cristian Cruceru  <crucerucalincristian@gmail.com>
//

// Self
#include "AnnotatePlugin.h"

// Marble
#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "EditGroundOverlayDialog.h"
#include "EditPolygonDialog.h"
#include "GeoDataDocument.h"
#include "GeoDataGroundOverlay.h"
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
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetInputHandler.h"
#include "PlacemarkTextAnnotation.h"
#include "TextureLayer.h"
#include "SceneGraphicsTypes.h"

// Qt
#include <QFileDialog>
#include <QAction>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QMessageBox>
#include <QPair>


namespace Marble
{

AnnotatePlugin::AnnotatePlugin( const MarbleModel *model )
    : RenderPlugin( model ),
      m_widgetInitialized( false ),
      m_marbleWidget( 0 ),
      m_overlayRmbMenu( new QMenu( m_marbleWidget ) ),
      m_polygonRmbMenu( new QMenu( m_marbleWidget ) ),
      m_nodeRmbMenu( new QMenu( m_marbleWidget ) ),
      m_annotationDocument( new GeoDataDocument ),
      m_polygonPlacemark( 0 ),
      m_movedItem( 0 ),
      m_addingPlacemark( false ),
      m_drawingPolygon( false ),
      m_addingPolygonHole( false ),
      m_removingItem( false ),
      // m_networkAccessManager( 0 ),
      m_isInitialized( false )
{
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );
    connect( this, SIGNAL(visibilityChanged(bool, QString)), SLOT(enableModel(bool)) );

    m_annotationDocument->setName( tr("Annotations") );
    m_annotationDocument->setDocumentRole( UserDocument );

    GeoDataStyle style;
    GeoDataPolyStyle polyStyle;

    polyStyle.setColor( QColor( 0, 255, 255, 80 ) );
    style.setId( "polygon" );
    style.setPolyStyle( polyStyle );
    m_annotationDocument->addStyle( style );
}

AnnotatePlugin::~AnnotatePlugin()
{
    if ( m_marbleWidget ) {
        m_marbleWidget->model()->treeModel()->removeDocument( m_annotationDocument );
    }

    delete m_annotationDocument;
    // delete m_networkAccessManager;
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
    return QStringList() << "ALWAYS_ON_TOP";
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

QIcon AnnotatePlugin::icon() const
{
    return QIcon( ":/icons/draw-placemark.png");
}


void AnnotatePlugin::initialize()
{
    if ( !m_isInitialized ) {
        m_widgetInitialized = false;

        delete m_polygonPlacemark;
        m_polygonPlacemark = 0;

        delete m_movedItem;
        m_movedItem = 0;

        m_addingPlacemark = false;
        m_drawingPolygon = false;
        m_removingItem = false;
        m_isInitialized = true;
    }
}

bool AnnotatePlugin::isInitialized() const
{
    return m_isInitialized;
}

QString AnnotatePlugin::runtimeTrace() const
{
    return QString("Annotate Items: %1").arg( m_annotationDocument->size() );
}

const QList<QActionGroup*> *AnnotatePlugin::actionGroups() const
{
    return &m_actions;
}

const QList<QActionGroup*> *AnnotatePlugin::toolbarActionGroups() const
{
    return &m_toolbarActions;
}

bool AnnotatePlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    QListIterator<SceneGraphicsItem*> iter( m_graphicsItems );
    while ( iter.hasNext() ) {
        iter.next()->paint( painter, viewport );
    }

    return true;
}

void AnnotatePlugin::enableModel( bool enabled )
{
    if ( enabled ) {
        if ( m_marbleWidget ) {
            setupActions( m_marbleWidget );
            m_marbleWidget->model()->treeModel()->addDocument( m_annotationDocument );
        }
    } else {
        setupActions( 0 );
        if ( m_marbleWidget ) {
            m_marbleWidget->model()->treeModel()->removeDocument( m_annotationDocument );
        }
    }
}

void AnnotatePlugin::setAddingPlacemark( bool enabled )
{
    m_addingPlacemark = enabled ;
}

void AnnotatePlugin::setDrawingPolygon( bool enabled )
{
    m_drawingPolygon = enabled;
    if ( enabled ) {
        m_polygonPlacemark = new GeoDataPlacemark;

        GeoDataPolygon *polygon = new GeoDataPolygon( Tessellate );
        polygon->outerBoundary().setTessellate( true );
        m_polygonPlacemark->setGeometry( polygon );

        m_polygonPlacemark->setParent( m_annotationDocument );
        m_polygonPlacemark->setStyleUrl( "#polygon" );
        m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, m_polygonPlacemark );
    } else {
        const GeoDataPolygon *poly = dynamic_cast<const GeoDataPolygon*>( m_polygonPlacemark->geometry() );
        Q_ASSERT( poly );

        if ( !poly->outerBoundary().isEmpty() ) {
            AreaAnnotation *area = new AreaAnnotation( m_polygonPlacemark );
            m_graphicsItems.append( area );
            m_marbleWidget->update();
        } else {
            m_marbleWidget->model()->treeModel()->removeFeature( m_polygonPlacemark );
            delete m_polygonPlacemark;
        }
        m_polygonPlacemark = 0;
    }
}

void AnnotatePlugin::setAddingPolygonHole( bool enabled )
{
    if ( !enabled && m_holedPolygon &&
         !m_holedPolygon->innerBoundaries().isEmpty() &&
         m_holedPolygon->innerBoundaries().last().size() <= 2 ) {
        m_holedPolygon->innerBoundaries().last().clear();
    }

    m_addingPolygonHole = enabled;
    m_holedPolygon = 0;

    emit repaintNeeded( QRegion() );
}

void AnnotatePlugin::setAddingOverlay( bool enabled )
{
	m_addingOverlay = enabled;
}

void AnnotatePlugin::setMergingNodes( bool enabled )
{
    if ( !enabled && m_mergedArea ) {
        // Restore the normal state.
        m_mergedArea->setState( AreaAnnotation::Normal );
    }

    m_mergingNodes = enabled;
    m_mergedArea = 0;
}

void AnnotatePlugin::setRemovingItems( bool enabled )
{
    m_removingItem = enabled;
}

void AnnotatePlugin::addOverlay()
{
	if ( !m_addingOverlay ) {
		return;
	}

	GeoDataGroundOverlay *overlay = new GeoDataGroundOverlay();
	EditGroundOverlayDialog *dialog = new EditGroundOverlayDialog( overlay, m_marbleWidget->textureLayer(), m_marbleWidget );
	dialog->exec();

	m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, overlay );

	emit overlayAdded();
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

    const int result = QMessageBox::question( m_marbleWidget,
                                              QObject::tr( "Clear all annotations" ),
                                              QObject::tr( "Are you sure you want to clear all annotations?" ),
                                              QMessageBox::Yes | QMessageBox::Cancel );

    if ( result == QMessageBox::Yes ) {
        // It gets deleted three lines further, when calling qDeleteAll().
        m_movedItem = 0;
        delete m_polygonPlacemark;
        m_polygonPlacemark = 0;

        qDeleteAll( m_graphicsItems );
        m_graphicsItems.clear();
        m_marbleWidget->model()->treeModel()->removeDocument( m_annotationDocument );
        m_annotationDocument->clear();
        m_marbleWidget->model()->treeModel()->addDocument( m_annotationDocument );
    }
}

void AnnotatePlugin::saveAnnotationFile()
{
    QString const filename = QFileDialog::getSaveFileName( 0, tr("Save Annotation File"),
                                 QString(), tr("All Supported Files (*.kml);;KML file (*.kml)"));
    if ( !filename.isNull() ) {
        GeoWriter writer;
        //FIXME: a better way to do this?
        writer.setDocumentType( kml::kmlTag_nameSpaceOgc22 );
        QFile file( filename );
        file.open( QIODevice::WriteOnly );
        if ( !writer.write( &file, m_annotationDocument ) ) {
            mDebug() << "Could not write the file " << filename;
        }
        file.close();
    }
}

void AnnotatePlugin::loadAnnotationFile()
{
    QString const filename = QFileDialog::getOpenFileName(0, tr("Open Annotation File"),
                     QString(), tr("All Supported Files (*.kml);;Kml Annotation file (*.kml)"));

    if ( filename.isNull() ) {
        return;
    }

    QFile file( filename );
    if ( !file.exists() ) {
        mDebug() << "File " << filename << " does not exist!";
        return;
    }

    file.open( QIODevice::ReadOnly );
    GeoDataParser parser( GeoData_KML );
    if ( !parser.read( &file ) ) {
        mDebug() << "Could not parse file " << filename;
        return;
    }

    GeoDataDocument *document = dynamic_cast<GeoDataDocument*>( parser.releaseDocument() );
    Q_ASSERT( document );
    file.close();

    foreach ( GeoDataFeature *feature, document->featureList() ) {
        if ( feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( feature );

            if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType ) {
                GeoDataPlacemark *newPlacemark = new GeoDataPlacemark( *placemark );
                PlacemarkTextAnnotation *annotation = new PlacemarkTextAnnotation( newPlacemark );
                m_graphicsItems.append( annotation );
                m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, newPlacemark );
            } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
                GeoDataPlacemark *newPlacemark = new GeoDataPlacemark( *placemark );
                newPlacemark->setParent( m_annotationDocument );
                newPlacemark->setStyleUrl( placemark->styleUrl() );
                AreaAnnotation *annotation = new AreaAnnotation( newPlacemark );
                m_graphicsItems.append( annotation );
                m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, newPlacemark );
            }
        }
    }
    m_marbleWidget->centerOn( document->latLonAltBox() );

    delete document;
    emit repaintNeeded( QRegion() );
}

bool AnnotatePlugin::eventFilter(QObject *watched, QEvent *event)
{
    if ( !m_widgetInitialized ) {
        MarbleWidget *marbleWidget = qobject_cast<MarbleWidget*>( watched );
        if ( marbleWidget ) {
            m_marbleWidget = marbleWidget;

            setupGroundOverlayModel();
            setupOverlayRmbMenu();
            setupPolygonRmbMenu();
            setupNodeRmbMenu();
            setupActions( marbleWidget );

            m_marbleWidget->model()->treeModel()->addDocument( m_annotationDocument );
            m_widgetInitialized = true;

            return true;
        }
        return false;
    }

    // So far only accept mouse events.
    if ( event->type() != QEvent::MouseButtonPress &&
         event->type() != QEvent::MouseButtonRelease &&
         event->type() != QEvent::MouseMove ) {
        return false;
    }

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>( event );
    Q_ASSERT( mouseEvent );

    // Get the geocoordinates from mouse pos screen coordinates.
    qreal lon, lat;
    bool isOnGlobe = m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                                     mouseEvent->pos().y(),
                                                     lon, lat,
                                                     GeoDataCoordinates::Radian );
    if ( !isOnGlobe ) {
        if ( m_movedItem ) {
            m_movedItem = 0;
            return true;
        }
        return false;
    }

    // Deal with adding placemarks and polygons;
    if ( ( m_addingPlacemark && handleAddingPlacemark( mouseEvent ) ) ||
         ( m_drawingPolygon && handleAddingPolygon( mouseEvent ) ) ) {
        return true;
    }

    // It is important to deal with Ground Overlay mouse release event here because it uses the
    // texture layer in order to make the rendering more efficient.
    if ( mouseEvent->type() == QEvent::MouseButtonRelease && m_groundOverlayModel.rowCount() ) {
        handleReleaseOverlay( mouseEvent );
    }

    // It is important to deal with the MouseMove event here because it changes the state of the selected
    // item irrespective of the longitude/latitude the cursor moved to (excepting when it is outside the
    // globe, which is treated above).
    if ( mouseEvent->type() == QEvent::MouseMove && m_movedItem &&
         handleMovingSelectedItem( mouseEvent ) ) {
        return true;
    }


    // Pass the event to Graphic Items.
    foreach ( SceneGraphicsItem *item, m_graphicsItems ) {
        QListIterator<QRegion> it( item->regions() );

        while ( it.hasNext() ) {
            QRegion region = it.next();
            if ( !region.contains( mouseEvent->pos() ) )
                continue;

            // The flow here is as it follows: first check if there is anything we may want to do
            // before the item itself handles the event. For example, we don't want the item to handle
            // the event when having selected "Removing item"; instead, we want to remove the item in
            // this situation; the same applies for adding polygon holes, merging nodes and showing rmb
            // menus so far. Then, if there is nothing to do before the item handles the event, let it
            // handle the event.
            if ( ( m_removingItem && handleRemovingItem( mouseEvent, item ) ) ||

                 ( m_addingPolygonHole && handleAddingHole( mouseEvent, item ) ) ||

                 ( m_mergingNodes && handleMergingNodes( mouseEvent, item ) ) ||

                 ( handleShowingRmbMenus( mouseEvent, item ) ) ||

                 ( mouseEvent->type() == QEvent::MouseButtonPress &&
                   handleMousePressEvent( mouseEvent, item ) ) ||

                 ( mouseEvent->type() == QEvent::MouseButtonRelease &&
                   handleMouseReleaseEvent( mouseEvent, item ) ) ) {
                return true;
            }
        }
    }

    // If the event gets here, it most probably means it is a map interaction event, or something
    // that has nothing to do with the annotate plugin items. We "deal" with this situation because,
    // for example, we may need to deselect some selected items.
    handleUncaughtEvents( mouseEvent );

    return false;
}

bool AnnotatePlugin::handleAddingPlacemark( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    qreal lon, lat;
    m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                    mouseEvent->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );
    const GeoDataCoordinates coords( lon, lat );


    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setCoordinate( coords );
    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, placemark );

    PlacemarkTextAnnotation *textAnnotation = new PlacemarkTextAnnotation( placemark );
    m_graphicsItems.append( textAnnotation );

    emit placemarkAdded();
    return true;
}

bool AnnotatePlugin::handleAddingPolygon( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() != Qt::LeftButton ||
         mouseEvent->type() != QEvent::MouseButtonPress ) {
        return false;
    }

    qreal lon, lat;
    m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                    mouseEvent->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );
    const GeoDataCoordinates coords( lon, lat );


    m_marbleWidget->model()->treeModel()->removeFeature( m_polygonPlacemark );
    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( m_polygonPlacemark->geometry() );
    poly->outerBoundary().append( coords );
    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, m_polygonPlacemark );

    return true;
}

void AnnotatePlugin::handleReleaseOverlay( QMouseEvent *mouseEvent )
{
    qreal lon, lat;
    m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                    mouseEvent->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );
    const GeoDataCoordinates coords( lon, lat );

    // Events caught by ground overlays at mouse release. So far we have: displaying the overlay frame
    // (marking it as selected), removing it and showing a rmb menu with options.
    for ( int i = 0; i < m_groundOverlayModel.rowCount(); ++i ) {
        QModelIndex index = m_groundOverlayModel.index( i, 0 );
        GeoDataGroundOverlay *overlay = dynamic_cast<GeoDataGroundOverlay*>(
           qvariant_cast<GeoDataObject*>( index.data( MarblePlacemarkModel::ObjectPointerRole ) ) );

        if ( overlay->latLonBox().contains( coords ) ) {
            if ( mouseEvent->button() == Qt::LeftButton ) {
                if ( m_removingItem ) {
                    m_marbleWidget->model()->treeModel()->removeFeature( overlay );
                    emit itemRemoved();
                } else {
                    displayOverlayFrame( overlay );
                }
            } else if ( mouseEvent->button() == Qt::RightButton ) {
                showOverlayRmbMenu( overlay, mouseEvent->x(), mouseEvent->y() );
            }
        }
    }
}

bool AnnotatePlugin::handleMovingSelectedItem( QMouseEvent *mouseEvent )
{
    // Handling easily the mouse move by calling for each scene graphic item their own mouseMoveEvent
    // handler and updating the placemark geometry.
    if ( m_movedItem->sceneEvent( mouseEvent ) ) {
        m_marbleWidget->model()->treeModel()->updateFeature( m_movedItem->placemark() );
        return true;
    }

    return false;
}

bool AnnotatePlugin::handleMousePressEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    // Return false if the item's mouse press event handler returns false.
    if ( !item->sceneEvent( mouseEvent ) ) {
        return false;
    }

    // The item gets selected at each mouse press event.
    m_movedItem = item;

    // For ground overlays, if the current item is not contained in m_groundOverlayFrames, clear
    // all frames, which means the overlay gets deselected on each "extern" click.
    if ( !m_groundOverlayFrames.values().contains( item ) ) {
        clearOverlayFrames();
    }

    m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );
    return true;
}

bool AnnotatePlugin::handleMouseReleaseEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    // Return false if the mouse release event handler of the item returns false.
    if ( !item->sceneEvent( mouseEvent ) ) {
        return false;
    }

    // The moved item gets deselected at mouse release event.
    m_movedItem = 0;

    m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );
    return true;
}

bool AnnotatePlugin::handleRemovingItem( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    if ( mouseEvent->type() != QEvent::MouseButtonPress ||
         mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    const int result = QMessageBox::question( m_marbleWidget,
                                              QObject::tr( "Remove current item" ),
                                              QObject::tr( "Are you sure you want to remove the current item?" ),
                                              QMessageBox::Yes | QMessageBox::No );

    if ( result == QMessageBox::Yes ) {
        m_movedItem = 0;
        m_graphicsItems.removeAll( item );
        m_marbleWidget->model()->treeModel()->removeFeature( item->feature() );
        delete item->feature();
        delete item;
        emit itemRemoved();
    }

    return true;
}

bool AnnotatePlugin::handleAddingHole( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    // Ignore if it is not a LMB press or if someone by mistake clicks another scene graphic
    // element while having checked "Add Polygon Hole".
    if ( mouseEvent->type() != QEvent::MouseButtonPress ||
         mouseEvent->button() != Qt::LeftButton ||
         item->graphicType() != SceneGraphicTypes::SceneGraphicAreaAnnotation ) {
        return false;
    }

    qreal lon, lat;
    m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                    mouseEvent->pos().y(),
                                    lon, lat,
                                    GeoDataCoordinates::Radian );
    const GeoDataCoordinates coords( lon, lat );

    // We can now be sure that the scene graphic item is an area annotation and its
    // geometry is a polygon.
    AreaAnnotation *area = static_cast<AreaAnnotation*>( item );
    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( item->placemark()->geometry() );
    Q_ASSERT( area );
    Q_ASSERT( poly );

    // If it is the first click in the interior of a polygon and the event position is not
    // a hole of the polygon, we initialize the polygon on which we want to draw a hole.
    if ( !m_holedPolygon && !area->isInnerBoundsPoint( mouseEvent->pos() ) ) {
        m_holedPolygon = poly;
        poly->innerBoundaries().append( GeoDataLinearRing( Tessellate ) );
    } else if ( m_holedPolygon != poly || area->isInnerBoundsPoint( mouseEvent->pos() ) ) {
        // Ignore clicks on other polygons if the polygon has already been initialized or
        // if the event position is contained by one of the polygon's holes.
        return false;
    }
    m_holedPolygon->innerBoundaries().last().append( coords );

    m_marbleWidget->model()->treeModel()->updateFeature( area->placemark() );
    return true;
}

bool AnnotatePlugin::handleMergingNodes( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    if ( mouseEvent->type() != QEvent::MouseButtonPress ||
         mouseEvent->button() != Qt::LeftButton ||
         item->graphicType() != SceneGraphicTypes::SceneGraphicAreaAnnotation ) {
        return false;
    }

    // We can now be sure that the scene graphic item is an area annotation and its
    // geometry is a polygon.
    AreaAnnotation *area = static_cast<AreaAnnotation*>( item );
    Q_ASSERT( area );

    if ( m_mergedArea != area && !area->isInnerBoundsPoint( mouseEvent->pos(), true ) ) {
        // If the polygon has been initialized but it is different than the previously selected one,
        // change the state of the older one to Normal and to MergingNodes to the new one. This makes
        // possible merging nodes in different polygons without unchecking and checking again "Merge
        // Nodes".
        if ( m_mergedArea ) {
            m_mergedArea->setState( AreaAnnotation::Normal );
        }
        area->setState( AreaAnnotation::MergingNodes );
        m_mergedArea = area;
    } else if ( area->isInnerBoundsPoint( mouseEvent->pos(), true ) ) {
        // Ignore clicks on polygon's inner boundaries.
        return false;
    }

    // We can now be sure that mousePressEvent in AreaAnnotation will return true.
    Q_ASSERT( area->sceneEvent( mouseEvent) );
    QPair<int, int> &mergedNodes = area->mergedNodes();

    // Ignore event if the first node to be merged is set to -1. This means that actually no node
    // has been clicked (so far this happens when clicking the interior of the polygon).
    if ( mergedNodes.first == -1 ) {
        return false;
    }

    // If only one node has been clicked wait for the second one to be clicked.
    if ( mergedNodes.second == -1 ) {
        return true;
    } else if ( mergedNodes.second == mergedNodes.first ) {
        // Do not allow merging a node with itself. Without dealing with this case, the node got
        // removed and maybe this is not one might want.
        area->setMergedNodes( QPair<int, int>(-1, -1) );
        return true;
    }

    // Keep the two nodes sorted.
    if ( mergedNodes.first > mergedNodes.second ) {
        qSwap<int>( mergedNodes.first, mergedNodes.second );
    }

    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( area->placemark()->geometry() );
    Q_ASSERT( poly );

    // Store the initial inner boundaries and outer boundary in case the polygon will have an invalid
    // shape after merging two nodes.
    GeoDataLinearRing initialOuterBoundary = poly->outerBoundary();
    QVector<GeoDataLinearRing> initialInnerBoundaries = poly->innerBoundaries();

    GeoDataLinearRing &outer = poly->outerBoundary();
    if ( ( mergedNodes.first >= outer.size() && mergedNodes.second < outer.size() ) ||
         ( mergedNodes.first < outer.size() && mergedNodes.second >= outer.size() ) ) {
        QMessageBox::warning( m_marbleWidget,
                              QString( "Operation not permitted"),
                              QString( "Cannot merge a node from polygon's outer boundary with"
                                       " a node from one of its inner boundaries!" ) );
        area->setMergedNodes( QPair<int, int>(-1, -1) );
        // Made the user aware of the impossibility of merging those nodes and return true so that
        // the event do not propagate.
        return true;
    }

    QList<int> &selectedNodes = m_mergedArea->selectedNodes();
    int sizeOffset = 0;

    // If the selected nodes are part of one of the polygon's inner boundary.
    if ( mergedNodes.first - outer.size() >= 0 && mergedNodes.second - outer.size() >= 0 ) {
        QVector<GeoDataLinearRing> &inners = poly->innerBoundaries();

        mergedNodes.first -= outer.size();
        mergedNodes.second -= outer.size();
        sizeOffset += outer.size();

        // The merging is done by removing the first selected node and changing the coordinates
        // of the second one.
        for ( int i = 0; i < inners.size(); ++i ) {
            if ( mergedNodes.first - inners.at(i).size() < 0 &&
                 mergedNodes.second - inners.at(i).size() < 0 ) {
                inners[i].at(mergedNodes.second) = inners.at(i).at(mergedNodes.second).interpolate(
                                                        inners.at(i).at(mergedNodes.first), 0.5 );
                inners[i].remove( mergedNodes.first );
                // If this inner boundary has only two remaining nodes, remove it all.
                if ( inners.at(i).size() <= 2 ) {
                    inners[i].clear();

                    // Remove any of these three nodes (including the earlier removed one) from the
                    // selectedNodes list.
                    selectedNodes.removeAll( sizeOffset );
                    selectedNodes.removeAll( sizeOffset + 1 );
                    selectedNodes.removeAll( sizeOffset + 2 );

                    // Decrement the indexes of selected nodes from other inner boundaries which have
                    // been drawn after this one.
                    QList<int>::iterator itBegin = selectedNodes.begin();
                    QList<int>::const_iterator itEnd = selectedNodes.constEnd();

                    for ( ; itBegin != itEnd; ++itBegin ) {
                        if ( *itBegin > sizeOffset + 2 ) {
                            *itBegin -= 3;
                        }
                    }

                    area->setMergedNodes( QPair<int, int>(-1, -1) );
                    return true;
                }

                break;
            } else if ( mergedNodes.first - inners.at(i).size() < 0 ||
                        mergedNodes.second - inners.at(i).size() < 0 ) {
                // Even though they are set to (-1, -1) below, before the warning takes the focus, the
                // paint methods are called so we make sure the correct nodes are painted, so that the
                // user knows which nodes he tried to merge.
                mergedNodes.first += sizeOffset;
                mergedNodes.second += sizeOffset;

                QMessageBox::warning( m_marbleWidget,
                                      QString( "Operation not permitted"),
                                      QString( "Cannot merge two nodes from two different"
                                               " inner boundaries!") );
                area->setMergedNodes( QPair<int, int>(-1, -1) );
                return true;
            } else {
                mergedNodes.first -= inners.at(i).size();
                mergedNodes.second -= inners.at(i).size();
                sizeOffset += inners.at(i).size();
            }
        }
    } else {
        outer.at(mergedNodes.second) = outer.at(mergedNodes.second).interpolate(
                                            outer.at(mergedNodes.first), 0.5 );
        outer.remove( mergedNodes.first );

        // If the polygon's outer boundary has only two nodes remained after merging, remove it all.
        if ( outer.size() <= 2 ) {
            m_graphicsItems.removeAll( area );
            m_marbleWidget->model()->treeModel()->removeFeature( area->feature() );
            delete area->feature();
            delete area;

            return true;
        }

        // When merging two nodes from polygon's outer boundary, check if the polygon is still valid.
        if ( !area->isValidPolygon() ) {
            poly->outerBoundary() = initialOuterBoundary;
            poly->innerBoundaries() = initialInnerBoundaries;

            QMessageBox::warning( m_marbleWidget,
                                  QString( "Operation not permitted"),
                                  QString( "The polygon would have an invalid shape after this"
                                           " operation (e.g. its outerboundary would not contain"
                                           " all its inner boundaries).") );
            area->setMergedNodes( QPair<int, int>(-1, -1) );
            return true;
        }
    }


    // Reconstruct clicked nodes to resemble the indexes stored in selectedNodes list.
    mergedNodes.first += sizeOffset;
    mergedNodes.second += sizeOffset;

    // When having one of the two merged nodes marked as selected, the resulting node will also be
    // selected.
    if ( selectedNodes.contains( mergedNodes.first ) || selectedNodes.contains( mergedNodes.second ) ) {
        selectedNodes.removeAll( mergedNodes.first );
        selectedNodes.removeAll( mergedNodes.second );
        selectedNodes.append( mergedNodes.second );
    }

    QList<int>::iterator itBegin = selectedNodes.begin();
    QList<int>::const_iterator itEnd = selectedNodes.constEnd();

    // Decrement the indexes of the selected nodes which have bigger indexes than the
    // node with a smaller index.
    for ( ; itBegin != itEnd; ++itBegin ) {
        if ( *itBegin > mergedNodes.first ) {
            (*itBegin)--;
        }
    }

    area->setMergedNodes( QPair<int, int>(-1, -1) );
    return true;
}

bool AnnotatePlugin::handleShowingRmbMenus( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    // We get here when mousePressEvent returns false.
    if ( item->graphicType() != SceneGraphicTypes::SceneGraphicAreaAnnotation ||
         mouseEvent->type() != QEvent::MouseButtonPress ||
         mouseEvent->button() != Qt::RightButton ) {
        return false;
    }

    AreaAnnotation *area = static_cast<AreaAnnotation*>( item );
    Q_ASSERT( area );

    // Call AreaAnnotation::mousePressEvent in order to get the node index which has been
    // right-clicked.
    area->sceneEvent( mouseEvent );

    if ( area->rightClickedNode() == -1 ) {
        showPolygonRmbMenu( area, mouseEvent->x(), mouseEvent->y() );
    } else if ( area->rightClickedNode() >= 0 ){
        showNodeRmbMenu( area, mouseEvent->x(), mouseEvent->y() );
    } else {
        // If the region clicked is the interior of an innerBoundary of a polygon,
        // we pass the event handling further. This guarantees that the events are
        // caught by imbricated polygons irrespective of their number (e.g. we can
        // have polygon within polygon within polygon, etc ).
        Q_ASSERT( area->isInnerBoundsPoint( mouseEvent->pos() ) );
        return false;
    }

    m_marbleWidget->model()->treeModel()->updateFeature( area->placemark() );
    return true;
}

void AnnotatePlugin::handleUncaughtEvents( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );

    // If the event is not caught by any of the annotate plugin specific items, clear the frames
    // (which have the meaning of deselecting the overlay).
    if ( !m_groundOverlayFrames.isEmpty() &&
         mouseEvent->type() != QEvent::MouseMove && mouseEvent->type() != QEvent::MouseButtonRelease ) {
        clearOverlayFrames();
    }
}

void AnnotatePlugin::setupActions(MarbleWidget *widget)
{
    qDeleteAll( m_actions );
    m_actions.clear();
    m_toolbarActions.clear();

    if ( widget ) {
        QActionGroup *group = new QActionGroup( 0 );
        group->setExclusive( false );

        // QActionGroup *nonExclusiveGroup = new QActionGroup(0);
        // nonExclusiveGroup->setExclusive( false );


        QAction *enableInputAction = new QAction( this );
        enableInputAction->setText( tr("Enable Moving Map") );
        enableInputAction->setCheckable( true );
        enableInputAction->setChecked( true );
        enableInputAction->setIcon( QIcon( ":/icons/hand.png") );
        connect( enableInputAction, SIGNAL(toggled(bool)),
                 widget, SLOT(setInputEnabled(bool)) );

        QAction *drawPolygon = new QAction( this );
        drawPolygon->setText( tr("Add Polygon") );
        drawPolygon->setCheckable( true );
        drawPolygon->setIcon( QIcon( ":/icons/draw-polygon.png") );
        connect( drawPolygon, SIGNAL(toggled(bool)),
                 this, SLOT(setDrawingPolygon(bool)) );

        QAction *addHole = new QAction( this );
        addHole->setText( tr("Add Polygon Hole") );
        // TODO: set icon
        addHole->setCheckable( true );
        connect( addHole, SIGNAL(toggled(bool)),
                 this, SLOT(setAddingPolygonHole(bool)) );

        QAction *mergeNodes = new QAction( this );
        mergeNodes->setText( tr("Merge Nodes") );
        // TODO: est icon
        mergeNodes->setCheckable( true );
        connect( mergeNodes, SIGNAL(toggled(bool)),
                 this, SLOT(setMergingNodes(bool)) );

        QAction *addPlacemark= new QAction( this );
        addPlacemark->setText( tr("Add Placemark") );
        addPlacemark->setCheckable( true );
        addPlacemark->setIcon( QIcon( ":/icons/draw-placemark.png") );
        connect( addPlacemark, SIGNAL(toggled(bool)),
                 this, SLOT(setAddingPlacemark(bool)) );
        connect( this, SIGNAL(placemarkAdded()) ,
                 addPlacemark, SLOT(toggle()) );

        QAction *addOverlay = new QAction( this );
        addOverlay->setText( tr("Add Ground Overlay") );
        addOverlay->setCheckable( true );
        addOverlay->setIcon( QIcon( ":/icons/draw-overlay.png") );
        connect( addOverlay, SIGNAL(toggled(bool)),
                 this, SLOT(setAddingOverlay(bool)) );
        connect( addOverlay, SIGNAL(toggled(bool)),
                 this, SLOT(addOverlay()) );
        connect( this, SIGNAL(overlayAdded()),
                 addOverlay, SLOT(toggle()) );

        QAction *removeItem = new QAction( this );
        removeItem->setText( tr("Remove Item") );
        removeItem->setCheckable( true );
        removeItem->setIcon( QIcon( ":/icons/edit-delete-shred.png") );
        connect( removeItem, SIGNAL(toggled(bool)),
                 this, SLOT(setRemovingItems(bool)) );
        connect( this, SIGNAL(itemRemoved()),
                 removeItem, SLOT(toggle()) );

        QAction *loadAnnotationFile = new QAction( this );
        loadAnnotationFile->setText( tr("Load Annotation File" ) );
        loadAnnotationFile->setIcon( QIcon( ":/icons/document-import.png") );
        connect( loadAnnotationFile, SIGNAL(triggered()),
                 this, SLOT(loadAnnotationFile()) );

        QAction *saveAnnotationFile = new QAction( this );
        saveAnnotationFile->setText( tr("Save Annotation File") );
        saveAnnotationFile->setIcon( QIcon( ":/icons/document-export.png") );
        connect( saveAnnotationFile, SIGNAL(triggered()),
                 this, SLOT(saveAnnotationFile()) );

        QAction *clearAnnotations = new QAction( this );
        clearAnnotations->setText( tr("Clear all Annotations") );
        clearAnnotations->setIcon( QIcon( ":/icons/remove.png") );
        connect( drawPolygon, SIGNAL(toggled(bool)),
                 clearAnnotations, SLOT(setDisabled(bool)) );
        connect( clearAnnotations, SIGNAL(triggered()),
                 this, SLOT(clearAnnotations()) );

        QAction *beginSeparator = new QAction( this );
        beginSeparator->setSeparator( true );
        QAction *polygonEndSeparator = new QAction( this );
        polygonEndSeparator->setSeparator( true );
        QAction *removeItemBeginSeparator = new QAction( this );
        removeItemBeginSeparator->setSeparator( true );
        QAction *removeItemEndSeparator = new QAction( this );
        removeItemEndSeparator->setSeparator( true );
        QAction *endSeparator = new QAction ( this );
        endSeparator->setSeparator( true );


        // QAction* downloadOsm = new QAction( this );
        // downloadOsm->setText( tr("Download Osm File") );
        // downloadOsm->setToolTip(tr("Download Osm File for selected area"));
        // connect( downloadOsm, SIGNAL(triggered()),
        //          this, SLOT(downloadOsmFile()) );


        group->addAction( enableInputAction );
        group->addAction( beginSeparator );
        group->addAction( drawPolygon );
        group->addAction( addHole );
        group->addAction( mergeNodes );
        group->addAction( polygonEndSeparator );
        group->addAction( addPlacemark );
        group->addAction( addOverlay );
        group->addAction( removeItemBeginSeparator );
        group->addAction( removeItem );
        group->addAction( removeItemEndSeparator );
        group->addAction( loadAnnotationFile );
        group->addAction( saveAnnotationFile );
        group->addAction( clearAnnotations );
        group->addAction( endSeparator );

        // nonExclusiveGroup->addAction( downloadOsm );

        m_actions.append( group );
        // m_actions.append( nonExclusiveGroup );

        m_toolbarActions.append( group );
        // m_toolbarActions.append( nonExclusiveGroup );
    }

    emit actionGroupsChanged();
}

void AnnotatePlugin::setupGroundOverlayModel()
{
    m_groundOverlayModel.setSourceModel( m_marbleWidget->model()->groundOverlayModel() );
    m_groundOverlayModel.setDynamicSortFilter( true );
    m_groundOverlayModel.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    m_groundOverlayModel.sort( 0, Qt::AscendingOrder );
}

void AnnotatePlugin::setupOverlayRmbMenu()
{
    QAction *removeOverlay = new QAction( tr( "Remove Ground Overlay" ), m_overlayRmbMenu );
    QAction *editOverlay = new QAction( tr( "Edit Ground Overlay" ), m_overlayRmbMenu );

    m_overlayRmbMenu->addAction( editOverlay );
    m_overlayRmbMenu->addAction( removeOverlay );

    connect( editOverlay, SIGNAL(triggered()), this, SLOT(editOverlay()) );
    connect( removeOverlay, SIGNAL(triggered()), this, SLOT(removeOverlay()) );
}

void AnnotatePlugin::showOverlayRmbMenu( GeoDataGroundOverlay *overlay, qreal x, qreal y )
{
    m_rmbOverlay = overlay;
    m_overlayRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}

void AnnotatePlugin::editOverlay()
{
    displayOverlayFrame( m_rmbOverlay );
    displayOverlayEditDialog( m_rmbOverlay );
}

void AnnotatePlugin::removeOverlay()
{
    m_marbleWidget->model()->treeModel()->removeFeature( m_rmbOverlay );
    clearOverlayFrames();
}

void AnnotatePlugin::displayOverlayFrame( GeoDataGroundOverlay *overlay )
{
    if ( !m_groundOverlayFrames.keys().contains( overlay ) ) {

        GeoDataPlacemark *rectangle_placemark = new GeoDataPlacemark;
        rectangle_placemark->setGeometry( new GeoDataPolygon );
        rectangle_placemark->setParent( m_annotationDocument );
        rectangle_placemark->setStyleUrl( "#polygon" );

        m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, rectangle_placemark );

        GroundOverlayFrame *frame = new GroundOverlayFrame( rectangle_placemark, overlay, m_marbleWidget->textureLayer() );
        m_graphicsItems.append( frame );
        m_groundOverlayFrames.insert( overlay, frame );
    }
}

void AnnotatePlugin::displayOverlayEditDialog( GeoDataGroundOverlay *overlay )
{
    QPointer<EditGroundOverlayDialog> dialog = new EditGroundOverlayDialog(
                                                        overlay,
                                                        m_marbleWidget->textureLayer(),
                                                        m_marbleWidget );

    connect( dialog, SIGNAL(groundOverlayUpdated(GeoDataGroundOverlay*)),
             this, SLOT(updateOverlayFrame(GeoDataGroundOverlay*)) );

    dialog->exec();
    delete dialog;
}


void AnnotatePlugin::updateOverlayFrame( GeoDataGroundOverlay *overlay )
{
    GroundOverlayFrame *frame = static_cast<GroundOverlayFrame *>( m_groundOverlayFrames.value( overlay ) );
    if ( frame ) {
        frame->update();
    }
}

void AnnotatePlugin::clearOverlayFrames()
{

    foreach ( GeoDataGroundOverlay *overlay, m_groundOverlayFrames.keys() ) {
        GroundOverlayFrame *frame = static_cast<GroundOverlayFrame *>( m_groundOverlayFrames.value( overlay ) );
        m_graphicsItems.removeAll( m_groundOverlayFrames.value( overlay ) );
        m_marbleWidget->model()->treeModel()->removeFeature( frame->placemark() );
        delete frame->placemark();
        delete frame;
    }

    m_groundOverlayFrames.clear();
}


void AnnotatePlugin::setupPolygonRmbMenu()
{
    QAction *unselectNodes = new QAction( tr( "Deselect All Nodes" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( unselectNodes );
    connect( unselectNodes, SIGNAL(triggered()), this, SLOT(unselectNodes()) );

    QAction *deleteAllSelected = new QAction( tr( "Delete All Selected Nodes" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( deleteAllSelected );
    connect( deleteAllSelected, SIGNAL(triggered()), this, SLOT(deleteSelectedNodes()) );

    QAction *removePolygon = new QAction( tr( "Remove Polygon" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( removePolygon );
    connect( removePolygon, SIGNAL(triggered()), this, SLOT(removePolygon()) );

    m_polygonRmbMenu->addSeparator();

    QAction *showEditDialog = new QAction( tr( "Properties" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( showEditDialog );
    connect( showEditDialog, SIGNAL(triggered()), this, SLOT(editPolygon()) );
}


void AnnotatePlugin::showPolygonRmbMenu( AreaAnnotation *selectedArea, qreal x, qreal y )
{
    m_rmbSelectedArea = selectedArea;

    if ( selectedArea->selectedNodes().isEmpty() ) {
        m_polygonRmbMenu->actions().at(1)->setEnabled( false );
        m_polygonRmbMenu->actions().at(0)->setEnabled( false );
    } else {
        m_polygonRmbMenu->actions().at(1)->setEnabled( true );
        m_polygonRmbMenu->actions().at(0)->setEnabled( true );
    }

    m_polygonRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}


void AnnotatePlugin::unselectNodes()
{
    m_rmbSelectedArea->selectedNodes().clear();
}

void AnnotatePlugin::deleteSelectedNodes()
{
    QList<int> &selectedNodes = m_rmbSelectedArea->selectedNodes();

    // If there are no selected nodes, exit the function.
    if ( !selectedNodes.size() ) {
        return;
    }

    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( m_rmbSelectedArea->placemark()->geometry() );
    // Copy the current polygon's inner boundaries and outer boundary, to be able to recover them
    // if the deletion fails (the polygon becomes invalid after deleting the selected nodes).
    QVector<GeoDataLinearRing> innerBounds = poly->innerBoundaries();
    GeoDataLinearRing outerBound = poly->outerBoundary();

    // Sorting and iterating through the list of selected nodes backwards is important because when
    // caling {outerBoundary,innerBoundary[i]}.remove, the indexes of the selected nodes bigger than
    // the removed one's (in one step of the iteration) should be all decremented due to the way
    // QVector::remove works.
    // Sorting and iterating backwards is, therefore, faster than iterating through the list and at
    // each iteration, iterate one more time through the list in order to decrement the above
    // mentioned nodes (O(N * logN) < O(N ^ 2) in terms of complexity).
    qSort( selectedNodes.begin(), selectedNodes.end() );

    QListIterator<int> it( selectedNodes );
    it.toBack();

    // Deal with removing the selected nodes from the polygon's inner boundaries.
    while ( it.hasPrevious() ) {
        int nodeIndex = it.previous();

        if ( nodeIndex < poly->outerBoundary().size() ) {
            it.next();
            break;
        }

        nodeIndex -= poly->outerBoundary().size();
        for ( int i = 0; i < poly->innerBoundaries().size(); ++i ) {
            if ( nodeIndex - poly->innerBoundaries().at(i).size() < 0 ) {
                poly->innerBoundaries()[i].remove( nodeIndex );
                break;
            } else {
                nodeIndex -= poly->innerBoundaries().at(i).size();
            }
        }
    }
    // If one of the polygon's inner boundaries has 0, 1 or 2 nodes remained after
    // removing the selected ones, remove this entire inner boundary.
    for ( int i = 0; i < poly->innerBoundaries().size(); ++i ) {
        if ( poly->innerBoundaries().at(i).size() <= 2 ) {
            poly->innerBoundaries()[i].clear();
        }
    }



    // Deal with removing the selected nodes from the polygon's outer boundaries.
    while ( it.hasPrevious() ) {
        poly->outerBoundary().remove( it.previous() );
    }

    // If the number of nodes remained after deleting the selected ones is 0, 1 or 2, we remove the
    // entire polygon.
    if ( poly->outerBoundary().size() <= 2 ) {
        selectedNodes.clear();

        m_graphicsItems.removeAll( m_rmbSelectedArea );
        m_marbleWidget->model()->treeModel()->removeFeature( m_rmbSelectedArea->feature() );
        delete m_rmbSelectedArea->feature();
        delete m_rmbSelectedArea;

        return;
    }

    // If the polygon is no longer valid (e.g. its outer boundary ring itersects one of its inner
    // boundaries ring), recover it to the last valid shape and popup a warning.
    if ( !m_rmbSelectedArea->isValidPolygon() ) {
        poly->innerBoundaries() = innerBounds;
        poly->outerBoundary() = outerBound;

        QMessageBox::warning( m_marbleWidget,
                              QString( "Operation not permitted" ),
                              QString( "Cannot delete the selected nodes" ) );
    } else {
        selectedNodes.clear();
    }
}

void AnnotatePlugin::removePolygon()
{
    m_graphicsItems.removeAll( m_rmbSelectedArea );
    m_marbleWidget->model()->treeModel()->removeFeature( m_rmbSelectedArea->feature() );
    delete m_rmbSelectedArea->feature();
    delete m_rmbSelectedArea;
}

void AnnotatePlugin::editPolygon()
{
    displayPolygonEditDialog( m_rmbSelectedArea->placemark() );
}

void AnnotatePlugin::displayPolygonEditDialog( GeoDataPlacemark *placemark )
{
    EditPolygonDialog *dialog = new EditPolygonDialog( placemark, m_marbleWidget );

    connect( dialog, SIGNAL(polygonUpdated(GeoDataFeature*)),
             this, SIGNAL(repaintNeeded()) );
    connect( dialog, SIGNAL(polygonUpdated(GeoDataFeature*)),
             m_marbleWidget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );

    dialog->show();
}

void AnnotatePlugin::setupNodeRmbMenu()
{
    QAction *selectNode = new QAction( tr( "Select Node" ), m_nodeRmbMenu );
    QAction *deleteNode = new QAction( tr( "Delete Node" ), m_nodeRmbMenu );

    m_nodeRmbMenu->addAction( selectNode );
    m_nodeRmbMenu->addAction( deleteNode );

    connect( selectNode, SIGNAL(triggered()), this, SLOT(selectNode()) );
    connect( deleteNode, SIGNAL(triggered()), this, SLOT(deleteNode()) );
}

void AnnotatePlugin::showNodeRmbMenu( AreaAnnotation *area, qreal x, qreal y )
{
    // Check whether the node is already selected; we change the text of the
    // action accordingly.
    if ( area->selectedNodes().contains( area->rightClickedNode() ) ) {
        m_nodeRmbMenu->actions().at(0)->setText( tr("Deselect Node") );
    } else {
        m_nodeRmbMenu->actions().at(0)->setText( tr("Select Node") );
    }

    m_rmbSelectedArea = area;
    m_nodeRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}

void AnnotatePlugin::selectNode()
{
    if ( m_rmbSelectedArea->selectedNodes().contains(  m_rmbSelectedArea->rightClickedNode() ) ) {
        m_rmbSelectedArea->selectedNodes().removeAll( m_rmbSelectedArea->rightClickedNode() );
    } else {
        m_rmbSelectedArea->selectedNodes().append( m_rmbSelectedArea->rightClickedNode() );
    }
}


void AnnotatePlugin::deleteNode()
{
    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( m_rmbSelectedArea->placemark()->geometry() );

    // Copy the current polygon's inner boundaries and outer boundary, to be able to recover them
    // if the deletion fails (the polygon becomes invalid after deleting the selected nodes).
    QVector<GeoDataLinearRing> innerBounds = poly->innerBoundaries();
    GeoDataLinearRing outerBound = poly->outerBoundary();

    int index = m_rmbSelectedArea->rightClickedNode();

    // If the right clicked node is one of those nodes which form one of the inner boundaries
    // of the polygon.
    if ( index - poly->outerBoundary().size() >= 0 ) {
        QVector<GeoDataLinearRing> &innerRings = poly->innerBoundaries();

        index -= poly->outerBoundary().size();
        for ( int i = 0; i < innerRings.size(); ++i ) {
            // If we've found the inner boundary it is a part of, we remove it; also, if the
            // linear ring has only 2 nodes remained after deletion, we remove the entire
            // inner boundary.
            if ( index - innerRings.at(i).size() < 0 ) {
                innerRings[i].remove( index );
                if ( innerRings.at(i).size() <= 2 ) {
                    innerRings[i].clear();
                }

                break;
            } else {
                index -= innerRings.at(i).size();
            }
        }
    } else {
        poly->outerBoundary().remove( index );

        // If the polygon has only 2 nodes, we remove it all.
        if ( poly->outerBoundary().size() <= 2 ) {
             m_rmbSelectedArea->selectedNodes().clear();

            m_graphicsItems.removeAll( m_rmbSelectedArea );
            m_marbleWidget->model()->treeModel()->removeFeature( m_rmbSelectedArea->feature() );
            delete m_rmbSelectedArea->feature();
            delete m_rmbSelectedArea;

            return;
        }
    }

    // If the polygon is no longer valid (e.g. its outer boundary ring itersects one of its inner
    // boundaries ring), recover it to the last valid shape and popup a warning.
    if ( !m_rmbSelectedArea->isValidPolygon() ) {
        poly->innerBoundaries() = innerBounds;
        poly->outerBoundary() = outerBound;

        QMessageBox::warning( m_marbleWidget,
                              QString( "Operation not permitted"),
                              QString( "Cannot delete the selected node" ) );

        return;
    }

    // If the node is selected, remove it from the selected list of nodes as well.
    m_rmbSelectedArea->selectedNodes().removeAll( m_rmbSelectedArea->rightClickedNode() );

    QList<int>::iterator itBegin = m_rmbSelectedArea->selectedNodes().begin();
    QList<int>::const_iterator itEnd = m_rmbSelectedArea->selectedNodes().constEnd();

    // Decrement the indexes of the selected nodes which have bigger indexes than the
    // removed one's.
    for ( ; itBegin != itEnd; ++itBegin ) {
        if ( *itBegin > m_rmbSelectedArea->rightClickedNode() ) {
            (*itBegin)--;
        }
    }
}

//void AnnotatePlugin::readOsmFile( QIODevice *device, bool flyToFile )
//{
//}

}

Q_EXPORT_PLUGIN2( AnnotatePlugin, Marble::AnnotatePlugin )

#include "AnnotatePlugin.moc"
