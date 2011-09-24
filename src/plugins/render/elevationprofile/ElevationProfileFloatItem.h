//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Florian Eßer <f.esser@rwth-aachen.de>
//

#ifndef ELEVATIONPROFILEFLOATITEM_H
#define ELEVATIONPROFILEFLOATITEM_H

#include <QtCore/QObject>
#include <QProcess>
#include <QThread>

#include "AbstractFloatItem.h"
#include "PluginAboutDialog.h"
#include "MarbleWidget.h"
#include "routing/RoutingModel.h"
// #include "ElevationDataGenerator.h"


namespace Ui
{
    class ElevationProfileConfigWidget;
}

namespace Marble
{

class PluginAboutDialog;

/**
 * @short The class that creates an interactive elvation profile.
 *
 */

class ElevationProfileFloatItem : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( ElevationProfileFloatItem )
 public:
    explicit ElevationProfileFloatItem( const QPointF &point = QPointF( 220, 10.5 ),
                                        const QSizeF &size = QSizeF( 0.0, 50.0 ) );
    ~ElevationProfileFloatItem();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    QDialog *aboutDialog();


    void initialize ();

    bool isInitialized () const;

    void changeViewport( ViewportParams *viewport );

    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer = 0 );

    QDialog *configDialog();

 protected:
    bool eventFilter( QObject *object, QEvent *e );
    virtual void contextMenuEvent( QWidget *w, QContextMenuEvent *e );

 private Q_SLOTS:
    void updateData();
    void updateVisiblePoints();
    void forceRepaint();
    void readSettings();
    void writeSettings();
    void toggleZoomToViewport();

 Q_SIGNALS:
    void dataUpdated();

 private:
    PluginAboutDialog *m_aboutDialog;
    QDialog *m_configDialog;
    Ui::ElevationProfileConfigWidget *ui_configWidget;

    QString  m_target;

    int      m_leftGraphMargin;
    int      m_eleGraphWidth;
    qreal    m_viewportWidth;
    qreal    m_eleGraphHeight;

    int      m_fontHeight;
    int      m_bestDivisorX;
    qreal    m_pixelIntervalX;
    qreal    m_valueIntervalX;

    int      m_bestDivisorY;
    int      m_pixelIntervalY;
    qreal    m_valueIntervalY;

    QString  m_unitX;
    QString  m_unitY;

    bool     m_mouseInWidget;
    qreal    m_cursorPositionX;

    bool     m_isInitialized;

    QMenu*   m_contextMenu;

    MarbleWidget*     m_marbleWidget;
    RoutingModel*     m_routingModel;
    RoutingLayer*     m_routingLayer;
    bool              m_routeAvailable;
    int               m_firstVisiblePoint;
    int               m_lastVisiblePoint;
    bool              m_zoomToViewport;
    QList<QPointF>    m_eleData;
    GeoDataLineString m_points;
    qreal             m_maxElevation;


    void calcScaleX( const qreal distance );
    void calcScaleY( const qreal distance );
    void calculateDistances();
    void calculateElevations();

//     ElevationDataGenerator* const dg;
};


}

#endif // ELEVATIONPROFILEFLOATITEM_H
