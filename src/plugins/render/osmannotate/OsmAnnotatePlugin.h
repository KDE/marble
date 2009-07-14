//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Andrew Manson <g.real.ate@gmail.com>"
//

//
// This class is a test plugin.
//

#ifndef MARBLEOSMANNOTATEPLUGIN_H
#define MARBLEOSMANNOTATEPLUGIN_H

#include <QtCore/QObject>
#include <QtGui/QActionGroup>
#include <QtGui/QToolBar>
#include <QtGui/QGroupBox>

#include "RenderPlugin.h"
#include "TmpGraphicsItem.h"

namespace Marble
{

    class MarbleWidget;
    class GeoDataDocument;

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


    QList<QActionGroup*> actionGroups();

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

    //Intended to be replaced by an actual model system
    QList<TmpGraphicsItem*> model;

    //Only examples as a preview
    QWidget*    vid;
    QWidget*    but;

    bool    widgetInitalised;


signals:
    void redraw();
public slots:
    void loadOsmFile();

    void setAddingPlacemark( bool );
    void setDrawingPolygon( bool );
 protected:
    bool eventFilter(QObject* watched, QEvent* event);
private:
    QList<QActionGroup*> setupActions(MarbleWidget* m);

    GeoDataLineString* tmp_lineString;
    QList<GeoGraphicsItem*>* m_itemModel;

    bool m_addingPlacemark;
    bool m_drawingPolygon;
};

}

#endif // MARBLEOSMANNOTATEPLUGIN_H
