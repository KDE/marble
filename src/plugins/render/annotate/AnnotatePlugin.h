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
#include "AreaAnnotation.h"

#include <QObject>
#include <QMenu>
#include <QSortFilterProxyModel>


class QNetworkAccessManager;
class QNetworkReply;

namespace Marble
{

class MarbleWidget;
class TextureLayer;
class PlacemarkTextAnnotation;
class GeoDataDocument;
class GeoDataLinearRing;
class GeoDataLineString;


/**
 * @brief This class specifies the Marble layer interface of a plugin which
 * annotates maps with polygons and placemarks.
 */
class AnnotatePlugin :  public RenderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.kde.edu.marble.AnnotatePlugin" )
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

    QList<PluginAuthor> pluginAuthors() const;

    void initialize ();

    bool isInitialized () const;

    virtual QString runtimeTrace() const;

    virtual const QList<QActionGroup*> *actionGroups() const;
    virtual const QList<QActionGroup*> *toolbarActionGroups() const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos, GeoSceneLayer *layer = 0 );

signals:
    void placemarkAdded();
    void itemRemoved();
    void placemarkMoved();

public slots:
    void enableModel( bool enabled );

    void setDrawingPolygon( bool );
    void setAddingPolygonHole( bool );
    void setMergingNodes( bool );
    void setAddingNodes( bool );
    void setRemovingItems( bool );

    void addOverlay();
    void addTextAnnotation();

    //    void receiveNetworkReply( QNetworkReply* );
    //    void downloadOsmFile();

    void clearAnnotations();
    void saveAnnotationFile();
    void loadAnnotationFile();

private slots:
    void editTextAnnotationRmbMenu();
    void removeTextAnnotation();

    void editOverlay();
    void removeOverlay();
    void updateOverlayFrame( GeoDataGroundOverlay *overlay );

    void editPolygon();
    void removePolygon();
    void selectNode();
    void deleteNode();
    void deselectNodes();
    void deleteSelectedNodes();
    void setAreaAvailable();

protected:
    bool eventFilter( QObject *watched, QEvent *event );

private:
    void setupActions( MarbleWidget *marbleWidget );

    void setupTextAnnotationRmbMenu();
    void showTextAnnotationRmbMenu( PlacemarkTextAnnotation *placemark, qreal x, qreal y );

    void setupGroundOverlayModel();
    void setupOverlayRmbMenu();
    void showOverlayRmbMenu( GeoDataGroundOverlay *overlay, qreal x, qreal y );
    void displayOverlayEditDialog( GeoDataGroundOverlay *overlay );
    void displayOverlayFrame( GeoDataGroundOverlay *overlay );
    void clearOverlayFrames();

    void setupPolygonRmbMenu();
    void setupNodeRmbMenu();
    void showPolygonRmbMenu( AreaAnnotation *selectedArea, qreal x, qreal y );
    void showNodeRmbMenu( AreaAnnotation *area, qreal x, qreal y );

    void handleUncaughtEvents( QMouseEvent *mouseEvent );
    void handleReleaseOverlay( QMouseEvent *mouseEvent );

    bool handleAddingPlacemark( QMouseEvent *mouseEvent );
    bool handleAddingPolygon( QMouseEvent *mouseEvent );
    bool handleMovingSelectedItem( QMouseEvent *mouseEvent );

    void handleRemovingItem( SceneGraphicsItem *item );
    void handleRequests( QMouseEvent *mouseEvent, SceneGraphicsItem *item );

    void handleSuccessfulPressEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item );
    void handleSuccessfulHoverEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item );
    void handleSuccessfulReleaseEvent( QMouseEvent *mouseEvent, SceneGraphicsItem *item );

    void announceStateChanged( SceneGraphicsItem::ActionState newState );
    void setupCursor( SceneGraphicsItem *item );

    //    void readOsmFile( QIODevice* device, bool flyToFile );


    bool m_widgetInitialized;
    MarbleWidget *m_marbleWidget;

    QMenu *m_overlayRmbMenu;
    QMenu *m_polygonRmbMenu;
    QMenu *m_nodeRmbMenu;
    QMenu *m_textAnnotationRmbMenu;

    QList<QActionGroup*>    m_actions;
    QList<QActionGroup*>    m_toolbarActions;
    QSortFilterProxyModel   m_groundOverlayModel;
    QMap<GeoDataGroundOverlay*, SceneGraphicsItem*> m_groundOverlayFrames;

    GeoDataDocument*          m_annotationDocument;
    QList<SceneGraphicsItem*> m_graphicsItems;

    SceneGraphicsItem       *m_movedItem;
    SceneGraphicsItem       *m_lastItem;

    GeoDataPlacemark        *m_polygonPlacemark;
    GeoDataGroundOverlay    *m_rmbOverlay;
    AreaAnnotation          *m_selectedArea;
    PlacemarkTextAnnotation *m_selectedTextAnnotation;

    //    QNetworkAccessManager* m_networkAccessManager;
    //    QErrorMessage m_errorMessage;

    bool m_drawingPolygon;
    bool m_removingItem;
    bool m_isInitialized;
};


}

#endif // MARBLE_ANNOTATEPLUGIN_H
