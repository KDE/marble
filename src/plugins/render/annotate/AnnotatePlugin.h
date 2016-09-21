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


#ifndef MARBLE_ANNOTATEPLUGIN_H
#define MARBLE_ANNOTATEPLUGIN_H

#include "RenderPlugin.h"
#include "SceneGraphicsItem.h"
#include "GeoDataGroundOverlay.h"
#include "GroundOverlayFrame.h"

#include <QMenu>
#include <QSortFilterProxyModel>


namespace Marble
{

class MarbleWidget;
class GeoDataDocument;
class GeoDataLinearRing;
class AreaAnnotation;
class PolylineAnnotation;
class PlacemarkTextAnnotation;
class OsmPlacemarkData;


/**
 * @brief This class specifies the Marble layer interface of a plugin which
 * annotates maps with polygons and placemarks.
 */
class AnnotatePlugin :  public RenderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.AnnotatePlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( AnnotatePlugin )

public:
    explicit AnnotatePlugin(const MarbleModel *model = 0);
    virtual ~AnnotatePlugin();

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QIcon icon () const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    void initialize ();

    bool isInitialized () const;

    virtual QString runtimeTrace() const;

    virtual const QList<QActionGroup*> *actionGroups() const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos, GeoSceneLayer *layer = 0 );

Q_SIGNALS:
    void placemarkMoved();
    void nodeAdded( const GeoDataCoordinates &coordinates );
    void itemMoved( GeoDataPlacemark *placemark );
    void mouseMoveGeoPosition( const QString& );

private Q_SLOTS:
    void enableModel( bool enabled );
    void askToRemoveFocusItem();
    void removeFocusItem();
    void clearAnnotations();
    void saveAnnotationFile();
    void loadAnnotationFile();
    void copyItem();
    void cutItem();
    void pasteItem();

    void addTextAnnotation();
    void editTextAnnotation();
    void stopEditingTextAnnotation( int result );

    void addOverlay();
    void editOverlay();
    void removeOverlay();
    void updateOverlayFrame( GeoDataGroundOverlay *overlay );

    void addPolygon();
    void stopEditingPolygon( int result );
    void setAddingPolygonHole( bool enabled );
    void setAddingNodes( bool enabled );
    void editPolygon();
    void selectNode();
    void deleteNode();
    void deselectNodes();
    void deleteSelectedNodes();
    void setAreaAvailable();

    void addPolyline();
    void editPolyline();
    void stopEditingPolyline( int result );
    void setPolylineAvailable();

    void addRelation( const OsmPlacemarkData &relationOsmData );

protected:
    bool eventFilter( QObject *watched, QEvent *event );

private:
    void addContextItems();
    void setupActions( MarbleWidget *marbleWidget );
    void disableActions( QActionGroup *group );
    void enableAllActions( QActionGroup *group );
    void enableActionsOnItemType( const QString &type );
    void disableFocusActions();

    void setupTextAnnotationRmbMenu();
    void showTextAnnotationRmbMenu( qreal x, qreal y );

    void setupGroundOverlayModel();
    void setupOverlayRmbMenu();
    void showOverlayRmbMenu( GeoDataGroundOverlay *overlay, qreal x, qreal y );
    void displayOverlayFrame( GeoDataGroundOverlay *overlay );
    void clearOverlayFrames();

    void setupPolygonRmbMenu();
    void setupNodeRmbMenu();
    void showPolygonRmbMenu( qreal x, qreal y );
    void showNodeRmbMenu( qreal x, qreal y );

    void setupPolylineRmbMenu();
    void showPolylineRmbMenu( qreal x, qreal y );

    void handleUncaughtEvents( QMouseEvent *mouseEvent );
    void handleReleaseOverlay( QMouseEvent *mouseEvent );
    bool handleDrawingPolyline( QMouseEvent *mouseEvent );
    bool handleDrawingPolygon( QMouseEvent *mouseEvent );
    bool handleMovingSelectedItem( QMouseEvent *mouseEvent );

    void handleRequests( QMouseEvent *mouseEvent, SceneGraphicsItem *item );
    void handleSuccessfulPressEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item );
    void handleSuccessfulHoverEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item );
    void handleSuccessfulReleaseEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item );

    void announceStateChanged( SceneGraphicsItem::ActionState newState );
    void setupCursor( SceneGraphicsItem *item );

    const GeoDataCoordinates mouseGeoDataCoordinates( QMouseEvent *mouseEvent );

    bool m_isInitialized;
    bool m_widgetInitialized;
    MarbleWidget *m_marbleWidget;

    QMenu *m_overlayRmbMenu;
    QMenu *m_polygonRmbMenu;
    QMenu *m_nodeRmbMenu;
    QMenu *m_textAnnotationRmbMenu;
    QMenu *m_polylineRmbMenu;

    QList<QActionGroup*> m_actions;
    QSortFilterProxyModel m_groundOverlayModel;
    QMap<GeoDataGroundOverlay*, SceneGraphicsItem*> m_groundOverlayFrames;

    // A list of all osm relations
    QHash<qint64, OsmPlacemarkData> m_osmRelations;


    GeoDataDocument* m_annotationDocument;
    QList<SceneGraphicsItem*> m_graphicsItems;

    SceneGraphicsItem *m_movedItem;
    SceneGraphicsItem *m_focusItem;
    SceneGraphicsItem *m_editedItem;
    GeoDataGroundOverlay *m_rmbOverlay;

    GeoDataPlacemark *m_polylinePlacemark;
    GeoDataPlacemark *m_polygonPlacemark;

    GeoDataCoordinates m_fromWhereToCopy;
    SceneGraphicsItem *m_clipboardItem;
    QAction *m_pasteGraphicItem;

    bool m_drawingPolygon;
    bool m_drawingPolyline;
    bool m_addingPlacemark;
    bool m_editingDialogIsShown;
};

}

#endif // MARBLE_ANNOTATEPLUGIN_H
