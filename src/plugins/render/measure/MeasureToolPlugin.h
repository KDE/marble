//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn     <tackat@kde.org>
// Copyright 2007      Inge Wallin      <ingwa@kde.org>
// Copyright 2011      Michael Henning  <mikehenning@eclipse.net>
//

//
// MeasureToolPlugin enables Marble to set and display measure points
//

#ifndef MARBLE_MEASURETOOLPLUGIN_H
#define MARBLE_MEASURETOOLPLUGIN_H

#include "GeoDataLineString.h"
#include "RenderPlugin.h"
#include "MarbleWidget.h"
#include "MarbleWidgetPopupMenu.h"

#include <QtCore/QObject>
#include <QtGui/QFont>
#include <QtGui/QPen>
#include <QtGui/QAction>

namespace Marble
{

class MeasureToolPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( MeasureToolPlugin )

 public:
    MeasureToolPlugin();

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


    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

 Q_SIGNALS:
    void  numberOfMeasurePointsChanged( int newNumber );

 public Q_SLOTS:
    bool  eventFilter( QObject *object, QEvent *event );

 private:
    void  drawMeasurePoints( GeoPainter *painter, ViewportParams *viewport );
    void  drawMark( GeoPainter* painter, int x, int y );
    void  drawTotalDistanceLabel( GeoPainter *painter,
                                  qreal totalDistance );
    void  addContextItems();
    void  removeContextItems();

 private Q_SLOTS:
    void  setNumberOfMeasurePoints( int number );
    void  addMeasurePointEvent();

    void  addMeasurePoint( qreal lon, qreal lat );
    void  removeLastMeasurePoint();
    void  removeMeasurePoints();

 private:
    Q_DISABLE_COPY( MeasureToolPlugin )

    // The line strings in the distance path.
    GeoDataLineString m_measureLineString;

    QFont   m_font_regular;
    int     m_fontascent;

    QPen    m_pen;

    QAction *m_addMeasurePointAction;
    QAction *m_removeLastMeasurePointAction;
    QAction *m_removeMeasurePointsAction;
    QAction *m_separator;

    MarbleWidget* m_marbleWidget;
};

}

#endif // MARBLE_MEASURETOOLPLUGIN_H
