//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009       Andrew Manson  <g.real.ate@gmail.com>
// Copyright 2013       Thibaut Gridel <tgridel@free.fr>
// Copyright 2014       Calin Cruceru  <crucerucalincristian@gmail.com>
//

// Self
#include "AnnotatePlugin.h"

// Qt
#include <QFileDialog>
#include <QAction>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QColor>

// Marble
#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "EditGroundOverlayDialog.h"
#include "EditTextAnnotationDialog.h"
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
#include "AreaAnnotation.h"
#include "PlacemarkTextAnnotation.h"
#include "TextureLayer.h"
#include "SceneGraphicsTypes.h"
#include "MergingPolygonNodesAnimation.h"
#include "MergingPolylineNodesAnimation.h"
#include "MarbleWidgetPopupMenu.h"
#include "PolylineAnnotation.h"
#include "EditPolylineDialog.h"
#include "ParsingRunnerManager.h"


namespace Marble
{

AnnotatePlugin::AnnotatePlugin( const MarbleModel *model )
    : RenderPlugin( model ),
      m_widgetInitialized( false ),
      m_marbleWidget( 0 ),
      m_overlayRmbMenu( new QMenu( m_marbleWidget ) ),
      m_polygonRmbMenu( new QMenu( m_marbleWidget ) ),
      m_nodeRmbMenu( new QMenu( m_marbleWidget ) ),
      m_textAnnotationRmbMenu( new QMenu( m_marbleWidget ) ),
      m_polylineRmbMenu( new QMenu( m_marbleWidget ) ),
      m_annotationDocument( new GeoDataDocument ),
      m_movedItem( 0 ),
      m_focusItem( 0 ),
      m_polylinePlacemark( 0 ),
      m_polygonPlacemark( 0 ),
      m_clipboardItem( 0 ),
      m_drawingPolygon( false ),
      m_drawingPolyline( false ),
      m_isInitialized( false ),
      m_editingDialogIsShown( false )
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
    qDeleteAll( m_graphicsItems );
    if ( m_marbleWidget ) {
        m_marbleWidget->model()->treeModel()->removeDocument( m_annotationDocument );
    }

    delete m_overlayRmbMenu;
    delete m_polygonRmbMenu;
    delete m_nodeRmbMenu;
    delete m_textAnnotationRmbMenu;
    delete m_polylineRmbMenu;

    delete m_annotationDocument;
    // delete m_networkAccessManager;

    delete m_clipboardItem;
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
            << PluginAuthor( "Thibaut Gridel", "<tgridel@free.fr>" )
            << PluginAuthor( "Calin Cruceru", "<crucerucalincristian@gmail.com>" );
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

        m_drawingPolygon = false;
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

void AnnotatePlugin::setAddingPolygonHole( bool enabled )
{
    if ( enabled ) {
        announceStateChanged( SceneGraphicsItem::AddingPolygonHole );
    } else {
        announceStateChanged( SceneGraphicsItem::Editing );
    }
}

void AnnotatePlugin::setMergingNodes( bool enabled )
{
    if ( enabled ) {
        announceStateChanged( SceneGraphicsItem::MergingNodes );
    } else {
        announceStateChanged( SceneGraphicsItem::Editing );
    }
}

void AnnotatePlugin::setAddingNodes( bool enabled )
{
    if ( enabled ) {
        announceStateChanged( SceneGraphicsItem::AddingNodes );
    } else {
        announceStateChanged( SceneGraphicsItem::Editing );
    }
}

void AnnotatePlugin::setAreaAvailable( AreaAnnotation *targetedArea )
{
    targetedArea->setBusy( false );
    emit repaintNeeded();
}

void AnnotatePlugin::setPolylineAvailable( PolylineAnnotation *targetedPolyline )
{
    targetedPolyline->setBusy( false );
    emit repaintNeeded();
}

void AnnotatePlugin::askToRemoveFocusItem()
{
    const int result = QMessageBox::question( m_marbleWidget,
                                              QObject::tr( "Remove current item" ),
                                              QObject::tr( "Are you sure you want to remove the current item?" ),
                                              QMessageBox::Yes | QMessageBox::No );
    if ( result == QMessageBox::Yes ) {
        removeFocusItem();
    }
}

void AnnotatePlugin::removeFocusItem()
{
    // Ground Overlays will always be a special case..
    if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicGroundOverlay ) {
        for ( int i = 0; i < m_groundOverlayModel.rowCount(); ++i ) {
            const QModelIndex index = m_groundOverlayModel.index( i, 0 );
            GeoDataGroundOverlay *overlay = dynamic_cast<GeoDataGroundOverlay*>(
               qvariant_cast<GeoDataObject*>( index.data( MarblePlacemarkModel::ObjectPointerRole ) ) );

            m_marbleWidget->model()->treeModel()->removeFeature( overlay );
        }

        clearOverlayFrames();
    } else {
        disableFocusActions();

        m_graphicsItems.removeAll( m_focusItem );
        m_marbleWidget->model()->treeModel()->removeFeature( m_focusItem->feature() );

        delete m_focusItem->feature();
        delete m_focusItem;
        m_movedItem = 0;
        m_focusItem = 0;
    }
}

void AnnotatePlugin::clearAnnotations()
{

    const int result = QMessageBox::question( m_marbleWidget,
                                              QObject::tr( "Clear all annotations" ),
                                              QObject::tr( "Are you sure you want to clear all annotations?" ),
                                              QMessageBox::Yes | QMessageBox::Cancel );

    if ( result == QMessageBox::Yes ) {
        m_movedItem = 0;

        delete m_polygonPlacemark;
        m_polygonPlacemark = 0;
        delete m_polylinePlacemark;
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
    const QString filename = QFileDialog::getSaveFileName( 0,
                                                           tr("Save Annotation File"),
                                                           QString(),
                                                           tr("All Supported Files (*.kml *.osm);;"
                                                              "KML file (*.kml);;"
                                                              "Open Street Map file (*.osm)") );
    if ( !filename.isNull() ) {
        GeoWriter writer;
        // FIXME: This should be consistent with the way the loading is done.
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
    const QString filename = QFileDialog::getOpenFileName( 0,
                                                           tr("Open Annotation File"),
                                                           QString(),
                                                           tr("All Supported Files (*.kml *.osm);;"
                                                              "Kml Annotation file (*.kml);;"
                                                              "Open Street Map file (*.osm)") );
    if ( filename.isNull() ) {
        return;
    }

    ParsingRunnerManager manager( m_marbleWidget->model()->pluginManager() );
    GeoDataDocument *document = manager.openFile( filename );
    Q_ASSERT( document );

    // FIXME: The same problem as in the case of copying/cutting graphic items applies here:
    // the files do not load properly because the geometry copy is not a deep copy.
    foreach ( GeoDataFeature *feature, document->featureList() ) {
        if ( feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( feature );

            GeoDataPlacemark *newPlacemark = new GeoDataPlacemark( *placemark );
            if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType ) {
                PlacemarkTextAnnotation *placemark = new PlacemarkTextAnnotation( newPlacemark );
                m_graphicsItems.append( placemark );
            } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
                newPlacemark->setParent( m_annotationDocument );
                newPlacemark->setStyleUrl( placemark->styleUrl() );
                AreaAnnotation *polygonAnnotation = new AreaAnnotation( newPlacemark );
                m_graphicsItems.append( polygonAnnotation );
            } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
                newPlacemark->setParent( m_annotationDocument );
                newPlacemark->setStyleUrl( placemark->styleUrl() );
                PolylineAnnotation *polylineAnnotation = new PolylineAnnotation( newPlacemark );
                m_graphicsItems.append( polylineAnnotation );
            }
            m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, newPlacemark );
        }
    }
    m_marbleWidget->centerOn( document->latLonAltBox() );

    delete document;
    emit repaintNeeded( QRegion() );
}

bool AnnotatePlugin::eventFilter( QObject *watched, QEvent *event )
{
    if ( !m_widgetInitialized ) {
        MarbleWidget *marbleWidget = qobject_cast<MarbleWidget*>( watched );

        if ( marbleWidget ) {
            m_marbleWidget = marbleWidget;

            addContextItems();
            setupGroundOverlayModel();
            setupOverlayRmbMenu();
            setupPolygonRmbMenu();
            setupPolylineRmbMenu();
            setupNodeRmbMenu();
            setupTextAnnotationRmbMenu();
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

    QMouseEvent * const mouseEvent = dynamic_cast<QMouseEvent*>( event );
    Q_ASSERT( mouseEvent );

    // Get the geocoordinates from mouse pos screen coordinates.
    qreal lon, lat;
    const bool isOnGlobe = m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                                           mouseEvent->pos().y(),
                                                           lon, lat,
                                                           GeoDataCoordinates::Radian );
    if ( !isOnGlobe ) {
        return false;
    }

    // Deal with adding polygons and polylines.
    if ( ( m_drawingPolygon && handleDrawingPolygon( mouseEvent ) ) ||
         ( m_drawingPolyline && handleDrawingPolyline( mouseEvent ) ) ) {
        return true;
    }

    // It is important to deal with Ground Overlay mouse release event here because it uses the
    // texture layer in order to make the rendering more efficient.
    if ( mouseEvent->type() == QEvent::MouseButtonRelease && m_groundOverlayModel.rowCount() ) {
        handleReleaseOverlay( mouseEvent );
    }

    // It is important to deal with the MouseMove event here because it changes the state of the
    // selected item irrespective of the longitude/latitude the cursor moved to (excepting when
    // it is outside the globe, which is treated above).
    if ( mouseEvent->type() == QEvent::MouseMove && m_movedItem &&
         handleMovingSelectedItem( mouseEvent ) ) {
        setupCursor( m_movedItem );
        return true;
    }

    // Pass the event to Graphic Items.
    foreach ( SceneGraphicsItem *item, m_graphicsItems ) {
        if ( !item->containsPoint( mouseEvent->pos() ) ) {
            continue;
        }

        if ( m_editingDialogIsShown && mouseEvent->type() == QEvent::MouseButtonPress &&
             mouseEvent->button() == Qt::RightButton) {
            return true;
        }

        if ( !item->hasFocus() &&
             item->graphicType() != SceneGraphicsTypes::SceneGraphicGroundOverlay ) {
            if ( mouseEvent->type() == QEvent::MouseButtonPress &&
                 mouseEvent->button() == Qt::LeftButton ) {
                item->setFocus( true );
                enableActionsOnItemType( item->graphicType() );

                if ( m_focusItem && m_focusItem != item ) {
                    m_focusItem->setFocus( false );
                }
                m_focusItem = item;
                m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );
                return true;
            }

            return false;
        }

        if ( item->sceneEvent( event ) ) {
            setupCursor( item );

            if ( mouseEvent->type() == QEvent::MouseButtonPress ) {
                handleSuccessfulPressEvent( mouseEvent, item );
            } else if ( mouseEvent->type() == QEvent::MouseMove ) {
                handleSuccessfulHoverEvent( mouseEvent, item );
            } else if ( mouseEvent->type() == QEvent::MouseButtonRelease ) {
                handleSuccessfulReleaseEvent( mouseEvent, item );
            }

            handleRequests( mouseEvent, item );
            return true;
        }
    }

    // If the event gets here, it most probably means it is a map interaction event, or something
    // that has nothing to do with the annotate plugin items. We "deal" with this situation because,
    // for example, we may need to deselect some selected items.
    handleUncaughtEvents( mouseEvent );

    return false;
}

bool AnnotatePlugin::handleDrawingPolygon( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->type() == QEvent::MouseMove ) {
        setupCursor( 0 );
        return true;
    } else if ( mouseEvent->button() == Qt::LeftButton &&
                mouseEvent->type() == QEvent::MouseButtonPress ) {
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

    return false;
}

bool AnnotatePlugin::handleDrawingPolyline( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->type() == QEvent::MouseMove ) {
        setupCursor( 0 );
        return true;
    } else if ( mouseEvent->button() == Qt::LeftButton &&
                mouseEvent->type() == QEvent::MouseButtonPress ) {
        qreal lon, lat;
        m_marbleWidget->geoCoordinates( mouseEvent->pos().x(),
                                        mouseEvent->pos().y(),
                                        lon, lat,
                                        GeoDataCoordinates::Radian );
        const GeoDataCoordinates coords( lon, lat );

        m_marbleWidget->model()->treeModel()->removeFeature( m_polylinePlacemark );
        GeoDataLineString *line = dynamic_cast<GeoDataLineString*>( m_polylinePlacemark->geometry() );
        line->append( coords );
        m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, m_polylinePlacemark );

        return true;
    }

    return false;
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
        const QModelIndex index = m_groundOverlayModel.index( i, 0 );
        GeoDataGroundOverlay *overlay = dynamic_cast<GeoDataGroundOverlay*>(
           qvariant_cast<GeoDataObject*>( index.data( MarblePlacemarkModel::ObjectPointerRole ) ) );

        if ( overlay->latLonBox().contains( coords ) ) {
            if ( mouseEvent->button() == Qt::LeftButton ) {
                displayOverlayFrame( overlay );
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

        if ( m_movedItem->graphicType() == SceneGraphicsTypes::SceneGraphicTextAnnotation ) {
            emit placemarkMoved();
        }

        return true;
    }

    return false;
}

void AnnotatePlugin::handleSuccessfulPressEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    Q_UNUSED( mouseEvent );

    // Update the item's placemark.
    m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );

    // Store a pointer to the item for possible following move events only if its state is
    // either 'Editing' or 'AddingNodes' and the the mouse left button has been used.
    if ( ( item->state() == SceneGraphicsItem::Editing ||
           item->state() == SceneGraphicsItem::AddingNodes ) &&
         mouseEvent->button() == Qt::LeftButton ) {
        m_movedItem = item;
    }
}

void AnnotatePlugin::handleSuccessfulHoverEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    Q_UNUSED( mouseEvent );
    m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );
}

void AnnotatePlugin::handleSuccessfulReleaseEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    Q_UNUSED( mouseEvent );
    // The item gets 'deselected' (from moving) at mouse release.
    m_movedItem = 0;

    // Update the item's placemark.
    m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );
}

void AnnotatePlugin::handleRequests( QMouseEvent *mouseEvent, SceneGraphicsItem *item )
{
    if ( item->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation ) {
        AreaAnnotation * const area = static_cast<AreaAnnotation*>( item );

        if ( area->request() == SceneGraphicsItem::ShowPolygonRmbMenu ) {
            showPolygonRmbMenu( mouseEvent->pos().x(), mouseEvent->pos().y() );
        } else if ( area->request() == SceneGraphicsItem::ShowNodeRmbMenu ) {
            showNodeRmbMenu( mouseEvent->pos().x(), mouseEvent->pos().y() );
        } else if ( area->request() == SceneGraphicsItem::StartPolygonAnimation ) {
            QPointer<MergingPolygonNodesAnimation> animation = area->animation();

            connect( animation, SIGNAL(nodesMoved()), this, SIGNAL(repaintNeeded()) );
            connect( animation, SIGNAL(animationFinished(AreaAnnotation*)),
                     this, SLOT(setAreaAvailable(AreaAnnotation*)) );

            area->setBusy( true );
            animation->startAnimation();
      } else if ( area->request() == SceneGraphicsItem::OuterInnerMergingWarning ) {
            QMessageBox::warning( m_marbleWidget,
                                  tr( "Operation not permitted" ),
                                  tr( "Cannot merge a node from polygon's outer boundary "
                                      "with a node from one of its inner boundaries." ) );
        } else if ( area->request() == SceneGraphicsItem::InnerInnerMergingWarning ) {
            QMessageBox::warning( m_marbleWidget,
                                  tr( "Operation not permitted" ),
                                  tr( "Cannot merge two nodes from two different inner "
                                      "boundaries." ) );
        } else if ( area->request() == SceneGraphicsItem::InvalidShapeWarning ) {
            QMessageBox::warning( m_marbleWidget,
                                  tr( "Operation not permitted" ),
                                  tr( "Cannot merge the selected nodes. Most probably "
                                      "this would make the polygon's outer boundary not "
                                      "contain all its inner boundary nodes." ) );
        } else if ( area->request() == SceneGraphicsItem::RemovePolygonRequest ) {
            removeFocusItem();
        }
    } else if ( item->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation ) {
        PolylineAnnotation * const polyline = static_cast<PolylineAnnotation*>( item );

        if ( polyline->request() == SceneGraphicsItem::ShowPolylineRmbMenu ) {
            showPolylineRmbMenu( mouseEvent->x(), mouseEvent->y() );
        } else if ( polyline->request() == SceneGraphicsItem::ShowNodeRmbMenu ) {
            showNodeRmbMenu( mouseEvent->x(), mouseEvent->y() );
        } else if ( polyline->request() == SceneGraphicsItem::StartPolylineAnimation ) {
            QPointer<MergingPolylineNodesAnimation> animation = polyline->animation();

            connect( animation, SIGNAL(nodesMoved()), this, SIGNAL(repaintNeeded()) );
            connect( animation, SIGNAL(animationFinished(PolylineAnnotation*)),
                     this, SLOT(setPolylineAvailable(PolylineAnnotation*)) );

            polyline->setBusy( true );
            animation->startAnimation();
        } else if ( polyline->request() == SceneGraphicsItem::RemovePolylineRequest ) {
            removeFocusItem();
        }
    } else if ( item->graphicType() == SceneGraphicsTypes::SceneGraphicTextAnnotation ) {
        PlacemarkTextAnnotation * const textAnnotation = static_cast<PlacemarkTextAnnotation*>( item );

        if ( textAnnotation->request() == SceneGraphicsItem::ShowPlacemarkRmbMenu ) {
            showTextAnnotationRmbMenu( mouseEvent->x(), mouseEvent->y() );
        }
    }
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

    if ( m_focusItem && m_focusItem->graphicType() != SceneGraphicsTypes::SceneGraphicGroundOverlay ) {
        if ( ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation &&
               !static_cast<AreaAnnotation*>( m_focusItem )->isBusy() ) ||
             ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation &&
               !static_cast<PolylineAnnotation*>( m_focusItem )->isBusy() ) ) {
            m_focusItem->dealWithItemChange( 0 );
            m_marbleWidget->model()->treeModel()->updateFeature( m_focusItem->placemark() );
        }

        if ( mouseEvent->type() == QEvent::MouseButtonPress ) {
            m_focusItem->setFocus( false );
            disableFocusActions();
            m_marbleWidget->model()->treeModel()->updateFeature( m_focusItem->placemark() );
            m_focusItem = 0;
        }
    }
}

void AnnotatePlugin::setupActions( MarbleWidget *widget )
{
    qDeleteAll( m_actions );
    m_actions.clear();
    m_toolbarActions.clear();

    if ( !widget ) {
        return;
    }

    QActionGroup *group = new QActionGroup( 0 );
    group->setExclusive( true );


    QAction *selectItem = new QAction( QIcon( ":/icons/hand.png"),
                                       tr("Select Item"),
                                       this );
    selectItem->setCheckable( true );
    selectItem->setChecked( true );

    QAction *drawPolygon = new QAction( QIcon( ":/icons/draw-polygon.png"),
                                        tr("Add Polygon"),
                                        this );
    connect( drawPolygon, SIGNAL(triggered()), this, SLOT(addPolygon()) );

    QAction *addHole = new QAction( QIcon(":/icons/16x16/add-holes.png"),
                                    tr("Add Polygon Hole"),
                                    this );
    addHole->setCheckable( true );
    addHole->setEnabled( false );
    connect( addHole, SIGNAL(toggled(bool)), this, SLOT(setAddingPolygonHole(bool)) );

    QAction *mergeNodes = new QAction( QIcon(":/icons/16x16/merge-nodes.png"),
                                       tr("Merge Nodes"),
                                       this );
    mergeNodes->setCheckable( true );
    mergeNodes->setEnabled( false );
    connect( mergeNodes, SIGNAL(toggled(bool)), this, SLOT(setMergingNodes(bool)) );

    QAction *addNodes = new QAction( QIcon(":/icons/16x16/add-nodes.png"),
                                     tr("Add Nodes"),
                                     this );
    addNodes->setCheckable( true );
    addNodes->setEnabled( false );
    connect( addNodes, SIGNAL(toggled(bool)), this, SLOT(setAddingNodes(bool)) );

    QAction *addTextAnnotation = new QAction( QIcon( ":/icons/draw-placemark.png"),
                                              tr("Add Placemark"),
                                              this );
    connect( addTextAnnotation, SIGNAL(triggered()), this, SLOT(addTextAnnotation()) );

    QAction *addPath = new QAction( tr("Add Path"), this );
    connect( addPath, SIGNAL(triggered()), this, SLOT(addPolyline()) );

    QAction *addOverlay = new QAction( QIcon( ":/icons/draw-overlay.png"),
                                       tr("Add Ground Overlay"),
                                       this );
    connect( addOverlay, SIGNAL(triggered()), this, SLOT(addOverlay()) );

    QAction *removeItem = new QAction( QIcon( ":/icons/edit-delete-shred.png"),
                                       tr("Remove Item"),
                                       this );
    removeItem->setEnabled( false );
    connect( removeItem, SIGNAL(triggered()), this, SLOT(askToRemoveFocusItem()) );

    QAction *loadAnnotationFile = new QAction( QIcon( ":/icons/document-import.png"),
                                               tr("Load Annotation File" ),
                                               this );
    connect( loadAnnotationFile, SIGNAL(triggered()), this, SLOT(loadAnnotationFile()) );

    QAction *saveAnnotationFile = new QAction( QIcon( ":/icons/document-export.png"),
                                               tr("Save Annotation File"),
                                               this );
    connect( saveAnnotationFile, SIGNAL(triggered()), this, SLOT(saveAnnotationFile()) );

    QAction *clearAnnotations = new QAction( QIcon( ":/icons/remove.png"),
                                             tr("Clear all Annotations"),
                                             this );
    connect( drawPolygon, SIGNAL(toggled(bool)), clearAnnotations, SLOT(setDisabled(bool)) );
    connect( clearAnnotations, SIGNAL(triggered()), this, SLOT(clearAnnotations()) );


    QAction *sep1 = new QAction( this );
    sep1->setSeparator( true );
    QAction *sep2 = new QAction( this );
    sep2->setSeparator( true );
    QAction *sep3 = new QAction( this );
    sep3->setSeparator( true );
    QAction *sep4 = new QAction( this );
    sep4->setSeparator( true );


    group->addAction( loadAnnotationFile );
    group->addAction( saveAnnotationFile );
    group->addAction( sep1 );
    group->addAction( addTextAnnotation );
    group->addAction( drawPolygon );
    group->addAction( addPath );
    group->addAction( addOverlay );
    group->addAction( sep2 );
    group->addAction( selectItem );
    group->addAction( addHole );
    group->addAction( mergeNodes );
    group->addAction( addNodes );
    group->addAction( removeItem );
    group->addAction( sep3 );
    group->addAction( clearAnnotations );
    group->addAction( sep4 );

    m_actions.append( group );
    m_toolbarActions.append( group );

    emit actionGroupsChanged();
}

void AnnotatePlugin::disableActions( QActionGroup *group )
{
    for ( int i = 0; i < group->actions().size(); ++i ) {
        if ( group->actions().at(i)->text() != tr("Select Item") ) {
            group->actions().at(i)->setEnabled( false );
        }
    }
}

void AnnotatePlugin::enableAllActions( QActionGroup *group )
{
    for ( int i = 0; i < group->actions().size(); ++i ) {
        group->actions().at(i)->setEnabled( true );
    }
}

void AnnotatePlugin::enableActionsOnItemType( const QString &type )
{
    if ( type == SceneGraphicsTypes::SceneGraphicAreaAnnotation ||
         type == SceneGraphicsTypes::SceneGraphicPolylineAnnotation ) {
        m_actions.first()->actions().at(9)->setEnabled( true );
        m_actions.first()->actions().at(10)->setEnabled( true );
        m_actions.first()->actions().at(11)->setEnabled( true );
    }

    m_actions.first()->actions().at(12)->setEnabled( true );
}

void AnnotatePlugin::disableFocusActions()
{
    m_actions.first()->actions().at(8)->setChecked( true );

    m_actions.first()->actions().at(9)->setEnabled( false );
    m_actions.first()->actions().at(10)->setEnabled( false );
    m_actions.first()->actions().at(11)->setEnabled( false );
    m_actions.first()->actions().at(12)->setEnabled( false );
}

void AnnotatePlugin::addContextItems()
{
    MarbleWidgetPopupMenu * const menu = m_marbleWidget->popupMenu();

    m_pasteGraphicItem = new QAction( tr( "Paste Graphic Item" ), this );
    m_pasteGraphicItem->setEnabled( false );
    connect( m_pasteGraphicItem, SIGNAL(triggered()), SLOT(pasteItem()) );

    QAction *separator = new QAction( this );
    separator->setSeparator( true );

    if ( ! MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        menu->addAction( Qt::RightButton, m_pasteGraphicItem );
        menu->addAction( Qt::RightButton, separator );
    }
}

void AnnotatePlugin::setupTextAnnotationRmbMenu()
{
    QAction *cutItem = new QAction( tr( "Cut"), m_textAnnotationRmbMenu );
    m_textAnnotationRmbMenu->addAction( cutItem );
    connect( cutItem, SIGNAL(triggered()), this, SLOT(cutItem()) );

    QAction *copyItem = new QAction( tr( "Copy"), m_textAnnotationRmbMenu );
    m_textAnnotationRmbMenu->addAction( copyItem );
    connect( copyItem, SIGNAL(triggered()), this, SLOT(copyItem()) );

    QAction *removeItem = new QAction( tr( "Remove" ), m_textAnnotationRmbMenu );
    m_textAnnotationRmbMenu->addAction( removeItem );
    connect( removeItem, SIGNAL(triggered()), this, SLOT(askToRemoveFocusItem()) );

    m_textAnnotationRmbMenu->addSeparator();

    QAction *properties = new QAction( tr( "Properties" ), m_textAnnotationRmbMenu );
    m_textAnnotationRmbMenu->addAction( properties );
    connect( properties, SIGNAL(triggered()), this, SLOT(editTextAnnotation()) );
}

void AnnotatePlugin::showTextAnnotationRmbMenu( qreal x, qreal y )
{
    m_textAnnotationRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}

void AnnotatePlugin::editTextAnnotation()
{
    QPointer<EditTextAnnotationDialog> dialog = new EditTextAnnotationDialog( m_focusItem->placemark(),
                                                                              m_marbleWidget );
    connect( dialog, SIGNAL(textAnnotationUpdated(GeoDataFeature*)),
             m_marbleWidget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );
    connect( this, SIGNAL(placemarkMoved()),
             dialog, SLOT(updateDialogFields()) );
    connect( dialog, SIGNAL(finished(int)),
             this, SLOT(stopEditingTextAnnotation()) );

    disableActions( m_actions.first() );
    dialog->show();
    m_editingDialogIsShown = true;
}

void AnnotatePlugin::addTextAnnotation()
{
    // Get the normalized coordinates of the focus point. There will be automatically added a new
    // placemark.
    qreal lat = m_marbleWidget->focusPoint().latitude();
    qreal lon = m_marbleWidget->focusPoint().longitude();
    GeoDataCoordinates::normalizeLonLat( lon, lat );

    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setCoordinate( lon, lat );
    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, placemark );

    PlacemarkTextAnnotation *textAnnotation = new PlacemarkTextAnnotation( placemark );
    textAnnotation->setFocus( true );
    m_graphicsItems.append( textAnnotation );

    QPointer<EditTextAnnotationDialog> dialog = new EditTextAnnotationDialog( placemark, m_marbleWidget );
    dialog->setFirstTimeEditing( true );

    connect( dialog, SIGNAL(textAnnotationUpdated(GeoDataFeature*)),
             m_marbleWidget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );
    connect( this, SIGNAL(placemarkMoved()),
             dialog, SLOT(updateDialogFields()) );
    connect( dialog, SIGNAL(removeRequested()),
             this, SLOT(removeFocusItem()) );
    connect( dialog, SIGNAL(finished(int)),
             this, SLOT(stopEditingTextAnnotation()) );

    if ( m_focusItem ) {
        m_focusItem->setFocus( false );
    }
    m_focusItem = textAnnotation;
    disableActions( m_actions.first() );

    dialog->move( m_marbleWidget->mapToGlobal( QPoint( 0, 0 ) ) );
    dialog->show();
}

void AnnotatePlugin::stopEditingTextAnnotation()
{
    m_editingDialogIsShown = false;

    announceStateChanged( SceneGraphicsItem::Editing );
    enableAllActions( m_actions.first() );
    disableFocusActions();
    enableActionsOnItemType( SceneGraphicsTypes::SceneGraphicTextAnnotation );
}

void AnnotatePlugin::setupGroundOverlayModel()
{
    m_editingDialogIsShown = false;
    m_groundOverlayModel.setSourceModel( m_marbleWidget->model()->groundOverlayModel() );
    m_groundOverlayModel.setDynamicSortFilter( true );
    m_groundOverlayModel.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    m_groundOverlayModel.sort( 0, Qt::AscendingOrder );
}

void AnnotatePlugin::setupOverlayRmbMenu()
{
    QAction *editOverlay = new QAction( tr( "Properties" ), m_overlayRmbMenu );
    m_overlayRmbMenu->addAction( editOverlay );
    connect( editOverlay, SIGNAL(triggered()), this, SLOT(editOverlay()) );

    m_overlayRmbMenu->addSeparator();

    QAction *removeOverlay = new QAction( tr( "Remove" ), m_overlayRmbMenu );
    m_overlayRmbMenu->addAction( removeOverlay );
    connect( removeOverlay, SIGNAL(triggered()), this, SLOT(removeOverlay()) );
}

void AnnotatePlugin::addOverlay()
{
    GeoDataGroundOverlay *overlay = new GeoDataGroundOverlay();
    QPointer<EditGroundOverlayDialog> dialog = new EditGroundOverlayDialog(
                                                                 overlay,
                                                                 m_marbleWidget->textureLayer(),
                                                                 m_marbleWidget );
    dialog->exec();
    delete dialog;
    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, overlay );
}

void AnnotatePlugin::showOverlayRmbMenu( GeoDataGroundOverlay *overlay, qreal x, qreal y )
{
    m_rmbOverlay = overlay;
    m_overlayRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}

void AnnotatePlugin::editOverlay()
{
    displayOverlayFrame( m_rmbOverlay );

    QPointer<EditGroundOverlayDialog> dialog = new EditGroundOverlayDialog(
                                                        m_rmbOverlay,
                                                        m_marbleWidget->textureLayer(),
                                                        m_marbleWidget );
    connect( dialog, SIGNAL(groundOverlayUpdated(GeoDataGroundOverlay*)),
             this, SLOT(updateOverlayFrame(GeoDataGroundOverlay*)) );

    dialog->exec();
    delete dialog;
}

void AnnotatePlugin::removeOverlay()
{
    m_marbleWidget->model()->treeModel()->removeFeature( m_rmbOverlay );
    clearOverlayFrames();
}

void AnnotatePlugin::displayOverlayFrame( GeoDataGroundOverlay *overlay )
{
    if ( m_groundOverlayFrames.keys().contains( overlay ) ) {
        return;
    }

    GeoDataPolygon *polygon = new GeoDataPolygon( Tessellate );
    polygon->outerBoundary().setTessellate( true );

    GeoDataPlacemark *rectangle_placemark = new GeoDataPlacemark;
    rectangle_placemark->setGeometry( polygon );
    rectangle_placemark->setParent( m_annotationDocument );
    rectangle_placemark->setStyleUrl( "#polygon" );

    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, rectangle_placemark );

    GroundOverlayFrame *frame = new GroundOverlayFrame( rectangle_placemark,
                                                        overlay,
                                                        m_marbleWidget->textureLayer() );
    m_graphicsItems.append( frame );
    m_groundOverlayFrames.insert( overlay, frame );

    if ( m_focusItem ) {
        m_focusItem->setFocus( false );
    }
    m_focusItem = frame;
    enableActionsOnItemType( SceneGraphicsTypes::SceneGraphicGroundOverlay );
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
    m_focusItem = 0;
    disableFocusActions();
}

void AnnotatePlugin::setupPolygonRmbMenu()
{
    QAction *deselectNodes = new QAction( tr( "Deselect All Nodes" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( deselectNodes );
    connect( deselectNodes, SIGNAL(triggered()), this, SLOT(deselectNodes()) );

    QAction *deleteAllSelected = new QAction( tr( "Delete All Selected Nodes" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( deleteAllSelected );
    connect( deleteAllSelected, SIGNAL(triggered()), this, SLOT(deleteSelectedNodes()) );

    m_polygonRmbMenu->addSeparator();

    QAction *cutPolygon = new QAction( tr( "Cut"), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( cutPolygon );
    connect( cutPolygon, SIGNAL(triggered()), this, SLOT(cutItem()) );

    QAction *copyPolygon = new QAction( tr( "Copy"), m_polygonRmbMenu );
    copyPolygon->setEnabled( false ); // FIXME
    m_polygonRmbMenu->addAction( copyPolygon );
    connect( copyPolygon, SIGNAL(triggered()), this, SLOT(copyItem()) );

    QAction *removePolygon = new QAction( tr( "Remove" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( removePolygon );
    connect( removePolygon, SIGNAL(triggered()), this, SLOT(askToRemoveFocusItem()) );

    m_polygonRmbMenu->addSeparator();

    QAction *showEditDialog = new QAction( tr( "Properties" ), m_polygonRmbMenu );
    m_polygonRmbMenu->addAction( showEditDialog );
    connect( showEditDialog, SIGNAL(triggered()), this, SLOT(editPolygon()) );
}

void AnnotatePlugin::showPolygonRmbMenu( qreal x, qreal y )
{
    // We need to store the coordinates from where the rmb menu is shown so that in case of
    // selecting Copy/Cut, we can move the polygon.
    qreal lon, lat;
    m_marbleWidget->geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Radian );
    m_fromWhereToCopy = GeoDataCoordinates( lon, lat );

    if ( !static_cast<AreaAnnotation*>( m_focusItem )->hasNodesSelected() ) {
        m_polygonRmbMenu->actions().at(1)->setEnabled( false );
        m_polygonRmbMenu->actions().at(0)->setEnabled( false );
    } else {
        m_polygonRmbMenu->actions().at(1)->setEnabled( true );
        m_polygonRmbMenu->actions().at(0)->setEnabled( true );
    }

    m_polygonRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}

void AnnotatePlugin::addPolygon()
{
    m_drawingPolygon = true;

    // Setup the geometry
    GeoDataPolygon *poly = new GeoDataPolygon( Tessellate );
    poly->outerBoundary().setTessellate( true );

    m_polygonPlacemark = new GeoDataPlacemark;
    m_polygonPlacemark->setGeometry( poly );
    m_polygonPlacemark->setParent( m_annotationDocument );
    m_polygonPlacemark->setStyleUrl( "#polygon" );

    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, m_polygonPlacemark );

    AreaAnnotation *polygon = new AreaAnnotation( m_polygonPlacemark );
    polygon->setState( SceneGraphicsItem::DrawingPolygon );
    polygon->setFocus( true );
    m_graphicsItems.append( polygon );
    m_marbleWidget->update();

    QPointer<EditPolygonDialog> dialog = new EditPolygonDialog( m_polygonPlacemark, m_marbleWidget );
    dialog->setFirstTimeEditing( true );

    connect( dialog, SIGNAL(polygonUpdated(GeoDataFeature*)),
             m_marbleWidget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );
    connect( dialog, SIGNAL(removeRequested()),
             this, SLOT(removeFocusItem()) );
    connect( dialog, SIGNAL(finished(int)),
             this, SLOT(stopEditingPolygon()) );

    // If we had 'selected' other graphic item when pressing 'Add Polygon', change the focus of that
    // item.
    if ( m_focusItem ) {
        m_focusItem->setFocus( false );
    }
    m_focusItem = polygon;
    disableActions( m_actions.first() );

    dialog->move( m_marbleWidget->mapToGlobal( QPoint( 0, 0 ) ) );
    dialog->show();
}

void AnnotatePlugin::stopEditingPolygon()
{
    m_editingDialogIsShown = false;
    m_drawingPolygon = false;
    m_drawingPolygon = 0;
    m_polygonPlacemark = 0;

    announceStateChanged( SceneGraphicsItem::Editing );
    enableAllActions( m_actions.first() );
    disableFocusActions();
    enableActionsOnItemType( SceneGraphicsTypes::SceneGraphicAreaAnnotation );
}

void AnnotatePlugin::deselectNodes()
{
    if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation ) {
        AreaAnnotation * const area = static_cast<AreaAnnotation*>( m_focusItem );
        area->deselectAllNodes();

        if ( area->request() == SceneGraphicsItem::NoRequest ) {
            m_marbleWidget->model()->treeModel()->updateFeature( area->placemark() );
        }
    } else if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation ) {
        PolylineAnnotation * const polyline = static_cast<PolylineAnnotation*>( m_focusItem );
        polyline->deselectAllNodes();

        if ( polyline->request() == SceneGraphicsItem::NoRequest ) {
            m_marbleWidget->model()->treeModel()->updateFeature( polyline->placemark() );
        }
    }
}

void AnnotatePlugin::deleteSelectedNodes()
{
    if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation ) {
        AreaAnnotation * const area = static_cast<AreaAnnotation*>( m_focusItem );
        area->deleteAllSelectedNodes();
    } else if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation ) {
        PolylineAnnotation * const polyline = static_cast<PolylineAnnotation*>( m_focusItem );
        polyline->deleteAllSelectedNodes();
    }

    if ( m_focusItem->request() == SceneGraphicsItem::NoRequest ) {
        m_marbleWidget->model()->treeModel()->updateFeature( m_focusItem->placemark() );
    } else if ( m_focusItem->request() == SceneGraphicsItem::RemovePolygonRequest ||
                m_focusItem->request() == SceneGraphicsItem::RemovePolylineRequest ) {
        removeFocusItem();
    } else if ( m_focusItem->request() == SceneGraphicsItem::InvalidShapeWarning ) {
        QMessageBox::warning( m_marbleWidget,
                              tr( "Operation not permitted" ),
                              tr( "Cannot delete one of the selected nodes. Most probably "
                                  "this would make the polygon's outer boundary not "
                                  "contain all its inner boundary nodes." ) );
    }
}

void AnnotatePlugin::editPolygon()
{
    EditPolygonDialog *dialog = new EditPolygonDialog( m_focusItem->placemark(), m_marbleWidget );

    connect( dialog, SIGNAL(polygonUpdated(GeoDataFeature*)),
             m_marbleWidget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );
    connect( dialog, SIGNAL(finished(int)),
             this, SLOT(stopEditingPolygon()) );

    disableActions( m_actions.first() );

    dialog->move( m_marbleWidget->mapToGlobal( QPoint( 0, 0 ) ) );
    dialog->show();
    m_editingDialogIsShown = true;
}

void AnnotatePlugin::setupNodeRmbMenu()
{
    QAction *selectNode = new QAction( tr( "Select Node" ), m_nodeRmbMenu );
    m_nodeRmbMenu->addAction( selectNode );
    connect( selectNode, SIGNAL(triggered()), this, SLOT(selectNode()) );

    QAction *deleteNode = new QAction( tr( "Delete Node" ), m_nodeRmbMenu );
    m_nodeRmbMenu->addAction( deleteNode );
    connect( deleteNode, SIGNAL(triggered()), this, SLOT(deleteNode()) );
}

void AnnotatePlugin::showNodeRmbMenu( qreal x, qreal y )
{
    // Check whether the node is already selected; we change the text of the action
    // accordingly.
    bool isSelected = false;
    if ( ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation &&
           static_cast<AreaAnnotation*>( m_focusItem )->clickedNodeIsSelected() ) ||
         ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation &&
           static_cast<PolylineAnnotation*>( m_focusItem )->clickedNodeIsSelected() ) ) {
        isSelected = true;
    }

    m_nodeRmbMenu->actions().first()->setText( isSelected ? tr("Deselect Node") : tr("Select Node") );
    m_nodeRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}

void AnnotatePlugin::selectNode()
{
    if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation ) {
        AreaAnnotation * const area = static_cast<AreaAnnotation*>( m_focusItem );
        area->changeClickedNodeSelection();
    } else if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation ) {
        PolylineAnnotation *const polyline = static_cast<PolylineAnnotation*>( m_focusItem );
        polyline->changeClickedNodeSelection();
    }

    if ( m_focusItem->request() == SceneGraphicsItem::NoRequest ) {
        m_marbleWidget->model()->treeModel()->updateFeature( m_focusItem->placemark() );
    }
}

void AnnotatePlugin::deleteNode()
{
    if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation ) {
        AreaAnnotation *area = static_cast<AreaAnnotation*>( m_focusItem );
        area->deleteClickedNode();
    } else if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation ) {
        PolylineAnnotation *polyline = static_cast<PolylineAnnotation*>( m_focusItem );
        polyline->deleteClickedNode();
    }

    if ( m_focusItem->request() == SceneGraphicsItem::NoRequest ) {
        m_marbleWidget->model()->treeModel()->updateFeature( m_focusItem->placemark() );
    } else if ( m_focusItem->request() == SceneGraphicsItem::RemovePolygonRequest ||
                m_focusItem->request() == SceneGraphicsItem::RemovePolylineRequest ) {
        removeFocusItem();
    } else if ( m_focusItem->request() == SceneGraphicsItem::InvalidShapeWarning ) {
        QMessageBox::warning( m_marbleWidget,
                              tr( "Operation not permitted" ),
                              tr( "Cannot delete one of the selected nodes. Most probably "
                                  "this would make the polygon's outer boundary not "
                                  "contain all its inner boundary nodes." ) );
    }
}

void AnnotatePlugin::setupPolylineRmbMenu()
{
    QAction *deselectNodes = new QAction( tr( "Deselect All Nodes" ), m_polylineRmbMenu );
    m_polylineRmbMenu->addAction( deselectNodes );
    connect( deselectNodes, SIGNAL(triggered()), this, SLOT(deselectNodes()) );

    QAction *deleteAllSelected = new QAction( tr( "Delete All Selected Nodes" ), m_polylineRmbMenu );
    m_polylineRmbMenu->addAction( deleteAllSelected );
    connect( deleteAllSelected, SIGNAL(triggered()), this, SLOT(deleteSelectedNodes()) );

    m_polylineRmbMenu->addSeparator();

    QAction *cutItem = new QAction( tr( "Cut"), m_polylineRmbMenu );
    m_polylineRmbMenu->addAction( cutItem );
    connect( cutItem, SIGNAL(triggered()), this, SLOT(cutItem()) );

    QAction *copyItem = new QAction( tr( "Copy"), m_polylineRmbMenu );
    copyItem->setEnabled( false ); // FIXME
    m_polylineRmbMenu->addAction( copyItem );
    connect( copyItem, SIGNAL(triggered()), this, SLOT(copyItem()) );

    QAction *removeItem = new QAction( tr( "Remove" ), m_polylineRmbMenu );
    m_polylineRmbMenu->addAction( removeItem );
    connect( removeItem, SIGNAL(triggered()), this, SLOT(askToRemoveFocusItem()) );

    m_polylineRmbMenu->addSeparator();

    QAction *properties = new QAction( tr( "Properties" ), m_polylineRmbMenu );
    m_polylineRmbMenu->addAction( properties );
    connect( properties, SIGNAL(triggered()), this, SLOT(editPolyline()) );
}

void AnnotatePlugin::showPolylineRmbMenu( qreal x, qreal y )
{
    qreal lon, lat;
    m_marbleWidget->geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Radian );
    m_fromWhereToCopy = GeoDataCoordinates( lon, lat );

    if ( !static_cast<PolylineAnnotation*>( m_focusItem )->hasNodesSelected() ) {
        m_polylineRmbMenu->actions().at(1)->setEnabled( false );
        m_polylineRmbMenu->actions().at(0)->setEnabled( false );
    } else {
        m_polylineRmbMenu->actions().at(1)->setEnabled( true );
        m_polylineRmbMenu->actions().at(0)->setEnabled( true );
    }

    m_polylineRmbMenu->popup( m_marbleWidget->mapToGlobal( QPoint( x, y ) ) );
}

void AnnotatePlugin::editPolyline()
{
    QPointer<EditPolylineDialog> dialog = new EditPolylineDialog( m_focusItem->placemark(),
                                                                  m_marbleWidget );
    connect( dialog, SIGNAL(polylineUpdated(GeoDataFeature*)),
             m_marbleWidget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );
    connect( dialog, SIGNAL(finished(int)),
             this, SLOT(stopEditingPolyline()) );

    disableActions( m_actions.first() );
    dialog->show();
    m_editingDialogIsShown = true;
}

void AnnotatePlugin::addPolyline()
{
    m_drawingPolyline = true;

    m_polylinePlacemark = new GeoDataPlacemark;
    m_polylinePlacemark->setGeometry( new GeoDataLineString( Tessellate ) );
    m_polylinePlacemark->setParent( m_annotationDocument );
    m_polylinePlacemark->setStyleUrl( "#polyline" );

    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, m_polylinePlacemark );

    PolylineAnnotation *polyline = new PolylineAnnotation( m_polylinePlacemark );
    polyline->setState( SceneGraphicsItem::DrawingPolyline );
    polyline->setFocus( true );
    m_graphicsItems.append( polyline );
    m_marbleWidget->update();

    QPointer<EditPolylineDialog> dialog = new EditPolylineDialog( m_polylinePlacemark, m_marbleWidget );
    dialog->setFirstTimeEditing( true );

    connect( dialog, SIGNAL(polylineUpdated(GeoDataFeature*)),
             m_marbleWidget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );
    connect( dialog, SIGNAL(removeRequested()),
             this, SLOT(removeFocusItem()) );
    connect( dialog, SIGNAL(finished(int)),
             this, SLOT(stopEditingPolyline()) );

    if ( m_focusItem ) {
        m_focusItem->setFocus( false );
    }
    m_focusItem = polyline;
    disableActions( m_actions.first() );

    dialog->move( m_marbleWidget->mapToGlobal( QPoint( 0, 0 ) ) );
    dialog->show();
}

void AnnotatePlugin::stopEditingPolyline()
{
    m_drawingPolyline = false;
    m_drawingPolyline = 0;
    m_polylinePlacemark = 0;

    m_editingDialogIsShown = false;
    announceStateChanged( SceneGraphicsItem::Editing );
    enableAllActions( m_actions.first() );
    disableFocusActions();
    enableActionsOnItemType( SceneGraphicsTypes::SceneGraphicPolylineAnnotation );
}

void AnnotatePlugin::announceStateChanged( SceneGraphicsItem::ActionState newState )
{
    foreach ( SceneGraphicsItem *item, m_graphicsItems ) {
        item->setState( newState );
        m_marbleWidget->model()->treeModel()->updateFeature( item->placemark() );
    }
}

void AnnotatePlugin::setupCursor( SceneGraphicsItem *item )
{
    if ( !item || item->state() == SceneGraphicsItem::AddingNodes ) {
        m_marbleWidget->setCursor( Qt::DragCopyCursor );
    } else {
        m_marbleWidget->setCursor( Qt::PointingHandCursor );
    }
}

void AnnotatePlugin::cutItem()
{
    disableFocusActions();

    // If there is already an item copied/cut, free its memory and replace it with this one.
    // The same applies when copying.
    if ( m_clipboardItem ) {
        delete m_clipboardItem->feature();
        delete m_clipboardItem;
        m_clipboardItem = 0;
    }

    m_clipboardItem = m_focusItem;
    m_pasteGraphicItem->setEnabled( true );

    m_graphicsItems.removeAll( m_focusItem );
    m_marbleWidget->model()->treeModel()->removeFeature( m_focusItem->feature() );

    m_focusItem = 0;
}

void AnnotatePlugin::copyItem()
{
    if ( m_clipboardItem ) {
        delete m_clipboardItem->feature();
        delete m_clipboardItem;
        m_clipboardItem = 0;
    }

    // Just copy the placemark and instantiate a new object based on its graphic type.
    // FIXME: Here is obvious a problem in the case of AreaAnnotation (when copying a
    // placemark which has a GeoDataPolygon geometry?). Later Edit: The same applies for
    // polylines (GeoDataLineString geometries).
    GeoDataPlacemark *placemark = new GeoDataPlacemark( *m_focusItem->placemark() );
    if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicAreaAnnotation ) {
        m_clipboardItem = new AreaAnnotation( placemark );
    } else if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicTextAnnotation ) {
        m_clipboardItem = new PlacemarkTextAnnotation( placemark );
    } else if ( m_focusItem->graphicType() == SceneGraphicsTypes::SceneGraphicPolylineAnnotation ) {
        m_clipboardItem = new PolylineAnnotation( placemark );
    }

    m_pasteGraphicItem->setEnabled( true );
}

void AnnotatePlugin::pasteItem()
{
    const QPoint eventPoint = m_marbleWidget->popupMenu()->mousePosition();

    qreal lon, lat;
    m_marbleWidget->geoCoordinates( eventPoint.x(), eventPoint.y(), lon, lat, GeoDataCoordinates::Radian );
    const GeoDataCoordinates newCoords( lon, lat );

    m_clipboardItem->move( m_fromWhereToCopy, newCoords );
    m_marbleWidget->model()->treeModel()->addFeature( m_annotationDocument, m_clipboardItem->placemark() );
    m_graphicsItems.append( m_clipboardItem );

    m_clipboardItem->setFocus( true );
    enableActionsOnItemType( m_clipboardItem->graphicType() );
    m_focusItem = m_clipboardItem;
    m_clipboardItem = 0;

    m_pasteGraphicItem->setEnabled( false );
}

}

Q_EXPORT_PLUGIN2( AnnotatePlugin, Marble::AnnotatePlugin )

#include "AnnotatePlugin.moc"
