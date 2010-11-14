//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Andrew Manson <g.real.ate@gmail.com>
//

//
// This class is a test plugin.
//

#ifndef MARBLEOSMANNOTATEPLUGIN_H
#define MARBLEOSMANNOTATEPLUGIN_H

#include <QtCore/QObject>
#include <QtGui/QActionGroup>
#include <QtGui/QErrorMessage>
#include <QtGui/QToolBar>
#include <QtGui/QGroupBox>

class QNetworkAccessManager;
class QNetworkReply;

#include "RenderPlugin.h"
#include "TmpGraphicsItem.h"

namespace Marble
{

    class MarbleWidget;
    class TextAnnotation;
    class PointScreenGraphicsItem;

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class OsmAnnotatePlugin :  public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( OsmAnnotatePlugin )

 public:
    OsmAnnotatePlugin();
    virtual ~OsmAnnotatePlugin();

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;


    void initialize ();

    bool isInitialized () const;

    virtual QList<QActionGroup*>* actionGroups() const;
    virtual QList<QActionGroup*>* toolbarActionGroups() const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos, GeoSceneLayer * layer = 0 );

    bool    widgetInitalised;


signals:
    void redraw();
    void placemarkAdded();

public slots:
    void loadOsmFile();
    void downloadOsmFile();
    void saveAnnotationFile();
    void loadAnnotationFile();

    void selectArea( bool );

    void setAddingPlacemark( bool );
    void setDrawingPolygon( bool );

    void receiveNetworkReply( QNetworkReply* );

protected:
    bool eventFilter(QObject* watched, QEvent* event);
private:
    void setupActions(MarbleWidget* m);
    void readOsmFile( QIODevice* device, bool flyToFile );

    MarbleWidget* m_marbleWidget;

    QList<QActionGroup*>    *m_actions;
    QList<QActionGroup*>    *m_toolbarActions;

    //Intended to be replaced by an actual model system
    //FIXME: Merge the two models once TmpGraphicsItem is eliminated
    QList<TmpGraphicsItem*> model;
    QList<GeoGraphicsItem*>* m_itemModel;

    QList<TextAnnotation*> annotations() const;

    //used while creating new polygons
    GeoDataLineString* m_tmp_lineString;

    QPair<PointScreenGraphicsItem*, PointScreenGraphicsItem*> m_selectionBox;

    bool m_addingPlacemark;
    bool m_drawingPolygon;
    QNetworkAccessManager* m_networkAccessManager;
    QErrorMessage m_errorMessage;
    bool m_isInitialized;
};

}

#endif // MARBLEOSMANNOTATEPLUGIN_H
